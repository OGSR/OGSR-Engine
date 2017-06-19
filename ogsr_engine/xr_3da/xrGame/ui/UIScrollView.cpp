#include "stdafx.h"
#include "UIScrollView.h"
#include "UIScrollBar.h"
#include "../ui_base.h"
#include "../UICursor.h"
#include "../../xr_input.h"	

CUIScrollView::CUIScrollView()
{
	m_rightIndent		= 0.0f;
	m_leftIndent		= 0.0f;
	m_vertInterval		= 0.0f;
   	m_upIndent			= 0.0f;
	m_downIndent		= 0.0f;
	m_flags.zero		();
	SetFixedScrollBar	(true);
	m_pad = NULL;
	m_VScrollBar = NULL;
}

CUIScrollView::~CUIScrollView()
{
	Clear	();
}

void CUIScrollView::SendMessage	(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
	if (CHILD_CHANGED_SIZE == msg && m_pad->IsChild(pWnd))
		m_flags.set			(eNeedRecalc,TRUE);
}

void CUIScrollView::ForceUpdate(){
	m_flags.set			(eNeedRecalc,TRUE);
}

void CUIScrollView::Init				()
{
	if (!m_pad)
	{
        m_pad						= xr_new<CUIWindow>(); m_pad->SetAutoDelete(true);
		AttachChild					(m_pad);
	}
	m_pad->SetWndPos			(0.0f,0.0f);
	if (!m_VScrollBar)
	{
        m_VScrollBar				= xr_new<CUIScrollBar>();m_VScrollBar->SetAutoDelete(true);
		AttachChild					(m_VScrollBar);
		Register					(m_VScrollBar);
		AddCallback					("scroll_v",	SCROLLBAR_VSCROLL, CUIWndCallback::void_function (this, &CUIScrollView::OnScrollV) );
	}
	if (!!m_scrollbar_profile)
        m_VScrollBar->Init			(GetWndSize().x, 0.0f, GetWndSize().y, false, *m_scrollbar_profile);
	else
		m_VScrollBar->Init			(GetWndSize().x, 0.0f, GetWndSize().y, false);
	m_VScrollBar->SetWndPos			(m_VScrollBar->GetWndPos().x - m_VScrollBar->GetWndSize().x, m_VScrollBar->GetWndPos().y);
	m_VScrollBar->SetWindowName		("scroll_v");
	m_VScrollBar->SetStepSize		(_max(1,iFloor(GetHeight()/10)));
	m_VScrollBar->SetPageSize		(iFloor(GetHeight()));
	

}

void CUIScrollView::SetScrollBarProfile(LPCSTR profile){
	m_scrollbar_profile = profile;
}

void CUIScrollView::AddWindow			(CUIWindow* pWnd, bool auto_delete)
{
	if(auto_delete)		pWnd->SetAutoDelete	(true);

	m_pad->AttachChild	(pWnd);
	m_flags.set			(eNeedRecalc,TRUE);
}

void CUIScrollView::RemoveWindow		(CUIWindow* pWnd)
{
	m_pad->DetachChild	(pWnd);
	m_flags.set			(eNeedRecalc,TRUE);
}

void CUIScrollView::Clear				()
{
	m_pad->DetachAll	();
	m_flags.set			(eNeedRecalc,TRUE);
	
	m_flags.set			(eNeedRecalc,TRUE);
	ScrollToBegin		();
}

void CUIScrollView::Update				()
{
	if(m_flags.test	(eNeedRecalc) )
		RecalcSize			();

	inherited::Update();
}

void CUIScrollView::RecalcSize			()
{
	if(!m_pad)			return;
	Fvector2			pad_size;
	pad_size.set		(0.0f, 0.0f);

	Fvector2			item_pos;
	item_pos.set		(m_rightIndent, m_vertInterval + m_upIndent);
	pad_size.y			+= m_upIndent;
	pad_size.y			+= m_downIndent;

	if(GetVertFlip()){
		for(WINDOW_LIST::reverse_iterator it = m_pad->GetChildWndList().rbegin(); m_pad->GetChildWndList().rend() != it; ++it)
		{
			(*it)->SetWndPos		(item_pos);
			item_pos.y				+= (*it)->GetWndSize().y;
			item_pos.y				+= m_vertInterval; 
			pad_size.y				+= (*it)->GetWndSize().y;
			pad_size.y				+= m_vertInterval;
			pad_size.x				= _max(pad_size.x, (*it)->GetWndSize().x);
		}

	}else{
		for(WINDOW_LIST_it it = m_pad->GetChildWndList().begin(); m_pad->GetChildWndList().end() != it; ++it)
		{
			(*it)->SetWndPos		(item_pos);
			item_pos.y				+= (*it)->GetWndSize().y;
			item_pos.y				+= m_vertInterval; 
			pad_size.y				+= (*it)->GetWndSize().y;
			pad_size.y				+= m_vertInterval;
			pad_size.x				= _max(pad_size.x, (*it)->GetWndSize().x);
		}
	};

	m_pad->SetWndSize			(pad_size);


	if(m_flags.test(eInverseDir) )
		m_pad->SetWndPos		(m_pad->GetWndPos().x, GetHeight()-m_pad->GetHeight());

	UpdateScroll				();

	m_flags.set			(eNeedRecalc,FALSE);
}

void CUIScrollView::UpdateScroll		()
{

	Fvector2 w_pos					= m_pad->GetWndPos();
	m_VScrollBar->SetHeight(GetHeight());
	m_VScrollBar->SetRange		(0,iFloor(m_pad->GetHeight()*Scroll2ViewV()));

	m_VScrollBar->SetScrollPos	(iFloor(-w_pos.y));
}

float CUIScrollView::Scroll2ViewV	(){
	float h = m_VScrollBar->GetHeight();
	return (h + GetVertIndent())/h;
}

void CUIScrollView::SetFixedScrollBar	(bool b)
{
	m_flags.set(eFixedScrollBar,b);
}

void CUIScrollView::Draw				()
{
	if(m_flags.test	(eNeedRecalc) )
		RecalcSize			();

	Frect				visible_rect;
	GetAbsoluteRect		(visible_rect);
	visible_rect.top	+= m_upIndent;
	visible_rect.bottom -= m_downIndent;
	UI()->PushScissor					(visible_rect);
	int iDone = 0;

	for(	WINDOW_LIST_it it = m_pad->GetChildWndList().begin(); 
			m_pad->GetChildWndList().end()!=it; 
			++it)
	{
		Frect	item_rect;
		(*it)->GetAbsoluteRect(item_rect);
		if(visible_rect.intersected		(item_rect)){
			if ((*it)->GetVisible())
                (*it)->Draw();
			iDone						= 1;
		}else
			if(iDone==1)	break;
	}
	UI()->PopScissor					();

	if(NeedShowScrollBar())
		m_VScrollBar->Draw					();
}

bool CUIScrollView::NeedShowScrollBar(){
	return m_flags.test(eFixedScrollBar) || GetHeight()<m_pad->GetHeight();
}

void CUIScrollView::OnScrollV			(CUIWindow*, void*)
{
	int s_pos					= m_VScrollBar->GetScrollPos();
	Fvector2 w_pos				= m_pad->GetWndPos();
	m_pad->SetWndPos			(w_pos.x,float(-s_pos));
}

bool CUIScrollView::OnMouse(float x, float y, EUIMessages mouse_action)
{
	if(inherited::OnMouse(x,y,mouse_action)) return true;

	switch (mouse_action){
		case WINDOW_MOUSE_WHEEL_UP:
			m_VScrollBar->TryScrollDec();
			return true;
		break;
		case WINDOW_MOUSE_WHEEL_DOWN:
			m_VScrollBar->TryScrollInc();
			return true;
		break;
		case WINDOW_MOUSE_MOVE:
			if( pInput->iGetAsyncBtnState(0) ){
				Fvector2	curr_pad_pos = m_pad->GetWndPos	();
				curr_pad_pos.y				+= GetUICursor()->GetCursorPositionDelta().y;
				
				float max_pos = m_pad->GetHeight() - GetHeight();
				max_pos							= _max(0.0f,max_pos);
				clamp							(curr_pad_pos.y,-max_pos,0.0f);
				m_pad->SetWndPos				(curr_pad_pos);
				UpdateScroll					();
				return true;
			}
		break;
	};
	return false;
}

int CUIScrollView::GetMinScrollPos()
{
	return m_VScrollBar->GetMinRange();
}

int CUIScrollView::GetMaxScrollPos()
{
	return m_VScrollBar->GetMaxRange();
}
int CUIScrollView::GetCurrentScrollPos()
{
	return m_VScrollBar->GetMinRange();
}

void CUIScrollView::SetScrollPos(int value)
{
	clamp(value,GetMinScrollPos(),GetMaxScrollPos());
	m_VScrollBar->SetScrollPos(value);
	OnScrollV(NULL,NULL);
}

void CUIScrollView::ScrollToBegin		()
{
	if(m_flags.test	(eNeedRecalc) )
		RecalcSize			();

	m_VScrollBar->SetScrollPos(m_VScrollBar->GetMinRange());
	OnScrollV(NULL,NULL);
}

void CUIScrollView::ScrollToEnd			()
{
	if(m_flags.test	(eNeedRecalc) )
		RecalcSize			();

	m_VScrollBar->SetScrollPos(m_VScrollBar->GetMaxRange());
	OnScrollV(NULL,NULL);
}

void CUIScrollView::SetRightIndention	(float val)
{
	m_rightIndent		= val;
	m_flags.set			(eNeedRecalc,TRUE);
}

void CUIScrollView::SetLeftIndention	(float val)
{
	m_leftIndent			= val;
	m_flags.set			(eNeedRecalc,TRUE);
}

void CUIScrollView::SetUpIndention(float val){
	m_upIndent			= val;
	m_flags.set			(eNeedRecalc,TRUE);
}

void CUIScrollView::SetDownIndention(float val){
	m_downIndent			= val;
	m_flags.set			(eNeedRecalc,TRUE);
}

u32 CUIScrollView::GetSize				()
{
	return m_pad->GetChildNum		();
}

CUIWindow* CUIScrollView::GetItem		(u32 idx)
{
	WINDOW_LIST_it it = m_pad->GetChildWndList().begin();
	std::advance(it, idx);
	return (*it);
}

float CUIScrollView::GetDesiredChildWidth(){
	if (NeedShowScrollBar())
        return GetWidth() - m_VScrollBar->GetWidth() - m_rightIndent - m_leftIndent;
	else
		return GetWidth() - m_rightIndent - m_leftIndent;
}

float CUIScrollView::GetHorizIndent(){
	return m_rightIndent + m_leftIndent;
}

float CUIScrollView::GetVertIndent(){
	return m_upIndent + m_downIndent;
}

void CUIScrollView::SetSelected			(CUIWindow* w)
{
	if(!m_flags.test(eItemsSelectabe)) return;

	for(WINDOW_LIST_it it = m_pad->GetChildWndList().begin(); m_pad->GetChildWndList().end()!=it; ++it)
	{
		smart_cast<CUISelectable*>(*it)->SetSelected(*it==w);
	}
}

CUIWindow* CUIScrollView::GetSelected(){
	if(!m_flags.test(eItemsSelectabe))
		return NULL;

	for(WINDOW_LIST_it it = m_pad->GetChildWndList().begin(); m_pad->GetChildWndList().end()!=it; ++it)
	{
		if (smart_cast<CUISelectable*>(*it)->GetSelected())
			return *it;
	}

	return NULL;
}

void CUIScrollView::UpdateChildrenLenght(){
	float len = GetDesiredChildWidth();
	for(WINDOW_LIST_it it = m_pad->GetChildWndList().begin(); m_pad->GetChildWndList().end()!=it; ++it)
	{
		(*it)->SetWidth(len);
	}
}

