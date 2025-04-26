// Blender_Tree.h: interface for the CBlender_Tree class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

class CBlender_Tree : public IBlenderXr
{
private:
    xrP_BOOL oBlend;
    xrP_BOOL oNotAnTree;

public:
    virtual LPCSTR getComment() { return "LEVEL: trees/bushes"; }
    virtual BOOL canBeDetailed() { return TRUE; }

    virtual void Save(IWriter& fs);
    virtual void Load(IReader& fs, u16 version);

    virtual void SaveIni(CInifile* ini_file, LPCSTR section);
    virtual void LoadIni(CInifile* ini_file, LPCSTR section);

    virtual void Compile(CBlender_Compile& C);

    CBlender_Tree();
    virtual ~CBlender_Tree();
};
