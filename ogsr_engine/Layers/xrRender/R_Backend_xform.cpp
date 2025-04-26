#include "stdafx.h"

#include "r_backend_xform.h"

void R_xforms::set_W(const Fmatrix& m)
{
    m_w.set(m);
    m_wv.mul_43(m_v, m_w);
    m_wvp.mul(m_p, m_wv);
    if (c_w)
        cmd_list.set_c(c_w, m_w);
    if (c_wv)
        cmd_list.set_c(c_wv, m_wv);
    if (c_wvp)
        cmd_list.set_c(c_wvp, m_wvp);
    m_bInvWValid = false;
    if (c_invw)
        apply_invw();
    cmd_list.stat.xforms++;
}
void R_xforms::set_V(const Fmatrix& m)
{
    m_v.set(m);
    m_wv.mul_43(m_v, m_w);
    m_vp.mul(m_p, m_v);
    m_wvp.mul(m_p, m_wv);
    if (c_v)
        cmd_list.set_c(c_v, m_v);
    if (c_vp)
        cmd_list.set_c(c_vp, m_vp);
    if (c_wv)
        cmd_list.set_c(c_wv, m_wv);
    if (c_wvp)
        cmd_list.set_c(c_wvp, m_wvp);
    cmd_list.stat.xforms++;
}
void R_xforms::set_P(const Fmatrix& m)
{
    m_p.set(m);
    m_vp.mul(m_p, m_v);
    m_wvp.mul(m_p, m_wv);
    if (c_p)
        cmd_list.set_c(c_p, m_p);
    if (c_vp)
        cmd_list.set_c(c_vp, m_vp);
    if (c_wvp)
        cmd_list.set_c(c_wvp, m_wvp);
    cmd_list.stat.xforms++;
}

void R_xforms::set_W_old(const Fmatrix& m)
{
    m_w_old.set(m);
    m_wv_old.mul_43(m_v_old, m_w_old);
    m_wvp_old.mul(m_p_old, m_wv_old);

    if (c_w_old)
        cmd_list.set_c(c_w_old, m_w_old);
    if (c_wv_old)
        cmd_list.set_c(c_wv_old, m_wv_old);
    if (c_wvp_old)
        cmd_list.set_c(c_wvp_old, m_wvp_old);
}
void R_xforms::set_V_old(const Fmatrix& m)
{
    m_v_old.set(m);
    m_wv_old.mul_43(m_v_old, m_w_old);
    m_vp_old.mul(m_p_old, m_v_old);
    m_wvp_old.mul(m_p_old, m_wv_old);

    if (c_v_old)
        cmd_list.set_c(c_v_old, m_v_old);
    if (c_vp_old)
        cmd_list.set_c(c_vp_old, m_vp_old);
    if (c_wv_old)
        cmd_list.set_c(c_wv_old, m_wv_old);
    if (c_wvp_old)
        cmd_list.set_c(c_wvp_old, m_wvp_old);
}
void R_xforms::set_P_old(const Fmatrix& m)
{
    m_p_old.set(m);
    m_vp_old.mul(m_p_old, m_v_old);
    m_wvp_old.mul(m_p_old, m_wv_old);
    if (c_p_old)
        cmd_list.set_c(c_p_old, m_p_old);
    if (c_vp_old)
        cmd_list.set_c(c_vp_old, m_vp_old);
    if (c_wvp_old)
        cmd_list.set_c(c_wvp_old, m_wvp_old);
}

void R_xforms::apply_invw()
{
    VERIFY(c_invw);

    if (!m_bInvWValid)
    {
        m_invw.invert_b(m_w);
        m_bInvWValid = true;
    }

    cmd_list.set_c(c_invw, m_invw);
}

void R_xforms::unmap()
{
    c_w = nullptr;
    c_invw = nullptr;
    c_v = nullptr;
    c_p = nullptr;
    c_wv = nullptr;
    c_vp = nullptr;
    c_wvp = nullptr;
    c_w_old = nullptr;
    c_v_old = nullptr;
    c_p_old = nullptr;
    c_wv_old = nullptr;
    c_vp_old = nullptr;
    c_wvp_old = nullptr;
}
R_xforms::R_xforms(CBackend& cmd_list_in) : cmd_list(cmd_list_in)
{
    unmap();
    m_w.identity();
    m_invw.identity();
    m_v.identity();
    m_p.identity();
    m_wv.identity();
    m_vp.identity();
    m_wvp.identity();
    m_w_old.identity();
    m_v_old.identity();
    m_p_old.identity();
    m_wv_old.identity();
    m_vp_old.identity();
    m_wvp_old.identity();
    m_bInvWValid = true;
}
