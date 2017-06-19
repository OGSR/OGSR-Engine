#include "stdafx.h"
#include "level_debug.h"
#include "../xr_object.h"
#include "level.h"
#include "HUDManager.h"

#ifdef DEBUG
#	include "debug_renderer.h"
#endif

#ifdef DEBUG

CLevelDebug::CLevelDebug()
{

}

CLevelDebug::~CLevelDebug()
{	
	free_mem();
}

CLevelDebug::CObjectInfo &CLevelDebug::object_info(CObject *obj, LPCSTR class_name)
{
	OBJECT_INFO_MAP_IT	obj_it = m_objects_info.find(obj);
	if (obj_it != m_objects_info.end()) {

		CLASS_INFO_MAP_IT	class_it = 	obj_it->second.find(class_name);

		if (class_it != obj_it->second.end()) {
			return (*(class_it->second));
		} else {
			CObjectInfo *new_info = xr_new<CObjectInfo>();
			obj_it->second.insert(mk_pair(class_name, new_info));
			return (*(new_info));
		}
	} else {
		CLASS_INFO_MAP	temp_map;

		CObjectInfo *new_info = xr_new<CObjectInfo>();
		temp_map.insert			(mk_pair(class_name, new_info));
		m_objects_info.insert	(mk_pair(obj, temp_map));

		return (*(new_info));
	}
}

CLevelDebug::CTextInfo &CLevelDebug::text(void *class_ptr, LPCSTR class_name)
{
	SKey key(class_ptr, class_name);

	TEXT_INFO_MAP_IT it = m_text_info.find(key);
	if (it != m_text_info.end()) {
		return (*it->second);
	} else {
		CTextInfo *new_info = xr_new<CTextInfo>();
		m_text_info.insert(mk_pair(key, new_info));
		return (*(new_info));
	}
}

CLevelDebug::CLevelInfo &CLevelDebug::level_info(void *class_ptr, LPCSTR class_name)
{
	SKey key(class_ptr, class_name);

	LEVEL_INFO_MAP_IT it = m_level_info.find(key);
	if (it != m_level_info.end()) {
		return (*it->second);
	} else {
		CLevelInfo *new_info = xr_new<CLevelInfo>();
		m_level_info.insert(mk_pair(key, new_info));
		return (*(new_info));
	}
}


void CLevelDebug::free_mem()
{
	//free object info
	for (OBJECT_INFO_MAP_IT it_obj = m_objects_info.begin(); it_obj != m_objects_info.end(); ++it_obj) {
		for (CLASS_INFO_MAP_IT it_class = it_obj->second.begin(); it_class != it_obj->second.end(); ++it_class){
			xr_delete(it_class->second);
		}
	}

	// free text info 
	for (TEXT_INFO_MAP_IT it = m_text_info.begin(); it != m_text_info.end(); ++it){
		xr_delete(it->second);
	}

	// free text info 
	for (LEVEL_INFO_MAP_IT it = m_level_info.begin(); it != m_level_info.end(); ++it){
		xr_delete(it->second);
	}
}

void CLevelDebug::draw_object_info()
{

	// handle all of the objects
	for (OBJECT_INFO_MAP_IT it = m_objects_info.begin(); it != m_objects_info.end(); ++it) {

		// если объект невалидный - удалить информацию
		if (!it->first || it->first->getDestroy()) {
			for (CLASS_INFO_MAP_IT it_class = it->second.begin(); it_class != it->second.end(); ++it_class){
				xr_delete(it_class->second);
			}
			m_objects_info.erase(it);
			break;
		}

		Fmatrix		res;
		res.mul		(Device.mFullTransform,it->first->XFORM());

		Fvector4	v_res;

		float		delta_height = 0.f;

		// handle all of the classes
		for (CLASS_INFO_MAP_IT class_it = it->second.begin(); class_it != it->second.end(); ++class_it) {

			// get up on 2 meters
			res.transform(v_res, class_it->second->get_shift_pos());

			// check if the object in sight
			if (v_res.z < 0 || v_res.w < 0)										continue;
			if (v_res.x < -1.f || v_res.x > 1.f || v_res.y<-1.f || v_res.y>1.f) continue;

			// get real (x,y)
			float x = (1.f + v_res.x)/2.f * (Device.dwWidth);
			float y = (1.f - v_res.y)/2.f * (Device.dwHeight) - delta_height;
			float start_y = y;

			// handle all of the text inside class
			class_it->second->draw_info(x,y);

			delta_height = start_y - y;
		}


	}
}

void CLevelDebug::draw_text()
{
	// handle all of the classes
	for (TEXT_INFO_MAP_IT it = m_text_info.begin(); it != m_text_info.end(); ++it) {
		it->second->draw_text();
	}
}

void CLevelDebug::draw_level_info()
{
	// handle all of the classes
	for (LEVEL_INFO_MAP_IT it = m_level_info.begin(); it != m_level_info.end(); ++it) {
		it->second->draw_info();
	}
}

//////////////////////////////////////////////////////////////////////////
// CObjectInfo
//////////////////////////////////////////////////////////////////////////


void CLevelDebug::CObjectInfo::add_item	(LPCSTR text, u32 color, u32 id) 
{
	inherited::add_item(SInfoItem(text, color, id));
}

struct DrawInfoPredicate {
	float			x;
	float			y;
	float			delta_height;

	DrawInfoPredicate(float coord_x, float coord_y, float delta_h) {
		x				= coord_x;
		y				= coord_y;
		delta_height	= delta_h;
	}

	void operator() (const CLevelDebug::SInfoItem &s) {
		HUD().Font().pFontMedium->SetAligment(CGameFont::alLeft);
		HUD().Font().pFontMedium->SetColor	(s.color);
		HUD().Font().pFontMedium->OutSet	(x, y-=delta_height);
		HUD().Font().pFontMedium->OutNext	(*(s.text));
	}
};

void CLevelDebug::CObjectInfo::draw_info	(float x, float &y)
{
	DrawInfoPredicate	pred(x,y,m_delta_height);
	process				(pred);	
	y					= pred.y;
}


//////////////////////////////////////////////////////////////////////////
// CTextInfo
//////////////////////////////////////////////////////////////////////////

void CLevelDebug::CTextInfo::add_item	(LPCSTR text, float x, float y, u32 color, u32 id) 
{
	inherited::add_item(STextItem(text, x, y, color, id));
}

struct DrawTextPredicate {
	void operator() (const CLevelDebug::STextItem &s) {
		HUD().Font().pFontMedium->SetAligment(CGameFont::alLeft);
		HUD().Font().pFontMedium->SetColor	(s.color);
		HUD().Font().pFontMedium->OutSet	(s.x, s.y);
		HUD().Font().pFontMedium->OutNext	(*(s.text));
	}
};

void CLevelDebug::CTextInfo::draw_text	()
{
	DrawTextPredicate	pred;
	process				(pred);	
}

//////////////////////////////////////////////////////////////////////////
// CLevelInfo
//////////////////////////////////////////////////////////////////////////

void CLevelDebug::CLevelInfo::add_item(const Fvector &pos, u32 color, u32 id)
{
	inherited::add_item(SLevelItem(pos, color, id));
}

void CLevelDebug::CLevelInfo::add_item(const Fvector &pos1, const Fvector &pos2, u32 color, u32 id)
{
	inherited::add_item(SLevelItem(pos1, pos2, color, id));
}

void CLevelDebug::CLevelInfo::add_item(const Fvector &pos, float radius, u32 color, u32 id)
{
	inherited::add_item(SLevelItem(pos, radius, color, id));
}

struct DrawLevelPredicate {
	void operator() (CLevelDebug::SLevelItem s) {

		if (s.ptype == CLevelDebug::SLevelItem::ePoint) {
			Level().debug_renderer().draw_aabb(s.position1,0.35f,0.35f,0.35f,s.color);

			Fvector upV;
			upV		= s.position1;
			upV.y	+= 5.0f;

			Level().debug_renderer().draw_line(Fidentity,s.position1,upV,s.color);

		} else if (s.ptype == CLevelDebug::SLevelItem::eLine) {
			Level().debug_renderer().draw_line(Fidentity,s.position1,s.position2,s.color);			
		} else if (s.ptype == CLevelDebug::SLevelItem::eBox) {
			Level().debug_renderer().draw_aabb(s.position1,s.radius,s.radius,s.radius,s.color);
		}
	}
};

void CLevelDebug::CLevelInfo::draw_info()
{
	DrawLevelPredicate	pred;
	process				(pred);	
}

void CLevelDebug::on_destroy_object(CObject *obj)
{
	// handle all of the objects
	for (OBJECT_INFO_MAP_IT it = m_objects_info.begin(); it != m_objects_info.end(); ++it) {
		// если объект невалидный - удалить информацию
		if (it->first == obj) {
			for (CLASS_INFO_MAP_IT it_class = it->second.begin(); it_class != it->second.end(); ++it_class){
				xr_delete(it_class->second);
			}
			m_objects_info.erase(it);
			break;
		}
	}
}

#endif
