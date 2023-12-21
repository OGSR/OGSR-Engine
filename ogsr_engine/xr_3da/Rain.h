// Rain.h: interface for the CRain class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "../xrcdb/xr_collide_defs.h"

inline const float max_desired_items = std::thread::hardware_concurrency() < 3u ? 1500.f : 2500.f;
inline const float min_desired_items = max_desired_items / 10.f;

constexpr float source_radius = 15.f; // 12.5f;
constexpr float source_offset = 40.f;

constexpr float max_distance = source_offset * 1.5f; // 1.25f;
constexpr float sink_offset = -(max_distance - source_offset);

constexpr float drop_length = 7.f;
constexpr float drop_width = 0.40f;
constexpr float drop_angle = deg2rad(15.0f); // 3.0
constexpr float drop_max_angle = deg2rad(35.f); // 10;
constexpr float drop_max_wind_vel = 20.0f;
constexpr float drop_speed_min = 40.f;
constexpr float drop_speed_max = 80.f;

constexpr size_t max_particles = 1000;
constexpr u32 particles_cache = 400;

constexpr float particles_time = .3f;

// refs
class ENGINE_API IRender_DetailModel;

#include "../Include/xrRender/FactoryPtr.h"
#include "../Include/xrRender/RainRender.h"
//
class ENGINE_API CEffect_Rain
{
    friend class dxRainRender;

private:
    struct Item
    {
        Fvector P;
        Fvector Phit;
        Fvector D;
        float fSpeed;
        u32 dwTime_Life;
        u32 dwTime_Hit;
        u32 uv_set;
        void invalidate() { dwTime_Life = 0; }
    };
    struct Particle
    {
        Particle *next, *prev;
        Fmatrix mXForm;
        Fsphere bounds;
        float time;
    };
    enum States
    {
        stIdle = 0,
        stWorking
    };

private:
    // Visualization	(rain) and (drops)
    FactoryPtr<IRainRender> m_pRender;
    /*
    // Visualization	(rain)
    ref_shader						SH_Rain;
    ref_geom						hGeom_Rain;

    // Visualization	(drops)
    IRender_DetailModel*			DM_Drop;
    ref_geom						hGeom_Drops;
    */

    // Data and logic
    xr_vector<Item> items;
    States state;

    // Particles
    xr_vector<Particle> particle_pool;
    Particle* particle_active;
    Particle* particle_idle;

    // Sounds
    ref_sound snd_Ambient;

    // Utilities
    void p_create();
    void p_destroy();

    void p_remove(Particle* P, Particle*& LST);
    void p_insert(Particle* P, Particle*& LST);
    int p_size(Particle* LST);
    Particle* p_allocate();
    void p_free(Particle* P);

    // Some methods
    void Born(Item& dest, const float radius, const float speed, const float vel, const Fvector2& offset, const Fvector3& axis);
    void Hit(Fvector& pos);
    BOOL RayPick(const Fvector& s, const Fvector& d, float& range, collide::rq_target tgt);
    void RenewItem(Item& dest, float height, BOOL bHit);

public:
    CEffect_Rain();
    ~CEffect_Rain();

    void Render();
    void Calculate();
    void OnFrame();
    void InvalidateState() { state = stIdle; }
};
