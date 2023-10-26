#include "stdafx.h"
#include <dinput.h>
#include "..\xr_3da\xr_ioconsole.h"
#include "..\xr_3da\xr_input.h"
#include "..\xr_3da\xr_ioc_cmd.h"
#include "xr_level_controller.h"
#include "string_table.h"

#define DEF_ACTION(a1, a2) {a1, a2, #a2},

xr_vector<_action> actions = {
    DEF_ACTION("left", kLEFT) DEF_ACTION("right", kRIGHT) DEF_ACTION("up", kUP) DEF_ACTION("down", kDOWN) DEF_ACTION("jump", kJUMP) DEF_ACTION("crouch", kCROUCH)
        DEF_ACTION("crouch_toggle", kCROUCH_TOGGLE) DEF_ACTION("accel", kACCEL) DEF_ACTION("sprint_toggle", kSPRINT_TOGGLE)

            DEF_ACTION("forward", kFWD) DEF_ACTION("back", kBACK) DEF_ACTION("lstrafe", kL_STRAFE) DEF_ACTION("rstrafe", kR_STRAFE)

                DEF_ACTION("llookout", kL_LOOKOUT) DEF_ACTION("rlookout", kR_LOOKOUT)

                    DEF_ACTION("turn_engine", kENGINE)

                        DEF_ACTION("cam_1", kCAM_1) DEF_ACTION("cam_2", kCAM_2) DEF_ACTION("cam_3", kCAM_3)

                                DEF_ACTION("torch", kTORCH) DEF_ACTION("night_vision", kNIGHT_VISION) DEF_ACTION("wpn_1", kWPN_1) DEF_ACTION("wpn_2", kWPN_2)
                                    DEF_ACTION("wpn_3", kWPN_3) DEF_ACTION("wpn_4", kWPN_4) DEF_ACTION("wpn_5", kWPN_5) DEF_ACTION("wpn_6", kWPN_6) DEF_ACTION("wpn_8", kWPN_8)
                                        DEF_ACTION("laser_on", kLASER_ON) DEF_ACTION("wpn_next", kWPN_NEXT) DEF_ACTION("wpn_fire", kWPN_FIRE) DEF_ACTION("wpn_zoom", kWPN_ZOOM)
                                            DEF_ACTION("wpn_zoom_inc", kWPN_ZOOM_INC) DEF_ACTION("wpn_zoom_dec", kWPN_ZOOM_DEC) DEF_ACTION("wpn_reload", kWPN_RELOAD) DEF_ACTION(
                                                "wpn_func", kWPN_FUNC) DEF_ACTION("wpn_firemode_prev", kWPN_FIREMODE_PREV) DEF_ACTION("wpn_firemode_next", kWPN_FIREMODE_NEXT)

                                                DEF_ACTION("pause", kPAUSE) DEF_ACTION("drop", kDROP) DEF_ACTION("use", kUSE) DEF_ACTION("scores", kSCORES)
                                                    DEF_ACTION("flashlight", kFLASHLIGHT) DEF_ACTION("screenshot", kSCREENSHOT)
                                                        DEF_ACTION("quit", kQUIT) DEF_ACTION("console", kCONSOLE) DEF_ACTION("inventory", kINVENTORY)
                                                            DEF_ACTION("active_jobs", kACTIVE_JOBS)
                                                                DEF_ACTION("map", kMAP) DEF_ACTION("contacts", kCONTACTS) DEF_ACTION("ext_1", kEXT_1)

                                                                            DEF_ACTION("next_slot", kNEXT_SLOT) DEF_ACTION("prev_slot", kPREV_SLOT)

                                                                                                DEF_ACTION("use_bandage", kUSE_BANDAGE) DEF_ACTION("use_medkit", kUSE_MEDKIT)

                                                                                                    DEF_ACTION("quick_save", kQUICK_SAVE) DEF_ACTION("quick_load", kQUICK_LOAD)

                                                                                                        DEF_ACTION("hide_hud", kHIDEHUD) DEF_ACTION("show_hud", kSHOWHUD)
};

xr_vector<_binding> g_key_bindings;

_keyboard keyboards[] = {{"kESCAPE", DIK_ESCAPE},
                         {"k1", DIK_1},
                         {"k2", DIK_2},
                         {"k3", DIK_3},
                         {"k4", DIK_4},
                         {"k5", DIK_5},
                         {"k6", DIK_6},
                         {"k7", DIK_7},
                         {"k8", DIK_8},
                         {"k9", DIK_9},
                         {"k0", DIK_0},
                         {"kMINUS", DIK_MINUS},
                         {"kEQUALS", DIK_EQUALS},
                         {"kBACK", DIK_BACK},
                         {"kTAB", DIK_TAB},
                         {"kQ", DIK_Q},
                         {"kW", DIK_W},
                         {"kE", DIK_E},
                         {"kR", DIK_R},
                         {"kT", DIK_T},
                         {"kY", DIK_Y},
                         {"kU", DIK_U},
                         {"kI", DIK_I},
                         {"kO", DIK_O},
                         {"kP", DIK_P},
                         {"kLBRACKET", DIK_LBRACKET},
                         {"kRBRACKET", DIK_RBRACKET},
                         {"kRETURN", DIK_RETURN},
                         {"kLCONTROL", DIK_LCONTROL},
                         {"kA", DIK_A},
                         {"kS", DIK_S},
                         {"kD", DIK_D},
                         {"kF", DIK_F},
                         {"kG", DIK_G},
                         {"kH", DIK_H},
                         {"kJ", DIK_J},
                         {"kK", DIK_K},
                         {"kL", DIK_L},
                         {"kSEMICOLON", DIK_SEMICOLON},
                         {"kAPOSTROPHE", DIK_APOSTROPHE},
                         {"kGRAVE", DIK_GRAVE},
                         {"kLSHIFT", DIK_LSHIFT},
                         {"kBACKSLASH", DIK_BACKSLASH},
                         {"kZ", DIK_Z},
                         {"kX", DIK_X},
                         {"kC", DIK_C},
                         {"kV", DIK_V},
                         {"kB", DIK_B},
                         {"kN", DIK_N},
                         {"kM", DIK_M},
                         {"kCOMMA", DIK_COMMA},
                         {"kPERIOD", DIK_PERIOD},
                         {"kSLASH", DIK_SLASH},
                         {"kRSHIFT", DIK_RSHIFT},
                         {"kMULTIPLY", DIK_MULTIPLY},
                         {"kLMENU", DIK_LMENU},
                         {"kSPACE", DIK_SPACE},
                         {"kCAPITAL", DIK_CAPITAL},
                         {"kF1", DIK_F1},
                         {"kF2", DIK_F2},
                         {"kF3", DIK_F3},
                         {"kF4", DIK_F4},
                         {"kF5", DIK_F5},
                         {"kF6", DIK_F6},
                         {"kF7", DIK_F7},
                         {"kF8", DIK_F8},
                         {"kF9", DIK_F9},
                         {"kF10", DIK_F10},
                         {"kNUMLOCK", DIK_NUMLOCK},
                         {"kSCROLL", DIK_SCROLL},
                         {"kNUMPAD7", DIK_NUMPAD7},
                         {"kNUMPAD8", DIK_NUMPAD8},
                         {"kNUMPAD9", DIK_NUMPAD9},
                         {"kSUBTRACT", DIK_SUBTRACT},
                         {"kNUMPAD4", DIK_NUMPAD4},
                         {"kNUMPAD5", DIK_NUMPAD5},
                         {"kNUMPAD6", DIK_NUMPAD6},
                         {"kADD", DIK_ADD},
                         {"kNUMPAD1", DIK_NUMPAD1},
                         {"kNUMPAD2", DIK_NUMPAD2},
                         {"kNUMPAD3", DIK_NUMPAD3},
                         {"kNUMPAD0", DIK_NUMPAD0},
                         {"kDECIMAL", DIK_DECIMAL},
                         {"kF11", DIK_F11},
                         {"kF12", DIK_F12},
                         {"kF13", DIK_F13},
                         {"kF14", DIK_F14},
                         {"kF15", DIK_F15},
                         {"kKANA", DIK_KANA},
                         {"kCONVERT", DIK_CONVERT},
                         {"kNOCONVERT", DIK_NOCONVERT},
                         {"kYEN", DIK_YEN},
                         {"kNUMPADEQUALS", DIK_NUMPADEQUALS},
                         {"kCIRCUMFLEX", DIK_CIRCUMFLEX},
                         {"kAT", DIK_AT},
                         {"kCOLON", DIK_COLON},
                         {"kUNDERLINE", DIK_UNDERLINE},
                         {"kKANJI", DIK_KANJI},
                         {"kSTOP", DIK_STOP},
                         {"kAX", DIK_AX},
                         {"kUNLABELED", DIK_UNLABELED},
                         {"kNUMPADENTER", DIK_NUMPADENTER},
                         {"kRCONTROL", DIK_RCONTROL},
                         {"kNUMPADCOMMA", DIK_NUMPADCOMMA},
                         {"kDIVIDE", DIK_DIVIDE},
                         {"kSYSRQ", DIK_SYSRQ},
                         {"kRMENU", DIK_RMENU},
                         {"kHOME", DIK_HOME},
                         {"kUP", DIK_UP},
                         {"kPRIOR", DIK_PRIOR},
                         {"kLEFT", DIK_LEFT},
                         {"kRIGHT", DIK_RIGHT},
                         {"kEND", DIK_END},
                         {"kDOWN", DIK_DOWN},
                         {"kNEXT", DIK_NEXT},
                         {"kINSERT", DIK_INSERT},
                         {"kDELETE", DIK_DELETE},
                         {"kLWIN", DIK_LWIN},
                         {"kRWIN", DIK_RWIN},
                         {"kAPPS", DIK_APPS},
                         {"kPAUSE", DIK_PAUSE},
                         {"mouse1", MOUSE_1},
                         {"mouse2", MOUSE_2},
                         {"mouse3", MOUSE_3},
                         {"mouse4", MOUSE_4},
                         {"mouse5", MOUSE_5},
                         {"mouse6", MOUSE_6},
                         {"mouse7", MOUSE_7},
                         {"mouse8", MOUSE_8},
                         {NULL, 0}};

void initialize_bindings()
{
#ifdef DEBUG
    int i1 = 0;
    while (true)
    {
        _keyboard& _k1 = keyboards[i1];
        if (_k1.key_name == NULL)
            break;
        int i2 = i1;
        while (true)
        {
            _keyboard& _k2 = keyboards[i2];
            if (_k2.key_name == NULL)
                break;
            if (_k1.dik == _k2.dik && i1 != i2)
            {
                Msg("%s==%s", _k1.key_name, _k2.key_name);
            }
            ++i2;
        }
        ++i1;
    }
#endif

    constexpr LPCSTR keyboard_section = "custom_keyboard_action";

    if (pSettings->section_exist(keyboard_section))
    {
        u32 action_count = pSettings->line_count(keyboard_section);

        LPCSTR name;
        LPCSTR value;

        size_t id = actions.size();

        for (u32 i = 0; i < action_count; ++i)
        {
            pSettings->r_line(keyboard_section, i, &name, &value);

            _action n;
            n.id = (EGameActions)id++;
            n.action_name = name;
            n.export_name = value;
            actions.push_back(n);
        }
    }

    // last action
    _action nL;
    nL.id = kLASTACTION;
    nL.action_name = NULL;
    nL.export_name = NULL;
    actions.push_back(nL);

    for (size_t idx = 0; idx < actions.size(); ++idx)
    {
        if (actions[idx].id != kLASTACTION)
        {
            _binding b;
            b.m_action = &actions[idx];
            g_key_bindings.emplace_back(std::move(b));
        }
    }
}

void remap_keys()
{
    int idx = 0;
    string128 buff;
    while (keyboards[idx].key_name)
    {
        buff[0] = 0;
        _keyboard& kb = keyboards[idx];
        bool res = pInput->get_dik_name(kb.dik, buff, 128);
        if (res)
            kb.key_local_name = buff;
        else
            kb.key_local_name = kb.key_name;

        //.		Msg("[%s]-[%s]",kb.key_name, kb.key_local_name.c_str());
        ++idx;
    }
}

LPCSTR id_to_action_name(int _id)
{
    int idx = 0;
    while (actions[idx].action_name)
    {
        if (_id == actions[idx].id)
            return actions[idx].action_name;
        ++idx;
    }
    Msg("! cant find corresponding [action_name] for id");
    return NULL;
}

EGameActions action_name_to_id(LPCSTR _name)
{
    _action* action = action_name_to_ptr(_name);
    if (action)
        return action->id;
    else
        return kNOTBINDED;
}

_action* action_name_to_ptr(LPCSTR _name)
{
    int idx = 0;
    while (actions[idx].action_name)
    {
        if (!stricmp(_name, actions[idx].action_name))
            return &actions[idx];
        ++idx;
    }
    Msg("! cant find corresponding [id] for action_name [%s]", _name);
    return nullptr;
}

LPCSTR dik_to_keyname(int _dik)
{
    _keyboard* kb = dik_to_ptr(_dik, true);
    if (kb)
        return kb->key_name;
    else
        return NULL;
}

_keyboard* dik_to_ptr(int _dik, bool bSafe)
{
    int idx = 0;
    while (keyboards[idx].key_name)
    {
        _keyboard& kb = keyboards[idx];
        if (kb.dik == _dik)
            return &keyboards[idx];
        ++idx;
    }
    if (!bSafe)
        Msg("! cant find corresponding [_keyboard] for dik");
    return NULL;
}

int keyname_to_dik(LPCSTR _name)
{
    _keyboard* kb = keyname_to_ptr(_name);
    if (kb)
        return kb->dik;
    else
        return NULL;
}

_keyboard* keyname_to_ptr(LPCSTR _name)
{
    int idx = 0;
    while (keyboards[idx].key_name)
    {
        _keyboard& kb = keyboards[idx];
        if (!stricmp(_name, kb.key_name))
            return &keyboards[idx];
        ++idx;
    }

    Msg("! cant find corresponding [_keyboard*] for keyname %s", _name);
    return NULL;
}

bool is_binded(EGameActions _action_id, int _dik)
{
    _binding* pbinding = &g_key_bindings.at(_action_id);
    if (pbinding->m_keyboard[0] && pbinding->m_keyboard[0]->dik == _dik)
        return true;

    if (pbinding->m_keyboard[1] && pbinding->m_keyboard[1]->dik == _dik)
        return true;

    return false;
}

int get_action_dik(EGameActions _action_id)
{
    _binding* pbinding = &g_key_bindings.at(_action_id);

    if (pbinding->m_keyboard[0])
        return pbinding->m_keyboard[0]->dik;

    if (pbinding->m_keyboard[1])
        return pbinding->m_keyboard[1]->dik;

    return 0;
}

EGameActions get_binded_action(int _dik)
{
    for (const auto& binding : g_key_bindings)
    {
        if (binding.m_keyboard[0] && binding.m_keyboard[0]->dik == _dik)
            return binding.m_action->id;

        if (binding.m_keyboard[1] && binding.m_keyboard[1]->dik == _dik)
            return binding.m_action->id;
    }
    return kNOTBINDED;
}

void GetActionAllBinding(LPCSTR _action, char* dst_buff, int dst_buff_sz)
{
    const EGameActions action_id = action_name_to_id(_action);

    _binding* pbinding{};
    if (action_id == kNOTBINDED)
    {
        Msg("!![%s] Action [%s] not found! Fix it or remove from text!", __FUNCTION__, _action);
        pbinding = &g_key_bindings.front();
    }
    else
    {
        pbinding = &g_key_bindings.at(action_id);
    }

    string128 prim;
    string128 sec;
    prim[0] = 0;
    sec[0] = 0;

    if (pbinding->m_keyboard[0])
    {
        strcpy_s(prim, pbinding->m_keyboard[0]->key_local_name.c_str());
    }
    if (pbinding->m_keyboard[1])
    {
        strcpy_s(sec, pbinding->m_keyboard[1]->key_local_name.c_str());
    }

    sprintf_s(dst_buff, dst_buff_sz, "%s%s%s", prim[0] ? prim : "", (sec[0] && prim[0]) ? " , " : "", sec[0] ? sec : "");
}

ConsoleBindCmds bindConsoleCmds;
BOOL bRemapped = FALSE;

class CCC_Bind : public IConsole_Command
{
    int m_work_idx;

public:
    CCC_Bind(LPCSTR N, int idx) : IConsole_Command(N), m_work_idx(idx){};
    virtual void Execute(LPCSTR args)
    {
        string256 action;
        string256 key;
        *action = 0;
        *key = 0;

        sscanf(args, "%s %s", action, key);
        if (!*action)
            return;

        if (!*key)
            return;

        if (Core.Features.test(xrCore::Feature::remove_alt_keybinding) && m_work_idx > 0)
            return;

        if (!bRemapped)
        {
            remap_keys();
            bRemapped = TRUE;
        }

        if (!action_name_to_ptr(action))
            return;

        int action_id = action_name_to_id(action);
        if (action_id == kNOTBINDED)
            return;

        _keyboard* pkeyboard = keyname_to_ptr(key);
        if (!pkeyboard)
            return;

        auto& curr_pbinding = g_key_bindings.at(action_id);

        curr_pbinding.m_keyboard[m_work_idx] = pkeyboard;

        for (auto& binding : g_key_bindings)
        {
            if (&binding == &curr_pbinding)
                continue;

            if (binding.m_keyboard[0] == pkeyboard)
                binding.m_keyboard[0] = NULL;

            if (binding.m_keyboard[1] == pkeyboard)
                binding.m_keyboard[1] = NULL;
        }

        CStringTable::ReparseKeyBindings();
    }
    virtual void Save(IWriter* F)
    {
        if (m_work_idx == 0)
            F->w_printf("unbindall\r\n");

        for (const auto& pbinding : g_key_bindings)
            if (pbinding.m_keyboard[m_work_idx])
                F->w_printf("%s %s %s\r\n", cName, pbinding.m_action->action_name, pbinding.m_keyboard[m_work_idx]->key_name);
    }
};

class CCC_UnBind : public IConsole_Command
{
    int m_work_idx;

public:
    CCC_UnBind(LPCSTR N, int idx) : IConsole_Command(N), m_work_idx(idx) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args)
    {
        int action_id = action_name_to_id(args);
        _binding* pbinding = &g_key_bindings.at(action_id);
        pbinding->m_keyboard[m_work_idx] = NULL;

        CStringTable::ReparseKeyBindings();
    }
};

class CCC_ListActions : public IConsole_Command
{
public:
    CCC_ListActions(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };

    virtual void Execute(LPCSTR args)
    {
        Log("- --- Action list start ---");
        for (const auto& pbinding : g_key_bindings)
            Msg("- %s", pbinding.m_action->action_name);
        Log("- --- Action list end   ---");
    }
};

class CCC_UnBindAll : public IConsole_Command
{
public:
    CCC_UnBindAll(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };

    virtual void Execute(LPCSTR args)
    {
        for (auto& pbinding : g_key_bindings)
        {
            pbinding.m_keyboard[0] = NULL;
            pbinding.m_keyboard[1] = NULL;
        }

        bindConsoleCmds.clear();
        //.		Console->Execute("cfg_load default_controls.ltx");

        string_path _cfg;
        string_path cmd;
        FS.update_path(_cfg, "$game_config$", "default_controls.ltx");
        strconcat(sizeof(cmd), cmd, "cfg_load", " ", _cfg);
        Console->Execute(cmd);
    }
};

class CCC_BindList : public IConsole_Command
{
public:
    CCC_BindList(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };

    virtual void Execute(LPCSTR args)
    {
        Log("- --- Bind list start ---");
        string512 buff;

        for (const auto& pbinding : g_key_bindings)
        {
            sprintf_s(buff, "[%s] primary is[%s] secondary is[%s]", pbinding.m_action->action_name,
                      (pbinding.m_keyboard[0]) ? pbinding.m_keyboard[0]->key_local_name.c_str() : "NULL",
                      (pbinding.m_keyboard[1]) ? pbinding.m_keyboard[1]->key_local_name.c_str() : "NULL");
            Log(buff);
        }
        Log("- --- Bind list end   ---");
    }
};

class CCC_BindConsoleCmd : public IConsole_Command
{
public:
    CCC_BindConsoleCmd(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        string512 console_command;
        string256 key;
        int cnt = _GetItemCount(args, ' ');
        _GetItems(args, 0, cnt - 1, console_command, ' ');
        _GetItem(args, cnt - 1, key, ' ');

        int dik = keyname_to_dik(key);
        bindConsoleCmds.bind(dik, console_command);
    }

    virtual void Save(IWriter* F) { bindConsoleCmds.save(F); }
};

class CCC_UnBindConsoleCmd : public IConsole_Command
{
public:
    CCC_UnBindConsoleCmd(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = FALSE; };

    virtual void Execute(LPCSTR args)
    {
        int _dik = keyname_to_dik(args);
        bindConsoleCmds.unbind(_dik);
    }
};

void ConsoleBindCmds::bind(int dik, LPCSTR N)
{
    _conCmd& c = m_bindConsoleCmds[dik];
    c.cmd = N;
}
void ConsoleBindCmds::unbind(int dik)
{
    xr_map<int, _conCmd>::iterator it = m_bindConsoleCmds.find(dik);
    if (it == m_bindConsoleCmds.end())
        return;

    m_bindConsoleCmds.erase(it);
}

void ConsoleBindCmds::clear() { m_bindConsoleCmds.clear(); }

bool ConsoleBindCmds::execute(int dik)
{
    xr_map<int, _conCmd>::iterator it = m_bindConsoleCmds.find(dik);
    if (it == m_bindConsoleCmds.end())
        return false;

    Console->Execute(it->second.cmd.c_str());
    return true;
}

void ConsoleBindCmds::save(IWriter* F)
{
    xr_map<int, _conCmd>::iterator it = m_bindConsoleCmds.begin();

    for (; it != m_bindConsoleCmds.end(); ++it)
    {
        LPCSTR keyname = dik_to_keyname(it->first);
        F->w_printf("bind_console %s %s\n", *it->second.cmd, keyname);
    }
}

void CCC_RegisterInput()
{
    initialize_bindings();
    CMD2(CCC_Bind, "bind", 0);
    CMD2(CCC_Bind, "bind_sec", 1);
    CMD2(CCC_UnBind, "unbind", 0);
    CMD2(CCC_UnBind, "unbind_sec", 1);
    CMD1(CCC_UnBindAll, "unbindall");
    CMD1(CCC_ListActions, "list_actions");

    CMD1(CCC_BindList, "bind_list");
    CMD1(CCC_BindConsoleCmd, "bind_console");
    CMD1(CCC_UnBindConsoleCmd, "unbind_console");
};
