// Blender_Screen_SET.h: interface for the Blender_Screen_SET class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CBlender_Particle : public IBlenderXr
{
    xrP_TOKEN oBlend;

    xrP_BOOL oClamp;

public:
    virtual LPCSTR getComment() { return "particles"; }

    virtual void Save(IWriter& fs);
    virtual void Load(IReader& fs, u16 version);
    
    virtual void SaveIni(CInifile* ini_file, LPCSTR section);
    virtual void LoadIni(CInifile* ini_file, LPCSTR section);

    virtual void Compile(CBlender_Compile& C);

    CBlender_Particle();
    virtual ~CBlender_Particle();
};
