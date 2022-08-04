// HUDCrosshair.h:  крестик прицела, отображающий текущую дисперсию
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ui_defs.h"

#define HUD_CURSOR_SECTION "hud_cursor"

class CHUDCrosshair
{
private:
    float cross_length_perc;
    float min_radius_perc;
    float max_radius_perc;

    //текущий радиус прицела
    float radius;
    float target_radius;
    float radius_speed_perc;

    ui_shader hShader;

public:
    u32 cross_color;

    CHUDCrosshair();
    ~CHUDCrosshair();

    void OnRender(const Fvector2& center, const Fvector2& scr_size);
    void SetDispersion(float disp);

    void Load();
};