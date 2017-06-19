// File:        UIComboBox_script.cpp
// Description: exports CUIComobBox to LUA environment
// Created:     11.12.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
//
// Copyright 2004 GSC Game World
//

#include "pch_script.h"
#include "UIComboBox.h"

using namespace luabind;

#pragma optimize("s",on)
void CUIComboBox::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUIComboBox, CUIWindow>("CUIComboBox")
		.def(						constructor<>())
		.def("Init",				(void (CUIComboBox::*)(float, float, float))   &CUIComboBox::Init)
		.def("Init",				(void (CUIComboBox::*)(float, float, float, float))   &CUIComboBox::Init)		
		.def("SetVertScroll",		&CUIComboBox::SetVertScroll)
		.def("SetListLength",		&CUIComboBox::SetListLength)
		.def("CurrentID",			&CUIComboBox::CurrentID)
		.def("SetCurrentID",		&CUIComboBox::SetItem)
		
//		.def("AddItem",				(void (CUIComboBox::*)(LPCSTR, bool)) CUIComboBox::AddItem)
		.def("AddItem",				(CUIListBoxItem* (CUIComboBox::*)(LPCSTR)) &CUIComboBox::AddItem_)
		.def("GetText",				&CUIComboBox::GetText)
	];
}