// exxZERO Time Stamp AddIn. Document modified at : Thursday, March 07, 2002 14:12:50 , by user : Oles , from computer : OLES
#include "stdafx.h"
#include "HitMarker.h"
#include "../render.h"
#include "../LightAnimLibrary.h"
#include "UIStaticItem.h"

static Fvector2			as_PC[5];
static Fvector2			as_TC[5];
const static u32		as_id[4*3] = {0,1,4,  1,2,4,  2,3,4,  3,0,4};


//--------------------------------------------------------------------
CHitMarker::CHitMarker()
{
	InitShader		(pSettings->r_string("hud_hitmark","hit_mark_texture"));
}

void CHitMarker::InitShader	(LPCSTR tex_name)
{
	hShader2.create	("hud\\default", tex_name);
}

//--------------------------------------------------------------------
CHitMarker::~CHitMarker()
{
	while( m_HitMarks.size() ){
		xr_delete	( m_HitMarks.front() );
		m_HitMarks.pop_front	();
	}
} 
//--------------------------------------------------------------------
const static float fShowTime = 0.5f;
void CHitMarker::Render()
{
	float h1,p1;
	Device.vCameraDirection.getHP	(h1,p1);

	while( m_HitMarks.size() && !m_HitMarks.front()->IsActive() ){
		xr_delete	( m_HitMarks.front() );
		m_HitMarks.pop_front	();
	}

	HITMARKS::iterator it = m_HitMarks.begin();
	HITMARKS::iterator it_e = m_HitMarks.end();
	for(;it!=it_e;++it)
		(*it)->Draw(-h1);

}
//--------------------------------------------------------------------

void CHitMarker::Hit(int id, const Fvector& dir){

	Fvector hit_dir = dir;
	hit_dir.mul(-1.0f);
	m_HitMarks.push_back	(xr_new<SHitMark>(hShader2,hit_dir));
}
//--------------------------------------------------------------------



SHitMark::SHitMark		(const ref_shader& sh, const Fvector& dir)
{
	m_StartTime							= Device.fTimeGlobal;
	m_lanim								= LALib.FindItem("hud_hit_mark");
	m_HitDirection						= dir.getH();
	m_UIStaticItem						= xr_new<CUIStaticItem>();
	m_UIStaticItem->SetShader			(sh);
	m_UIStaticItem->SetPos				(256.0f, 128.0f);
	m_UIStaticItem->SetRect				(.0f, .0f, 512.0f, 512.0f);
}

void SHitMark::UpdateAnim	()
{
	int frame;
	u32 clr			= m_lanim->CalculateRGB(Device.fTimeGlobal-m_StartTime,frame);
	m_UIStaticItem->SetColor		(subst_alpha(m_UIStaticItem->GetColor(), color_get_A(clr)));
}

SHitMark::~SHitMark		()
{
	xr_delete(m_UIStaticItem);
}

bool	SHitMark::IsActive()
{
	return ((Device.fTimeGlobal-m_StartTime) < m_lanim->Length_sec());
}

void	SHitMark::Draw(float cam_dir)
{
	UpdateAnim						();

	float res_h						= cam_dir + m_HitDirection;
	m_UIStaticItem->Render			(res_h);
}
