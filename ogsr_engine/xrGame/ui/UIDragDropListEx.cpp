#include "stdafx.h"
#include "UIDragDropListEx.h"
#include "UIScrollBar.h"
#include "../object_broker.h"
#include "UICellItem.h"

#include "UIInventoryWnd.h"
#include "UITradeWnd.h"
#include "UICarBodyWnd.h"


CUIDragItem* CUIDragDropListEx::m_drag_item = NULL;

void CUICell::Clear()
{
	m_bMainItem = false;
	if(m_item)	m_item->SetOwnerList(NULL);
	m_item		= NULL; 
	//cell_disabled = false;
	//is_highlighted = false;
}

CUIDragDropListEx::CUIDragDropListEx()
{
	m_flags.zero				();
	m_container					= xr_new<CUICellContainer>(this);
	m_vScrollBar				= xr_new<CUIScrollBar>();
	m_vScrollBar->SetAutoDelete	(true);
	m_selected_item				= NULL;
	m_bConditionProgBarVisible  = false;

	SetCellSize					(Ivector2().set(50,50));
	SetCellsCapacity			(Ivector2().set(0,0));

	AttachChild					(m_container);
	AttachChild					(m_vScrollBar);

	m_vScrollBar->SetWindowName	("scroll_v");
	Register					(m_vScrollBar);
	AddCallback("scroll_v", SCROLLBAR_VSCROLL, fastdelegate::MakeDelegate(this, &CUIDragDropListEx::OnScrollV));
	AddCallback("cell_item", DRAG_DROP_ITEM_DRAG, fastdelegate::MakeDelegate(this, &CUIDragDropListEx::OnItemStartDragging));
	AddCallback("cell_item", DRAG_DROP_ITEM_DROP, fastdelegate::MakeDelegate(this, &CUIDragDropListEx::OnItemDrop));
	AddCallback("cell_item", DRAG_DROP_ITEM_SELECTED, fastdelegate::MakeDelegate(this, &CUIDragDropListEx::OnItemSelected));
	AddCallback("cell_item", DRAG_DROP_ITEM_RBUTTON_CLICK, fastdelegate::MakeDelegate(this, &CUIDragDropListEx::OnItemRButtonClick));
	AddCallback("cell_item", DRAG_DROP_ITEM_DB_CLICK, fastdelegate::MakeDelegate(this, &CUIDragDropListEx::OnItemDBClick));

	back_color = 0xFFFFFFFF;
}

CUIDragDropListEx::~CUIDragDropListEx()
{
	DestroyDragItem		();

	delete_data					(m_container);
}

void CUIDragDropListEx::SetAutoGrow(bool b)						
{
	m_flags.set(flAutoGrow,b);
}
bool CUIDragDropListEx::IsAutoGrow()								
{
	return !!m_flags.test(flAutoGrow);
}
void CUIDragDropListEx::SetGrouping(bool b)						
{
	m_flags.set(flGroupSimilar,b);
}
bool CUIDragDropListEx::IsGrouping()
{
	return !!m_flags.test(flGroupSimilar);
}
void CUIDragDropListEx::SetCustomPlacement(bool b)
{
	m_flags.set(flCustomPlacement,b);
}

bool CUIDragDropListEx::GetCustomPlacement()
{
	return !!m_flags.test(flCustomPlacement);
}

bool CUIDragDropListEx::GetVerticalPlacement()
{
	return !!m_flags.test(flVerticalPlacement);
}

void CUIDragDropListEx::SetAlwaysShowScroll(bool b)
{
	m_flags.set(flAlwaysShowScroll,b);
}

void CUIDragDropListEx::SetVerticalPlacement(bool b)
{
	m_flags.set(flVerticalPlacement, b);
}

void CUIDragDropListEx::SetVirtualCells(bool b)
{
	m_flags.set(flVirtualCells,b);
}

bool CUIDragDropListEx::GetVirtualCells()
{
	return !!m_flags.test(flVirtualCells);
}

void CUIDragDropListEx::SetHighlightCellSp( bool b ) {
  m_flags.set( flHighlightCellSp, b );
}

bool CUIDragDropListEx::GetHighlightCellSp() {
  return !!m_flags.test( flHighlightCellSp );
}

void CUIDragDropListEx::SetHighlightAllCells( bool b ) {
  m_flags.set( flHighlightAllCells, b );
}

bool CUIDragDropListEx::GetHighlightAllCells() {
  return !!m_flags.test( flHighlightAllCells );
}

void CUIDragDropListEx::SendMessage(CUIWindow* pWnd, s16 msg, void* pData)
{
	CUIWndCallback::OnEvent(pWnd, msg, pData);
}

void CUIDragDropListEx::Init(float x, float y, float w, float h)
{
	inherited::SetWndRect				(x,y,w,h);
	m_vScrollBar->Init					(w, 0, h, false);
	m_vScrollBar->SetWndPos				(m_vScrollBar->GetWndPos().x - m_vScrollBar->GetWidth(), m_vScrollBar->GetWndPos().y);
/*
//.	m_background->Init					(0,0,w,h);
//.	m_background->Init					("ui\\ui_frame_02_back",0,0,w,h);
//.	Frect rect; rect.set				(0,0,64,64);
//.	m_background->GetUIStaticItem().SetOriginalRect(rect);
//.	m_background->SetStretchTexture		(true);
*/
}

void CUIDragDropListEx::OnScrollV(CUIWindow* w, void* pData)
{
	m_container->SetWndPos		(m_container->GetWndPos().x, float(-m_vScrollBar->GetScrollPos()));
}

void CUIDragDropListEx::CreateDragItem(CUICellItem* itm)
{
	R_ASSERT							(!m_drag_item);
	m_drag_item							= itm->CreateDragItem();
	GetParent()->SetMouseCapture				(m_drag_item, true);

	Fvector2 p;
	itm->GetAbsolutePos(p);
	itm->OnMouse(p.x, p.y, EUIMessages::DRAG_DROP_ITEM_DRAG);
}

void CUIDragDropListEx::DestroyDragItem()
{
	if(m_selected_item && m_drag_item && m_drag_item->ParentItem()==m_selected_item)
	{
		VERIFY(GetParent()->GetMouseCapturer()==m_drag_item);
		GetParent()->SetMouseCapture( m_drag_item, false );

		delete_data							(m_drag_item);
	}
}

Fvector2 CUIDragDropListEx::GetDragItemPosition()
{
	return GetUICursor()->GetCursorPosition(); //m_drag_item->GetPosition();
}

void CUIDragDropListEx::OnItemStartDragging(CUIWindow* w, void* pData)
{
	// Избегаем случая двойного драга. Real Wolf. 12.11.14.
	if (m_drag_item)
		return;

	OnItemSelected						(w, pData);
	CUICellItem* itm		= smart_cast<CUICellItem*>(w);

	if(itm!=m_selected_item)	return;
	
	if(m_f_item_start_drag && m_f_item_start_drag(itm) ) return;

	CreateDragItem						(itm);
}

void CUIDragDropListEx::OnItemDrop(CUIWindow* w, void* pData)
{
	OnItemSelected						(w, pData);
	CUICellItem*		itm				= smart_cast<CUICellItem*>(w);
	VERIFY								(itm->OwnerList() == itm->OwnerList());

	if (itm)
	{
		Fvector2 p;
		itm->GetAbsolutePos(p);
		itm->OnMouse(p.x, p.y, EUIMessages::DRAG_DROP_ITEM_DROP);
	}

	if(m_f_item_drop && m_f_item_drop(itm) ){
		DestroyDragItem						();
		return;
	}

	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	CUIDragDropListEx*	new_owner		= m_drag_item->BackList();

	bool b				= (old_owner==new_owner)&&!GetCustomPlacement();

	if(old_owner&&new_owner && !b)
	{
		CUICellItem* i					= old_owner->RemoveItem(itm, (old_owner==new_owner) );
		while(i->ChildsCount())
		{
			CUICellItem* _chld				= i->PopChild();
			new_owner->SetItem				(_chld, old_owner->GetDragItemPosition());
		}
		new_owner->SetItem				(i,old_owner->GetDragItemPosition());
	}
	DestroyDragItem						();
}

void CUIDragDropListEx::OnItemDBClick(CUIWindow* w, void* pData)
{
	OnItemSelected						(w, pData);
	CUICellItem*		itm				= smart_cast<CUICellItem*>(w);

	if(m_f_item_db_click && m_f_item_db_click(itm) ){
		DestroyDragItem						();
		return;
	}

	CUIDragDropListEx*	old_owner		= itm->OwnerList();
	VERIFY								(m_drag_item==NULL);
	VERIFY								(old_owner == this);

	if(old_owner&&old_owner->GetCustomPlacement())
	{
		CUICellItem* i					= old_owner->RemoveItem(itm, true);
		old_owner->SetItem				(i);
	}

	DestroyDragItem						();
}

void CUIDragDropListEx::OnItemSelected(CUIWindow* w, void* pData)
{
	m_selected_item						= smart_cast<CUICellItem*>(w);
	VERIFY								(m_selected_item);
	if(m_f_item_selected)
		m_f_item_selected(m_selected_item);
}

void CUIDragDropListEx::OnItemRButtonClick(CUIWindow* w, void* pData)
{
	OnItemSelected						(w, pData);
	CUICellItem*		itm				= smart_cast<CUICellItem*>(w);
	if(m_f_item_rbutton_click) 
		m_f_item_rbutton_click(itm);
}

void CUIDragDropListEx::GetClientArea(Frect& r)
{
	GetAbsoluteRect				(r);
	if(m_vScrollBar->GetVisible() || m_flags.test(flAlwaysShowScroll))
		r.x2 -= m_vScrollBar->GetWidth	();
}

void CUIDragDropListEx::ClearAll(bool bDestroy)
{
	DestroyDragItem			();
	m_container->ClearAll	(bDestroy);
	m_selected_item			= NULL;
	m_container->SetWndPos	(0,0);
	ResetCellsCapacity		();
}

void CUIDragDropListEx::Compact()
{
	auto& wl = m_container->GetChildWndList();
	ClearAll						(false);

	CUIWindow::WINDOW_LIST_it it	= wl.begin();
	CUIWindow::WINDOW_LIST_it it_e	= wl.end();
	for(;it!=it_e;++it)
	{
		CUICellItem*	itm			= smart_cast<CUICellItem*>(*it);
		SetItem						(itm);
	}
}


#include "../HUDManager.h"
void CUIDragDropListEx::Draw()
{
	inherited::Draw				();

	if(0 && bDebug){
		CGameFont* F		= UI()->Font()->pFontDI;
		F->SetAligment		(CGameFont::alCenter);
		F->SetHeightI		(0.02f);
		F->OutSetI			(0.f,-0.5f);
		F->SetColor			(0xffffffff);
		Ivector2			pt = m_container->PickCell(GetUICursor()->GetCursorPosition());
		F->OutNext			("%d-%d",pt.x, pt.y);
	};

}

void CUIDragDropListEx::Update()
{
	inherited::Update			();

	if( m_drag_item ){
		Frect	wndRect;
		GetAbsoluteRect(wndRect);
		Fvector2 cp			= GetUICursor()->GetCursorPosition();
		if(wndRect.in(cp)){
			if(NULL==m_drag_item->BackList())
				m_drag_item->SetBackList(this);
		}else
			if( this==m_drag_item->BackList() )
				m_drag_item->SetBackList(NULL);
	}
}

void CUIDragDropListEx::ReinitScroll()
{
		float h1 = m_container->GetWndSize().y;
		float h2 = GetWndSize().y;
		VERIFY						(_valid(h1));
		VERIFY						(_valid(h2));
		float dh = h1-h2;
		m_vScrollBar->Show			( (dh > 0) || m_flags.test(flAlwaysShowScroll) );
		m_vScrollBar->Enable		( (dh > 0) || m_flags.test(flAlwaysShowScroll) );

		if ( dh < 0 )
		{
//			dh = 0;
			m_vScrollBar->SetRange	(0, 0);
		}
		else
		{
			m_vScrollBar->SetRange	(0, iFloor(dh));
		}
		m_vScrollBar->SetStepSize	(CellSize().y / 3);
		m_vScrollBar->SetPageSize	(1);
		
		m_vScrollBar->SetScrollPos(m_vScrollBar->GetScrollPos());

		m_container->SetWndPos(m_container->GetWndPos().x, float(-m_vScrollBar->GetScrollPos()));
}

#include "../xr_3da/xr_input.h"
#include "../Level.h"

bool CUIDragDropListEx::OnMouse(float x, float y, EUIMessages mouse_action)
{
	bool b = inherited::OnMouse		(x,y,mouse_action);

	if(m_vScrollBar->IsShown())
	{
		bool with_shift = Level().IR_GetKeyState(DIK_LSHIFT) || Level().IR_GetKeyState(DIK_RSHIFT);

		switch(mouse_action){
		case WINDOW_MOUSE_WHEEL_DOWN:
				m_vScrollBar->TryScrollInc();
				if (with_shift) 
				{
					m_vScrollBar->TryScrollInc();
					m_vScrollBar->TryScrollInc();
				}
				return true;
				break;

		case WINDOW_MOUSE_WHEEL_UP:
				m_vScrollBar->TryScrollDec();
				if (with_shift)
				{
					m_vScrollBar->TryScrollDec();
					m_vScrollBar->TryScrollDec();
				}
				return true;
				break;
		}
	}
	return b;
}

const Ivector2& CUIDragDropListEx::CellsCapacity()
{
	return m_container->CellsCapacity();
}

void CUIDragDropListEx::SetCellsCapacity(const Ivector2 c)
{
	m_container->SetCellsCapacity(c);
}

void CUIDragDropListEx::SetCellsAvailable(const u32 count) { m_container->SetCellsAvailable(count); }

void CUIDragDropListEx::enable_highlight(const bool enable) { m_container->enable_highlight(enable); }

const Ivector2& CUIDragDropListEx::CellSize()
{
	return m_container->CellSize();
}

const Ivector2& CUIDragDropListEx::CellsSpacing()
{
	return m_container->CellsSpacing();
}

void CUIDragDropListEx::SetCellSize(const Ivector2 new_sz)			
{
	m_container->SetCellSize(new_sz);
}

void CUIDragDropListEx::SetCellsSpacing(const Ivector2& new_sz)
{
	m_container->SetCellsSpacing(new_sz);
}

int CUIDragDropListEx::ScrollPos()
{
	return m_vScrollBar->GetScrollPos();
}

void CUIDragDropListEx::SetScrollPos(int _pos)
{
	m_vScrollBar->SetScrollPos(_pos);
	m_vScrollBar->Refresh();
}

void CUIDragDropListEx::SetItem(CUICellItem* itm) //auto
{
	if(m_container->AddSimilar(itm)){
		return;
	}

	Ivector2 dest_cell_pos =	m_container->FindFreeCell(itm->GetGridSize());

	SetItem						(itm,dest_cell_pos);
}

void CUIDragDropListEx::SetItem(CUICellItem* itm, Fvector2 abs_pos) // start at cursor pos
{
	if(m_container->AddSimilar(itm))	return;

	const Ivector2 dest_cell_pos =	m_container->PickCell		(abs_pos);

	if(m_container->ValidCell(dest_cell_pos) && m_container->IsRoomFree(dest_cell_pos,itm->GetGridSize()))
		SetItem						(itm, dest_cell_pos);
	else
		SetItem						(itm);
}

void CUIDragDropListEx::SetItem(CUICellItem* itm, Ivector2 cell_pos) // start at cell
{
	if(m_container->AddSimilar(itm))	return;

	if (!(m_container->IsRoomFree(cell_pos, itm->GetGridSize())))
		Msg("!![CUIDragDropListEx::SetItem] !(m_container->IsRoomFree(cell_pos, itm->GetGridSize()))");
	//R_ASSERT						(m_container->IsRoomFree(cell_pos, itm->GetGridSize()));

	m_container->PlaceItemAtPos	(itm, cell_pos);

	itm->SetWindowName			("cell_item");
	Register					(itm);
	itm->SetOwnerList			(this);
}

bool CUIDragDropListEx::CanSetItem(CUICellItem* itm){
	if (m_container->HasFreeSpace(itm->GetGridSize()))
		return true;
	Compact();

	return m_container->HasFreeSpace(itm->GetGridSize());
}

CUICellItem* CUIDragDropListEx::RemoveItem(CUICellItem* itm, bool force_root)
{
	CUICellItem* i				= m_container->RemoveItem		(itm, force_root);
	i->SetOwnerList				((CUIDragDropListEx*)NULL);
	return						i;
}

u32 CUIDragDropListEx::ItemsCount()
{
	return m_container->GetChildWndList().size();
}

bool CUIDragDropListEx::IsOwner(CUICellItem* itm){
	return m_container->IsChild(itm);
}

CUICellItem* CUIDragDropListEx::GetItemIdx(u32 idx)
{
	R_ASSERT(idx<ItemsCount());
	WINDOW_LIST_it it = m_container->GetChildWndList().begin();
	std::advance	(it, idx);
	return smart_cast<CUICellItem*>(*it);
}

void CUIDragDropListEx::clear_select_armament()
{
	m_container->clear_select_armament();
}

void CUIDragDropListEx::SetCellsVertAlignment(xr_string alignment)
{
	if(strchr(alignment.c_str(), 't'))
	{
		m_virtual_cells_alignment.y = 0;
		return;
	}
	if(strchr(alignment.c_str(), 'b'))
	{
		m_virtual_cells_alignment.y = 2;
		return;
	}
	m_virtual_cells_alignment.y = 1;
}
void CUIDragDropListEx::SetCellsHorizAlignment(xr_string alignment)
{
	if(strchr(alignment.c_str(), 'l'))
	{
		m_virtual_cells_alignment.x = 0;
		return;
	}
	if(strchr(alignment.c_str(), 'r'))
	{
		m_virtual_cells_alignment.x = 2;
		return;
	}
	m_virtual_cells_alignment.x = 1;
}

Ivector2 CUIDragDropListEx::PickCell(const Fvector2& abs_pos) 
{
	return m_container->PickCell(abs_pos);
};

CUICell& CUIDragDropListEx::GetCellAt(const Ivector2& pos) 
{
	return m_container->GetCellAt(pos);
};

CUICellContainer::CUICellContainer(CUIDragDropListEx* parent)
{
	m_pParentDragDropList		= parent;
	hShader->create("hud\\fog_of_war", "ui\\ui_grid");
	m_cellSpacing.set			( 0, 0 );
}

CUICellContainer::~CUICellContainer()
{
}

bool CUICellContainer::AddSimilar(CUICellItem* itm)
{
	if(!m_pParentDragDropList->IsGrouping())	return false;

	CUICellItem* i		= FindSimilar(itm);
	R_ASSERT			(i!=itm);
	R_ASSERT			(0==itm->ChildsCount());
	if(i){	
		i->PushChild			(itm);
		itm->SetOwnerList		(m_pParentDragDropList);
	}
	
	return (i!=NULL);
}

CUICellItem* CUICellContainer::FindSimilar(CUICellItem* itm)
{
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)
	{
#ifdef DEBUG
		CUICellItem* i = smart_cast<CUICellItem*>(*it);
#else
		CUICellItem* i = (CUICellItem*)(*it);
#endif
		R_ASSERT		(i!=itm);
		if(i->EqualTo(itm))
			return i;
	}
	return NULL;
}

void CUICellContainer::PlaceItemAtPos(CUICellItem* itm, Ivector2& cell_pos)
{
	Ivector2 cs				= itm->GetGridSize();
	Ivector2 cp				= cell_pos;
	if (m_pParentDragDropList->GetVerticalPlacement())
	{
		std::swap(cs.x, cs.y);
		if (cp.y == 0)
			cp.y = m_pParentDragDropList->CellsCapacity().y-cs.y;
	}
	for(int x=0; x<cs.x; ++x)
		for(int y=0; y<cs.y; ++y){
			CUICell& C		= GetCellAt(Ivector2().set(x,y).add(cp));
			C.SetItem		(itm,(x==0&&y==0));
		}

	itm->SetWndSize			( Fvector2().set( (m_cellSize.x*cs.x),		(m_cellSize.y*cs.y)		 )	);
	if(!m_pParentDragDropList->GetVirtualCells())
		itm->SetWndPos			( Fvector2().set( ((CellsSpacing().x+m_cellSize.x)*cp.x), ((CellsSpacing().y+m_cellSize.y)*cp.y))	);
	else
	{
		Ivector2 alignment_vec	= m_pParentDragDropList->GetVirtualCellsAlignment();
		Fvector2 pos = Fvector2().set(0,0);
		if(alignment_vec.x == 1)
			pos.x = (GetWndSize().x-cs.x*(CellsSpacing().x+m_cellSize.x))/2;
		else if(alignment_vec.x == 2)
			pos.x = GetWndSize().x-cs.x*(CellsSpacing().x+m_cellSize.x);
		
		if(alignment_vec.y == 1)
			pos.y = (GetWndSize().y-cs.y*(CellsSpacing().y+m_cellSize.y))/2;
		else if(alignment_vec.y == 2)
			pos.y = GetWndSize().y-cs.y*(CellsSpacing().y+m_cellSize.y);
		itm->SetWndPos(pos);
	}

	AttachChild				(itm);
	itm->OnAfterChild		(m_pParentDragDropList);
}

CUICellItem* CUICellContainer::RemoveItem(CUICellItem* itm, bool force_root)
{
	for(WINDOW_LIST_it it = m_ChildWndList.begin(); m_ChildWndList.end()!=it; ++it)
	{
		CUICellItem* i		= (CUICellItem*)(*it);
		
		if(i->HasChild(itm))
		{
			CUICellItem* iii	= i->PopChild();
			R_ASSERT			(0==iii->ChildsCount());
			return				iii;
		}
	}

	if(!force_root && itm->ChildsCount())
	{
		CUICellItem* iii	=	itm->PopChild();
		R_ASSERT			(0==iii->ChildsCount());
		return				iii;
	}

	Ivector2 pos			= GetItemPos(itm);
	Ivector2 cs				= itm->GetGridSize();

	if (m_pParentDragDropList->GetVerticalPlacement())
		std::swap(cs.x, cs.y);

	for(int x=0; x<cs.x;++x)
		for(int y=0; y<cs.y;++y)
		{
			CUICell& C		= GetCellAt(Ivector2().set(x,y).add(pos));
			C.Clear			();
		}

	itm->SetOwnerList		(NULL);
	DetachChild				(itm);
	return					itm;
}

Ivector2 CUICellContainer::FindFreeCell	(const Ivector2& _size)
{
	Ivector2 tmp;
	Ivector2 size = _size;
	if (m_pParentDragDropList->GetVerticalPlacement())
		std::swap(size.x, size.y);

	for(tmp.y=0; tmp.y<=m_cellsCapacity.y-size.y; ++tmp.y )
		for(tmp.x=0; tmp.x<=m_cellsCapacity.x-size.x; ++tmp.x )
			if(IsRoomFree(tmp,_size))
				return  tmp;

	if(m_pParentDragDropList->IsAutoGrow())
	{
		Grow	();
		return							FindFreeCell	(size);
	}else{
		m_pParentDragDropList->Compact		();
		for(tmp.y=0; tmp.y<=m_cellsCapacity.y-size.y; ++tmp.y )
			for(tmp.x=0; tmp.x<=m_cellsCapacity.x-size.x; ++tmp.x )
				if(IsRoomFree(tmp,_size))
					return  tmp;

		Msg("!![CUICellContainer::FindFreeCell] There are no free room to place item");
	}
	return			tmp;
}

bool CUICellContainer::HasFreeSpace		(const Ivector2& _size){
	Ivector2 tmp;
	Ivector2 size = _size;

	if (m_pParentDragDropList->GetVerticalPlacement())
		std::swap(size.x, size.y);

	for(tmp.y=0; tmp.y<=m_cellsCapacity.y-size.y; ++tmp.y )
		for(tmp.x=0; tmp.x<=m_cellsCapacity.x-size.x; ++tmp.x )
			if(IsRoomFree(tmp,_size))
				return true;

	return false;
}

bool CUICellContainer::IsRoomFree(const Ivector2& pos, const Ivector2& _size)
{
	Ivector2 tmp;
	Ivector2 size = _size;
	if (m_pParentDragDropList->GetVerticalPlacement())
		std::swap(size.x, size.y);
	for(tmp.x =pos.x; tmp.x<pos.x+size.x; ++tmp.x)
		for(tmp.y =pos.y; tmp.y<pos.y+size.y; ++tmp.y)
		{
			if(!ValidCell(tmp))		return		false;

			CUICell& C				= GetCellAt(tmp);

			if (!C.Empty() || C.cell_disabled)
				return false;
		}
	return true;
}

void CUICellContainer::SetCellsCapacity(const Ivector2& c)
{
	m_cellsCapacity				= c;
	m_cells.resize				(c.x*c.y);
	ReinitSize					();
}

void CUICellContainer::SetCellsAvailable(const u32 count) {
	u32 i{};
	for (auto& cell : m_cells)
		cell.cell_disabled = (++i > count);
}

void CUICellContainer::enable_highlight(const bool enable) {
	for (auto& cell : m_cells)
		cell.is_highlighted = enable;
}

void CUICellContainer::SetCellSize(const Ivector2& new_sz)
{
	m_cellSize					= new_sz;
	ReinitSize					();
}

void CUICellContainer::SetCellsSpacing(const Ivector2& c)
{
	m_cellSpacing				= c;
	ReinitSize					();
}

Ivector2 CUICellContainer::TopVisibleCell()
{
	return Ivector2().set	(0, iFloor(m_pParentDragDropList->ScrollPos()/float(CellSize().y+CellsSpacing().y)));
}

CUICell& CUICellContainer::GetCellAt(const Ivector2& pos)
{
	if (!ValidCell(pos))
		Msg("!![%s] invalid cell position: [%d , %d]", __FUNCTION__, pos.x, pos.y);
	//R_ASSERT(ValidCell(pos));

	return m_cells.at(m_cellsCapacity.x * pos.y + pos.x);
}

Ivector2 CUICellContainer::GetItemPos(CUICellItem* itm)
{
	for(int x=0; x<m_cellsCapacity.x ;++x)
		for(int y=0; y<m_cellsCapacity.y ;++y){
			Ivector2 p;
			p.set(x,y);
		if(GetCellAt(p).m_item==itm)
			return p;
		}

		R_ASSERT(0);
		return Ivector2().set(-1,-1);
}

u32 CUICellContainer::GetCellsInRange(const Irect& rect, UI_CELLS_VEC& res)
{
	res.clear_not_free();
	for (int x = rect.x1; x <= rect.x2; ++x)
		for (int y = rect.y1; y <= rect.y2; ++y)
			res.push_back(GetCellAt(Ivector2().set(x, y)));

	res.erase(std::unique(res.begin(), res.end()), res.end());
	return res.size();
}

void CUICellContainer::ReinitSize()
{
	Ivector2							sz;
	sz.add								(CellsSpacing(), CellSize());
	sz.mul								(CellsCapacity());
	//sz.sub								(CellsSpacing());
	SetWndSize							(Fvector2().set(sz.x,sz.y));
	m_pParentDragDropList->ReinitScroll	();
}

void CUICellContainer::Grow()
{
	SetCellsCapacity	(Ivector2().set(m_cellsCapacity.x,m_cellsCapacity.y+1));
}

void CUICellContainer::Shrink()
{
}

bool CUICellContainer::ValidCell(const Ivector2& pos) const
{
	return !(pos.x<0 || pos.y<0 || pos.x>=m_cellsCapacity.x || pos.y>=m_cellsCapacity.y);
}

void CUICellContainer::ClearAll(bool bDestroy)
{
	{
		UI_CELLS_VEC_IT it		= m_cells.begin();
		UI_CELLS_VEC_IT it_e	= m_cells.end();
		for(;it!=it_e;++it)
			(*it).Clear();
	}
	while( !m_ChildWndList.empty() )
	{
		CUIWindow* w			= m_ChildWndList.back();
		CUICellItem* wc			= smart_cast<CUICellItem*>(w);
		VERIFY					(!wc->IsAutoDelete());
		DetachChild				(wc);	
		
		while( wc->ChildsCount() )
		{
			CUICellItem* ci		= wc->PopChild();
			R_ASSERT			(ci->ChildsCount()==0);

			if(bDestroy)
				delete_data		(ci);
		}
		
		if(bDestroy){
			delete_data			(wc);
		}
	}

}

Ivector2 CUICellContainer::PickCell(const Fvector2& abs_pos)
{
	Ivector2 res;
	Fvector2 ap;
	GetAbsolutePos							(ap);
	ap.sub									(abs_pos);
	ap.mul									(-1);
	res.x			= iFloor(ap.x/(m_cellSize.x+CellsSpacing().x*(m_cellsCapacity.x-1)/m_cellsCapacity.x));
	res.y			= iFloor(ap.y/(m_cellSize.y+CellsSpacing().y*(m_cellsCapacity.y-1)/m_cellsCapacity.y));
	if(!ValidCell(res))						res.set(-1, -1);
	return res;
}


CUICellContainer::CellTextureType CUICellContainer::get_select_mode( const int x, const int y ) {
  Ivector2 cpos{ x, y };
  cpos.add( TopVisibleCell() );
  CUICell& ui_cell = GetCellAt( cpos );

  CellTextureType select_mode = CellTextureTypeNormal;
  if ( !ui_cell.Empty() ) {
    if ( Core.Features.test( xrCore::Feature::select_mode_1342 ) ) {
      if ( ui_cell.m_item->m_selected )
        select_mode = CellTextureTypeCursorHover;
      else if ( ui_cell.m_item->m_select_armament )
        select_mode = CellTextureTypeArmanent;
      else if ( ui_cell.m_item->m_select_untradable )
        select_mode = CellTextureTypeUntradable;
      else if ( ui_cell.m_item->m_select_equipped )
        select_mode = CellTextureTypeEquipped;
    }
    else {
      if ( ui_cell.m_item->m_select_armament )
        select_mode = CellTextureTypeArmanent;
      else if ( ui_cell.m_item->m_select_untradable )
        select_mode = CellTextureTypeUntradable;
      else if ( ui_cell.m_item->m_select_equipped )
        select_mode = CellTextureTypeEquipped;
      else if ( ui_cell.m_item->m_selected )
        select_mode = CellTextureTypeCursorHover;
    }
  }

  if (ui_cell.is_highlighted)
    select_mode = CellTextureTypeAvailableSlots;

  if (ui_cell.cell_disabled)
    select_mode = CellTextureTypeBlockedSlots;

  return select_mode;
}


void CUICellContainer::Draw()
{
	Frect clientArea;
	m_pParentDragDropList->GetClientArea(clientArea);

	Ivector2			cell_cnt = m_pParentDragDropList->CellsCapacity();
	if					(cell_cnt.x==0 || cell_cnt.y==0)	return;

	Ivector2			cell_sz = CellSize();
	cell_sz.add			(CellsSpacing());

	Irect				tgt_cells;
	tgt_cells.lt		= TopVisibleCell();
	tgt_cells.x2		= iFloor( (float(clientArea.width())+float(cell_sz.x)-EPS)/float(cell_sz.x)) + tgt_cells.lt.x;
	tgt_cells.y2		= iFloor( (float(clientArea.height())+float(cell_sz.y)-EPS)/float(cell_sz.y)) + tgt_cells.lt.y;

	clamp				(tgt_cells.x2, 0, cell_cnt.x-1);
	clamp				(tgt_cells.y2, 0, cell_cnt.y-1);

	Fvector2			lt_abs_pos;
	GetAbsolutePos		(lt_abs_pos);

	Fvector2					drawLT;
	drawLT.set( lt_abs_pos.x + tgt_cells.lt.x * cell_sz.x, lt_abs_pos.y + tgt_cells.lt.y * cell_sz.y );
	UI()->ClientToScreenScaled	(drawLT, drawLT.x, drawLT.y);

	constexpr Fvector2 pts[]{
		{ 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f, 1.0f },
		{ 0.0f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 1.0f }
	};
	constexpr float tx{ 0.125f }, ty{ 1.0f };
	constexpr Fvector2 uvs[]{
		{ 0.0f, 0.0f }, { tx, 0.0f }, { tx,   ty },
		{ 0.0f, 0.0f }, { tx, ty   }, { 0.0f, ty }
	};
	constexpr Fvector2 tps[]{
		{ tx * 0, 0.0f }, { tx * 1, 0.0f }, { tx * 2, 0.0f },
		{ tx * 3, 0.0f }, { tx * 4, 0.0f }, { tx * 5, 0.0f },
		{ tx * 6, 0.0f }, { tx * 7, 0.0f }
	};

	// calculate cell size in screen pixels
	Fvector2 f_len, sp_len;
	UI()->ClientToScreenScaled(f_len, float(CellSize().x), float(CellSize().y) );
	UI()->ClientToScreenScaled(sp_len, float(CellsSpacing().x), float(CellsSpacing().y) );

	// fill cell buffer
	u32 max_prim_cnt = ((tgt_cells.width() + 1)*(tgt_cells.height() + 1) * 6);
	UIRender->StartPrimitive(max_prim_cnt, IUIRender::ptTriList, UI()->m_currentPointType);

	Fvector2 hl_len = f_len;
	if ( m_pParentDragDropList->GetHighlightCellSp() )
	  hl_len.add( sp_len );

	CellTextureType parent_select_mode = CellTextureTypeNormal;
	if (m_pParentDragDropList->GetHighlightAllCells() && m_pParentDragDropList->ItemsCount()) {
		for (int x = 0; x <= tgt_cells.width(); ++x) {
			for (int y = 0; y <= tgt_cells.height(); ++y) {
				parent_select_mode = get_select_mode(x, y);
				if (parent_select_mode != CellTextureTypeNormal) break;
			}
			if (parent_select_mode != CellTextureTypeNormal) break;
		}
	}

	for (int x=0; x<=tgt_cells.width(); ++x){
		for (int y=0; y<=tgt_cells.height(); ++y){
			const Fvector2 rect_offset{ (drawLT.x + f_len.x * x + sp_len.x * x), (drawLT.y + f_len.y * y + sp_len.y * y) };
			const CellTextureType select_mode = parent_select_mode == CellTextureTypeNormal ? get_select_mode(x, y) : parent_select_mode;
			const Fvector2& tp = tps[select_mode]; //GetTexUVLT

			for (u32 k=0; k<6; ++k){
				const Fvector2& p	= pts[k];
				const Fvector2& uv	= uvs[k];
				UIRender->PushPoint(iFloor( rect_offset.x + p.x*hl_len.x )-0.5f, 
					iFloor( rect_offset.y + p.y*hl_len.y )-0.5f,
					0,
					m_pParentDragDropList->back_color,
					tp.x+uv.x, tp.y+uv.y);
			}
		}
	}

	UI()->PushScissor(clientArea);

	UIRender->SetShader(*hShader);
	UIRender->FlushPrimitive();

	//draw shown items in range
	if( GetCellsInRange(tgt_cells,m_cells_to_draw) ){
		UI_CELLS_VEC_IT it = m_cells_to_draw.begin();
		for(;it!=m_cells_to_draw.end();++it)
			if( !(*it).Empty() && !(*it).m_item->m_b_already_drawn ){
				(*it).m_item->Draw				();
			}
	}

	UI()->PopScissor			();
}

void CUICellContainer::clear_select_armament()
{
	auto TopWnd = this->GetTop();
	auto InvWnd = smart_cast<CUIInventoryWnd*>(TopWnd);
	auto CarBodyWnd = smart_cast<CUICarBodyWnd*>(TopWnd);
	CUITradeWnd* TradeWnd = nullptr;

	if (!InvWnd && !CarBodyWnd) { //Окно торговли приаттачено к CUITalkWnd, поэтому придётся извращаться с его поиском
		auto Parent = this->GetParent();
		do {
			TradeWnd = smart_cast<CUITradeWnd*>(Parent);
			Parent = Parent->GetParent();
		} while (!TradeWnd);
	}

	for (auto& cell : m_cells) {
		if (cell.m_item) {
			if (InvWnd && InvWnd->IsShown()) {
				if (cell.m_item != InvWnd->m_pCurrentCellItem)
					cell.m_item->m_select_armament = false;
			}
			else if (TradeWnd && TradeWnd->IsShown()) {
				if (cell.m_item != TradeWnd->m_pCurrentCellItem)
					cell.m_item->m_select_armament = false;
			}
			else if (CarBodyWnd && CarBodyWnd->IsShown()) {
				if (cell.m_item != CarBodyWnd->m_pCurrentCellItem)
					cell.m_item->m_select_armament = false;
			}
		}
	}
}
