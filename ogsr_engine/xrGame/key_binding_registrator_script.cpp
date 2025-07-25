#include "stdafx.h"
#include <dinput.h>
#include "key_binding_registrator.h"
#include "xr_level_controller.h"
#include "../xr_3da/xr_input.h"

using namespace luabind;

int dik_to_bind(int dik) { return get_binded_action(dik); }

int bind_to_dik(EGameActions bind) { return get_action_dik(bind); }

BOOL key_state(int key) { return pInput->iGetAsyncKeyState(key); }

void key_binding_registrator::script_register(lua_State* L)
{
    auto game_actions = class_<enum_exporter<EGameActions>>("key_bindings");

    for (const auto& action : actions)
    {
        if (action.id != kLASTACTION)
            game_actions = std::move(game_actions).enum_("commands")[value(action.export_name, action.id)];
    }

    module(
        L)[def("dik_to_bind", &dik_to_bind), def("bind_to_dik", &bind_to_dik), def("dik_to_keyname", &dik_to_keyname), def("keyname_to_dik", &keyname_to_dik),
           def("key_state", &key_state),

           std::move(game_actions),

           class_<key_binding_registrator>("DIK_keys")
               .enum_("dik_keys")
                   [value("DIK_ESCAPE", int(DIK_ESCAPE)), value("DIK_2", int(DIK_2)), value("DIK_4", int(DIK_4)), value("DIK_6", int(DIK_6)), value("DIK_8", int(DIK_8)),
                    value("DIK_0", int(DIK_0)), value("DIK_EQUALS", int(DIK_EQUALS)), value("DIK_TAB", int(DIK_TAB)), value("DIK_W", int(DIK_W)), value("DIK_R", int(DIK_R)),
                    value("DIK_Y", int(DIK_Y)), value("DIK_I", int(DIK_I)), value("DIK_P", int(DIK_P)), value("DIK_RBRACKET", int(DIK_RBRACKET)),
                    value("DIK_LCONTROL", int(DIK_LCONTROL)), value("DIK_S", int(DIK_S)), value("DIK_F", int(DIK_F)), value("DIK_H", int(DIK_H)), value("DIK_K", int(DIK_K)),
                    value("DIK_SEMICOLON", int(DIK_SEMICOLON)), value("DIK_GRAVE", int(DIK_GRAVE)), value("DIK_BACKSLASH", int(DIK_BACKSLASH)), value("DIK_X", int(DIK_X)),
                    value("DIK_V", int(DIK_V)), value("DIK_N", int(DIK_N)), value("DIK_COMMA", int(DIK_COMMA)), value("DIK_SLASH", int(DIK_SLASH)),
                    value("DIK_MULTIPLY", int(DIK_MULTIPLY)), value("DIK_SPACE", int(DIK_SPACE)), value("DIK_F1", int(DIK_F1)), value("DIK_F3", int(DIK_F3)),
                    value("DIK_F5", int(DIK_F5)), value("DIK_F7", int(DIK_F7)), value("DIK_F9", int(DIK_F9)), value("DIK_NUMLOCK", int(DIK_NUMLOCK)),
                    value("DIK_NUMPAD7", int(DIK_NUMPAD7)), value("DIK_NUMPAD9", int(DIK_NUMPAD9)), value("DIK_NUMPAD4", int(DIK_NUMPAD4)), value("DIK_NUMPAD6", int(DIK_NUMPAD6)),
                    value("DIK_NUMPAD1", int(DIK_NUMPAD1)), value("DIK_NUMPAD3", int(DIK_NUMPAD3)), value("DIK_DECIMAL", int(DIK_DECIMAL)), value("DIK_F12", int(DIK_F12)),
                    //				value("DIK_F14",						int(DIK_F14			)),
                    //				value("DIK_KANA",						int(DIK_KANA		)),
                    //				value("DIK_NOCONVERT",					int(DIK_NOCONVERT	)),
                    //				value("DIK_NUMPADEQUALS",				int(DIK_NUMPADEQUALS)),
                    //				value("DIK_AT",							int(DIK_AT			)),
                    //				value("DIK_UNDERLINE",					int(DIK_UNDERLINE	)),
                    //				value("DIK_STOP",						int(DIK_STOP		)),
                    //				value("DIK_UNLABELED",					int(DIK_UNLABELED	)),
                    value("DIK_RCONTROL", int(DIK_RCONTROL)), value("DIK_DIVIDE", int(DIK_DIVIDE)), value("DIK_RMENU", int(DIK_RMENU)), value("DIK_UP", int(DIK_UP)),
                    value("DIK_LEFT", int(DIK_LEFT)), value("DIK_END", int(DIK_END)), value("DIK_NEXT", int(DIK_NEXT)), value("DIK_DELETE", int(DIK_DELETE)),
                    value("DIK_RWIN", int(DIK_RWIN)), value("DIK_PAUSE", int(DIK_PAUSE)), value("MOUSE_2", int(MOUSE_2)), value("DIK_1", int(DIK_1)), value("DIK_3", int(DIK_3)),
                    value("DIK_5", int(DIK_5)), value("DIK_7", int(DIK_7)), value("DIK_9", int(DIK_9)), value("DIK_MINUS", int(DIK_MINUS)), value("DIK_BACK", int(DIK_BACK)),
                    value("DIK_Q", int(DIK_Q)), value("DIK_E", int(DIK_E)), value("DIK_T", int(DIK_T)), value("DIK_U", int(DIK_U)), value("DIK_O", int(DIK_O)),
                    value("DIK_LBRACKET", int(DIK_LBRACKET)), value("DIK_RETURN", int(DIK_RETURN)), value("DIK_A", int(DIK_A)), value("DIK_D", int(DIK_D)),
                    value("DIK_G", int(DIK_G)), value("DIK_J", int(DIK_J)), value("DIK_L", int(DIK_L)), value("DIK_APOSTROPHE", int(DIK_APOSTROPHE)),
                    value("DIK_LSHIFT", int(DIK_LSHIFT)), value("DIK_Z", int(DIK_Z)), value("DIK_C", int(DIK_C)), value("DIK_B", int(DIK_B)), value("DIK_M", int(DIK_M)),
                    value("DIK_PERIOD", int(DIK_PERIOD)), value("DIK_RSHIFT", int(DIK_RSHIFT)), value("DIK_LMENU", int(DIK_LMENU)), value("DIK_CAPITAL", int(DIK_CAPITAL)),
                    value("DIK_F2", int(DIK_F2)), value("DIK_F4", int(DIK_F4)), value("DIK_F6", int(DIK_F6)), value("DIK_F8", int(DIK_F8)), value("DIK_F10", int(DIK_F10)),
                    value("DIK_SCROLL", int(DIK_SCROLL)), value("DIK_NUMPAD8", int(DIK_NUMPAD8)), value("DIK_SUBTRACT", int(DIK_SUBTRACT)), value("DIK_NUMPAD5", int(DIK_NUMPAD5)),
                    value("DIK_ADD", int(DIK_ADD)), value("DIK_NUMPAD2", int(DIK_NUMPAD2)), value("DIK_NUMPAD0", int(DIK_NUMPAD0)), value("DIK_F11", int(DIK_F11)),
                    //				value("DIK_F13",						int(DIK_F13			)),
                    //				value("DIK_F15",						int(DIK_F15			)),
                    //				value("DIK_CONVERT",					int(DIK_CONVERT		)),
                    //				value("DIK_YEN",						int(DIK_YEN			)),
                    //				value("DIK_CIRCUMFLEX",					int(DIK_CIRCUMFLEX	)),
                    //				value("DIK_COLON",						int(DIK_COLON		)),
                    //				value("DIK_KANJI",						int(DIK_KANJI		)),
                    //				value("DIK_AX",							int(DIK_AX			)),
                    value("DIK_NUMPADENTER", int(DIK_NUMPADENTER)),
                    //				value("DIK_NUMPADCOMMA",				int(DIK_NUMPADCOMMA	)),
                    value("DIK_SYSRQ", int(DIK_SYSRQ)), value("DIK_HOME", int(DIK_HOME)), value("DIK_PRIOR", int(DIK_PRIOR)), value("DIK_RIGHT", int(DIK_RIGHT)),
                    value("DIK_DOWN", int(DIK_DOWN)), value("DIK_INSERT", int(DIK_INSERT)), value("DIK_LWIN", int(DIK_LWIN)), value("DIK_APPS", int(DIK_APPS)),
                    value("MOUSE_1", int(MOUSE_1)), value("MOUSE_2", int(MOUSE_2)), value("MOUSE_3", int(MOUSE_3)), value("MOUSE_4", int(MOUSE_4)), value("MOUSE_5", int(MOUSE_5)),
                    value("MOUSE_6", int(MOUSE_6)), value("MOUSE_7", int(MOUSE_7)), value("MOUSE_8", int(MOUSE_8))]];
}
