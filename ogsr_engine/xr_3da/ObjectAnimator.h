#pragma once

#include "motion.h"

// refs
class ENGINE_API CObjectAnimator
{
private:
    using MotionVec = xr_vector<COMotion*>;

protected:

    shared_str m_Name;

    Fmatrix m_XFORM;

    MotionVec m_Motions;
    float m_Speed;

    COMotion* m_Current;

    void LoadMotions(LPCSTR fname);
    void SetActiveMotion(COMotion* mot);

public:
    CObjectAnimator();
    virtual ~CObjectAnimator();

    bool bLoop;
    SAnimParams m_MParam;

    void Clear();
    void Load(LPCSTR name);

    IC LPCSTR Name() const { return *m_Name; }
    float& Speed() { return m_Speed; }

    COMotion* Play(bool bLoop, LPCSTR name = nullptr);
    void Pause(bool val) { return m_MParam.Pause(val); }
    void Stop();
    IC BOOL IsPlaying() const { return m_MParam.bPlay; }

    IC const Fmatrix& XFORM() { return m_XFORM; }
    float GetLength();
    // Update
    void Update(float dt);
};
