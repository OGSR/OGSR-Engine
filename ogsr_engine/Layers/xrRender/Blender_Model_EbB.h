// BlenderDefault.h: interface for the CBlenderDefault class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CBlender_Model_EbB : public IBlenderXr
{
private:
    string64 oT2_Name; // name of secondary texture

    xrP_BOOL oBlend;

public:
    virtual LPCSTR getComment() { return "MODEL: env^base"; }

    virtual void Save(IWriter& fs);
    virtual void Load(IReader& fs, u16 version);

    virtual void SaveIni(CInifile* ini_file, LPCSTR section);
    virtual void LoadIni(CInifile* ini_file, LPCSTR section);

    virtual void Compile(CBlender_Compile& C);

    CBlender_Model_EbB();
    virtual ~CBlender_Model_EbB();
};
