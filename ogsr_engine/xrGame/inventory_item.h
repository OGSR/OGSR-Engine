////////////////////////////////////////////////////////////////////////////
//	Module 		: inventory_item.h
//	Created 	: 24.03.2003
//  Modified 	: 29.01.2004
//	Author		: Victor Reutsky, Yuri Dobronravin
//	Description : Inventory item
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "inventory_space.h"
#include "hit_immunity.h"
#include "attachable_item.h"

enum EHandDependence{
	hdNone	= 0,
	hd1Hand	= 1,
	hd2Hand	= 2
};

class CSE_Abstract;
class CGameObject;
class CFoodItem;
class CMissile;
class CHudItem;
class CWeaponAmmo;
class CWeapon;
class CPhysicsShellHolder;
class NET_Packet;
class CEatableItem;
struct SPHNetState;
struct net_update_IItem;
struct net_updateData;
class CInventoryOwner;

struct SHit;

class CInventoryItem : 
	public CAttachableItem,
	public CHitImmunity
#ifdef DEBUG
	, public pureRender
#endif
{
private:
	typedef CAttachableItem inherited;
protected:
	enum EIIFlags{				FdropManual			=(1<<0),
								FCanTake			=(1<<1),
								FCanTrade			=(1<<2),
								Fbelt				=(1<<3),
								Fruck				=(1<<4),
								FRuckDefault		=(1<<5),
								FUsingCondition		=(1<<6),
								FAllowSprint		=(1<<7),
								Fuseful_for_NPC		=(1<<8),
								FInInterpolation	=(1<<9),
								FInInterpolate		=(1<<10),
								FIsQuestItem		=(1<<11),
								FCanTakeActor		=(1<<12),
	};

	Flags16						m_flags;
public:
								CInventoryItem		();
	virtual						~CInventoryItem		();

public:
	virtual void				Load				(LPCSTR section);

	virtual LPCSTR				Name				();
	virtual LPCSTR				NameShort			();
//.	virtual LPCSTR				NameComplex			();
	shared_str					ItemDescription		() { return m_Description; }
	virtual void				GetBriefInfo		(xr_string& str_name, xr_string& icon_sect_name, xr_string& str_count) {};
	
	virtual void				OnEvent				(NET_Packet& P, u16 type);
	
	virtual bool				Useful				() const;									// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool				Attach				(PIItem pIItem, bool b_send_event) {return false;}
	virtual bool				Detach				(PIItem pIItem) {return false;}
	//при детаче спаунится новая вещь при заданно названии секции
	virtual bool				Detach				(const char* item_section_name, bool b_spawn_item);
	virtual bool				CanAttach			(PIItem pIItem) {return false;}
	virtual bool				CanDetach			(LPCSTR item_section_name) {return false;}

	virtual EHandDependence		HandDependence		()	const	{return hd1Hand;};
	virtual bool				IsSingleHanded		()	const	{return true;};	
	virtual bool				Activate			();									// !!! Переопределить. (см. в Inventory.cpp)
	virtual void				Deactivate			();								// !!! Переопределить. (см. в Inventory.cpp)
	virtual bool				Action				(s32 cmd, u32 flags) {return false;}	// true если известная команда, иначе false

	virtual bool				IsHidden			()	const	{return true;}
	virtual bool				IsHiding			()	const	{return false;}
	virtual bool 				IsShowing			()  const	{return false;}

	virtual void				OnH_B_Chield		();
	virtual void				OnH_A_Chield		();
    virtual void				OnH_B_Independent	(bool just_before_destroy);
	virtual void				OnH_A_Independent	();

	virtual void				save				(NET_Packet &output_packet);
	virtual void				load				(IReader &input_packet);
	virtual BOOL				net_SaveRelevant	()								{return TRUE;}


	virtual void				UpdateCL			();

	virtual	void				Hit					(SHit* pHDS);

			BOOL				GetDropManual		() const	{ return m_flags.test(FdropManual);}
			void				SetDropManual		(BOOL val)	{ m_flags.set(FdropManual, val);}

			BOOL				IsInvalid			() const;

			BOOL				IsQuestItem			()	const	{return m_flags.test(FIsQuestItem);}			
			u32					Cost				() const	{ return m_cost; }
	virtual float				Weight				() 			{ return m_weight;}		

public:
	CInventory*					m_pCurrentInventory;

	shared_str					m_name;
	shared_str					m_nameShort;
	shared_str					m_nameComplex;

	EItemPlace					m_eItemPlace;


	virtual void				OnMoveToSlot		() {};
	virtual void				OnMoveToBelt		() {};
	virtual void				OnMoveToRuck		() {};
					
			int					GetGridWidth		() const ;
			int					GetGridHeight		() const ;
			const shared_str&	GetIconName			() const		{return m_icon_name;};
			int					GetXPos				() const ;
			int					GetYPos				() const ;
	//---------------------------------------------------------------------
			float				GetKillMsgXPos		() const ;
			float				GetKillMsgYPos		() const ;
			float				GetKillMsgWidth		() const ;
			float				GetKillMsgHeight	() const ;
	//---------------------------------------------------------------------
			float				GetCondition		() const					{return m_fCondition;}
	virtual	float				GetConditionToShow	() const					{return GetCondition();}
			void				ChangeCondition		(float fDeltaCondition);

	virtual u8					GetSlot				()  const					{return m_slot;}
			void				SetSlot				(u8 _slot_id)				{ m_slot = _slot_id; }

			bool				Belt				()							{return !!m_flags.test(Fbelt);}
			void				Belt				(bool on_belt)				{m_flags.set(Fbelt,on_belt);}
			bool				Ruck				()							{return !!m_flags.test(Fruck);}
			void				Ruck				(bool on_ruck)				{m_flags.set(Fruck,on_ruck);}
			bool				RuckDefault			()							{return !!m_flags.test(FRuckDefault);}
			
	virtual bool				CanTake				() const					{return !!m_flags.test(FCanTake);}
			bool				CanTakeActor		() const					{ return !!m_flags.test(FCanTakeActor); }
			bool				CanTrade			() const;
	virtual bool 				IsNecessaryItem	    (CInventoryItem* item);
	virtual bool				IsNecessaryItem	    (const shared_str& item_sect){return false;};
protected:
	
	u8							m_slot;
	u32							m_cost;
	float						m_weight;
	float						m_fCondition;
	shared_str					m_Description;

	ALife::_TIME_ID				m_dwItemRemoveTime;
	ALife::_TIME_ID				m_dwItemIndependencyTime;

	float						m_fControlInertionFactor;
	shared_str					m_icon_name;

	////////// network //////////////////////////////////////////////////
public:
	virtual void				make_Interpolation	();
	virtual void				PH_B_CrPr			(); // actions & operations before physic correction-prediction steps
	virtual void				PH_I_CrPr			(); // actions & operations after correction before prediction steps
#ifdef DEBUG
	virtual void				PH_Ch_CrPr			(); // 
#endif
	virtual void				PH_A_CrPr			(); // actions & operations after phisic correction-prediction steps

	virtual void				net_Import			(NET_Packet& P);					// import from server
	virtual void				net_Export			(NET_Packet& P);					// export to server

public:
	virtual void				activate_physic_shell		();
	virtual u16					bone_count_to_synchronize	() const;

	virtual bool				NeedToDestroyObject			() const;
	virtual ALife::_TIME_ID		TimePassedAfterIndependant	() const;

	virtual	bool				IsSprintAllowed				() const		{return !!m_flags.test(FAllowSprint);} ;

	virtual	float				GetControlInertionFactor(	) const			{return m_fControlInertionFactor;};

protected:
	virtual void				UpdateXForm	();
			
protected:
	net_updateData*				m_net_updateData;
	net_updateData*				NetSync						();
	void						CalculateInterpolationParams();

public:
	virtual BOOL				net_Spawn				(CSE_Abstract* DC);
	virtual void				net_Destroy				();
	virtual void				reload					(LPCSTR section);
	virtual void				reinit					();
	virtual bool				can_kill				() const;
	virtual CInventoryItem*		can_kill				(CInventory *inventory) const;
	virtual const CInventoryItem*can_kill				(const xr_vector<const CGameObject*> &items) const;
	virtual CInventoryItem*		can_make_killing		(const CInventory *inventory) const;
	virtual bool				ready_to_kill			() const;
	IC		bool				useful_for_NPC			() const;
#ifdef DEBUG
	virtual void				OnRender					();
#endif

public:
	virtual DLL_Pure*			_construct					();
	IC	CPhysicsShellHolder&	object						() const{ VERIFY		(m_object); return		(*m_object);}
	virtual void				on_activate_physic_shell	() { R_ASSERT(0); } //sea

protected:
	float						m_holder_range_modifier;
	float						m_holder_fov_modifier;

public:
	virtual	void				modify_holder_params		(float &range, float &fov) const;

protected:
	IC	CInventoryOwner&		inventory_owner				() const;

private:
	CPhysicsShellHolder*		m_object;

public:
	virtual CInventoryItem		*cast_inventory_item		()	{return this;}
	virtual CAttachableItem		*cast_attachable_item		()	{return this;}
	virtual CPhysicsShellHolder	*cast_physics_shell_holder	()	{return 0;}
	virtual CEatableItem		*cast_eatable_item			()	{return 0;}
	virtual CWeapon				*cast_weapon				()	{return 0;}
	virtual CFoodItem			*cast_food_item				()	{return 0;}
	virtual CMissile			*cast_missile				()	{return 0;}
	virtual CHudItem			*cast_hud_item				()	{return 0;}
	virtual CWeaponAmmo			*cast_weapon_ammo			()	{return 0;}
	virtual CGameObject			*cast_game_object			()  {return 0;};
};

#include "inventory_item_inline.h"