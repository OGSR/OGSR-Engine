#include "stdafx.h"

#include "imgui.h"

#include "embedded_editor_helper.h"
#include "embedded_editor_weather.h"

#include "../../XR_3DA/Environment.h"
#include "../../XR_3DA/thunderbolt.h"
#include "../../XR_3DA/xr_efflensflare.h"

#include "game_cl_base.h"

#include "GamePersistent.h"
#include "Level.h"

bool editor_override_time{};
bool editor_override_weather{};
bool editor_weather_no_mixer{};
bool editor_override_sun_position{};

namespace
{

bool editor_override_weather_params{};
bool editor_override_sun_position_params{};

float editor_longitude{};
float editor_altitude{};

Fvector2 prev_sun_hp[24]{};
constexpr const char* sun_hour_positions[]{"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"};
static_assert(std::size(prev_sun_hp) == std::size(sun_hour_positions));

xr_set<shared_str> modifiedWeathers;

constexpr Fvector convert(const Fvector& v) { return {v.z, v.y, v.x}; }
constexpr Fvector4 convert(const Fvector4& v) { return {v.z, v.y, v.x, v.w}; }

void saveWeather(const shared_str& name, const xr_vector<CEnvDescriptor*>& env)
{
    CInifile f(nullptr, FALSE, FALSE, FALSE);
    for (auto* el : env)
    {
        if (el->env_ambient)
            f.w_string(el->m_identifier.c_str(), "ambient", el->env_ambient->name().c_str());

        f.w_fvector3(el->m_identifier.c_str(), "ambient_color", el->ambient);
        f.w_fvector4(el->m_identifier.c_str(), "clouds_color", el->clouds_color);
        f.w_string(el->m_identifier.c_str(), "clouds_texture", el->clouds_texture_name.c_str());
        f.w_float(el->m_identifier.c_str(), "far_plane", el->far_plane);
        f.w_float(el->m_identifier.c_str(), "fog_distance", el->fog_distance);
        f.w_float(el->m_identifier.c_str(), "fog_density", el->fog_density);
        f.w_fvector3(el->m_identifier.c_str(), "fog_color", el->fog_color);
        f.w_fvector4(el->m_identifier.c_str(), "hemisphere_color", el->hemi_color);
        f.w_fvector3(el->m_identifier.c_str(), "rain_color", el->rain_color);
        f.w_float(el->m_identifier.c_str(), "rain_density", el->rain_density);
        f.w_fvector3(el->m_identifier.c_str(), "sky_color", el->sky_color);
        f.w_float(el->m_identifier.c_str(), "sky_rotation", rad2deg(el->sky_rotation));
        f.w_string(el->m_identifier.c_str(), "sky_texture", el->sky_texture_name.c_str());
        f.w_string(el->m_identifier.c_str(), "sun", el->lens_flare_id.c_str());
        f.w_fvector3(el->m_identifier.c_str(), "sun_color", el->sun_color);
        f.w_float(el->m_identifier.c_str(), "sun_shafts_intensity", el->m_fSunShaftsIntensity);
        f.w_string(el->m_identifier.c_str(), "thunderbolt_collection", el->tb_id.c_str());
        f.w_float(el->m_identifier.c_str(), "thunderbolt_duration", el->bolt_duration);
        f.w_float(el->m_identifier.c_str(), "thunderbolt_period", el->bolt_period);
        f.w_float(el->m_identifier.c_str(), "wind_direction", rad2deg(el->wind_direction));
        f.w_float(el->m_identifier.c_str(), "wind_velocity", el->wind_velocity);
        f.w_float(el->m_identifier.c_str(), "sun_altitude", rad2deg(el->sun_dir.getH()));
        f.w_float(el->m_identifier.c_str(), "sun_longitude", rad2deg(el->sun_dir.getP()));
    }
    string_path fileName;
    FS.update_path(fileName, "$game_weathers$", name.c_str());
    strconcat(sizeof(fileName), fileName, fileName, ".ltx");
    f.save_as(fileName);
}

void SaveSunPositions(CEnvironment& env)
{
    env.m_sun_pos_config->bReadOnly = false;

    for (int i = 0; i < std::size(prev_sun_hp); i++)
    {
        char sun_identifier[10];
        sprintf_s(sun_identifier, i >= 10 ? "%d:00:00" : "0%d:00:00", i);

        env.m_sun_pos_config->w_float(sun_identifier, "sun_altitude", env.sun_hp[i].x);
        env.m_sun_pos_config->w_float(sun_identifier, "sun_longitude", env.sun_hp[i].y);

        prev_sun_hp[i].set(FLT_MAX, FLT_MAX);
    }

    string_path fileName;
    FS.update_path(fileName, fsgame::game_configs, "environment\\sun_positions.ltx");
    env.m_sun_pos_config->save_as(fileName);

    env.m_sun_pos_config->bReadOnly = true;
}

void ResetSunPositions(CEnvironment& env)
{
    for (int i = 0; i < std::size(prev_sun_hp); i++)
    {
        if (!fsimilar(prev_sun_hp[i].x, FLT_MAX) && !fsimilar(prev_sun_hp[i].y, FLT_MAX))
        {
            env.sun_hp[i].set(prev_sun_hp[i]);
            prev_sun_hp[i].set(FLT_MAX, FLT_MAX);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// from https://www.strchr.com/natural_sorting
////////////////////////////////////////////////////////////////////////////////////////////////////

int count_digits(const char* s)
{
    const char* p = s;
    while (isdigit(*p))
        p++;
    return (int)(p - s);
}

int compare_naturally(const void* a_ptr, const void* b_ptr)
{
    const char* a = (const char*)a_ptr;
    const char* b = (const char*)b_ptr;

    for (;;)
    {
        if (isdigit(*a) && isdigit(*b))
        {
            int a_count = count_digits(a);
            int diff = a_count - count_digits(b);
            if (diff)
                return diff;
            diff = memcmp(a, b, a_count);
            if (diff)
                return diff;
            a += a_count;
            b += a_count;
        }
        if (*a != *b)
            return *a - *b;
        if (*a == '\0')
            return 0;
        a++, b++;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////

bool SelectTexture(const char* label, shared_str& texName)
{
    string1024 tex;
    strcpy_s(tex, texName.data());

    bool changed = false;
    static shared_str prevValue;

    ImGui::PushID(label);
    if (ImGui::InputText("", tex, 100))
    {
        texName = tex;
        changed = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("..."))
    {
        ImGui::OpenPopup("Choose texture");
        prevValue = texName;
    }
    ImGui::SameLine();
    ImGui::Text(label);

    ImGui::SetNextWindowSize(ImVec2(250, 400), ImGuiCond_FirstUseEver);
    if (ImGui::BeginPopupModal("Choose texture", nullptr, 0))
    {
        string_path curr_dir, curr_fileName;
        _splitpath(tex, nullptr, nullptr, curr_fileName, nullptr);
        strconcat(sizeof(fn), curr_fileName, curr_fileName, ".dds");

        _splitpath(prevValue.c_str(), nullptr, curr_dir, nullptr, nullptr);

        ImGui::Text("Current dir: %s", curr_dir);

        static xr_map<xr_string, xr_vector<xr_string>> dirs;
        auto filtered = dirs[curr_dir];
        if (filtered.empty())
        {
            xr_vector<LPSTR>* files = FS.file_list_open("$game_textures$", curr_dir, FS_ListFiles);
            if (files)
            {
                filtered.resize(files->size());
                auto e = std::copy_if(files->begin(), files->end(), filtered.begin(), [](auto x) { return strstr(x, "#small") == nullptr && strstr(x, ".thm") == nullptr; });
                filtered.resize(e - filtered.begin());
                std::sort(filtered.begin(), filtered.end(), [](auto a, auto b) { return compare_naturally(a.c_str(), b.c_str()) < 0; });

                dirs[curr_dir] = filtered;
            }
            FS.file_list_close(files);
        }

        int cur = -1;
        for (size_t i = 0; i != filtered.size(); i++)
        {
            if (filtered[i] == curr_fileName)
            {
                cur = i;
                break;
            }
        }

        if (ImGui_ListBox(
                "", &cur,
                [](void* data, int idx, const char** out_text) -> bool {
                    const auto& textures = *static_cast<xr_vector<xr_string>*>(data);
                    *out_text = textures.at(idx).c_str();
                    return true;
                },
                &filtered, filtered.size(), ImVec2(-4.0f, -30.0f)))
        {
            strconcat(100, tex, curr_dir, filtered[cur].c_str());

            const LPSTR ext = strext(tex);
            if (ext && (0 == _stricmp(ext, ".tga") || 0 == _stricmp(ext, ".dds") || 0 == _stricmp(ext, ".bmp") || 0 == _stricmp(ext, ".ogm")))
            {
                *ext = 0;
            }

            texName = tex;
            changed = true;
        }

        if (ImGui::Button("OK", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();

        if (ImGui::Button("Cancel", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();

            texName = prevValue;
            changed = true;
        }

        ImGui::EndPopup();
    }

    ImGui::PopID();
    return changed;
}

} // namespace

void CImGuiWeatherWnd::Render()
{
    m_Name = modifiedWeathers.empty() ? "Weather###Weather" : "Weather*###Weather";
    
    if (!RenderBegin())
    {
        RenderEnd();
        return;
    }

    //-- why not
    if (!g_pGamePersistent->Environment().USED_COP_WEATHER)
    {
    
        ImGui::Text("Weather editor is only available for COP weather.");
        ImGui::End();
        return;
    }

    CEnvironment& env = GamePersistent().Environment();
    CEnvDescriptor* cur = env.Current[0];

    int sun_index = iFloor(Level().GetEnvironmentGameDayTimeSec() / (DAY_LENGTH / 24));

    const u64 time = Level().GetEnvironmentGameTime() / 1000;
    ImGui::Text("Time: %02d:%02d:%02d", int(time / (60 * 60) % 24), int(time / 60 % 60), int(time % 60));

    float tf = Level().GetGameTimeFactor();
    if (ImGui::SliderFloat("Time factor", &tf, 0.0f, 10000.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
    {
        //Level().SetGameTimeFactor(tf);

        Level().Server->game->SetGameTimeFactor(tf);
        env.SetGameTime(Level().GetEnvironmentGameDayTimeSec(), Level().game->GetEnvironmentGameTimeFactor());
    }

    ImGui::Separator();

    ImGui::Text("Main parameters");
    
    ImGui::Checkbox("Override weather", &editor_override_weather);

    ImGui::BeginDisabled(!editor_override_weather);

    xr_vector<shared_str> cycles;
    cycles.reserve(env.WeatherCycles.size());

    int iCycle = -1;
    for (const auto& el : env.WeatherCycles | std::views::keys)
    {
        cycles.push_back(el);
        if (el == env.GetWeather())
            iCycle = cycles.size() - 1;
    }

    if (ImGui::Combo(
            "Weather cycle", &iCycle,
            [](void* data, int idx) {
                const auto& cycles = *static_cast<xr_vector<shared_str>*>(data);
                return cycles.at(idx).c_str();
            },
            &cycles, cycles.size()))
    {
        env.SetWeather(cycles.at(iCycle), true);
    }

    ImGui::EndDisabled();

    ImGui::Checkbox("Override time", &editor_override_time);

    ImGui::BeginDisabled(!editor_override_time);

    static int sp{};

    int sel = -1;

    for (int i = 0; i != env.CurrentWeather->size(); i++)
        if (cur->m_identifier == env.CurrentWeather->at(i)->m_identifier)
            sel = i;

    if (ImGui::Combo(
            "Current section", &sel,
            [](void* data, int idx) {
                const auto& envs = *static_cast<xr_vector<CEnvDescriptor*>*>(data);
                return envs.at(idx)->m_identifier.c_str();
            },
            env.CurrentWeather, env.CurrentWeather->size()))
    {
        env.SetGameTime(env.CurrentWeather->at(sel)->exec_time + 0.5f, Level().game->GetEnvironmentGameTimeFactor());
        env.SetWeather(cycles.at(iCycle), true);

        if (env.m_static_sun_movement)
        {
            sun_index = iFloor(env.CurrentWeather->at(sel)->exec_time + 0.5f) / (DAY_LENGTH / 24);

            if (editor_override_sun_position)
                sun_index = sp;

            editor_altitude = env.sun_hp[sun_index].x;
            editor_longitude = env.sun_hp[sun_index].y;

            cur->sun_dir.setHP(deg2rad(editor_altitude), deg2rad(editor_longitude));
        }
        else
        {
            editor_altitude = cur->sun_dir.getH();
            editor_longitude = cur->sun_dir.getP();
        }
    }
    else if (editor_override_time && sel != -1)
    {
        sun_index = iFloor(env.CurrentWeather->at(sel)->exec_time + 0.5f) / (DAY_LENGTH / 24);
    }

    ImGui::EndDisabled();

    ImGui::Checkbox("Disable weather mixer", &editor_weather_no_mixer);

    ImGui::Separator();

    ImGui::Text("Ambient light parameters");

    sel = -1;
    for (int i = 0; i != env.m_ambients_config->sections_ordered().size(); i++)
        if (cur->env_ambient->name() == env.m_ambients_config->sections_ordered()[i].second->Name)
            sel = i;

    bool changed = false;
    if (ImGui::Combo(
            "ambient", &sel,
            [](void* data, int idx) {
                const auto* ini = static_cast<CInifile*>(data);
                return ini->sections_ordered().at(idx).second->Name.c_str();
            },
            env.m_ambients_config, env.m_ambients_config->sections_ordered().size()))
    {
        cur->env_ambient = env.AppendEnvAmb(env.m_ambients_config->sections_ordered().at(sel).second->Name);
        changed = true;
    }
    if (ImGui::ColorEdit3("ambient_color", (float*)&cur->ambient))
        changed = true;

    ImGui::Separator();

    ImGui::Text("Clouds parameters");

    if (ImGui::ColorEdit4("clouds_color", (float*)&cur->clouds_color, ImGuiColorEditFlags_AlphaBar))
        changed = true;

    if (SelectTexture("clouds_texture", cur->clouds_texture_name))
    {
        cur->on_unload();
        cur->on_prepare();
        changed = true;
    }

    ImGui::Separator();

    ImGui::Text("Fog parameters");

    if (ImGui::SliderFloat("far_plane", &cur->far_plane, 0.01f, 1000.0f))
        changed = true;
    if (ImGui::SliderFloat("fog_distance", &cur->fog_distance, 0.0f, 1000.0f))
        changed = true;
    if (ImGui::SliderFloat("fog_density", &cur->fog_density, 0.0f, 10.0f))
        changed = true;
    if (ImGui::ColorEdit3("fog_color", (float*)&cur->fog_color))
        changed = true;

    ImGui::Separator();

    ImGui::Text("Hemi parameters");

    if (ImGui::ColorEdit4("hemisphere_color", (float*)&cur->hemi_color, ImGuiColorEditFlags_AlphaBar))
        changed = true;

    ImGui::Separator();

    ImGui::Text("Rain parameters");

    if (ImGui::SliderFloat("rain_density", &cur->rain_density, 0.0f, 1.0f))
        changed = true;
    if (ImGui::ColorEdit3("rain_color", (float*)&cur->rain_color))
        changed = true;

    ImGui::Separator();

    ImGui::Text("Sky parameters");

    Fvector temp;
    temp = convert(cur->sky_color);
    if (ImGui::ColorEdit3("sky_color", (float*)&temp))
        changed = true;
    cur->sky_color = convert(temp);
    if (ImGui::SliderFloat("sky_rotation", &cur->sky_rotation, 0.0f, 6.28318f))
        changed = true;

    if (SelectTexture("sky_texture", cur->sky_texture_name))
    {
        string1024 buf;
        xr_strconcat(buf, cur->sky_texture_name.data(), "#small");
        cur->sky_texture_env_name = buf;
        cur->on_unload();
        cur->on_prepare();
        changed = true;
    }

    ImGui::Separator();

    ImGui::Text("Sun parameters");

    sel = -1;
    for (int i = 0; i != env.m_suns_config->sections_ordered().size(); i++)
        if (cur->lens_flare_id == env.m_suns_config->sections_ordered()[i].second->Name)
            sel = i;

    if (ImGui::Combo(
            "sun", &sel,
            [](void* data, int idx) {
                const auto* ini = static_cast<CInifile*>(data);
                return ini->sections_ordered().at(idx).second->Name.c_str();
            },
            env.m_suns_config, env.m_suns_config->sections_ordered().size()))
    {
        cur->lens_flare_id = env.eff_LensFlare->AppendDef(env, env.m_suns_config->sections_ordered().at(sel).second->Name.c_str());
        env.eff_LensFlare->Invalidate();
        changed = true;
    }
    if (ImGui::ColorEdit3("sun_color", (float*)&cur->sun_color))
        changed = true;

    if (ImGui::SliderFloat("sun_shafts_intensity", &cur->m_fSunShaftsIntensity, 0.0f, 2.0f))
        changed = true;

    if (env.m_static_sun_movement || !env.m_dynamic_sun_movement)
    {
        ImGui::Separator();
        ImGui::Text("Sun position parameters");
    }

    if (!env.m_static_sun_movement && !env.m_dynamic_sun_movement)
    {
        if (ImGui::SliderFloat("sun_altitude", &editor_altitude, -360.0f, 360.0f))
        {
            changed = true;
            cur->sun_dir.setHP(deg2rad(editor_altitude), deg2rad(editor_longitude));
        }
        else
            editor_altitude = cur->sun_dir.getH();

        if (ImGui::SliderFloat("sun_longitude", &editor_longitude, -360.0f, 360.0f))
        {
            changed = true;
            cur->sun_dir.setHP(deg2rad(editor_altitude), deg2rad(editor_longitude));
        }
        else
            editor_longitude = cur->sun_dir.getP();
    }

    if (env.m_static_sun_movement)
    {
        static bool initialized = false;
        if (!initialized)
        {
            for (auto& fv : prev_sun_hp)
                fv.set(FLT_MAX, FLT_MAX);

            initialized = true;
        }

        ImGui::Checkbox("Override sun position", &editor_override_sun_position);

        if (editor_override_sun_position)
        {
            if (ImGui::Combo("Sun Position Index", &sp, sun_hour_positions, std::size(sun_hour_positions)))
            {
                editor_altitude = env.sun_hp[sp].x;
                editor_longitude = env.sun_hp[sp].y;
                cur->sun_dir.setHP(deg2rad(editor_altitude), deg2rad(editor_longitude));
            }

            sun_index = sp;
        }

        ImGui::BeginDisabled(!editor_override_sun_position);

        bool sun_pos_changed{};

        if (ImGui::SliderFloat("sun_altitude", &editor_altitude, -360.0f, 360.0f))
        {
            sun_pos_changed = true;
            if (fsimilar(prev_sun_hp[sun_index].x, FLT_MAX) && fsimilar(prev_sun_hp[sun_index].y, FLT_MAX))
                prev_sun_hp[sun_index].set(env.sun_hp[sun_index]);

            env.sun_hp[sun_index].x = editor_altitude;

            cur->sun_dir.setHP(deg2rad(editor_altitude), deg2rad(editor_longitude));
        }
        else
            editor_altitude = env.sun_hp[sun_index].x;

        if (ImGui::SliderFloat("sun_longitude", &editor_longitude, -360.0f, 360.0f))
        {
            sun_pos_changed = true;
            if (fsimilar(prev_sun_hp[sun_index].x, FLT_MAX) && fsimilar(prev_sun_hp[sun_index].y, FLT_MAX))
                prev_sun_hp[sun_index].set(env.sun_hp[sun_index]);

            env.sun_hp[sun_index].y = editor_longitude;

            cur->sun_dir.setHP(deg2rad(editor_altitude), deg2rad(editor_longitude));
        }
        else
            editor_longitude = env.sun_hp[sun_index].y;

        ImGui::EndDisabled();

        if (sun_pos_changed)
            editor_override_sun_position_params = true;

        if (editor_override_sun_position_params)
        {
            if (ImGui::Button("Save"))
            {
                SaveSunPositions(env);

                editor_override_sun_position_params = false;
            }

            ImGui::SameLine();

            if (ImGui::Button("Reset"))
            {
                ResetSunPositions(env);
                editor_override_sun_position_params = false;
            }
        }
    }

    ImGui::Separator();

    ImGui::Text("Thunder bolt parameters");

    sel = -1;
    for (int i = 0; i < env.m_thunderbolt_collections_config->sections_ordered().size(); i++)
        if (cur->tb_id == env.m_thunderbolt_collections_config->sections_ordered().at(i).second->Name)
            sel = i;

    if (ImGui::Combo(
            "thunderbolt_collection", &sel,
            [](void* data, int idx) {
                if (idx < 0)
                    return "";
                const auto* ini = static_cast<CInifile*>(data);
                return ini->sections_ordered().at(idx).second->Name.c_str();
            },
            env.m_thunderbolt_collections_config, env.m_thunderbolt_collections_config->sections_ordered().size()))
    {
        LPCSTR sect = (sel < 0) ? "" : env.m_thunderbolt_collections_config->sections_ordered().at(sel).second->Name.c_str();

        cur->tb_id = env.eff_Thunderbolt->AppendDef(env, env.m_thunderbolt_collections_config, env.m_thunderbolts_config, sect);
        changed = true;
    }

    if (ImGui::SliderFloat("thunderbolt_duration", &cur->bolt_duration, 0.0f, 2.0f))
        changed = true;
    if (ImGui::SliderFloat("thunderbolt_period", &cur->bolt_period, 0.0f, 10.0f))
        changed = true;

    ImGui::Separator();

    ImGui::Text("Wind parameters");

    if (ImGui::SliderFloat("wind_velocity", &cur->wind_velocity, 0.0f, 1000.0f))
        changed = true;
    if (ImGui::SliderFloat("wind_direction", &cur->wind_direction, 0.0f, 360.0f))
        changed = true;

    ImGui::Separator();

    if (changed)
    {
        modifiedWeathers.insert(env.GetWeather());
        editor_override_weather_params = true;
    }

    if (editor_override_weather_params && ImGui::Button("Save"))
    {
        for (const auto& name : modifiedWeathers)
            saveWeather(name, env.WeatherCycles[name]);

        modifiedWeathers.clear();

        editor_override_weather_params = false;
    }

    ImGui::SameLine();

    if (editor_override_weather_params && ImGui::Button("Reset"))
    {
        editor_override_weather_params = false;

        env.SetWeather(env.GetWeather(), true);        

        modifiedWeathers.clear();
    }

    RenderEnd();
}