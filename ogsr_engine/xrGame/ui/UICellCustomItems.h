#pragma once
#include "UICellItem.h"
#include "../Weapon.h"

class CUIInventoryCellItem : public CUICellItem
{
    typedef CUICellItem inherited;

protected:
    bool b_auto_drag_childs;

public:
    CUIInventoryCellItem(CInventoryItem* itm);
    virtual void Update();
    virtual bool EqualTo(CUICellItem* itm);
    virtual CUIDragItem* CreateDragItem();
    CInventoryItem* object() { return static_cast<CInventoryItem*>(m_pData); }

    // Real Wolf: Для коллбеков. 25.07.2014.
    virtual void OnFocusReceive();
    virtual void OnFocusLost();
    virtual bool OnMouse(float, float, EUIMessages);
    // Real Wolf: Для метода get_cell_item(). 25.07.2014.
    virtual ~CUIInventoryCellItem();
};

class CUIAmmoCellItem : public CUIInventoryCellItem
{
    typedef CUIInventoryCellItem inherited;

public:
    CUIAmmoCellItem(CWeaponAmmo* itm);
    virtual ~CUIAmmoCellItem() = default;

    virtual void Update();
    virtual void UpdateItemText() override;
    virtual bool EqualTo(CUICellItem* itm);
    CWeaponAmmo* object() { return static_cast<CWeaponAmmo*>(m_pData); }
};

class CUIWeaponCellItem : public CUIInventoryCellItem
{
    typedef CUIInventoryCellItem inherited;

public:
    enum eAddonType
    {
        eSilencer = 0,
        eScope,
        eLauncher,
        eMaxAddon
    };
    CUIStatic* m_addons[eMaxAddon];

protected:
    Fvector2 m_addon_offset[eMaxAddon];
    void CreateIcon(eAddonType, CIconParams& params);
    void DestroyIcon(eAddonType);
    CUIStatic* GetIcon(eAddonType);
    void InitAddon(CUIStatic* s, CIconParams& params, Fvector2 offset, bool b_rotate);
    void InitAllAddons(CUIStatic* s_silencer, CUIStatic* s_scope, CUIStatic* s_launcher, bool b_vertical);
    bool is_scope();
    bool is_silencer();
    bool is_launcher();

public:
    CUIWeaponCellItem(CWeapon* itm);
    virtual ~CUIWeaponCellItem();
    virtual void Update();
    CWeapon* object() { return (CWeapon*)m_pData; }
    virtual void OnAfterChild(CUIDragDropListEx* parent_list);
    CUIDragItem* CreateDragItem();
    virtual bool EqualTo(CUICellItem* itm);
    CUIStatic* get_addon_static(u32 idx) { return m_addons[idx]; }
    Fvector2 get_addon_offset(u32 idx) { return m_addon_offset[idx]; }
};
