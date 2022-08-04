// CDemoPlay.h: interface for the CDemoPlay class.
//
//////////////////////////////////////////////////////////////////////

#pragma once
#include "effector.h"

// refs
class COMotion;
struct SAnimParams;

// class
class ENGINE_API CDemoPlay : public CEffectorCam
{
    COMotion* m_pMotion;
    SAnimParams* m_MParam;

    xr_vector<Fmatrix> seq;
    int m_count;
    float fStartTime{};
    float fSpeed;
    u32 dwCyclesLeft;

    // statistics
    BOOL stat_started;
    CTimer stat_Timer_frame;
    CTimer stat_Timer_total;
    u32 stat_StartFrame{};
    xr_vector<float> stat_table;

    void stat_Start();
    void stat_Stop();

public:
    virtual BOOL ProcessCam(SCamEffectorInfo& info);

    CDemoPlay(const char* name, float ms, u32 cycles, float life_time = 60 * 60 * 1000);
    virtual ~CDemoPlay();
};
