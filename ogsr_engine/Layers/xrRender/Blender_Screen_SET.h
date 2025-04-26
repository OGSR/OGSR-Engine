// Blender_Screen_SET.h: interface for the Blender_Screen_SET class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CBlender_Screen_SET : public IBlenderXr
{
    xrP_TOKEN oBlend;
    xrP_INTEGER oAREF;
    xrP_BOOL oZTest;
    xrP_BOOL oZWrite;
    xrP_BOOL oLighting;
    xrP_BOOL oFog;
    xrP_BOOL oClamp;

public:
    virtual LPCSTR getComment() { return "basic (simple)"; }

    virtual void Save(IWriter& fs);
    virtual void Load(IReader& fs, u16 version);

    virtual void SaveIni(CInifile* ini_file, LPCSTR section);
    virtual void LoadIni(CInifile* ini_file, LPCSTR section);

    virtual void Compile(CBlender_Compile& C);

    CBlender_Screen_SET();
    virtual ~CBlender_Screen_SET();
};
