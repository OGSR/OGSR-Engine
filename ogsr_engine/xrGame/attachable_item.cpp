////////////////////////////////////////////////////////////////////////////
//	Module 		: attachable_item.cpp
//	Created 	: 11.02.2004
//  Modified 	: 11.02.2004
//	Author		: Dmitriy Iassenev
//	Description : Attachable item
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "physicsshellholder.h"
#include "attachable_item.h"
#include "inventoryowner.h"
#include "inventory.h"
#include "../xr_3da/xr_input.h"
#include "HUDManager.h"

CAttachableItem* CAttachableItem::m_dbgItem{};

IC CPhysicsShellHolder& CAttachableItem::object() const { return (item().object()); }

DLL_Pure* CAttachableItem::_construct()
{
    VERIFY(!m_item);
    m_item = smart_cast<CInventoryItem*>(this);
    VERIFY(m_item);
    return (&item().object());
}

CAttachableItem::~CAttachableItem()
{
    if (CAttachableItem::m_dbgItem == this)
        CAttachableItem::m_dbgItem = nullptr;
}

void CAttachableItem::reload(LPCSTR section)
{
    if (!pSettings->line_exist(section, "attach_angle_offset"))
        return;

    Fvector angle_offset = pSettings->r_fvector3(section, "attach_angle_offset");
    Fvector position_offset = pSettings->r_fvector3(section, "attach_position_offset");
    m_offset.setHPB(VPUSH(angle_offset));
    m_offset.c = position_offset;
    m_bone_name = pSettings->r_string(section, "attach_bone_name");
    //	enable							(m_auto_attach = !!(READ_IF_EXISTS(pSettings,r_bool,section,"auto_attach",TRUE)));
    enable(false);
#ifdef DEBUG
    m_valid = true;
#endif
}

void CAttachableItem::OnH_A_Chield()
{
    //	VERIFY							(m_valid);
    const CInventoryOwner* inventory_owner = smart_cast<const CInventoryOwner*>(object().H_Parent());
    if (inventory_owner && inventory_owner->attached(&item()))
        object().setVisible(true);
}

void CAttachableItem::renderable_Render()
{
    //	VERIFY							(m_valid);
    ::Render->set_Transform(&object().XFORM());
    ::Render->add_Visual(object().Visual());
}

void CAttachableItem::OnH_A_Independent()
{
    //	VERIFY							(m_valid);
    enable(false /*m_auto_attach*/);
}

void CAttachableItem::enable(bool value)
{
    //	VERIFY							(m_valid);
    if (!object().H_Parent())
    {
        m_enabled = value;
        return;
    }

    if (value && !enabled() && object().H_Parent())
    {
        CGameObject* game_object = smart_cast<CGameObject*>(object().H_Parent());
        CAttachmentOwner* owner = smart_cast<CAttachmentOwner*>(game_object);
        //		VERIFY				(owner);
        if (owner)
        {
            m_enabled = value;
            owner->attach(&item());
            object().setVisible(true);
        }
    }

    if (!value && enabled() && object().H_Parent())
    {
        CGameObject* game_object = smart_cast<CGameObject*>(object().H_Parent());
        CAttachmentOwner* owner = smart_cast<CAttachmentOwner*>(game_object);
        //		VERIFY				(owner);
        if (owner)
        {
            m_enabled = value;
            owner->detach(&item());
            object().setVisible(false);
        }
    }
}

bool CAttachableItem::can_be_attached() const
{
    //	VERIFY							(m_valid);
    if (!item().m_pCurrentInventory)
        return (false);

    if (!item().m_pCurrentInventory->IsBeltUseful())
        return (true);

    if (item().m_eItemPlace != eItemPlaceBelt)
        return (false);

    return (true);
}
void CAttachableItem::afterAttach()
{
    VERIFY(m_valid);
    object().processing_activate();
}

void CAttachableItem::afterDetach()
{
    VERIFY(m_valid);
    object().processing_deactivate();
}

extern float adj_delta_pos, adj_delta_rot;

void CAttachableItem::ParseCurrentItem(CGameFont* F) {}

void CAttachableItem::SaveAttachableParams() { Msg("!![%s] It's not implemented now", __FUNCTION__); }

bool attach_adjust_mode_keyb(int dik)
{
    if (!CAttachableItem::m_dbgItem)
        return false;

    if (pInput->iGetAsyncKeyState(DIK_LSHIFT) && pInput->iGetAsyncKeyState(DIK_RETURN))
    {
        CAttachableItem::m_dbgItem->SaveAttachableParams();
        return true;
    }

    const bool b_move = pInput->iGetAsyncKeyState(DIK_LSHIFT);
    const bool b_rot = pInput->iGetAsyncKeyState(DIK_LMENU);

    if (!b_move && !b_rot)
        return false;

    const int axis = pInput->iGetAsyncKeyState(DIK_Z) ? 0 : (pInput->iGetAsyncKeyState(DIK_X) ? 1 : (pInput->iGetAsyncKeyState(DIK_C) ? 2 : -1));

    if (axis == -1)
        return false;

    if (dik == DIK_PGUP)
    {
        if (b_move)
            CAttachableItem::m_dbgItem->mov(axis, adj_delta_pos);
        else
            CAttachableItem::m_dbgItem->rot(axis, adj_delta_rot);
        return true;
    }
    else if (dik == DIK_PGDN)
    {
        if (b_move)
            CAttachableItem::m_dbgItem->mov(axis, -adj_delta_pos);
        else
            CAttachableItem::m_dbgItem->rot(axis, -adj_delta_rot);
        return true;
    }

    return false;
}

void attach_draw_adjust_mode()
{
    if (!CAttachableItem::m_dbgItem)
        return;

    string1024 _text;

    CGameFont* F = UI()->Font()->pFontDI;
    F->SetAligment(CGameFont::alCenter);
    F->OutSetI(0.f, -0.8f);
    F->SetColor(D3DCOLOR_XRGB(125, 0, 0));
    sprintf_s(_text, "Adjusting attachable item [%s]", CAttachableItem::m_dbgItem->object().cNameSect().c_str());
    F->OutNext(_text);

    CAttachableItem::m_dbgItem->ParseCurrentItem(F);

    sprintf_s(_text, "move step  [%3.3f] rotate step  [%3.3f]", adj_delta_pos, adj_delta_rot);
    F->OutNext(_text);

    F->OutNext("HOLD LShift to move. ALT to rotate");
    F->OutNext("HOLD [Z]-x axis [X]-y axis [C]-z axis");

    F->OutNext("PageUP/PageDown - move.");
    F->OutSkip();

    F->OutNext("Console commands: adjust_delta_pos, adjust_delta_rot");
    F->OutSkip();

    const Fvector _pos = CAttachableItem::m_dbgItem->get_pos_offset();
    sprintf_s(_text, "attach_position_offset IS [%3.3f][%3.3f][%3.3f]", _pos.x, _pos.y, _pos.z);
    F->OutNext(_text);

    const Fvector _ang = CAttachableItem::m_dbgItem->get_angle_offset();
    sprintf_s(_text, "attach_angle_offset IS [%3.3f][%3.3f][%3.3f]", _ang.x, _ang.y, _ang.z);
    F->OutNext(_text);
}
