#pragma once

namespace fsgame
{
constexpr LPCSTR fs_root = "$fs_root$";
constexpr LPCSTR app_root = "$app_root$";
constexpr LPCSTR app_data_root = "$app_data_root$";

constexpr LPCSTR game_data = "$game_data$";
constexpr LPCSTR game_ai = "$game_ai$";
constexpr LPCSTR game_spawn = "$game_spawn$";
constexpr LPCSTR game_anims = "$game_anims$";
constexpr LPCSTR game_levels = "$game_levels$";
constexpr LPCSTR game_meshes = "$game_meshes$";
constexpr LPCSTR game_shaders = "$game_shaders$";
constexpr LPCSTR game_sounds = "$game_sounds$";
constexpr LPCSTR game_textures = "$game_textures$";
constexpr LPCSTR game_scripts = "$game_scripts$";
constexpr LPCSTR game_configs = "$game_config$";
constexpr LPCSTR game_particles = "$game_particles$";
constexpr LPCSTR game_weathers = "$game_weathers$";
constexpr LPCSTR game_weather_effects = "$game_weather_effects$";

constexpr LPCSTR level = "$level$";
constexpr LPCSTR game_saves = "$game_saves$";
constexpr LPCSTR screenshots = "$screenshots$";
constexpr LPCSTR logs = "$logs$";

constexpr LPCSTR mod_dir = "$mod_dir$";

constexpr LPCSTR game_sounds_reference = "$game_sounds_reference$";
constexpr LPCSTR game_textures_reference = "$game_textures_reference$";
constexpr LPCSTR game_meshes_reference = "$game_meshes_reference$";
}

namespace fsgame::level_files
{
constexpr LPCSTR build_lights = "build.lights"; // hemi light

constexpr LPCSTR level = "level"; // main level data - visual, shaders, portals etc

constexpr LPCSTR level_geom = "level.geom";
constexpr LPCSTR level_geomx = "level.geomx";

constexpr LPCSTR level_cform = "level.cform";

constexpr LPCSTR level_graph = "level.graph";
constexpr LPCSTR level_ai = "level.ai";
constexpr LPCSTR level_gct = "level.gct";

constexpr LPCSTR level_details = "level.details";
constexpr LPCSTR level_hom = "level.hom";
constexpr LPCSTR level_fog_vol = "level.fog_vol";
constexpr LPCSTR level_snd_env = "level.snd_env";
constexpr LPCSTR level_som = "level.som";

constexpr LPCSTR level_ps_static = "level.ps_static";
constexpr LPCSTR level_ps_static_ltx = "level.ps_static.ltx";

constexpr LPCSTR level_snd_static = "level.snd_static";
constexpr LPCSTR level_snd_static_ltx = "level.snd_static.ltx";

constexpr LPCSTR level_env_mod = "level.env_mod";
constexpr LPCSTR level_env_mod_ltx = "level.env_mod.ltx";
}

enum FS_List
{
    FS_ListFiles = (1 << 0),
    FS_ListFolders = (1 << 1),
    FS_ClampExt = (1 << 2),
    FS_RootOnly = (1 << 3),
    FS_NoLower = 1 << 4,

    FS_forcedword = u32(-1)
};

class XRCORE_API FS_Path
{
public:
    enum
    {
        flRecurse = (1 << 0),
        flNotif = (1 << 1),
        flNeedRescan = (1 << 2),
    };

public:
    LPSTR m_Path;
    LPSTR m_Root;
    LPSTR m_Add;
    LPSTR m_DefExt;
    Flags32 m_Flags;

public:
    FS_Path(LPCSTR _Root, LPCSTR _Add, LPCSTR _DefExt = nullptr, u32 flags = 0);
    ~FS_Path();
    LPCSTR _update(string_path& dest, LPCSTR src) const;

    void _set(LPSTR add);
    void _set_root(LPSTR root);
};

struct _finddata64i32_t;
#define _FINDDATA_T _finddata64i32_t

struct XRCORE_API FS_File
{
    enum
    {
        flSubDir = (1 << 0),
        flVFS = (1 << 1),
    };
    unsigned attrib{};
    time_t time_write{};
    long size{};
    xr_string name; // low-case name

    void set(const xr_string& nm, long sz, time_t time_write, unsigned attr, const bool lower = true);

public:
    FS_File() {}
    FS_File(const xr_string& nm);
    FS_File(const _FINDDATA_T& f);
    FS_File(const xr_string& nm, const _FINDDATA_T& f);
    FS_File(const xr_string& nm, long sz, time_t time_write, unsigned attr);
    FS_File(const xr_string& nm, long sz, time_t time_write, unsigned attr, const bool lower);

    bool operator<(const FS_File& _X) const { return xr_strcmp(name.c_str(), _X.name.c_str()) < 0; }
};
DEFINE_SET(FS_File, FS_FileSet, FS_FileSetIt);

extern bool XRCORE_API pattern_match(LPCSTR s, LPCSTR mask);
