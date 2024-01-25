#pragma once

// refs
class CSoundRender_Environment : public CSound_environment
{
public:
    u32 version;
    shared_str name;

    u32 Environment; // sorce environment
    float Room; // room effect level at low frequencies
    float RoomHF; // room effect high-frequency level re. low frequency level
    float RoomRolloffFactor; // like DS3D flRolloffFactor but for room effect
    float DecayTime; // reverberation decay time at low frequencies
    float DecayHFRatio; // high-frequency to low-frequency decay time ratio
    float Reflections; // early reflections level relative to room effect
    float ReflectionsDelay; // initial reflection delay time
    float Reverb; // late reverberation level relative to room effect
    float ReverbDelay; // late reverberation delay time relative to initial reflection
    float EnvironmentSize; // environment size in meters
    float EnvironmentDiffusion; // environment diffusion
    float AirAbsorptionHF; // change in level per meter at 5 kHz
public:
    CSoundRender_Environment(void);
    ~CSoundRender_Environment(void);
    void set_identity();
    void set_default();
    void clamp();
    void lerp(CSoundRender_Environment& A, CSoundRender_Environment& B, float f);

    bool load(IReader* fs);
    void save(IWriter* fs) const;

    void loadIni(CInifile* ini, LPCSTR name);
    void saveIni(CInifile* ini, LPCSTR name) const;
};

class SoundEnvironment_LIB
{
public:
    DEFINE_VECTOR(CSoundRender_Environment*, SE_VEC, SE_IT);

private:
    SE_VEC library;

public:
    SoundEnvironment_LIB() { library.reserve(16); }

    void Load(LPCSTR f_name);
    bool Save(LPCSTR f_name) const;

    void LoadIni(CInifile* ini);
    bool SaveIni(CInifile* ini) const;

    void Unload();

    int GetID(LPCSTR name);

    CSoundRender_Environment* Get(int id) const;

    SE_VEC& Library();
};
