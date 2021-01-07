#include "stdafx.h"
#include "UIInventoryWnd.h"
#include "../level.h"
#include "../actor.h"
#include "../ActorCondition.h"
#include "../hudmanager.h"
#include "../inventory.h"
#include "UIInventoryUtilities.h"

#include "UICellItem.h"
#include "UICellItemFactory.h"
#include "UIDragDropListEx.h"
#include "UI3tButton.h"

#include "../game_object_space.h"
#include "../script_callback_ex.h"
#include "../script_game_object.h"

CUICellItem* CUIInventoryWnd::CurrentItem()
{
	return m_pCurrentCellItem;
}

PIItem CUIInventoryWnd::CurrentIItem()
{
	return	(m_pCurrentCellItem)?(PIItem)m_pCurrentCellItem->m_pData : NULL;
}

void CUIInventoryWnd::SetCurrentItem(CUICellItem* itm)
{
	if(m_pCurrentCellItem == itm) return;
	m_pCurrentCellItem				= itm;
	UIItemInfo.InitItem			(CurrentIItem());

	if (m_pCurrentCellItem) {
		m_pCurrentCellItem->m_select_armament = true;
		auto script_obj = CurrentIItem()->object().lua_game_object();
		g_actor->callback(GameObject::eCellItemSelect)(script_obj);
	}
}

void CUIInventoryWnd::SendMessage(CUIWindow *pWnd, s16 msg, void *pData)
{
	if(pWnd == &UIPropertiesBox &&	msg==PROPERTY_CLICKED)
	{
		ProcessPropertiesBoxClicked	();
	}else 
	if (UIExitButton == pWnd && BUTTON_CLICKED == msg)
	{
		GetHolder()->StartStopMenu			(this,true);
	}

	CUIWindow::SendMessage(pWnd, msg, pData);
}


void CUIInventoryWnd::InitInventory_delayed()
{
	m_b_need_reinit = true;
	m_b_need_update_stats = true;
}

void CUIInventoryWnd::InitInventory() 
{
	CInventoryOwner *pInvOwner	= smart_cast<CInventoryOwner*>(Level().CurrentEntity());
	if(!pInvOwner)				return;

	m_pInv						= &pInvOwner->inventory();

	int bag_scroll = m_pUIBagList->ScrollPos();

	UIPropertiesBox.Hide		();
	ClearAllLists				();
	CUIWindow::Reset();
	SetCurrentItem				(NULL);

	//Slots
	PIItem _itm							= m_pInv->m_slots[FIRST_WEAPON_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIPistolList->SetItem		(itm);
	}

	if (Core.Features.test(xrCore::Feature::ogse_new_slots)) {
	_itm = m_pInv->m_slots[KNIFE_SLOT].m_pIItem;
	if (_itm)
	{
		CUICellItem* itm = create_cell_item(_itm);
		m_pUIKnifeList->SetItem(itm);
	}
	}

	_itm								= m_pInv->m_slots[SECOND_WEAPON_SLOT].m_pIItem;
	if(_itm)
	{
		CUICellItem* itm				= create_cell_item(_itm);
		m_pUIAutomaticList->SetItem		(itm);
	}
	_itm = m_pInv->m_slots[APPARATUS_SLOT].m_pIItem;

	if (Core.Features.test(xrCore::Feature::ogse_new_slots)) {

	if (_itm)
	{
		CUICellItem* itm = create_cell_item(_itm);
		m_pUIBinocularList->SetItem(itm);
	}

	_itm = m_pInv->m_slots[DETECTOR_SLOT].m_pIItem;
	if (_itm)
	{
		CUICellItem* itm = create_cell_item(_itm);
		m_pUIDetectorList->SetItem(itm);
	}

	_itm = m_pInv->m_slots[TORCH_SLOT].m_pIItem;
	if (_itm)
	{
		CUICellItem* itm = create_cell_item(_itm);
		m_pUITorchList->SetItem(itm);
	}

	_itm = m_pInv->m_slots[HELMET_SLOT].m_pIItem;
	if (_itm)
	{
		CUICellItem* itm = create_cell_item(_itm);
		m_pUIHelmetList->SetItem(itm);
	}

	_itm = m_pInv->m_slots[NIGHT_VISION_SLOT].m_pIItem;
	if (_itm)
	{
		CUICellItem* itm = create_cell_item(_itm);
		m_pUINightVisionList->SetItem(itm);
	}

	_itm = m_pInv->m_slots[BIODETECTOR_SLOT].m_pIItem;
	if (_itm)
	{
		CUICellItem* itm = create_cell_item(_itm);
		m_pUIBIODetList->SetItem(itm);
	}

	}

	PIItem _outfit						= m_pInv->m_slots[OUTFIT_SLOT].m_pIItem;
	CUICellItem* outfit					= (_outfit)?create_cell_item(_outfit):NULL;
	m_pUIOutfitList->SetItem			(outfit);

	TIItemContainer::iterator it, it_e;
	for(it=m_pInv->m_belt.begin(),it_e=m_pInv->m_belt.end(); it!=it_e; ++it) 
	{
		CUICellItem* itm			= create_cell_item(*it);
		m_pUIBeltList->SetItem		(itm);
	}

	ruck_list		= m_pInv->m_ruck;
	std::sort		(ruck_list.begin(),ruck_list.end(),InventoryUtilities::GreaterRoomInRuck);

	int i=1;
	for(it=ruck_list.begin(),it_e=ruck_list.end(); it!=it_e; ++it,++i) 
	{
	  if ( !(*it)->m_flags.test( CInventoryItem::FIHiddenForInventory ) ) {
	    CUICellItem* itm = create_cell_item( *it );
	    m_pUIBagList->SetItem( itm );
	  }
	}
	//fake
	_itm								= m_pInv->m_slots[GRENADE_SLOT].m_pIItem;
	if(_itm)
	{
	  if ( !_itm->m_flags.test( CInventoryItem::FIHiddenForInventory ) ) {
	    CUICellItem* itm = create_cell_item( _itm );
	    m_pUIBagList->SetItem( itm );
	  }
	}

	m_pUIBagList->SetScrollPos(bag_scroll);

	UpdateWeight();

	m_b_need_reinit					= false;
}  

void CUIInventoryWnd::DropCurrentItem(bool b_all)
{

	CActor *pActor			= smart_cast<CActor*>(Level().CurrentEntity());
	if(!pActor)				return;

	if(!b_all && CurrentIItem() && !CurrentIItem()->IsQuestItem())
	{
		SendEvent_Item_Drop		(CurrentIItem());
		SetCurrentItem			(NULL);
		UpdateWeight();
		return;
	}

	if(b_all && CurrentIItem() && !CurrentIItem()->IsQuestItem())
	{
		u32 cnt = CurrentItem()->ChildsCount();

		for(u32 i=0; i<cnt; ++i){
			CUICellItem*	itm				= CurrentItem()->PopChild();
			PIItem			iitm			= (PIItem)itm->m_pData;
			SendEvent_Item_Drop				(iitm);
		}

		SendEvent_Item_Drop					(CurrentIItem());
		SetCurrentItem						(NULL);
		UpdateWeight();
		return;
	}
}

//------------------------------------------
bool CUIInventoryWnd::ToSlot(CUICellItem* itm, u8 _slot_id, bool force_place)
{
	PIItem	iitem = (PIItem)itm->m_pData;
	//Msg("~~[CUIInventoryWnd::ToSlot] Name [%s], slot [%u]", iitem->object().cName().c_str(), _slot_id);
	//LogStackTrace("ST:\n");
	iitem->SetSlot(_slot_id);
	return ToSlot(itm, force_place);
}

bool CUIInventoryWnd::ToSlot(CUICellItem* itm, bool force_place)
{
	CUIDragDropListEx*	old_owner			= itm->OwnerList();
	PIItem	iitem							= (PIItem)itm->m_pData;
	u8 _slot								= iitem->GetSlot();

	if(GetInventory()->CanPutInSlot(iitem)){
		CUIDragDropListEx* new_owner		= GetSlotList(_slot);
		
		if(_slot==GRENADE_SLOT && !new_owner )return true; //fake, sorry (((

#ifdef DEBUG
		bool result =
#endif
		GetInventory()->Slot(iitem);
		VERIFY								(result);

		CUICellItem* i						= old_owner->RemoveItem(itm, (old_owner==new_owner) );
		
		new_owner->SetItem					(i);
	
		SendEvent_Item2Slot					(iitem);

		SendEvent_ActivateSlot				(iitem);
		
		/************************************************** added by Ray Twitty (aka Shadows) START **************************************************/
		// обновляем статик веса в инвентаре
		UpdateWeight();
		/*************************************************** added by Ray Twitty (aka Shadows) END ***************************************************/
		
		return								true;
	}else
	{ // in case slot is busy
		if(!force_place ||  _slot==NO_ACTIVE_SLOT || GetInventory()->m_slots[_slot].m_bPersistent) return false;

		CUIDragDropListEx* slot_list		= GetSlotList(_slot);
		VERIFY								(slot_list->ItemsCount()==1);

		CUICellItem* slot_cell				= slot_list->GetItemIdx(0);
		VERIFY(slot_cell && ((PIItem)slot_cell->m_pData) == GetInventory()->m_slots[_slot].m_pIItem);

		dont_update_belt_flag = _slot == OUTFIT_SLOT;

#ifdef DEBUG
		bool result =
#endif
		ToBag(slot_cell, false);
		VERIFY								(result);

		return ToSlot						(itm, false);
	}
}

bool CUIInventoryWnd::ToBag(CUICellItem* itm, bool b_use_cursor_pos)
{
	PIItem	iitem						= (PIItem)itm->m_pData;

	if(GetInventory()->CanPutInRuck(iitem))
	{
		CUIDragDropListEx*	old_owner		= itm->OwnerList();
		CUIDragDropListEx*	new_owner		= NULL;
		if(b_use_cursor_pos){
				new_owner					= CUIDragDropListEx::m_drag_item->BackList();
				VERIFY						(new_owner==m_pUIBagList);
		}else
				new_owner					= m_pUIBagList;

#ifdef DEBUG
		bool result =
#endif
		GetInventory()->Ruck(iitem);
		VERIFY								(result);
		CUICellItem* i						= old_owner->RemoveItem(itm, (old_owner==new_owner) );

		/************************************************** added by Ray Twitty (aka Shadows) START **************************************************/
		// обновляем статик веса в инвентаре
		UpdateWeight();
		/*************************************************** added by Ray Twitty (aka Shadows) END ***************************************************/
		
		if(b_use_cursor_pos)
			new_owner->SetItem				(i,old_owner->GetDragItemPosition());
		else
			new_owner->SetItem				(i);

		SendEvent_Item2Ruck					(iitem);
		return true;
	}
	return false;
}

bool CUIInventoryWnd::ToBelt(CUICellItem* itm, bool b_use_cursor_pos)
{
	PIItem	iitem						= (PIItem)itm->m_pData;

	if(GetInventory()->CanPutInBelt(iitem))
	{
		CUIDragDropListEx*	old_owner		= itm->OwnerList();
		CUIDragDropListEx*	new_owner		= NULL;
		if(b_use_cursor_pos){
				new_owner					= CUIDragDropListEx::m_drag_item->BackList();
				VERIFY						(new_owner==m_pUIBeltList);
		}else
				new_owner					= m_pUIBeltList;
#ifdef DEBUG
		bool result =
#endif
		GetInventory()->Belt(iitem);
		VERIFY								(result);
		CUICellItem* i						= old_owner->RemoveItem(itm, (old_owner==new_owner) );
		
	//.	UIBeltList.RearrangeItems();
		if(b_use_cursor_pos)
			new_owner->SetItem				(i,old_owner->GetDragItemPosition());
		else
			new_owner->SetItem				(i);

		SendEvent_Item2Belt					(iitem);

		/************************************************** added by Ray Twitty (aka Shadows) START **************************************************/
		// обновляем статик веса в инвентаре
		UpdateWeight();
		/*************************************************** added by Ray Twitty (aka Shadows) END ***************************************************/

		return								true;
	}
	return									false;
}

void CUIInventoryWnd::AddItemToBag(PIItem pItem)
{
	CUICellItem* itm						= create_cell_item(pItem);
	m_pUIBagList->SetItem					(itm);
}

bool CUIInventoryWnd::OnItemStartDrag(CUICellItem* itm)
{
	return false; //default behaviour
}

bool CUIInventoryWnd::OnItemSelected(CUICellItem* itm)
{
	SetCurrentItem(itm);

	itm->ColorizeItems( { m_pUIBagList, m_pUIBeltList, m_pUIPistolList, m_pUIAutomaticList, m_pUIKnifeList, m_pUIHelmetList, m_pUIBIODetList, m_pUINightVisionList, m_pUIDetectorList, m_pUITorchList, m_pUIBinocularList, m_pUIOutfitList } );
	return false;
}

bool CUIInventoryWnd::OnItemDrop(CUICellItem* itm)
{
	auto old_owner = itm->OwnerList();
	auto new_owner = CUIDragDropListEx::m_drag_item->BackList();
	if (old_owner == new_owner || !old_owner || !new_owner)
		return false;

	auto t_new = GetType(new_owner);
	auto t_old = GetType(old_owner);

	if (t_new == t_old && t_new != iwSlot)
		return true;

	switch (t_new) {
	case iwSlot:
	{
          auto item = CurrentIItem();

          bool     can_put  = false;
          Ivector2 max_size = new_owner->CellSize();

          LPCSTR name = item->object().Name_script();
          int item_w  = item->GetGridWidth();
          int item_h  = item->GetGridHeight();

          if ( new_owner->GetVerticalPlacement() )
            std::swap( max_size.x, max_size.y );

          if ( item_w <= max_size.x && item_h <= max_size.y ) {
            for ( u8 i = 0; i < SLOTS_TOTAL; i++ )
              if ( new_owner == GetSlotList( i ) ) {
                if ( item->IsPlaceable( i, i ) ) {
                  item->SetSlot( i );
                  can_put = true;
                }
                else {
                  if ( !DropItem( CurrentIItem(), new_owner ) && item->GetSlotsCount() > 0 )
                    Msg( "! cannot put item %s into slot %d, allowed slots {%s}", name, i, item->GetSlotsSect() );
                }
                break;
              }   // for-if
          }
          else
            Msg( "!#ERROR: item %s to large for slot: (%d x %d) vs (%d x %d) ", name, item_w, item_h, max_size.x, max_size.y );

          // при невозможности поместить в выбранный слот
          if ( !can_put ) {
            // восстановление не требуется, слот не был назначен
            return true;
          }

          if( GetSlotList( item->GetSlot() ) == new_owner )
            ToSlot( itm, true );
	}break;
	case iwBag: {
		ToBag(itm, true);
	}break;
	case iwBelt: {
		ToBelt(itm, true);
	}break;
	};

	DropItem(CurrentIItem(), new_owner);

	return true;
}

bool CUIInventoryWnd::OnItemDbClick(CUICellItem* itm)
{
	auto __item = (PIItem)itm->m_pData;
	auto old_owner = itm->OwnerList();

	if (TryUseItem(__item))
		return true;

	auto t_old = GetType(old_owner);

	switch (t_old) {
	case iwSlot: {
		ToBag(itm, false);
	}break;

	case iwBag:
	{
          // Пытаемся найти свободный слот из списка разрешенных.
          // Если его нету, то принудительно займет первый слот,
          // указанный в списке.
          auto slots = __item->GetSlots();
          for ( u8 i = 0; i < (u8)slots.size(); ++i ) {
            __item->SetSlot( slots[ i ] );
            if ( ToSlot( itm, false ) )
              return true;
          }
          __item->SetSlot( slots.size() ? slots[ 0 ]: NO_ACTIVE_SLOT );
          if ( !ToSlot( itm, false ) )
            if ( !ToBelt( itm, false ) )
              ToSlot( itm, true );
	}break;

	case iwBelt: {
		ToBag(itm, false);
	}break;
	};

	return true;
}


bool CUIInventoryWnd::OnItemRButtonClick(CUICellItem* itm)
{
	SetCurrentItem				(itm);
	ActivatePropertiesBox		();
	return						false;
}

CUIDragDropListEx* CUIInventoryWnd::GetSlotList( u8 slot_idx ) {
  if( slot_idx == NO_ACTIVE_SLOT || GetInventory()->m_slots[ slot_idx ].m_bPersistent )
    return NULL;
  return m_slots_array[ slot_idx ];
}

void CUIInventoryWnd::ClearAllLists()
{
	m_pUIBagList->ClearAll					(true);
	m_pUIBeltList->ClearAll					(true);
	m_pUIOutfitList->ClearAll				(true);
	m_pUIPistolList->ClearAll				(true);
	if (Core.Features.test(xrCore::Feature::ogse_new_slots))
		m_pUIKnifeList->ClearAll(true);
	m_pUIAutomaticList->ClearAll			(true);
	if (Core.Features.test(xrCore::Feature::ogse_new_slots)) {
		m_pUIDetectorList->ClearAll(true);
		m_pUITorchList->ClearAll(true);
		m_pUIHelmetList->ClearAll(true);
		m_pUINightVisionList->ClearAll(true);
		m_pUIBIODetList->ClearAll(true);
		m_pUIBinocularList->ClearAll(true);
	}
}


void CUIInventoryWnd::UpdateWeight() {
  InventoryUtilities::UpdateWeight( UIBagWnd, true );
}
