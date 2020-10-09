#include "stdafx.h"
#include "map_manager.h"
#include "alife_registry_wrappers.h"
#include "inventoryowner.h"
#include "level.h"
#include "actor.h"
#include "relation_registry.h"
#include "GameObject.h"
#include "map_location.h"
#include "GameTaskManager.h"
#include "xrServer.h"
#include "game_object_space.h"
#include "script_callback_ex.h"
#include "alife_simulator_header.h"

struct FindLocationBySpotID{
	shared_str	spot_id;
	u16			object_id;
	FindLocationBySpotID(const shared_str& s, u16 id):spot_id(s),object_id(id){}
	bool operator () (const SLocationKey& key){
		return (spot_id==key.spot_type)&&(object_id==key.object_id);
	}
};
struct FindLocationByID{
	u16			object_id;
	FindLocationByID(u16 id):object_id(id){}
	bool operator () (const SLocationKey& key){
		return (object_id==key.object_id);
	}
};

struct FindLocation{
	CMapLocation*			ml;
	FindLocation(CMapLocation* m):ml(m){}
	bool operator () (const SLocationKey& key){
		return (ml==key.location);
	}
};

void SLocationKey::save(IWriter &stream)
{
	stream.w		(&object_id,sizeof(object_id));

	stream.w_stringZ(spot_type);
	stream.w_u8(location->IsUserDefined() ? 1 : 0);
	location->save	(stream);
}
	
void SLocationKey::load(IReader &stream)
{
	stream.r		(&object_id,sizeof(object_id));

	stream.r_stringZ(spot_type);

	u8 bUserDefined = stream.r_u8();
	if (bUserDefined)
	{
		Level().Server->PerformIDgen(object_id);
		location = xr_new<CMapLocation>(*spot_type, object_id, true);
	}
	else
	{
		location = xr_new<CMapLocation>(*spot_type, object_id);
	}
	location->load(stream);
}

void SLocationKey::destroy()
{
	if (location && location->IsUserDefined())
		Level().Server->FreeID(object_id, 0);

	delete_data(location);
}

void CMapLocationRegistry::save(IWriter &stream)
{
	stream.w_u32			((u32)objects().size());
	iterator				I = m_objects.begin();
	iterator				E = m_objects.end();
	for ( ; I != E; ++I) {
		u32					size = 0;
		Locations::iterator	i = (*I).second.begin();
		Locations::iterator	e = (*I).second.end();
		for ( ; i != e; ++i) {
			VERIFY			((*i).location);
			if ((*i).location->Serializable())
				++size;
		}
		stream.w			(&(*I).first,sizeof((*I).first));
		stream.w_u32		(size);
		i					= (*I).second.begin();
		for ( ; i != e; ++i)
			if ((*i).location->Serializable())
				(*i).save	(stream);
	}
}


CMapManager::CMapManager()
{
	m_locations = xr_new<CMapLocationWrapper>();
	m_locations->registry().init(1);
}

CMapManager::~CMapManager()
{
	delete_data		(m_locations);
}

void CMapManager::initialize(u16 id)
{
	VERIFY(0);
	m_locations->registry().init(id);// actor's id
}
CMapLocation* CMapManager::AddMapLocation(const shared_str& spot_type, u16 id)
{
	FindLocationBySpotID key(spot_type, id);
	Locations_it it = std::find_if(Locations().begin(),Locations().end(),key);
	if( it == Locations().end() ){
		CMapLocation* l = xr_new<CMapLocation>(*key.spot_id, key.object_id);
		Locations().emplace_back(key.spot_id, key.object_id).location = l;
		if (g_actor)
			Actor()->callback(GameObject::eMapLocationAdded)(*spot_type, id);
		return l;
	}else
		(*it).location->AddRef();

	return (*it).location;
}

CMapLocation* CMapManager::AddRelationLocation(CInventoryOwner* pInvOwner)
{
	if(!Level().CurrentViewEntity())return NULL;

	ALife::ERelationType relation = ALife::eRelationTypeFriend;
	CInventoryOwner* pActor = smart_cast<CInventoryOwner*>(Level().CurrentViewEntity());
	relation =  RELATION_REGISTRY().GetRelationType(pInvOwner, pActor);
	shared_str sname = RELATION_REGISTRY().GetSpotName(relation);

	CEntityAlive* pEntAlive = smart_cast<CEntityAlive*>(pInvOwner);
	if( !pEntAlive->g_Alive() ) sname = "deadbody_location";


	FindLocationBySpotID key(sname, pInvOwner->object_id());
	Locations_it it = std::find_if(Locations().begin(),Locations().end(),key);
	if( it == Locations().end() ){
		CMapLocation* l = xr_new<CRelationMapLocation>(*key.spot_id, key.object_id, pActor->object_id(), pInvOwner->object_id());
		Locations().emplace_back(key.spot_id, key.object_id).location = l;
		return l;
	}else
		(*it).location->AddRef();

	return (*it).location;
}


CMapLocation* CMapManager::AddUserLocation(const shared_str& spot_type, const shared_str& level_name, Fvector position)
{
	u16 _id = Level().Server->PerformIDgen(0xffff);
	CMapLocation* l = xr_new<CMapLocation>(spot_type.c_str(), _id, true);
	l->InitUserSpot(level_name, position);
#pragma todo("KRodin: на данный момент юзерские метки используют дефицитные ID объектов. В конструктор класса CMapLocation можно смело отправлять u16(-1), а вот хранилище Locations() я бы трогать побоялся, т.к. не уверен, что из него не достают ID и что-то там с ним делают. В идеале конечно надо это всё перепилить.")
	Locations().emplace_back(SLocationKey(spot_type, _id));
	Locations().back().location = l;
	return l;
}


void CMapManager::RemoveMapLocation(const shared_str& spot_type, u16 id)
{
	FindLocationBySpotID key(spot_type, id);
	Locations_it it = std::find_if(Locations().begin(),Locations().end(),key);
	if( it!=Locations().end() ){

		if( 1==(*it).location->RefCount() ){
			delete_data				(*it);
			Locations().erase		(it);
		}else
			(*it).location->Release();
	}
}

void CMapManager::RemoveMapLocationByObjectID(u16 id) //call on destroy object
{
	FindLocationByID key(id);
	Locations_it it = std::find_if(Locations().begin(),Locations().end(),key);
	while( it!= Locations().end() ){
		
			delete_data				(*it);
			Locations().erase		(it);

			it = std::find_if(Locations().begin(),Locations().end(),key);
	}
}

void CMapManager::RemoveMapLocation			(CMapLocation* ml)
{
	FindLocation key(ml);
	Locations_it it = std::find_if(Locations().begin(),Locations().end(),key);
	if( it!=Locations().end() ){
		delete_data				(*it);
		Locations().erase		(it);
	}
}


u16 CMapManager::HasMapLocation(const shared_str& spot_type, u16 id)
{
	CMapLocation* l = GetMapLocation(spot_type, id);
	
	return (l)?l->RefCount():0;
}

CMapLocation* CMapManager::GetMapLocation(const shared_str& spot_type, u16 id)
{
	FindLocationBySpotID key(spot_type, id);
	Locations_it it = std::find_if(Locations().begin(),Locations().end(),key);
	if( it!=Locations().end() )
		return (*it).location;
	
	return 0;
}

void CMapManager::Update()
{
	Locations_it it = Locations().begin();
	for(; it!=Locations().end();++it){
		(*it).actual = (*it).location->Update();
	}
	std::sort( Locations().begin(),Locations().end() );

	while( (!Locations().empty())&&(!Locations().back().actual) ){
		delete_data(Locations().back().location);
		Locations().pop_back();
	}
}

void CMapManager::DisableAllPointers			()
{
	Locations_it it = Locations().begin();
	for(; it!=Locations().end();++it)
		(*it).location->DisablePointer	();
}


Locations&	CMapManager::Locations	() 
{
	return m_locations->registry().objects();
}

#ifdef DEBUG
void CMapManager::Dump						()
{
	Msg("begin of map_locations dump");
	Locations_it it = Locations().begin();
	for(; it!=Locations().end();++it){
		Msg("spot_type=[%s] object_id=[%d]",*((*it).spot_type), (*it).object_id);
		(*it).location->Dump();
	}

	Msg("end of map_locations dump");
}
#endif
