#include "stdafx.h"

#include "script_ui_registrator.h"
#include "UI\UIMultiTextStatic.h"
#include "MainMenu.h"

using namespace luabind;

CMainMenu* MainMenu();

#pragma optimize("s", on)
void UIRegistrator::script_register(lua_State* L)
{
    module(L)[

        class_<CGameFont>("CGameFont")
            .enum_("EAligment")[value("alLeft", int(CGameFont::alLeft)), value("alRight", int(CGameFont::alRight)), value("alCenter", int(CGameFont::alCenter))],

        class_<CUICaption>("CUICaption").def("addCustomMessage", &CUICaption::addCustomMessage).def("setCaption", &CUICaption::setCaption),

        class_<CMainMenu, CDialogHolder>("CMainMenu")
            .def("GetGSVer", &CMainMenu::GetGSVer)
            .def("PlaySound", &CMainMenu::PlaySound)
            .def("IsActive", &CMainMenu::IsActive)]
        ,

        module(L, "main_menu")[def("get_main_menu", &MainMenu)];
}
