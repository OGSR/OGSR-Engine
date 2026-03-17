#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"

#include "x_ray.h"
#include "std_classes.h"
#include "customHUD.h"
#include "render.h"
#include "gamefont.h"
#include "xrLevel.h"
#include "CameraManager.h"
#include "xr_object.h"

ENGINE_API IGame_Level* g_pGameLevel = NULL;

IGame_Level::IGame_Level()
{
    m_pCameras = xr_new<CCameraManager>(true);
    pLevel = nullptr;
    bReady = false;
    pCurrentEntity = nullptr;
    pCurrentViewEntity = nullptr;

    g_pGameLevel = this; // that is for pure_relcase
}

//#include "resourcemanager.h"

IGame_Level::~IGame_Level()
{
    xr_delete(pLevel);

    // Render-level unload
    Render->level_Unload();
    xr_delete(m_pCameras);

    // Unregister
    Device.seqRender.Remove(this);
    Device.seqFrame.Remove(this);
    CCameraManager::ResetPP();

    g_pGameLevel = nullptr;

    xr_delete(g_hud);
}

void IGame_Level::net_Stop()
{
    for (int i = 0; i < 6; i++)
        Objects.Update(true); //Simp: true тут важно, не форсированный апдейт обджектлиста должен быть только на апдейте. в Level_network.cpp тоже должно быть true везде!

    // Destroy all objects
    Objects.Unload();
    R_ASSERT(Objects.o_count() == 0, "Objects not empty on level unload!");

    IR_Release();

    bReady = false;
}

//-------------------------------------------------------------------------------------------

BOOL IGame_Level::Load(u32 dwNum)
{
    // Initialize level data
    string_path temp;
    if (!FS.exist(temp, fsgame::level, fsgame::level_files::level_ltx))
        FATAL("Can't find level configuration file '%s'.", temp);
    pLevel = xr_new<CInifile>(temp);

    // Open
    g_pGamePersistent->LoadTitle("st_opening_stream");
    IReader* LL_Stream = FS.r_open(fsgame::level, fsgame::level_files::level);
    IReader& fs = *LL_Stream;

    // Header
    hdrLEVEL H;
    fs.r_chunk_safe(fsL_HEADER, &H, sizeof(H));
    R_ASSERT(XRCL_PRODUCTION_VERSION == H.XRLC_version, "Incompatible level version.");

    // CForms
    g_pGamePersistent->LoadTitle("st_loading_cform");
    ObjectSpace.Load();

    if (!g_hud)
        g_hud = (CCustomHUD*)NEW_INSTANCE(CLSID_HUDMANAGER);

    Render->level_Load(LL_Stream);
    //  Msg						("* S-CREATE: %f ms, %d times",tscreate.result,tscreate.count);

    // Objects
    g_pGamePersistent->Environment().mods_load();
    R_ASSERT(Load_GameSpecific_Before());
    Objects.Load();

    // Done
    FS.r_close(LL_Stream);
    bReady = true;
    IR_Capture();
    Device.seqRender.Add(this);
    Device.seqFrame.Add(this);

    return TRUE;
}

void IGame_Level::OnRender()
{
}

void IGame_Level::OnFrame()
{
    ZoneScoped;

    // Update all objects
    VERIFY(bReady);
    Objects.Update(false);
    g_hud->OnFrame();

    // Ambience
    if (Sounds_Random.size() && (Device.dwTimeGlobal > Sounds_Random_dwNextTime))
    {
        Sounds_Random_dwNextTime = Device.dwTimeGlobal + ::Random.randI(10000, 20000);
        Fvector pos;
        pos.random_dir().normalize().mul(::Random.randF(30, 100)).add(Device.vCameraPosition);
        int id = ::Random.randI(Sounds_Random.size());
        if (Sounds_Random_Enabled)
        {
            Sounds_Random[id].play_at_pos(nullptr, pos, 0);
            Sounds_Random[id].set_volume(1.f);
            Sounds_Random[id].set_range(10, 200);
        }
    }
}

void IGame_Level::SetEntity(CObject* O)
{
    if (pCurrentEntity)
        pCurrentEntity->On_LostEntity();

    if (O)
        O->On_SetEntity();

    pCurrentEntity = pCurrentViewEntity = O;
}

void IGame_Level::SetViewEntity(CObject* O)
{
    if (pCurrentViewEntity)
        pCurrentViewEntity->On_LostEntity();

    if (O)
        O->On_SetEntity();

    pCurrentViewEntity = O;
}