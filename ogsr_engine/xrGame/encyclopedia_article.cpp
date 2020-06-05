///////////////////////////////////////////////////////////////
// encyclopedia_article.cpp
// структура, хранящая и загружающая статьи в энциклопедию
///////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "encyclopedia_article.h"
#include "ui/xrUIXmlParser.h"
#include "ui/UIXmlInit.h"
#include "ui/UIInventoryUtilities.h"
#include "object_broker.h"

using namespace InventoryUtilities;

void ARTICLE_DATA::load (IReader& stream) 
{
	load_data(receive_time, stream); 
	load_data(article_id, stream); 
	load_data(readed, stream); 
	load_data(article_type, stream);
}

void ARTICLE_DATA::save (IWriter& stream)		
{
	save_data(receive_time, stream); 
	save_data(article_id, stream); 
	save_data(readed, stream); 
	save_data(article_type, stream);
}

CEncyclopediaArticle::CEncyclopediaArticle() {
  readed = false;
}

CEncyclopediaArticle::~CEncyclopediaArticle()
{
	if( data()->image.GetParent() )
		data()->image.GetParent()->DetachChild( &(data()->image) );
}

/*
void CEncyclopediaArticle::Load	(ARTICLE_STR_ID str_id)
{
	Load	(id_to_index::IdToIndex(str_id));
}
*/
void CEncyclopediaArticle::Load	(shared_str  id)
{
	m_ArticleId = id;
	inherited_shared::load_shared(m_ArticleId, NULL);
}


void CEncyclopediaArticle::load_shared	(LPCSTR)
{
	const ITEM_DATA& item_data = *id_to_index::GetById(m_ArticleId);

	CUIXml*		pXML		= item_data._xml;
	pXML->SetLocalRoot		(pXML->GetRoot());

	//loading from XML
	XML_NODE* pNode = pXML->NavigateToNode(id_to_index::tag_name, item_data.pos_in_file);
	THROW3(pNode, "encyclopedia article id=", *item_data.id);

	//текст
	data()->text = pXML->Read(pNode, "text", 0, "");
	//имя
	data()->name = pXML->ReadAttrib(pNode, "name", "");
	//группа
	data()->group = pXML->ReadAttrib(pNode, "group", "");
	//секция ltx, откуда читать данные
	LPCSTR ltx = pXML->Read(pNode, "ltx", 0, NULL);


	if(ltx)
	{
		CIconParams params( ltx );
		params.set_shader( &( data()->image ) );
		data()->image.ClipperOn();
		data()->image.TextureAvailable(true);
	}
	else 
	{
		if( pXML->NavigateToNode(pNode,"texture",0) ){
			pXML->SetLocalRoot(pNode);
			CUIXmlInit::InitTexture(*pXML, "", 0, &data()->image);
			pXML->SetLocalRoot(pXML->GetRoot());
		}
	}

	if(data()->image.TextureAvailable() ){
		Frect r = data()->image.GetUIStaticItem().GetOriginalRect();
		data()->image.SetAutoDelete(false);

		const int minSize = 65;

		// Сначала устанавливаем если надо минимально допустимые размеры иконки
		if (r.width() < minSize)
		{
			float dx = minSize - r.width();
			r.x2 += dx;
			data()->image.SetTextureOffset(dx / 2, data()->image.GetTextureOffeset()[1]);
		}

		if (r.height() < minSize)
		{
			float dy = minSize - r.height();
			r.y2 += dy;
			data()->image.SetTextureOffset(data()->image.GetTextureOffeset()[0], dy / 2);
		}

		data()->image.SetWndRect(0, 0, r.width(), r.height());
	};

	// Тип статьи
	xr_string atricle_type = pXML->ReadAttrib(pNode, "article_type", "encyclopedia");
	if(0==stricmp(atricle_type.c_str(),"encyclopedia")){
		data()->articleType = ARTICLE_DATA::eEncyclopediaArticle;
	}else
	if(0==stricmp(atricle_type.c_str(),"journal")){
		data()->articleType = ARTICLE_DATA::eJournalArticle;
	}else
	if(0==stricmp(atricle_type.c_str(),"task")){
		data()->articleType = ARTICLE_DATA::eTaskArticle;
	}else
	if(0==stricmp(atricle_type.c_str(),"info")){
		data()->articleType = ARTICLE_DATA::eInfoArticle;
	}else{
		Msg("incorrect article type definition for [%s]",*item_data.id);
	}

	data()->ui_template_name = pXML->ReadAttrib(pNode, "ui_template", "common");
	data()->sort = !!pXML->ReadAttribInt( pNode, "sort", 0 );
}

void CEncyclopediaArticle::InitXmlIdToIndex()
{
	if(!id_to_index::tag_name)
		id_to_index::tag_name = "article";
	if(!id_to_index::file_str)
		id_to_index::file_str = pSettings->r_string("encyclopedia", "files");
}
