
#pragma once
#include "object_interfaces.h"
#include "alife_space.h"
#include "game_graph_space.h"

class CMapSpot;
class CMiniMapSpot;
class CMapSpotPointer;
class CUICustomMap;
class CInventoryOwner;

class CMapLocation :public IPureDestroyableObject
{
public:
enum ELocationFlags
{
	eSerailizable		= (1<<0),
	eHideInOffline		= (1<<1),
	eTTL				= (1<<2),
	ePosToActor			= (1<<3),
	ePointerEnabled		= (1<<4),
	eSpotEnabled		= (1<<5),
	eUserDefined		= (1<<6),
};

protected:
	flags32					m_flags;
	shared_str				m_hint;
	CMapSpot*				m_level_spot;
	CMapSpotPointer*		m_level_spot_pointer;
	CMiniMapSpot*			m_minimap_spot;
	CMapSpotPointer*		m_minimap_spot_pointer;

	CMapSpot*				m_level_map_spot_border;
	CMapSpot*				m_mini_map_spot_border;

	u16						m_objectID;
	u16						m_refCount;
	int						m_ttl;
	u32						m_actual_time;
	Fvector					m_position_global; //last global position, actual time only current frame 
	Fvector2 				m_position_on_map; //last position on parent map, actual time only current frame

	struct SCachedValues{
		u32					m_updatedFrame;
		Fvector2			m_Position;
		Fvector2			m_Direction;
		shared_str			m_LevelName;
		bool				m_Actuality;
	};
	SCachedValues			m_cached;
private:
							CMapLocation					(const CMapLocation&){}; //disable copy ctor

protected :
	void					LoadSpot						(LPCSTR type, bool bReload); 
	void					UpdateSpot						(CUICustomMap* map, CMapSpot* sp );
	void					UpdateSpotPointer				(CUICustomMap* map, CMapSpotPointer* sp );
	CMapSpotPointer*		GetSpotPointer					(CMapSpot* sp);
	CMapSpot*				GetSpotBorder					(CMapSpot* sp);
public:
							CMapLocation					(LPCSTR type, u16 object_id);
	virtual					~CMapLocation					();
	virtual void			destroy							();
			LPCSTR			GetHint							();
	void					SetHint							(const shared_str& hint);
	bool					PointerEnabled					()					{return SpotEnabled() && !!m_flags.test(ePointerEnabled);};
	void					EnablePointer					()					{m_flags.set(ePointerEnabled,TRUE);};
	void					DisablePointer					()					{m_flags.set(ePointerEnabled,FALSE);};

	bool					SpotEnabled						()					{return !!m_flags.test(eSpotEnabled);};
	void					EnableSpot						()					{m_flags.set(eSpotEnabled,TRUE);};
	void					DisableSpot						()					{m_flags.set(eSpotEnabled,FALSE);};
	bool					IsUserDefined					() const			{return !!m_flags.test(eUserDefined);}
	virtual void			UpdateMiniMap					(CUICustomMap* map);
	virtual void			UpdateLevelMap					(CUICustomMap* map);

	virtual Fvector2		Position						();
	virtual Fvector2		Direction						();
	virtual shared_str		LevelName						();
	u16						RefCount						() {return m_refCount;}
	void					SetRefCount						(u16 c)		{m_refCount=c;}
	u16						AddRef							();// {++m_refCount; return m_refCount;}
	u16						Release							() {--m_refCount; return m_refCount;}
	u16						ObjectID						() {return m_objectID;}
	virtual		bool		Update							(); //returns actual
	Fvector					GetLastPosition					() {return m_position_global;};
	bool					Serializable					() const {return !!m_flags.test(eSerailizable);}
	void					SetSerializable					(bool b) {m_flags.set(eSerailizable,b);}

	virtual void			save							(IWriter &stream);
	virtual void			load							(IReader &stream);
	virtual bool			CanBeSelected					()						{return true;}
	virtual bool			CanBeUserRemoved				()						{return false;}

#ifdef DEBUG
	virtual void			Dump							(){};
#endif

};

class CRelationMapLocation :public CMapLocation
{
	typedef CMapLocation inherited;
	shared_str				m_curr_spot_name;
	u16						m_pInvOwnerEntityID;
	u16						m_pInvOwnerActorID;
	ALife::ERelationType	m_last_relation;
	bool					m_b_was_visible_last_frame;
protected:
	bool					IsVisible							();
public:
							CRelationMapLocation			(const shared_str& type, u16 object_id, u16 pInvOwnerActorID, u16 pInvOwnerEntityID);
	virtual					~CRelationMapLocation			();
	virtual bool			Update							(); //returns actual

	virtual void			UpdateMiniMap					(CUICustomMap* map);
	virtual void			UpdateLevelMap					(CUICustomMap* map);

#ifdef DEBUG
	virtual void			Dump							();
#endif
};

class CUserDefinedMapLocation :public CMapLocation
{
	typedef CMapLocation inherited;
	shared_str				m_level_name;
	Fvector					m_position;
public:
	GameGraph::_GRAPH_ID	m_graph_id;
							CUserDefinedMapLocation			(LPCSTR type, u16 object_id);
	virtual					~CUserDefinedMapLocation		();
	virtual bool			Update							(); //returns actual
	virtual Fvector2		Position						();
	virtual Fvector2		Direction						();
	virtual shared_str		LevelName						();

			void			InitExternal					(const shared_str& level_name, const Fvector& pos);
	virtual void			save							(IWriter &stream);
	virtual void			load							(IReader &stream);

	virtual bool			CanBeSelected					()						{return true;}
	virtual bool			CanBeUserRemoved				()						{return true;}
#ifdef DEBUG
	virtual void			Dump							();
#endif
};
