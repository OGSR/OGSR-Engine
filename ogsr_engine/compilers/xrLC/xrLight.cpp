#include "stdafx.h"
#include "build.h"
#include "xrThread.h"
#include "xrSyncronize.h"

xrCriticalSection	task_CS
#ifdef PROFILE_CRITICAL_SECTIONS
	(MUTEX_PROFILE_ID(task_C_S))
#endif // PROFILE_CRITICAL_SECTIONS
;
xr_vector<int>		task_pool;

class CLMThread		: public CThread
{
public:
	HASH			H;
	CDB::COLLIDER	DB;
	base_lighting	LightsSelected;
public:
	CLMThread	(u32 ID) : CThread(ID)
	{
		// thMonitor= TRUE;
		thMessages	= FALSE;
	}

	virtual void	Execute()
	{
		CDeflector* D	= 0;

		for (;;) 
		{
			// Get task
			task_CS.Enter		();
			thProgress			= 1.f - float(task_pool.size())/float(g_deflectors.size());
			if (task_pool.empty())	
			{
				task_CS.Leave		();
				return;
			}

			D					= g_deflectors[task_pool.back()];
			task_pool.pop_back	();
			task_CS.Leave		();

			// Perform operation
			try {
				D->Light	(&DB,&LightsSelected,H);
			} catch (...)
			{
				clMsg("* ERROR: CLMThread::Execute - light");
			}
		}
	}
};

void CBuild::Light()
{
	//****************************************** Implicit
	{
		FPU::m64r		();
		Phase			("LIGHT: Implicit...");
		mem_Compact		();
		ImplicitLighting();
	}

	//****************************************** Lmaps
	{
		FPU::m64r		();
		Phase			("LIGHT: LMaps...");
		mem_Compact		();

		// Randomize deflectors
		std::random_shuffle	(g_deflectors.begin(),g_deflectors.end());
		for					(u32 dit = 0; dit<g_deflectors.size(); dit++)	task_pool.push_back(dit);

		// Main process (4 threads)
		Status			("Lighting...");
		CThreadManager	threads;
		const	u32	thNUM	= 6;
		CTimer	start_time;	start_time.Start();				
		for				(int L=0; L<thNUM; L++)	threads.start(xr_new<CLMThread> (L));
		threads.wait	(500);
		clMsg			("%f seconds",start_time.GetElapsed_sec());
	}

	//****************************************** Vertex
	FPU::m64r		();
	Phase			("LIGHT: Vertex...");
	mem_Compact		();

	LightVertex		();

	//****************************************** Merge LMAPS
	{
		FPU::m64r		();
		Phase			("LIGHT: Merging lightmaps...");
		mem_Compact		();

		xrPhase_MergeLM	();
	}
}

//-----------------------------------------------------------------------
typedef	xr_multimap<float,vecVertex>	mapVert;
typedef	mapVert::iterator				mapVertIt;
mapVert*								g_trans;
xrCriticalSection						g_trans_CS
#ifdef PROFILE_CRITICAL_SECTIONS
	(MUTEX_PROFILE_ID(g_trans_CS))
#endif // PROFILE_CRITICAL_SECTIONS
;

void	g_trans_register_internal		(Vertex* V)
{
	R_ASSERT	(V);

	const float eps		= EPS_L;
	const float eps2	= 2.f*eps;
	
	// Search
	const float key		= V->P.x;
	mapVertIt	it		= g_trans->lower_bound	(key);
	mapVertIt	it2		= it;

	// Decrement to the start and inc to end
	while (it!=g_trans->begin() && ((it->first+eps2)>key)) it--;
	while (it2!=g_trans->end() && ((it2->first-eps2)<key)) it2++;
	if (it2!=g_trans->end())	it2++;
	
	// Search
	for (; it!=it2; it++)
	{
		vecVertex&	VL		= it->second;
		Vertex* Front		= VL.front();
		R_ASSERT			(Front);
		if (Front->P.similar(V->P,eps))
		{
			VL.push_back		(V);
			return;
		}
	}

	// Register
	mapVertIt	ins			= g_trans->insert(mk_pair(key,vecVertex()));
	ins->second.reserve		(32);
	ins->second.push_back	(V);
}
void	g_trans_register	(Vertex* V)
{
	g_trans_CS.Enter			();
	g_trans_register_internal	(V);
	g_trans_CS.Leave			();
}

//////////////////////////////////////////////////////////////////////////
const u32				VLT_END		= u32(-1);
class CVertexLightTasker
{
	xrCriticalSection	cs;
	volatile u32		index;	
public:
	CVertexLightTasker	() : index(0)
#ifdef PROFILE_CRITICAL_SECTIONS
		,cs(MUTEX_PROFILE_ID(CVertexLightTasker))
#endif // PROFILE_CRITICAL_SECTIONS
	{};
	
	void	init		()
	{
		index			= 0;
	}

	u32		get			()
	{
		cs.Enter		();
		u32 _res		=	index;
		if (_res>=g_vertices.size())	_res	=	VLT_END;
		else							index	+=	1;
		cs.Leave		();
		return			_res;
	}
};
CVertexLightTasker		VLT;

class CVertexLightThread : public CThread
{
public:
	CVertexLightThread(u32 ID) : CThread(ID)
	{
		thMessages	= FALSE;
	}
	virtual void		Execute	()
	{
		CDB::COLLIDER	DB;
		DB.ray_options	(0);
		
		u32	counter		= 0;
		for (;; counter++)
		{
			u32 id				= VLT.get();
			if (id==VLT_END)	break;

			Vertex* V		= g_vertices[id];
			R_ASSERT		(V);
			
			// Get transluency factor
			float		v_trans		= 0.f;
			BOOL		bVertexLight= FALSE;
			u32 		L_flags		= 0;
			for (u32 f=0; f<V->adjacent.size(); f++)
			{
				Face*	F								=	V->adjacent		[f];
				v_trans									+=	F->Shader().vert_translucency;
				if	(F->Shader().flags.bLIGHT_Vertex)	bVertexLight		= TRUE;
			}
			v_trans				/=	float(V->adjacent.size());

			// 
			if (bVertexLight)	{
				base_color_c		vC, old;
				V->C._get			(old);
				LightPoint			(&DB, RCAST_Model, vC, V->P, V->N, pBuild->L_static, (b_norgb?LP_dont_rgb:0)|(b_nosun?LP_dont_sun:0)|LP_dont_hemi, 0);
				vC._tmp_			= v_trans;
				vC.mul				(.5f);
				vC.hemi				= old.hemi;			// preserve pre-calculated hemisphere
				V->C._set			(vC);
				g_trans_register	(V);
			}

			thProgress			= float(counter) / float(g_vertices.size());
		}
	}
};

#define NUM_THREADS			4
void CBuild::LightVertex	()
{
	g_trans				= xr_new<mapVert>	();

	// Start threads, wait, continue --- perform all the work
	Status				("Calculating...");
	CThreadManager		Threads;
	VLT.init			();
	CTimer	start_time;	start_time.Start();				
	for (u32 thID=0; thID<NUM_THREADS; thID++)	Threads.start(xr_new<CVertexLightThread>(thID));
	Threads.wait		();
	clMsg				("%f seconds",start_time.GetElapsed_sec());

	// Process all groups
	Status				("Transluenting...");
	for (mapVertIt it=g_trans->begin(); it!=g_trans->end(); it++)
	{
		// Unique
		vecVertex&	VL	= it->second;
		std::sort		(VL.begin(),VL.end());
		VL.erase		(std::unique(VL.begin(),VL.end()),VL.end());

		// Calc summary color
		base_color_c	C;
		for (u32 v=0; v<VL.size(); v++)
		{
			base_color_c	cc;	VL[v]->C._get(cc);
			C.max			(cc);
		}

		// Calculate final vertex color
		for (u32 v=0; v<VL.size(); v++)
		{
			base_color_c		vC;
			VL[v]->C._get		(vC);

			// trans-level
			float	level		= vC._tmp_;

			// 
			base_color_c		R;
			R.lerp				(vC,C,level);
			R.max				(vC);
			VL[v]->C._set		(R);
		}
	}
	xr_delete	(g_trans);
}
