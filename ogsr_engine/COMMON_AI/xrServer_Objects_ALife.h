////////////////////////////////////////////////////////////////////////////
//	Module 		: xrServer_Objects_ALife.h
//	Created 	: 19.09.2002
//  Modified 	: 04.06.2003
//	Author		: Oles Shyshkovtsov, Alexander Maksimchuk, Victor Reutskiy and Dmitriy Iassenev
//	Description : Server objects for ALife simulator
////////////////////////////////////////////////////////////////////////////

#ifndef xrServer_Objects_ALifeH
#define xrServer_Objects_ALifeH

#include "xrServer_Objects.h"
#include "alife_space.h"
#include "game_graph_space.h"

#pragma warning(push)
#pragma warning(disable:4005)

#ifdef XRGAME_EXPORTS
	class 	CALifeSimulator;
#endif

class CSE_ALifeItemWeapon;
class CSE_ALifeDynamicObject;
class CSE_ALifeObject;
class CALifeSmartTerrainTask;
class CALifeMonsterAbstract;

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeSchedulable,IPureSchedulableObject)
	CSE_ALifeItemWeapon				*m_tpCurrentBestWeapon;
	CSE_ALifeDynamicObject			*m_tpBestDetector;
	u64								m_schedule_counter;

									CSE_ALifeSchedulable	(LPCSTR caSection);
	virtual							~CSE_ALifeSchedulable	();
	// we need this to prevent virtual inheritance :-(
	virtual CSE_Abstract			*base					() = 0;
	virtual const CSE_Abstract		*base					() const = 0;
	virtual CSE_Abstract			*init					();
	virtual CSE_ALifeSchedulable	*cast_schedulable		() {return this;};
	virtual CSE_Abstract			*cast_abstract			() {return 0;};
	// end of the virtual inheritance dependant code
	virtual bool					need_update				(CSE_ALifeDynamicObject *object);
	virtual u32						ef_creature_type		() const;
	virtual u32						ef_anomaly_type			() const;
	virtual u32						ef_weapon_type			() const;
	virtual u32						ef_detector_type		() const;
	virtual bool					natural_weapon			() const {return true;}
	virtual bool					natural_detector		() const {return true;}
#ifdef XRGAME_EXPORTS
	virtual	CSE_ALifeItemWeapon		*tpfGetBestWeapon		(ALife::EHitType		&tHitType,			float		&fHitPower) = 0;
	virtual bool					bfPerformAttack			()											{return(true);};
	virtual	void					vfUpdateWeaponAmmo		()											{};
	virtual	void					vfProcessItems			()											{};
	virtual	void					vfAttachItems			(ALife::ETakeType		tTakeType = ALife::eTakeTypeAll)		{};
	virtual	ALife::EMeetActionType	tfGetActionType			(CSE_ALifeSchedulable	*tpALifeSchedulable,int			iGroupIndex, bool bMutualDetection) = 0;
	virtual bool					bfActive				()															= 0;
	virtual CSE_ALifeDynamicObject	*tpfGetBestDetector		()															= 0;
#endif
};
add_to_type_list(CSE_ALifeSchedulable)
#define script_type_list save_type_list(CSE_ALifeSchedulable)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeGraphPoint,CSE_Abstract)
public:
	shared_str						m_caConnectionLevelName;
	shared_str						m_caConnectionPointName;
	u8								m_tLocations[GameGraph::LOCATION_TYPE_COUNT];

									CSE_ALifeGraphPoint(LPCSTR caSection);
	virtual							~CSE_ALifeGraphPoint();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeGraphPoint)
#define script_type_list save_type_list(CSE_ALifeGraphPoint)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeObject,CSE_Abstract,CRandom)
	enum {
		flUseSwitches		= u32(1) << 0,
		flSwitchOnline		= u32(1) << 1,
		flSwitchOffline		= u32(1) << 2,
		flInteractive		= u32(1) << 3,
		flVisibleForAI		= u32(1) << 4,
		flUsefulForAI		= u32(1) << 5,
		flOfflineNoMove		= u32(1) << 6,
		flUsedAI_Locations	= u32(1) << 7,
		flGroupBehaviour	= u32(1) << 8,
		flCanSave			= u32(1) << 9,
		flVisibleForMap		= u32(1) << 10,
		flUseSmartTerrains	= u32(1) << 11,
		flCheckForSeparator	= u32(1) << 12,
	};

public:
	typedef CSE_Abstract inherited;
	GameGraph::_GRAPH_ID			m_tGraphID;
	float							m_fDistance;
	bool							m_bOnline;
	bool							m_bDirectControl;
	u32								m_tNodeID;
	Flags32							m_flags;
	ALife::_STORY_ID				m_story_id;
	ALife::_SPAWN_STORY_ID			m_spawn_story_id;

#ifdef XRGAME_EXPORTS
	CALifeSimulator					*m_alife_simulator;
#endif

									CSE_ALifeObject		(LPCSTR caSection);
	virtual							~CSE_ALifeObject	();
	virtual bool					used_ai_locations	() const;
	virtual bool					can_save			() const;
	virtual bool					can_switch_online	() const;
	virtual bool					can_switch_offline	() const;
	virtual bool					interactive			() const;
	virtual CSE_ALifeObject			*cast_alife_object	() {return this;}
			bool					move_offline		() const;
			void					can_switch_online	(bool value);
			void					can_switch_offline	(bool value);
			void					interactive			(bool value);
			void					move_offline		(bool value);
			void					used_ai_locations	(bool value);
			bool					visible_for_map		() const;
			void					visible_for_map		(bool value);
	virtual u32						ef_equipment_type	() const;
	virtual u32						ef_main_weapon_type	() const;
	virtual u32						ef_weapon_type		() const;
	virtual u32						ef_detector_type	() const;
#ifdef XRGAME_EXPORTS
	virtual void					spawn_supplies		(LPCSTR);
	virtual void					spawn_supplies		();
			CALifeSimulator			&alife				() const;
	virtual Fvector					draw_level_position	() const;
	virtual	bool					keep_saved_data_anyway	() const;
#endif
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeObject)
#define script_type_list save_type_list(CSE_ALifeObject)

SERVER_ENTITY_DECLARE_BEGIN0(CSE_ALifeGroupAbstract)
	ALife::OBJECT_VECTOR			m_tpMembers;
	bool							m_bCreateSpawnPositions;
	u16								m_wCount;
	ALife::_TIME_ID					m_tNextBirthTime;

									CSE_ALifeGroupAbstract	(LPCSTR caSection);
	virtual							~CSE_ALifeGroupAbstract	();
	virtual	CSE_Abstract			*init					();
	virtual CSE_Abstract			*base					() = 0;
	virtual const CSE_Abstract		*base					() const = 0;
	virtual CSE_ALifeGroupAbstract	*cast_group_abstract	() {return this;};
	virtual CSE_Abstract			*cast_abstract			() {return 0;};
#ifdef XRGAME_EXPORTS
	virtual	bool					synchronize_location	();
	virtual	void					try_switch_online		();
	virtual	void					try_switch_offline		();
	virtual	void					switch_online			();
	virtual	void					switch_offline			();
	virtual	bool					redundant				() const;
#endif
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeGroupAbstract)
#define script_type_list save_type_list(CSE_ALifeGroupAbstract)

template<class __A> class CSE_ALifeGroupTemplate : public __A, public CSE_ALifeGroupAbstract {
	typedef __A					inherited1;
	typedef CSE_ALifeGroupAbstract inherited2;
public:
									CSE_ALifeGroupTemplate(LPCSTR caSection) : __A(pSettings->line_exist(caSection,"monster_section") ? pSettings->r_string(caSection,"monster_section") : caSection), CSE_ALifeGroupAbstract(caSection)
	{
	};
	
	virtual							~CSE_ALifeGroupTemplate()
	{
	};
	
	virtual void STATE_Read			(NET_Packet	&tNetPacket, u16 size)
	{
		inherited1::STATE_Read		(tNetPacket,size);
		inherited2::STATE_Read		(tNetPacket,size);
	};

	virtual void STATE_Write		(NET_Packet	&tNetPacket)
	{
		inherited1::STATE_Write		(tNetPacket);
		inherited2::STATE_Write		(tNetPacket);
	};

	virtual void UPDATE_Read		(NET_Packet	&tNetPacket)
	{
		inherited1::UPDATE_Read		(tNetPacket);
		inherited2::UPDATE_Read		(tNetPacket);
	};

	virtual void UPDATE_Write		(NET_Packet	&tNetPacket)
	{
		inherited1::UPDATE_Write	(tNetPacket);
		inherited2::UPDATE_Write	(tNetPacket);
	};

	virtual CSE_Abstract *init		()
	{
		inherited1::init			();
		inherited2::init			();
		return						(base());
	}

	virtual CSE_Abstract *base		()
	{
		return						(inherited1::base());
	}

	virtual const CSE_Abstract *base() const
	{
		return						(inherited1::base());
	}

	virtual CSE_Abstract			*cast_abstract			()
	{
		return						(this);
	}

	virtual CSE_ALifeGroupAbstract	*cast_group_abstract	()
	{
		return						(this);
	}

#ifdef XRGAME_EXPORTS
	virtual	void					switch_online			()
	{
		inherited2::switch_online	();
	}

	virtual	void					switch_offline			()
	{
		inherited2::switch_offline	();
	}

	virtual	bool					synchronize_location	()
	{
		return						(inherited2::synchronize_location());
	}

	virtual	void					try_switch_online		()
	{
		inherited2::try_switch_online	();
	}

	virtual	void					try_switch_offline		()
	{
		inherited2::try_switch_offline	();
	}

	virtual	bool					redundant				() const
	{
		return							(inherited2::redundant());
	}
#endif
};

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeDynamicObject,CSE_ALifeObject)
	ALife::_TIME_ID					m_tTimeID;
	u64								m_switch_counter;
	
									CSE_ALifeDynamicObject	(LPCSTR caSection);
	virtual							~CSE_ALifeDynamicObject	();
#ifdef XRGAME_EXPORTS
	virtual void					on_spawn				();
	virtual void					on_before_register		();
	virtual void					on_register				();
	virtual void					on_unregister			();
	virtual	bool					synchronize_location	();
	virtual	void					try_switch_online		();
	virtual	void					try_switch_offline		();
	virtual	void					switch_online			();
	virtual	void					switch_offline			();
	virtual	void					add_online				(const bool &update_registries);
	virtual	void					add_offline				(const xr_vector<ALife::_OBJECT_ID> &saved_children, const bool &update_registries);
	virtual	bool					redundant				() const;
			void					attach					(CSE_ALifeInventoryItem *tpALifeInventoryItem,	bool		bALifeRequest,	bool bAddChildren = true);
			void					detach					(CSE_ALifeInventoryItem *tpALifeInventoryItem,	ALife::OBJECT_IT	*I = 0,	bool bALifeRequest = true,	bool bRemoveChildren = true);
#endif
	virtual CSE_ALifeDynamicObject	*cast_alife_dynamic_object	() {return this;}
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeDynamicObject)
#define script_type_list save_type_list(CSE_ALifeDynamicObject)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeDynamicObjectVisual,CSE_ALifeDynamicObject,CSE_Visual)
									CSE_ALifeDynamicObjectVisual(LPCSTR caSection);
	virtual							~CSE_ALifeDynamicObjectVisual();
	virtual CSE_Visual* __stdcall	visual					();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeDynamicObjectVisual)
#define script_type_list save_type_list(CSE_ALifeDynamicObjectVisual)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifePHSkeletonObject,CSE_ALifeDynamicObjectVisual,CSE_PHSkeleton)
									CSE_ALifePHSkeletonObject(LPCSTR caSection);
	virtual							~CSE_ALifePHSkeletonObject();
	virtual bool					can_save				() const;
	virtual bool					used_ai_locations		() const;
	virtual	void					load					(NET_Packet &tNetPacket);
	virtual CSE_Abstract			*cast_abstract			() {return this;}
public:
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifePHSkeletonObject)
#define script_type_list save_type_list(CSE_ALifePHSkeletonObject)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeSpaceRestrictor,CSE_ALifeDynamicObject,CSE_Shape)
	u8								m_space_restrictor_type;

									CSE_ALifeSpaceRestrictor	(LPCSTR caSection);
	virtual							~CSE_ALifeSpaceRestrictor	();
	virtual ISE_Shape*  __stdcall	shape						();
	virtual bool					can_switch_offline			() const;
	virtual bool					used_ai_locations			() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeSpaceRestrictor)
#define script_type_list save_type_list(CSE_ALifeSpaceRestrictor)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeLevelChanger,CSE_ALifeSpaceRestrictor)
	GameGraph::_GRAPH_ID			m_tNextGraphID;
	u32								m_dwNextNodeID;
	Fvector							m_tNextPosition;
	Fvector							m_tAngles;
	shared_str						m_caLevelToChange;
	shared_str						m_caLevelPointToChange;
	u8							m_SilentMode;

									CSE_ALifeLevelChanger		(LPCSTR caSection);
	virtual							~CSE_ALifeLevelChanger		();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeLevelChanger)
#define script_type_list save_type_list(CSE_ALifeLevelChanger)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeSmartZone,CSE_ALifeSpaceRestrictor,CSE_ALifeSchedulable)
									CSE_ALifeSmartZone			(LPCSTR caSection);
	virtual							~CSE_ALifeSmartZone			();
	virtual CSE_Abstract			*base						();
	virtual const CSE_Abstract		*base						() const;
	virtual CSE_Abstract			*init						();
	virtual CSE_Abstract			*cast_abstract				() {return this;};
	virtual CSE_ALifeSchedulable	*cast_schedulable			() {return this;};
	virtual void					update						();
	virtual float					detect_probability			();
	virtual void					smart_touch					(CSE_ALifeMonsterAbstract *monster);
	virtual bool					used_ai_locations			() const {return true;};
	virtual CSE_ALifeSmartZone		*cast_smart_zone			() {return this;};
#ifdef XRGAME_EXPORTS
	virtual bool					bfActive					();
	virtual	CSE_ALifeItemWeapon		*tpfGetBestWeapon			(ALife::EHitType		&tHitType,			float		&fHitPower);
	virtual CSE_ALifeDynamicObject	*tpfGetBestDetector			();
	virtual	ALife::EMeetActionType	tfGetActionType				(CSE_ALifeSchedulable	*tpALifeSchedulable,int			iGroupIndex, bool bMutualDetection);
	// additional functionality
	virtual bool					enabled						(CSE_ALifeMonsterAbstract *object) const {return false;};
	virtual float					suitable					(CSE_ALifeMonsterAbstract *object) const {return 0.f;};
	virtual void					register_npc				(CSE_ALifeMonsterAbstract *object) {};
	virtual void					unregister_npc				(CSE_ALifeMonsterAbstract *object) {};
	virtual	CALifeSmartTerrainTask	*task						(CSE_ALifeMonsterAbstract *object) {return 0;};
#endif
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeSmartZone)
#define script_type_list save_type_list(CSE_ALifeSmartZone)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeObjectPhysic,CSE_ALifeDynamicObjectVisual,CSE_PHSkeleton)
	u32 							type;
	f32 							mass;
    shared_str 						fixed_bones;
									CSE_ALifeObjectPhysic	(LPCSTR caSection);
    virtual 						~CSE_ALifeObjectPhysic	();
	virtual bool					used_ai_locations		() const;
	virtual bool					can_save				() const;
	virtual	void					load					(NET_Packet &tNetPacket);
	virtual CSE_Abstract			*cast_abstract			() {return this;}
//	virtual	void					load					(IReader& r){inherited::load(r);}
//	using inherited::load(IReader&);

SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeObjectPhysic)
#define script_type_list save_type_list(CSE_ALifeObjectPhysic)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeObjectHangingLamp,CSE_ALifeDynamicObjectVisual,CSE_PHSkeleton)

    enum{
        flPhysic					= (1<<0),
		flCastShadow				= (1<<1),
		flR1						= (1<<2),
		flR2						= (1<<3),
		flTypeSpot					= (1<<4),
        flPointAmbient				= (1<<5),
		flVolumetricLight			= (1<<6),
		flUseFlare					= (1<<7),
    };

    Flags16							flags;
// light color    
    u32								color;
    float							brightness;
    shared_str						color_animator;
// light texture    
	shared_str						light_texture;
// range
    float							range;
	float							m_virtual_size;
// bones&motions
	shared_str						light_ambient_bone;
	shared_str						light_main_bone;
    shared_str						fixed_bones;
// spot
	float							spot_cone_angle;
// ambient    
    float							m_ambient_radius;
    float							m_ambient_power;
	shared_str						m_ambient_texture;
// glow    
	shared_str						glow_texture;
	float							glow_radius;
// game
    float							m_health;

//lsf
	float							m_speed = 0.f;
	float							m_amount = 0.f;
	float							m_smap_jitter = 0.f;
	
                                    CSE_ALifeObjectHangingLamp	(LPCSTR caSection);
    virtual							~CSE_ALifeObjectHangingLamp	();
	virtual	void					load						(NET_Packet &tNetPacket);
	virtual bool					used_ai_locations			() const;
	virtual bool					match_configuration			() const;
	virtual bool		__stdcall	validate					();
	virtual void 		__stdcall	on_render					(CDUInterface* du, ISE_AbstractLEOwner* owner, bool bSelected, const Fmatrix& parent,int priority, bool strictB2F);
	virtual CSE_Abstract			*cast_abstract			() {return this;}
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeObjectHangingLamp)
#define script_type_list save_type_list(CSE_ALifeObjectHangingLamp)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectProjector,CSE_ALifeDynamicObjectVisual)
									CSE_ALifeObjectProjector	(LPCSTR caSection);
	virtual							~CSE_ALifeObjectProjector	();
	virtual bool					used_ai_locations	() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeObjectProjector)
#define script_type_list save_type_list(CSE_ALifeObjectProjector)

SERVER_ENTITY_DECLARE_BEGIN3(CSE_ALifeHelicopter,CSE_ALifeDynamicObjectVisual,CSE_Motion,CSE_PHSkeleton)
	shared_str							engine_sound;
									CSE_ALifeHelicopter			(LPCSTR caSection);
	virtual							~CSE_ALifeHelicopter		();
	virtual	void					load						(NET_Packet &tNetPacket);
	virtual bool					can_save					() const;
	virtual bool					used_ai_locations			() const;
	virtual CSE_Motion*	__stdcall	motion						();
	virtual CSE_Abstract			*cast_abstract			() {return this;}

SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeHelicopter)
#define script_type_list save_type_list(CSE_ALifeHelicopter)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeCar,CSE_ALifeDynamicObjectVisual,CSE_PHSkeleton)
	struct SDoorState				
	{
		void read	(NET_Packet& P);
		void write   (NET_Packet& P);
		u8 open_state;
		float health;
	};
	struct SWheelState				
	{
		void read	(NET_Packet& P);
		void write   (NET_Packet& P);
		float health;
	};
	xr_vector<SDoorState>			door_states;
	xr_vector<SWheelState>			wheel_states;
	float							health;
									CSE_ALifeCar		(LPCSTR caSection);
	virtual							~CSE_ALifeCar		();
	virtual bool					used_ai_locations	() const;
	virtual	void					load				(NET_Packet &tNetPacket);
	virtual bool					can_save			() const;
	virtual CSE_Abstract			*cast_abstract		() {return this;}
protected:
	virtual void					data_load				(NET_Packet &tNetPacket);
	virtual void					data_save				(NET_Packet &tNetPacket);
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeCar)
#define script_type_list save_type_list(CSE_ALifeCar)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeObjectBreakable,CSE_ALifeDynamicObjectVisual)
    float							m_health;
									CSE_ALifeObjectBreakable	(LPCSTR caSection);
	virtual							~CSE_ALifeObjectBreakable	();
	virtual bool					used_ai_locations	() const;
	virtual bool					can_switch_offline	() const;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeObjectBreakable)
#define script_type_list save_type_list(CSE_ALifeObjectBreakable)

SERVER_ENTITY_DECLARE_BEGIN2(CSE_ALifeObjectClimable,CSE_Shape,CSE_ALifeDynamicObject)
CSE_ALifeObjectClimable	(LPCSTR caSection);
virtual							~CSE_ALifeObjectClimable	();
virtual bool					used_ai_locations	() const;
virtual bool					can_switch_offline	() const;
virtual ISE_Shape*  __stdcall	shape				();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeObjectClimable)
#define script_type_list save_type_list(CSE_ALifeObjectClimable)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeMountedWeapon,CSE_ALifeDynamicObjectVisual)
									CSE_ALifeMountedWeapon	(LPCSTR caSection);
	virtual							~CSE_ALifeMountedWeapon	();
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeMountedWeapon)
#define script_type_list save_type_list(CSE_ALifeMountedWeapon)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeStationaryMgun,CSE_ALifeDynamicObjectVisual)
	bool							m_bWorking{};
	Fvector							m_destEnemyDir;

									CSE_ALifeStationaryMgun	(LPCSTR caSection);
	virtual							~CSE_ALifeStationaryMgun	();
	
SERVER_ENTITY_DECLARE_END
//add_to_type_list(CSE_ALifeStationaryMgun)
//#define script_type_list save_type_list(CSE_ALifeStationaryMgun)

SERVER_ENTITY_DECLARE_BEGIN(CSE_ALifeTeamBaseZone,CSE_ALifeSpaceRestrictor)
									CSE_ALifeTeamBaseZone	(LPCSTR caSection);
	virtual							~CSE_ALifeTeamBaseZone	();

	u8								m_team;
SERVER_ENTITY_DECLARE_END
add_to_type_list(CSE_ALifeTeamBaseZone)
#define script_type_list save_type_list(CSE_ALifeTeamBaseZone)


extern void add_online_impl		(CSE_ALifeDynamicObject *object, const bool &update_registries);
extern void add_offline_impl	(CSE_ALifeDynamicObject *object, const xr_vector<ALife::_OBJECT_ID> &saved_children, const bool &update_registries);

class CSE_InventoryBoxAbstract {
protected:
	bool			m_opened;
public:
	CSE_InventoryBoxAbstract() = default;
};

class CSE_InventoryBox :public CSE_ALifeDynamicObjectVisual, public CSE_InventoryBoxAbstract
{
public:
						CSE_InventoryBox	(LPCSTR caSection):CSE_ALifeDynamicObjectVisual(caSection){};
	virtual				~CSE_InventoryBox	(){};
#ifdef XRGAME_EXPORTS
	virtual void		add_offline			(const xr_vector<ALife::_OBJECT_ID> &saved_children, const bool &update_registries)
	{
		add_offline_impl (smart_cast<CSE_ALifeDynamicObjectVisual*>(this), saved_children, update_registries);
		CSE_ALifeDynamicObjectVisual::add_offline(saved_children, update_registries);
	}
	virtual void		add_online			(const bool &update_registries)
	{
		add_online_impl (smart_cast<CSE_ALifeDynamicObjectVisual*>(this), update_registries);
		CSE_ALifeDynamicObjectVisual::add_online(update_registries);
	};
#endif
};

#pragma warning(pop)

#endif
