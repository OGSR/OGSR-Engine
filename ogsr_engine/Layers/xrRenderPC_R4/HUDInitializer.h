#pragma once

class CHUDTransformHelper
{
    bool b_auto_setup;
    bool b_hud_zero_pos;
    Fmatrix mView_saved;
    Fmatrix mProject_saved;
    Fmatrix mFullTransform_saved;
    CBackend& m_cmd_list;

public:
    CHUDTransformHelper(CBackend& cmd_list_in, bool setup, bool hud_zero_pos = false);
    ~CHUDTransformHelper();

    void SetHUDMode() const;
    void SetDefaultMode() const;
};