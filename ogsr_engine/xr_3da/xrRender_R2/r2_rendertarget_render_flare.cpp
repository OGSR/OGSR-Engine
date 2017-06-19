#include "stdafx.h"
#include "..\xrRender\xrRender_console.h"
#include "..\xr_efflensflare.h"

#define BLEND_INC_SPEED 8.0f
#define BLEND_DEC_SPEED 4.0f

extern int reset_frame;
inline void	blend_lerp(float& cur, float tgt, float speed, float dt)
{
	float diff = tgt - cur;
	float diff_a = abs(diff);
	if (diff_a < EPS_S)	return;
	float mot = speed*dt;
	if (mot>diff_a) mot = diff_a;
	cur += (diff / diff_a)*mot;
}
BOOL RayPick_LF(const Fvector& s, const Fvector& d, float& range, collide::rq_target tgt)
{
	BOOL bRes 			= TRUE;
	collide::rq_result	RQ;
	CObject* E 			= g_pGameLevel->CurrentViewEntity();
	bRes 				= g_pGameLevel->ObjectSpace.RayPick( s,d,range,tgt,RQ,E);	
    if (bRes) range 	= RQ.range;
    return bRes;
}

void	CRenderTarget::render_flare(light* L, BOOL is_sun)
{
	if (!rt_flares) return;
//	if (reset_frame==Device.dwFrame || reset_frame==Device.dwFrame - 1)		return;
	phase_flares();

	Fvector vLightDir,vecX,vecY,vecSx,vecSy;

	Fvector vLightPos = L->position;

	vLightDir.sub(vLightPos, Device.vCameraPosition);
	// Calculate the point directly in front of us, on the far clip plane
	float 	fDistance	= vLightDir.magnitude();
	
	Fvector vecPos,vecDir,vecLight;

	Fmatrix	matEffCamPos;
	matEffCamPos.identity();

	// Calculate our position and direction
	matEffCamPos.i.set(Device.vCameraRight);
	matEffCamPos.j.set(Device.vCameraTop);
	matEffCamPos.k.set(Device.vCameraDirection);
	vecPos.set(Device.vCameraPosition);

	vecDir.set(0.0f, 0.0f, 1.0f);
	matEffCamPos.transform_dir(vecDir);
	vecDir.normalize();

	// Figure out if light is behind something else
	vecX.set(1.0f, 0.0f, 0.0f);
	matEffCamPos.transform_dir(vecX);
	vecX.normalize();
	R_ASSERT( _valid(vecX) );

	vecY.crossproduct(vecX, vecDir);
	R_ASSERT( _valid(vecY) );

	Fvector dir = vLightDir;
	dir.normalize();
//	dir.mul(-1);
	float max_dist = fDistance;
	RayPick_LF(Device.vCameraPosition, dir, max_dist, collide::rqtBoth);
	float vis = ((fDistance - max_dist) > 0.2f)?0.0f:1.0f;
	
	blend_lerp(L->fBlend,vis,BLEND_DEC_SPEED,Device.fTimeDelta);
//	Msg("max_dist = %f, fDistance = %f, vis =%f, fBlend = %f", max_dist,fDistance,vis,L->fBlend);
	clamp( L->fBlend, 0.0f, 1.0f );

	vLightDir.normalize();

	if (L->fBlend <= EPS_L) return;

	// Figure out of light (or flare) might be visible
	vecLight.set(vLightPos);

	float fDot = vLightDir.dotproduct(vecDir);

	if(fDot <= 0.01f) return;
	
	Fvector				scr_pos;
	Device.mFullTransform.transform	( scr_pos, vecLight );
	float kx = 1, ky = 1;
	float sun_blend		= 0.5f;
	float sun_max		= 2.5f;
	scr_pos.y			*= -1;

	if (_abs(scr_pos.x) > sun_blend)	kx = ((sun_max - (float)_abs(scr_pos.x))) / (sun_max - sun_blend);
	if (_abs(scr_pos.y) > sun_blend)	ky = ((sun_max - (float)_abs(scr_pos.y))) / (sun_max - sun_blend);

	float fGradientValue;

	if (!((_abs(scr_pos.x) > sun_max) || (_abs(scr_pos.y) > sun_max))){
		fGradientValue	= kx * ky * L->fBlend;
	}else
		fGradientValue	= 0;

	vecSx.mul			(vecX, fGradientValue*(1.f + 0.02f*fDistance)*3.f);
    vecSy.mul			(vecY, fGradientValue*(1.f + 0.02f*fDistance)*3.f);
	Fcolor color;
	color.set			( L->color );
    color.mul_rgba		( fGradientValue*L->fBlend );
    u32 c				= color.get();
	u32					VS_Offset;
	FVF::LIT *pv		= (FVF::LIT*) RCache.Vertex.Lock(4,flare_geom.stride(),VS_Offset);

    pv->set				(vecLight.x+vecSx.x-vecSy.x, vecLight.y+vecSx.y-vecSy.y, vecLight.z+vecSx.z-vecSy.z, c, 0, 0); pv++;
    pv->set				(vecLight.x+vecSx.x+vecSy.x, vecLight.y+vecSx.y+vecSy.y, vecLight.z+vecSx.z+vecSy.z, c, 0, 1); pv++;
    pv->set				(vecLight.x-vecSx.x-vecSy.x, vecLight.y-vecSx.y-vecSy.y, vecLight.z-vecSx.z-vecSy.z, c, 1, 0); pv++;
    pv->set				(vecLight.x-vecSx.x+vecSy.x, vecLight.y-vecSx.y+vecSy.y, vecLight.z-vecSx.z+vecSy.z, c, 1, 1); pv++;

    RCache.Vertex.Unlock	(4,flare_geom.stride());

	RCache.set_xform_world	(Fidentity);
	RCache.set_Geometry		(flare_geom);

	RCache.set_Stencil		(FALSE);
	RCache.set_CullMode		(CULL_NONE);

	RCache.set_Shader		(s_flare);
	RCache.set_c			("flare_params", 0.01f*fDistance, L->is_require_flare()?1.f:0.f, is_sun?1.f:0.f, ps_r2_pp_flags.test(R2PP_FLAG_LENS_DIRT_CONTROL)?1.f:0.f);
	RCache.Render			(D3DPT_TRIANGLELIST,VS_Offset, 0,4,0,2);
}