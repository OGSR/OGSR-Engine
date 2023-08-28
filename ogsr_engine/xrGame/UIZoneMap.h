#pragma once

#include "ui/UIStatic.h"

class CActor;
class CUICustomMap;
//////////////////////////////////////////////////////////////////////////

class CUIZoneMap
{
    CUICustomMap* m_activeMap;
    float m_fScale;

    CUIStatic* m_background{};
    CUIStatic* m_center{};
    CUIStatic* m_compass{};
    CUIStatic* m_clipFrame{};
    CUIStatic* m_pointerDistanceText{};
    CUIStatic* m_clock_wnd{};

    bool m_rounded{};
    u32 m_alpha{};

public:
    CUIZoneMap();
    virtual ~CUIZoneMap();

    void SetHeading(float angle) const;
    void Init();

    void Render();
    void UpdateRadar(Fvector pos) const;

    void SetScale(float s) { m_fScale = s; }
    float GetScale() const { return m_fScale; }

    bool ZoomIn();
    bool ZoomOut();

    void ApplyZoom() const;

    CUIStatic* Background() const { return m_background; };
    CUIStatic* ClipFrame() const { return m_clipFrame; }; // alpet: для экспорта в скрипты
    CUIStatic* Compass() const { return m_compass; }; // alpet: для экспорта в скрипты

    void SetupCurrentMap();
};