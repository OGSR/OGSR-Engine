//=============================================================================
//  Filename:   UIEncyclopediaWnd.cpp
//	Created by Roman E. Marchenko, vortex@gsc-game.kiev.ua
//	Copyright 2004. GSC Game World
//	---------------------------------------------------------------------------
//  Encyclopedia window
//=============================================================================

#include "StdAfx.h"
#include "UIEncyclopediaWnd.h"
#include "UIXmlInit.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIAnimatedStatic.h"
#include "UIListWnd.h"
#include "UIScrollView.h"
#include "UITreeViewItem.h"
#include "UIPdaAux.h"
#include "UIEncyclopediaArticleWnd.h"
#include "../encyclopedia_article.h"
#include "../alife_registry_wrappers.h"
#include "../actor.h"
#include "../object_broker.h"
#include "../string_table.h"

#define				ENCYCLOPEDIA_DIALOG_XML		"encyclopedia.xml"

CUIEncyclopediaWnd::CUIEncyclopediaWnd()
{
	prevArticlesCount	= 0;
}

CUIEncyclopediaWnd::~CUIEncyclopediaWnd()
{
	DeleteArticles();
}


void CUIEncyclopediaWnd::Init()
{
	CUIXml		uiXml;
	bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, ENCYCLOPEDIA_DIALOG_XML);
	R_ASSERT3(xml_result, "xml file not found", ENCYCLOPEDIA_DIALOG_XML);

	CUIXmlInit	xml_init;

	xml_init.InitWindow		(uiXml, "main_wnd", 0, this);

	// Load xml data
	UIEncyclopediaIdxBkg		= xr_new<CUIFrameWindow>(); UIEncyclopediaIdxBkg->SetAutoDelete(true);
	AttachChild(UIEncyclopediaIdxBkg);
	xml_init.InitFrameWindow(uiXml, "right_frame_window", 0, UIEncyclopediaIdxBkg);

	xml_init.InitFont(uiXml, "tree_item_font", 0, m_uTreeItemColor, m_pTreeItemFont);
	R_ASSERT(m_pTreeItemFont);
	xml_init.InitFont(uiXml, "tree_root_font", 0, m_uTreeRootColor, m_pTreeRootFont);
	R_ASSERT(m_pTreeRootFont);


	UIEncyclopediaIdxHeader		= xr_new<CUIFrameLineWnd>(); UIEncyclopediaIdxHeader->SetAutoDelete(true);
	UIEncyclopediaIdxBkg->AttachChild(UIEncyclopediaIdxHeader);
	xml_init.InitFrameLine(uiXml, "right_frame_line", 0, UIEncyclopediaIdxHeader);

	UIAnimation					= xr_new<CUIAnimatedStatic>(); UIAnimation->SetAutoDelete(true);
	UIEncyclopediaIdxHeader->AttachChild(UIAnimation);
	xml_init.InitAnimatedStatic(uiXml, "a_static", 0, UIAnimation);

	UIEncyclopediaInfoBkg		= xr_new<CUIFrameWindow>();UIEncyclopediaInfoBkg->SetAutoDelete(true);
	AttachChild(UIEncyclopediaInfoBkg);
	xml_init.InitFrameWindow(uiXml, "left_frame_window", 0, UIEncyclopediaInfoBkg);

	UIEncyclopediaInfoHeader	= xr_new<CUIFrameLineWnd>();UIEncyclopediaInfoHeader->SetAutoDelete(true);
	UIEncyclopediaInfoBkg->AttachChild(UIEncyclopediaInfoHeader);

	UIEncyclopediaInfoHeader->UITitleText.SetElipsis(CUIStatic::eepBegin, 20);
	xml_init.InitFrameLine(uiXml, "left_frame_line", 0, UIEncyclopediaInfoHeader);

	UIArticleHeader				= xr_new<CUIStatic>(); UIArticleHeader->SetAutoDelete(true);
	UIEncyclopediaInfoBkg->AttachChild(UIArticleHeader);
	xml_init.InitStatic(uiXml, "article_header_static", 0, UIArticleHeader);

	UIIdxList					= xr_new<CUIListWnd>(); UIIdxList->SetAutoDelete(true);
	UIEncyclopediaIdxBkg->AttachChild(UIIdxList);
	xml_init.InitListWnd(uiXml, "idx_list", 0, UIIdxList);
	UIIdxList->SetMessageTarget(this);
	UIIdxList->EnableScrollBar(true);

	UIInfoList					= xr_new<CUIScrollView>(); UIInfoList->SetAutoDelete(true);
	UIEncyclopediaInfoBkg->AttachChild(UIInfoList);
	xml_init.InitScrollView(uiXml, "info_list", 0, UIInfoList);

	xml_init.InitAutoStatic(uiXml, "left_auto_static", UIEncyclopediaInfoBkg);
	xml_init.InitAutoStatic(uiXml, "right_auto_static", UIEncyclopediaIdxBkg);
}

#include "../string_table.h"
void CUIEncyclopediaWnd::SendMessage(CUIWindow *pWnd, s16 msg, void* pData)
{
	if (UIIdxList == pWnd && LIST_ITEM_CLICKED == msg)
	{
		CUITreeViewItem *pTVItem = static_cast<CUITreeViewItem*>(pData);
		R_ASSERT		(pTVItem);
		
		if( pTVItem->vSubItems.size() )
		{
			auto& A = m_ArticlesDB[pTVItem->vSubItems[0]->GetValue()];

			xr_string caption		= ALL_PDA_HEADER_PREFIX;
			caption					+= "/";
			caption					+= CStringTable().translate(A.data()->group).c_str();

			UIEncyclopediaInfoHeader->UITitleText.SetText(caption.c_str());
			UIArticleHeader->SetTextST(*(A.data()->group));
			SetCurrentArtice		(NULL);
		}else
		{
			auto& A = m_ArticlesDB[pTVItem->GetValue()];
			xr_string caption		= ALL_PDA_HEADER_PREFIX;
			caption					+= "/";
			caption					+= CStringTable().translate(A.data()->group).c_str();
			caption					+= "/";
			caption					+= CStringTable().translate(A.data()->name).c_str();

			UIEncyclopediaInfoHeader->UITitleText.SetText(caption.c_str());
			SetCurrentArtice		(pTVItem);
			UIArticleHeader->SetTextST(*(A.data()->name));
		}
	}

	inherited::SendMessage(pWnd, msg, pData);
}

void CUIEncyclopediaWnd::Draw()
{
	UpdateArticles();
	inherited::Draw();
}


void CUIEncyclopediaWnd::ReloadArticles() {
  if ( Actor() && Actor()->encyclopedia_registry->registry().objects_ptr()->size() < prevArticlesCount )
    ResetArticles();
  else
    m_flags.set( eNeedReload, TRUE );
}


void CUIEncyclopediaWnd::Show(bool status)
{
	if (status)
		ReloadArticles();

	inherited::Show(status);
}


bool CUIEncyclopediaWnd::HasArticle(shared_str id)
{
	ReloadArticles();

	for (auto& Art : m_ArticlesDB)
		if (Art.Id() == id)
			return true;

	return false;
}


void CUIEncyclopediaWnd::DeleteArticles()
{
	UIIdxList->RemoveAll();
	m_ArticlesDB.clear();
}

void CUIEncyclopediaWnd::SetCurrentArtice(CUITreeViewItem *pTVItem)
{
	UIInfoList->ScrollToBegin();
	UIInfoList->Clear();

	if(!pTVItem) return;

	// для начала проверим, что нажатый элемент не рутовый
	if (!pTVItem->IsRoot())
	{

		CUIEncyclopediaArticleWnd*	article_info = xr_new<CUIEncyclopediaArticleWnd>();
		article_info->Init			("encyclopedia_item.xml","encyclopedia_wnd:objective_item");
		article_info->SetArticle	(&m_ArticlesDB[pTVItem->GetValue()]);
		UIInfoList->AddWindow		(article_info, true);

		// Пометим как прочитанную
		if (!pTVItem->IsArticleReaded())
		{
			if(Actor()->encyclopedia_registry->registry().objects_ptr())
			{
				for(ARTICLE_VECTOR::iterator it = Actor()->encyclopedia_registry->registry().objects().begin();
					it != Actor()->encyclopedia_registry->registry().objects().end(); it++)
				{
					if (ARTICLE_DATA::eEncyclopediaArticle == it->article_type &&
						m_ArticlesDB[pTVItem->GetValue()].Id() == it->article_id)
					{
						it->readed = true;
						break;
					}
				}
			}
		}
	}
}

CEncyclopediaArticle* CUIEncyclopediaWnd::AddArticle( shared_str article_id, bool bReaded )
{
	for (auto& Art : m_ArticlesDB)
		if (Art.Id() == article_id)
			return nullptr;

	// Добавляем элемент
	auto& a = m_ArticlesDB.emplace_back();
	a.Load(article_id);
	a.readed = bReaded;
	return &a;
}

void CUIEncyclopediaWnd::Reset()
{
	inherited::Reset	();
	ResetArticles();
}


void CUIEncyclopediaWnd::ResetArticles() {
  m_flags.set( eNeedReload, TRUE );
  DeleteArticles();
  prevArticlesCount = 0;
}


void CUIEncyclopediaWnd::FillEncyclopedia() {
  ResetArticles();
  UpdateArticles();
}


void CUIEncyclopediaWnd::UpdateArticles() {
  if ( m_flags.test( eNeedReload ) && Actor()->encyclopedia_registry->registry().objects_ptr() ) {
    if ( Actor()->encyclopedia_registry->registry().objects_ptr()->size() > prevArticlesCount ) {
      auto it = Actor()->encyclopedia_registry->registry().objects_ptr()->begin();
      std::advance( it, prevArticlesCount );
      bool need_sort = false;
      for ( ; it != Actor()->encyclopedia_registry->registry().objects_ptr()->end(); it++ ) {
        if ( ARTICLE_DATA::eEncyclopediaArticle == it->article_type ) {
          auto* a = AddArticle( it->article_id, it->readed );
          if (a && a->data()->sort)
            need_sort = true;
          if ( prevArticlesCount > 0 && a && !need_sort ) {
            // Теперь создаем иерархию вещи по заданному пути
            CreateTreeBranch(
              a->data()->group, a->data()->name, UIIdxList,
              m_ArticlesDB.size() - 1, m_pTreeRootFont, m_uTreeRootColor,
              m_pTreeItemFont, m_uTreeItemColor, it->readed
            );
          }
        }
      }
      if ( prevArticlesCount == 0 || need_sort ) {
        if ( need_sort ) {
          UIIdxList->RemoveAll();

          std::vector< std::vector<CEncyclopediaArticle*> > groups;
          for ( auto& a : m_ArticlesDB ) {
            bool found = false;
            for ( auto& g : groups ) {
              if ( xr_strcmp( g.front()->data()->group, a.data()->group ) == 0 ) {
                found = true;
                g.push_back( &a );
                break;
              }
            }
            if ( !found ) {
              groups.emplace_back().push_back( &a );
            }
          }
          for ( auto& g : groups ) {
            auto sort_it = std::find_if(
              g.begin(), g.end(),
              []( auto* a ) -> bool {
                return a->data()->sort;
              }
            );
            if ( sort_it != g.end() )
              std::stable_sort(
                g.begin(), g.end(),
                []( auto* a, auto* b ) -> bool {
                  return xr_strcmp( CStringTable().translate( a->data()->name ), CStringTable().translate( b->data()->name ) ) < 0;
                }
              );
          }

          std::vector<CEncyclopediaArticle> m_ArticlesDB_new;
          for ( auto& g : groups )
            for ( const auto* a : g )
              m_ArticlesDB_new.push_back( *a );
		  m_ArticlesDB = std::move(m_ArticlesDB_new);
        }
        int idx = 0;
        for ( auto& a : m_ArticlesDB )
          // Теперь создаем иерархию вещи по заданному пути
          CreateTreeBranch(
            a.data()->group, a.data()->name, UIIdxList, idx++,
            m_pTreeRootFont, m_uTreeRootColor, m_pTreeItemFont,
            m_uTreeItemColor, a.readed
          );
      }
      prevArticlesCount = Actor()->encyclopedia_registry->registry().objects_ptr()->size();
    }
    m_flags.set( eNeedReload, FALSE );
  }
}
