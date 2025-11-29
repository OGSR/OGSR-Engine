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

float editor_longitude = 0.0;
float editor_altitude = 0.0;

Fvector convert(const Fvector& v)
{
    Fvector result;
    result.set(v.z, v.y, v.x);
    return result;
}

Fvector4 convert(const Fvector4& v)
{
    Fvector4 result;
    result.set(v.z, v.y, v.x, v.w);
    return result;
}

bool enumCycle(void* data, int idx, const char** item)
{
    xr_vector<shared_str>* cycles = (xr_vector<shared_str>*)data;
    *item = (*cycles)[idx].c_str();
    return true;
}

bool enumWeather(void* data, int idx, const char** item)
{
    xr_vector<CEnvDescriptor*>* envs = (xr_vector<CEnvDescriptor*>*)data;
    *item = (*envs)[idx]->m_identifier.c_str();
    return true;
}

const char* empty = "";

bool enumIniWithEmpty(void* data, int idx, const char** item)
{
    if (idx == 0)
    {
        *item = empty;
    }
    else
    {
        CInifile* ini = (CInifile*)data;
        *item = ini->sections_ordered()[idx - 1].second->Name.c_str();
    }
    return true;
}

bool enumIni(void* data, int idx, const char** item)
{
    CInifile* ini = (CInifile*)data;
    *item = ini->sections_ordered()[idx].second->Name.c_str();
    return true;
}

bool s_ScriptWeather{};
bool s_ScriptTime{};
bool s_ScriptWeatheParams{};
bool s_ScriptNoMixer{};

xr_set<shared_str> modifiedWeathers;

void saveWeather(shared_str name, const xr_vector<CEnvDescriptor*>& env)
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
        f.w_float(el->m_identifier.c_str(), "tree_amplitude_intensity", el->m_fTreeAmplitudeIntensity);
        f.w_float(el->m_identifier.c_str(), "water_intensity", el->m_fWaterIntensity);
        f.w_float(el->m_identifier.c_str(), "wind_direction", rad2deg(el->wind_direction));
        f.w_float(el->m_identifier.c_str(), "wind_velocity", el->wind_velocity);
        f.w_float(el->m_identifier.c_str(), "sun_altitude", rad2deg(el->sun_dir.getH()));
        f.w_float(el->m_identifier.c_str(), "sun_longitude", rad2deg(el->sun_dir.getP()));

        // f.w_float(el->m_identifier.c_str(), "lowland_fog_density", el->lowland_fog_density);
        // f.w_float(el->m_identifier.c_str(), "lowland_fog_height", el->lowland_fog_height);

        // f.w_float(el->m_identifier.c_str(), "swing_normal_amp1", el->m_cSwingDesc[0].amp1);
        // f.w_float(el->m_identifier.c_str(), "swing_normal_amp2", el->m_cSwingDesc[0].amp2);
        // f.w_float(el->m_identifier.c_str(), "swing_normal_rot1", el->m_cSwingDesc[0].rot1);
        // f.w_float(el->m_identifier.c_str(), "swing_normal_rot2", el->m_cSwingDesc[0].rot2);
        // f.w_float(el->m_identifier.c_str(), "swing_normal_speed", el->m_cSwingDesc[0].speed);
        // f.w_float(el->m_identifier.c_str(), "swing_fast_amp1", el->m_cSwingDesc[1].amp1);
        // f.w_float(el->m_identifier.c_str(), "swing_fast_amp2", el->m_cSwingDesc[1].amp2);
        // f.w_float(el->m_identifier.c_str(), "swing_fast_rot1", el->m_cSwingDesc[1].rot1);
        // f.w_float(el->m_identifier.c_str(), "swing_fast_rot2", el->m_cSwingDesc[1].rot2);
        // f.w_float(el->m_identifier.c_str(), "swing_fast_speed", el->m_cSwingDesc[1].speed);
        // f.w_fvector3(el->m_identifier.c_str(), "dof", el->dof_value);
        // f.w_float(el->m_identifier.c_str(), "dof_kernel", el->dof_kernel);
        // f.w_float(el->m_identifier.c_str(), "dof_sky", el->dof_sky);
    }
    string_path fileName;
    FS.update_path(fileName, "$game_weathers$", name.c_str());
    strconcat(sizeof(fileName), fileName, fileName, ".ltx");
    f.save_as(fileName);
}

//void nextTexture(char* tex, int texSize, int offset)
//{
//    string_path dir, fn;
//    _splitpath(tex, nullptr, dir, fn, nullptr);
//    strconcat(sizeof(fn), fn, fn, ".dds");
//    xr_vector<LPSTR>* files = FS.file_list_open("$game_textures$", dir, FS_ListFiles);
//    if (!files)
//        return;
//    size_t index = 0;
//    for (size_t i = 0; i != files->size(); i++)
//        if (strcmp((*files)[i], fn) == 0)
//        {
//            index = i;
//            break;
//        }
//    size_t newIndex = index;
//    while (true)
//    {
//        newIndex = (newIndex + offset + files->size()) % files->size();
//        if (strstr((*files)[newIndex], "#small") == nullptr && strstr((*files)[newIndex], ".thm") == nullptr)
//            break;
//    }
//    string_path newFn;
//    _splitpath((*files)[newIndex], nullptr, nullptr, newFn, nullptr);
//    // strconcat(texSize, tex, dir, newFn);
//    strcpy_s(tex, texSize, dir);
//    strcat_s(tex, texSize, newFn);
//    FS.file_list_close(files);
//}

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

        if (ImGui_ListBox("", &cur,   
            [](void* data, int idx, const char** out_text) -> bool {
                const auto textures = static_cast<xr_vector<xr_string>*>(data);
                *out_text = (*textures)[idx].c_str();
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

void ShowWeatherEditor(bool& show)
{
    if (!ImGui::Begin(modifiedWeathers.empty() ? "Weather###Weather" : "Weather*###Weather", &show))
    {
        ImGui::End();
        return;
    }

    int sel = -1;

    CEnvironment& env = GamePersistent().Environment();
    CEnvDescriptor* cur = env.Current[0];

    u64 time = Level().GetEnvironmentGameTime() / 1000;
    ImGui::Text("Time: %02d:%02d:%02d", int(time / (60 * 60) % 24), int(time / 60 % 60), int(time % 60));

    float tf = Level().GetGameTimeFactor();
    if (ImGui::SliderFloat("Time factor", &tf, 0.0f, 10000.0f, "%.3f", ImGuiSliderFlags_Logarithmic))
    {
        //Level().SetGameTimeFactor(tf);

        Level().Server->game->SetGameTimeFactor(tf);
        env.SetGameTime(Level().GetEnvironmentGameDayTimeSec(), Level().game->GetEnvironmentGameTimeFactor());
    }

    xr_vector<shared_str> cycles;
    int iCycle = -1;
    for (const auto& el : env.WeatherCycles)
    {
        cycles.push_back(el.first);
        if (el.first == env.GetWeather())
            iCycle = cycles.size() - 1;
    }

    ImGui::Text("Main parameters");
    
    ImGui::Checkbox("Script weather", &s_ScriptWeather);

    ImGui::BeginDisabled(!s_ScriptWeather);
    if (ImGui::Combo("Weather cycle", &iCycle, enumCycle, &cycles, env.WeatherCycles.size()))
    {
        env.SetWeather(cycles[iCycle], true);
    }

    ImGui::EndDisabled();

    ImGui::Checkbox("Script time", &s_ScriptTime);

    for (int i = 0; i != env.CurrentWeather->size(); i++)
        if (cur->m_identifier == env.CurrentWeather->at(i)->m_identifier)
            sel = i;

    ImGui::BeginDisabled(!s_ScriptTime);
    if (ImGui::Combo("Current section", &sel, enumWeather, env.CurrentWeather, env.CurrentWeather->size()))
    {
        env.SetGameTime(env.CurrentWeather->at(sel)->exec_time + 0.5f, Level().game->GetEnvironmentGameTimeFactor());
        env.SetWeather(cycles[iCycle], true);
    }
    ImGui::EndDisabled();

    ImGui::Separator();

    ImGui::Checkbox("Disable weather mixer", &s_ScriptNoMixer);

    bool changed = false;
    sel = -1;

    ImGui::Text("Ambient light parameters");

    for (int i = 0; i != env.m_ambients_config->sections_ordered().size(); i++)
        if (cur->env_ambient->name() == env.m_ambients_config->sections_ordered()[i].second->Name)
            sel = i;

    if (ImGui::Combo("ambient", &sel, enumIni, env.m_ambients_config, env.m_ambients_config->sections_ordered().size()))
    {
        cur->env_ambient = env.AppendEnvAmb(env.m_ambients_config->sections_ordered()[sel].second->Name);
        changed = true;
    }
    if (ImGui::ColorEdit3("ambient_color", (float*)&cur->ambient))
        changed = true;

    ImGui::Text("Clouds parameters");

    if (ImGui::ColorEdit4("clouds_color", (float*)&cur->clouds_color, ImGuiColorEditFlags_AlphaBar))
        changed = true;

    if (SelectTexture("clouds_texture", cur->clouds_texture_name))
    {
        cur->on_unload();
        cur->on_prepare();
        changed = true;
    }

    ImGui::Text("Fog parameters");

    if (ImGui::SliderFloat("far_plane", &cur->far_plane, 0.01f, 10000.0f))
        changed = true;
    if (ImGui::SliderFloat("fog_distance", &cur->fog_distance, 0.0f, 10000.0f))
        changed = true;
    if (ImGui::SliderFloat("fog_density", &cur->fog_density, 0.0f, 10.0f))
        changed = true;
    if (ImGui::ColorEdit3("fog_color", (float*)&cur->fog_color))
        changed = true;

    // if (ImGui::SliderFloat("lowland_fog_density", &cur->lowland_fog_density, 0.0f, 5.0f))
    //     changed = true;
    // if (ImGui::SliderFloat("lowland_fog_height", &cur->lowland_fog_height, 0.0f, 100.0f))
    //     changed = true;

    ImGui::Text("Hemi parameters");

    if (ImGui::ColorEdit4("hemisphere_color", (float*)&cur->hemi_color, ImGuiColorEditFlags_AlphaBar))
        changed = true;

    ImGui::Text("Rain parameters");

    if (ImGui::SliderFloat("rain_density", &cur->rain_density, 0.0f, 1.0f))
        changed = true;
    if (ImGui::ColorEdit3("rain_color", (float*)&cur->rain_color))
        changed = true;

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

    ImGui::Text("Sun parameters");

    sel = -1;
    for (int i = 0; i != env.m_suns_config->sections_ordered().size(); i++)
        if (cur->lens_flare_id == env.m_suns_config->sections_ordered()[i].second->Name)
            sel = i;

    if (ImGui::Combo("sun", &sel, enumIni, env.m_suns_config, env.m_suns_config->sections_ordered().size()))
    {
        cur->lens_flare_id = env.eff_LensFlare->AppendDef(env, env.m_suns_config->sections_ordered()[sel].second->Name.c_str());
        env.eff_LensFlare->Invalidate();
        changed = true;
    }
    if (ImGui::ColorEdit3("sun_color", (float*)&cur->sun_color))
        changed = true;

    if (ImGui::SliderFloat("sun_altitude", &editor_altitude, -360.0f, 360.0f))
    {
        changed = true;
        if (changed)
            cur->sun_dir.setHP(deg2rad(editor_longitude), deg2rad(editor_altitude));
        else
            editor_altitude = cur->sun_dir.getH();
    }
    if (ImGui::SliderFloat("sun_longitude", &editor_longitude, -360.0f, 360.0f))
    {
        changed = true;
        if (changed)
            cur->sun_dir.setHP(deg2rad(editor_longitude), deg2rad(editor_altitude));
    }

    if (ImGui::SliderFloat("sun_shafts_intensity", &cur->m_fSunShaftsIntensity, 0.0f, 2.0f))
        changed = true;

    sel = 0;
    for (int i = 0; i != env.m_thunderbolt_collections_config->sections_ordered().size(); i++)
        if (cur->tb_id == env.m_thunderbolt_collections_config->sections_ordered()[i].second->Name)
            sel = i + 1;

    ImGui::Text("Thunder bolt parameters");

    if (ImGui::Combo("thunderbolt_collection", &sel, enumIniWithEmpty, env.m_thunderbolt_collections_config, env.m_thunderbolt_collections_config->sections_ordered().size() + 1))
    {
        LPCSTR sect = (sel == 0) ? "" : env.m_thunderbolt_collections_config->sections_ordered()[sel - 1].second->Name.c_str();

        cur->tb_id = env.eff_Thunderbolt->AppendDef(env, env.m_thunderbolt_collections_config, env.m_thunderbolts_config, sect);
        changed = true;
    }

    if (ImGui::SliderFloat("thunderbolt_duration", &cur->bolt_duration, 0.0f, 2.0f))
        changed = true;
    if (ImGui::SliderFloat("thunderbolt_period", &cur->bolt_period, 0.0f, 10.0f))
        changed = true;

    ImGui::Text("Water parameters");

    if (ImGui::SliderFloat("water_intensity", &cur->m_fWaterIntensity, 0.0f, 2.0f))
        changed = true;

    ImGui::Text("Wind parameters");

    if (ImGui::SliderFloat("wind_velocity", &cur->wind_velocity, 0.0f, 1000.0f))
        changed = true;
    if (ImGui::SliderFloat("wind_direction", &cur->wind_direction, 0.0f, 360.0f))
        changed = true;

    ImGui::Text("Trees parameters");

    if (ImGui::SliderFloat("trees_amplitude_intensity", &cur->m_fTreeAmplitudeIntensity, 0.01f, 0.250f))
        changed = true;

    //ImGui::Text("Grass swing parameters");

    // if (ImGui::SliderFloat("swing_normal_amp1", &cur->m_cSwingDesc[0].amp1, 0.0f, 10.0f))
    //     changed = true;
    // if (ImGui::SliderFloat("swing_normal_amp2", &cur->m_cSwingDesc[0].amp2, 0.0f, 10.0f))
    //     changed = true;
    // if (ImGui::SliderFloat("swing_normal_rot1", &cur->m_cSwingDesc[0].rot1, 0.0f, 300.0f))
    //     changed = true;
    // if (ImGui::SliderFloat("swing_normal_rot2", &cur->m_cSwingDesc[0].rot2, 0.0f, 300.0f))
    //     changed = true;
    // if (ImGui::SliderFloat("swing_normal_speed", &cur->m_cSwingDesc[0].speed, 0.0f, 10.0f))
    //     changed = true;
    // if (ImGui::SliderFloat("swing_fast_amp1", &cur->m_cSwingDesc[1].amp1, 0.0f, 10.0f))
    //     changed = true;
    // if (ImGui::SliderFloat("swing_fast_amp2", &cur->m_cSwingDesc[1].amp2, 0.0f, 10.0f))
    //     changed = true;
    // if (ImGui::SliderFloat("swing_fast_rot1", &cur->m_cSwingDesc[1].rot1, 0.0f, 300.0f))
    //     changed = true;
    // if (ImGui::SliderFloat("swing_fast_rot2", &cur->m_cSwingDesc[1].rot2, 0.0f, 300.0f))
    //     changed = true;
    // if (ImGui::SliderFloat("swing_fast_speed", &cur->m_cSwingDesc[1].speed, 0.0f, 10.0f))
    //     changed = true;

    //ImGui::Text("DoF parameters");

    // if (ImGui::InputFloat3("dof", (float*)&cur->dof_value), 3)
    //     changed = true;
    // if (ImGui::SliderFloat("dof_kernel", &cur->dof_kernel, 0.0f, 10.0f))
    //     changed = true;
    // if (ImGui::SliderFloat("dof_sky", &cur->dof_sky, -10000.0f, 10000.0f))
    //     changed = true;

    if (changed)
    {
        modifiedWeathers.insert(env.GetWeather());
        s_ScriptWeatheParams = true;
    }

    if (s_ScriptWeatheParams && ImGui::Button("Save"))
    {
        for (auto& name : modifiedWeathers)
            saveWeather(name, env.WeatherCycles[name]);

        modifiedWeathers.clear();

        s_ScriptWeatheParams = false;
    }

    ImGui::SameLine();

    if (s_ScriptWeatheParams && ImGui::Button("Reset"))
    {
        s_ScriptWeatheParams = false;

        env.SetWeather(env.GetWeather(), true);        

        modifiedWeathers.clear();
    }

    ImGui::End();
}