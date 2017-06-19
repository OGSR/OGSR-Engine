#include "stdafx.h"
#include "r2.h"
#include "..\fbasicvisual.h"
#include "..\xr_object.h"
#include "..\CustomHUD.h"
#include "..\igame_persistent.h"
#include "..\environment.h"
#include "..\SkeletonCustom.h"
#include "..\xrRender\LightTrack.h"
#include "..\xrRender\xrRender_console.h"
#include "..\Rain.h"

CRender										RImplementation;
#ifndef OGSE_DEBUG
#define USE_SHADER_CACHE
#endif

//////////////////////////////////////////////////////////////////////////
class CGlow				: public IRender_Glow
{
public:
	bool				bActive;
public:
	CGlow() : bActive(false)		{ }
	virtual void					set_active			(bool b)					{ bActive=b;		}
	virtual bool					get_active			()							{ return bActive;	}
	virtual void					set_position		(const Fvector& P)			{ }
	virtual void					set_direction		(const Fvector& D)			{ }
	virtual void					set_radius			(float R)					{ }
	virtual void					set_texture			(LPCSTR name)				{ }
	virtual void					set_color			(const Fcolor& C)			{ }
	virtual void					set_color			(float r, float g, float b)	{ }
};

float		r_dtex_range		= 50.f;
//////////////////////////////////////////////////////////////////////////
ShaderElement*			CRender::rimp_select_sh_dynamic	(IRender_Visual	*pVisual, float cdist_sq)
{
	int		id	= SE_R2_SHADOW;
	if	(CRender::PHASE_NORMAL == RImplementation.phase)
	{
		id = ((_sqrt(cdist_sq)-pVisual->vis.sphere.R)<r_dtex_range)?SE_R2_NORMAL_HQ:SE_R2_NORMAL_LQ;
	}
	return pVisual->shader->E[id]._get();
}
//////////////////////////////////////////////////////////////////////////
ShaderElement*			CRender::rimp_select_sh_static	(IRender_Visual	*pVisual, float cdist_sq)
{
	int		id	= SE_R2_SHADOW;
	if	(CRender::PHASE_NORMAL == RImplementation.phase)
	{
		id = ((_sqrt(cdist_sq)-pVisual->vis.sphere.R)<r_dtex_range)?SE_R2_NORMAL_HQ:SE_R2_NORMAL_LQ;
	}
	return pVisual->shader->E[id]._get();
}
static class cl_parallax		: public R_constant_setup		{	virtual void setup	(R_constant* C)
{
	float			h			=	ps_r2_df_parallax_h;
	RCache.set_c	(C,h,-h/2.f,1.f/r_dtex_range,1.f/r_dtex_range);
}}	binder_parallax;

extern ENGINE_API BOOL r2_sun_static;
//////////////////////////////////////////////////////////////////////////
// Just two static storage
void					CRender::create					()
{
	Device.seqFrame.Add	(this,REG_PRIORITY_HIGH+0x12345678);

	m_skinning			= -1;

	// hardware
	o.smapsize			= 2048;
	o.mrt				= (HW.Caps.raster.dwMRT_count >= 3);
	o.mrtmixdepth		= (HW.Caps.raster.b_MRT_mixdepth);

	// Check for NULL render target support
	D3DFORMAT	nullrt	= (D3DFORMAT)MAKEFOURCC('N','U','L','L');
	o.nullrt			= HW.support	(nullrt,			D3DRTYPE_SURFACE, D3DUSAGE_RENDERTARGET);
	/*
	if (o.nullrt)		{
	Msg				("* NULLRT supported and used");
	};
	*/
	if (o.nullrt)		{
		Msg				("* NULLRT supported");

		//.	    _tzset			();
		//.		??? _strdate	( date, 128 );	???
		//.		??? if (date < 22-march-07)		
		if (0)
		{
			u32 device_id	= HW.Caps.id_device;
			bool disable_nullrt = false;
			switch (device_id)	
			{
			case 0x190:
			case 0x191:
			case 0x192:
			case 0x193:
			case 0x194:
			case 0x197:
			case 0x19D:
			case 0x19E:{
				disable_nullrt = true;	//G80
				break;
					   }
			case 0x400:
			case 0x401:
			case 0x402:
			case 0x403:
			case 0x404:
			case 0x405:
			case 0x40E:
			case 0x40F:{
				disable_nullrt = true;	//G84
				break;
					   }
			case 0x420:
			case 0x421:
			case 0x422:
			case 0x423:
			case 0x424:
			case 0x42D:
			case 0x42E:
			case 0x42F:{
				disable_nullrt = true;	// G86
				break;
					   }
			}
			if (disable_nullrt)	o.nullrt=false;
		};
		if (o.nullrt)	Msg				("* ...and used");
	};


	// SMAP / DST
	o.HW_smap_FETCH4	= FALSE;
	o.HW_smap			= HW.support	(D3DFMT_D24X8,			D3DRTYPE_TEXTURE,D3DUSAGE_DEPTHSTENCIL);
	o.HW_smap_PCF		= o.HW_smap		;
	if (o.HW_smap)		{
		o.HW_smap_FORMAT	= D3DFMT_D24X8;
		Msg				("* HWDST/PCF supported and used");
	}

	o.fp16_filter		= HW.support	(D3DFMT_A16B16G16R16F,	D3DRTYPE_TEXTURE,D3DUSAGE_QUERY_FILTER);
	o.fp16_blend		= HW.support	(D3DFMT_A16B16G16R16F,	D3DRTYPE_TEXTURE,D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING);

	// search for ATI formats
	if (!o.HW_smap && (0==strstr(Core.Params,"-nodf24")) )		{
		o.HW_smap		= HW.support	((D3DFORMAT)(MAKEFOURCC('D','F','2','4')),	D3DRTYPE_TEXTURE,D3DUSAGE_DEPTHSTENCIL);
		if (o.HW_smap)	{
			o.HW_smap_FORMAT= MAKEFOURCC	('D','F','2','4');
			o.HW_smap_PCF	= FALSE			;
			o.HW_smap_FETCH4= TRUE			;
		}
		Msg				("* DF24/F4 supported and used [%X]", o.HW_smap_FORMAT);
	}

	// emulate ATI-R4xx series
	if (strstr(Core.Params,"-r4xx"))	{
		o.mrtmixdepth	= FALSE;
		o.HW_smap		= FALSE;
		o.HW_smap_PCF	= FALSE;
		o.fp16_filter	= FALSE;
		o.fp16_blend	= FALSE;
	}

	VERIFY2				(o.mrt && (HW.Caps.raster.dwInstructions>=256),"Hardware doesn't meet minimum feature-level");
	if (o.mrtmixdepth)		o.albedo_wo		= FALSE	;
	else if (o.fp16_blend)	o.albedo_wo		= FALSE	;
	else					o.albedo_wo		= TRUE	;

	// nvstencil on NV40 and up
	o.nvstencil			= FALSE;
	// KD: we really don't need this on modern cards because of unwanted bugs. 
//	if ((HW.Caps.id_vendor==0x10DE)&&(HW.Caps.id_device>=0x40))	o.nvstencil = TRUE;
//	if (strstr(Core.Params,"-nonvs"))		o.nvstencil	= FALSE;

	// nv-dbt
	o.nvdbt				= HW.support	((D3DFORMAT)MAKEFOURCC('N','V','D','B'), D3DRTYPE_SURFACE, 0);
	if (o.nvdbt)		Msg	("* NV-DBT supported and used");

	// options (smap-pool-size)
	if (strstr(Core.Params,"-smap1536"))	o.smapsize	= 1536;
	if (strstr(Core.Params,"-smap2048"))	o.smapsize	= 2048;
	if (strstr(Core.Params,"-smap2560"))	o.smapsize	= 2560;
	if (strstr(Core.Params,"-smap3072"))	o.smapsize	= 3072;
	if (strstr(Core.Params,"-smap4096"))	o.smapsize	= 4096;
	// KD: additional smap resolutions
	D3DCAPS9 caps;
	CHK_DX(HW.pDevice->GetDeviceCaps(&caps));
	u32 video_mem = HW.pDevice->GetAvailableTextureMem();
	if ((caps.MaxTextureHeight >= 6144) && (video_mem > 512) && strstr(Core.Params,"-smap6144"))
		o.smapsize	= 6144;
	if ((caps.MaxTextureHeight >= 8192) && (video_mem > 512) && strstr(Core.Params,"-smap8192"))
		o.smapsize	= 8192;

	Msg("Shadow Map resolution: %dx%d", o.smapsize, o.smapsize);

	// gloss
	char*	g			= strstr(Core.Params,"-gloss ");
	o.forcegloss		= g?	TRUE	:FALSE	;
	if (g)				{
		o.forcegloss_v		= float	(atoi	(g+xr_strlen("-gloss ")))/255.f;
	}

	// options
//	o.bug				= (strstr(Core.Params,"-bug"))?			TRUE	:FALSE	;
	o.sunfilter			= (strstr(Core.Params,"-sunfilter"))?	TRUE	:FALSE	;
//.	o.sunstatic			= (strstr(Core.Params,"-sunstatic"))?	TRUE	:FALSE	;
	o.sunstatic			= r2_sun_static;
	o.sjitter			= (strstr(Core.Params,"-sjitter"))?		TRUE	:FALSE	;
	o.depth16			= (strstr(Core.Params,"-depth16"))?		TRUE	:FALSE	;
	o.noshadows			= (strstr(Core.Params,"-noshadows"))?	TRUE	:FALSE	;
	o.Tshadows			= (strstr(Core.Params,"-tsh"))?			TRUE	:FALSE	;
	o.mblur				= (strstr(Core.Params,"-mblur"))?		TRUE	:FALSE	;
	o.distortion_enabled= (strstr(Core.Params,"-nodistort"))?	FALSE	:TRUE	;
	o.distortion		= o.distortion_enabled;
	o.disasm			= (strstr(Core.Params,"-disasm"))?		TRUE	:FALSE	;
	o.forceskinw		= (strstr(Core.Params,"-skinw"))?		TRUE	:FALSE	;

	// constants
	::Device.Resources->RegisterConstantSetup	("parallax",	&binder_parallax);

	c_lmaterial					= "L_material";
	c_sbase						= "s_base";

	Target						= xr_new<CRenderTarget>		();	// Main target

	Models						= xr_new<CModelPool>		();
	PSLibrary.OnCreate			();
	HWOCC.occq_create			(occq_size);

	//rmNormal					();
	marker						= 0;
	R_CHK						(HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT,&q_sync_point[0]));
	R_CHK						(HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT,&q_sync_point[1]));

	xrRender_apply_tf			();
	::PortalTraverser.initialize();
}

void					CRender::destroy				()
{
	::PortalTraverser.destroy	();
	_RELEASE					(q_sync_point[1]);
	_RELEASE					(q_sync_point[0]);
	HWOCC.occq_destroy			();
	xr_delete					(Models);
	xr_delete					(Target);
	PSLibrary.OnDestroy			();
	Device.seqFrame.Remove		(this);
}
extern int reset_frame;
void CRender::reset_begin()
{
	// Update incremental shadowmap-visibility solver
	// BUG-ID: 10646
	{
		u32 it=0;
		for (it=0; it<Lights_LastFrame.size(); it++)	{
			if (0==Lights_LastFrame[it])	continue	;
			try {
				Lights_LastFrame[it]->svis.resetoccq ()	;
			} catch (...)
			{
				Msg	("! Failed to flush-OCCq on light [%d] %X",it,*(u32*)(&Lights_LastFrame[it]));
			}
		}
		Lights_LastFrame.clear	();
	}
	reset_frame = Device.dwFrame;
	xr_delete(Target);
	// KD: let's reload details while changed details options on vid_restart
	if (b_loaded && ((dm_current_size != dm_size) || (ps_r__Detail_density	!= ps_current_detail_density)))
	{
		Details->Unload				();
		xr_delete					(Details);
	}

	HWOCC.occq_destroy			();
	_RELEASE					(q_sync_point[1]);
	_RELEASE					(q_sync_point[0]);
}

void CRender::reset_end()
{
	R_CHK						(HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT,&q_sync_point[0]));
	R_CHK						(HW.pDevice->CreateQuery(D3DQUERYTYPE_EVENT,&q_sync_point[1]));
	HWOCC.occq_create			(occq_size);

	Target						=	xr_new<CRenderTarget>	();

	// KD: let's reload details while changed details options on vid_restart
	if (b_loaded && ((dm_current_size != dm_size) || (ps_r__Detail_density	!= ps_current_detail_density)))
	{
		Details						=	xr_new<CDetailManager>	();
		Details->Load();
	}

	xrRender_apply_tf			();
}
/*
void CRender::OnFrame()
{
	Models->DeleteQueue			();
	if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_CALC))	{
		Device.seqParallel.insert	(Device.seqParallel.begin(),
			fastdelegate::FastDelegate0<>(&HOM,&CHOM::MT_RENDER));
	}
}*/
void CRender::OnFrame()
{
	Models->DeleteQueue			();
	if (ps_r2_ls_flags.test(R2FLAG_EXP_MT_CALC))	{
		// MT-details (@front)
		Device.seqParallel.insert	(Device.seqParallel.begin(),
			fastdelegate::FastDelegate0<>(Details,&CDetailManager::MT_CALC));

		// MT-HOM (@front)
		Device.seqParallel.insert	(Device.seqParallel.begin(),
			fastdelegate::FastDelegate0<>(&HOM,&CHOM::MT_RENDER));
	}
}

BOOL CRender::is_sun()
{
	if (o.sunstatic)		return FALSE;
	Fcolor					sun_color			= ((light*)Lights.sun_adapted._get())->color;
	return					(ps_r2_ls_flags.test(R2FLAG_SUN) && (u_diffuse2s(sun_color.r,sun_color.g,sun_color.b)>EPS));
}

// Implementation
IRender_ObjectSpecific*	CRender::ros_create				(IRenderable* parent)				{ return xr_new<CROS_impl>();			}
void					CRender::ros_destroy			(IRender_ObjectSpecific* &p)		{ xr_delete(p);							}
IRender_Visual*			CRender::model_Create			(LPCSTR name, IReader* data)		{ return Models->Create(name,data);		}
IRender_Visual*			CRender::model_CreateChild		(LPCSTR name, IReader* data)		{ return Models->CreateChild(name,data);}
IRender_Visual*			CRender::model_Duplicate		(IRender_Visual* V)					{ return Models->Instance_Duplicate(V);	}
void					CRender::model_Delete			(IRender_Visual* &V, BOOL bDiscard)	{ Models->Delete(V, bDiscard);			}
IRender_DetailModel*	CRender::model_CreateDM			(IReader*	F)
{
	CDetail*	D		= xr_new<CDetail> ();
	D->Load				(F);
	return D;
}
void					CRender::model_Delete			(IRender_DetailModel* & F)
{
	if (F)
	{
		CDetail*	D	= (CDetail*)F;
		D->Unload		();
		xr_delete		(D);
		F				= NULL;
	}
}
IRender_Visual*			CRender::model_CreatePE			(LPCSTR name)	
{ 
	PS::CPEDef*	SE			= PSLibrary.FindPED	(name);		R_ASSERT3(SE,"Particle effect doesn't exist",name);
	return					Models->CreatePE	(SE);
}
IRender_Visual*			CRender::model_CreateParticles	(LPCSTR name)	
{ 
	PS::CPEDef*	SE			= PSLibrary.FindPED	(name);
	if (SE) return			Models->CreatePE	(SE);
	else{
		PS::CPGDef*	SG		= PSLibrary.FindPGD	(name);		R_ASSERT3(SG,"Particle effect or group doesn't exist",name);
		return				Models->CreatePG	(SG);
	}
}
void					CRender::models_Prefetch		()					{ Models->Prefetch	();}
void					CRender::models_Clear			(BOOL b_complete)	{ Models->ClearPool	(b_complete);}

ref_shader				CRender::getShader				(int id)			{ VERIFY(id<int(Shaders.size()));	return Shaders[id];	}
IRender_Portal*			CRender::getPortal				(int id)			{ VERIFY(id<int(Portals.size()));	return Portals[id];	}
IRender_Sector*			CRender::getSector				(int id)			{ VERIFY(id<int(Sectors.size()));	return Sectors[id];	}
IRender_Sector*			CRender::getSectorActive		()					{ return pLastSector;									}
IRender_Visual*			CRender::getVisual				(int id)			{ VERIFY(id<int(Visuals.size()));	return Visuals[id];	}
D3DVERTEXELEMENT9*		CRender::getVB_Format			(int id, BOOL	_alt)	{ 
	if (_alt)	{ VERIFY(id<int(xDC.size()));	return xDC[id].begin();	}
	else		{ VERIFY(id<int(nDC.size()));	return nDC[id].begin(); }
}
IDirect3DVertexBuffer9*	CRender::getVB					(int id, BOOL	_alt)	{
	if (_alt)	{ VERIFY(id<int(xVB.size()));	return xVB[id];		}
	else		{ VERIFY(id<int(nVB.size()));	return nVB[id];		}
}
IDirect3DIndexBuffer9*	CRender::getIB					(int id, BOOL	_alt)	{ 
	if (_alt)	{ VERIFY(id<int(xIB.size()));	return xIB[id];		}
	else		{ VERIFY(id<int(nIB.size()));	return nIB[id];		}
}
FSlideWindowItem*		CRender::getSWI					(int id)			{ VERIFY(id<int(SWIs.size()));		return &SWIs[id];	}
IRender_Target*			CRender::getTarget				()					{ return Target;										}

IRender_Light*			CRender::light_create			()					{ return Lights.Create();								}
IRender_Glow*			CRender::glow_create			()					{ return xr_new<CGlow>();								}

void					CRender::flush					()					{ r_dsgraph_render_graph	(0);						}

BOOL					CRender::occ_visible			(vis_data& P)		{ return HOM.visible(P);								}
BOOL					CRender::occ_visible			(sPoly& P)			{ return HOM.visible(P);								}
BOOL					CRender::occ_visible			(Fbox& P)			{ return HOM.visible(P);								}

void					CRender::add_Visual				(IRender_Visual*		V )	{ add_leafs_Dynamic(V);								}
void					CRender::add_Geometry			(IRender_Visual*		V )	{ add_Static(V,View->getMask());					}
void					CRender::add_StaticWallmark		(ref_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* verts)
{
	if (T->suppress_wm)	return;
	VERIFY2							(_valid(P) && _valid(s) && T && verts && (s>EPS_L), "Invalid static wallmark params");
	Wallmarks->AddStaticWallmark	(T,verts,P,&*S,s);
}

void					CRender::clear_static_wallmarks	()
{
	Wallmarks->clear				();
}

void					CRender::add_SkeletonWallmark	(intrusive_ptr<CSkeletonWallmark> wm)
{
	Wallmarks->AddSkeletonWallmark				(wm);
}
void					CRender::add_SkeletonWallmark	(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size)
{
	Wallmarks->AddSkeletonWallmark				(xf, obj, sh, start, dir, size);
}
void					CRender::add_Occluder			(Fbox2&	bb_screenspace	)
{
	HOM.occlude			(bb_screenspace);
}
void					CRender::set_Object				(IRenderable*	O )	
{ 
	val_pObject				= O;
}
void					CRender::rmNear				()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		=	{0,0,T->get_width(),T->get_height(),0,0.02f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void					CRender::rmFar				()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		=	{0,0,T->get_width(),T->get_height(),0.99999f,1.f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}
void					CRender::rmNormal			()
{
	IRender_Target* T	=	getTarget	();
	D3DVIEWPORT9 VP		= {0,0,T->get_width(),T->get_height(),0,1.f };
	CHK_DX				(HW.pDevice->SetViewport(&VP));
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CRender::CRender()
{
}

CRender::~CRender()
{
}

#include "..\GameFont.h"
void	CRender::Statistics	(CGameFont* _F)
{
	CGameFont&	F	= *_F;
	F.OutNext	(" **** LT:%2d,LV:%2d **** ",stats.l_total,stats.l_visible		);	stats.l_visible = 0;
	F.OutNext	("    S(%2d)   | (%2d)NS   ",stats.l_shadowed,stats.l_unshadowed);
	F.OutNext	("smap use[%2d], merge[%2d], finalclip[%2d]",stats.s_used,stats.s_merged-stats.s_used,stats.s_finalclip);
	stats.s_used = 0; stats.s_merged = 0; stats.s_finalclip = 0;
	F.OutSkip	();
	F.OutNext	(" **** Occ-Q(%03.1f) **** ",100.f*f32(stats.o_culled)/f32(stats.o_queries?stats.o_queries:1));
	F.OutNext	(" total  : %2d",	stats.o_queries	);	stats.o_queries = 0;
	F.OutNext	(" culled : %2d",	stats.o_culled	);	stats.o_culled	= 0;
	F.OutSkip	();
	u32	ict		= stats.ic_total + stats.ic_culled;
	F.OutNext	(" **** iCULL(%03.1f) **** ",100.f*f32(stats.ic_culled)/f32(ict?ict:1));
	F.OutNext	(" visible: %2d",	stats.ic_total	);	stats.ic_total	= 0;
	F.OutNext	(" culled : %2d",	stats.ic_culled	);	stats.ic_culled	= 0;
#ifdef DEBUG
	HOM.stats	();
#endif
}

/////////
#include "D3Dcompiler.h"
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"d3dcompiler.lib")
/*
extern "C"
{
LPCSTR WINAPI	D3DXGetPixelShaderProfile	(LPDIRECT3DDEVICE9  pDevice);
LPCSTR WINAPI	D3DXGetVertexShaderProfile	(LPDIRECT3DDEVICE9	pDevice);
};
*/
HRESULT	CRender::shader_compile(
	LPCSTR							name,
	LPCSTR                          pSrcData,
	UINT                            SrcDataLen,
	void*							_pDefines,
	void*							_pInclude,
	LPCSTR                          pFunctionName,
	LPCSTR                          pTarget,
	DWORD                           Flags,
	void*							_ppShader,
	void*							_ppErrorMsgs,
	void*							_ppConstantTable)
{
	D3DXMACRO						defines[128];
	int								def_it = 0;
	CONST D3DXMACRO*                pDefines = (CONST D3DXMACRO*)	_pDefines;
	char							c_smapsize[32];
	char							c_rmapsize[32];
	char							sunshafts_quality[32];
	char							dof_quality[32];
	char							ssao_quality[32];
	char							aa_quality[32];
	char							parallax_quality[32];
	char							soft_shadows_quality[32];
	char							c_gloss[32];
	char							str[32];
	char							str1[32];
	char							str2[32];
	char							refl_quality[32];
	char							refl_mode[32];
	char							lmap_shadow_mode[32];
	if (pDefines) {
		// transfer existing defines
		for (;; def_it++) {
			if (0 == pDefines[def_it].Name)	break;
			defines[def_it] = pDefines[def_it];
		}
	}
	//	Msg("Compiling shader %s.%s", name, pTarget);
	// options
	{
		sprintf(c_smapsize, "%d", u32(o.smapsize));
		defines[def_it].Name = "SMAP_size";
		defines[def_it].Definition = c_smapsize;
		def_it++;
	}
	if (ps_r2_test_flags.test(R2FLAG_RAIN_MAP)) {
		sprintf(c_rmapsize, "%d", u32(rmap_size));
		defines[def_it].Name = "RMAP_size";
		defines[def_it].Definition = c_rmapsize;
		def_it++;

		/*		sprintf						(c_rmap_dist,"%d",u32(rmap_dist));
		defines[def_it].Name		=	"RMAP_dist";
		defines[def_it].Definition	=	c_rmapsize;
		def_it						++	;*/
	}
	if (o.fp16_filter) {
		defines[def_it].Name = "FP16_FILTER";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.fp16_blend) {
		defines[def_it].Name = "FP16_BLEND";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.HW_smap) {
		defines[def_it].Name = "USE_HWSMAP";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.HW_smap_PCF) {
		defines[def_it].Name = "USE_HWSMAP_PCF";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.HW_smap_FETCH4) {
		defines[def_it].Name = "USE_FETCH4";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.sjitter) {
		defines[def_it].Name = "USE_SJITTER";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (HW.Caps.raster_major >= 3) {
		defines[def_it].Name = "USE_BRANCHING";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (HW.Caps.geometry.bVTF) {
		defines[def_it].Name = "USE_VTF";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.Tshadows) {
		defines[def_it].Name = "USE_TSHADOWS";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.mblur || ps_r2_pp_flags.test(R2PP_FLAG_MBLUR)) {
		defines[def_it].Name = "USE_MBLUR";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.sunfilter) {
		defines[def_it].Name = "USE_SUNFILTER";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.sunstatic) {
		defines[def_it].Name = "USE_R2_STATIC_SUN";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (o.forcegloss) {
		sprintf(c_gloss, "%f", o.forcegloss_v);
		defines[def_it].Name = "FORCE_GLOSS";
		defines[def_it].Definition = c_gloss;
		def_it++;
	}
	if (o.forceskinw) {
		defines[def_it].Name = "SKIN_COLOR";
		defines[def_it].Definition = "1";
		def_it++;
	}
	/*	if (o.nvdbt && ps_r2_ls_flags.test(R2FLAG_USE_NVDBT))			{
	defines[def_it].Name		=	"USE_NVDBT";
	defines[def_it].Definition	=	"1";
	def_it						++	;
	}*/

	// skinning
	if (m_skinning<0) {
		defines[def_it].Name = "SKIN_NONE";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (0 == m_skinning) {
		defines[def_it].Name = "SKIN_0";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (1 == m_skinning) {
		defines[def_it].Name = "SKIN_1";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (2 == m_skinning) {
		defines[def_it].Name = "SKIN_2";
		defines[def_it].Definition = "1";
		def_it++;
	}

	// KD
	if (ps_r2_dof_flags.test(R2FLAG_DOF_COMMON | R2FLAG_DOF_ZOOM | R2FLAG_DOF_RELOAD))
	{
		defines[def_it].Name = "USE_DDOF";
		defines[def_it].Definition = "1";
		def_it++;

		if (ps_r2_dof_flags.test(R2FLAG_DOF_COMMON))
		{
			defines[def_it].Name = "USE_DYNAMIC_DDOF";
			defines[def_it].Definition = "1";
			def_it++;
		}

		if (ps_r2_dof_flags.test(R2FLAG_DOF_ZOOM))
		{
			defines[def_it].Name = "USE_ZOOM_DDOF";
			defines[def_it].Definition = "1";
			def_it++;
		}

		if (ps_r2_dof_flags.test(R2FLAG_DOF_RELOAD))
		{
			defines[def_it].Name = "USE_RELOAD_DDOF";
			defines[def_it].Definition = "1";
			def_it++;
		}

		// quality
		sprintf(dof_quality, "%d", ps_dof_quality);
		defines[def_it].Name = "DDOF_QUALITY";
		defines[def_it].Definition = dof_quality;
		def_it++;
	}
	if (ps_r2_pp_flags.test(R2PP_FLAG_RAIN_DROPS))
	{
		defines[def_it].Name = "USE_RAINDROPS";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (ps_r2_pp_flags.test(R2PP_FLAG_SUNSHAFTS))
	{
		defines[def_it].Name = "USE_SUNSHAFTS";
		defines[def_it].Definition = "1";
		def_it++;

		// quality
		sprintf(sunshafts_quality, "%d", ps_sunshafts_quality);
		defines[def_it].Name = "SUNSHAFTS_QUALITY";
		defines[def_it].Definition = sunshafts_quality;
		def_it++;
	}
	if (ps_r2_pp_flags.test(R2PP_FLAG_SSAO)) {
		defines[def_it].Name = "USE_SSAO";
		defines[def_it].Definition = "1";
		def_it++;

		// quality
		sprintf(ssao_quality, "%d", ps_ssao_quality);
		defines[def_it].Name = "SSAO_QUALITY";
		defines[def_it].Definition = ssao_quality;
		def_it++;
	}

	if (ps_r2_pp_flags.test(R2PP_FLAG_AA)) {
		// quality
		sprintf(aa_quality, "%d", ps_aa_quality);
		defines[def_it].Name = "AA_QUALITY";
		defines[def_it].Definition = aa_quality;
		def_it++;
	}

	if (0 != ps_soft_shadows) {
		sprintf(soft_shadows_quality, "%d", ps_soft_shadows);
		defines[def_it].Name = "SOFT_SHADOWS";
		defines[def_it].Definition = soft_shadows_quality;
		def_it++;
	}
	if (0 != ps_steep_parallax) {
		sprintf(parallax_quality, "%d", ps_steep_parallax);
		defines[def_it].Name = "ALLOW_STEEP_PARALLAX";
		defines[def_it].Definition = parallax_quality;
		def_it++;
	}
	if (ps_r2_ls_flags.test(R2FLAG_SOFT_WATER)) {
		defines[def_it].Name = "USE_SOFT_WATER";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (ps_r2_ls_flags.test(R2FLAG_SOFT_PARTICLES)) {
		defines[def_it].Name = "USE_SOFT_PARTICLES";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (ps_r2_test_flags.test(R2FLAG_HW_INSTANSING)) {
		defines[def_it].Name = "USE_HWI";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (ps_r2_ls_flags.test(R2FLAG_DETAIL_BUMP_DEBUG)) {
		defines[def_it].Name = "DETAIL_BUMP_DEBUG";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (ps_r2_test_flags.test(R2FLAG_REPROJECT)) {
		defines[def_it].Name = "SMAA_REPROJECTION";
		defines[def_it].Definition = "1";
		def_it++;
	}

	if (ps_r2_test_flags.test(R2FLAG_RAIN_MAP)) {
		defines[def_it].Name = "USE_RAIN_MAP";
		defines[def_it].Definition = "1";
		def_it++;
	}

	if (ps_r2_ls_flags.test(R2FLAG_GBUFFER_OPT)) {
		defines[def_it].Name = "USE_GBUFFER_OPTIMIZATION";
		defines[def_it].Definition = "1";
		def_it++;
	}

	if (ps_r2_ls_flags.test(R2FLAG_LENS_FLARES)) {
		defines[def_it].Name = "USE_LENS_FLARES";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (ps_r2_ls_flags.test(R2FLAG_LENS_DIRT)) {
		defines[def_it].Name = "USE_LENS_DIRT";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (ps_r2_ls_flags.test(R2FLAG_AO)) {
		defines[def_it].Name = "USE_AO";
		defines[def_it].Definition = "1";
		def_it++;
	}

	sprintf(str, "%d", u32(ps_r2_blur_params.y));
	defines[def_it].Name = "NUM_OF_BLUR_SAMPLES";
	defines[def_it].Definition = str;
	def_it++;

	if (ps_r2_sharpening > 0.0) {
		sprintf(str1, "%f", ps_r2_sharpening);
		defines[def_it].Name = "IMAGE_SHARPENING";
		defines[def_it].Definition = str1;
		def_it++;
	}
	if (ps_r2_ao_intensity > 0.0) {
		sprintf(str2, "%f", ps_r2_ao_intensity);
		defines[def_it].Name = "AO_INTENSITY";
		defines[def_it].Definition = str2;
		def_it++;
	}
	if (ps_r2_pp_flags.test(R2PP_FLAG_REFLECTIONS)) {
		defines[def_it].Name = "USE_REFLECTIONS";
		defines[def_it].Definition = "1";
		def_it++;

		/*		if (ps_r2_ls_flags.test(R2FLAG_HQ_REFLECTIONS))		{
		defines[def_it].Name		=	"USE_HQ_REFLECTIONS";
		defines[def_it].Definition	=	"1";
		def_it						++;
		}*/

		// quality
		sprintf(refl_quality, "%d", ps_refl_quality);
		defines[def_it].Name = "REFLECTIONS_QUALITY";
		defines[def_it].Definition = refl_quality;
		def_it++;
	}

	// where to draw
	sprintf(refl_mode, "%d", ps_refl_mode);
	defines[def_it].Name = "REFLECTIONS_MODE";
	defines[def_it].Definition = refl_mode;
	def_it++;

	if (ps_r2_ls_flags.test(R2FLAG_WET_SURFACES)) {
		defines[def_it].Name = "USE_WET_SURFACES";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (ps_r2_ls_flags.test(R2FLAG_PUDDLES)) {
		defines[def_it].Name = "USE_PUDDLES";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (ps_r2_test_flags.test(R2FLAG_PCSS)) {
		defines[def_it].Name = "USE_PCSS";
		defines[def_it].Definition = "1";
		def_it++;
	}
	if (ps_r2_pp_flags.test(R2PP_FLAG_LIGHT_SHADOW_FADE)) {
		sprintf(lmap_shadow_mode, "%d", ps_light_shadow_quality);
		defines[def_it].Name = "LAMP_SHADOW_FADE_QUALITY";
		defines[def_it].Definition = lmap_shadow_mode;
		def_it++;
	}

	// finish
	defines[def_it].Name = 0;
	defines[def_it].Definition = 0;
	def_it++;

#ifdef USE_SHADER_CACHE
	char* defs = (char*)Memory.mem_alloc(1024);

	R_ASSERT(defs);

	strconcat(1024, defs, defines[0].Name, defines[0].Definition);

	for (u32 i = 1;; i++) {
		if (0 == defines[i].Name)	break;
		strcat(defs, defines[i].Name);
		strcat(defs, defines[i].Definition);
	}

	string_path			sh_name;
	strconcat(sizeof(sh_name), sh_name, "shader_cache\\", name, ('v' == pTarget[0]) ? ".vs" : ".ps");
	IReader*	R = FS.r_open("$logs$", sh_name);
	if (R)
	{
		//		Msg ("Found saved shader: %s", sh_name);
		char* saved_defs = (char*)Memory.mem_alloc(1024);
		R_ASSERT(saved_defs);
		R->r_stringZ(saved_defs, 1024);
		//		Msg ("SAVED DEFS: %s", saved_defs);
		//		Msg ("DEFS: %s", defs);
		if (!strcmp(defs, saved_defs))
		{
			//			Msg ("Got it!");
			ID3DBlob**                  ppShader = (ID3DBlob**)_ppShader;
			D3DCreateBlob(R->elapsed(), ppShader);
			R->r((*ppShader)->GetBufferPointer(), (int)(*ppShader)->GetBufferSize());
			FS.r_close(R);
			Memory.mem_free((void*)defs);
			Memory.mem_free((void*)saved_defs);
			return D3D_OK;
		}
		else
		{
			Memory.mem_free((void*)saved_defs);
		}
	}

	//	Msg ("NO SAVED SHADER!!: %s", sh_name);

#endif
	// 
	if (0 == xr_strcmp(pFunctionName, "main")) {
		if ('v' == pTarget[0])			pTarget = D3DXGetVertexShaderProfile(HW.pDevice);	// vertex	"vs_2_a"; //	
		else							pTarget = D3DXGetPixelShaderProfile(HW.pDevice);	// pixel	"ps_2_a"; //	
	}

	HRESULT		_result;
	ID3DInclude*                pInclude = (ID3DInclude *)_pInclude;
	ID3DBlob**                  ppShader = (ID3DBlob**)_ppShader;
	ID3DBlob**                  ppErrorMsgs = (ID3DBlob**)_ppErrorMsgs;
	_result = D3DCompile((LPCVOID)pSrcData, (SIZE_T)SrcDataLen, name, (const D3D_SHADER_MACRO *)defines, pInclude, pFunctionName, pTarget,
		D3DCOMPILE_DEBUG | D3DCOMPILE_PACK_MATRIX_ROW_MAJOR/*|D3DCOMPILE_SKIP_OPTIMIZATION*/ | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, ppShader, ppErrorMsgs);

	if (SUCCEEDED(_result))
	{
		ID3DBlob*		code = *((ID3DBlob**)_ppShader);
#ifdef USE_SHADER_CACHE
		string_path			cname;
		strconcat(sizeof(cname), cname, "shader_cache\\", name, ('v' == pTarget[0]) ? ".vs" : ".ps");
		IWriter*			W = FS.w_open("$logs$", cname);
		W->w_stringZ(defs);
		W->w(code->GetBufferPointer(), (int)code->GetBufferSize());
		FS.w_close(W);
		Memory.mem_free((void*)defs);
#endif
		if (o.disasm)
		{
			ID3DBlob*		disasm = 0;
			D3DDisassemble(LPCVOID(code->GetBufferPointer()), code->GetBufferSize(), 0, 0, &disasm);
			string_path			dname;
			strconcat(sizeof(dname), dname, "disasm\\", name, ('v' == pTarget[0]) ? ".vs" : ".ps"/*"_vs.txt":"_ps.txt"*/);
			IWriter*			W = FS.w_open("$logs$", dname);
			W->w(disasm->GetBufferPointer(), (u32)disasm->GetBufferSize());
			FS.w_close(W);
			_RELEASE(disasm);
		}
	}
	else
	{
		VERIFY(ppErrorMsgs);
		Log("! error: ", (LPCSTR)(*((ID3DBlob**)ppErrorMsgs))->GetBufferPointer());
	}

	return		_result;
}

void CRender::set_thermovision_data(Fvector* _w_timers, Fvector4* _w_states)
{
	w_states.set(*_w_states);
	w_timers.set(*_w_timers);
}
BOOL CRender::is_required_lens_dirt() 
{
	return ps_r2_pp_flags.test(R2PP_FLAG_LENS_DIRT_CONTROL); 
};