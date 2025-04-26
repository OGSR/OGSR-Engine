// Blender.h: interface for the IBlender class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "../../xr_3da/properties.h"
#include "Blender_Recorder.h"

#pragma pack(push, 4)

class ECORE_API CBlender_DESC
{
public:
    CLASS_ID CLS;

    string128 cName;
    string32 cComputer;
    u32 cTime;
    u16 version;

    CBlender_DESC()
    {
        CLS = CLASS_ID(0);

        cName[0] = 0;
        cComputer[0] = 0;
        cTime = 0;
        version = 0;
    }
};

class ECORE_API IBlender
{
    friend class CBlender_Compile;

protected:
    CBlender_DESC description;

    xrP_INTEGER oPriority;
    xrP_BOOL oStrictSorting;

    string64 oT_Name;

public:

    CBlender_DESC& getDescription() { return description; }
    virtual LPCSTR getName() { return description.cName; }
    virtual LPCSTR getComment() = 0;

    virtual BOOL canBeDetailed() { return FALSE; }
    virtual BOOL canUseSteepParallax() { return FALSE; }

    virtual void Compile(CBlender_Compile& C);

    IBlender();
    virtual ~IBlender();
};

class ECORE_API IBlenderXr : public IBlender, public CPropertyBase
{
protected:
    static void WriteInteger(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_INTEGER v);
    static void WriteBool(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_BOOL v);
    static void WriteToken(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_TOKEN v);

    static void ReadInteger(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_INTEGER& v);
    static void ReadBool(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_BOOL& v);
    static void ReadToken(CInifile* ini_file, LPCSTR section, LPCSTR line, xrP_TOKEN& v);

public:
    static IBlenderXr* Create(CLASS_ID cls);
    static void Destroy(IBlenderXr*& B);

    virtual void Save(IWriter& fs);
    virtual void Load(IReader& fs, u16 version);

    virtual void SaveIni(CInifile* ini_file, LPCSTR section);
    virtual void LoadIni(CInifile* ini_file, LPCSTR section);
};

#pragma pack(pop)
