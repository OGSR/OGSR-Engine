#include "stdafx.h"
#include "../../xr_3da/cl_intersect.h"
#include "../xrRender/du_cone.h"

BOOL CRenderTarget::enable_scissor(light* L) // true if intersects near plane
{
    // Msg	("%d: %x type(%d), pos(%f,%f,%f)",Device.dwFrame,u32(L),u32(L->flags.type),VPUSH(L->position));

    // Near plane intersection
    BOOL near_intersect = FALSE;
    {
        const Fmatrix& M = Device.mFullTransform;
        Fvector4 plane;
        plane.x = -(M._14 + M._13);
        plane.y = -(M._24 + M._23);
        plane.z = -(M._34 + M._33);
        plane.w = -(M._44 + M._43);
        const float denom = -1.0f / _sqrt(_sqr(plane.x) + _sqr(plane.y) + _sqr(plane.z));
        plane.mul(denom);
        Fplane P;
        P.n.set(plane.x, plane.y, plane.z);
        P.d = plane.w;
        const float p_dist = P.classify(L->spatial.sphere.P) - L->spatial.sphere.R;
        near_intersect = (p_dist <= 0);
    }
#ifdef DEBUG
    {
        Fsphere S;
        S.set(L->spatial.sphere.P, L->spatial.sphere.R);
        dbg_spheres.push_back(mk_pair(S, L->color));
    }
#endif

    // Scissor
    //. disable scissor because some bugs prevent it to work through multi-portals
    //. if (!HW.Caps.bScissor)	return		near_intersect;
    return near_intersect;
}