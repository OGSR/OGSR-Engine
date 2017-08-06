#pragma once


class CRenderTarget		: public IRender_Target
{
private:
	BOOL				bAvailable;
	u32					rtWidth;
	u32					rtHeight;

	u32					curWidth;
	u32					curHeight;

	ref_rt				RT;
	ref_rt				RT_distort;
	IDirect3DSurface9*	ZB;

	ref_shader			s_postprocess;
	ref_shader			s_postprocess_D;
	ref_geom			g_postprocess;
	
	float				im_noise_time;
	u32					im_noise_shift_w;
	u32					im_noise_shift_h;
	
	float				param_blur;
	float				param_gray;
	float				param_duality_h;
	float				param_duality_v;
	float				param_noise;
	float				param_noise_scale;
	float				param_noise_fps;

	u32					param_color_base;
	u32					param_color_gray;
	u32					param_color_add;

	u32					frame_distort;
public:
	IDirect3DSurface9*	pTempZB;

private:
	BOOL				Create				()	;
	BOOL				NeedPostProcess		()	;
	BOOL				Available			()	{ return bAvailable; }
	BOOL				Perform				()	;

	void				calc_tc_noise		(Fvector2& p0, Fvector2& p1);
	void				calc_tc_duality_ss	(Fvector2& r0, Fvector2& r1, Fvector2& l0, Fvector2& l1);
	void				phase_distortion	();
public:
	CRenderTarget		();
	~CRenderTarget		();

	void				Begin				();
	void				End					();

	virtual void		set_blur			(float	f)		{ param_blur=f;												}
	virtual void		set_gray			(float	f)		{ param_gray=f;												}
	virtual void		set_duality_h		(float	f)		{ param_duality_h=_abs(f);									}
	virtual void		set_duality_v		(float	f)		{ param_duality_v=_abs(f);									}
	virtual void		set_noise			(float	f)		{ param_noise=f;											}
	virtual void		set_noise_scale		(float	f)		{ param_noise_scale=f;										}
	virtual void		set_noise_fps		(float	f)		{ param_noise_fps=_abs(f)+EPS_S;							}

	virtual void		set_color_base		(u32	f)		{ param_color_base=f;										}
	virtual void		set_color_gray		(u32	f)		{ param_color_gray=f;										}
	virtual void		set_color_add		(u32	f)		{ param_color_add=f;										}

	virtual u32			get_width			()				{ return curWidth;											}
	virtual u32			get_height			()				{ return curHeight;											}
};
