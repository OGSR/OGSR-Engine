#ifndef __XR_HITMARKER_H__
#define __XR_HITMARKER_H__
#pragma once

#include "ui_defs.h"

class CUIStaticItem;
class CLAItem;

struct SHitMark
{
    CUIStaticItem* m_UIStaticItem;
    float m_StartTime;
    float m_HitDirection;
    CLAItem* m_lanim;

    SHitMark(const ui_shader& sh, const Fvector& dir);
    ~SHitMark();
    bool IsActive();
    void UpdateAnim();
    void Draw(float dir);
};

class CHitMarker
{
public:
    FactoryPtr<IUIShader> hShader2;

    typedef xr_deque<SHitMark*> HITMARKS;
    HITMARKS m_HitMarks;

public:
    CHitMarker();
    ~CHitMarker();

    void Render();
    void Hit(int id, const Fvector& dir);
    void InitShader(LPCSTR tex_name);
};

#endif // __XR_HITMARKER_H__
