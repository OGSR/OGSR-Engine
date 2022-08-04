#pragma once
#include "inventory_space.h"
#include "GameObject.h"

class IInventoryBox
{
protected:
    xr_vector<u16> m_items;

    void ProcessEvent(CGameObject* O, NET_Packet& P, u16 type);

public:
    bool m_in_use;
    IInventoryBox();
    void AddAvailableItems(TIItemContainer& items_container) const;
    bool IsEmpty() const;
    u32 GetSize() const;
    CScriptGameObject* GetObjectByName(LPCSTR);
    CScriptGameObject* GetObjectByIndex(u32);

    virtual CGameObject* cast_game_object() { return NULL; };
    virtual CInventoryItem* cast_inventory_item() { return NULL; }
    virtual CGameObject& object() = 0;
    virtual bool IsOpened() const { return true; }
};

template <class Based>
class CCustomInventoryBox : public Based, public IInventoryBox
{
    typedef Based inherited;
    typedef IInventoryBox inherited2;

public:
    virtual void OnEvent(NET_Packet& P, u16 type)
    {
        inherited::OnEvent(P, type);
        ProcessEvent(smart_cast<CGameObject*>(this), P, type);
    };

    virtual BOOL net_Spawn(CSE_Abstract* DC)
    {
        inherited::net_Spawn(DC);
        inherited::setVisible(TRUE);
        inherited::setEnabled(TRUE);
        inherited::set_tip_text("inventory_box_use");
        return TRUE;
    }

    virtual CGameObject& object() { return *smart_cast<CGameObject*>(this); }
};

class CInventoryBox : public CCustomInventoryBox<CGameObject> // CBasicInventoryBox
{
public:
    CInventoryBox() {}
};
