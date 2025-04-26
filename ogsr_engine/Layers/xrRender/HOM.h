// HOM.h: interface for the CHOM class.
//
//////////////////////////////////////////////////////////////////////
#pragma once

#include "../../xr_3da/IGame_Persistent.h"

class occTri;

class CHOM : public pureRender
{
private:
    xrXRC xrc;
    CDB::MODEL* m_pModel;
    occTri* m_pTris;

    Fmatrix m_xform;
    Fmatrix m_xform_01;

    u32 tris_in_frame_visible;
    u32 tris_in_frame;

    void Render_DB(CFrustum& base);

public:
    void Load();
    void Unload();

    void DispatchRender();

    BOOL visible(vis_data& vis);
    //BOOL visible(Fbox3& B);
    BOOL visible(sPoly& P);
    BOOL visible(Fbox2& B, float depth) const; // viewport-space (0..1)

    bool Allowed() const { return !ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_HOM) && m_pModel; }

    CHOM();
    ~CHOM();

    virtual void OnRender();

    void stats();
};
