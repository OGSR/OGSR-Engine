#pragma once

#include "../../xrcdb/ispatial.h"

#include "light_package.h"
#include "light_smapvis.h"

#define MIN_VIRTUAL_SIZE 0.01f

extern Fvector3 ps_ssfx_volumetric;

class light : public IRender_Light, public ISpatial
{
private:
    bool b_originShadow{};
    bool b_originVolumetric{};
    
    vis_data hom;    

    light* omnipart[6];
    light* omniparent{};

    u32 m_xform_frame;

public:
    struct
    {
        u32 type : 4;
        u32 bStatic : 1;
        u32 bActive : 1;
        u32 bShadow : 1;
        u32 bVolumetric : 1;
        u32 bHudMode : 1;
        u32 bFlare : 1;
        u32 bMoveable : 1;
    } flags;

    Fvector position;
    Fvector direction;

    Fvector right;
    float range;
    float cone;
    Fcolor color;

    u32 frame_render;

    float m_volumetric_quality;
    float m_volumetric_intensity;
    float m_volumetric_distance;

    float fBlend; // For flares

    float falloff{}; // precalc to make light equal to zero at light range
    float attenuation0{}; // Constant attenuation
    float attenuation1{}; // Linear attenuation
    float attenuation2{}; // Quadratic attenuation

    float virtual_size;

    smapvis svis[R__NUM_CONTEXTS]; // used for 6-cubemap faces

    ref_shader s_spot;
    ref_shader s_point;
    ref_shader s_volumetric;

    Fmatrix m_xform;

    struct _vis
    {
        u32 frame2test; // frame the test is sheduled to
        u32 query_id; // ID of occlusion query
        u32 query_order; // order of occlusion query
        bool visible; // visible/invisible
        bool pending; // test is still pending
        u16 smap_ID;
        float distance;
    } vis;

    union _xform
    {
        struct _D
        {
            Fmatrix combine;
            s32 minX, maxX;
            s32 minY, maxY;
            BOOL transluent;
        } D[R__NUM_SUN_CASCADES]; // directional
        struct _S
        {
            Fmatrix view;
            Fmatrix project;
            Fmatrix combine;
            u32 size;
            u32 posX;
            u32 posY;
            BOOL transluent;
        } S; // spot
    } X;

    RayPickAsync FlareRayPick;

public:
    light();
    virtual ~light();

    virtual void set_type(LT type) { flags.type = type; }
    virtual void set_active(bool b);
    virtual bool get_active() { return flags.bActive; }
    virtual void set_shadow(bool b)
    {
        flags.bShadow = b;
        b_originShadow = b;
    }
    virtual bool get_shadow() override { return b_originShadow; }

    virtual void set_volumetric(bool b)
    {
        flags.bVolumetric = b;
        b_originVolumetric = b;
    }
    virtual bool get_volumetric() override { return b_originVolumetric; }

    virtual void set_volumetric_quality(float fValue) { m_volumetric_quality = fValue; }
    virtual void set_volumetric_intensity(float fValue) { m_volumetric_intensity = fValue; }
    virtual void set_volumetric_distance(float fValue) { m_volumetric_distance = fValue; }

    virtual void set_position(const Fvector& P);
    virtual void set_rotation(const Fvector& D, const Fvector& R);
    virtual void set_cone(float angle);

    virtual void set_range(float R);
    virtual float get_range() const override { return range; }

    virtual void set_virtual_size(float S)
    {
        virtual_size = (S > MIN_VIRTUAL_SIZE) ? S : MIN_VIRTUAL_SIZE;
    }
    virtual void set_texture(LPCSTR name) override;

    virtual void set_color(const Fcolor& C) { color.set(C); }

    virtual void set_color(float r, float g, float b) { color.set(r, g, b, 1); }
    Fcolor get_color() const override { return color; }

    virtual void set_flare(bool b) { flags.bFlare = b; }
    virtual bool get_flare() override { return flags.bFlare; }

    virtual void set_hud_mode(bool b) { flags.bHudMode = b; }

    virtual bool get_hud_mode() { return flags.bHudMode; }

    virtual void set_moveable(bool b) override { flags.bMoveable = b; }
    virtual bool get_moveable() override { return flags.bMoveable || flags.bHudMode; }

    virtual void spatial_move();
    virtual Fvector spatial_sector_point();

    virtual IRender_Light* dcast_Light() { return this; }

    vis_data& get_homdata();
    float get_LOD() const;

    void xform_calc();
    void optimize_smap_size();
    void export_to(light_Package& dest);

    void vis_prepare(CBackend& cmd_list);
    void vis_update();

    void set_attenuation_params(float a0, float a1, float a2, float fo);
};
