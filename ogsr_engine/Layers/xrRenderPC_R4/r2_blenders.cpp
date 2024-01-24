#include "stdafx.h"

#include "../xrRender/uber_deffer.h"

#include "blender_deffer_flat.h"
#include "blender_deffer_model.h"
#include "blender_deffer_aref.h"

#include "../xrRender/blender_BmmD.h"
#include "../xrRender/blender_screen_set.h"
#include "../xrRender/blender_tree.h"
#include "../xrRender/blender_detail_still.h"
#include "../xrRender/blender_particle.h"
#include "../xrRender/Blender_Model_EbB.h"
#include "../xrRender/blender_Lm(EbB).h"

class CBlender_Editor_Selection : public IBlenderXr
{
public:
    virtual LPCSTR getComment() { return "EDITOR: selection"; }

    void Save(IWriter& fs)
    {
        IBlenderXr::Save(fs);
        string64 oT_Factor;
        xrPWRITE_PROP(fs, "TFactor", xrPID_CONSTANT, oT_Factor);
    }

    void Load(IReader& fs, u16 version)
    {
        IBlenderXr::Load(fs, version);
        string64 oT_Factor;
        xrPREAD_PROP(fs, xrPID_CONSTANT, oT_Factor);
    }

    virtual void Compile(CBlender_Compile& C)
    {
        IBlender::Compile(C);

        C.r_Pass("editor", "simple_color", FALSE, TRUE, FALSE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA);
        C.r_End();
    }

    CBlender_Editor_Selection() { description.CLS = B_EDITOR_SEL; }
    virtual ~CBlender_Editor_Selection() {}
};

class CBlender_Editor_Wire : public IBlenderXr
{
public:
    virtual LPCSTR getComment() { return "EDITOR: wire"; }

    void Save(IWriter& fs)
    {
        IBlenderXr::Save(fs);
        string64 oT_Factor;
        xrPWRITE_PROP(fs, "TFactor", xrPID_CONSTANT, oT_Factor);
    }

    void Load(IReader& fs, u16 version)
    {
        IBlenderXr::Load(fs, version);
        string64 oT_Factor;
        xrPREAD_PROP(fs, xrPID_CONSTANT, oT_Factor);
    }

    virtual void Compile(CBlender_Compile& C)
    {
        C.r_Pass("editor", "simple_color", FALSE, TRUE, TRUE);
        C.r_End();
    }

    CBlender_Editor_Wire() { description.CLS = B_EDITOR_WIRE; }
    virtual ~CBlender_Editor_Wire() {}
};

IBlenderXr* CRender::blender_create(CLASS_ID cls)
{
    switch (cls)
    {
    case B_DEFAULT: return xr_new<CBlender_deffer_flat>();
    case B_DEFAULT_AREF: return xr_new<CBlender_deffer_aref>(true);
    case B_VERT: return xr_new<CBlender_deffer_flat>();
    case B_VERT_AREF: return xr_new<CBlender_deffer_aref>(false);
    case B_SCREEN_SET: return xr_new<CBlender_Screen_SET>();
    case B_LmEbB: return xr_new<CBlender_LmEbB>();
    case B_LmBmmD: return xr_new<CBlender_BmmD>();
    case B_BmmD: return xr_new<CBlender_BmmD>();
    case B_MODEL: return xr_new<CBlender_deffer_model>();
    case B_MODEL_EbB: return xr_new<CBlender_Model_EbB>();
    case B_DETAIL: return xr_new<CBlender_Detail_Still>();
    case B_TREE: return xr_new<CBlender_Tree>();
    case B_PARTICLE: return xr_new<CBlender_Particle>();
    case B_EDITOR_WIRE: return xr_new<CBlender_Editor_Wire>();
    case B_EDITOR_SEL: return xr_new<CBlender_Editor_Selection>();

    default: return nullptr;
    }
}

void CRender::blender_destroy(IBlenderXr*& B) { xr_delete(B); }
