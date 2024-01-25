#pragma once

// refs
class CSoundRender_Environment : public CSound_environment
{
public:
    u32 version;

    CSoundRender_Environment(void);
    ~CSoundRender_Environment(void);
    void set_identity();
    void set_default();
    void clamp();
    void lerp(CSoundRender_Environment& A, CSoundRender_Environment& B, float f);
    void set_from(CSoundRender_Environment& A);

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
