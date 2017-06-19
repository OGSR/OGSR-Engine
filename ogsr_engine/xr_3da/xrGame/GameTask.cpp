#include "pch_script.h"
#include "GameTask.h"
#include "ui/xrUIXmlParser.h"
#include "encyclopedia_article.h"
#include "map_location.h"
#include "map_manager.h"

#include "level.h"
#include "actor.h"
#include "script_engine.h"
#include "script_callback_ex.h"
#include "script_game_object.h"
#include "ai_space.h"
#include "alife_object_registry.h"
#include "alife_simulator.h"
#include "alife_story_registry.h"
#include "game_object_space.h"
#include "object_broker.h"
#include "ui/uitexturemaster.h"

ALife::_STORY_ID	story_id		(LPCSTR story_id);
u16					storyId2GameId	(ALife::_STORY_ID);



using namespace luabind;

ALife::_STORY_ID	story_id	(LPCSTR story_id)
{
	int res=
							(
		object_cast<int>(
			luabind::object(
				luabind::get_globals(
					ai().script_engine().lua()
				)
				["story_ids"]
			)
			[story_id]
		)
	);
	return ALife::_STORY_ID(res);
}

u16 storyId2GameId	(ALife::_STORY_ID id)
{
	if(ai().get_alife()){ 
		CSE_ALifeDynamicObject* so = ai().alife().story_objects().object(id, true);
		return (so)?so->ID:u16(-1);
	}else{
		u32 cnt = Level().Objects.o_count();
		for(u32 it=0;it<cnt;++it){
			CObject* O = Level().Objects.o_get_by_iterator(it);
			CGameObject* GO = smart_cast<CGameObject*>(O);
			if(GO->story_id()==id)
				return GO->ID();
		}
	return u16(-1);
	}
}

CUIXml*	g_gameTaskXml=NULL;

CGameTask::CGameTask(const TASK_ID& id)
{
	m_ReceiveTime	= 0;
	m_FinishTime	= 0;
	m_Title			= NULL;
	m_priority		= u32(-1);
	Load			(id);
}

CGameTask::CGameTask()
{
	m_ReceiveTime	= 0;
	m_FinishTime	= 0;
	m_Title			= NULL;
	m_ID			= NULL;
}

void CGameTask::Load(const TASK_ID& id)
{
	m_ID							= id;

	if(!g_gameTaskXml){
		g_gameTaskXml				= xr_new<CUIXml>();
		g_gameTaskXml->Init			(CONFIG_PATH, "gameplay", "game_tasks.xml");
	}
	XML_NODE* task_node				= g_gameTaskXml->NavigateToNodeWithAttribute("game_task","id",*id);

	THROW3							(task_node, "game task id=", *id);
	g_gameTaskXml->SetLocalRoot		(task_node);
	m_Title							= g_gameTaskXml->Read(g_gameTaskXml->GetLocalRoot(), "title", 0, NULL);
	m_priority						= g_gameTaskXml->ReadAttribInt(g_gameTaskXml->GetLocalRoot(), "prio", -1);
#ifdef DEBUG
	if(m_priority == u32(-1))
	{
		Msg("Game Task [%s] has no priority", *id);
	}
#endif // DEBUG
	int tag_num						= g_gameTaskXml->GetNodesNum(g_gameTaskXml->GetLocalRoot(),"objective");
	m_Objectives.clear		();
	for(int i=0; i<tag_num; i++)
	{
		XML_NODE*	l_root = NULL;
		l_root							= g_gameTaskXml->NavigateToNode("objective",i); 
		g_gameTaskXml->SetLocalRoot		(l_root);

		m_Objectives.push_back			(SGameTaskObjective(this,i));
		SGameTaskObjective&				objective = m_Objectives.back();

//.
		LPCSTR tag_text					= g_gameTaskXml->Read(l_root, "text", 0, NULL);
		objective.description			= tag_text;
//.
		tag_text						= g_gameTaskXml->Read(l_root, "article", 0, NULL);
		if(tag_text)
			objective.article_id		= tag_text;

//.
		tag_text						= g_gameTaskXml->ReadAttrib(l_root, "key", NULL);
		if(tag_text)
			objective.article_key		= tag_text;

//.
		if(i==0)
		{
			objective.icon_texture_name		= g_gameTaskXml->Read(g_gameTaskXml->GetLocalRoot(), "icon", 0, NULL);
			if( objective.icon_texture_name.size() && 
				0 != stricmp(*objective.icon_texture_name, "ui\\ui_icons_task") )
			{
				objective.icon_rect			= CUITextureMaster::GetTextureRect(*objective.icon_texture_name);
				objective.icon_rect.rb.sub(objective.icon_rect.rb, objective.icon_rect.lt);
				objective.icon_texture_name	= CUITextureMaster::GetTextureFileName(*objective.icon_texture_name);
			}else 
				if(objective.icon_texture_name.size()){
				objective.icon_rect.x1			= g_gameTaskXml->ReadAttribFlt(l_root, "icon", 0, "x");
				objective.icon_rect.y1			= g_gameTaskXml->ReadAttribFlt(l_root, "icon", 0, "y");
				objective.icon_rect.x2			= g_gameTaskXml->ReadAttribFlt(l_root, "icon", 0, "width");
				objective.icon_rect.y2			= g_gameTaskXml->ReadAttribFlt(l_root, "icon", 0, "height");
			}
		}
//.
		objective.map_location			= g_gameTaskXml->Read(l_root, "map_location_type", 0, NULL);

		LPCSTR object_story_id			= g_gameTaskXml->Read(l_root, "object_story_id", 0, NULL);

//*
		LPCSTR ddd;
		ddd								= g_gameTaskXml->Read(l_root, "map_location_hidden", 0, NULL);
		if(ddd)
			objective.def_location_enabled = false;

		bool b1,b2;
		b1								= (0==objective.map_location.size());
		b2								= (NULL==object_story_id);
		VERIFY3							(b1==b2,"check [map_location_type] and [object_story_id] fields in objective definition for: ",*objective.description);
		
//.
		objective.object_id				= u16(-1);

//.
		objective.map_hint				= g_gameTaskXml->ReadAttrib(l_root, "map_location_type", 0, "hint", NULL);

		if(object_story_id){
			ALife::_STORY_ID _sid		= story_id(object_story_id);
			objective.object_id			= storyId2GameId(_sid);
		}


//------infoportion_complete
		int info_num					= g_gameTaskXml->GetNodesNum(l_root,"infoportion_complete");
		objective.m_completeInfos.resize(info_num);
		int j;
		for(j=0; j<info_num; ++j)
			objective.m_completeInfos[j]= g_gameTaskXml->Read(l_root, "infoportion_complete", j, NULL);

//------infoportion_fail
		info_num						= g_gameTaskXml->GetNodesNum(l_root,"infoportion_fail");
		objective.m_failInfos.resize	(info_num);

		for(j=0; j<info_num; ++j)
			objective.m_failInfos[j]	= g_gameTaskXml->Read(l_root, "infoportion_fail", j, NULL);

//------infoportion_set_complete
		info_num						= g_gameTaskXml->GetNodesNum(l_root,"infoportion_set_complete");
		objective.m_infos_on_complete.resize(info_num);
		for(j=0; j<info_num; ++j)
			objective.m_infos_on_complete[j]= g_gameTaskXml->Read(l_root, "infoportion_set_complete", j, NULL);

//------infoportion_set_fail
		info_num						= g_gameTaskXml->GetNodesNum(l_root,"infoportion_set_fail");
		objective.m_infos_on_fail.resize(info_num);
		for(j=0; j<info_num; ++j)
			objective.m_infos_on_fail[j]= g_gameTaskXml->Read(l_root, "infoportion_set_fail", j, NULL);


//------function_complete
		LPCSTR		str;
		bool functor_exists;
		info_num						= g_gameTaskXml->GetNodesNum(l_root,"function_complete");
		objective.m_complete_lua_functions.resize(info_num);
		for(j=0; j<info_num; ++j){
			str							= g_gameTaskXml->Read(l_root, "function_complete", j, NULL);
			functor_exists				= ai().script_engine().functor(str ,objective.m_complete_lua_functions[j]);
			THROW3						(functor_exists, "Cannot find script function described in task objective  ", str);
		}


//------function_fail
		info_num						= g_gameTaskXml->GetNodesNum(l_root,"function_fail");
		objective.m_fail_lua_functions.resize(info_num);
		for(j=0; j<info_num; ++j){
			str							= g_gameTaskXml->Read(l_root, "function_fail", j, NULL);
			functor_exists				= ai().script_engine().functor(str ,objective.m_fail_lua_functions[j]);
			THROW3						(functor_exists, "Cannot find script function described in task objective  ", str);
		}

//------function_on_complete
		info_num						= g_gameTaskXml->GetNodesNum(l_root,"function_call_complete");
		objective.m_lua_functions_on_complete.resize(info_num);
		for(j=0; j<info_num; ++j){
			str							= g_gameTaskXml->Read(l_root, "function_call_complete", j, NULL);
			functor_exists				= ai().script_engine().functor(str ,objective.m_lua_functions_on_complete[j]);
			THROW3						(functor_exists, "Cannot find script function described in task objective  ", str);
		}


//------function_on_fail
		info_num						= g_gameTaskXml->GetNodesNum(l_root,"function_call_fail");
		objective.m_lua_functions_on_fail.resize(info_num);
		for(j=0; j<info_num; ++j){
			str							= g_gameTaskXml->Read(l_root, "function_call_fail", j, NULL);
			functor_exists				= ai().script_engine().functor(str ,objective.m_lua_functions_on_fail[j]);
			THROW3						(functor_exists, "Cannot find script function described in task objective  ", str);
		}

		g_gameTaskXml->SetLocalRoot		(task_node);
	}
	g_gameTaskXml->SetLocalRoot		(g_gameTaskXml->GetRoot());
}

bool CGameTask::HasLinkedMapLocations			()
{
	for(u32 i=0; i<m_Objectives.size(); ++i)
		if(m_Objectives[i].LinkedMapLocation()!=NULL)return true;

	return false;
}

bool CGameTask::HasInProgressObjective()
{
	for(u32 i=1; i<m_Objectives.size(); ++i)
		if(m_Objectives[i].TaskState()==eTaskStateInProgress)return true;
	return false;
}

SGameTaskObjective::SGameTaskObjective		(CGameTask* parent, int _idx)
:description		(NULL),
article_id			(NULL),
map_location		(NULL),
object_id			(u16(-1)),
task_state			(eTaskStateInProgress),
def_location_enabled(true),
parent				(parent),
idx					(_idx)
{
}

SGameTaskObjective::SGameTaskObjective()
:description		(NULL),
article_id			(NULL),
map_location		(NULL),
object_id			(u16(-1)),
task_state			(eTaskStateInProgress),
def_location_enabled(true),
parent				(NULL),
idx					(0)
{
}

CMapLocation* SGameTaskObjective::LinkedMapLocation		()
{
	if( map_location.size()==0) return NULL;
	return Level().MapManager().GetMapLocation(map_location, object_id);
}

void SGameTaskObjective::SetTaskState		(ETaskState new_state)
{
	task_state = new_state;
	if( (new_state==eTaskStateFail) || (new_state==eTaskStateCompleted) ){

		if( task_state==eTaskStateFail ){
				SendInfo				(m_infos_on_fail);
				CallAllFuncs			(m_lua_functions_on_fail);
		}else
		if( task_state==eTaskStateCompleted ){
				SendInfo				(m_infos_on_complete);
				CallAllFuncs			(m_lua_functions_on_complete);
		}
		//callback for scripters
		ChangeStateCallback();
	}
}

ETaskState SGameTaskObjective::UpdateState	()
{
	if( (idx==0) && (parent->m_ReceiveTime != parent->m_TimeToComplete) )
	{
		if(Level().GetGameTime() > parent->m_TimeToComplete)
		{
			return		eTaskStateFail;
		}
	}
//check fail infos
	if( CheckInfo(m_failInfos) )
		return		eTaskStateFail;

//check fail functor
	if( CheckFunctions(m_fail_lua_functions) )
		return		eTaskStateFail;
	
//check complete infos
	if( CheckInfo(m_completeInfos) )
		return		eTaskStateCompleted;


//check complete functor
	if( CheckFunctions(m_complete_lua_functions) )
		return		eTaskStateCompleted;

	
	return TaskState();
}

void SGameTaskObjective::SendInfo		(xr_vector<shared_str>& v)
{
	xr_vector<shared_str>::iterator it	= v.begin();
	for(;it!=v.end();++it)
		Actor()->TransferInfo					((*it),true);

}

bool SGameTaskObjective::CheckInfo		(xr_vector<shared_str>& v)
{
	bool res = false;
	xr_vector<shared_str>::iterator it	= v.begin();
	for(;it!=v.end();++it){
		res = Actor()->HasInfo					(*it);
		if(!res) break;
	}
	return res;
}

bool SGameTaskObjective::CheckFunctions	(xr_vector<luabind::functor<bool> >& v)
{
	bool res = false;
	xr_vector<luabind::functor<bool> >::iterator it	= v.begin();
	for(;it!=v.end();++it){
		if( (*it).is_valid() ) res = (*it)(*(parent->m_ID), idx);
		if(!res) break;
	}
	return res;

}

void SGameTaskObjective::CallAllFuncs	(xr_vector<luabind::functor<bool> >& v)
{
	xr_vector<luabind::functor<bool> >::iterator it	= v.begin();
	for(;it!=v.end();++it){
		if( (*it).is_valid() ) (*it)(*(parent->m_ID), idx);
	}
}
void SGameTaskObjective::SetDescription_script(LPCSTR _descr)
{
	description = _descr;
}

void SGameTaskObjective::SetArticleID_script(LPCSTR _id)
{
	article_id = _id;
}

void SGameTaskObjective::SetMapHint_script(LPCSTR _str)
{
	map_hint = _str;
}

void SGameTaskObjective::SetMapLocation_script(LPCSTR _str)
{
	map_location = _str;
}

void SGameTaskObjective::SetObjectID_script(u16 id)
{
	object_id = id;
}

void SGameTaskObjective::SetIconName_script(LPCSTR _str)
{
	icon_texture_name	= _str;
	icon_rect			= CUITextureMaster::GetTextureRect(icon_texture_name.c_str());
	icon_rect.rb.sub	(icon_rect.rb, icon_rect.lt);
	icon_texture_name	= CUITextureMaster::GetTextureFileName(icon_texture_name.c_str());
}

void SGameTaskObjective::SetArticleKey_script(LPCSTR _str)
{
	article_key = _str;
}

void SGameTaskObjective::AddCompleteInfo_script(LPCSTR _str)
{
	m_completeInfos.push_back(_str);
}

void SGameTaskObjective::AddFailInfo_script(LPCSTR _str)
{
	m_failInfos.push_back(_str);
}

void SGameTaskObjective::AddOnCompleteInfo_script(LPCSTR _str)
{
	m_infos_on_complete.push_back(_str);
}

void SGameTaskObjective::AddOnFailInfo_script(LPCSTR _str)
{
	m_infos_on_fail.push_back(_str);
}

void SGameTaskObjective::AddCompleteFunc_script(LPCSTR _str)
{
	m_pScriptHelper.m_s_complete_lua_functions.push_back(_str);
}
void SGameTaskObjective::AddFailFunc_script(LPCSTR _str)
{
	m_pScriptHelper.m_s_fail_lua_functions.push_back(_str);
}
void SGameTaskObjective::AddOnCompleteFunc_script(LPCSTR _str)
{
	m_pScriptHelper.m_s_lua_functions_on_complete.push_back(_str);
}
void SGameTaskObjective::AddOnFailFunc_script(LPCSTR _str)
{
	m_pScriptHelper.m_s_lua_functions_on_fail.push_back(_str);
}



void CGameTask::Load_script(LPCSTR _id)		
{
	Load(_id);
}

void CGameTask::SetTitle_script(LPCSTR _title)		
{
	m_Title	= _title;
}

void CGameTask::SetPriority_script(int _prio)		
{
	m_priority	= _prio;
}

void CGameTask::AddObjective_script(SGameTaskObjective* O)	
{
	O->m_pScriptHelper.init_functors(O->m_pScriptHelper.m_s_complete_lua_functions,		O->m_complete_lua_functions);
	O->m_pScriptHelper.init_functors(O->m_pScriptHelper.m_s_fail_lua_functions,			O->m_fail_lua_functions);
	O->m_pScriptHelper.init_functors(O->m_pScriptHelper.m_s_lua_functions_on_complete,	O->m_lua_functions_on_complete);
	O->m_pScriptHelper.init_functors(O->m_pScriptHelper.m_s_lua_functions_on_fail,		O->m_lua_functions_on_fail);

	m_Objectives.push_back(*O);
}

void SGameTaskObjective::ChangeStateCallback()
{
	Actor()->callback(GameObject::eTaskStateChange)(parent, this, task_state);
}


void SGameTaskObjective::save(IWriter &stream)
{
		save_data(idx,					stream);
		save_data(task_state,			stream);

		save_data(description,			stream);
		save_data(map_location,			stream);
		save_data(object_id,			stream);
		save_data(task_state,			stream);
		save_data(def_location_enabled,	stream);
		save_data(map_hint,				stream);
		save_data(icon_texture_name,	stream);
		save_data(icon_rect,			stream);
		save_data(article_id,			stream);

		save_data(m_completeInfos,		stream);
		save_data(m_failInfos,			stream);
		save_data(m_infos_on_complete,	stream);
		save_data(m_infos_on_fail,		stream);

		bool b_script					= m_pScriptHelper.not_empty();
		save_data(b_script,				stream);
		if(b_script)
			save_data(m_pScriptHelper,	stream);

}

void SGameTaskObjective::load(IReader &stream)
{
		load_data(idx,					stream);
		load_data(task_state,			stream);

		load_data(description,			stream);
		load_data(map_location,			stream);
		load_data(object_id,			stream);
		load_data(task_state,			stream);
		load_data(def_location_enabled,	stream);
		load_data(map_hint,				stream);
		load_data(icon_texture_name,	stream);
		load_data(icon_rect,			stream);
		load_data(article_id,			stream);

		load_data(m_completeInfos,		stream);
		load_data(m_failInfos,			stream);
		load_data(m_infos_on_complete,	stream);
		load_data(m_infos_on_fail,		stream);


		bool b_script;
		load_data(b_script,				stream);
		if(b_script){
			load_data(m_pScriptHelper,	stream);

			m_pScriptHelper.init_functors	(m_pScriptHelper.m_s_complete_lua_functions,	m_complete_lua_functions);
			m_pScriptHelper.init_functors	(m_pScriptHelper.m_s_fail_lua_functions,		m_fail_lua_functions);
			m_pScriptHelper.init_functors	(m_pScriptHelper.m_s_lua_functions_on_complete, m_lua_functions_on_complete);
			m_pScriptHelper.init_functors	(m_pScriptHelper.m_s_lua_functions_on_fail,		m_lua_functions_on_fail);
		}
}

void SScriptObjectiveHelper::init_functors(xr_vector<shared_str>& v_src, xr_vector<luabind::functor<bool> >& v_dest)
{
	xr_vector<shared_str>::iterator it		= v_src.begin();
	xr_vector<shared_str>::iterator it_e	= v_src.end();
	v_dest.resize(v_src.size());

	for(u32 idx=0 ;it!=it_e;++it,++idx)
	{
			bool functor_exists		= ai().script_engine().functor(*(*it) ,v_dest[idx]);
			if(!functor_exists)		Log("Cannot find script function described in task objective  ", *(*it));
	}
}

void SScriptObjectiveHelper::load(IReader &stream)
{
		load_data(m_s_complete_lua_functions,		stream);
		load_data(m_s_fail_lua_functions,			stream);
		load_data(m_s_lua_functions_on_complete,	stream);
		load_data(m_s_lua_functions_on_fail,		stream);
}

void SScriptObjectiveHelper::save(IWriter &stream)
{
		save_data(m_s_complete_lua_functions,		stream);
		save_data(m_s_fail_lua_functions,			stream);
		save_data(m_s_lua_functions_on_complete,	stream);
		save_data(m_s_lua_functions_on_fail,		stream);
}

void SGameTaskKey::save(IWriter &stream)
{
	save_data(task_id,						stream);
	save_data(game_task->m_ReceiveTime,		stream);
	save_data(game_task->m_FinishTime,		stream);
	save_data(game_task->m_TimeToComplete,	stream);
	save_data(game_task->m_Title,			stream);

	u32 cnt	= game_task->m_Objectives.size();
	save_data(cnt, stream);

	OBJECTIVE_VECTOR_IT it		= game_task->m_Objectives.begin();
	OBJECTIVE_VECTOR_IT it_e	= game_task->m_Objectives.end();
	for(;it!=it_e;++it)
		save_data(*it, stream);

}

void SGameTaskKey::load(IReader &stream)
{
	load_data(task_id,						stream);
	game_task = xr_new<CGameTask>			(task_id);
	load_data(game_task->m_ReceiveTime,		stream);
	load_data(game_task->m_FinishTime,		stream);
	load_data(game_task->m_TimeToComplete,	stream);

	load_data(game_task->m_Title,			stream);

	u32 cnt;
	load_data(cnt, stream);

	if(cnt>game_task->m_Objectives.size())
		game_task->m_Objectives.resize(cnt);

	for(u32 i=0; i<cnt; ++i){
		load_data(game_task->m_Objectives[i], stream);
		game_task->m_Objectives[i].parent = game_task;
	}
}

void SGameTaskKey::destroy()
{
	delete_data(game_task);
}