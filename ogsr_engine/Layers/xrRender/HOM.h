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

    u32 VisibleTriangleCount;
    u32 FrustumTriangleCount;
    u32 CulledOutCount;

    void Render_DB(CFrustum& base);

public:
    void Load();
    void Unload();

    void DispatchRender();

    BOOL visible(vis_data& vis);
    BOOL visible(sPoly& P);

    bool Allowed() const { return !ps_r2_ls_flags_ext.test(R2FLAGEXT_DISABLE_HOM) && m_pModel; }

    CHOM();
    ~CHOM();

    virtual void OnRender();

    void stats();
};
