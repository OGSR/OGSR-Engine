// LightTrack.h: interface for the CLightTrack class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

const float lt_inc = 4.f;
const float lt_dec = 2.f;
const int lt_hemisamples = 26;

class CROS_impl : public IRender_ObjectSpecific
{
public:
    enum CubeFaces
    {
        CUBE_FACE_POS_X,
        CUBE_FACE_POS_Y,
        CUBE_FACE_POS_Z,
        CUBE_FACE_NEG_X,
        CUBE_FACE_NEG_Y,
        CUBE_FACE_NEG_Z,
        NUM_FACES
    };

    struct Item
    {
        u32 frame_touched; // to track creation & removal
        light* source; //
        collide::ray_cache cache; //
        float test; // note range: (-1[no]..1[yes])
        float energy; //
    };
    struct Light
    {
        light* source;
        float energy;
        Fcolor color;
    };

public:
    // general
    u32 MODE;
    std::atomic<u32> dwFrame;
    std::atomic<u32> dwFrameSmooth;
    bool skip{};

    //
    xr_vector<Item> track; // everything what touches
    xr_vector<Light> lights; //

    bool result[lt_hemisamples]{};
    collide::ray_cache cache[lt_hemisamples];
    collide::ray_cache cache_sun;
    s32 result_count;
    u32 result_iterator;
    u32 result_frame;
    s32 result_sun;

public:
    u32 shadow_gen_frame{};
    u32 shadow_recv_frame;
    int shadow_recv_slot;

private:
    float hemi_cube[NUM_FACES];
    float hemi_cube_smooth[NUM_FACES];

    float hemi_value;
    float hemi_smooth;
    float sun_value;
    float sun_smooth;

    Fvector approximate;


    Fvector last_position;
    s32 ticks_to_update;
    s32 sky_rays_uptodate;

public:
    virtual float get_luminocity()
    {
        float result = _max(approximate.x, _max(approximate.y, approximate.z));
        clamp(result, 0.f, 1.f);
        return (result);
    };
    virtual float get_luminocity_hemi() { return get_hemi(); }
    virtual float* get_luminocity_hemi_cube() { return hemi_cube_smooth; }

    void add(light* source);
    void update(IRenderable* O);
    void update_smooth(IRenderable* O = nullptr);

    ICF float get_hemi()
    {
        if (dwFrameSmooth != Device.dwFrame)
            update_smooth();
        return hemi_smooth;
    }
    ICF float get_sun()
    {
        if (dwFrameSmooth != Device.dwFrame)
            update_smooth();
        return sun_smooth;
    }
    ICF Fvector3& get_approximate()
    {
        if (dwFrameSmooth != Device.dwFrame)
            update_smooth();
        return approximate;
    }

    const float* get_hemi_cube()
    {
        if (dwFrameSmooth != Device.dwFrame)
            update_smooth();
        return hemi_cube_smooth;
    }

    CROS_impl();
    virtual ~CROS_impl(){};

private:
    // static inline CubeFaces get_cube_face(Fvector3& dir);

    // Accumulates light from direction for corresponding faces
    static inline void accum_hemi(float* hemi_cube, const Fvector3& dir, const float scale);

    // Calculates sun part of ambient occlusion
    void calc_sun_value(const Fvector& position, const CObject* _object);

    // Calculates sky part of ambient occlusion
    void calc_sky_hemi_value(const Fvector& position, const CObject* _object);

    // prepares static or hemisphere lights for ambient occlusion calculations
    void prepare_lights(const Fvector& position, IRenderable* O);

    //	Updates only if makes a desizion that update is necessary
    void smart_update(IRenderable* O);
};
