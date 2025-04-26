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
    cmd_list.set_c(C, m_w);
};
IC void R_xforms::set_c_invw(R_constant* C)
{
    c_invw = C;
    apply_invw();
};
IC void R_xforms::set_c_v(R_constant* C)
{
    c_v = C;
    cmd_list.set_c(C, m_v);
};
IC void R_xforms::set_c_p(R_constant* C)
{
    c_p = C;
    cmd_list.set_c(C, m_p);
};
IC void R_xforms::set_c_wv(R_constant* C)
{
    c_wv = C;
    cmd_list.set_c(C, m_wv);
};
IC void R_xforms::set_c_vp(R_constant* C)
{
    c_vp = C;
    cmd_list.set_c(C, m_vp);
};
IC void R_xforms::set_c_wvp(R_constant* C)
{
    c_wvp = C;
    cmd_list.set_c(C, m_wvp);
};
IC void R_xforms::set_c_w_old(R_constant* C)
{
    c_w_old = C;
    cmd_list.set_c(C, m_w_old);
};
IC void R_xforms::set_c_v_old(R_constant* C)
{
    c_v_old = C;
    cmd_list.set_c(C, m_v_old);
};
IC void R_xforms::set_c_p_old(R_constant* C)
{
    c_p_old = C;
    cmd_list.set_c(C, m_p_old);
};
IC void R_xforms::set_c_wv_old(R_constant* C)
{
    c_wv_old = C;
    cmd_list.set_c(C, m_wv_old);
};
IC void R_xforms::set_c_vp_old(R_constant* C)
{
    c_vp_old = C;
    cmd_list.set_c(C, m_vp_old);
};
IC void R_xforms::set_c_wvp_old(R_constant* C)
{
    c_wvp_old = C;
    cmd_list.set_c(C, m_wvp_old);
};

IC ID3DRenderTargetView* CBackend::get_RT(u32 ID) const
{
    VERIFY((ID >= 0) && (ID < 4));

    return pRT[ID];
}

IC ID3DDepthStencilView* CBackend::get_ZB() const { return pZB; }

ICF void CBackend::set_States(ID3DState* _state)
{
//	DX10 Manages states using it's own algorithm. Don't mess with it.
    {
        PGO(Msg("PGO:state_block"));
        stat.states++;
        state = _state;
        state->Apply(*this);
    }
}

IC void CBackend::set_Pass(SPass* P)
{
    ZoneScoped;

    set_States(P->state);
    set_PS(P->ps);
    set_VS(P->vs);
    set_GS(P->gs);
    set_HS(P->hs);
    set_DS(P->ds);
    set_CS(P->cs);
    set_Constants(P->constants);
    set_Textures(P->T);
}

IC void CBackend::set_Element(ShaderElement* S, u32 pass)
{
    ZoneScoped;

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

IC void CBackend::gpu_mark_begin(const wchar_t* name) { pAnnotation->BeginEvent(name); }

IC void CBackend::gpu_mark_end() { pAnnotation->EndEvent(); }
