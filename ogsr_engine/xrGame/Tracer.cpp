// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:11:06 , by user : Oles , from computer : OLES
// Tracer.cpp: implementation of the CTracer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Tracer.h"
#include "..\xr_3da\render.h"

constexpr u32 MAX_TRACERS = (1024 * 5);
constexpr float TRACER_SIZE = 0.13f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#define TRACERS_COLOR_TABLE "tracers_color_table"
CTracer::CTracer()
{
    const char* sh_name = READ_IF_EXISTS(pSettings, r_string, "bullet_manager", "tracer_shader", "effects\\bullet_tracer");
    const char* tx_name = READ_IF_EXISTS(pSettings, r_string, "bullet_manager", "tracer_texture", "fx\\fx_tracer");
    sh_Tracer->create(sh_name, tx_name);

    m_aColors.clear();
    for (u8 i = 0; i < 255; i++)
    {
        shared_str LineName;
        LineName.sprintf("color_%d", i);
        if (!pSettings->line_exist(TRACERS_COLOR_TABLE, LineName))
            break;
        float r, g, b;
        sscanf(pSettings->r_string(TRACERS_COLOR_TABLE, *LineName), "%f,%f,%f", &r, &g, &b);
        m_aColors.push_back(color_argb_f(1.0f, r, g, b));
    };
}

CTracer::~CTracer() {}

IC void FillSprite_Circle(const Fvector& pos, const float width, const float length, u32 color)
{
    const Fvector& T = Device.vCameraTop;
    const Fvector& R = Device.vCameraRight;
    Fvector Vr, Vt;
    Vr.x = R.x * width;
    Vr.y = R.y * width;
    Vr.z = R.z * width;
    Vt.x = T.x * length;
    Vt.y = T.y * length;
    Vt.z = T.z * length;

    Fvector a, b, c, d;
    a.sub(Vt, Vr);
    b.add(Vt, Vr);
    c.invert(a);
    d.invert(b);

    Fbox2 t_crcl;
    t_crcl.min.set(32.0f / 64.0f, 0.0f);
    t_crcl.max.set(1.0f, 32.0f / 512.0f);

    //	TODO: return code back to indexed rendering since we use quads
    //	Tri 1
    UIRender->PushPoint(d.x + pos.x, d.y + pos.y, d.z + pos.z, color, t_crcl.min.x, t_crcl.max.y); // 0.f,1.f);
    UIRender->PushPoint(a.x + pos.x, a.y + pos.y, a.z + pos.z, color, t_crcl.min.x, t_crcl.min.y); // 0.f,0.f);
    UIRender->PushPoint(c.x + pos.x, c.y + pos.y, c.z + pos.z, color, t_crcl.max.x, t_crcl.max.y); // 1.f,1.f);
                                                                                                   //	Tri 2
    UIRender->PushPoint(c.x + pos.x, c.y + pos.y, c.z + pos.z, color, t_crcl.max.x, t_crcl.max.y); // 1.f,1.f);
    UIRender->PushPoint(a.x + pos.x, a.y + pos.y, a.z + pos.z, color, t_crcl.min.x, t_crcl.min.y); // 0.f,0.f);
    UIRender->PushPoint(b.x + pos.x, b.y + pos.y, b.z + pos.z, color, t_crcl.max.x, t_crcl.min.y); // 1.f,0.f);
}

IC void FillSprite_Line(const Fvector& pos, const Fvector& dir, const float width, const float length, u32 color)
{
    const Fvector& T = dir;
    Fvector R;
    R.crossproduct(T, Device.vCameraDirection).normalize_safe();

    Fvector Vr, Vt;
    Vr.x = R.x * width;
    Vr.y = R.y * width;
    Vr.z = R.z * width;

    Vt.x = T.x * length;
    Vt.y = T.y * length;
    Vt.z = T.z * length;

    Fvector a, b, c, d;
    a.sub(Vt, Vr);
    b.add(Vt, Vr);
    c.invert(a);
    d.invert(b);

    Fbox2 t_tracer;
    t_tracer.min.set(0.0f, 1.0f);
    t_tracer.max.set(16.0f / 64.0f, 0.0f);

    //	TODO: return code back to indexed rendering since we use quads
    //	Tri 1
    UIRender->PushPoint(d.x + pos.x, d.y + pos.y, d.z + pos.z, color, t_tracer.min.x, t_tracer.max.y);
    UIRender->PushPoint(a.x + pos.x, a.y + pos.y, a.z + pos.z, color, t_tracer.min.x, t_tracer.min.y);
    UIRender->PushPoint(c.x + pos.x, c.y + pos.y, c.z + pos.z, color, t_tracer.max.x, t_tracer.max.y);
    //	Tri 2
    UIRender->PushPoint(c.x + pos.x, c.y + pos.y, c.z + pos.z, color, t_tracer.max.x, t_tracer.max.y);
    UIRender->PushPoint(a.x + pos.x, a.y + pos.y, a.z + pos.z, color, t_tracer.min.x, t_tracer.min.y);
    UIRender->PushPoint(b.x + pos.x, b.y + pos.y, b.z + pos.z, color, t_tracer.max.x, t_tracer.min.y);
}

void CTracer::Render(const Fvector& pos, const Fvector& center, const Fvector& dir, float length, float width, u8 colorID)
{
    if (::Render->ViewBase.testSphere_dirty((Fvector&)center, length * .5f))
    {
        if (colorID >= m_aColors.size())
            colorID = 0;
        FillSprite_Circle(pos, width * .5f, width * .5f, m_aColors[colorID]);
        FillSprite_Line(center, dir, width * .5f, length * .5f, m_aColors[colorID]);
    }
}
