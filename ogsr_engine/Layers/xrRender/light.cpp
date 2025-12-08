#include "StdAfx.h"
#include "light.h"

constexpr float RSQRTDIV2 = 0.70710678118654752440084436210485f;

light::light(void) : ISpatial(g_SpatialSpace)
{
    spatial.type = STYPE_LIGHTSOURCE;
    spatial.dbg_name = "light";
    flags.type = POINT;

    flags.bStatic = false;
    flags.bActive = false;
    flags.bShadow = false;
    flags.bVolumetric = false;
    flags.bHudMode = false;
    flags.bFlare = false;

    position.set(0, -1000, 0);
    direction.set(0, -1, 0);
    right.set(0, 0, 0);
    range = 8.f;
    cone = deg2rad(60.f);
    color.set(1, 1, 1, 1);

    m_volumetric_quality = 1;
    // m_volumetric_quality	= 0.5;
    m_volumetric_intensity = 0.3;
    m_volumetric_distance = 1;

    fBlend = 0;

    frame_render = 0;

    virtual_size = .1f; // Ray Twitty (aka Shadows): по умолчанию надо 0.1, чтобы не пришлось вызывать для каждого лайта установку виртуального размера
    ZeroMemory(omnipart, sizeof(omnipart));

    s_spot = nullptr;
    s_point = nullptr;

    vis.frame2test = 0; // xffffffff;
    vis.query_id = 0;
    vis.visible = true;
    vis.pending = false;

    // TODO cmd_list ??

    for (int id = 0; id < R__NUM_CONTEXTS; ++id)
        svis[id].context_id = id;
}

light::~light()
{
    set_active(false);

    for (auto& f : omnipart)
        xr_delete(f);

    // remove from Lights_LastFrame

    for (auto& it : RImplementation.Lights_LastFrame)
    {
        if (it == this)
        {
            it = nullptr;
            break;
        }
    }

    if (!ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_SMAPVIS) && !get_moveable())
    {
        for (auto& svi : svis)
            svi.resetoccq();
    }

    if (vis.pending)
    {
        RImplementation.occq_free(vis.query_id, true);
    }

    FlareRayPick.Discard();
}

void light::set_texture(LPCSTR name)
{
    ZoneScoped;

    if ((nullptr == name) || (0 == name[0]))
    {
        // default shaders
        s_spot.destroy();
        s_point.destroy();
        s_volumetric.destroy();
        return;
    }

    s_spot = GetCachedShader("accum_spot", name);
    s_point = GetCachedShader("accum_omni", name);
    s_volumetric = GetCachedShader("accum_volumetric", name);
}

void light::set_active(bool b)
{
    if (b)
    {
        if (flags.bActive)
            return;
        flags.bActive = true;
        spatial_register();
        spatial_move();
        // Msg								("!!! L-register: %X",u32(this));

#ifdef DEBUG
        Fvector zero = {0, -1000, 0};
        if (position.similar(zero))
        {
            Msg("- Uninitialized light position.");
        }
#endif // DEBUG
    }
    else
    {
        if (!flags.bActive)
            return;
        flags.bActive = false;
        spatial_move();
        spatial_unregister();
        // Msg								("!!! L-unregister: %X",u32(this));
    }
}

void light::set_position(const Fvector& P)
{
    const float eps = EPS_L; //_max	(range*0.001f,EPS_L);
    if (position.similar(P, eps))
        return;
    position.set(P);
    spatial_move();
}

void light::set_range(float R)
{
    const float eps = std::max(range * 0.1f, EPS_L);
    if (fsimilar(range, R, eps))
        return;
    range = R;
    spatial_move();
};

void light::set_cone(float angle)
{
    if (fsimilar(cone, angle))
        return;
    VERIFY(cone < deg2rad(121.f)); // 120 is hard limit for lights
    cone = angle;
    spatial_move();
}
void light::set_rotation(const Fvector& D, const Fvector& R)
{
    const Fvector old_D = direction;
    direction.normalize(D);
    right.normalize(R);
    if (!fsimilar(1.f, old_D.dotproduct(D)))
        spatial_move();
}

void light::spatial_move()
{
    switch (flags.type)
    {
    case IRender_Light::POINT: {
        spatial.sphere.set(position, range);
    }
    break;
    case IRender_Light::SPOT: {
        // minimal enclosing sphere around cone
        VERIFY(cone < deg2rad(121.f), "Too large light-cone angle. Maybe you have passed it in 'degrees'?");
        if (cone >= PI_DIV_2)
        {
            // obtused-angled
            spatial.sphere.P.mad(position, direction, range);
            spatial.sphere.R = range * tanf(cone / 2.f);
        }
        else
        {
            // acute-angled
            spatial.sphere.R = range / (2.f * _sqr(_cos(cone / 2.f)));
            spatial.sphere.P.mad(position, direction, spatial.sphere.R);
        }
    }
    break;
    case IRender_Light::OMNIPART: {
        // is it optimal? seems to be...
        // spatial.sphere.P.mad		(position,direction,range);
        // spatial.sphere.R			= range;
        // This is optimal.
        const float fSphereR = range * RSQRTDIV2;
        spatial.sphere.P.mad(position, direction, fSphereR);
        spatial.sphere.R = fSphereR;
    }
    break;
    }

    // update spatial DB
    ISpatial::spatial_move();

    if (!ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_SMAPVIS) && !get_moveable())
    {
        for (auto& svi : svis)
            svi.invalidate();
    }
}

vis_data& light::get_homdata()
{
    // commit vis-data
    hom.sphere.set(spatial.sphere.P, spatial.sphere.R);
    hom.box.set(spatial.sphere.P, spatial.sphere.P);
    hom.box.grow(spatial.sphere.R);
    return hom;
};

const Fvector& light::spatial_sector_point() const { return position; }

//////////////////////////////////////////////////////////////////////////

// Xforms
void light::xform_calc()
{
    if (Device.dwFrame == m_xform_frame)
        return;
    m_xform_frame = Device.dwFrame;

    ZoneScoped;

    // build final rotation / translation
    Fvector L_dir, L_up, L_right;

    // dir
    L_dir.set(direction);
    float l_dir_m = L_dir.magnitude();
    if (_valid(l_dir_m) && l_dir_m > EPS_S)
        L_dir.div(l_dir_m);
    else
        L_dir.set(0, 0, 1);

    // R&N
    if (right.square_magnitude() > EPS)
    {
        // use specified 'up' and 'right', just enshure ortho-normalization
        L_right.set(right);
        L_right.normalize();
        L_up.crossproduct(L_dir, L_right);
        L_up.normalize();
        L_right.crossproduct(L_up, L_dir);
        L_right.normalize();
    }
    else
    {
        // auto find 'up' and 'right' vectors
        L_up.set(0, 1, 0);
        if (_abs(L_up.dotproduct(L_dir)) > .99f)
            L_up.set(0, 0, 1);
        L_right.crossproduct(L_up, L_dir);
        L_right.normalize();
        L_up.crossproduct(L_dir, L_right);
        L_up.normalize();
    }

    // matrix
    Fmatrix mR;
    mR.i = L_right;
    mR._14 = 0;
    mR.j = L_up;
    mR._24 = 0;
    mR.k = L_dir;
    mR._34 = 0;
    mR.c = position;
    mR._44 = 1;

    // switch
    switch (flags.type)
    {
    case IRender_Light::POINT: {
        // scale of identity sphere
        float L_R = range;
        Fmatrix mScale;
        mScale.scale(L_R, L_R, L_R);
        m_xform.mul_43(mR, mScale);
    }
    break;
    case IRender_Light::SPOT: {
        // scale to account range and angle
        float s = 2.f * range * tanf(cone / 2.f);
        Fmatrix mScale;
        mScale.scale(s, s, range); // make range and radius
        m_xform.mul_43(mR, mScale);
    }
    break;
    case IRender_Light::OMNIPART: {
        float L_R = 2 * range; // volume is half-radius
        Fmatrix mScale;
        mScale.scale(L_R, L_R, L_R);
        m_xform.mul_43(mR, mScale);
    }
    break;
    default: m_xform.identity(); break;
    }
}

void light::optimize_smap_size()
{
    ZoneScoped;

    // Build EYE-space xform
    Fvector L_dir, L_up, L_right, L_pos;
    L_dir.set(direction);
    L_dir.normalize();
    if (right.square_magnitude() > EPS)
    {
        // use specified 'up' and 'right', just enshure ortho-normalization
        L_right.set(right);
        L_right.normalize();
        L_up.crossproduct(L_dir, L_right);
        L_up.normalize();
        L_right.crossproduct(L_up, L_dir);
        L_right.normalize();
    }
    else
    {
        // auto find 'up' and 'right' vectors
        L_up.set(0, 1, 0);
        if (_abs(L_up.dotproduct(L_dir)) > .99f)
            L_up.set(0, 0, 1);
        L_right.crossproduct(L_up, L_dir);
        L_right.normalize();
        L_up.crossproduct(L_dir, L_right);
        L_up.normalize();
    }
    L_pos.set(position);

    const int _cached_size = X.S.size;
    X.S.posX = 0;
    X.S.posY = 0;
    X.S.size = SMAP_adapt_max;

    // Compute approximate screen area (treating it as an point light) - R*R/dist_sq
    // Note: we clamp screen space area to ONE, although it is not correct at all
    float dist = Device.vCameraPosition.distance_to(spatial.sphere.P) - spatial.sphere.R;
    if (dist < 0)
        dist = 0;
    const float ssa = clampr(range * range / (1.f + dist * dist), 0.f, 1.f);

    // compute intensity
    const float intensity0 = (color.r + color.g + color.b) / 3.f;
    const float intensity1 = (color.r * 0.2125f + color.g * 0.7154f + color.b * 0.0721f);
    const float intensity = (intensity0 + intensity1) / 2.f; // intensity1 tends to underestimate...

    // [SSS 19] Improve this code later?
    // compute how much duelling frusta occurs	[-1..1]-> 1 + [-0.5 .. +0.5]
    // float duel_dot = 1.f - 0.5f * Device.vCameraDirection.dotproduct(L_dir);

    // compute how large the light is - give more texels to larger lights, assume 8m as being optimal radius
    const float sizefactor = range / 8.f; // 4m = .5, 8m=1.f, 16m=2.f, 32m=4.f

    // compute how wide the light frustum is - assume 90deg as being optimal
    const float widefactor = cone / deg2rad(90.f); //

    // factors
    const float factor0 = powf(ssa, 1.f / 2.f); // ssa is quadratic
    const float factor1 = powf(intensity, 1.f / 16.f); // less perceptually important?
    // float factor2 = powf(duel_dot, 1.f / 4.f); // difficult to fast-change this -> visible
    const float factor3 = powf(sizefactor, 1.f / 4.f); // this shouldn't make much difference
    const float factor4 = powf(widefactor, 1.f / 2.f); // make it linear ???

    // float factor = ps_r2_ls_squality * factor0 * factor1 * factor2 * factor3 * factor4;
    const float factor = ps_r2_ls_squality * factor0 * factor1 * factor3 * factor4;

    // final size calc
    const u32 max_size = RImplementation.o.smapsize <= static_cast<u32>(ps_ssfx_shadows.y) ? RImplementation.o.smapsize : static_cast<u32>(ps_ssfx_shadows.y);

    const u32 _size = std::clamp<u32>(iFloor(factor * SMAP_adapt_optimal), static_cast<u32>(ps_ssfx_shadows.x), max_size);

    const int _epsilon = iCeil(float(_size) * 0.01f);
    int _diff = _abs(int(_size) - int(_cached_size));

    X.S.size = (_diff >= _epsilon) ? _size : _cached_size;

    // make N pixel border
    X.S.view.build_camera_dir(L_pos, L_dir, L_up);

    // _min(L->cone + deg2rad(4.5f), PI*0.98f) - Here, it is needed to enlarge the shadow map frustum to include also
    // displaced pixels and the pixels neighbor to the examining one.

    float tan_shift;
    if (flags.type == IRender_Light::OMNIPART) // [ SSS ] 0.3f fix almost all frustum problems... 0.5f was the old value ( SSS 19 ) but was causing issues?
        tan_shift = 0.3f;
    else if(flags.type == IRender_Light::POINT)
        tan_shift = 0.2007129f; // deg2rad(11.5f);
    else
        tan_shift = 0.0610865f; // deg2rad(3.5f);

    X.S.project.build_projection(cone + tan_shift, 1.f, virtual_size, range + EPS_S);
    X.S.combine.mul(X.S.project, X.S.view);
}

//								+X,				-X,				+Y,				-Y,			+Z,				-Z
constexpr Fvector cmNorm[6] = {{0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, 0.f, -1.f}, {0.f, 0.f, 1.f}, {0.f, 1.f, 0.f}, {0.f, 1.f, 0.f}};
constexpr Fvector cmDir[6] = {{1.f, 0.f, 0.f}, {-1.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, {0.f, -1.f, 0.f}, {0.f, 0.f, 1.f}, {0.f, 0.f, -1.f}};

void light::export_to(light_Package& package)
{
    const float distance = Device.vCameraPosition.distance_to(position); // refresh distance

    if (distance > (g_pGamePersistent->Environment().CurrentEnv->fog_distance))
        return;

    if (ps_r2_ls_flags.test(R2FLAG_LIGHT_NO_DIST_SHADOWS) && (distance > 100.f /*|| range > 25.f*/))
    {
        flags.bShadow = false;
        flags.bVolumetric = false;
    }
    else
    {
        flags.bShadow = b_originShadow;
        flags.bVolumetric = b_originVolumetric;
    }

    if (flags.bShadow)
    {
        switch (flags.type)
        {
        case IRender_Light::POINT: {

            // tough: create/update 6 shadowed lights
            if (!omnipart[0])
                for (auto& f : omnipart)
                {
                    f = xr_new<light>();

                    f->set_type(IRender_Light::OMNIPART);
                    f->flags.bShadow = true;
                }

            for (int f = 0; f < 6; f++)
            {
                light* L = omnipart[f];

                L->set_position(position);
                
                Fvector R;
                R.crossproduct(cmNorm[f], cmDir[f]);
                L->set_rotation(cmDir[f], R);

                L->set_cone(PI_DIV_2 + 0.5f); // Add some extra angle to avoid problems with the shadow map frustum.
                L->set_range(range);

                // надо еще экспортировать
                L->set_virtual_size(virtual_size);
                L->set_color(color);

                L->spatial.sector_id = spatial.sector_id; //. dangerous?

                L->s_spot = s_spot;
                L->s_point = s_point;
                L->s_volumetric = s_volumetric;

                //	Igor: add volumetric support
                L->flags.bVolumetric = flags.bVolumetric;
                L->set_volumetric_quality(m_volumetric_quality);
                L->set_volumetric_intensity(m_volumetric_intensity);
                L->set_volumetric_distance(m_volumetric_distance);

                L->set_flare(flags.bFlare);
                L->set_moveable(get_moveable());

                package.v_shadowed.push_back(L);
            }
        }
        break;
        case IRender_Light::SPOT:
            package.v_shadowed.push_back(this);
            break;
        }
    }
    else
    {
        switch (flags.type)
        {
        case IRender_Light::POINT: package.v_point.push_back(this); break;
        case IRender_Light::SPOT: package.v_spot.push_back(this); break;
        }
    }
}

void light::set_attenuation_params(float a0, float a1, float a2, float fo)
{
    attenuation0 = a0;
    attenuation1 = a1;
    attenuation2 = a2;
    falloff = fo;
}

extern float r_ssaGLOD_start, r_ssaGLOD_end;

float light::get_LOD() const
{
    if (!flags.bShadow) // easy
        return 1;

    const float distSQ = Device.vCameraPosition.distance_to_sqr(spatial.sphere.P) + EPS;
    const float ssa = ps_r2_slight_fade * spatial.sphere.R / distSQ;
    const float lod = _sqrt(clampr((ssa - r_ssaGLOD_end) / (r_ssaGLOD_start - r_ssaGLOD_end), 0.f, 1.f));
    return lod;
}
