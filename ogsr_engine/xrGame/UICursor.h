#pragma once

#include "ui_base.h"
#include "UIStaticItem.h"

class CUIStatic;

class CUICursor : public pureRender
{
    bool bVisible{};
    Fvector2 vPos{}, vPrevPos{};
    std::unique_ptr<CUIStatic> m_static;

    void InitInternal();

public:
    CUICursor();
    virtual ~CUICursor();
    void OnRender() override;

    Fvector2 GetCursorPositionDelta() const;
    Fvector2 GetCursorPosition() const;

    void SetUICursorPosition(const Fvector2& pos);
    void UpdateCursorPosition(const int _dx, const int _dy);

    bool IsVisible() const { return bVisible; }
    void Show() { bVisible = true; }
    void Hide() { bVisible = false; }
};
