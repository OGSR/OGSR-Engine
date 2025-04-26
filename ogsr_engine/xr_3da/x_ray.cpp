//-----------------------------------------------------------------------------
// File: x_ray.cpp
//
// Programmers:
//	Oles		- Oles Shishkovtsov
//	AlexMX		- Alexander Maksimchuk
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"
#include "xr_input.h"
#include "xr_ioconsole.h"
#include "x_ray.h"
#include "std_classes.h"
#include "LightAnimLibrary.h"
#include "../xrcdb/ispatial.h"
#include "ILoadingScreen.h"
#include "DiscordRPC.hpp"
#include "Render.h"
#include "splash.h"

#define CORE_FEATURE_SET(feature, section) Core.Features.set(xrCore::Feature::feature, READ_IF_EXISTS(pSettings, r_bool, section, #feature, false))

ENGINE_API CApplication* pApp{};
ENGINE_API bool IS_OGSR_GA{};
ENGINE_API CInifile* pGameIni{};
int max_load_stage{};

bool use_reshade{};
extern bool init_reshade();
extern void unregister_reshade();

// startup point
void InitEngine()
{
    Engine.Initialize();
    Device.Initialize();
}

void InitSettings()
{
    string_path fname;
    FS.update_path(fname, "$game_config$", "system.ltx");
    pSettings = xr_new<CInifile>(fname, TRUE);
    CHECK_OR_EXIT(!pSettings->sections().empty(), make_string("Cannot find file %s.\nReinstalling application may fix this problem.", fname));

    FS.update_path(fname, "$game_config$", "game.ltx");
    pGameIni = xr_new<CInifile>(fname, TRUE);
    CHECK_OR_EXIT(!pGameIni->sections().empty(), make_string("Cannot find file %s.\nReinstalling application may fix this problem.", fname));

    IS_OGSR_GA = strstr(READ_IF_EXISTS(pSettings, r_string, "mod_ver", "mod_ver", "nullptr"), "OGSR");

    // load custom shader params declarations

    // Simp: сюда добавлены параметры для актуальных шейдеров, в конфигах их держать не практично (потерялся параметр - и шейдер будет работать не правильно.)
    shader_exports.set_custom_params("shader_param_grayscale", {});

    shader_exports.set_custom_params("pnv_color_old", {});
    shader_exports.set_custom_params("pnv_params_old", {});

    shader_exports.set_custom_params("pnv_color", {});

    shader_exports.set_custom_params("heat_vision_steps", {});
    shader_exports.set_custom_params("heat_vision_blurring", {});
    shader_exports.set_custom_params("heat_fade_distance", {});

    shader_exports.set_custom_params("breath_size", {});
    shader_exports.set_custom_params("breath_idx", {});

    shader_exports.set_custom_params("gasmask_inertia", {});
    shader_exports.set_custom_params("device_inertia", {});

    shader_exports.set_custom_params("mark_number", {});
    shader_exports.set_custom_params("mark_color", {});

    shader_exports.set_custom_params("s3ds_param_1", {});
    shader_exports.set_custom_params("s3ds_param_2", {});
    shader_exports.set_custom_params("s3ds_param_3", {});
    shader_exports.set_custom_params("s3ds_param_4", {});
    //

    if (pSettings->section_exist("shader_params_export"))
    {
        Msg("[shader_params_export] section found!!!");

        int tb_count = pSettings->line_count("shader_params_export");
        for (int tb_idx = 0; tb_idx < tb_count; tb_idx++)
        {
            LPCSTR N, V;
            if (pSettings->r_line("shader_params_export", tb_idx, &N, &V))
            {
                if (strstr(N, "_save"))
                    continue;

                shader_exports.set_custom_params(N, Fvector4{});
            }
        }
    }
}
void InitConsole()
{
    Console = xr_new<CConsole>();
    Console->Initialize();

    strcpy_s(Console->ConfigFile, "user.ltx");
    if (strstr(Core.Params, "-ltx "))
    {
        string64 c_name;
        sscanf(strstr(Core.Params, "-ltx ") + 5, "%[^ ] ", c_name);
        strcpy_s(Console->ConfigFile, c_name);
    }

    CORE_FEATURE_SET(colorize_ammo, "dragdrop");
    CORE_FEATURE_SET(colorize_untradable, "dragdrop");
    CORE_FEATURE_SET(equipped_untradable, "dragdrop");
    CORE_FEATURE_SET(select_mode_1342, "dragdrop");
    CORE_FEATURE_SET(highlight_equipped, "dragdrop");
    CORE_FEATURE_SET(af_radiation_immunity_mod, "features");
    CORE_FEATURE_SET(condition_jump_weight_mod, "features");
    CORE_FEATURE_SET(forcibly_equivalent_slots, "features");
    CORE_FEATURE_SET(slots_extend_menu, "features");
    CORE_FEATURE_SET(dynamic_sun_movement, "features");
    CORE_FEATURE_SET(wpn_bobbing, "features");
    CORE_FEATURE_SET(show_inv_item_condition, "features");
    CORE_FEATURE_SET(remove_alt_keybinding, "features");
    CORE_FEATURE_SET(binoc_firing, "features");
    CORE_FEATURE_SET(stop_anim_playing, "features");
    CORE_FEATURE_SET(corpses_collision, "features");
    CORE_FEATURE_SET(more_hide_weapon, "features");
    CORE_FEATURE_SET(keep_inprogress_tasks_only, "features");
    CORE_FEATURE_SET(show_dialog_numbers, "features");
    CORE_FEATURE_SET(objects_radioactive, "features");
    CORE_FEATURE_SET(af_zero_condition, "features");
    CORE_FEATURE_SET(af_satiety, "features");
    CORE_FEATURE_SET(af_psy_health, "features");
    CORE_FEATURE_SET(outfit_af, "features");
    CORE_FEATURE_SET(gd_master_only, "features");
    CORE_FEATURE_SET(scope_textures_autoresize, "features");
    CORE_FEATURE_SET(ogse_new_slots, "features");
    CORE_FEATURE_SET(ogse_wpn_zoom_system, "features");
    CORE_FEATURE_SET(wpn_cost_include_addons, "features");
    CORE_FEATURE_SET(hard_ammo_reload, "features");
    CORE_FEATURE_SET(engine_ammo_repacker, "features");
    CORE_FEATURE_SET(ruck_flag_preferred, "features");
    CORE_FEATURE_SET(old_outfit_slot_style, "features");
    CORE_FEATURE_SET(npc_simplified_shooting, "features");
    CORE_FEATURE_SET(use_trade_deficit_factor, "features");
    CORE_FEATURE_SET(show_objectives_ondemand, "features");
    CORE_FEATURE_SET(pickup_check_overlaped, "features");
    CORE_FEATURE_SET(actor_thirst, "features");
    CORE_FEATURE_SET(autoreload_wpn, "features");
    CORE_FEATURE_SET(no_progress_bar_animation, "features");
    CORE_FEATURE_SET(disable_dialog_break, "features");
    CORE_FEATURE_SET(busy_actor_restrictions, "features");
}

void InitInput() { pInput = xr_new<CInput>(); }
void destroyInput() { xr_delete(pInput); }

void InitSound1() { CSound_manager_interface::_create(0); }

void InitSound2() { CSound_manager_interface::_create(1); }
void destroySound() { CSound_manager_interface::_destroy(); }

void destroySettings()
{
    xr_delete(pSettings);
    xr_delete(pGameIni);
}
void destroyConsole()
{
    //Console->Destroy();
    xr_delete(Console);
}
void destroyEngine()
{
    Device.Destroy();
    Engine.Destroy();
}

void execUserScript()
{
    Console->Execute("unbindall");

    if (FS.exist("$app_data_root$", Console->ConfigFile))
    {
        Console->ExecuteScript(Console->ConfigFile);
    }
    else
    {
        string_path default_full_name;

        FS.update_path(default_full_name, "$game_config$", "rspec_default.ltx");

        Console->ExecuteScript(default_full_name);
    }
}

void Startup()
{
    InitSound1();
    execUserScript();
    InitSound2();

    // ...command line for auto start
    {
        LPCSTR pStartup = strstr(Core.Params, "-start ");
        if (pStartup)
            Console->Execute(pStartup + 1);
    }
    {
        LPCSTR pStartup = strstr(Core.Params, "-load ");
        if (pStartup)
            Console->Execute(pStartup + 1);
    }

    // Initialize APP

    Device.Create();
    LALib.OnCreate();
    pApp = xr_new<CApplication>();
    g_pGamePersistent = (IGame_Persistent*)NEW_INSTANCE(CLSID_GAME_PERSISTANT);
    g_SpatialSpace = xr_new<ISpatial_DB>();
    g_SpatialSpacePhysic = xr_new<ISpatial_DB>();

    Discord.Init();

	// Reshade
#pragma todo("Simp: нужен вообще этот решейд???")
    use_reshade = init_reshade();
    if (use_reshade)
        Msg("--[ReShade]: Loaded compatibility addon");
    else
        Msg("!![ReShade]: ReShade not installed or version too old - didn't load compatibility addon");

    // Main cycle
    Memory.mem_usage();

    Device.Run();

	// Reshade
    if (use_reshade)
        unregister_reshade();

    // Destroy APP
    xr_delete(g_SpatialSpacePhysic);
    xr_delete(g_SpatialSpace);
    xr_delete(g_pGamePersistent);
    xr_delete(pApp);
    Engine.Event.Dump();

    // Destroying
    destroyInput();

    destroySettings();

    LALib.OnDestroy();

    destroyConsole();

    destroySound();

    destroyEngine();
}

constexpr auto dwStickyKeysStructSize = sizeof(STICKYKEYS);
constexpr auto dwFilterKeysStructSize = sizeof(FILTERKEYS);
constexpr auto dwToggleKeysStructSize = sizeof(TOGGLEKEYS);

struct damn_keys_filter
{
    BOOL bScreenSaverState;

    // Sticky & Filter & Toggle keys

    STICKYKEYS StickyKeysStruct;
    FILTERKEYS FilterKeysStruct;
    TOGGLEKEYS ToggleKeysStruct;

    DWORD dwStickyKeysFlags;
    DWORD dwFilterKeysFlags;
    DWORD dwToggleKeysFlags;

    damn_keys_filter()
    {
        // Screen saver stuff

        bScreenSaverState = FALSE;

        // Saveing current state
        SystemParametersInfo(SPI_GETSCREENSAVEACTIVE, 0, (PVOID)&bScreenSaverState, 0);

        if (bScreenSaverState)
            // Disable screensaver
            SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, NULL, 0);

        dwStickyKeysFlags = 0;
        dwFilterKeysFlags = 0;
        dwToggleKeysFlags = 0;

        ZeroMemory(&StickyKeysStruct, dwStickyKeysStructSize);
        ZeroMemory(&FilterKeysStruct, dwFilterKeysStructSize);
        ZeroMemory(&ToggleKeysStruct, dwToggleKeysStructSize);

        StickyKeysStruct.cbSize = dwStickyKeysStructSize;
        FilterKeysStruct.cbSize = dwFilterKeysStructSize;
        ToggleKeysStruct.cbSize = dwToggleKeysStructSize;

        // Saving current state
        SystemParametersInfo(SPI_GETSTICKYKEYS, dwStickyKeysStructSize, (PVOID)&StickyKeysStruct, 0);
        SystemParametersInfo(SPI_GETFILTERKEYS, dwFilterKeysStructSize, (PVOID)&FilterKeysStruct, 0);
        SystemParametersInfo(SPI_GETTOGGLEKEYS, dwToggleKeysStructSize, (PVOID)&ToggleKeysStruct, 0);

        if (StickyKeysStruct.dwFlags & SKF_AVAILABLE)
        {
            // Disable StickyKeys feature
            dwStickyKeysFlags = StickyKeysStruct.dwFlags;
            StickyKeysStruct.dwFlags = 0;
            SystemParametersInfo(SPI_SETSTICKYKEYS, dwStickyKeysStructSize, (PVOID)&StickyKeysStruct, 0);
        }

        if (FilterKeysStruct.dwFlags & FKF_AVAILABLE)
        {
            // Disable FilterKeys feature
            dwFilterKeysFlags = FilterKeysStruct.dwFlags;
            FilterKeysStruct.dwFlags = 0;
            SystemParametersInfo(SPI_SETFILTERKEYS, dwFilterKeysStructSize, (PVOID)&FilterKeysStruct, 0);
        }

        if (ToggleKeysStruct.dwFlags & TKF_AVAILABLE)
        {
            // Disable FilterKeys feature
            dwToggleKeysFlags = ToggleKeysStruct.dwFlags;
            ToggleKeysStruct.dwFlags = 0;
            SystemParametersInfo(SPI_SETTOGGLEKEYS, dwToggleKeysStructSize, (PVOID)&ToggleKeysStruct, 0);
        }
    }

    ~damn_keys_filter()
    {
        if (bScreenSaverState)
            // Restoring screen saver
            SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, TRUE, NULL, 0);

        if (dwStickyKeysFlags)
        {
            // Restore StickyKeys feature
            StickyKeysStruct.dwFlags = dwStickyKeysFlags;
            SystemParametersInfo(SPI_SETSTICKYKEYS, dwStickyKeysStructSize, (PVOID)&StickyKeysStruct, 0);
        }

        if (dwFilterKeysFlags)
        {
            // Restore FilterKeys feature
            FilterKeysStruct.dwFlags = dwFilterKeysFlags;
            SystemParametersInfo(SPI_SETFILTERKEYS, dwFilterKeysStructSize, (PVOID)&FilterKeysStruct, 0);
        }

        if (dwToggleKeysFlags)
        {
            // Restore FilterKeys feature
            ToggleKeysStruct.dwFlags = dwToggleKeysFlags;
            SystemParametersInfo(SPI_SETTOGGLEKEYS, dwToggleKeysStructSize, (PVOID)&ToggleKeysStruct, 0);
        }
    }
};

int APIENTRY WinMain_impl(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow)
{
    HANDLE hCheckPresenceMutex = INVALID_HANDLE_VALUE;
    if (!strstr(lpCmdLine, "-multi_instances"))
    { // Check for another instance
        constexpr const char* STALKER_PRESENCE_MUTEX = "STALKER-SoC";
        hCheckPresenceMutex = OpenMutex(READ_CONTROL, FALSE, STALKER_PRESENCE_MUTEX);
        if (hCheckPresenceMutex == nullptr)
        {
            // New mutex
            hCheckPresenceMutex = CreateMutex(nullptr, FALSE, STALKER_PRESENCE_MUTEX);
            if (hCheckPresenceMutex == nullptr)
            {
                // Shit happens
                return 0;
            }
        }
        else
        {
            // Already running
            CloseHandle(hCheckPresenceMutex);
            return 0;
        }
    }

    Debug._initialize();

    // SetThreadAffinityMask		(GetCurrentThread(),1);

    // Title window

    DisableProcessWindowsGhosting();

    ShowSplash(hInstance);

    LPCSTR fsgame_ltx_name = "-fsltx ";
    string_path fsgame = "";
    if (strstr(lpCmdLine, fsgame_ltx_name))
    {
        int sz = xr_strlen(fsgame_ltx_name);
        sscanf(strstr(lpCmdLine, fsgame_ltx_name) + sz, "%[^ ] ", fsgame);
    }

    Core._initialize("xray", NULL, TRUE, fsgame[0] ? fsgame : NULL);
    InitSettings();

    {
        damn_keys_filter filter;
        (void)filter;

        InitEngine();
        InitInput();
        InitConsole();

        Engine.External.Initialize();
        Console->Execute("stat_memory");
        Startup();
        Core._destroy();

        if (!strstr(lpCmdLine, "-multi_instances")) // Delete application presence mutex
            CloseHandle(hCheckPresenceMutex);
    }
    // here damn_keys_filter class instanse will be destroyed

    return 0;
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow)
{
    gModulesLoaded = true;

    WinMain_impl(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

    ExitFromWinMain = true;

    return 0;
}

CApplication::CApplication() : loadingScreen(nullptr)
{
    ll_dwReference = 0;

    // events
    eQuit = Engine.Event.Handler_Attach("KERNEL:quit", this);
    eStart = Engine.Event.Handler_Attach("KERNEL:start", this);
    eDisconnect = Engine.Event.Handler_Attach("KERNEL:disconnect", this);

    // levels
    Level_Current = 0;
    Level_Scan();

    // Register us
    Device.seqFrame.Add(this, REG_PRIORITY_HIGH + 1000);

    Console->Show();
}

CApplication::~CApplication()
{
    Console->Hide();

    Device.seqFrame.Remove(this);

    // events
    Engine.Event.Handler_Detach(eDisconnect, this);
    Engine.Event.Handler_Detach(eStart, this);
    Engine.Event.Handler_Detach(eQuit, this);
}

void CApplication::OnEvent(EVENT E, u64 P1, u64 P2)
{
    if (E == eQuit)
    {
        PostQuitMessage(0);

        for (auto& Level : Levels)
        {
            xr_free(Level.folder);
        }
    }
    else if (E == eStart)
    {
        LPSTR op_server = LPSTR(P1);
        LPSTR op_client = LPSTR(P2);
        R_ASSERT(0 == g_pGameLevel);
        R_ASSERT(g_pGamePersistent);

        {
            Console->Execute("main_menu off");
            Console->Hide();
            
            g_pGamePersistent->PreStart(op_server);
            
            g_pGameLevel = (IGame_Level*)NEW_INSTANCE(CLSID_GAME_LEVEL);

            pApp->LoadBegin();
            g_pGamePersistent->Start(op_server);
            g_pGameLevel->net_Start(op_server, op_client);
            pApp->LoadEnd();
        }
        xr_free(op_server);
        xr_free(op_client);
    }
    else if (E == eDisconnect)
    {
        if (g_pGameLevel)
        {
            Console->Execute("main_menu off");
            Console->Hide();

            g_pGameLevel->net_Stop();
            xr_delete(g_pGameLevel);

            Console->Show();

            if ((FALSE == Engine.Event.Peek("KERNEL:quit")) && (FALSE == Engine.Event.Peek("KERNEL:start")))
            {
                Console->Execute("main_menu on");
            }
        }

        g_pGamePersistent->Disconnect();
    }
}

static CTimer phase_timer;
extern ENGINE_API BOOL g_appLoaded = FALSE;

void CApplication::LoadBegin(bool quick)
{
    ll_dwReference++;
    if (1 == ll_dwReference)
    {
        g_appLoaded = FALSE;

        phase_timer.Start();

        load_stage = 0;
        if (quick)
            max_load_stage = 4; // при быстром сохранении меньше фаз загрузки. 4 вроде б
        else 
            max_load_stage = 16; // 17; //KRodin: пересчитал кол-во стадий, у нас их 15 при создании НИ + 1 на автопаузу
    }
}

void CApplication::LoadEnd()
{
    ll_dwReference--;
    if (0 == ll_dwReference)
    {
        Msg("* phase time: %d ms", phase_timer.GetElapsed_ms());
        Msg("* phase cmem: %d K", Memory.mem_usage() / 1024);
        Console->Execute("stat_memory");
        g_appLoaded = TRUE;
    }
}

void CApplication::SetLoadingScreen(ILoadingScreen* newScreen)
{
    if (loadingScreen)
    {
        Log("! Trying to create new loading screen, but there is already one..");
        xr_delete(newScreen);
        return;
    }

    loadingScreen = newScreen;
}

void CApplication::DestroyLoadingScreen() { xr_delete(loadingScreen); }

void CApplication::LoadDraw() const
{
    if (g_appLoaded)
        return;
    Device.dwFrame += 1;

    if (!Device.Begin())
        return;

    load_draw_internal();

    Device.End();
}

void CApplication::LoadForceFinish() { loadingScreen->ForceFinish(); }

void CApplication::SetLoadStageTitle(pcstr _ls_title) { loadingScreen->SetStageTitle(_ls_title); }

void CApplication::LoadTitleInt() { loadingScreen->SetStageTip(); }

void CApplication::LoadStage()
{
    VERIFY(ll_dwReference);

    Msg("* phase time: %d ms", phase_timer.GetElapsed_ms());
    phase_timer.Start();
    Msg("* phase cmem: %d K", Memory.mem_usage() / 1024);


    LoadDraw();

    ++load_stage;
    // Msg("--LoadStage is [%d]", load_stage);
}

// Sequential
void CApplication::OnFrame()
{
    ZoneScoped;

    Engine.Event.OnFrame();

    {
        static u32 last_frame{0};

        if (Device.dwFrame > last_frame)
        {
            g_SpatialSpace->update(true);
            g_SpatialSpacePhysic->update(false);

            last_frame = Device.dwFrame + 30;
        }
    }

    if (g_pGameLevel)
        g_pGameLevel->SoundEvent_Dispatch();
}

void CApplication::Level_Append(LPCSTR folder)
{
    string_path N1, N2, N3, N4;
    strconcat(sizeof(N1), N1, folder, "level");
    strconcat(sizeof(N2), N2, folder, "level.ltx");
    strconcat(sizeof(N3), N3, folder, "level.geom");
    strconcat(sizeof(N4), N4, folder, "level.cform");
    if (FS.exist("$game_levels$", N1) && FS.exist("$game_levels$", N2) && FS.exist("$game_levels$", N3) && FS.exist("$game_levels$", N4))
    {
        Levels.emplace_back(sLevelInfo{xr_strdup(folder)});
    }
}

void CApplication::Level_Scan()
{
    xr_vector<char*>* folder = FS.file_list_open("$game_levels$", FS_ListFolders | FS_RootOnly);
    R_ASSERT(folder && folder->size());

    for (auto& i : *folder)
        Level_Append(i);

    FS.file_list_close(folder);

#ifdef DEBUG
    folder = FS.file_list_open("$game_levels$", "$debug$\\", FS_ListFolders | FS_RootOnly);
    if (folder)
    {
        string_path tmp_path;
        for (u32 i = 0; i < folder->size(); i++)
        {
            strconcat(sizeof(tmp_path), tmp_path, "$debug$\\", (*folder)[i]);
            Level_Append(tmp_path);
        }

        FS.file_list_close(folder);
    }
#endif
}

// Taken from OpenXray/xray-16 and refactored
void generate_logo_path(string_path& path, pcstr level_name, int num = -1)
{
    strconcat(sizeof(path), path, "intro\\intro_", level_name);

    if (num < 0)
        return;

    string16 buff;
    xr_strcat(path, sizeof(path), "_");
    xr_strcat(path, sizeof(path), itoa(num + 1, buff, 10));
}

// Taken from OpenXray/xray-16 and refactored
// Return true if logo exists
// Always sets the path even if logo doesn't exist
bool validate_logo_path(string_path& path, pcstr level_name, int num = -1)
{
    generate_logo_path(path, level_name, num);
    string_path temp;
    return FS.exist(temp, "$game_textures$", path, ".dds") || FS.exist(temp, "$level$", path, ".dds");
}

void CApplication::Level_Set(u32 L)
{
    if (L >= Levels.size())
        return;

    Level_Current = L;
    FS.get_path("$level$")->_set(Levels[L].folder);

    std::string temp = Levels[L].folder;
    temp.pop_back();
    const char* level_name = temp.c_str();

    static string_path path;
    path[0] = 0;
    
    int count = 0;
    while (true)
    {
        if (validate_logo_path(path, level_name, count))
            count++;
        else
            break;
    }

    if (count)
    {
        const int curr = ::Random.randI(count);
        generate_logo_path(path, level_name, curr);
    }
    else if (!validate_logo_path(path, level_name))
    {
        if (!validate_logo_path(path, "no_start_picture"))
            path[0] = 0;
    }

    if (path[0])
        loadingScreen->SetLevelLogo(path);

    loadingScreen->SetLevelText(level_name);
}

int CApplication::Level_ID(LPCSTR name)
{
    char buffer[256];
    strconcat(sizeof(buffer), buffer, name, "\\");
    for (u32 I = 0; I < Levels.size(); I++)
    {
        if (0 == stricmp(buffer, Levels[I].folder))
            return int(I);
    }
    return -1;
}

extern void render_reshade_effects();

void CApplication::load_draw_internal() const
{
    if (use_reshade)
        render_reshade_effects();

    loadingScreen->Update(load_stage, max_load_stage);
}

#pragma todo("Simp: нужно ли это? сомневаюсь.")
// Always request high performance GPU
extern "C" {
// https://docs.nvidia.com/gameworks/content/technologies/desktop/optimus.htm
_declspec(dllexport) u32 NvOptimusEnablement = 0x00000001; // NVIDIA Optimus

// https://gpuopen.com/amdpowerxpressrequesthighperformance/
_declspec(dllexport) u32 AmdPowerXpressRequestHighPerformance = 0x00000001; // PowerXpress or Hybrid Graphics
}