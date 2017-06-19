// LightTrack.h: interface for the CLightTrack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_)
#define AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_
#pragma once

const	float				lt_inc			= 4.f	;
const	float				lt_dec			= 2.f	;
const	int					lt_hemisamples	= 26	;

class	CROS_impl			: public IRender_ObjectSpecific
{
public:
	struct	Item			{
		u32					frame_touched		;	// to track creation & removal
		light*				source				;	// 
		collide::ray_cache	cache				;	//
		float				test				;	// note range: (-1[no]..1[yes])
		float				energy				;	//
	};
	struct	Light			{
		light*				source				;
		float				energy				;
		Fcolor				color				;
	};
public:
	// general
	u32						MODE				;
	u32						dwFrame				;
	u32						dwFrameSmooth		;

	// 
	xr_vector<Item>			track				;	// everything what touches
	xr_vector<Light>		lights				;	// 

	bool					result				[lt_hemisamples];
	collide::ray_cache		cache				[lt_hemisamples];
	collide::ray_cache		cache_sun			;
	s32						result_count		;
	u32						result_iterator		;
	u32						result_frame		;
	s32						result_sun			;
public:
	u32						shadow_gen_frame	;
	u32						shadow_recv_frame	;
	int						shadow_recv_slot	;
private:
	float					hemi_value			;
	float					hemi_smooth			;
	float					sun_value			;
	float					sun_smooth			;

	Fvector					approximate			;
public:
	virtual	void			force_mode			(u32 mode)		{ MODE = mode;															};
	virtual float			get_luminocity		()				{ Fvector one; return one.set(1,1,1).div(3.f).dotproduct(approximate);	};
	virtual float			get_luminocity_hemi	()				{ return get_hemi();}

	void					add					(light*			L);
	void					update				(IRenderable*	O);
	void					update_smooth		(IRenderable*	O=0);
	
	ICF	float				get_hemi			()	{
		if (dwFrameSmooth!=Device.dwFrame)		update_smooth();
		return									hemi_smooth;
	}
	ICF	float				get_sun				()	{
		if (dwFrameSmooth!=Device.dwFrame)		update_smooth();
		return									sun_smooth;
	}
	ICF Fvector3&			get_approximate		()	{
		if (dwFrameSmooth!=Device.dwFrame)		update_smooth();
		return									approximate;
	}

	CROS_impl				();
	virtual ~CROS_impl	()	{};
};

#endif // !defined(AFX_LIGHTTRACK_H__89914D61_AC0B_4C7C_BA8C_D7D810738CE7__INCLUDED_)
