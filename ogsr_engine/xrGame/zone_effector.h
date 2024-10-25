#pragma once

class CActor;
class CAnimatorCamLerpEffector;
class CPostprocessAnimatorLerp;

class CZoneEffector
{
    float r_min_perc{};
    float r_max_perc{};
    float radius{1.f};
    float m_factor{0.1f};
    CAnimatorCamLerpEffector* m_cam_effector;
    CPostprocessAnimatorLerp* m_pp_effector;
    shared_str m_pp_fname;
    shared_str m_cam_fname;

public:
    CZoneEffector();
    ~CZoneEffector();

    CActor* m_pActor{};

    void Load(LPCSTR section);
    void SetRadius(float r);
    void Update(float dist);
    void Stop();
    float GetFactor();

private:
    void Activate();
};
