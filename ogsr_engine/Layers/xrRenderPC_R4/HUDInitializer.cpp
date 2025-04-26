#include "stdafx.h"

#include "HUDInitializer.h"

CHUDTransformHelper::CHUDTransformHelper(CBackend& cmd_list, const bool setup, const bool hud_zero_pos)
    : m_cmd_list(cmd_list)
{
    b_auto_setup = setup;
    b_hud_zero_pos = hud_zero_pos;

    mView_saved = Device.mView;
    mProject_saved = Device.mProject;
    mFullTransform_saved = Device.mFullTransform;

    if (b_auto_setup)
        SetHUDMode();
}

CHUDTransformHelper::~CHUDTransformHelper()
{
    if (b_auto_setup)
        SetDefaultMode();
}

void CHUDTransformHelper::SetHUDMode() const
{
    if (b_hud_zero_pos)
    {
        Device.mView.set(Device.mView_hud);
        Device.mProject.set(Device.mProject_hud);
        Device.mFullTransform.set(Device.mFullTransform_hud);

        m_cmd_list.set_xform_view(Device.mView);
        m_cmd_list.set_xform_project(Device.mProject);

        m_cmd_list.set_xform_view_old(Device.mView_hud_old);
        m_cmd_list.set_xform_project_old(Device.mProject_hud_old);
    }
    else
    {
        Device.mView.set(Device.mView_hud2);
        Device.mProject.set(Device.mProject_hud2);
        Device.mFullTransform.set(Device.mFullTransform_hud2);

        m_cmd_list.set_xform_view(Device.mView);
        m_cmd_list.set_xform_project(Device.mProject);

        m_cmd_list.set_xform_view_old(Device.mView_hud_old2);
        m_cmd_list.set_xform_project_old(Device.mProject_hud_old2);
    }
}

void CHUDTransformHelper::SetDefaultMode() const
{
    Device.mView.set(mView_saved);
    Device.mProject.set(mProject_saved);
    Device.mFullTransform.set(mFullTransform_saved);

    m_cmd_list.set_xform_view(Device.mView);
    m_cmd_list.set_xform_project(Device.mProject);

    m_cmd_list.set_xform_view_old(Device.mView_old);
    m_cmd_list.set_xform_project_old(Device.mProject_old);
}