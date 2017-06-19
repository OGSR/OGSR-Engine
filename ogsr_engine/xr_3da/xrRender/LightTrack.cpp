// LightTrack.cpp: implementation of the CROS_impl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LightTrack.h"
#include "..\xr_object.h"

#ifdef _EDITOR
#include "igame_persistent.h"
#include "environment.h"
#else
#include "..\igame_persistent.h"
#include "..\environment.h"
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CROS_impl::CROS_impl	()
{
	approximate.set		( 0,0,0 );
	dwFrame				= u32(-1);
	shadow_recv_frame	= u32(-1);
	shadow_recv_slot	= -1;

	result_count		= 0;
	result_iterator		= 0;
	result_frame		= u32(-1);
	result_sun			= 0;
	hemi_value			= 0.5f;
	hemi_smooth			= 0.5f;
	sun_value			= 0.2f;
	sun_smooth			= 0.2f;

#if RENDER==R_R1
	MODE				= IRender_ObjectSpecific::TRACE_ALL											;
#else
	MODE				= IRender_ObjectSpecific::TRACE_HEMI + IRender_ObjectSpecific::TRACE_SUN	;
#endif
}

void	CROS_impl::add		(light* source)
{
	// Search
	for (xr_vector<Item>::iterator I=track.begin(); I!=track.end(); I++)	
		if (source == I->source)	{ I->frame_touched = Device.dwFrame; return; }

	// Register _new_
	track.push_back		(Item());
	Item&	L			= track.back();
	L.frame_touched		= Device.dwFrame;
	L.source			= source;
	L.cache.verts[0].set(0,0,0);
	L.cache.verts[1].set(0,0,0);
	L.cache.verts[2].set(0,0,0);
	L.test				= 0.f;
	L.energy			= 0.f;
}

IC bool	pred_energy			(const CROS_impl::Light& L1, const CROS_impl::Light& L2)	{ return L1.energy>L2.energy; }
//////////////////////////////////////////////////////////////////////////
#pragma warning(push)
#pragma warning(disable:4305)
const float		hdir		[lt_hemisamples][3] = 
{
	{0.00000,	1.00000,	0.00000	},
	{0.52573,	0.85065,	0.00000	},
	{0.16246,	0.85065,	0.50000	},
	{-0.42533,	0.85065,	0.30902	},
	{-0.42533,	0.85065,	-0.30902},
	{0.16246,	0.85065,	-0.50000},
	{0.89443,	0.44721,	0.00000	},
	{0.27639,	0.44721,	0.85065	},
	{-0.72361,	0.44721,	0.52573	},
	{-0.72361,	0.44721,	-0.52573},
	{0.27639,	0.44721,	-0.85065},
	{0.68819,	0.52573,	0.50000	},
	{-0.26287,	0.52573,	0.80902	},
	{-0.85065,	0.52573,	-0.00000},
	{-0.26287,	0.52573,	-0.80902},
	{0.68819,	0.52573,	-0.50000},
	{0.95106,	0.00000,	0.30902	},
	{0.58779,	0.00000,	0.80902	},
	{-0.00000,	0.00000,	1.00000	},
	{-0.58779,	0.00000,	0.80902	},
	{-0.95106,	0.00000,	0.30902	},
	{-0.95106,	0.00000,	-0.30902},
	{-0.58779,	0.00000,	-0.80902},
	{0.00000,	0.00000,	-1.00000},
	{0.58779,	0.00000,	-0.80902},
	{0.95106,	0.00000,	-0.30902}
};
#pragma warning(pop)

//////////////////////////////////////////////////////////////////////////
void	CROS_impl::update	(IRenderable* O)
{
	// clip & verify
	if					(dwFrame==Device.dwFrame)			return;
	dwFrame				= Device.dwFrame;
	if					(0==O)								return;
	if					(0==O->renderable.visual)			return;
	VERIFY				(dynamic_cast<CROS_impl*>			(O->renderable_ROS()));
	float	dt			=	Device.fTimeDelta;

	CObject*	_object	= dynamic_cast<CObject*>	(O);

	// select sample, randomize position inside object
	Fvector	position;	O->renderable.xform.transform_tiny	(position,O->renderable.visual->vis.sphere.P);
	float	radius;		radius	= O->renderable.visual->vis.sphere.R;
	position.y			+=  .3f * radius;
	Fvector	direction;	direction.random_dir();
//.			position.mad(direction,0.25f*radius);
//.			position.mad(direction,0.025f*radius);

	// sun-tracing
#if RENDER==R_R1
	light*	sun		=		(light*)RImplementation.L_DB->sun_adapted._get()	;
#else
	light*	sun		=		(light*)RImplementation.Lights.sun_adapted._get()	;
#endif
	if	(MODE & IRender_ObjectSpecific::TRACE_SUN)	{
		if  (--result_sun	< 0)	{
			result_sun		+=		::Random.randI(lt_hemisamples/4,lt_hemisamples/2)	;
			Fvector	direction;	direction.set	(sun->direction).invert().normalize	();
			sun_value		=	!(g_pGameLevel->ObjectSpace.RayTest(position,direction,500.f,collide::rqtBoth,&cache_sun,_object))?1.f:0.f;
		}
	}
	
	// hemi-tracing
	bool	bFirstTime	=	(0==result_count);
	if	(MODE & IRender_ObjectSpecific::TRACE_HEMI)	{
		for (u32 it=0; it<(u32)ps_r2_dhemi_count;	it++)		{	// five samples per one frame
			u32	sample		=	0				;
			if	(result_count<lt_hemisamples)	{ sample=result_count; result_count++;							}
			else								{ sample=(result_iterator%lt_hemisamples); result_iterator++;	}

			// take sample
			Fvector	direction;	direction.set	(hdir[sample][0],hdir[sample][1],hdir[sample][2]).normalize	();
//.			result[sample]	=	!g_pGameLevel->ObjectSpace.RayTest(position,direction,50.f,collide::rqtBoth,&cache[sample],_object);
			result[sample]	=	!g_pGameLevel->ObjectSpace.RayTest(position,direction,50.f,collide::rqtStatic,&cache[sample],_object);
			//	Msg				("%d:-- %s",sample,result[sample]?"true":"false");
		}
	}

	// hemi & sun: update and smooth
//	float	l_f				=	dt*lt_smooth;
//	float	l_i				=	1.f-l_f;
	int		_pass			=	0;
	for (int it=0; it<result_count; it++)	if (result[it])	_pass	++;
	hemi_value				=	float	(_pass)/float(result_count?result_count:1);
	hemi_value				*=	ps_r2_dhemi_scale;
	if (bFirstTime)			hemi_smooth		= hemi_value;
	update_smooth			()	;

	// light-tracing
	BOOL	bTraceLights	= MODE & IRender_ObjectSpecific::TRACE_LIGHTS;
	if		((!O->renderable_ShadowGenerate()) && (!O->renderable_ShadowReceive()))	bTraceLights = FALSE;
	if		(bTraceLights)	{
		// Select nearest lights
		Fvector					bb_size	=	{radius,radius,radius};
		g_SpatialSpace->q_box				(RImplementation.lstSpatial,0,STYPE_LIGHTSOURCE,position,bb_size);
		for (u32 o_it=0; o_it<RImplementation.lstSpatial.size(); o_it++)	{
			ISpatial*	spatial		= RImplementation.lstSpatial[o_it];
			light*		source		= (light*)	(spatial->dcast_Light());
			VERIFY		(source);	// sanity check
			float	R				= radius+source->range;
			if (position.distance_to(source->position) < R)		add	(source);
		}

		// Trace visibility
		lights.clear	();
		float traceR	= radius*.5f;
		for (s32 id=0; id<s32(track.size()); id++)
		{
			// remove untouched lights
			xr_vector<CROS_impl::Item>::iterator I	= track.begin()+id;
			if (I->frame_touched!=Device.dwFrame)	{ track.erase(I) ; id--	; continue ; }

			// Trace visibility
			Fvector				P,D;
			float		amount	= 0;
			light*		xrL		= I->source;
			Fvector&	LP		= xrL->position;
			P.mad				(position,P.random_dir(),traceR);		// Random point inside range

			// point/spot
			float	f			=	D.sub(P,LP).magnitude();
			if (g_pGameLevel->ObjectSpace.RayTest(LP,D.div(f),f,collide::rqtStatic,&I->cache,_object))	amount -=	lt_dec;
			else																						amount +=	lt_inc;
			I->test				+=	amount * dt;	clamp	(I->test,-.5f,1.f);
			I->energy			=	.9f*I->energy + .1f*I->test;

			// 
			float	E			=	I->energy * xrL->color.intensity	();
			if (E > EPS)		{
				// Select light
				lights.push_back			(CROS_impl::Light())		;
				CROS_impl::Light&	L		= lights.back()				;
				L.source					= xrL						;
				L.color.mul_rgb				(xrL->color,I->energy/2)	;
				L.energy					= I->energy/2				;
				if (!xrL->flags.bStatic)	{ L.color.mul_rgb(.5f); L.energy *= .5f; }
			}
		}

		// Sun
		float	E			=	sun_smooth * sun->color.intensity	();
		if (E > EPS)		{
			// Select light
			lights.push_back			(CROS_impl::Light())		;
			CROS_impl::Light&	L		= lights.back()				;
			L.source					= sun						;
			L.color.mul_rgb				(sun->color,sun_smooth/2)	;
			L.energy					= sun_smooth				;
		}

		// Sort lights by importance - important for R1-shadows
		std::sort	(lights.begin(),lights.end(), pred_energy);
	}

	// Process ambient lighting and approximate average lighting
	// Process our lights to find average luminiscense
	CEnvDescriptor&	desc	=	g_pGamePersistent->Environment().CurrentEnv;
	Fvector			accum	=	{ desc.ambient.x,		desc.ambient.y,		desc.ambient.z		};
	Fvector			hemi	=	{ desc.hemi_color.x,	desc.hemi_color.y,	desc.hemi_color.z	};
	Fvector			sun_	=	{ desc.sun_color.x,		desc.sun_color.y,	desc.sun_color.z	};
	if (MODE & IRender_ObjectSpecific::TRACE_HEMI	)	hemi.mul(hemi_smooth); else hemi.mul(.2f);
					accum.add	( hemi );
	if (MODE & IRender_ObjectSpecific::TRACE_SUN	)	sun_.mul(sun_smooth); else sun_.mul(.2f);
					accum.add	( sun_ );
	if (MODE & IRender_ObjectSpecific::TRACE_LIGHTS )	{
		Fvector		lacc	=	{ 0,0,0 };
		for (u32 lit=0; lit<lights.size(); lit++)	{
			float	d	=	lights[lit].source->position.distance_to(position);
			float	r	=	lights[lit].source->range;
			float	a	=	clampr(1.f - d/(r+EPS),0.f,1.f)*(lights[lit].source->flags.bStatic?1.f:2.f);
			lacc.x		+=	lights[lit].color.r*a;
			lacc.y		+=	lights[lit].color.g*a;
			lacc.z		+=	lights[lit].color.b*a;
		}
//		lacc.x		*= desc.lmap_color.x;
//		lacc.y		*= desc.lmap_color.y;
//		lacc.z		*= desc.lmap_color.z;
//		Msg				("- rgb[%f,%f,%f]",lacc.x,lacc.y,lacc.z);
		accum.add		(lacc);
	} else 			accum.set	( .1f, .1f, .1f );
	approximate				=	accum;
}

extern float ps_r2_lt_smooth;

// hemi & sun: update and smooth
void	CROS_impl::update_smooth	(IRenderable* O)
{
	if (dwFrameSmooth == Device.dwFrame)
		return;

	dwFrameSmooth			=	Device.dwFrame					;
	if (O && (0==result_count))	update(O)						;	// First time only
	float	l_f				=	Device.fTimeDelta*ps_r2_lt_smooth;
	clamp	(l_f,0.f,1.f)	;
	float	l_i				=	1.f-l_f							;
	hemi_smooth				=	hemi_value*l_f + hemi_smooth*l_i;
	sun_smooth				=	sun_value *l_f + sun_smooth *l_i;
}
