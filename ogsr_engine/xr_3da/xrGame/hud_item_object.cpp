////////////////////////////////////////////////////////////////////////////
//	Module 		: hud_item_object.cpp
//	Created 	: 24.03.2003
//  Modified 	: 27.12.2004
//	Author		: Yuri Dobronravin
//	Description : HUD item
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "hud_item_object.h"

CHudItemObject::CHudItemObject			()
{
}

CHudItemObject::~CHudItemObject			()
{
}

DLL_Pure *CHudItemObject::_construct	()
{
	CInventoryItemObject::_construct();
	CHudItem::_construct		();
	return						(this);
}

void CHudItemObject::Load				(LPCSTR section)
{
	CInventoryItemObject::Load	(section);
	CHudItem::Load				(section);
}

bool CHudItemObject::Action				(s32 cmd, u32 flags)
{
	if (CInventoryItemObject::Action(cmd, flags))
		return					(true);
	return						(CHudItem::Action(cmd,flags));
}

void CHudItemObject::SwitchState		(u32 S)
{
	CHudItem::SwitchState		(S);
}

void CHudItemObject::OnStateSwitch		(u32 S)
{
	CHudItem::OnStateSwitch		(S);
}

void CHudItemObject::OnEvent			(NET_Packet& P, u16 type)
{
	CInventoryItemObject::OnEvent(P,type);
	CHudItem::OnEvent			(P,type);
}

void CHudItemObject::OnH_A_Chield		()
{
	CHudItem::OnH_A_Chield				();
	CInventoryItemObject::OnH_A_Chield	();
}

void CHudItemObject::OnH_B_Chield		()
{
	CInventoryItemObject::OnH_B_Chield	();
	CHudItem::OnH_B_Chield				();
}

void CHudItemObject::OnH_B_Independent	(bool just_before_destroy)
{
	CHudItem::OnH_B_Independent				(just_before_destroy);
	CInventoryItemObject::OnH_B_Independent	(just_before_destroy);
}

void CHudItemObject::OnH_A_Independent	()
{
	CHudItem::OnH_A_Independent				();
	CInventoryItemObject::OnH_A_Independent	();
}

BOOL CHudItemObject::net_Spawn			(CSE_Abstract* DC)
{
	return						(
		CInventoryItemObject::net_Spawn(DC) &&
		CHudItem::net_Spawn(DC)
	);
}

void CHudItemObject::net_Destroy		()
{
	CHudItem::net_Destroy		();
	CInventoryItemObject::net_Destroy	();
}

bool CHudItemObject::Activate			()
{
	return						(CHudItem::Activate());
}

void CHudItemObject::Deactivate			()
{
	CHudItem::Deactivate		();
}

void CHudItemObject::UpdateCL			()
{
	CInventoryItemObject::UpdateCL	();
	CHudItem::UpdateCL				();
}

void CHudItemObject::renderable_Render	()
{
	CHudItem::renderable_Render	();
}

void CHudItemObject::on_renderable_Render()
{
	CInventoryItemObject::renderable_Render();
}
