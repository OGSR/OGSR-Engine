#include "common.h"

v2p_flat main(v_tree I)
{
    float3x4 m_xform = float3x4(I.m0, I.m1, I.m2);
    float4 consts = I.consts;

    I.Nh = unpack_D3DCOLOR(I.Nh);
    I.T = unpack_D3DCOLOR(I.T);
    I.B = unpack_D3DCOLOR(I.B);

    v2p_flat o;

    // Transform to world coords
    float3 pos = mul(m_xform, I.P);

    float base = m_xform._24; // take base height from matrix
    float H = pos.y - base; // height of vertex (scaled, rotated, etc.)

    float4 f_pos = float4(pos.xyz, 1);
    float4 f_pos_previous = f_pos;

    // Final xform
    float3 Pe = mul(m_V, f_pos);
    float hemi = I.Nh.w * consts.z + consts.w;
    o.hpos = mul(m_VP, f_pos);

    /////////////
    o.hpos_old = mul(m_VP_old, f_pos_previous);
    o.hpos_curr = o.hpos;
    o.hpos.xy = get_taa_jitter(o.hpos);
    /////////////

    float3x3 m_xform_v = mul((float3x3)m_V, (float3x3)m_xform);
    o.N = mul(m_xform_v, unpack_bx2(I.Nh));
    o.tcdh = float4((I.tc * consts).xyyy);
    o.position = float4(Pe, hemi);

#ifdef USE_TDETAIL
    o.tcdbump = o.tcdh * dt_params; // dt tc
#endif

    return o;
}
