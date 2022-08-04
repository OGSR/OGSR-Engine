// Tracer.h: interface for the CTracer class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ui_defs.h"

class CBulletManager;

class CTracer
{
    friend CBulletManager;

protected:
    ui_shader sh_Tracer;
    xr_vector<u32> m_aColors;

public:
    CTracer();
    ~CTracer();

    void Render(const Fvector& pos, const Fvector& center, const Fvector& dir, float length, float width, u8 colorID);
};
