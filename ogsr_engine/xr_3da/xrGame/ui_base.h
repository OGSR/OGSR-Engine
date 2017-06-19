#pragma once

#define UI_BASE_WIDTH	1024.0f
#define UI_BASE_HEIGHT	768.0f

struct CFontManager;
class CUICursor;


class CDeviceResetNotifier :public pureDeviceReset
{
public:
						CDeviceResetNotifier					()	{Device.seqDeviceReset.Add(this,REG_PRIORITY_NORMAL);};
	virtual				~CDeviceResetNotifier					()	{Device.seqDeviceReset.Remove(this);};
	virtual void		OnDeviceReset							()	{};

};



//---------------------------------------------------------------------------------------
// 2D Frustum & 2D Vertex
//---------------------------------------------------------------------------------------
struct S2DVert{
	Fvector2	pt;
	Fvector2	uv;
				S2DVert		(){}
				S2DVert		(float pX, float pY, float tU, float tV){pt.set(pX,pY);uv.set(tU,tV);}
	void		set			(float pt_x, float pt_y, float uv_x, float uv_y){pt.set(pt_x,pt_y);uv.set(uv_x,uv_y);}
	void		set			(const Fvector2& _pt, const Fvector2& _uv){pt.set(_pt);uv.set(_uv);}
	void		rotate_pt	(const Fvector2& pivot, float cosA, float sinA, float kx);
};
#define UI_FRUSTUM_MAXPLANES	12
#define UI_FRUSTUM_SAFE			(UI_FRUSTUM_MAXPLANES*4)
typedef svector<S2DVert,UI_FRUSTUM_SAFE>		sPoly2D;

class C2DFrustum{//only rect form
	svector<Fplane2,FRUSTUM_MAXPLANES> planes;
	Frect						m_rect;
public:
	void		CreateFromRect	(const Frect& rect);
	sPoly2D*	ClipPoly		(sPoly2D& S, sPoly2D& D) const;
};

class ui_core: public CDeviceResetNotifier
{
	C2DFrustum		m_2DFrustum;
	C2DFrustum		m_2DFrustumPP;
	bool			m_bPostprocess;

	CFontManager*	m_pFontManager;
	CUICursor*		m_pUICursor;

	Fvector2		m_pp_scale_;
	Fvector2		m_scale_;
	Fvector2*		m_current_scale;

	IC float		ClientToScreenScaledX			(float left)				{return left * m_current_scale->x;};
	IC float		ClientToScreenScaledY			(float top)					{return top * m_current_scale->y;};
public:
	xr_stack<Frect> m_Scissors;
	
					ui_core							();
					~ui_core						();
	CFontManager*	Font							()							{return m_pFontManager;}
	CUICursor*		GetUICursor						()							{return m_pUICursor;}

	void			ClientToScreenScaled			(Fvector2& dest, float left, float top);
	void			ClientToScreenScaled			(Fvector2& src_and_dest);
	void			ClientToScreenScaledWidth		(float& src_and_dest);
	void			ClientToScreenScaledHeight		(float& src_and_dest);

	Frect			ScreenRect						();
	const C2DFrustum& ScreenFrustum					(){return (m_bPostprocess)?m_2DFrustumPP:m_2DFrustum;}
	void			PushScissor						(const Frect& r, bool overlapped=false);
	void			PopScissor						();

	void			pp_start						();
	void			pp_stop							();
	void			RenderFont						();

	virtual void	OnDeviceReset					();
	static	bool	is_16_9_mode					();
	shared_str		get_xml_name					(LPCSTR fn);
	float			get_current_kx					();
};

extern CUICursor*	GetUICursor						();
extern ui_core*		UI								();
