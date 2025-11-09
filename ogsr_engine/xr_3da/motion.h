//----------------------------------------------------
#pragma once

#include "bone.h"

// refs
class CEnvelope;
class IWriter;
class IReader;

enum EChannelType
{
    ctUnsupported = -1,

    ctPositionX = 0,
    ctPositionY,
    ctPositionZ,

    ctRotationH,
    ctRotationP,
    ctRotationB,

    ctMaxChannel
};

//--------------------------------------------------------------------------
class ENGINE_API CCustomMotion
{
protected:
    enum EMotionType
    {
        mtObject = 0,
        mtSkeleton,
        ForceDWORD = u32(-1)
    };

    EMotionType mtype{};
    int iFrameStart, iFrameEnd;
    float fFPS;

public:
    shared_str name;

public:
    CCustomMotion();
    CCustomMotion(CCustomMotion* src);
    virtual ~CCustomMotion();

    void SetName(const char* n)
    {
        string256 tmp;
        tmp[0] = 0;
        if (n)
        {
            strcpy_s(tmp, n);
            _strlwr(tmp);
        }
        name = tmp;
    }
    LPCSTR Name() const { return name.c_str(); }

    int FrameStart() const { return iFrameStart; }
    int FrameEnd() const { return iFrameEnd; }

    float FPS() { return fFPS; }

    int Length() const { return iFrameEnd - iFrameStart; }

    void SetParam(int s, int e, float fps)
    {
        iFrameStart = s;
        iFrameEnd = e;
        fFPS = fps;
    }

    virtual void Save(IWriter& F);
    virtual bool Load(IReader& F);

    virtual void SaveMotion(const char* buf) = 0;
    virtual bool LoadMotion(const char* buf) = 0;
};

//--------------------------------------------------------------------------
class ENGINE_API COMotion : public CCustomMotion
{
    CEnvelope* envs[ctMaxChannel];

public:
    COMotion();
    COMotion(COMotion* src);
    virtual ~COMotion();

    void Clear();

    void _Evaluate(float t, Fvector& T, Fvector& R);

    virtual void Save(IWriter& F);
    virtual bool Load(IReader& F);

    virtual void SaveMotion(const char* buf);
    virtual bool LoadMotion(const char* buf);
};

//--------------------------------------------------------------------------

enum ESMFlags
{
    esmFX = 1 << 0,
    esmStopAtEnd = 1 << 1,
    esmNoMix = 1 << 2,
    esmSyncPart = 1 << 3,
    esmUseFootSteps = 1 << 4,
    esmRootMover = 1 << 5,
    esmIdle = 1 << 6,
    esmUseWeaponBone = 1 << 7,
};

struct ECORE_API SAnimParams
{
    float t;
    float min_t;
    float max_t;
    BOOL bPlay;
    BOOL bWrapped;

public:
    SAnimParams()
    {
        bWrapped = false;
        bPlay = false;
        t = 0.f;
        min_t = 0.f;
        max_t = 0.f;
    }

    void Set(CCustomMotion* M);
    void Set(float start_frame, float end_frame, float fps);

    float Frame() const { return t; }

    void Update(float dt, float speed, bool loop);

    void Play()
    {
        bPlay = true;
        t = min_t;
    }
    void Stop()
    {
        bPlay = false;
        t = min_t;
    }
    void Pause(bool val) { bPlay = !val; }
};
