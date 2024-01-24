#pragma once

class CBlender_deffer_model : public IBlenderXr
{
public:
    xrP_INTEGER oAREF;
    xrP_BOOL oBlend;

public:
    virtual LPCSTR getComment() { return "LEVEL: deffer-model-flat"; }

    virtual BOOL canBeDetailed() { return TRUE; }

    virtual void Save(IWriter& fs);
    virtual void Load(IReader& fs, u16 version);

    virtual void SaveIni(CInifile* ini_file, LPCSTR section);
    virtual void LoadIni(CInifile* ini_file, LPCSTR section);

    virtual void Compile(CBlender_Compile& C);

    CBlender_deffer_model();
    virtual ~CBlender_deffer_model();

private:
    xrP_TOKEN oTessellation;
};
