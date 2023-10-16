#include "stdafx.h"
#include "torch.h"
#include "script_game_object.h"
#include "inventory_item_object.h"
#include "../xr_3da/LightAnimLibrary.h"

IRender_Light* CTorch::GetLight(int target) const
{
    if (target == 0)
        return light_render._get();
    else if (light_omni)
        return light_omni._get();

    return nullptr;
}

void CTorch::SetAnimation(LPCSTR name) { lanim = LALib.FindItem(name); }

void CTorch::SetBrightness(float brightness)
{
    fBrightness = brightness;
    auto c = m_color;
    c.mul_rgb(fBrightness);
    light_render->set_color(c);
}

void CTorch::SetColor(const Fcolor& color, int target)
{
    switch (target)
    {
    case 0:
        m_color = color;
        light_render->set_color(m_color);
        break;
    case 1:
        if (light_omni)
            light_omni->set_color(color);
        break;
    case 2:
        if (glow_render)
            glow_render->set_color(color);
        break;
    }
}

void CTorch::SetRGB(float r, float g, float b, int target)
{
    Fcolor c;
    c.a = 1;
    c.r = r;
    c.g = g;
    c.b = b;
    SetColor(c, target);
}

void CTorch::SetAngle(float angle, int target)
{
    switch (target)
    {
    case 0: light_render->set_cone(angle); break;
    case 1:
        if (light_omni)
            light_omni->set_cone(angle);
        break;
    }
}

void CTorch::SetRange(float range, int target)
{
    switch (target)
    {
    case 0: {
        light_render->set_range(range);
        calc_m_delta_h(range);
        break;
    }
    case 1:
        if (light_omni)
            light_omni->set_range(range);
        break;
    case 2:
        if (glow_render)
            glow_render->set_radius(range);
        break;
    }
}

void CTorch::SetTexture(LPCSTR texture, int target)
{
    switch (target)
    {
    case 0: light_render->set_texture(texture); break;
    case 1:
        if (light_omni)
            light_omni->set_texture(texture);
        break;
    case 2:
        if (glow_render)
            glow_render->set_texture(texture);
        break;
    }
}
void CTorch::SetVirtualSize(float size, int target)
{
    switch (target)
    {
    case 0: light_render->set_virtual_size(size);
    case 1:
        if (light_omni)
            light_omni->set_virtual_size(size);
    }
}

using namespace luabind;
#pragma optimize("s", on)
void CTorch::script_register(lua_State* L)
{
    module(L)[class_<CTorch, CGameObject /*CInventoryItemObject*/>("CTorch")
                  .def(constructor<>())
                  // alpet: управление параметрами света
                  .def_readonly("on", &CTorch::m_switched_on)
                  .def("enable", (void(CTorch::*)(bool))(&CTorch::Switch))
                  .def("switch", (void(CTorch::*)())(&CTorch::Switch))
                  .def("get_light", &CTorch::GetLight)
                  .def("set_animation", &CTorch::SetAnimation)
                  .def("set_angle", &CTorch::SetAngle)
                  .def("set_brightness", &CTorch::SetBrightness)
                  .def("set_color", &CTorch::SetColor)
                  .def("set_rgb", &CTorch::SetRGB)
                  .def("set_range", &CTorch::SetRange)
                  .def("set_texture", &CTorch::SetTexture)
                  .def("set_virtual_size", &CTorch::SetVirtualSize)
                  // работа с ПНВ
                  .def_readonly("nvd_on", &CTorch::m_bNightVisionOn)
                  .def("enable_nvd", (void(CTorch::*)(bool))(&CTorch::SwitchNightVision))
                  .def("switch_nvd", (void(CTorch::*)())(&CTorch::SwitchNightVision))

                  ,
              def("get_torch_obj", [](CScriptGameObject* script_obj) { return smart_cast<CTorch*>(&script_obj->object()); })];
}
