////////////////////////////////////////////////////////////////////////////
//	Module 		: inventory_item_object.h
//	Created 	: 24.03.2003
//  Modified 	: 27.12.2004
//	Author		: Victor Reutsky, Yuri Dobronravin
//	Description : Inventory item object implementation
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "physic_item.h"
#include "inventory_item.h"

class CInventoryItemObject : public CInventoryItem, public CPhysicItem
{
public:
    CInventoryItemObject();
    virtual ~CInventoryItemObject();
    virtual DLL_Pure* _construct();

public:
    virtual CPhysicsShellHolder* cast_physics_shell_holder() { return this; }
    virtual CInventoryItem* cast_inventory_item() { return this; }
    virtual CAttachableItem* cast_attachable_item() { return this; }
    virtual CWeapon* cast_weapon() { return 0; }
    virtual CFoodItem* cast_food_item() { return 0; }
    virtual CMissile* cast_missile() { return 0; }
    virtual CHudItem* cast_hud_item() { return 0; }
    virtual CWeaponAmmo* cast_weapon_ammo() { return 0; }
    virtual CGameObject* cast_game_object() { return this; };

public:
    virtual void Load(LPCSTR section);
    virtual LPCSTR Name();
    virtual LPCSTR NameShort();
    //.	virtual LPCSTR	NameComplex				();
    virtual void Hit(SHit* pHDS);

    virtual void OnH_B_Independent(bool just_before_destroy);
    virtual void OnH_A_Independent();
    virtual void OnH_B_Chield();
    virtual void OnH_A_Chield();
    virtual void UpdateCL();
    virtual void OnEvent(NET_Packet& P, u16 type);
    virtual BOOL net_Spawn(CSE_Abstract* DC);
    virtual void net_Destroy();
    virtual void net_Export(CSE_Abstract* E);
    virtual void save(NET_Packet& output_packet);
    virtual void load(IReader& input_packet);
    virtual BOOL net_SaveRelevant() { return TRUE; }
    virtual void renderable_Render();
    virtual void reload(LPCSTR section);
    virtual void reinit();
    virtual void activate_physic_shell();
    virtual void on_activate_physic_shell();
    virtual void modify_holder_params(float& range, float& fov) const;

protected:
#ifdef DEBUG
    virtual void OnRender();
#endif

public:
    virtual bool Useful() const;

public:
    virtual u32 ef_weapon_type() const;
};

#include "inventory_item_inline.h"
