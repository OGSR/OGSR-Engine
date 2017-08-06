#include "stdafx.h"
#include "..\xrRender\xrRender_console.h"
#include "r2_puddles.h"

#define sqrt_2 1.4142f
#define MAX_PUDDLE_RADIUS 30.f
#define PUDDLES_FLOODING_SPEED 0.5f

void	CRenderTarget::phase_puddles	()
{
	u_setrt(rt_Generic_0,0,0,HW.pBaseZB);
	SPuddle *points = &(Puddles->points[0]);
	bool already_rendered = false;
	for (u32 i = 0; i < Puddles->size; ++i)
	{
//		u32 mask = 0xff;
		SPuddle *point = &(points[i]);
		u32 res				= RImplementation.ViewBase.testSphere_dirty		(point->P, point->radius); // test visibility
		if (res != fcvNone) {			
			if (!already_rendered)
			{
				RCache.set_Stencil		(FALSE);
				RCache.set_CullMode		(CULL_CCW);
				RCache.set_Shader		(s_water);
				already_rendered = true;
			}

			RCache.set_xform_world	(point->xform);
			RCache.set_Geometry		(Puddles->mesh->rm_geom);
			RCache.set_c			("c_depth", point->max_depth, 0, 0, 0);
			RCache.Render			(D3DPT_TRIANGLELIST,Puddles->mesh->vBase,0,Puddles->mesh->vCount,Puddles->mesh->iBase,Puddles->mesh->dwPrimitives);
		}
	}
}