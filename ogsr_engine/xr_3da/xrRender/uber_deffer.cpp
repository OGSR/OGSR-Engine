#include "stdafx.h"
#include "uber_deffer.h"
#include "xrRender_console.h"

void fix_texture_name(LPSTR fn);

Fmatrix sun_near_shadow_xform;
Fmatrix sun_far_shadow_xform;
Fmatrix sun_clouds_shadow_xform;

void	uber_deffer	(CBlender_Compile& C, bool hq, LPCSTR _vspec, LPCSTR _pspec, BOOL _aref, LPCSTR _detail_replace, bool DO_NOT_FINISH)
{
	// Uber-parse
	string256		fname,fnameA,fnameB;
	strcpy			(fname,*C.L_textures[0]);	//. andy if (strext(fname)) *strext(fname)=0;
	fix_texture_name(fname);
	ref_texture		_t;		_t.create			(fname);
	bool			bump	= _t.bump_exist		();

	// detect lmap
	bool			lmap	= true;
	if	(C.L_textures.size()<3)	lmap = false;
	else {
		pcstr		tex		= C.L_textures[2].c_str();
		if (tex[0]=='l' && tex[1]=='m' && tex[2]=='a' && tex[3]=='p')	lmap = true	;
		else															lmap = false;
	}


	string256		ps,vs,dt,dtA,dtB;
	strconcat		(sizeof(vs),vs,"deffer_", _vspec, lmap?"_lmh":""	);
	strconcat		(sizeof(ps),ps,"deffer_", _pspec, lmap?"_lmh":""	);
	strcpy_s		(dt,sizeof(dt),_detail_replace?_detail_replace:( C.detail_texture?C.detail_texture:"" ) );

	if	(_aref)		{ strcat(ps,"_aref");	}

	if	(!bump)		{
		fnameA[0] = fnameB[0] = 0;
		strcat			(vs,"_flat");
		strcat			(ps,"_flat");
		if (hq && (C.bDetail_Diffuse || C.bDetail_Bump) )	{
			strcat		(vs,"_d");
			strcat		(ps,"_d");
		}
	} else {
		strcpy			(fnameA,_t.bump_get().c_str());
		strconcat		(sizeof(fnameB),fnameB,fnameA,"#");
		// KD: forming bump name if detail bump needed
		if (C.bDetail_Bump)
		{
			strcpy_s		(dtA,dt);
			strconcat		(sizeof(dtA),dtA,dtA,"_bump");
			strconcat		(sizeof(dtB),dtB,dtA,"#");
		} else {
			dtA[0] = dtB[0] = 0;
		}
		strcat			(vs,"_bump");
		strcat			(ps,"_bump");
		if (hq && (C.bDetail_Diffuse || C.bDetail_Bump) )	{
			strcat		(vs,"_d"	);
			strcat		(ps,"_d"	);
//			strcat		(dt,"_bump"	);
		}
	}

	// HQ
	if (bump && hq)		{
		// KD: forming new shader names in case of detail bump or parallax
		if (C.bDetail_Bump)
			strcat			(ps,"_db");
		if (C.bParallax)
			strcat			(ps,"_steep");
		strcat			(vs,"-hq");
		strcat			(ps,"-hq");
	}

	// Uber-construct
	C.r_Pass		(vs,ps,	FALSE);
	C.r_Sampler		("s_base",		C.L_textures[0],	false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	C.r_Sampler		("s_bumpX",		fnameB,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);	// should be before base bump
	C.r_Sampler		("s_bump",		fnameA,				false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	C.r_Sampler		("s_bumpD",		dt,					false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	C.r_Sampler		("s_detail",	dt,					false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	// KD: samplers for detail bump registering
	if (bump && hq && C.bDetail_Bump) {
		C.r_Sampler		("s_detailBump",		dtA,		false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
		C.r_Sampler		("s_detailBumpX",		dtB,		false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	}
	if (lmap)C.r_Sampler("s_hemi",	C.L_textures[2],	false,	D3DTADDRESS_CLAMP,	D3DTEXF_LINEAR,		D3DTEXF_NONE,	D3DTEXF_LINEAR);
	if (!DO_NOT_FINISH)		C.r_End	();
};
class cl_various : public R_constant_setup {
	u32			marker;
	Fvector4	result;
	virtual void setup(R_constant* C) {
		//		if (marker!=Device.dwFrame)	{
		BOOL sun = ::Render->is_sun();
		result.set(g_pGamePersistent->Environment().CurrentEnv.moon_road_intensity, (sun==TRUE)?1.f:0.f, ps_r2_sun_near, actor_torch_enabled);
		//		}
		RCache.set_c(C, result);
	}
};	static cl_various		binder_various;
class cl_sns_matrix : public R_constant_setup {
	u32			marker;
	Fmatrix		result;
	virtual void setup(R_constant* C) {
		if (marker != Device.dwFrame) {
			result.set(sun_near_shadow_xform);
		}
		RCache.set_c(C, result);
	}
};	static cl_sns_matrix		binder_sns_matrix;
class cl_sfs_matrix : public R_constant_setup {
	u32			marker;
	Fmatrix		result;
	virtual void setup(R_constant* C) {
		if (marker != Device.dwFrame) {
			result.set(sun_far_shadow_xform);
		}
		RCache.set_c(C, result);
	}
};	static cl_sfs_matrix		binder_sfs_matrix;
class cl_scs_matrix : public R_constant_setup {
	u32			marker;
	Fmatrix		result;
	virtual void setup(R_constant* C) {
		//		if (marker!=Device.dwFrame)	{
		result.set(sun_clouds_shadow_xform);
		//		}
		RCache.set_c(C, result);
	}
};	static cl_scs_matrix		binder_scs_matrix;
void uber_forward(CBlender_Compile& C, bool hq, LPCSTR _vspec, LPCSTR _pspec, u32 _aref, bool env, LPCSTR _detail_replace, bool DO_NOT_FINISH)
{
	// Uber-parse
	string256		fname, fnameA, fnameB;
	strcpy_s(fname, *C.L_textures[0]);	//. andy if (strext(fname)) *strext(fname)=0;
	fix_texture_name(fname);
	ref_texture		_t;		_t.create(fname);
	bool			bump = _t.bump_exist();

	// detect lmap
	bool			lmap = true;
	if (C.L_textures.size()<3)	lmap = false;
	else {
		const char*		tex = C.L_textures[2].c_str();
		if (tex[0] == 'l' && tex[1] == 'm' && tex[2] == 'a' && tex[3] == 'p')	lmap = true;
		else															lmap = false;
	}

	string256		ps, vs, dt, dtA, dtB;
	strconcat(sizeof(vs), vs, "forward_", _vspec, lmap ? "_lmh" : "");
	strconcat(sizeof(ps), ps, "forward_", _pspec, lmap ? "_lmh" : "");
	strcpy_s(dt, sizeof(dt), _detail_replace ? _detail_replace : (C.detail_texture ? C.detail_texture : ""));

	if (!bump) {
		fnameA[0] = fnameB[0] = 0;
		strcat(vs, "_flat");
		strcat(ps, "_flat");
		if (hq && (C.bDetail_Diffuse/* || (C.bFlags & bDetailBump)*/)) {
			strcat(vs, "_d");
			strcat(ps, "_d");
		}
	}
	else {
		strcpy_s(fnameA, _t.bump_get().c_str());
		strconcat(sizeof(fnameB), fnameB, fnameA, "#");
		if (C.bDetail_Bump)
		{
			strcpy_s(dtA, dt);
			strconcat(sizeof(dtA), dtA, dtA, "_bump");
			strconcat(sizeof(dtB), dtB, dtA, "#");
		}
		else {
			dtA[0] = dtB[0] = 0;
		}
		strcat(vs, "_bump");
		strcat(ps, "_bump");
		if (hq && (C.bDetail_Diffuse || (C.bDetail_Bump))) {
			strcat(vs, "_d");
			strcat(ps, "_d");
		}
	}

	// HQ
	if (bump && hq) {
		/*		if (C.bFlags & bDetailBump)
		strcat			(ps,"_db");*/
		if (C.bParallax)
			strcat(ps, "_steep");
		strcat(vs, "-hq");
		strcat(ps, "-hq");
	}

	// Uber-construct
	if (_aref)
		C.r_Pass(vs, ps, FALSE, TRUE, TRUE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, TRUE, _aref);
	else
		C.r_Pass(vs, ps, FALSE, TRUE, TRUE, TRUE, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA, FALSE);

	C.r_Constant("ogse_c_various", &binder_various);
	C.r_Constant("m_shadow_near_ogse", &binder_sns_matrix);
	C.r_Constant("m_shadow_far_ogse", &binder_sfs_matrix);
	C.r_Constant("m_shadow_clouds_ogse", &binder_scs_matrix);

	C.r_Sampler("s_base", C.L_textures[0], false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	C.r_Sampler("s_bumpX", fnameB, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);	// should be before base bump
	C.r_Sampler("s_bump", fnameA, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	//	C.r_Sampler		("s_bumpD",		dt,					false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	C.r_Sampler("s_detail", dt, false, D3DTADDRESS_WRAP, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR, D3DTEXF_ANISOTROPIC);
	/*	if (bump && hq && (C.bFlags & bDetailBump)) {

	C.r_Sampler		("s_detailBump",		dtA,		false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	C.r_Sampler		("s_detailBumpX",		dtB,		false,	D3DTADDRESS_WRAP,	D3DTEXF_ANISOTROPIC,D3DTEXF_LINEAR,	D3DTEXF_ANISOTROPIC);
	}*/
	if (lmap)C.r_Sampler("s_hemi", C.L_textures[2], false, D3DTADDRESS_CLAMP, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR);
	if (env)
	{
		C.r_Sampler("s_env0", r2_T_sky0, false, D3DTADDRESS_CLAMP, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR);
		C.r_Sampler("s_env1", r2_T_sky1, false, D3DTADDRESS_CLAMP, D3DTEXF_LINEAR, D3DTEXF_NONE, D3DTEXF_LINEAR);
	}
	if (!DO_NOT_FINISH)		C.r_End();
}