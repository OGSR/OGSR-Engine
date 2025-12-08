#include "StdAfx.h"
#include "../xrRender/light.h"
#include "../../xrCDB/cl_intersect.h"

constexpr u32 delay_small_min = 1;
constexpr u32 delay_small_max = 3;

constexpr u32 delay_visible_min = 20;
constexpr u32 delay_visible_max = 40;

int delay_invisible_min{2}, delay_invisible_max{4};

constexpr u32 cullfragments = 4;

void light::vis_prepare(CBackend& cmd_list)
{
    //	. test is sheduled for future	= keep old result
    //	. test time comes :)
    //		. camera inside light volume	= visible,	shedule for 'small' interval
    //		. perform testing				= ???,		pending

    const u32 frame = Device.dwFrame;
    if (frame < vis.frame2test)
        return;

    const float a0 = deg2rad(Device.fFOV * Device.fASPECT / 2.f);
    const float a1 = deg2rad(Device.fFOV / 2.f);
    const float x0 = VIEWPORT_NEAR / _cos(a0);
    const float x1 = VIEWPORT_NEAR / _cos(a1);
    const float c = _sqrt(x0 * x0 + x1 * x1);

    float safe_area = _max(_max(VIEWPORT_NEAR, _max(x0, x1)), c);

    // Msg	("sc[%f,%f,%f]/c[%f,%f,%f] - sr[%f]/r[%f]",VPUSH(spatial.center),VPUSH(position),spatial.radius,range);
    // Msg	("dist:%f, sa:%f",Device.vCameraPosition.distance_to(spatial.center),safe_area);
    bool skiptest = false;
    if (ps_r2_ls_flags.test(R2FLAG_EXP_DONT_TEST_UNSHADOWED) && !flags.bShadow)
        skiptest = true;
    if (ps_r2_ls_flags.test(R2FLAG_EXP_DONT_TEST_SHADOWED) && flags.bShadow)
        skiptest = true;

    vis.distance = Device.vCameraPosition.distance_to(spatial.sphere.P);
    if (skiptest || vis.distance <= (spatial.sphere.R * 1.01f + safe_area))
    {
        // small error
        vis.visible = true;
        vis.pending = false;

        vis.frame2test = frame + ::Random.randI(delay_small_min, delay_small_max);

        return;
    }

    // testing
    vis.pending = true;

    xform_calc();
    cmd_list.set_xform_world(m_xform);

    RImplementation.occq_begin(vis.query_id, cmd_list.context_id);
    //	Hack: Igor. Light is visible if it's frutum is visible. (Only for volumetric)
    //	Hope it won't slow down too much since there's not too much volumetric lights
    //	TODO: sort for performance improvement if this technique hurts
    if ((flags.type == IRender_Light::SPOT) && flags.bShadow && flags.bVolumetric && ps_ssfx_volumetric.x > 0)
        cmd_list.set_Stencil(FALSE);
    else
        cmd_list.set_Stencil(TRUE, D3DCMP_LESSEQUAL, 0x01, 0xff, 0x00);

    RImplementation.Target->draw_volume(cmd_list, this);
    RImplementation.occq_end(vis.query_id, cmd_list.context_id);
}

void light::vis_update()
{
    //	. not pending	->>> return (early out)
    //	. test-result:	visible:
    //		. shedule for 'large' interval
    //	. test-result:	invisible:
    //		. shedule for 'next-frame' interval

    if (!vis.pending)
        return;

    const auto fragments = RImplementation.occq_get(vis.query_id);
    if (fragments == R_occlusion::OCC_CONTINUE_WAIT)
    { // запрещаем vis_prepare, чтобы не создавались новые запросы, пока ждем текущий.
        vis.frame2test = u32(-1);
        return;
    }

    vis.visible = fragments > cullfragments;
    vis.pending = false;

    const u32 frame = Device.dwFrame;

    if (vis.visible)
    {
        vis.frame2test = frame + ::Random.randI(delay_visible_min, delay_visible_max);
    }
    else
    {
        vis.frame2test = frame + ::Random.randI(delay_invisible_min, delay_invisible_max);
    }
}
