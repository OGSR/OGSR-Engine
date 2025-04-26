#pragma once

class ENGINE_API CLAItem
{
public:
    shared_str cName{};
    float fFPS{};
    float def_fFPS{};

    DEFINE_MAP(int, u32, KeyMap, KeyPairIt);
    KeyMap Keys{}; // always in RGBA

    int iFrameCount{};

public:
    CLAItem();

    void InitDefault();

    void Load(IReader& F);

    float Length_sec() const { return float(iFrameCount) / fFPS; }
    u32 Length_ms() const { return iFloor(Length_sec() * 1000.f); }

    u32 CalculateRGB(float T, int& frame);
    u32 CalculateBGR(float T, int& frame);

    void SetFramerate(float framerate) { fFPS = framerate; }
    void ResetFramerate()
    {
        fFPS = def_fFPS;
    }
    void Resize(int new_len);
    void MoveKey(int from, int to);

    int FirstKeyFrame() const { return Keys.rend()->first; }
    int LastKeyFrame() const { return Keys.rbegin()->first; }

public:
    u32 InterpolateRGB(int frame);
    u32 InterpolateBGR(int frame);
};
DEFINE_VECTOR(CLAItem*, LAItemVec, LAItemIt);

class ENGINE_API ELightAnimLibrary
{
protected:
    LAItemVec Items;

    LAItemIt FindItemI(LPCSTR name);

public:
    ELightAnimLibrary();
    ~ELightAnimLibrary();

    CLAItem* FindItem(LPCSTR name);

    void OnCreate();
    void OnDestroy();

    void Load();
    void Unload();

    void DbgDumpInfo() const;
};

extern ENGINE_API ELightAnimLibrary LALib;
