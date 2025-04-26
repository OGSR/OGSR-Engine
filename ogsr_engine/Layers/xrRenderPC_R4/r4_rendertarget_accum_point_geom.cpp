#include "stdafx.h"

#include "../xrRender/du_sphere.h"
#include "../xrRenderDX10/dx10BufferUtils.h"


void CRenderTarget::accum_point_geom_create()
{
    //	u32	dwUsage				= D3DUSAGE_WRITEONLY;

    // vertices
    {
        const u32 vCount = DU_SPHERE_NUMVERTEX;
        const u32 vSize = 3 * 4;

        R_CHK(dx10BufferUtils::CreateVertexBuffer(&g_accum_point_vb, du_sphere_vertices, vCount * vSize));
    }

    // Indices
    {
        const u32 iCount = DU_SPHERE_NUMFACES * 3;

        R_CHK(dx10BufferUtils::CreateIndexBuffer(&g_accum_point_ib, du_sphere_faces, iCount * 2));
    }
}

void CRenderTarget::accum_point_geom_destroy()
{
#ifdef DEBUG
    _SHOW_REF("g_accum_point_ib", g_accum_point_ib);
#endif // DEBUG
    _RELEASE(g_accum_point_ib);
#ifdef DEBUG
    _SHOW_REF("g_accum_point_vb", g_accum_point_vb);
#endif // DEBUG
    _RELEASE(g_accum_point_vb);
}
