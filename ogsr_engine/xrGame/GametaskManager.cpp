#include "stdafx.h"
#include "GameTaskManager.h"
#include "alife_registry_wrappers.h"
#include "ui/xrUIXmlParser.h"
#include "GameTask.h"
#include "Level.h"
#include "map_manager.h"
#include "map_location.h"
#include "HUDManager.h"
#include "actor.h"
#include "UIGameSP.h"
#include "ui/UIPDAWnd.h"
#include "encyclopedia_article.h"
#include "ui/UIEventsWnd.h"
#include "..\xr_3da\DiscordRPC.hpp"
#include "string_table.h"

shared_str g_active_task_id;
u16 g_active_task_objective_id = u16(-1);


CGameTaskManager::CGameTaskManager()
{
    m_gametasks = xr_new<CGameTaskWrapper>();
}

CGameTaskManager::~CGameTaskManager() { delete_data(m_gametasks); }

void CGameTaskManager::initialize(u16 id)
{
    m_gametasks->registry().init(id); // actor's id
    if (Core.Features.test(xrCore::Feature::keep_inprogress_tasks_only))
        cleanup();

    if (g_active_task_id.size())
        SetActiveTask(g_active_task_id, g_active_task_objective_id);
}

GameTasks& CGameTaskManager::GameTasks() { return m_gametasks->registry().objects(); }

CGameTask* CGameTaskManager::HasGameTask(const TASK_ID& id)
{
    auto it = std::find_if(GameTasks().begin(), GameTasks().end(), [&](const SGameTaskKey& key) { return id == key.task_id; });
    if (it != GameTasks().end())
        return (*it).game_task;

    return 0;
}

CGameTask* CGameTaskManager::GiveGameTaskToActor(const TASK_ID& id, u32 timeToComplete, bool bCheckExisting)
{
    if (bCheckExisting && HasGameTask(id))
        return NULL;
    CGameTask* t = xr_new<CGameTask>(id);

    return GiveGameTaskToActor(t, timeToComplete, bCheckExisting);
}

CGameTask* CGameTaskManager::GiveGameTaskToActor(CGameTask* t, u32 timeToComplete, bool bCheckExisting)
{
    if (bCheckExisting && HasGameTask(t->m_ID))
        return nullptr;

    GameTasks().emplace_back(t->m_ID).game_task = t;
    t->m_ReceiveTime = Level().GetGameTime();
    t->m_TimeToComplete = t->m_ReceiveTime + timeToComplete;

    ARTICLE_VECTOR& article_vector = Actor()->encyclopedia_registry->registry().objects();

    SGameTaskObjective* obj{};
    for (u32 i = 0; i < t->m_Objectives.size(); ++i)
    {
        obj = &t->m_Objectives[i];
        if (obj->article_id.size())
        {
            FindArticleByIDPred pred(obj->article_id);
            if (std::find_if(article_vector.begin(), article_vector.end(), pred) == article_vector.end())
            {
                CEncyclopediaArticle article;
                article.Load(obj->article_id);
                article_vector.emplace_back(obj->article_id, Level().GetGameTime(), article.data()->articleType);
            }
        }

        if (obj->object_id != u16(-1) && obj->map_location.size() && obj->def_location_enabled)
        {
            CMapLocation* ml = Level().MapManager().AddMapLocation(obj->map_location, obj->object_id);
            if (obj->map_hint.size())
                ml->SetHint(obj->map_hint);
            ml->DisablePointer();
            ml->SetSerializable(true);
        }
    }
    CGameTask* _at = ActiveTask();
    if (!_at /*|| (_at->m_priority > t->m_priority)*/)
    {
        SetActiveTask(t->m_ID, 1, true);
    }

    //установить флажок необходимости прочтения тасков в PDA
    if (HUD().GetUI())
    {
        CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
        if (pGameSP)
            pGameSP->PdaMenu->PdaContentsChanged(pda_section::quests);
    }
    //if (t->m_ID!="user_task")
        t->Objective(0).ChangeStateCallback();

    return t;
}

void CGameTaskManager::SetTaskState(CGameTask* t, u16 objective_num, ETaskState state)
{
    SGameTaskObjective* o = &t->Objective(objective_num);
    if (o->TaskState() == state)
        return;

    bool isRoot = (objective_num == 0);

    CMapLocation* ml = o->LinkedMapLocation();
    bool bActive = ActiveObjective() == o;

    if ((state == eTaskStateFail || state == eTaskStateCompleted) && ml)
    {
        Level().MapManager().RemoveMapLocation(o->map_location, o->object_id);
        o->map_location = NULL;
        o->object_id = u16(-1);
    }

    o->SetTaskState(state);

    // highlight next objective if needed
    if ((isRoot || !t->HasInProgressObjective()) && (ActiveTask() == t))
    {
        SetActiveTask("", 1);
    }
    // not last
    else if (!isRoot && bActive && objective_num < (t->m_Objectives.size() - 1))
    {
        SetActiveTask(t->m_ID, objective_num + 1);
    }

    // setState for task and all sub-tasks
    if (isRoot)
    {
        for (u16 i = 0; i < t->m_Objectives.size(); ++i)
            if (t->Objective(i).TaskState() == eTaskStateInProgress)
                SetTaskState(t, i, state);
    }
    else if (state == eTaskStateCompleted && objective_num < (t->m_Objectives.size() - 1))
    {
        // enable hidden locations for next objective
        SGameTaskObjective& obj = t->Objective(objective_num + 1);

        if (!obj.def_location_enabled && obj.TaskState() == eTaskStateInProgress)
        {
            if (obj.object_id != u16(-1) && *obj.map_location)
            {
                CMapLocation* ml = Level().MapManager().AddMapLocation(obj.map_location, obj.object_id);
                if (obj.map_hint.size())
                    ml->SetHint(obj.map_hint);
                ml->DisablePointer();
                ml->SetSerializable(true);
            }
        }
    }

    if (isRoot && eTaskStateCompleted == state || eTaskStateFail == state)
        t->m_FinishTime = Level().GetGameTime();

    CUIGameSP* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
    if (pGameSP)
    {
        pGameSP->PdaMenu->PdaContentsChanged(pda_section::quests);
    }
}

void CGameTaskManager::SetTaskState(const TASK_ID& id, u16 objective_num, ETaskState state)
{
    CGameTask* t = HasGameTask(id);
    if (NULL == t)
    {
        Msg("actor does not has task [%s]", *id);
        return;
    }
    SetTaskState(t, objective_num, state);
}

void CGameTaskManager::UpdateTasks()
{
    auto& tasks = GameTasks();

    if (tasks.empty())
        return;

    auto act_task = ActiveTask();
    bool need_update_active_task = !act_task || act_task->Objective(0).TaskState() != eTaskStateInProgress;

    size_t processed{};
    auto iter = tasks.rbegin();
    while (iter != tasks.rend()) //Реверсивный перебор тасков из-за того что походу внутри SetTaskState таски могут удалиться, из-за этого обычный итератор тут крашится.
    {
        const auto size = tasks.size();

        auto& task = *(iter++);
        //

        auto* t = task.game_task;
        for (u16 i = 0; i < t->m_Objectives.size(); ++i)
        {
            auto& obj = t->Objective(i);
            if (obj.TaskState() != eTaskStateInProgress)
            {
                if (i == 0)
                    break;
                else
                    continue;
            }

            ETaskState state = obj.UpdateState();
            if (state == eTaskStateFail || state == eTaskStateCompleted)
            {
                SetTaskState(t, i, state);
                // Тут проверяем заново, потому что в функции выше активный таск может обновиться
                act_task = ActiveTask();
                need_update_active_task = !act_task || act_task->Objective(0).TaskState() != eTaskStateInProgress;
            }
            // Тут ставим активным только первый objective если он один либо второй, чтоб тут случайно не назначился тот который скрыт опцией show_objectives_ondemand.
            else if (need_update_active_task && ((i == 0 && t->m_Objectives.size() == 1) || (i == 1 && t->Objective(0).TaskState() == eTaskStateInProgress)))
            {
                SetActiveTask(t->m_ID, i);
                need_update_active_task = false;
            }
        }

        //
        if (size != tasks.size())
        {
            iter = tasks.rbegin();
            std::advance(iter, processed);
        }
        else
        {
            processed++;
        }
    }

    SGameTaskObjective* obj = ActiveObjective();
    if (obj)
    {
        Level().MapManager().DisableAllPointers();
        CMapLocation* ml = obj->LinkedMapLocation();
        if (ml && !ml->PointerEnabled())
            ml->EnablePointer();
    }
}

CGameTask* CGameTaskManager::ActiveTask()
{
    const TASK_ID& t_id = g_active_task_id;
    if (!t_id.size())
        return NULL;
    return HasGameTask(t_id);
}

void CGameTaskManager::SetActiveTask(const TASK_ID& id, u16 idx, const bool safe)
{
    g_active_task_id = id;

    if (safe)
    {
        auto* t = ActiveTask();
        if (t && t->m_Objectives.size() < (idx + 1))
        {
            ASSERT_FMT(!t->m_Objectives.empty(), "!![%s] m_Objectives is empty! Something strange!", __FUNCTION__);
            g_active_task_objective_id = t->m_Objectives.size() - 1; //Некторые таски могут содержать всего один objective

            if (g_active_task_objective_id == 0)
                Msg("!![%s - 1] g_active_task_objective_idx == 0", __FUNCTION__);
        }
        else
        {
            g_active_task_objective_id = idx;

            if (g_active_task_objective_id == 0)
                Msg("!![%s - 2] g_active_task_objective_idx == 0", __FUNCTION__);
        }
    }
    else
    {
        g_active_task_objective_id = idx;

        if (g_active_task_objective_id == 0)
            Msg("!![%s - 3] g_active_task_objective_idx == 0", __FUNCTION__);
    }

    Level().MapManager().DisableAllPointers();
    SGameTaskObjective* o = ActiveObjective();

    if (o)
    {
        CMapLocation* ml = o->LinkedMapLocation();
        if (ml)
            ml->EnablePointer();
    }

    Discord.Set_active_task_text(CStringTable().translate(o ? o->description : "st_no_active_task").c_str());
}

SGameTaskObjective* CGameTaskManager::ActiveObjective()
{
    CGameTask* t = ActiveTask();

    return (t) ? &t->Objective(g_active_task_objective_id) : NULL;
}

void CGameTaskManager::cleanup()
{
    xr_vector<shared_str> articles;
    GameTasks().erase(std::remove_if(GameTasks().begin(), GameTasks().end(),
                                     [&](const SGameTaskKey& k) {
                                         if (k.game_task->Objective(0).TaskState() != eTaskStateInProgress)
                                         {
                                             for (const auto& obj : k.game_task->m_Objectives)
                                                 if (obj.article_id.size())
                                                     articles.push_back(obj.article_id);
                                             return true;
                                         }
                                         return false;
                                     }),
                      GameTasks().end());

    if (!articles.empty())
        articles.erase(std::remove_if(articles.begin(), articles.end(),
                                      [&](const auto& article_id) {
                                          for (const auto& it : GameTasks())
                                          {
                                              for (const auto& obj : it.game_task->m_Objectives)
                                                  if (obj.article_id == article_id)
                                                      return true;
                                          }
                                          return false;
                                      }),
                       articles.end());

    if (!articles.empty())
    {
        auto& article_vector = Actor()->encyclopedia_registry->registry().objects();
        for (const auto& article_id : articles)
        {
            FindArticleByIDPred pred(article_id);
            article_vector.erase(std::remove_if(article_vector.begin(), article_vector.end(), pred), article_vector.end());
        }
        if (HUD().GetUI())
        {
            auto* pGameSP = smart_cast<CUIGameSP*>(HUD().GetUI()->UIGame());
            if (pGameSP)
            {
                pGameSP->PdaMenu->PdaContentsChanged(pda_section::encyclopedia, false);
                pGameSP->PdaMenu->PdaContentsChanged(pda_section::journal, false);
            }
        }
    }
}
