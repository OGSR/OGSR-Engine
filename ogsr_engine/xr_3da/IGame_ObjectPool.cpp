#include "stdafx.h"
#include "igame_level.h"
#include "IGame_Persistent.h"
#include "igame_objectpool.h"
#include "xr_object.h"

IGame_ObjectPool::IGame_ObjectPool(void) {}

IGame_ObjectPool::~IGame_ObjectPool(void) { R_ASSERT(m_PrefetchObjects.empty()); }

void IGame_ObjectPool::prefetch()
{
    R_ASSERT(m_PrefetchObjects.empty());

    int p_count = 0;
    ::Render->model_Logging(FALSE);

    string256 section;
    // prefetch objects
    strconcat(sizeof(section), section, "prefetch_objects_", g_pGamePersistent->m_game_params.m_game_type);
    if (pSettings->section_exist(section))
    {
        const auto& sect = pSettings->r_section(section);
        CTimer T;
        T.Start();
        Render->models_begin_prefetch1(true);
        for (const auto& item : sect.Ordered_Data)
        {
            if (pSettings->section_exist(item.first.c_str()))
            {
                CLASS_ID CLS = pSettings->r_clsid(item.first.c_str(), "class");
                p_count++;
                CObject* pObject = (CObject*)NEW_INSTANCE(CLS);
                pObject->Load(item.first.c_str());
                pObject->reload(item.first.c_str());
                VERIFY2(pObject->cNameSect().c_str(), item.first.c_str());
                m_PrefetchObjects.push_back(pObject);
            }
            else
                Msg("! [%s] unknown section %s in %s", __FUNCTION__, item.first.c_str(), section);
        }
        Render->models_begin_prefetch1(false);
        Msg("[%s] objects prefetching time (%zi): [%.2f s.]", __FUNCTION__, p_count, T.GetElapsed_sec());
    }

    // out statistic
    ::Render->model_Logging(TRUE);
}

void IGame_ObjectPool::clear()
{
    // Clear POOL
    ObjectVecIt it = m_PrefetchObjects.begin();
    ObjectVecIt itE = m_PrefetchObjects.end();
    for (; it != itE; it++) {
        (*it)->cNameVisual_set(0);
        xr_delete(*it);
    }
    m_PrefetchObjects.clear();
}

CObject* IGame_ObjectPool::create(LPCSTR name)
{
    CLASS_ID CLS = pSettings->r_clsid(name, "class");
    CObject* O = (CObject*)NEW_INSTANCE(CLS);
    O->Load(name);
    return O;
}

void IGame_ObjectPool::destroy(CObject* O) { xr_delete(O); }
