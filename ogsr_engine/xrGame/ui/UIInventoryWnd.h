#pragma once

class CInventory;

#include "UIDialogWnd.h"
#include "UIStatic.h"

#include "UIProgressBar.h"

#include "UIPropertiesBox.h"
#include "UIOutfitSlot.h"

#include "UIOutfitInfo.h"
#include "UIItemInfo.h"
#include "../inventory_space.h"

class CArtefact;
class CUI3tButton;
class CUIDragDropListEx;
class CUICellItem;

class CUIInventoryWnd: public CUIDialogWnd
{
private:
	typedef CUIDialogWnd	inherited;
	bool					m_b_need_reinit;
	bool					m_b_need_update_stats;
public:
							CUIInventoryWnd				();
	virtual					~CUIInventoryWnd			();

	virtual void			Init						();

	void					InitInventory				();
	void					InitInventory_delayed		();
	virtual bool StopAnyMove() { return !!Core.Features.test(xrCore::Feature::more_hide_weapon); }

	virtual void			SendMessage					(CUIWindow *pWnd, s16 msg, void *pData);
	virtual bool			OnMouse						(float x, float y, EUIMessages mouse_action);
	virtual bool			OnKeyboard					(int dik, EUIMessages keyboard_action);


	IC CInventory*			GetInventory				()					{return m_pInv;}

	virtual void			Update						();
	virtual void			Draw						();

	virtual void			Show						();
	virtual void			Hide						();

	void HideSlotsHighlight();
	void ShowSlotsHighlight(PIItem InvItem);

	void					AddItemToBag				(PIItem pItem);

protected:
	enum eInventorySndAction{	eInvSndOpen	=0,
								eInvSndClose,
								eInvItemToSlot,
								eInvItemToBelt,
								eInvItemToRuck,
								eInvProperties,
								eInvDropItem,
								eInvAttachAddon,
								eInvDetachAddon,
								eInvItemUse,
								eInvSndMax};

	ref_sound					sounds					[eInvSndMax];
	void						PlaySnd					(eInventorySndAction a);

	CUIStatic					UIBeltSlots;
	CUIStatic					UIBack;
	CUIStatic*					UIRankFrame;
	CUIStatic*					UIRank;

	CUIStatic					UIBagWnd;
	CUIStatic					UIMoneyWnd;
	CUIStatic					UIDescrWnd;
	CUIFrameWindow				UIPersonalWnd;

	CUI3tButton*				UIExitButton;

	CUIStatic					UIStaticBottom;
	CUIStatic					UIStaticTime;
	CUIStatic					UIStaticTimeString;

	CUIStatic					UIStaticPersonal;
		
	CUIDragDropListEx*			m_pUIBagList;
	CUIDragDropListEx*			m_pUIBeltList;

	CUIDragDropListEx*			m_pUIPistolList;
	CUIDragDropListEx*			m_pUIAutomaticList;
	CUIOutfitDragDropList*		m_pUIOutfitList;

	CUIDragDropListEx*			m_pUIKnifeList;
	CUIDragDropListEx*			m_pUIHelmetList;
	CUIDragDropListEx*			m_pUIBIODetList;
	CUIDragDropListEx*			m_pUINightVisionList;
	CUIDragDropListEx*			m_pUIDetectorList;
	CUIDragDropListEx*			m_pUITorchList;
	CUIDragDropListEx*			m_pUIBinocularList;

	// alpet: для индексированного доступа
	CUIDragDropListEx*			m_slots_array[ SLOTS_TOTAL ];

	void						ClearAllLists				();
	void						BindDragDropListEnents		(CUIDragDropListEx* lst);
	
	EListType					GetType						(CUIDragDropListEx* l);
	CUIDragDropListEx*			GetSlotList					(u8 slot_idx);

	bool				OnItemDrop					(CUICellItem* itm);
	bool				OnItemStartDrag				(CUICellItem* itm);
	bool				OnItemDbClick				(CUICellItem* itm);
	bool				OnItemSelected				(CUICellItem* itm);
	bool				OnItemRButtonClick			(CUICellItem* itm);


	CUIStatic					UIProgressBack;
	CUIStatic					UIProgressBack_rank;
	CUIProgressBar				UIProgressBarHealth;	
	CUIProgressBar				UIProgressBarPsyHealth;
	CUIProgressBar				UIProgressBarRadiation;
	CUIProgressBar				UIProgressBarRank;

	CUIPropertiesBox			UIPropertiesBox;
	
	//информация о персонаже
	CUIOutfitInfo				UIOutfitInfo;
	CUIItemInfo					UIItemInfo;

	CInventory*					m_pInv;
public:
	CUICellItem*				m_pCurrentCellItem;
protected:
	bool						DropItem					(PIItem itm, CUIDragDropListEx* lst);
	bool						TryUseItem					(PIItem itm);
	//----------------------	-----------------------------------------------
	void						SendEvent_Item2Slot			(PIItem	pItem);
	void						SendEvent_Item2Belt			(PIItem	pItem);
	void						SendEvent_Item2Ruck			(PIItem	pItem);
	void						SendEvent_Item_Drop			(PIItem	pItem);
	void						SendEvent_Item_Eat			(PIItem	pItem);
	void						SendEvent_ActivateSlot		(PIItem	pItem);

	//---------------------------------------------------------------------

	void						ProcessPropertiesBoxClicked	();
	void						ActivatePropertiesBox		();

	void						DropCurrentItem				(bool b_all);
	void						EatItem						(PIItem itm);
	
	bool						ToSlot						(CUICellItem* itm, u8 _slot_id, bool force_place);
	bool						ToSlot						(CUICellItem* itm, bool force_place);
	bool						ToBag						(CUICellItem* itm, bool b_use_cursor_pos);
	bool						ToBelt						(CUICellItem* itm, bool b_use_cursor_pos);


	void						AttachAddon					(PIItem item_to_upgrade);
	void						DetachAddon					(const char* addon_name);

	void						SetCurrentItem				(CUICellItem* itm);
	CUICellItem*				CurrentItem					();

	TIItemContainer				ruck_list;
	u32							m_iCurrentActiveSlot;
private:
	bool dont_update_belt_flag{};
public:
	PIItem						CurrentIItem();
	void UpdateWeight();
	void UpdateOutfit();
};
