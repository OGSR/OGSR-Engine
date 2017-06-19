#include "stdafx.h"
#include "map_spot.h"
#include "map_location.h"

#include "ui/UIXmlInit.h"
#include "ui/UIMApWnd.h"
#include "level.h"
#include "../xr_object.h"
#include "object_broker.h"
#include "ui/UITextureMaster.h"

CMapSpot::CMapSpot(CMapLocation* ml)
:m_map_location(ml)
{
	ClipperOn			();
	m_bScale			= false;
}

CMapSpot::~CMapSpot()
{
}

void CMapSpot::Load(CUIXml* xml, LPCSTR path)
{
	CUIXmlInit::InitStatic(*xml,path,0,this);
	int i = xml->ReadAttribInt(path, 0, "scale", 0);
	m_bScale			= (i==1);

	m_originSize		= GetWndSize();
}

LPCSTR CMapSpot::GetHint() 
{
	return MapLocation()->GetHint();
};

void CMapSpot::Update()
{
	inherited::Update();
	if(m_bCursorOverWindow){
		VERIFY(m_dwFocusReceiveTime>=0);
		if( Device.dwTimeGlobal>(m_dwFocusReceiveTime+500) ){
			GetMessageTarget()->SendMessage(this, MAP_SHOW_HINT, NULL);
		}
	}
}

bool CMapSpot::OnMouseDown		(int mouse_btn)
{
/*
	if(left_button){
		GetMessageTarget()->SendMessage(this, MAP_SELECT_SPOT);
		return true;
	}else
		return false;
*/
		return false;
}


void CMapSpot::OnFocusLost		()
{
	inherited::OnFocusLost		();
	GetMessageTarget()->SendMessage(this, MAP_HIDE_HINT, NULL);
}


CMapSpotPointer::CMapSpotPointer(CMapLocation* ml)
:inherited(ml)
{
	ClipperOn();
}

CMapSpotPointer::~CMapSpotPointer()
{
}

LPCSTR CMapSpotPointer::GetHint()
{
	return NULL;
/*
	m_pointer_hint = "to: ";
	m_pointer_hint += inherited::GetHint();
	Fvector2 cam_pos;
	cam_pos.set(Device.vCameraPosition.x,Device.vCameraPosition.z);
	float dist = MapLocation()->Position().distance_to(cam_pos);
	string32 s;
	sprintf_s(s," [%.2f]m.", dist);
	m_pointer_hint += s;
	return m_pointer_hint.c_str();
*/
}

//////////////////////////////////////////////////
CMiniMapSpot::CMiniMapSpot(CMapLocation* ml)
:inherited(ml)
{
}

CMiniMapSpot::~CMiniMapSpot()
{
}

void CMiniMapSpot::Load(CUIXml* xml, LPCSTR path)
{
	inherited::Load(xml,path);

	string256 buf;
	XML_NODE* n = NULL;
	
	Frect base_rect;
	base_rect.x1 = 0;
	base_rect.y1 = 0;
	base_rect.x2 = xml->ReadAttribFlt(path, 0, "width", 0);
	base_rect.y2 = xml->ReadAttribFlt(path, 0, "height", 0);

	Frect _stored_rect = m_UIStaticItem.GetOriginalRect();

	strconcat(sizeof(buf), buf, path, ":texture_above");
	n = xml->NavigateToNode(buf,0);
	if(n){
		LPCSTR texture  = xml->Read(buf, 0, NULL);
		CUITextureMaster::InitTexture	(texture, "hud\\default", &m_UIStaticItem);
		if(strchr(texture,'\\'))
		{
			float x					= xml->ReadAttribFlt(buf, 0, "x", base_rect.x1);
			float y					= xml->ReadAttribFlt(buf, 0, "y", base_rect.y1);
			float width				= xml->ReadAttribFlt(buf, 0, "width", base_rect.width());
			float height			= xml->ReadAttribFlt(buf, 0, "height", base_rect.height());
			m_tex_rect_above.set	(x,y,x+width,y+height);
		}else
			m_tex_rect_above		= m_UIStaticItem.GetOriginalRect();

		m_icon_above				= m_UIStaticItem.GetShader		();
	}

	strconcat(sizeof(buf),buf, path, ":texture_below");
	n = xml->NavigateToNode(buf,0);
	if(n){
		LPCSTR texture  = xml->Read(buf, 0, NULL);
		CUITextureMaster::InitTexture	(texture, "hud\\default", &m_UIStaticItem);
		if(strchr(texture,'\\'))
		{
			float x					= xml->ReadAttribFlt(buf, 0, "x", base_rect.x1);
			float y					= xml->ReadAttribFlt(buf, 0, "y", base_rect.y1);
			float width				= xml->ReadAttribFlt(buf, 0, "width", base_rect.width());
			float height			= xml->ReadAttribFlt(buf, 0, "height", base_rect.height());
			m_tex_rect_below.set	(x,y,x+width,y+height);
		}else
			m_tex_rect_below		= m_UIStaticItem.GetOriginalRect();

		m_icon_below				= m_UIStaticItem.GetShader		();
	}
	strconcat(sizeof(buf),buf, path, ":texture");
	n = xml->NavigateToNode(buf,0);
	if(n){
		LPCSTR texture  = xml->Read(buf, 0, NULL);
		CUITextureMaster::InitTexture	(texture, "hud\\default", &m_UIStaticItem);
		if(strchr(texture,'\\'))
		{
			float x					= xml->ReadAttribFlt(buf, 0, "x", base_rect.x1);
			float y					= xml->ReadAttribFlt(buf, 0, "y", base_rect.y1);
			float width				= xml->ReadAttribFlt(buf, 0, "width", base_rect.width());
			float height			= xml->ReadAttribFlt(buf, 0, "height", base_rect.height());
			m_tex_rect_normal.set	(x,y,x+width,y+height);
		}else
			m_tex_rect_normal		= m_UIStaticItem.GetOriginalRect();

		m_icon_normal				= m_UIStaticItem.GetShader		();
	}

	m_UIStaticItem.SetOriginalRect	(_stored_rect);
}

void CMiniMapSpot::Draw()
{
	CObject* O = Level().CurrentViewEntity();
	if(O&&m_icon_above&&m_icon_below){
		float ml_y = MapLocation()->GetLastPosition().y;
		float d = O->Position().y-ml_y;

		if(d>1.8f){
			GetUIStaticItem().SetShader(m_icon_below);
			GetUIStaticItem().SetOriginalRect(m_tex_rect_below.x1,m_tex_rect_below.y1,m_tex_rect_below.width(),m_tex_rect_below.height());
		}else
		if(d<-1.8f){
			GetUIStaticItem().SetShader(m_icon_above);
			GetUIStaticItem().SetOriginalRect(m_tex_rect_above.x1,m_tex_rect_above.y1,m_tex_rect_above.width(),m_tex_rect_above.height());
		}else{
			GetUIStaticItem().SetShader(m_icon_normal);
			GetUIStaticItem().SetOriginalRect(m_tex_rect_normal.x1,m_tex_rect_normal.y1,m_tex_rect_normal.width(),m_tex_rect_normal.height());
		}
	};

	inherited::Draw();
}
