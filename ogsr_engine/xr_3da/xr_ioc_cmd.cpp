#include "stdafx.h"
#include "igame_level.h"

//#include "xr_effgamma.h"
#include "x_ray.h"
#include "xr_ioconsole.h"
#include "xr_ioc_cmd.h"
//#include "fbasicvisual.h"
#include "cameramanager.h"
#include "environment.h"
#include "xr_input.h"
#include "CustomHUD.h"
#include <regex>
#include "../Include/xrRender/RenderDeviceRender.h"
#include "xr_object.h"
#include "SkeletonMotions.h"
#include "IGame_Persistent.h"

xr_token* vid_quality_token = nullptr;

constexpr xr_token FpsLockToken[] = {{"nofpslock", 0}, {"fpslock60", 60}, {"fpslock120", 120}, {"fpslock144", 144}, {"fpslock240", 240}, {nullptr, 0}};

#ifdef DEBUG
constexpr xr_token vid_bpp_token[] = {{"16", 16}, {"32", 32}, {0, 0}};
#endif

void IConsole_Command::add_to_LRU(shared_str const& arg)
{
    if (arg.size() == 0 || bEmptyArgsHandled)
    {
        return;
    }

    bool dup = (std::find(m_LRU.begin(), m_LRU.end(), arg) != m_LRU.end());
    if (!dup)
    {
        m_LRU.push_back(arg);
        if (m_LRU.size() > LRU_MAX_COUNT)
        {
            m_LRU.erase(m_LRU.begin());
        }
    }
}

void IConsole_Command::add_LRU_to_tips(vecTips& tips)
{
    vecLRU::reverse_iterator it_rb = m_LRU.rbegin();
    vecLRU::reverse_iterator it_re = m_LRU.rend();
    for (; it_rb != it_re; ++it_rb)
    {
        tips.push_back((*it_rb));
    }
}

// =======================================================

class CCC_Quit : public IConsole_Command
{
public:
    CCC_Quit(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args)
    {
        Console->Hide();
        Engine.Event.Defer("KERNEL:disconnect");
        Engine.Event.Defer("KERNEL:quit");
    }
};
//-----------------------------------------------------------------------
#ifdef DEBUG_MEMORY_MANAGER
class CCC_MemStat : public IConsole_Command
{
public:
    CCC_MemStat(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args)
    {
        string_path fn;
        if (args && args[0])
            xr_sprintf(fn, sizeof(fn), "%s.dump", args);
        else
            strcpy_s_s(fn, sizeof(fn), "x:\\$memory$.dump");
        Memory.mem_statistic(fn);
        //		g_pStringContainer->dump			();
        //		g_pSharedMemoryContainer->dump		();
    }
};
#endif // DEBUG_MEMORY_MANAGER

#ifdef DEBUG_MEMORY_MANAGER
class CCC_DbgMemCheck : public IConsole_Command
{
public:
    CCC_DbgMemCheck(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args)
    {
        if (Memory.debug_mode)
        {
            Memory.dbg_check();
        }
        else
        {
            Msg("~ Run with -mem_debug options.");
        }
    }
};
#endif // DEBUG_MEMORY_MANAGER

class CCC_DbgStrCheck : public IConsole_Command
{
public:
    CCC_DbgStrCheck(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args) { g_pStringContainer->verify(); }
};

class CCC_DbgStrDump : public IConsole_Command
{
public:
    CCC_DbgStrDump(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args) { g_pStringContainer->dump(); }
};

//-----------------------------------------------------------------------
class CCC_MotionsStat : public IConsole_Command
{
public:
    CCC_MotionsStat(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args) { g_pMotionsContainer->dump(); }
};
class CCC_TexturesStat : public IConsole_Command
{
public:
    CCC_TexturesStat(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args)
    {
        Device.DumpResourcesMemoryUsage();
        // Device.Resources->_DumpMemoryUsage();
        //	TODO: move this console commant into renderer
        // VERIFY(0);
    }
};
//-----------------------------------------------------------------------
class CCC_E_Dump : public IConsole_Command
{
public:
    CCC_E_Dump(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args) { Engine.Event.Dump(); }
};
class CCC_E_Signal : public IConsole_Command
{
public:
    CCC_E_Signal(LPCSTR N) : IConsole_Command(N){};
    virtual void Execute(LPCSTR args)
    {
        char Event[128], Param[128];
        Event[0] = 0;
        Param[0] = 0;
        sscanf(args, "%[^,],%s", Event, Param);
        Engine.Event.Signal(Event, (u64)Param);
    }
};

//-----------------------------------------------------------------------
class CCC_Help : public IConsole_Command
{
public:
    CCC_Help(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args)
    {
        Log("- --- Command listing: start ---");
        CConsole::vecCMD_IT it;
        for (it = Console->Commands.begin(); it != Console->Commands.end(); it++)
        {
            IConsole_Command& C = *(it->second);
            TStatus _S;
            C.Status(_S);
            TInfo _I;
            C.Info(_I);

            Msg("%-20s (%-10s) --- %s", C.Name(), _S, _I);
        }
        Log("Key: Ctrl + A         === Select all ");
        Log("Key: Ctrl + C         === Copy to clipboard ");
        Log("Key: Ctrl + V         === Paste from clipboard ");
        Log("Key: Ctrl + X         === Cut to clipboard ");
        Log("Key: Ctrl + Z         === Undo ");
        Log("Key: Ctrl + Insert    === Copy to clipboard ");
        Log("Key: Shift + Insert   === Paste from clipboard ");
        Log("Key: Shift + Delete   === Cut to clipboard ");
        Log("Key: Insert           === Toggle mode <Insert> ");
        Log("Key: Back / Delete          === Delete symbol left / right ");

        Log("Key: Up   / Down            === Prev / Next command in tips list ");
        Log("Key: Ctrl + Up / Ctrl + Down === Prev / Next executing command ");
        Log("Key: Left, Right, Home, End {+Shift/+Ctrl}       === Navigation in text ");
        Log("Key: PageUp / PageDown      === Scrolling history ");
        Log("Key: Tab  / Shift + Tab     === Next / Prev possible command from list");
        Log("Key: Enter  / NumEnter      === Execute current command ");

        Log("- --- Command listing: end ----");
    }
};

XRCORE_API void _dump_open_files(int mode);
class CCC_DumpOpenFiles : public IConsole_Command
{
public:
    CCC_DumpOpenFiles(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = FALSE; };
    virtual void Execute(LPCSTR args)
    {
        int _mode = atoi(args);
        _dump_open_files(_mode);
    }
};

//-----------------------------------------------------------------------
class CCC_SaveCFG : public IConsole_Command
{
public:
    CCC_SaveCFG(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args)
    {
        string_path cfg_full_name;
        xr_strcpy(cfg_full_name, (xr_strlen(args) > 0) ? args : Console->ConfigFile);

        bool b_abs_name = xr_strlen(cfg_full_name) > 2 && cfg_full_name[1] == ':';

        if (!b_abs_name)
            FS.update_path(cfg_full_name, "$app_data_root$", cfg_full_name);

        if (strext(cfg_full_name))
            *strext(cfg_full_name) = 0;
        xr_strcat(cfg_full_name, ".ltx");

        BOOL b_allow = TRUE;
        if (FS.exist(cfg_full_name))
            b_allow = SetFileAttributes(cfg_full_name, FILE_ATTRIBUTE_NORMAL);

        if (b_allow)
        {
            IWriter* F = FS.w_open(cfg_full_name);
            CConsole::vecCMD_IT it;
            for (it = Console->Commands.begin(); it != Console->Commands.end(); it++)
                it->second->Save(F);
            FS.w_close(F);
            Msg("Config-file [%s] saved successfully", cfg_full_name);
        }
        else
            Msg("!Cannot store config file [%s]", cfg_full_name);
    }
};
CCC_LoadCFG::CCC_LoadCFG(LPCSTR N) : IConsole_Command(N){};

void CCC_LoadCFG::Execute(LPCSTR args)
{
    Msg("Executing config-script \"%s\"...", args);
    string_path cfg_name;

    xr_strcpy(cfg_name, args);
    if (strext(cfg_name))
        *strext(cfg_name) = 0;
    xr_strcat(cfg_name, ".ltx");

    string_path cfg_full_name;

    FS.update_path(cfg_full_name, "$app_data_root$", cfg_name);

    // if( NULL == FS.exist(cfg_full_name) )
    //	FS.update_path					(cfg_full_name, "$fs_root$", cfg_name);

    if (NULL == FS.exist(cfg_full_name))
        xr_strcpy(cfg_full_name, cfg_name);

    IReader* F = FS.r_open(cfg_full_name);

    string1024 str;
    if (F)
    {
        while (!F->eof())
        {
            if (F->tell() == 0 && strstr(cfg_full_name, "user.ltx"))
            {
                // Костыль от ситуации когда в редких случаях почему-то у игроков бьётся user.ltx - оказывается набит нулями, в результате чего игра не
                // запускается. Не понятно почему так происходит, поэтому сделал тут обработку такой ситуации.

                if (F->elapsed() >= sizeof(u8))
                {
                    if (F->r_u8() == 0)
                    {
                        Msg("!![%s] file [%s] broken!", __FUNCTION__, cfg_full_name);
                        FS.r_close(F);
                        FS.file_delete(cfg_full_name);
                        return;
                    }

                    F->seek(F->tell() - sizeof(u8));
                }
            }

            F->r_string(str, sizeof(str));
            if (allow(str))
                Console->Execute(str);
        }

        FS.r_close(F);
        Msg("[%s] successfully loaded.", cfg_full_name);
    }
    else
    {
        Msg("! Cannot open script file [%s]", cfg_full_name);
    }
}

CCC_LoadCFG_custom::CCC_LoadCFG_custom(LPCSTR cmd) : CCC_LoadCFG(cmd) { xr_strcpy(m_cmd, cmd); };
bool CCC_LoadCFG_custom::allow(LPCSTR cmd) { return (cmd == strstr(cmd, m_cmd)); };

//-----------------------------------------------------------------------
class CCC_Start : public IConsole_Command
{
private:
    std::string parse(const std::string& str)
    {
        static std::regex Reg("\\(([^)]+)\\)");
        std::smatch results;
        ASSERT_FMT(std::regex_search(str, results, Reg), "Failed parsing string: [%s]", str.c_str());
        return results[1].str();
    }

public:
    CCC_Start(const char* N) : IConsole_Command(N){};
    void Execute(const char* args) override
    {
        auto str = parse(args);
        Engine.Event.Defer("KERNEL:start", u64(xr_strdup(str.c_str())), u64(xr_strdup("localhost")));
    }
};

class CCC_Disconnect : public IConsole_Command
{
public:
    CCC_Disconnect(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args) { Engine.Event.Defer("KERNEL:disconnect"); }
};
//-----------------------------------------------------------------------
class CCC_VID_Reset : public IConsole_Command
{
public:
    CCC_VID_Reset(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args)
    {
        if (Device.b_is_Ready)
        {
            Device.Reset();
        }
    }
};
class CCC_VidMode : public CCC_Token
{
    u32 _dummy{};

public:
    CCC_VidMode(LPCSTR N) : CCC_Token(N, &_dummy, NULL) { bEmptyArgsHandled = FALSE; };
    virtual void Execute(LPCSTR args)
    {
        u32 _w, _h;
        int cnt = sscanf(args, "%dx%d", &_w, &_h);
        if (cnt == 2)
        {
            psCurrentVidMode[0] = _w;
            psCurrentVidMode[1] = _h;
        }
        else
        {
            Msg("! Wrong video mode [%s]", args);
            return;
        }
    }
    virtual void Status(TStatus& S) { xr_sprintf(S, sizeof(S), "%dx%d", psCurrentVidMode[0], psCurrentVidMode[1]); }
    virtual const xr_token* GetToken() override { return vid_mode_token; }
    virtual void Info(TInfo& I) { xr_strcpy(I, sizeof(I), "change screen resolution WxH"); }

    virtual void fill_tips(vecTips& tips, u32 mode)
    {
        TStatus str, cur;
        Status(cur);

        bool res = false;
        const xr_token* tok = GetToken();
        while (tok->name && !res)
        {
            if (!xr_strcmp(tok->name, cur))
            {
                xr_sprintf(str, sizeof(str), "%s  (current)", tok->name);
                tips.push_back(str);
                res = true;
            }
            tok++;
        }
        if (!res)
        {
            tips.push_back("---  (current)");
        }
        tok = GetToken();
        while (tok->name)
        {
            tips.push_back(tok->name);
            tok++;
        }
    }
};
//-----------------------------------------------------------------------
class CCC_SND_Restart : public IConsole_Command
{
public:
    CCC_SND_Restart(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args) { Sound->_restart(); }
};

//-----------------------------------------------------------------------
float ps_gamma = 1.f, ps_brightness = 1.f, ps_contrast = 1.f;
class CCC_Gamma : public CCC_Float
{
public:
    CCC_Gamma(LPCSTR N, float* V) : CCC_Float(N, V, 0.5f, 1.5f) {}

    virtual void Execute(LPCSTR args)
    {
        CCC_Float::Execute(args);
        // Device.Gamma.Gamma		(ps_gamma);
        Device.m_pRender->setGamma(ps_gamma);
        // Device.Gamma.Brightness	(ps_brightness);
        Device.m_pRender->setBrightness(ps_brightness);
        // Device.Gamma.Contrast	(ps_contrast);
        Device.m_pRender->setContrast(ps_contrast);
        // Device.Gamma.Update		();
        Device.m_pRender->updateGamma();
    }
};

//-----------------------------------------------------------------------
/*
#ifdef	DEBUG
extern  INT	g_bDR_LM_UsePointsBBox;
extern	INT	g_bDR_LM_4Steps;
extern	INT g_iDR_LM_Step;
extern	Fvector	g_DR_LM_Min, g_DR_LM_Max;

class CCC_DR_ClearPoint : public IConsole_Command
{
public:
    CCC_DR_ClearPoint(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args) {
        g_DR_LM_Min.x = 1000000.0f;
        g_DR_LM_Min.z = 1000000.0f;

        g_DR_LM_Max.x = -1000000.0f;
        g_DR_LM_Max.z = -1000000.0f;

        Msg("Local BBox (%f, %f) - (%f, %f)", g_DR_LM_Min.x, g_DR_LM_Min.z, g_DR_LM_Max.x, g_DR_LM_Max.z);
    }
};

class CCC_DR_TakePoint : public IConsole_Command
{
public:
    CCC_DR_TakePoint(LPCSTR N) : IConsole_Command(N)	{ bEmptyArgsHandled = TRUE; };
    virtual void Execute(LPCSTR args) {
        Fvector CamPos =  Device.vCameraPosition;

        if (g_DR_LM_Min.x > CamPos.x)	g_DR_LM_Min.x = CamPos.x;
        if (g_DR_LM_Min.z > CamPos.z)	g_DR_LM_Min.z = CamPos.z;

        if (g_DR_LM_Max.x < CamPos.x)	g_DR_LM_Max.x = CamPos.x;
        if (g_DR_LM_Max.z < CamPos.z)	g_DR_LM_Max.z = CamPos.z;

        Msg("Local BBox (%f, %f) - (%f, %f)", g_DR_LM_Min.x, g_DR_LM_Min.z, g_DR_LM_Max.x, g_DR_LM_Max.z);
    }
};

class CCC_DR_UsePoints : public CCC_Integer
{
public:
    CCC_DR_UsePoints(LPCSTR N, int* V, int _min=0, int _max=999) : CCC_Integer(N, V, _min, _max)	{};
    virtual void	Save	(IWriter *F)	{};
};
#endif
*/

#ifdef XRRENDER_STATIC
ENGINE_API BOOL r2_sun_static = FALSE;
ENGINE_API BOOL r2_advanced_pp = TRUE; //	advanced post process and effects
#else
ENGINE_API BOOL r2_sun_static = TRUE;
ENGINE_API BOOL r2_advanced_pp = FALSE; //	advanced post process and effects
#endif

#ifdef EXCLUDE_R1
u32 renderer_value = 1;
#else
u32 renderer_value = 2;
#endif

class CCC_r2 : public CCC_Token
{
    typedef CCC_Token inherited;

public:
    CCC_r2(LPCSTR N) : inherited(N, &renderer_value, nullptr){};
    virtual ~CCC_r2() = default;

    virtual void Execute(LPCSTR args)
    {
        //	vid_quality_token must be already created!
        tokens = vid_quality_token;

        inherited::Execute(args);

#ifndef XRRENDER_STATIC
        Msg("--[%s] Executing renderer: [%s], renderer_value: [%u]", __FUNCTION__, args, renderer_value);
#ifdef EXCLUDE_R1
        //	0..2 - r2
        //	3 - r3
        //	4 - r4
        psDeviceFlags.set(rsR2, ((renderer_value >= 0) && renderer_value < 3));
        psDeviceFlags.set(rsR3, (renderer_value == 3));
        psDeviceFlags.set(rsR4, (renderer_value == 4));

        r2_sun_static = renderer_value == 0;

        r2_advanced_pp = renderer_value >= 2;
#else
        //	0 - r1
        //	1..3 - r2
        //	4 - r3
        psDeviceFlags.set(rsR2, ((renderer_value > 0) && renderer_value < 4));
        psDeviceFlags.set(rsR3, (renderer_value == 4));
        psDeviceFlags.set(rsR4, (renderer_value >= 5));

        r2_sun_static = (renderer_value < 2);

        r2_advanced_pp = (renderer_value >= 3);
#endif
#endif
    }

    virtual void Save(IWriter* F)
    {
        tokens = vid_quality_token;
        inherited::Save(F);
    }
    virtual const xr_token* GetToken() override
    {
        tokens = vid_quality_token;
        return inherited::GetToken();
    }

    virtual void Status(TStatus& S)
    {
        tokens = vid_quality_token;
        inherited::Status(S);
    }
};

class CCC_soundDevice : public CCC_Token
{
    typedef CCC_Token inherited;

public:
    CCC_soundDevice(LPCSTR N) : inherited(N, &snd_device_id, NULL){};
    virtual ~CCC_soundDevice() {}

    virtual void Execute(LPCSTR args)
    {
        GetToken();
        if (!tokens)
            return;
        inherited::Execute(args);
    }

    virtual void Status(TStatus& S)
    {
        GetToken();
        if (!tokens)
            return;
        inherited::Status(S);
    }

    virtual const xr_token* GetToken() override
    {
        tokens = snd_devices_token;
        return inherited::GetToken();
    }

    virtual void Save(IWriter* F)
    {
        GetToken();
        if (!tokens)
            return;
        inherited::Save(F);
    }
};

class CCC_ExclusiveMode : public CCC_Mask
{
    using inherited = CCC_Mask;

public:
    CCC_ExclusiveMode(const char* N, Flags32* V, u32 M) : inherited(N, V, M) {}

    void Execute(const char* args) override
    {
        inherited::Execute(args);

        const bool val = GetValue();
        if (val != pInput->exclusive_mode())
            pInput->exclusive_mode(val);
    }
};

class ENGINE_API CCC_HideConsole : public IConsole_Command
{
public:
    CCC_HideConsole(LPCSTR N) : IConsole_Command(N) { bEmptyArgsHandled = true; }

    virtual void Execute(LPCSTR args) { Console->Hide(); }
    virtual void Status(TStatus& S) { S[0] = 0; }
    virtual void Info(TInfo& I) { xr_sprintf(I, sizeof(I), "hide console"); }
};

ENGINE_API float psHUD_FOV_def = 0.45f;
ENGINE_API float psHUD_FOV = psHUD_FOV_def;

// extern int			psSkeletonUpdate;
extern int rsDVB_Size;
extern int rsDIB_Size;
// extern float		r__dtex_range;

extern int g_ErrorLineCount;

extern float g_fontWidthScale;
extern float g_fontHeightScale;

extern float puddles_drying;
extern float puddles_wetting;

void CCC_Register()
{
    // General
    CMD1(CCC_Help, "help");
    CMD1(CCC_Quit, "quit");
    CMD1(CCC_Start, "start");
    CMD1(CCC_Disconnect, "disconnect");
    CMD1(CCC_SaveCFG, "cfg_save");
    CMD1(CCC_LoadCFG, "cfg_load");

    CMD1(CCC_MotionsStat, "stat_motions");
#ifdef DEBUG
    CMD1(CCC_TexturesStat, "stat_textures");
#endif // DEBUG

#ifdef DEBUG_MEMORY_MANAGER
    CMD1(CCC_MemStat, "dbg_mem_dump");
    CMD1(CCC_DbgMemCheck, "dbg_mem_check");
#endif // DEBUG_MEMORY_MANAGER

#ifdef DEBUG
    CMD3(CCC_Mask, "mt_particles", &psDeviceFlags, mtParticles);

    CMD1(CCC_DbgStrCheck, "dbg_str_check");
    CMD1(CCC_DbgStrDump, "dbg_str_dump");

    CMD3(CCC_Mask, "mt_sound", &psDeviceFlags, mtSound);
    CMD3(CCC_Mask, "mt_physics", &psDeviceFlags, mtPhysics);
    CMD3(CCC_Mask, "mt_network", &psDeviceFlags, mtNetwork);

    // Events
    CMD1(CCC_E_Dump, "e_list");
    CMD1(CCC_E_Signal, "e_signal");

    CMD3(CCC_Mask, "rs_wireframe", &psDeviceFlags, rsWireframe);
    CMD3(CCC_Mask, "rs_clear_bb", &psDeviceFlags, rsClearBB);
    CMD3(CCC_Mask, "rs_occlusion", &psDeviceFlags, rsOcclusion);

    CMD3(CCC_Mask, "rs_detail", &psDeviceFlags, rsDetails);
    // CMD4(CCC_Float,		"r__dtex_range",		&r__dtex_range,		5,		175	);

    CMD3(CCC_Mask, "rs_render_statics", &psDeviceFlags, rsDrawStatic);
    CMD3(CCC_Mask, "rs_render_dynamics", &psDeviceFlags, rsDrawDynamic);
#endif

    // Render device states
    CMD3(CCC_Mask, "rs_always_active", &psDeviceFlags, rsAlwaysActive);
    CMD3(CCC_Token, "r_fps_lock", &g_dwFPSlimit, FpsLockToken);

    CMD3(CCC_Mask, "rs_v_sync", &psDeviceFlags, rsVSync);
    //	CMD3(CCC_Mask,		"rs_disable_objects_as_crows",&psDeviceFlags,	rsDisableObjectsAsCrows	);
    CMD3(CCC_Mask, "rs_fullscreen", &psDeviceFlags, rsFullscreen);
    CMD3(CCC_Mask, "rs_stats", &psDeviceFlags, rsStatistic);

    CMD3(CCC_Mask, "rs_cam_pos", &psDeviceFlags, rsCameraPos);
#ifdef DEBUG
    CMD3(CCC_Mask, "rs_occ_draw", &psDeviceFlags, rsOcclusionDraw);
    CMD3(CCC_Mask, "rs_occ_stats", &psDeviceFlags, rsOcclusionStats);
    // CMD4(CCC_Integer,	"rs_skeleton_update",	&psSkeletonUpdate,	2,		128	);
#endif // DEBUG

//Вместо этих настроек теперь используется ES Color Grading
//    CMD2(CCC_Gamma, "rs_c_gamma", &ps_gamma);
//    CMD2(CCC_Gamma, "rs_c_brightness", &ps_brightness);
//    CMD2(CCC_Gamma, "rs_c_contrast", &ps_contrast);

    //	CMD4(CCC_Integer,	"rs_vb_size",			&rsDVB_Size,		32,		4096);
    //	CMD4(CCC_Integer,	"rs_ib_size",			&rsDIB_Size,		32,		4096);

    CMD3(CCC_Mask, "rs_hw_stats", &psDeviceFlags, rsHWInfo);

    // General video control
    CMD1(CCC_VidMode, "vid_mode");

    CMD1(CCC_VID_Reset, "vid_restart");

    // Sound
    CMD2(CCC_Float, "snd_volume_eff", &psSoundVEffects);
    CMD2(CCC_Float, "snd_volume_music", &psSoundVMusic);
    CMD1(CCC_SND_Restart, "snd_restart");
    CMD3(CCC_Mask, "snd_acceleration", &psSoundFlags, ss_Hardware);
    CMD3(CCC_Mask, "snd_efx", &psSoundFlags, ss_EAX);
    CMD4(CCC_Integer, "snd_targets", &psSoundTargets, 128, 1024);
    CMD4(CCC_Integer, "snd_cache_size", &psSoundCacheSizeMB, 32, 128);
    CMD4(CCC_Float, "snd_linear_fade", &psSoundLinearFadeFactor, 0.1f, 1.f);

#ifdef DEBUG
    CMD3(CCC_Mask, "snd_stats", &g_stats_flags, st_sound);
    CMD3(CCC_Mask, "snd_stats_min_dist", &g_stats_flags, st_sound_min_dist);
    CMD3(CCC_Mask, "snd_stats_max_dist", &g_stats_flags, st_sound_max_dist);
    CMD3(CCC_Mask, "snd_stats_ai_dist", &g_stats_flags, st_sound_ai_dist);
    CMD3(CCC_Mask, "snd_stats_info_name", &g_stats_flags, st_sound_info_name);
    CMD3(CCC_Mask, "snd_stats_info_object", &g_stats_flags, st_sound_info_object);

    CMD4(CCC_Integer, "error_line_count", &g_ErrorLineCount, 6, 1024);
#endif // DEBUG

    // Mouse
    CMD3(CCC_Mask, "mouse_invert", &psMouseInvert, 1);
    psMouseSens = 0.12f;
    CMD4(CCC_Float, "mouse_sens", &psMouseSens, 0.001f, 0.6f);

    // Camera
    CMD4(CCC_Float, "cam_inert", &psCamInert, 0.0f, 0.99f);
    CMD2(CCC_Float, "cam_slide_inert", &psCamSlideInert);

    CMD1(CCC_r2, "renderer");


    CMD1(CCC_soundDevice, "snd_device");

    // psSoundRolloff	= pSettings->r_float	("sound","rolloff");		clamp(psSoundRolloff,			EPS_S,	2.f);
    psSoundOcclusionScale = pSettings->r_float("sound", "occlusion_scale");
    clamp(psSoundOcclusionScale, 0.1f, .5f);

#ifdef DEBUG
    CMD1(CCC_DumpOpenFiles, "dump_open_files");
#endif

    CMD3(CCC_ExclusiveMode, "input_exclusive_mode", &psDeviceFlags, rsExclusiveMode);

    // extern int g_svTextConsoleUpdateRate;
    // CMD4(CCC_Integer, "sv_console_update_rate", &g_svTextConsoleUpdateRate, 1, 100);

    CMD1(CCC_HideConsole, "hide");

#ifdef DEBUG
    extern BOOL debug_destroy;
    CMD4(CCC_Integer, "debug_destroy", &debug_destroy, FALSE, TRUE);
#endif

    CMD4(CCC_Float, "g_font_scale_x", &g_fontWidthScale, 0.2f, 5.0f);
    CMD4(CCC_Float, "g_font_scale_y", &g_fontHeightScale, 0.2f, 5.0f);

    CMD4(CCC_Float, "rain_puddles_drying", &puddles_drying, 0.1f, 20.0f);
    CMD4(CCC_Float, "rain_puddles_wetting", &puddles_wetting, 0.1f, 20.0f);

    CMD4(CCC_Integer, "g_prefetch", &g_prefetch, 0, 1);
};
