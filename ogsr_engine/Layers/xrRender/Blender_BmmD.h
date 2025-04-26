// BlenderDefault.h: interface for the CBlenderDefault class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CBlender_BmmD : public IBlenderXr
{
public:
    string64 oT2_Name; // name of secondary texture

    string64 oR_Name; //. задел на будущее
    string64 oG_Name; //. задел на будущее
    string64 oB_Name; //. задел на будущее
    string64 oA_Name; //. задел на будущее
public:
    virtual LPCSTR getComment() { return "LEVEL: Implicit**detail"; }
    virtual BOOL canBeDetailed() { return TRUE; }

    virtual void Save(IWriter& fs);
    virtual void Load(IReader& fs, u16 version);

    virtual void SaveIni(CInifile* ini_file, LPCSTR section);
    virtual void LoadIni(CInifile* ini_file, LPCSTR section);

    virtual void Compile(CBlender_Compile& C);

    CBlender_BmmD();
    virtual ~CBlender_BmmD();
};
