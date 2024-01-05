#ifndef R_BACKEND_RUNTIMEH
#define R_BACKEND_RUNTIMEH
#pragma once

#include "sh_texture.h"
#include "sh_matrix.h"
#include "sh_constant.h"
#include "sh_rt.h"

#include "../xrRenderDX10/dx10R_Backend_Runtime.h"
#include "../xrRenderDX10/StateManager/dx10State.h"

IC void R_xforms::set_c_w(R_constant* C)
{
    c_w = C;
    RCache.set_c(C, m_w);
};
IC void R_xforms::set_c_invw(R_constant* C)
{
    c_invw = C;
    apply_invw();
};
IC void R_xforms::set_c_v(R_constant* C)
{
    c_v = C;
    RCache.set_c(C, m_v);
};
IC void R_xforms::set_c_p(R_constant* C)
{
    c_p = C;
    RCache.set_c(C, m_p);
};
IC void R_xforms::set_c_wv(R_constant* C)
{
    c_wv = C;
    RCache.set_c(C, m_wv);
};
IC void R_xforms::set_c_vp(R_constant* C)
{
    c_vp = C;
    RCache.set_c(C, m_vp);
};
IC void R_xforms::set_c_wvp(R_constant* C)
{
    c_wvp = C;
    RCache.set_c(C, m_wvp);
};

IC void CBackend::set_xform_world(const Fmatrix& M) { xforms.set_W(M); }
IC void CBackend::set_xform_view(const Fmatrix& M) { xforms.set_V(M); }
IC void CBackend::set_xform_project(const Fmatrix& M) { xforms.set_P(M); }
IC const Fmatrix& CBackend::get_xform_world() { return xforms.get_W(); }
IC const Fmatrix& CBackend::get_xform_view() { return xforms.get_V(); }
IC const Fmatrix& CBackend::get_xform_project() { return xforms.get_P(); }

IC ID3DRenderTargetView* CBackend::get_RT(u32 ID)
{
    VERIFY((ID >= 0) && (ID < 4));

    return pRT[ID];
}

IC ID3DDepthStencilView* CBackend::get_ZB() { return pZB; }

ICF void CBackend::set_States(ID3DState* _state)
{
    PGO(Msg("PGO:state_block"));
#ifdef DEBUG
    stat.states++;
#endif
    state = _state;
    state->Apply();
}


IC void CBackend::set_Element(ShaderElement* S, u32 pass)
{
    SPass& P = *(S->passes[pass]);
    set_States(P.state);
    set_PS(P.ps);
    set_VS(P.vs);
    set_GS(P.gs);
    set_HS(P.hs);
    set_DS(P.ds);
    set_CS(P.cs);

    set_Constants(P.constants);
    set_Textures(P.T);
}

ICF void CBackend::set_Shader(Shader* S, u32 pass) { set_Element(S->E[0], pass); }

#endif
