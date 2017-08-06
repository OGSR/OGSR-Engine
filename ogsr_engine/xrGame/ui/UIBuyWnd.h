#pragma once

#include "UIBuyWndBase.h"
#include "UIStatic.h"
#include "UIDragDropListEx.h"
#include "UIBuyWeaponTab.h"
#include "UIBagWnd.h"
#include "UI3tButton.h"
#include "UIPropertiesBox.h"
#include "UIItemInfo.h"

typedef enum {
	MP_SLOT_PISTOL,
	MP_SLOT_RIFLE,
	MP_SLOT_BELT,
	MP_SLOT_OUTFIT,

	MP_SLOT_NUM
} MP_BUY_SLOT;

class CUIWeaponCellItem;
class CWeapon;

class CUIBuyWnd :	public IBuyWnd
{
	typedef CUIDialogWnd	inherited;
public:
					CUIBuyWnd					();
	virtual			~CUIBuyWnd					();

	// IBuyWnd
	virtual void 				Init						(const shared_str& sectionName, const shared_str& sectionPrice);
	virtual void				BindDragDropListEvents		(CUIDragDropListEx* lst, bool bDrag);
	virtual const u8			GetItemIndex				(u32 slotNum, u32 idx, u8 &sectionNum);
	virtual const u8			GetBeltSize					();
	virtual const u8			GetWeaponIndexInBelt		(u32 indexInBelt, u8 &sectionId, u8 &itemId, u8 &count);
	virtual void				GetWeaponIndexByName		(const shared_str& sectionName, u8 &grpNum, u8 &idx);
	virtual u32					GetMoneyAmount				() const;
	virtual void				IgnoreMoney					(bool ignore);
	virtual void				SectionToSlot				(const u8 grpNum, u8 uIndexInSlot, bool bRealRepresentationSet);
	virtual void 				SetMoneyAmount				(u32 money);
	virtual bool 				CheckBuyAvailabilityInSlots	();
	virtual void				AddonToSlot					(int add_on, int slot, bool bRealRepresentationSet);
	virtual const shared_str&	GetWeaponNameByIndex		(u8 grpNum, u8 idx);
	virtual void 				SetSkin						(u8 SkinID);
	virtual void				IgnoreMoneyAndRank			(bool ignore);
	virtual void				ClearSlots					();
	virtual void				ClearRealRepresentationFlags();
	virtual const u8			GetWeaponIndex				(u32 slotNum);//
	virtual bool 				CanBuyAllItems				();
	virtual void 				ResetItems					();
	virtual void				SetRank						(u32 rank);
	virtual u32					GetRank						();

	virtual void				ItemToBelt					(const shared_str& sectionName);
	virtual void				ItemToRuck					(const shared_str& sectionName, u32 addons);
	virtual void				ItemToSlot					(const shared_str& sectionName, u32 addons);

	/////

	void 			OnTabChange					();
	void 			OnMenuLevelChange			();
	void 			CheckAddons					(CUICellItem* itm);
	void 			UpdItem						(CUICellItem* itm);
	void 			UpdAddon					(CUIWeaponCellItem* itm, CSE_ALifeItemWeapon::EWeaponAddonState add_on);

	// handlers
	void 			OnBtnOk						();
	void 			OnBtnCancel					();
	void 			OnBtnClear					();
	void 			OnMoneyChange				();
	void 			OnBtnBulletBuy				(int slot);
	void 			OnBtnRifleGrenade			();
	void 			AfterBuy					();
	void 			HightlightCurrAmmo			();

	// CUIWindow
	virtual bool 	OnKeyboard					(int dik, EUIMessages keyboard_action);
	virtual void 	SendMessage					(CUIWindow* pWnd, s16 msg, void* pData = 0);
	virtual void 	Update						();
	virtual void 	Show						();
	virtual void 	Hide						();

	// drag drop handlers
	bool	xr_stdcall	OnItemDrop				(CUICellItem* itm);
	bool	xr_stdcall	OnItemStartDrag			(CUICellItem* itm);
	bool	xr_stdcall	OnItemDbClick			(CUICellItem* itm);
	bool	xr_stdcall	OnItemSelected			(CUICellItem* itm);
	bool	xr_stdcall	OnItemRButtonClick		(CUICellItem* itm);

	

	// Получить данные о аддонах к оружию. Младшие 3 бита, если установлены в 1 означают:
	// 2 - Silencer, 1 - Grenade Launcher, 0 - Scope
	const u8			GetWeaponAddonInfoByIndex	(u8 idx);

	void				ReloadItemsPrices			();
	virtual bool	IsIgnoreMoneyAndRank		();

protected:
	void				DestroyAllItems				();
	void				SetCurrentItem				(CUICellItem* itm);
	CUICellItem*		CurrentItem					();
	CInventoryItem*		CurrentIItem				();
	CWeapon*			GetPistol					();
	CWeapon*			GetRifle					();
	void				ActivatePropertiesBox		();
	EListType			GetType						(CUIDragDropListEx* l);
	CUIDragDropListEx*	GetSlotList					(u32 slot_idx);
	MP_BUY_SLOT			GetLocalSlot				(u32 slot);
	bool				ToSlot						(CUICellItem* itm, bool force_place);
	bool				ToSlot						(CUICellItem* itm, u8 weapon_slot, bool force_place);
	bool				ToBag						(CUICellItem* itm, bool b_use_cursor_pos);
	bool				ToBelt						(CUICellItem* itm, bool b_use_cursor_pos);
	bool				CanPutInSlot				(CInventoryItem* iitm);
	bool				CanPutInBag					(CInventoryItem* iitm);
	bool				CanPutInBelt				(CInventoryItem* iitm);
	bool				ClearTooExpensiveItems		();
	bool				ClearSlot_ifTooExpensive	(int slot);
	bool				SlotToSection				(int slot);
	void				AttachAddon					(CInventoryItem* item_to_upgrade);
	void				DetachAddon					(const char* addon_name);
	void				ProcessPropertiesBoxClicked	();
	void				UpdateOutfit				();
	void				Highlight					(int slot);


	// data
	shared_str			m_sectionName;
	shared_str			m_sectionPrice;
	CUICellItem*		m_pCurrentCellItem;

	bool				m_bIgnoreMoneyAndRank;

	// background textures
	CUIStatic			m_slotsBack;
	CUIStatic			m_back;

	// buttons
	CUI3tButton 		m_btnOk;
	CUI3tButton 		m_btnCancel;
	CUI3tButton 		m_btnClear;

	CUI3tButton 		m_btnPistolBullet;
	CUI3tButton 		m_btnPistolSilencer;
	CUI3tButton 		m_btnRifleBullet;
	CUI3tButton 		m_btnRifleSilencer;
	CUI3tButton 		m_btnRifleScope;
	CUI3tButton 		m_btnRifleGrenadelauncer;
	CUI3tButton 		m_btnRifleGrenade;

	CUIStatic			m_moneyInfo;
	
	// controls
	CUIPropertiesBox	m_propertiesBox;
	CUIItemInfo			m_itemInfo;
	CUIStatic			m_rankInfo;
	CUIBagWnd			m_bag;
	CUIBuyWeaponTab		m_tab;
	CUIDragDropListEx*	m_list[MP_SLOT_NUM];

};