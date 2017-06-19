#include "stdafx.h"
#include "map_location.h"
#include "map_spot.h"
#include "map_manager.h"

#include "level.h"
#include "../xr_object.h"
#include "ai_space.h"
#include "game_graph.h"
#include "xrServer.h"
#include "xrServer_Objects_ALife_Monsters.h"
#include "ui/UIXmlInit.h"
#include "ui/UIMap.h"
#include "alife_simulator.h"
#include "graph_engine.h"
#include "actor.h"
#include "ai_object_location.h"
#include "alife_object_registry.h"
#include "relation_registry.h"
#include "InventoryOwner.h"
#include "object_broker.h"
#include "string_table.h"
#include "level_changer.h"
#include "actor_memory.h"
#include "visual_memory_manager.h"
#include "location_manager.h"

CMapLocation::CMapLocation(LPCSTR type, u16 object_id)
{
	m_flags.zero			();
	m_level_spot			= NULL;
	m_level_spot_pointer	= NULL;
	m_minimap_spot			= NULL;
	m_minimap_spot_pointer	= NULL;

	m_level_map_spot_border	= NULL;
	m_mini_map_spot_border	= NULL;

	m_objectID				= object_id;
	m_actual_time			= 0;

	LoadSpot				(type, false);
	m_refCount				= 1;
	
	DisablePointer		();

	EnableSpot				();
	m_cached.m_updatedFrame = u32(-1);
}

CMapLocation::~CMapLocation()
{
}

void CMapLocation::destroy()
{
	delete_data(m_level_spot);
	delete_data(m_level_spot_pointer);
	delete_data(m_minimap_spot);
	delete_data(m_minimap_spot_pointer);
	delete_data(m_level_map_spot_border);
	delete_data(m_mini_map_spot_border);
}

CUIXml*	g_uiSpotXml=NULL;
void CMapLocation::LoadSpot(LPCSTR type, bool bReload)
{
	if(!g_uiSpotXml){
		g_uiSpotXml				= xr_new<CUIXml>();
		bool xml_result			= g_uiSpotXml->Init(CONFIG_PATH, UI_PATH, "map_spots.xml");
		R_ASSERT3(xml_result, "xml file not found", "map_spots.xml");
	}

	XML_NODE* node = NULL;
	string512 path_base, path;
//	strconcat(path_base,"map_spots:",type);
	strcpy_s		(path_base,type);
	R_ASSERT3		(g_uiSpotXml->NavigateToNode(path_base,0), "XML node not found in file map_spots.xml", path_base);
	LPCSTR s		= g_uiSpotXml->ReadAttrib(path_base, 0, "hint", "no hint");
	SetHint			(s);
	
	s = g_uiSpotXml->ReadAttrib(path_base, 0, "store", NULL);
	if(s)
		m_flags.set( eSerailizable, TRUE);

	s = g_uiSpotXml->ReadAttrib(path_base, 0, "no_offline", NULL);
	if(s)
		m_flags.set( eHideInOffline, TRUE);

	m_ttl = g_uiSpotXml->ReadAttribInt(path_base, 0, "ttl", 0);
	if(m_ttl>0){
		m_flags.set( eTTL, TRUE);
		m_actual_time = Device.dwTimeGlobal+m_ttl*1000;
	}

	s = g_uiSpotXml->ReadAttrib(path_base, 0, "pos_to_actor", NULL);
	if(s)
		m_flags.set( ePosToActor, TRUE);

	/*
		bReload здесь бесполезен. Поскольку в теге может быть не указан
		level_spot (или другой тек), а при перезагрузке метки, где level_spot есть, память
		выделена из-за bReload не будет и произойдет вылет.
		Например: при смене отношения врага на друга в непосредственной близости от нпс при условии, что
		он был врагом на момент инициализации метки. Real Wolf.
	*/

	strconcat(sizeof(path),path,path_base,":level_map");
	node = g_uiSpotXml->NavigateToNode(path,0);
	if(node){
		LPCSTR str = g_uiSpotXml->ReadAttrib(path, 0, "spot", "");
		if( xr_strlen(str) ){
			if(!m_level_spot)
				m_level_spot = xr_new<CMapSpot>(this);
			m_level_spot->Load(g_uiSpotXml,str);
		}else{
			VERIFY( !(bReload&&m_level_spot) );
		}

		str = g_uiSpotXml->ReadAttrib(path, 0, "pointer", "");
		if( xr_strlen(str) ){
			if(!m_level_spot_pointer)
				m_level_spot_pointer = xr_new<CMapSpotPointer>(this);
			m_level_spot_pointer->Load(g_uiSpotXml,str);
		}else{
			VERIFY( !(bReload&&m_level_spot_pointer) );
		}
	};

	strconcat(sizeof(path),path,path_base,":mini_map");
	node = g_uiSpotXml->NavigateToNode(path,0);
	if(node){
		LPCSTR str = g_uiSpotXml->ReadAttrib(path, 0, "spot", "");
		if( xr_strlen(str) ){
			if(!m_minimap_spot)
				m_minimap_spot = xr_new<CMiniMapSpot>(this);
			m_minimap_spot->Load(g_uiSpotXml,str);
		}else{
			VERIFY( !(bReload&&m_minimap_spot) );
		}

		str = g_uiSpotXml->ReadAttrib(path, 0, "pointer", "");
		if( xr_strlen(str) ){
			if(!m_minimap_spot_pointer)
				m_minimap_spot_pointer = xr_new<CMapSpotPointer>(this);
			m_minimap_spot_pointer->Load(g_uiSpotXml,str);
		}else{
			VERIFY( !(bReload&&m_minimap_spot_pointer) );
		}
	};
	if(NULL==m_minimap_spot && NULL==m_level_spot)
		DisableSpot	();
}

Fvector2 CMapLocation::Position()
{
	if(m_cached.m_updatedFrame==Device.dwFrame) 
		return m_cached.m_Position;

	Fvector2 pos;
	pos.set(0.0f,0.0f);

	if(m_flags.test( ePosToActor) && Level().CurrentEntity()){
		m_position_global = Level().CurrentEntity()->Position();
		pos.set(m_position_global.x, m_position_global.z);
		m_cached.m_Position = pos;
		return pos;
	}

	CObject* pObject =  Level().Objects.net_Find(m_objectID);
	if(!pObject){
		if(ai().get_alife())		
		{
			CSE_ALifeDynamicObject* O = ai().alife().objects().object(m_objectID,true);
			if(O){
				m_position_global = O->draw_level_position();
				pos.set(m_position_global.x, m_position_global.z);
			}
		}
	
	}else{
		//if (GameID() != GAME_SINGLE) Msg("CMapLocation::Position()[%d]", m_objectID);
		m_position_global = pObject->Position();
		pos.set(m_position_global.x, m_position_global.z);
	}

	m_cached.m_Position = pos;
	return pos;
}

Fvector2 CMapLocation::Direction()
{
	if(m_cached.m_updatedFrame==Device.dwFrame) 
		return m_cached.m_Direction;

	Fvector2 res;
	res.set(0.0f,0.0f);

	if(Level().CurrentViewEntity()&&Level().CurrentViewEntity()->ID()==m_objectID ){
		res.set(Device.vCameraDirection.x,Device.vCameraDirection.z);
	}else{
		CObject* pObject =  Level().Objects.net_Find(m_objectID);
		if(!pObject)
			res.set(0.0f, 0.0f);
		else{
			const Fvector& op = pObject->Direction();
			res.set(op.x, op.z);
		}
	}

	if(m_flags.test(ePosToActor)){
		CObject* pObject =  Level().Objects.net_Find(m_objectID);
		if(pObject){
			Fvector2 dcp,obj_pos;
			dcp.set(Device.vCameraPosition.x, Device.vCameraPosition.z);
			obj_pos.set(pObject->Position().x, pObject->Position().z);
			res.sub(obj_pos, dcp);
			res.normalize_safe();
		}
		
	}
	
	m_cached.m_Direction	= res;
	return					res;
}

shared_str CMapLocation::LevelName()
{
	if(m_cached.m_updatedFrame==Device.dwFrame) 
		return m_cached.m_LevelName;

	if(ai().get_alife() && ai().get_game_graph())		
	{
		CSE_Abstract* E = ai().alife().objects().object(m_objectID,true);
		if(!E){
			Msg("- Critical: SMapLocation binded to non-existent object id=%d",m_objectID);
			return "ERROR";
		}
		
		CSE_ALifeObject* AO = smart_cast<CSE_ALifeObject*>(E);
		if(AO){	
			m_cached.m_LevelName = ai().game_graph().header().level(ai().game_graph().vertex(AO->m_tGraphID)->level_id()).name();
			return m_cached.m_LevelName;
		}else{	
			m_cached.m_LevelName = Level().name();
			return m_cached.m_LevelName;
		}
	}else{
		m_cached.m_LevelName = Level().name();
		return m_cached.m_LevelName;
	}
}

bool CMapLocation::Update() //returns actual
{
	if(m_cached.m_updatedFrame==Device.dwFrame) 
		return m_cached.m_Actuality;
		
	if(	m_flags.test(eTTL) ){
		if( m_actual_time < Device.dwTimeGlobal){
			m_cached.m_Actuality		= false;
			m_cached.m_updatedFrame		= Device.dwFrame;
			return						m_cached.m_Actuality;
		}
	}

	CObject* pObject =  Level().Objects.net_Find(m_objectID);
	
	//mp
	if ( GameID()!=GAME_SINGLE && (pObject) ){
			m_cached.m_Actuality		= true;
			Position					();
			Direction					();
			LevelName					();
			m_cached.m_updatedFrame		= Device.dwFrame;
			return						m_cached.m_Actuality;
	}
	
	//single
	if(pObject){
			m_cached.m_Actuality		= true;
			Position					();
			Direction					();
			LevelName					();
			m_cached.m_updatedFrame		= Device.dwFrame;
			return						m_cached.m_Actuality;
	}

	if(ai().get_alife())		
	{
		m_cached.m_Actuality = ( NULL != ai().alife().objects().object(m_objectID,true) );
		if(m_cached.m_Actuality){
			Position					();
			Direction					();
			LevelName					();
		}
		m_cached.m_updatedFrame			= Device.dwFrame;
		return							m_cached.m_Actuality;
	}

	m_cached.m_Actuality				= false;
	m_cached.m_updatedFrame				= Device.dwFrame;
	return								m_cached.m_Actuality;
}

extern xr_vector<CLevelChanger*>	g_lchangers;
xr_vector<u32> map_point_path;

void CMapLocation::UpdateSpot(CUICustomMap* map, CMapSpot* sp )
{
	if( map->MapName()==LevelName() ){
		CSE_ALifeDynamicObject* obj = NULL;
		
		if(ai().get_alife() && !IsUserDefined())
		{
			obj = ai().alife().objects().object(m_objectID,true);
			if(!obj)
			{
				Msg("- Critical: CMapLocation::UpdateSpot binded to non-existent object id=%d",m_objectID);
				return;
			}
		}
		
		if(	m_flags.test(eHideInOffline) && 
			ai().get_alife() && !IsUserDefined() &&
			!obj->m_bOnline )
			return;

		if(	!IsUserDefined() && 
			ai().get_alife() && 
			FALSE == obj->m_flags.test(CSE_ALifeObject::flVisibleForMap))
			return;

		//update spot position
		Fvector2 position = Position();

		m_position_on_map =	map->ConvertRealToLocal(position, (map->Heading()) ? false : true);

		sp->SetWndPos(m_position_on_map);
		Frect wnd_rect = sp->GetWndRect();

		if( map->IsRectVisible(wnd_rect) ) {

			//update heading if needed
			if( sp->Heading() ){
				Fvector2 dir_global = Direction();
				float h = dir_global.getH();
				float h_ = map->GetHeading()+h;
				sp->SetHeading( h_ );
			}

			Frect clip_rect = map->GetClipperRect();
			sp->SetClipRect( clip_rect );
			map->AttachChild(sp);
		}
		if( GameID() == GAME_SINGLE && GetSpotPointer(sp) ){
			CMapSpot* s = GetSpotBorder(sp);
			if(s){
				s->SetWndPos(sp->GetWndPos());
				map->AttachChild(s);
			}
		}
		if( GetSpotPointer(sp) && map->NeedShowPointer(wnd_rect)){
			UpdateSpotPointer( map, GetSpotPointer(sp) );
		}
	}else
	if(Level().name()==map->MapName() && GetSpotPointer(sp)){
		GameGraph::_GRAPH_ID		dest_graph_id;

		if(!IsUserDefined()){
			CSE_ALifeDynamicObject* obj = NULL;
			VERIFY(ai().get_alife());
			obj = ai().alife().objects().object(m_objectID);
			R_ASSERT(obj);
			dest_graph_id		= obj->m_tGraphID;
		}else{
			CUserDefinedMapLocation	*temp = smart_cast<CUserDefinedMapLocation*>(this);
			VERIFY					(temp);
			dest_graph_id			= temp->m_graph_id;
		}

		map_point_path.clear();

		VERIFY									(Actor());
		GraphEngineSpace::CGameVertexParams		params(Actor()->locations().vertex_types(),flt_max);
		bool res = ai().graph_engine().search(
			ai().game_graph(),
			Actor()->ai_location().game_vertex_id(),
			dest_graph_id,
			&map_point_path,
			params
		);
		if(res){
			xr_vector<u32>::reverse_iterator it = map_point_path.rbegin();
			xr_vector<u32>::reverse_iterator it_e = map_point_path.rend();

			xr_vector<CLevelChanger*>::iterator lit = g_lchangers.begin();
			xr_vector<CLevelChanger*>::iterator lit_e = g_lchangers.end();
			bool bDone						= false;
			for(; (it!=it_e)&&(!bDone) ;++it){
				for(lit=g_lchangers.begin();lit!=lit_e; ++lit){
					
					if((*it)==(*lit)->ai_location().game_vertex_id() ){
						bDone = true;
						break;
					}

				}
			}
			static bool bbb = false;
			if(!bDone&&bbb){
				Msg("Error. Path from actor to selected map spot does not contain level changer :(");
				Msg("Path:");
				xr_vector<u32>::iterator it			= map_point_path.begin();
				xr_vector<u32>::iterator it_e		= map_point_path.end();
				for(; it!=it_e;++it){
//					Msg("%d-%s",(*it),ai().game_graph().vertex(*it));
					Msg("[%d] level[%s]",(*it),*ai().game_graph().header().level(ai().game_graph().vertex(*it)->level_id()).name());
				}
				Msg("Available LevelChangers:");
				xr_vector<CLevelChanger*>::iterator lit,lit_e;
				lit_e							= g_lchangers.end();
				for(lit=g_lchangers.begin();lit!=lit_e; ++lit){
					GameGraph::_GRAPH_ID gid = (*lit)->ai_location().game_vertex_id();
					Msg("[%d]",gid);
					Fvector p = ai().game_graph().vertex(gid)->level_point();
					Msg("lch_name=%s pos=%f %f %f",*ai().game_graph().header().level(ai().game_graph().vertex(gid)->level_id()).name(), p.x, p.y, p.z);
				}

			
			};
			if(bDone){
				Fvector2 position;
				position.set			((*lit)->Position().x, (*lit)->Position().z);
				m_position_on_map		= map->ConvertRealToLocal(position, false);
				UpdateSpotPointer		(map, GetSpotPointer(sp));
			}
		}
	}


}

void CMapLocation::UpdateSpotPointer(CUICustomMap* map, CMapSpotPointer* sp )
{
	if(sp->GetParent()) return ;// already is child
	float		heading;
	Fvector2	pointer_pos;
	if( map->GetPointerTo(m_position_on_map, sp->GetWidth()/2, pointer_pos, heading) )
	{
		sp->SetWndPos(pointer_pos);
		sp->SetHeading(heading);

		Frect clip_rect = map->GetClipperRect();
		sp->SetClipRect( clip_rect );
		map->AttachChild(sp);

		Fvector2 tt = map->ConvertLocalToReal(m_position_on_map);
		Fvector ttt;
		ttt.set		(tt.x, 0.0f, tt.y);
		float dist_to_target = Level().CurrentEntity()->Position().distance_to(ttt);
		map->SetPointerDistance	(dist_to_target);
	}
}

void CMapLocation::UpdateMiniMap(CUICustomMap* map)
{
	CMapSpot* sp = m_minimap_spot;
	if(!sp) return;
	if(SpotEnabled())
		UpdateSpot(map, sp);

}

void CMapLocation::UpdateLevelMap(CUICustomMap* map)
{
	CMapSpot* sp = m_level_spot;
	if(!sp) return;
	if(SpotEnabled())
		UpdateSpot(map, sp);
}

u16	CMapLocation::AddRef() 
{
	++m_refCount; 
	if(	m_flags.test(eTTL) ){
		m_actual_time = Device.dwTimeGlobal+m_ttl*1000;
	}

	return m_refCount;
};

void CMapLocation::save(IWriter &stream)
{
	stream.w_u16	(RefCount());
	stream.w_stringZ(m_hint);
	stream.w_u32	(m_flags.flags);
}

void CMapLocation::load(IReader &stream)
{
	u16 c =			stream.r_u16();
	xr_string		hint;
	stream.r_stringZ(hint);
	SetHint			(hint.c_str());
	SetRefCount		(c);
	m_flags.flags	= stream.r_u32	();
}

void CMapLocation::SetHint	(const shared_str& hint)		
{
	m_hint = hint;
};

LPCSTR CMapLocation::GetHint	()					
{
	CStringTable	stbl;
	return *stbl.translate(m_hint);
};

CMapSpotPointer* CMapLocation::GetSpotPointer(CMapSpot* sp)
{
	R_ASSERT(sp);
	if(!PointerEnabled()) return NULL;
	if(sp==m_level_spot)
		return m_level_spot_pointer;
	else
	if(sp==m_minimap_spot)
		return m_minimap_spot_pointer;

	return NULL;
}

CMapSpot* CMapLocation::GetSpotBorder(CMapSpot* sp)
{
	R_ASSERT(sp);
	if(!PointerEnabled()) return NULL;
	if(sp==m_level_spot){
		if(NULL==m_level_map_spot_border){
			m_level_map_spot_border	= xr_new<CMapSpot>(this);
			m_level_map_spot_border->Load(g_uiSpotXml,"level_map_spot_border");
		}return m_level_map_spot_border;
	}else
		if(sp==m_minimap_spot){
		if(NULL==m_mini_map_spot_border){
			m_mini_map_spot_border	= xr_new<CMapSpot>(this);
			m_mini_map_spot_border->Load(g_uiSpotXml,"mini_map_spot_border");
		}return m_mini_map_spot_border;
	}
	return NULL;
}


CRelationMapLocation::CRelationMapLocation			(const shared_str& type, u16 object_id, u16 pInvOwnerActorID, u16 pInvOwnerEntityID)
:CMapLocation(*type,object_id)
{
	m_curr_spot_name	= type;
	m_pInvOwnerEntityID	= pInvOwnerEntityID;
	m_pInvOwnerActorID	= pInvOwnerActorID;
	m_b_was_visible_last_frame = false;
}

CRelationMapLocation::~CRelationMapLocation			()
{}

bool CRelationMapLocation::Update()
{
	if (false==inherited::Update() ) return false;
	
	bool bAlive = true;

	m_last_relation = ALife::eRelationTypeFriend;

	if(ai().get_alife())		
	{
		CSE_ALifeTraderAbstract*	pEnt = NULL;
		CSE_ALifeTraderAbstract*	pAct = NULL;
		CSE_ALifeDynamicObject*		temp = ai().alife().objects().object(m_pInvOwnerEntityID,true);
		pEnt = smart_cast<CSE_ALifeTraderAbstract*>(temp);
		pAct = smart_cast<CSE_ALifeTraderAbstract*>(ai().alife().objects().object(m_pInvOwnerActorID,true));
		if(!pEnt || !pAct)	return false;
		m_last_relation =  RELATION_REGISTRY().GetRelationType(pEnt, pAct);
		CSE_ALifeCreatureAbstract*		pCreature = smart_cast<CSE_ALifeCreatureAbstract*>(temp);
		if(pCreature) //maybe trader ?
			bAlive = pCreature->g_Alive		();
	}else{
		CInventoryOwner*			pEnt = NULL;
		CInventoryOwner*			pAct = NULL;

		pEnt = smart_cast<CInventoryOwner*>(Level().Objects.net_Find(m_pInvOwnerEntityID));
		pAct = smart_cast<CInventoryOwner*>(Level().Objects.net_Find(m_pInvOwnerActorID));
		if(!pEnt || !pAct)	return false;
		m_last_relation =  RELATION_REGISTRY().GetRelationType(pEnt, pAct);
		CEntityAlive* pEntAlive = smart_cast<CEntityAlive*>(pEnt);
		if(pEntAlive)
			bAlive = !!pEntAlive->g_Alive		();
	}
	shared_str sname;

	if(bAlive==false)
		sname = "deadbody_location";
	else
		sname = RELATION_REGISTRY().GetSpotName(m_last_relation);

	if(m_curr_spot_name != sname){
		LoadSpot(*sname, true);
		m_curr_spot_name = sname;
	}
	return true;
}

bool CRelationMapLocation::IsVisible	()
{
	bool res = true;
	if(m_last_relation==ALife::eRelationTypeEnemy || m_last_relation==ALife::eRelationTypeWorstEnemy){

		CObject* _object_ = Level().Objects.net_Find(m_pInvOwnerEntityID);
		if(_object_){
			CEntityAlive* ea = smart_cast<CEntityAlive*>(_object_);
			if(ea&&!ea->g_Alive()) return true;

			res =  Actor()->memory().visual().visible_now(smart_cast<const CGameObject*>(_object_));
		}
		else
			res = false;
	}
	if(m_b_was_visible_last_frame==false && res == true)
	{
		m_minimap_spot->ResetXformAnimation();
	}

	m_b_was_visible_last_frame = res;	
	return res;
}

void CRelationMapLocation::UpdateMiniMap(CUICustomMap* map)
{
	if(IsVisible())
		inherited::UpdateMiniMap		(map);
}

void CRelationMapLocation::UpdateLevelMap(CUICustomMap* map)
{
	if(IsVisible())
		inherited::UpdateLevelMap		(map);
}

#ifdef DEBUG
void CRelationMapLocation::Dump							()
{
	inherited::Dump();
	Msg("--CRelationMapLocation m_curr_spot_name=[%s]",*m_curr_spot_name);
}
#endif

CUserDefinedMapLocation::CUserDefinedMapLocation		(LPCSTR type, u16 object_id)
:inherited(type, object_id)
{
	m_flags.set			( eSerailizable, TRUE);
	m_flags.set			( eUserDefined, TRUE);
}

CUserDefinedMapLocation::~CUserDefinedMapLocation		()
{
}

void CUserDefinedMapLocation::InitExternal(const shared_str& level_name, const Fvector& pos)
{
	m_level_name		= level_name;
	m_position_global	= pos;
	m_position			= pos;
	m_graph_id			= GameGraph::_GRAPH_ID(-1);

	if(ai().get_alife()){
	const CGameGraph::SLevel& level		= ai().game_graph().header().level(*level_name);
		float min_dist					= flt_max;

		GameGraph::_GRAPH_ID n			= ai().game_graph().header().vertex_count();

		for (GameGraph::_GRAPH_ID i=0; i<n; ++i)
			if (ai().game_graph().vertex(i)->level_id() == level.id()) {
				float				distance = ai().game_graph().vertex(i)->game_point().distance_to_sqr(m_position);
				if (distance < min_dist) {
					min_dist		= distance;
					m_graph_id		= i;
				}
			}
		if (!ai().game_graph().vertex(m_graph_id)) {
			Msg		("! Cannot assign game vertex for CUserDefinedMapLocation [map=%s]", *level_name);
			R_ASSERT(ai().game_graph().vertex(m_graph_id));
		}
	}
}

bool CUserDefinedMapLocation::Update					()
{
	return true;
}

shared_str CUserDefinedMapLocation::LevelName()
{
	return m_level_name;
}

Fvector2 CUserDefinedMapLocation::Position()
{
	return Fvector2().set(m_position.x, m_position.z);
}

Fvector2 CUserDefinedMapLocation::Direction()
{
	return Fvector2().set(0.0f,0.0f);
}

void CUserDefinedMapLocation::save(IWriter &stream)
{
	inherited::save		(stream					);
	save_data			(m_level_name,	stream	);
	save_data			(m_position,	stream	);
	save_data			(m_graph_id,	stream	);
}

void CUserDefinedMapLocation::load(IReader &stream)
{
	inherited::load		(stream);
	load_data			(m_level_name,	stream	);
	load_data			(m_position,	stream	);
	load_data			(m_graph_id,	stream	);

	m_position_global	= m_position;
}

#ifdef DEBUG
void CUserDefinedMapLocation::Dump							()
{
	inherited::Dump();
	Msg("--CUserDefinedMapLocation");
}
#endif
