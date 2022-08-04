////////////////////////////////////////////////////////////////////////////
//	Module 		: inventory_container.h
//	Created 	: 12.11.2014
//  Modified 	: 11.12.2014
//	Author		: Alexander Petrov
//	Description : Mobile container class, based on inventory item
////////////////////////////////////////////////////////////////////////////
#pragma once
#include "InventoryBox.h"
#include "inventory_item.h"
#include "inventory_item_object.h"

typedef struct _SItemsInfo
{
    float weight;
    u32 cost;
    float info[8];
} SItemsInfo, *PSItemsInfo;

// CInventoryContainer
class CInventoryContainer : public CCustomInventoryBox<CInventoryItemObject>

{
private:
    typedef CCustomInventoryBox<CInventoryItemObject> inherited;
    bool m_opened;

    virtual u32 CalcItems(SItemsInfo& info) const;

public:
    CInventoryContainer();
    virtual bool CanTrade() const;
    virtual u32 Cost() const;
    virtual float Weight() const;
    virtual float RadiationRestoreSpeed() const;
    virtual DLL_Pure* _construct();
    virtual void OnEvent(NET_Packet& P, u16 type);
    virtual BOOL net_Spawn(CSE_Abstract* DC);

    virtual bool IsClosed() const { return !m_opened; }; // alpet: если закрыт - можно подобрать в инвентарь
    virtual bool IsOpened() const { return m_opened; }; // alpet: если открыт - в нем можно ковыряться

    void open();
    void close();
};
