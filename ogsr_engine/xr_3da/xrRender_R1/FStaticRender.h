#pragma once

#include "..\xrRender\r__dsgraph_structure.h"

#include "..\xrRender\PSLibrary.h"

#include "..\xrRender\hom.h"
#include "..\xrRender\detailmanager.h"
#include "glowmanager.h"
#include "..\xrRender\wallmarksengine.h"
#include "fstaticrender_rendertarget.h"
#include "..\xrRender\modelpool.h"

#include "lightShadows.h"
#include "lightProjector.h"
#include "lightPPA.h"
#include "..\xrRender\light_DB.h"

#include "../Fmesh.h"

// definition
class CRender													:	public R_dsgraph_structure
{
public:
	enum	{
		PHASE_NORMAL,
		PHASE_POINT,
		PHASE_SPOT
	};
	struct		_options	{
		u32		vis_intersect		: 1;	// config

		u32		distortion			: 1;	// run-time modified
		u32		disasm				: 1;	// config
		u32		forceskinw			: 1;	// config
	}			o;
	struct		_stats		{
		u32		o_queries,	o_culled;
	}			stats;
public:
	// Sector detection and visibility
	CSector*													pLastSector;
	Fvector														vLastCameraPos;
	u32															uLastLTRACK;
	xr_vector<IRender_Portal*>									Portals;
	xr_vector<IRender_Sector*>									Sectors;
	xrXRC														Sectors_xrc;
	CDB::MODEL*													rmPortals;
	CHOM														HOM;
//.	R_occlusion													HWOCC;
	
	// Global containers
	xr_vector<FSlideWindowItem>									SWIs;
	xr_vector<ref_shader>										Shaders;
	typedef svector<D3DVERTEXELEMENT9,MAXD3DDECLLENGTH+1>		VertexDeclarator;
	xr_vector<VertexDeclarator>									DCL;
	xr_vector<IDirect3DVertexBuffer9*>							VB;
	xr_vector<IDirect3DIndexBuffer9*>							IB;
	xr_vector<IRender_Visual*>									Visuals;
	CPSLibrary													PSLibrary;

	CLight_DB*													L_DB;
	CLightR_Manager*											L_Dynamic;
	CLightShadows*												L_Shadows;
	CLightProjector*											L_Projector;
	CGlowManager*												L_Glows;
	CWallmarksEngine*											Wallmarks;
	CDetailManager*												Details;
	CModelPool*													Models;

	CRenderTarget*												Target;			// Render-target

	// R1-specific global constants
	Fmatrix														r1_dlight_tcgen			;
	light*														r1_dlight_light			;
	float														r1_dlight_scale			;
	cl_light_PR													r1_dlight_binder_PR		;
	cl_light_C													r1_dlight_binder_color	;
	cl_light_XFORM												r1_dlight_binder_xform	;
	shared_str													c_ldynamic_props		;
private:
	// Loading / Unloading
	void								LoadBuffers				(CStreamReader	*fs);
	void								LoadVisuals				(IReader *fs);
	void								LoadLights				(IReader *fs);
	void								LoadSectors				(IReader *fs);
	void								LoadSWIs				(CStreamReader	*fs);

	BOOL								add_Dynamic				(IRender_Visual	*pVisual, u32 planes);		// normal processing
	void								add_Static				(IRender_Visual	*pVisual, u32 planes);
	void								add_leafs_Dynamic		(IRender_Visual	*pVisual);					// if detected node's full visibility
	void								add_leafs_Static		(IRender_Visual	*pVisual);					// if detected node's full visibility

public:
	ShaderElement*						rimp_select_sh_static	(IRender_Visual	*pVisual, float cdist_sq);
	ShaderElement*						rimp_select_sh_dynamic	(IRender_Visual	*pVisual, float cdist_sq);
	D3DVERTEXELEMENT9*					getVB_Format			(int id);
	IDirect3DVertexBuffer9*				getVB					(int id);
	IDirect3DIndexBuffer9*				getIB					(int id);
	FSlideWindowItem*					getSWI					(int id);
	IRender_Portal*						getPortal				(int id);
	IRender_Sector*						getSectorActive			();
	IRender_Visual*						model_CreatePE			(LPCSTR			name);
	void								ApplyBlur4				(FVF::TL4uv*	dest, u32 w, u32 h, float k);
	void								apply_object			(IRenderable*	O);
	IC void								apply_lmaterial			()				{};
public:
	// feature level
	virtual	GenerationLevel			get_generation			()	{ return IRender_interface::GENERATION_R1; }

	// Loading / Unloading
	virtual	void					create					();
	virtual	void					destroy					();
	virtual	void					reset_begin				();
	virtual	void					reset_end				();

	virtual	void					level_Load				(IReader*);
	virtual void					level_Unload			();
	
	virtual IDirect3DBaseTexture9*	texture_load			(LPCSTR	fname, u32& msize);
	virtual HRESULT					shader_compile			(
		LPCSTR							name,
		LPCSTR                          pSrcData,
		UINT                            SrcDataLen,
		void*							pDefines,
		void*							pInclude,
		LPCSTR                          pFunctionName,
		LPCSTR                          pTarget,
		DWORD                           Flags,
		void*							ppShader,
		void*							ppErrorMsgs,
		void*							ppConstantTable);

	// Information
	virtual void					Statistics				(CGameFont* F);
	virtual LPCSTR					getShaderPath			()									{ return "r1\\";	}
	virtual ref_shader				getShader				(int id);
	virtual IRender_Sector*			getSector				(int id);
	virtual IRender_Visual*			getVisual				(int id);
	virtual IRender_Sector*			detectSector			(const Fvector& P);
	virtual IRender_Target*			getTarget				();
	
	// Main 
	virtual void					flush					();
	virtual void					set_Object				(IRenderable*		O	);
	virtual	void					add_Occluder			(Fbox2&	bb_screenspace	);			// mask screen region as oclluded
	virtual void					add_Visual				(IRender_Visual*	V	);			// add visual leaf (no culling performed at all)
	virtual void					add_Geometry			(IRender_Visual*	V	);			// add visual(s)	(all culling performed)

	// wallmarks
	virtual void					add_StaticWallmark		(ref_shader& S, const Fvector& P, float s, CDB::TRI* T, Fvector* V);
	virtual void					clear_static_wallmarks	();
	virtual void					add_SkeletonWallmark	(intrusive_ptr<CSkeletonWallmark> wm);
	virtual void					add_SkeletonWallmark	(const Fmatrix* xf, CKinematics* obj, ref_shader& sh, const Fvector& start, const Fvector& dir, float size);
	
	//
	virtual IBlender*				blender_create			(CLASS_ID cls);
	virtual void					blender_destroy			(IBlender* &);

	//
	virtual IRender_ObjectSpecific*	ros_create				(IRenderable* parent);
	virtual void					ros_destroy				(IRender_ObjectSpecific* &);

	// Particle library
	virtual CPSLibrary*				ps_library				(){return &PSLibrary;}

	// Lighting
	virtual IRender_Light*			light_create			();
	virtual IRender_Glow*			glow_create				();
	
	// Models
	virtual IRender_Visual*			model_CreateParticles	(LPCSTR name);
	virtual IRender_DetailModel*	model_CreateDM			(IReader*F);
	virtual IRender_Visual*			model_Create			(LPCSTR name, IReader*data=0);
	virtual IRender_Visual*			model_CreateChild		(LPCSTR name, IReader*data);
	virtual IRender_Visual*			model_Duplicate			(IRender_Visual*	V);
	virtual void					model_Delete			(IRender_Visual* &	V, BOOL bDiscard);
	virtual void 					model_Delete			(IRender_DetailModel* & F);
	virtual void					model_Logging			(BOOL bEnable)				{ Models->Logging(bEnable);	}
	virtual void					models_Prefetch			();
	virtual void					models_Clear			(BOOL b_complete);
	
	// Occlusion culling
	virtual BOOL					occ_visible				(vis_data&	V);
	virtual BOOL					occ_visible				(Fbox&		B);
	virtual BOOL					occ_visible				(sPoly&		P);
	
	// Main
	virtual void					Calculate				();
	virtual void					Render					();
	virtual void					Screenshot				(ScreenshotMode mode=SM_NORMAL, LPCSTR name = 0);
	virtual void					OnFrame					();
	
	// Render mode
	virtual void					rmNear					();
	virtual void					rmFar					();
	virtual void					rmNormal				();

	// KD: need to know, what R2 phase is active now
	virtual u32						active_phase				()	{return phase;};
	virtual BOOL					is_sun						()	{return false;};
	void							set_thermovision_data		(Fvector* w_timers, Fvector4* w_states) {};
	virtual BOOL					is_required_lens_dirt		()	{return false;};

	// Constructor/destructor/loader
	CRender							();
	virtual ~CRender				();
};

extern CRender						RImplementation;
