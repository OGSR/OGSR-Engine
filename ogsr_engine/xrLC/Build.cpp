// Build.cpp: implementation of the CBuild class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "xrThread.h"
#include "xrSyncronize.h"

using namespace			std;

xr_vector<OGF_Base *>	g_tree;
BOOL					b_R2		= FALSE;
BOOL					b_noise		= FALSE;
BOOL					b_radiosity	= FALSE;
BOOL					b_nosun		= FALSE;
// KD start
BOOL					b_norgb		= FALSE;
BOOL					b_nolmaps	= FALSE;
BOOL					b_skipinvalid	= FALSE;
float					f_lmap_quality = 4.f;
// KD end
CThreadManager			mu_base;
CThreadManager			mu_secondary;
#define		MU_THREADS	4
BOOL					gl_linear	= FALSE;

//////////////////////////////////////////////////////////////////////

CBuild::CBuild()
{
}

CBuild::~CBuild()
{
}
 
extern u16		RegisterShader		(LPCSTR T);

// mu-light
class CMULight	: public CThread
{
	u32			low;
	u32			high;
public:
	CMULight	(u32 ID, u32 _low, u32 _high) : CThread(ID)	{	thMessages	= FALSE; low=_low; high=_high;	}

	virtual void	Execute	()
	{
		// Priority
		SetThreadPriority	(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
		Sleep				(0);

		// Light references
		for (u32 m=low; m<high; m++)
		{
			pBuild->mu_refs[m]->calc_lighting	();
			thProgress							= (float(m-low)/float(high-low));
		}
	}
};

class CMUThread : public CThread
{
public:
	CMUThread	(u32 ID) : CThread(ID)
	{
		thMessages	= FALSE;
	}
	virtual void	Execute()
	{
		u32					m;

		// Priority
		SetThreadPriority	(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);
		Sleep				(0);

		// Light models
		for (m=0; m<pBuild->mu_models.size(); m++)
		{
			pBuild->mu_models[m]->calc_materials();
			pBuild->mu_models[m]->calc_lighting	();
		}

		// Light references
		u32	stride			= pBuild->mu_refs.size()/MU_THREADS;
		u32	last			= pBuild->mu_refs.size()-stride*(MU_THREADS-1);
		for (u32 thID=0; thID<MU_THREADS; thID++)
			mu_secondary.start	(xr_new<CMULight> (thID,thID*stride,thID*stride+((thID==(MU_THREADS-1))?last:stride)));
	}
};

void CBuild::Light_prepare()
{
	for (vecFaceIt I=g_faces.begin();	I!=g_faces.end(); I++) (*I)->CacheOpacity();
	for (u32 m=0; m<mu_models.size(); m++)	mu_models[m]->calc_faceopacity();
}


//.#define CFORM_ONLY

void CBuild::Run	(LPCSTR P)
{
	if (strstr(Core.Params,"-att"))	gl_linear	= TRUE;

	//****************************************** Open Level
	strconcat					(sizeof(path),path,P,"\\")	;
	string_path					lfn				;
	IWriter* fs					= FS.w_open		(strconcat(sizeof(lfn),lfn,path,"level."));
	fs->open_chunk				(fsL_HEADER)	;
	hdrLEVEL H;	
	H.XRLC_version				= XRCL_PRODUCTION_VERSION;
	H.XRLC_quality				= g_params.m_quality;
	fs->w						(&H,sizeof(H));
	fs->close_chunk				();

	//****************************************** Dumb entry in shader-registration
	RegisterShader				("");

	//****************************************** Saving lights
	{
		string256			fn;
		IWriter*		fs	= FS.w_open	(strconcat(sizeof(fn),fn,pBuild->path,"build.lights"));
		fs->w_chunk			(0,&*L_static.rgb.begin(),L_static.rgb.size()*sizeof(R_Light));
		fs->w_chunk			(1,&*L_static.hemi.begin(),L_static.hemi.size()*sizeof(R_Light));
		fs->w_chunk			(2,&*L_static.sun.begin(),L_static.sun.size()*sizeof(R_Light));
		FS.w_close			(fs);
	}

	//****************************************** Optimizing + checking for T-junctions
	FPU::m64r					();
	Phase						("Optimizing...");
	mem_Compact					();
	PreOptimize					();
	CorrectTJunctions			();

	//****************************************** HEMI-Tesselate
	FPU::m64r					();
	Phase						("Adaptive HT...");
	mem_Compact					();
#ifndef CFORM_ONLY
	xrPhase_AdaptiveHT			();
#endif

	//****************************************** Building normals
	FPU::m64r					();
	Phase						("Building normals...");
	mem_Compact					();
	CalcNormals					();
	//SmoothVertColors			(5);

	//****************************************** Collision DB
	//should be after normals, so that double-sided faces gets separated
	FPU::m64r					();
	Phase						("Building collision database...");
	mem_Compact					();
	BuildCForm					();

#ifdef CFORM_ONLY
	return;
#endif

	BuildPortals				(*fs);

	//****************************************** T-Basis
	{
		FPU::m64r					();
		Phase						("Building tangent-basis...");
		xrPhase_TangentBasis		();
		mem_Compact					();
	}

	//****************************************** GLOBAL-RayCast model
	FPU::m64r					();
	Phase						("Building rcast-CFORM model...");
	mem_Compact					();
	Light_prepare				();
	BuildRapid					(TRUE);

	//****************************************** GLOBAL-ILLUMINATION
	if (b_radiosity)			
	{
		FPU::m64r					();
		Phase						("Radiosity-Solver...");
		mem_Compact					();
		Light_prepare				();
		xrPhase_Radiosity			();
	}

	//****************************************** Starting MU
	FPU::m64r					();
	Phase						("LIGHT: Starting MU...");
	mem_Compact					();
	Light_prepare				();
	mu_base.start				(xr_new<CMUThread> (0));

	//****************************************** Resolve materials
	FPU::m64r					();
	Phase						("Resolving materials...");
	mem_Compact					();
	xrPhase_ResolveMaterials	();
	IsolateVertices				(TRUE);

	//****************************************** UV mapping
	{
		FPU::m64r					();
		Phase						("Build UV mapping...");
		mem_Compact					();
		xrPhase_UVmap				();
		IsolateVertices				(TRUE);
	}

	//****************************************** Subdivide geometry
	FPU::m64r					();
	Phase						("Subdividing geometry...");
	mem_Compact					();
	xrPhase_Subdivide			();
	IsolateVertices				(TRUE);

	//****************************************** All lighting + lmaps building and saving
	if (!b_nolmaps)
		Light						();

	//****************************************** Merge geometry
	FPU::m64r					();
	Phase						("Merging geometry...");
	mem_Compact					();
	xrPhase_MergeGeometry		();

	//****************************************** Convert to OGF
	FPU::m64r					();
	Phase						("Converting to OGFs...");
	mem_Compact					();
	Flex2OGF					();

	//****************************************** Wait for MU
	FPU::m64r					();
	Phase						("LIGHT: Waiting for MU-thread...");
	mem_Compact					();
	mu_base.wait				(500);
	mu_secondary.wait			(500);

	//****************************************** Export MU-models
	FPU::m64r					();
	Phase						("Converting MU-models to OGFs...");
	mem_Compact					();
	{
		u32 m;
		Status			("MU : Models...");
		for (m=0; m<mu_models.size(); m++)	{
			mu_models[m]->calc_ogf			();
			mu_models[m]->export_geometry	();
		}

		Status			("MU : References...");
		for (m=0; m<mu_refs.size(); m++)
			mu_refs[m]->export_ogf		();
	}

	//****************************************** Destroy RCast-model
	FPU::m64r		();
	Phase			("Destroying ray-trace model...");
	mem_Compact		();
	xr_delete		(RCAST_Model);

	//****************************************** Build sectors
	FPU::m64r		();
	Phase			("Building sectors...");
	mem_Compact		();
	BuildSectors	();

	//****************************************** Saving MISC stuff
	FPU::m64r		();
	Phase			("Saving...");
	mem_Compact		();
	SaveLights		(*fs);

	fs->open_chunk	(fsL_GLOWS);
	for (u32 i=0; i<glows.size(); i++)
	{
		b_glow&	G	= glows[i];
		fs->w		(&G,4*sizeof(float));
		string1024	sid;
		strconcat	(sizeof(sid),sid,
			shader_render[materials[G.dwMaterial].shader].name,
			"/",
			textures		[materials[G.dwMaterial].surfidx].name
			);
		fs->w_u16	(RegisterShader(sid));
	}
	fs->close_chunk	();

	SaveTREE		(*fs);
	SaveSectors		(*fs);

	err_save		();
}

void CBuild::err_save	()
{
	string_path		log_name;
	strconcat		(sizeof(log_name),log_name,"build_",Core.UserName,".err");
	FS.update_path	(log_name,"$logs$",log_name);

	IWriter*		fs	= FS.w_open(log_name);
	IWriter&		err = *fs;

	// t-junction
	err.open_chunk	(0);
	err.w_u32		(err_tjunction.size()/(1*sizeof(Fvector)));
	err.w			(err_tjunction.pointer(), err_tjunction.size());
	err.close_chunk	();

	// m-edje
	err.open_chunk	(1);
	err.w_u32		(err_multiedge.size()/(2*sizeof(Fvector)));
	err.w			(err_multiedge.pointer(), err_multiedge.size());
	err.close_chunk	();

	// invalid
	err.open_chunk	(2);
	err.w_u32		(err_invalid.size()/(3*sizeof(Fvector)));
	err.w			(err_invalid.pointer(), err_invalid.size());
	err.close_chunk	();
}
