#include "StdAfx.h"
#include "UIProgressShape.h"

#include "UIStatic.h"

CUIProgressShape::CUIProgressShape(){
	m_pBackground	= NULL;
	m_bText			= false;
	m_pTexture		= xr_new<CUIStatic>();
	AttachChild		(m_pTexture);	
	m_pBackground	= xr_new<CUIStatic>();
	AttachChild		(m_pBackground);
};

CUIProgressShape::~CUIProgressShape(){
	xr_delete		(m_pTexture);
	xr_delete		(m_pBackground);
}	

void CUIProgressShape::SetPos(float pos){
	m_stage					= pos;
}

void CUIProgressShape::SetPos(int pos, int max){
	m_stage					= float(pos)/float(max);
	if (m_bText)
	{
		string256 _buff;
		m_pTexture->SetText(itoa(pos,_buff,10));
	}
}

void CUIProgressShape::SetTextVisible(bool b){
	m_bText = b;
}


void _make_rot(Fvector2& pt, const Fvector2& src, float sin_a, float cos_a, float angle){ 
	pt.x				= src.x*cos_a + src.y*sin_a;
	pt.y				= src.y*cos_a - src.x*sin_a;
}

float calc_color(u32 idx, u32 total, float stage, float max_stage)
{
	float kk			= ( stage/max_stage ) *  (float(total+1));
	float f				= 1/(exp((float(idx)-kk)*0.9f)+1.0f);

	return f;
}

void CUIProgressShape::Draw()
{
	if(m_pBackground)
		m_pBackground->Draw			();
	R_ASSERT						(m_pTexture);
	
	if(m_bText)
		m_pTexture->DrawText		();

	ref_shader sh					= m_pTexture->GetShader();
	ref_geom	gm					= GetUIGeom();
	RCache.set_Shader				(sh);
	CTexture* T						= RCache.get_ActiveTexture(0);
	Fvector2						tsize;
	tsize.set						(float(T->get_Width()),float(T->get_Height()));

	if (0 == m_sectorCount) return;

	u32	offset;
	FVF::TL*	pv					= (FVF::TL*)RCache.Vertex.Lock	(m_sectorCount*3, gm.stride(), offset);

	Frect pos_rect;
	m_pTexture->GetAbsoluteRect		(pos_rect);
	UI()->ClientToScreenScaled		(pos_rect.lt, pos_rect.x1, pos_rect.y1);
	UI()->ClientToScreenScaled		(pos_rect.rb, pos_rect.x2, pos_rect.y2);

	Fvector2						center_pos;
	pos_rect.getcenter				(center_pos);

	Frect tex_rect					= m_pTexture->GetUIStaticItem().GetOriginalRect();
	
	tex_rect.lt.x					/= tsize.x;
	tex_rect.lt.y					/= tsize.y;
	tex_rect.rb.x					/= tsize.x;
	tex_rect.rb.y					/= tsize.y;

	Fvector2						center_tex;
	tex_rect.getcenter				(center_tex);

	float		radius_pos			= pos_rect.width()/2.0f;
	float		radius_tex			= tex_rect.width()/2.0f;

	float		curr_angle			= 0.0f;
	float		sin_a				= _sin(curr_angle);
	float		cos_a				= _cos(curr_angle);
	Fvector2	start_pos_pt,	prev_pos_pt;
	Fvector2	start_tex_pt,	prev_tex_pt;
	

	start_pos_pt.set				(0.0f, -radius_pos);
	prev_pos_pt						= start_pos_pt;

	start_tex_pt.set				(0.0f, -radius_tex);
	prev_tex_pt						= start_tex_pt;

	for(u32 i=0; i<m_sectorCount; ++i)
	{
		float ffff					= calc_color		(i+1, m_sectorCount, m_stage, 1.0f);
		u32 color					= color_argb_f		(ffff,1.0f,1.0f,1.0f); 

		pv->set						(center_pos.x, center_pos.y, color, center_tex.x, center_tex.y);++pv;

		Fvector2	tp;
		tp.set						(prev_pos_pt);
		tp.add						(center_pos);

		Fvector2	tx;
		tx.set						(prev_tex_pt);
		tx.add						(center_tex);

		pv->set						(tp.x, tp.y, color, tx.x, tx.y);++pv;

		if(m_bClockwise)
			curr_angle				-= PI_MUL_2/float(m_sectorCount);
		else
			curr_angle				+= PI_MUL_2/float(m_sectorCount);

		sin_a						= _sin(curr_angle);
		cos_a						= _cos(curr_angle);

		_make_rot					(prev_pos_pt, start_pos_pt, sin_a, cos_a, curr_angle);
		_make_rot					(prev_tex_pt, start_tex_pt, sin_a, cos_a, curr_angle);

		tp.set						(prev_pos_pt);
		tp.add						(center_pos);

		tx.set						(prev_tex_pt);
		tx.add						(center_tex);

		pv->set						(tp.x, tp.y, color, tx.x, tx.y);++pv;

	if(!m_bClockwise)
		std::swap					(*(pv-1), *(pv-2));
	}


	RCache.Vertex.Unlock		(m_sectorCount*3, gm.stride());
	RCache.set_Geometry			(gm);
	
//	if(!m_bClockwise)
//		RCache.set_CullMode			(CULL_NONE);

	RCache.Render				(D3DPT_TRIANGLELIST, offset, m_sectorCount);

//	if(!m_bClockwise)
//		RCache.set_CullMode			(CULL_CCW);
}
