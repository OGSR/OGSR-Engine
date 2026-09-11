// File:        UIComboBox_script.cpp
// Description: exports CUIComobBox to LUA environment
// Created:     11.12.2004
// Author:      Serhiy O. Vynnychenko
// Mail:        narrator@gsc-game.kiev.ua
//
// Copyright 2004 GSC Game World
//

#include "stdafx.h"
#include "UIComboBox.h"
#include "UIListBoxItem.h"

using namespace luabind;


void CUIComboBox::script_register(lua_State* L)
{
    module(L)[class_<CUIComboBox, CUIWindow>("CUIComboBox")
                  .def(constructor<>())
                  .def("SetVertScroll", &CUIComboBox::SetVertScroll)
                  .def("SetListLength", &CUIComboBox::SetListLength)
                  .def("CurrentID", &CUIComboBox::CurrentID)
                  .def("disable_id", &CUIComboBox::disable_id)
                  .def("enable_id", &CUIComboBox::enable_id)
                  .def("AddItem", &CUIComboBox::AddItem_)
                  .def("GetText", &CUIComboBox::GetText)
                  .def("GetTextOf", &CUIComboBox::GetTextOf)
                  .def("SetText", &CUIComboBox::SetText)
                  .def("ClearList", &CUIComboBox::ClearList)
                  .def("SetCurrentOptValue", &CUIComboBox::SetCurrentOptValue)
                  .def("SetCurrentID", &CUIComboBox::SetItemToken)
                  .def("GetCurrentValueName", [](CUIComboBox* self) { return get_token_name(self->GetOptToken(), self->m_itoken_id); })
    ];
}