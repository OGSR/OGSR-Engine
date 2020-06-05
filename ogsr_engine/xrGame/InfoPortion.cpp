#include "stdafx.h"
#include "InfoPortion.h"
#include "gameobject.h"
#include "encyclopedia_article.h"
#include "gametask.h"
#include "ai_space.h"
#include "alife_simulator.h"
#include "alife_story_registry.h"
#include "xrServer_Objects_ALife.h"
#include "script_engine.h"
#include "ui\uixmlinit.h"
#include "object_broker.h"

void INFO_DATA::load (IReader& stream) 
{
	load_data(info_id, stream); 
	load_data(receive_time, stream);
}

void INFO_DATA::save (IWriter& stream) 
{
	save_data(info_id, stream); 
	save_data(receive_time, stream);
}


SInfoPortionData::SInfoPortionData ()
{
}
SInfoPortionData::~SInfoPortionData ()
{
}

CInfoPortion::CInfoPortion()
{
}

CInfoPortion::~CInfoPortion ()
{
}

void CInfoPortion::Load	(shared_str info_id)
{
	m_InfoId = info_id;
	inherited_shared::load_shared(m_InfoId, NULL);
}


void CInfoPortion::load_shared	(LPCSTR)
{
	const ITEM_DATA& item_data = *id_to_index::GetById(m_InfoId);

	CUIXml*		pXML		= item_data._xml;
	pXML->SetLocalRoot		(pXML->GetRoot());

	//loading from XML
	XML_NODE* pNode			= pXML->NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	THROW3					(pNode, "info_portion id=", *item_data.id);

	//список названий диалогов
	int dialogs_num			= pXML->GetNodesNum(pNode, "dialog");
	info_data()->m_DialogNames.clear();
	for(int i=0; i<dialogs_num; ++i)
	{
		shared_str dialog_name = pXML->Read(pNode, "dialog", i,"");
		info_data()->m_DialogNames.push_back(dialog_name);
	}

	
	//список названий порций информации, которые деактивируются,
	//после получения этой порции
	int disable_num = pXML->GetNodesNum(pNode, "disable");
	info_data()->m_DisableInfo.clear();
	for(int i=0; i<disable_num; ++i)
	{
		shared_str info_id		= pXML->Read(pNode, "disable", i,"");
		info_data()->m_DisableInfo.push_back(info_id);
	}

	//имена скриптовых функций
	info_data()->m_PhraseScript.Load(pXML, pNode);


	//индексы статей
	info_data()->m_Articles.clear();
	int articles_num	= pXML->GetNodesNum(pNode, "article");
	for(int i=0; i<articles_num; ++i)
	{
		LPCSTR article_str_id = pXML->Read(pNode, "article", i, NULL);
		THROW(article_str_id);
		info_data()->m_Articles.push_back(article_str_id);
	}

	info_data()->m_ArticlesDisable.clear();
	articles_num = pXML->GetNodesNum(pNode, "article_disable");
	for(int i=0; i<articles_num; ++i)
	{
		LPCSTR article_str_id = pXML->Read(pNode, "article_disable", i, NULL);
		THROW(article_str_id);
		info_data()->m_ArticlesDisable.push_back(article_str_id);
	}
	
	info_data()->m_GameTasks.clear();
	int task_num = pXML->GetNodesNum(pNode, "task");
	for(int i=0; i<task_num; ++i)
	{
		LPCSTR task_str_id = pXML->Read(pNode, "task", i, NULL);
		THROW(task_str_id);
		info_data()->m_GameTasks.push_back(task_str_id);
	}
}

void   CInfoPortion::InitXmlIdToIndex()
{
	if(!id_to_index::tag_name)
		id_to_index::tag_name = "info_portion";
	if(!id_to_index::file_str)
		id_to_index::file_str = pSettings->r_string("info_portions", "files");
}

void _destroy_item_data_vector_cont(T_VECTOR* vec)
{
	T_VECTOR::iterator it		= vec->begin();
	T_VECTOR::iterator it_e		= vec->end();

	xr_vector<CUIXml*>			_tmp;	
	for(;it!=it_e;++it)
	{
		xr_vector<CUIXml*>::iterator it_f = std::find(_tmp.begin(), _tmp.end(), (*it)._xml);
		if(it_f==_tmp.end())
//.		{
			_tmp.push_back	((*it)._xml);
//.			Msg("%s is unique",(*it)._xml->m_xml_file_name);
//.		}else
//.			Msg("%s already in list",(*it)._xml->m_xml_file_name);

	}
//.	Log("_tmp.size()",_tmp.size());
	delete_data	(_tmp);
}
