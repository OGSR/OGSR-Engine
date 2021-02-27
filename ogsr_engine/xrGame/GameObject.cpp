#include "stdafx.h"
#include "GameObject.h"
#include "PhysicsShell.h"
#include "ai_space.h"
#include "CustomMonster.h" 
#include "physicobject.h"
#include "HangingLamp.h"
#include "PhysicsShell.h"
#include "game_sv_single.h"
#include "level_graph.h"
#include "ph_shell_interface.h"
#include "script_game_object.h"
#include "xrserver_objects_alife.h"
#include "xrServer_Objects_ALife_Items.h"
#include "game_cl_base.h"
#include "object_factory.h"
#include "../Include/xrRender/Kinematics.h"
#include "game_graph.h"
#include "ai_debug.h"
#include "..\xr_3da\igame_level.h"
#include "level.h"
#include "../xr_3da/NET_Server_Trash/net_utils.h"
#include "script_callback_ex.h"
#include "MathUtils.h"
#include "game_cl_base_weapon_usage_statistic.h"
#include "game_level_cross_table.h"
#include "animation_movement_controller.h"
#include "game_object_space.h"
#include "alife_simulator.h"
#include "alife_object_registry.h"
#include "Car.h"

#include "ai_object_location.h"

#ifdef DEBUG
#	include "debug_renderer.h"
#	include "PHDebug.h"
#endif

CGameObject::CGameObject		()
{
	init						();
	//-----------------------------------------
	m_bCrPr_Activated			= false;
	m_dwCrPr_ActivationStep		= 0;
	m_spawn_time				= 0;
	m_ai_location				= xr_new<CAI_ObjectLocation>();
	m_server_flags.one			();

	auto dummy_callback = std::make_unique<GOCallbackInfo>();
	this->m_callbacks[GameObject::eDummy] = std::move(dummy_callback);

	m_anim_mov_ctrl				= 0;
}

CGameObject::~CGameObject		()
{
	VERIFY						( !animation_movement( ) );
	VERIFY						(!m_ini_file);
	VERIFY						(!m_lua_game_object);
	VERIFY						(!m_spawned);
	xr_delete					(m_ai_location);
	m_callbacks.clear();

        for ( auto& ft : feel_touch_addons ) {
          xr_delete( ft );
        }
        feel_touch_addons.clear();
}

CSE_ALifeDynamicObject* CGameObject::alife_object() const
{
	const CALifeSimulator *sim = ai().get_alife();
	if (sim)
		return sim->objects().object(ID(), true);
	return NULL;
}

void CGameObject::init			()
{
	m_lua_game_object			= 0;
	m_script_clsid				= -1;
	m_ini_file					= 0;
	m_spawned					= false;
}

void CGameObject::Load(LPCSTR section)
{
	inherited::Load			(section);
	ISpatial*		self				= smart_cast<ISpatial*> (this);
	if (self)	{
		// #pragma todo("to Dima: All objects are visible for AI ???")
		// self->spatial.type	|=	STYPE_VISIBLEFORAI;	
		self->spatial.type	&= ~STYPE_REACTTOSOUND;
	}
}

void CGameObject::reinit	()
{
	m_visual_callback.clear	();
	ai_location().reinit	();

	// clear callbacks
	for (auto& pair : m_callbacks)
		pair.second->m_callback.clear();
}

void CGameObject::reload	(LPCSTR section)
{
	m_script_clsid				= object_factory().script_clsid(CLS_ID);
}

void CGameObject::net_Destroy	()
{
#ifdef DEBUG
	if (psAI_Flags.test(aiDestroy))
		Msg					("Destroying client object [%d][%s][%x]",ID(),*cName(),this);
#endif

	VERIFY					(m_spawned);
	if (!m_spawned)
		Msg("!![%s] Already destroyed object detected: [%s]", __FUNCTION__, this->cName().c_str());

	if(animation_movement_controlled())
					destroy_anim_mov_ctrl	();

	xr_delete				(m_ini_file);

	m_script_clsid			= -1;
	if (Visual() && smart_cast<IKinematics*>(Visual()))
		smart_cast<IKinematics*>(Visual())->Callback	(0,0);

	inherited::net_Destroy						();
	setReady									(FALSE);
	g_pGameLevel->Objects.net_Unregister		(this);
	
	if (this == Level().CurrentEntity())
	{
		Level().SetEntity						(0);
		Level().SetControlEntity				(0);
	}

	Level().RemoveObject_From_4CrPr(this);

//.	Parent									= 0;

	CScriptBinder::net_Destroy				();

	xr_delete								(m_lua_game_object);
	m_spawned								= false;
}

void CGameObject::OnEvent		(NET_Packet& P, u16 type)
{
	switch (type)
	{
	case GE_HIT:
	case GE_HIT_STATISTIC:
		{
/*
			u16				id,weapon_id;
			Fvector			dir;
			float			power, impulse;
			s16				element;
			Fvector			position_in_bone_space;
			u16				hit_type;
			float			ap = 0.0f;

			P.r_u16			(id);
			P.r_u16			(weapon_id);
			P.r_dir			(dir);
			P.r_float		(power);
			P.r_s16			(element);
			P.r_vec3		(position_in_bone_space);
			P.r_float		(impulse);
			P.r_u16			(hit_type);	//hit type
			if ((ALife::EHitType)hit_type == ALife::eHitTypeFireWound)
			{
				P.r_float	(ap);
			}

			CObject*	Hitter = Level().Objects.net_Find(id);
			CObject*	Weapon = Level().Objects.net_Find(weapon_id);

			SHit	HDS = SHit(power, dir, Hitter, element, position_in_bone_space, impulse, (ALife::EHitType)hit_type, ap);
*/
			SHit	HDS;
			HDS.PACKET_TYPE = type;
			HDS.Read_Packet_Cont(P);
//			Msg("Hit received: %d[%d,%d]", HDS.whoID, HDS.weaponID, HDS.BulletID);
			CObject*	Hitter = Level().Objects.net_Find(HDS.whoID);
			CObject*	Weapon = Level().Objects.net_Find(HDS.weaponID);
			HDS.who		= Hitter;
			//-------------------------------------------------------
			SetHitInfo(Hitter, Weapon, HDS.bone(), HDS.p_in_bone_space, HDS.dir);
			Hit				(&HDS);
		}
		break;
	case GE_DESTROY:
		{
			if(H_Parent())
			{
				Msg("GE_DESTROY arrived, but H_Parent() exist. object[%d][%s] parent[%d][%s] [%d]", 
					ID(), cName().c_str(),
					H_Parent()->ID(), H_Parent()->cName().c_str(),
					Device.dwFrame);
			}
			setDestroy		(TRUE);
		}
		break;
	}
}

void VisualCallback(IKinematics *tpKinematics);

BOOL CGameObject::net_Spawn		(CSE_Abstract*	DC)
{
	VERIFY							(!m_spawned);
	if (m_spawned)
		Msg("!![%s] Already spawned object detected: [%s]", __FUNCTION__, this->cName().c_str());

	m_spawned						= true;
	m_spawn_time					= Device.dwFrame;
	CSE_Abstract					*E = (CSE_Abstract*)DC;
	VERIFY							(E);

	// Naming
	cName_set						(E->s_name);
	cNameSect_set					(E->s_name);
	if (E->name_replace()[0])
		cName_set					(E->name_replace());

	setID							(E->ID);
//	R_ASSERT(Level().Objects.net_Find(E->ID) == NULL);

	const CSE_Visual				*visual = smart_cast<const CSE_Visual*>(E);
	if (visual) {

		LPCSTR saved_visual = visual_name(E);

		if (pSettings->line_exist(cNameSect(), "visual")) {
			shared_str config_visual = pSettings->r_string(cNameSect(), "visual");

			string_path config_visual_file;
			if (0 == strext(*config_visual))
				strconcat(sizeof(config_visual_file), config_visual_file, *config_visual, ".ogf");
			else
				strcpy_s(config_visual_file, sizeof(config_visual_file), *config_visual);
			xr_strlwr(config_visual_file);

			string_path saved_visual_file;
			if (0 == strext(saved_visual))
				strconcat(sizeof(saved_visual_file), saved_visual_file, saved_visual, ".ogf");
			else
				strcpy_s(saved_visual_file, sizeof(saved_visual_file), saved_visual);
			xr_strlwr(saved_visual_file);

			bool keep_visual = READ_IF_EXISTS(pSettings, r_bool, cNameSect().c_str(), "keep_visual", false);
			if (keep_visual) {
				if (xr_strcmp(config_visual_file, saved_visual_file)) {
					Msg("! [%s]: changed visual_name[%s] found in %s, keep original %s instead", __FUNCTION__, saved_visual, cName().c_str(), config_visual.c_str());
				}
			}
			else if (!FS.exist(saved_visual) && !FS.exist("$level$", saved_visual_file) && !FS.exist("$game_meshes$", saved_visual_file)) {
				Msg("! [%s]: visual_name[%s] not found in %s, keep original %s instead", __FUNCTION__, saved_visual, cName().c_str(), config_visual.c_str());
			}
			else
				cNameVisual_set(saved_visual);
		}
		else
			cNameVisual_set(saved_visual);

		if (visual->flags.test(CSE_Visual::flObstacle)) {
			ISpatial				*self = smart_cast<ISpatial*>(this);
			self->spatial.type |= STYPE_OBSTACLE;
		}
	}
	
	// XForm
	XFORM().setXYZ					(E->o_Angle);
	Position().set					(E->o_Position);
#ifdef DEBUG
	if(ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject)&&stricmp(PH_DBG_ObjectTrack(),*cName())==0)
	{
		Msg("CGameObject::net_Spawn obj %s Position set from CSE_Abstract %f,%f,%f",PH_DBG_ObjectTrack(),Position().x,Position().y,Position().z);
	}
#endif
	VERIFY							(_valid(renderable.xform));
	VERIFY							(!fis_zero(DET(renderable.xform)));
	CSE_ALifeObject					*O = smart_cast<CSE_ALifeObject*>(E);
	if (O && xr_strlen(O->m_ini_string)) {
#pragma warning(push)
#pragma warning(disable:4238)
		m_ini_file					= xr_new<CInifile>(
			&IReader				(
				(void*)(*(O->m_ini_string)),
				O->m_ini_string.size()
			),
			FS.get_path("$game_config$")->m_Path
		);
#pragma warning(pop)
	}

	m_story_id						= ALife::_STORY_ID(-1);
	if (O)
		m_story_id					= O->m_story_id;

	// Net params
	setLocal						(E->s_flags.is(M_SPAWN_OBJECT_LOCAL));
	if (Level().IsDemoPlay() && OnClient())
	{
		setLocal(FALSE);
	};

	setReady						(TRUE);
	g_pGameLevel->Objects.net_Register	(this);

	m_server_flags.one				();
	if (O) {
		m_server_flags					= O->m_flags;
		if (O->m_flags.is(CSE_ALifeObject::flVisibleForAI))
			spatial.type				|= STYPE_VISIBLEFORAI;
		else
			spatial.type				= (spatial.type | STYPE_VISIBLEFORAI) ^ STYPE_VISIBLEFORAI;
	}
	if ( pSettings->line_exist( cNameSect(), "use_ai_locations" ) )
	  SetUseAI_Locations( !!pSettings->r_bool( cNameSect(), "use_ai_locations" ) );

	reload						(*cNameSect());
	CScriptBinder::reload	(*cNameSect());
	
	reinit						();
	CScriptBinder::reinit	();
#ifdef DEBUG
	if(ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject)&&stricmp(PH_DBG_ObjectTrack(),*cName())==0)
	{
		Msg("CGameObject::net_Spawn obj %s After Script Binder reinit %f,%f,%f",PH_DBG_ObjectTrack(),Position().x,Position().y,Position().z);
	}
#endif
	//load custom user data from server
	if(!E->client_data.empty())
	{	
//		Msg				("client data is present for object [%d][%s], load is processed",ID(),*cName());
		IReader			ireader = IReader(&*E->client_data.begin(), E->client_data.size());
		net_Load		(ireader);
	}
	else
	{
//		Msg				("no client data for object [%d][%s], load is skipped",ID(),*cName());
	}

	// if we have a parent
	if (0xffff != E->ID_Parent) {
		
		if (!Parent) {
			// // we need this to prevent illegal ref_dec/ref_add
			// this is obsolete, since ref_dec/ref_add are removed
			// but I propose do not touch this, or touch and then
			// test the whole spawn sequence
			Parent				= this;
			inherited::net_Spawn(DC);
			Parent				= 0;
		}
		else
			inherited::net_Spawn(DC);
	}
	else {
		if (ai().get_level_graph()) {
			CSE_ALifeObject			*l_tpALifeObject = smart_cast<CSE_ALifeObject*>(E);
			CSE_Temporary			*l_tpTemporary	= smart_cast<CSE_Temporary*>	(E);
			if (l_tpALifeObject && ai().level_graph().valid_vertex_id(l_tpALifeObject->m_tNodeID))
				ai_location().level_vertex	(l_tpALifeObject->m_tNodeID);
			else
				if (l_tpTemporary && ai().level_graph().valid_vertex_id(l_tpTemporary->m_tNodeID))
					ai_location().level_vertex	(l_tpTemporary->m_tNodeID);

			if (l_tpALifeObject && ai().game_graph().valid_vertex_id(l_tpALifeObject->m_tGraphID))
				ai_location().game_vertex			(l_tpALifeObject->m_tGraphID);

			if ( !_valid( Position() ) ) {
			  Fvector vertex_pos = ai().level_graph().vertex_position( ai_location().level_vertex_id() );
			  Msg( "! [%s]: %s has invalid Position[%f,%f,%f] level_vertex_id[%u][%f,%f,%f]", __FUNCTION__, cName().c_str(), Position().x, Position().y, Position().z, ai_location().level_vertex_id(), vertex_pos.x, vertex_pos.y, vertex_pos.z  );
			  Position().set( vertex_pos );
			  auto se_obj = alife_object();
			  if ( se_obj )
			    se_obj->o_Position.set( Position() );
			}

			validate_ai_locations	(false);

			// validating position
			if	(
					UsedAI_Locations() && 
					ai().level_graph().inside(
						ai_location().level_vertex_id(),
						Position()
					) &&
					can_validate_position_on_spawn()
				)
				Position().y		= EPS_L + ai().level_graph().vertex_plane_y(*ai_location().level_vertex(),Position().x,Position().z);
		
		}
 		inherited::net_Spawn	(DC);
	}

	m_bObjectRemoved			= false;

	spawn_supplies				();
#ifdef DEBUG
	if(ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject)&&stricmp(PH_DBG_ObjectTrack(),*cName())==0)
	{
		Msg("CGameObject::net_Spawn obj %s Before CScriptBinder::net_Spawn %f,%f,%f",PH_DBG_ObjectTrack(),Position().x,Position().y,Position().z);
	}
	BOOL ret =CScriptBinder::net_Spawn(DC);
#else
	return						(CScriptBinder::net_Spawn(DC));
#endif

#ifdef DEBUG
	if(ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject)&&stricmp(PH_DBG_ObjectTrack(),*cName())==0)
	{
		Msg("CGameObject::net_Spawn obj %s Before CScriptBinder::net_Spawn %f,%f,%f",PH_DBG_ObjectTrack(),Position().x,Position().y,Position().z);
	}
return ret;
#endif
}

void CGameObject::net_Save		(NET_Packet &net_packet)
{
	u32							position;
	net_packet.w_chunk_open16	(position);
	save						(net_packet);

	// Script Binder Save ---------------------------------------
#ifdef DEBUG	
	if (psAI_Flags.test(aiSerialize))	{
		Msg(">> **** Save script object [%s] *****", *cName());
		Msg(">> Before save :: packet position = [%u]", net_packet.w_tell());
	}

#endif

	CScriptBinder::save			(net_packet);

#ifdef DEBUG	

	if (psAI_Flags.test(aiSerialize))	{
		Msg(">> After save :: packet position = [%u]", net_packet.w_tell());
	}
#endif

	// ----------------------------------------------------------

	net_packet.w_chunk_close16	(position);
}

void CGameObject::net_Load		(IReader &ireader)
{
	load					(ireader);

	// Script Binder Load ---------------------------------------
#ifdef DEBUG	
	if (psAI_Flags.test(aiSerialize))	{
		Msg(">> **** Load script object [%s] *****", *cName());
		Msg(">> Before load :: reader position = [%i]", ireader.tell());
	}

#endif

	CScriptBinder::load		(ireader);


#ifdef DEBUG	

	if (psAI_Flags.test(aiSerialize))	{
		Msg(">> After load :: reader position = [%i]", ireader.tell());
	}
#endif
	// ----------------------------------------------------------
#ifdef DEBUG
	if(ph_dbg_draw_mask1.test(ph_m1_DbgTrackObject)&&stricmp(PH_DBG_ObjectTrack(),*cName())==0)
	{
		Msg("CGameObject::net_Load obj %s (loaded) %f,%f,%f",PH_DBG_ObjectTrack(),Position().x,Position().y,Position().z);
	}

#endif

}

void CGameObject::save			(NET_Packet &output_packet) 
{
}

void CGameObject::load			(IReader &input_packet)
{
}

void CGameObject::spawn_supplies()
{
	if (!spawn_ini() || ai().get_alife())
		return;

	if (!spawn_ini()->section_exist("spawn"))
		return;

	LPCSTR					N,V;
	float					p;
	bool bScope				=	false;
	bool bSilencer			=	false;
	bool bLauncher			=	false;

	for (u32 k = 0, j; spawn_ini()->r_line("spawn",k,&N,&V); k++) {
		VERIFY				(xr_strlen(N));
		j					= 1;
		p					= 1.f;
		
		float f_cond						= 1.0f;
		if (V && xr_strlen(V)) {
			int				n = _GetItemCount(V);
			string16		temp;
			if (n > 0)
				j			= atoi(_GetItem(V,0,temp)); //count
			
			if(NULL!=strstr(V,"prob="))
				p			=(float)atof(strstr(V,"prob=")+5);
			if (fis_zero(p))p = 1.f;
			if (!j)	j		= 1;
			if(NULL!=strstr(V,"cond="))
				f_cond		= (float)atof(strstr(V,"cond=")+5);
			bScope			=	(NULL!=strstr(V,"scope"));
			bSilencer		=	(NULL!=strstr(V,"silencer"));
			bLauncher		=	(NULL!=strstr(V,"launcher"));

		}
		for (u32 i=0; i<j; ++i)
			if (::Random.randF(1.f) < p){
				CSE_Abstract* A=Level().spawn_item	(N,Position(),ai_location().level_vertex_id(),ID(),true);

				CSE_ALifeInventoryItem*	pSE_InventoryItem = smart_cast<CSE_ALifeInventoryItem*>(A);
				if(pSE_InventoryItem)
						pSE_InventoryItem->m_fCondition = f_cond;

				CSE_ALifeItemWeapon* W =  smart_cast<CSE_ALifeItemWeapon*>(A);
				if (W) {
					if (W->m_scope_status			== CSE_ALifeItemWeapon::eAddonAttachable)
						W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonScope, bScope);
					if (W->m_silencer_status		== CSE_ALifeItemWeapon::eAddonAttachable)
						W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonSilencer, bSilencer);
					if (W->m_grenade_launcher_status == CSE_ALifeItemWeapon::eAddonAttachable)
						W->m_addon_flags.set(CSE_ALifeItemWeapon::eWeaponAddonGrenadeLauncher, bLauncher);
				}

				NET_Packet					P;
				A->Spawn_Write				(P,TRUE);
				Level().Send				(P,net_flags(TRUE));
				F_entity_Destroy			(A);
		}
	}
}

void CGameObject::setup_parent_ai_locations(bool assign_position)
{
//	CGameObject				*l_tpGameObject	= static_cast<CGameObject*>(H_Root());
	VERIFY					(H_Parent());
	CGameObject				*l_tpGameObject	= static_cast<CGameObject*>(H_Parent());
	VERIFY					(l_tpGameObject);

	// get parent's position
	if (assign_position && use_parent_ai_locations())
		Position().set		(l_tpGameObject->Position());

	// setup its ai locations
	if (!UsedAI_Locations())
		return;

	if (!ai().get_level_graph())
		return;

	if (l_tpGameObject->UsedAI_Locations() && ai().level_graph().valid_vertex_id(l_tpGameObject->ai_location().level_vertex_id()))
		ai_location().level_vertex	(l_tpGameObject->ai_location().level_vertex_id());
	else
		validate_ai_locations	(false);
//	VERIFY2						(l_tpGameObject->UsedAI_Locations(),*l_tpGameObject->cNameSect());
//	VERIFY2						(ai().level_graph().valid_vertex_id(l_tpGameObject->ai_location().level_vertex_id()),*cNameSect());
//	ai_location().level_vertex	(l_tpGameObject->ai_location().level_vertex_id());

	if (ai().game_graph().valid_vertex_id(l_tpGameObject->ai_location().game_vertex_id()))
		ai_location().game_vertex	(l_tpGameObject->ai_location().game_vertex_id());
	else
		ai_location().game_vertex	(ai().cross_table().vertex(ai_location().level_vertex_id()).game_vertex_id());
//	VERIFY2						(ai().game_graph().valid_vertex_id(l_tpGameObject->ai_location().game_vertex_id()),*cNameSect());
//	ai_location().game_vertex	(l_tpGameObject->ai_location().game_vertex_id());
}

void CGameObject::validate_ai_locations			(bool decrement_reference)
{
	if (!ai().get_level_graph())
		return;

	if (!UsedAI_Locations()) {
//		if (ai().get_game_graph() && ai().get_cross_table())
//			set_game_vertex		(ai().cross_table().vertex(level_vertex_id()).game_vertex_id());
		return;
	}

//	CTimer							timer;
//	timer.Start						();
	Fvector							center;
	Center							(center);
	center.x						= Position().x;
	center.z						= Position().z;
	u32								l_dwNewLevelVertexID = ai().level_graph().vertex(ai_location().level_vertex_id(),center);

#ifdef DEBUG
//	Msg								("%6d Searching for node for object %s (%.5f seconds)",Device.dwTimeGlobal,*cName(),timer.GetElapsed_sec());
#endif
	VERIFY							(ai().level_graph().valid_vertex_id(l_dwNewLevelVertexID));

#if 0
	if (decrement_reference && (ai_location().level_vertex_id() != l_dwNewLevelVertexID)) {
		Fvector						new_position = ai().level_graph().vertex_position(l_dwNewLevelVertexID);
		if (Position().y - new_position.y >= 1.5f) {
			u32						new_vertex_id = ai().level_graph().vertex(ai_location().level_vertex_id(),center);
			new_vertex_id			= new_vertex_id;
		}
	}
#endif

	if (decrement_reference && (ai_location().level_vertex_id() == l_dwNewLevelVertexID))
		return;

	ai_location().level_vertex		(l_dwNewLevelVertexID);

	if (ai().get_game_graph() && ai().get_cross_table()) {
		ai_location().game_vertex	(ai().cross_table().vertex(ai_location().level_vertex_id()).game_vertex_id());
		VERIFY						(ai().game_graph().valid_vertex_id(ai_location().game_vertex_id()));
	}
}

void CGameObject::spatial_move	()
{
	if (H_Parent())
		setup_parent_ai_locations	();
	else
		if (Visual())
			validate_ai_locations	();

	inherited::spatial_move			();
}

#ifdef DEBUG
void			CGameObject::dbg_DrawSkeleton	()
{
	CCF_Skeleton* Skeleton = smart_cast<CCF_Skeleton*>(collidable.model);
	if (!Skeleton) return;
	Skeleton->_dbg_refresh();

	const CCF_Skeleton::ElementVec& Elements = Skeleton->_GetElements();
	for (CCF_Skeleton::ElementVec::const_iterator I=Elements.begin(); I!=Elements.end(); I++){
		if (!I->valid())		continue;
		switch (I->type){
			case SBoneShape::stBox:{
				Fmatrix M;
				M.invert			(I->b_IM);
				Fvector h_size		= I->b_hsize;
				Level().debug_renderer().draw_obb	(M, h_size, color_rgba(0, 255, 0, 255));
								   }break;
			case SBoneShape::stCylinder:{
				Fmatrix M;
				M.c.set				(I->c_cylinder.m_center);
				M.k.set				(I->c_cylinder.m_direction);
				Fvector				h_size;
				h_size.set			(I->c_cylinder.m_radius,I->c_cylinder.m_radius,I->c_cylinder.m_height*0.5f);
				Fvector::generate_orthonormal_basis(M.k,M.j,M.i);
				Level().debug_renderer().draw_obb	(M, h_size, color_rgba(0, 127, 255, 255));
										}break;
			case SBoneShape::stSphere:{
				Fmatrix				l_ball;
				l_ball.scale		(I->s_sphere.R, I->s_sphere.R, I->s_sphere.R);
				l_ball.translate_add(I->s_sphere.P);
				Level().debug_renderer().draw_ellipse(l_ball, color_rgba(0, 255, 0, 255));
									  }break;
		};
	};	
}
#endif

void CGameObject::renderable_Render	()
{
	inherited::renderable_Render();
	::Render->set_Transform		(&XFORM());
	::Render->add_Visual		(Visual());
	Visual()->getVisData().hom_frame = Device.dwFrame;
}

/*
float CGameObject::renderable_Ambient	()
{
	return (ai().get_level_graph() && ai().level_graph().valid_vertex_id(level_vertex_id()) ? float(level_vertex()->light()/15.f) : 1.f);
}
*/

CObject::SavedPosition CGameObject::ps_Element(u32 ID) const
{
	VERIFY(ID<ps_Size());
	inherited::SavedPosition	SP	=	PositionStack[ID];
	return SP;
}

void CGameObject::u_EventGen(NET_Packet& P, u32 type, u32 dest)
{
	P.w_begin	(M_EVENT);
	P.w_u32		(Level().timeServer());
	P.w_u16		(u16(type&0xffff));
	P.w_u16		(u16(dest&0xffff));
}

void CGameObject::u_EventSend(NET_Packet& P, u32 dwFlags )
{
	Level().Send(P, dwFlags);
}

#include "bolt.h"
void CGameObject::OnH_B_Chield()
{
	inherited::OnH_B_Chield();
	///PHSetPushOut();????
}

void CGameObject::OnH_B_Independent(bool just_before_destroy)
{
	inherited::OnH_B_Independent(just_before_destroy);

	setup_parent_ai_locations	(false);

	CGameObject					*parent = smart_cast<CGameObject*>(H_Parent());
	VERIFY						(parent);
	if (ai().get_level_graph() && ai().level_graph().valid_vertex_id(parent->ai_location().level_vertex_id()))
		validate_ai_locations	(false);
}


#ifdef DEBUG

void CGameObject::OnRender()
{
	if (bDebug && Visual())
	{
		Fvector bc,bd; 
		Visual()->vis.box.get_CD	(bc,bd);
		Fmatrix	M = XFORM();		M.c.add (bc);
		Level().debug_renderer().draw_obb			(M,bd,color_rgba(0,0,255,255));
	}	
}
#endif

BOOL CGameObject::UsedAI_Locations()
{
	return					(m_server_flags.test(CSE_ALifeObject::flUsedAI_Locations));
}

void CGameObject::SetUseAI_Locations(bool _use)
{ 
	m_server_flags.set(CSE_ALifeObject::flUsedAI_Locations, _use); 
}

BOOL CGameObject::TestServerFlag(u32 Flag) const
{
	return					(m_server_flags.test(Flag));
}

void CGameObject::add_visual_callback		(visual_callback *callback)
{
	VERIFY(smart_cast<IKinematics*>(Visual()));
	VERIFY(std::find(visual_callbacks().begin(), visual_callbacks().end(), callback) == visual_callbacks().end());

	if (m_visual_callback.empty())	SetKinematicsCallback(true);
//		smart_cast<IKinematics*>(Visual())->Callback(VisualCallback,this);
	m_visual_callback.push_back	(callback);
}

void CGameObject::remove_visual_callback	(visual_callback *callback)
{
	CALLBACK_VECTOR_IT			I = std::find(m_visual_callback.begin(),m_visual_callback.end(),callback);
	VERIFY						(I != m_visual_callback.end());
	m_visual_callback.erase		(I);
	if (m_visual_callback.empty())	SetKinematicsCallback(false);
//		smart_cast<IKinematics*>(Visual())->Callback(0,0);
}

void CGameObject::SetKinematicsCallback		(bool set)
{
	if(!Visual())	return;
	if (set)
		smart_cast<IKinematics*>(Visual())->Callback(VisualCallback,this);
	else
		smart_cast<IKinematics*>(Visual())->Callback(0,0);
};

void VisualCallback	(IKinematics *tpKinematics)
{
	CGameObject						*game_object = static_cast<CGameObject*>(static_cast<CObject*>(tpKinematics->GetUpdateCallbackParam()));
	VERIFY							(game_object);
	
	CGameObject::CALLBACK_VECTOR_IT	I = game_object->visual_callbacks().begin();
	CGameObject::CALLBACK_VECTOR_IT	E = game_object->visual_callbacks().end();
	for ( ; I != E; ++I)
		(*I)						(tpKinematics);
}

CScriptGameObject *CGameObject::lua_game_object() const
{
	if (!m_spawned)
		Msg("!! [%s] you are trying to use a destroyed object [%x]", __FUNCTION__, this);

	THROW							(m_spawned);
	if (!m_lua_game_object)
		m_lua_game_object			= xr_new<CScriptGameObject>(const_cast<CGameObject*>(this));
	return							(m_lua_game_object);
}

void CGameObject::DestroyObject()			
{
	
	if(m_bObjectRemoved)	return;
	m_bObjectRemoved		= true;
	if (getDestroy())		return;

	if (Local())
	{	
		NET_Packet		P;
		u_EventGen		(P,GE_DESTROY,ID());
		u_EventSend		(P);
	}
}

void CGameObject::shedule_Update	(u32 dt)
{
	// Msg							("-SUB-:[%x][%s] CGameObject::shedule_Update",smart_cast<void*>(this),*cName());
	inherited::shedule_Update	(dt);
        FeelTouchAddonsUpdate();
	CScriptBinder::shedule_Update(dt);
}

BOOL CGameObject::net_SaveRelevant	()
{
	return	(CScriptBinder::net_SaveRelevant());
}

//игровое имя объекта
LPCSTR CGameObject::Name () const
{
	return	(*cName());
}

u32	CGameObject::ef_creature_type		() const
{
	string16	temp; CLSID2TEXT(CLS_ID,temp);
	R_ASSERT3	(false,"Invalid creature type request, virtual function is not properly overridden!",temp);
	return		(u32(-1));
}

u32	CGameObject::ef_equipment_type		() const
{
	string16	temp; CLSID2TEXT(CLS_ID,temp);
	R_ASSERT3	(false,"Invalid equipment type request, virtual function is not properly overridden!",temp);
	return		(u32(-1));
//	return		(6);
}

u32	CGameObject::ef_main_weapon_type	() const
{
	string16	temp; CLSID2TEXT(CLS_ID,temp);
	R_ASSERT3	(false,"Invalid main weapon type request, virtual function is not properly overridden!",temp);
	return		(u32(-1));
//	return		(5);
}

u32	CGameObject::ef_anomaly_type		() const
{
	string16	temp; CLSID2TEXT(CLS_ID,temp);
	R_ASSERT3	(false,"Invalid anomaly type request, virtual function is not properly overridden!",temp);
	return		(u32(-1));
}

u32	CGameObject::ef_weapon_type			() const
{
	string16	temp; CLSID2TEXT(CLS_ID,temp);
	R_ASSERT3	(false,"Invalid weapon type request, virtual function is not properly overridden!",temp);
	return		(u32(-1));
//	return		(u32(0));
}

u32	CGameObject::ef_detector_type		() const
{
	string16	temp; CLSID2TEXT(CLS_ID,temp);
	R_ASSERT3	(false,"Invalid detector type request, virtual function is not properly overridden!",temp);
	return		(u32(-1));
}

void CGameObject::net_Relcase			(CObject* O)
{
	inherited::net_Relcase		(O);
        FeelTouchAddonsRelcase( O );
	CScriptBinder::net_Relcase	(O);
}

CScriptCallbackEx<void>& CGameObject::callback(GameObject::ECallbackType type) const
{
	if (auto it = m_callbacks.find(type); it != m_callbacks.end())
		return (*it).second->m_callback;
	else //KRodin: вот по-нормальному, надо возвращать nullptr и проверять в вызывающей функции, есть ли каллбек вообще. Но это кучу функций переделать надо, поэтому буду возвращать фейковый каллбек.
		return m_callbacks.at(GameObject::eDummy)->m_callback;
}

LPCSTR CGameObject::visual_name			(CSE_Abstract *server_entity)
{
	const CSE_Visual			*visual	= smart_cast<const CSE_Visual*>(server_entity);
	VERIFY						(visual);
	return						(visual->get_visual());
}

void CGameObject::update_animation_movement_controller	()
{
	if (!animation_movement_controlled())
		return;

	m_anim_mov_ctrl->OnFrame	();

	if (m_anim_mov_ctrl->isActive())
		return;

	destroy_anim_mov_ctrl		();
}

void	CGameObject::		UpdateCL			( )
{
	inherited::UpdateCL			();
}

void	CGameObject::UpdateXFORM(const Fmatrix &upd)
{
	XFORM() = upd;	
	IKinematics *pK = PKinematics(Visual());
	if (pK)
	{
		Visual()->getVisData().sphere.P = upd.c;
		pK->CalculateBones_Invalidate();	 // позволит объекту быстрее объявиться в новой точке			
	}

	// OnChangePosition processing
	spatial_move();	
	/*

	const CLevelGraph &graph = ai().level_graph();
	if (graph.valid_vertex_position(upd.c))
	{
		u32 lvid = graph.vertex_id (upd.c);
		u16 gvid = ai().cross_table().vertex(lvid).game_vertex_id();
		ai_location().level_vertex(lvid);
	}
	*/
}

void	CGameObject::OnChangeVisual	( )
{
	inherited::OnChangeVisual( );
	if( animation_movement_controlled( ) )
	{
		destroy_anim_mov_ctrl( );
	}
}
bool CGameObject::shedule_Needed( )
{
	return						( !getDestroy( ) );
//	return						(processing_enabled() || CScriptBinder::object());
};

void	CGameObject::		create_anim_mov_ctrl( CBlend* b )
{
	//if(m_anim_mov_ctrl)
		//destroy_anim_mov_ctrl( );
	VERIFY( !animation_movement() );
	VERIFY(Visual());
	IKinematics *K = Visual( )->dcast_PKinematics( );
	VERIFY( K );
	m_anim_mov_ctrl = xr_new<animation_movement_controller>( &XFORM(), K, b ); 
}
void	CGameObject::		destroy_anim_mov_ctrl()
{
	xr_delete( m_anim_mov_ctrl );
}

///////////////////Перенесено из ai_object_location_impl.h////////////////////////
IC	void CAI_ObjectLocation::init()
{
	if (ai().get_level_graph())
		ai().level_graph().set_invalid_vertex(m_level_vertex_id);
	else
		m_level_vertex_id = u32(-1);

	if (ai().get_game_graph())
		ai().game_graph().set_invalid_vertex(m_game_vertex_id);
	else
		m_game_vertex_id = GameGraph::_GRAPH_ID(-1);
}

/*IC*/	void CAI_ObjectLocation::game_vertex(const CGameGraph::CVertex *game_vertex)
{
	VERIFY(ai().game_graph().valid_vertex_id(ai().game_graph().vertex_id(game_vertex)));
	m_game_vertex_id = ai().game_graph().vertex_id(game_vertex);
}

/*IC*/	void CAI_ObjectLocation::game_vertex(const GameGraph::_GRAPH_ID	game_vertex_id)
{
	VERIFY(ai().game_graph().valid_vertex_id(game_vertex_id));
	m_game_vertex_id = game_vertex_id;
}

/*IC*/	const CGameGraph::CVertex *CAI_ObjectLocation::game_vertex() const
{
	VERIFY(ai().game_graph().valid_vertex_id(m_game_vertex_id));
	return				(ai().game_graph().vertex(m_game_vertex_id));
}

/*IC*/	void CAI_ObjectLocation::level_vertex(const CLevelGraph::CVertex *level_vertex)
{
	VERIFY(ai().level_graph().valid_vertex_id(ai().level_graph().vertex_id(level_vertex)));
	m_level_vertex_id = ai().level_graph().vertex_id(level_vertex);
}

/*IC*/	void CAI_ObjectLocation::level_vertex(const u32 level_vertex_id)
{
	VERIFY(ai().level_graph().valid_vertex_id(level_vertex_id));
	m_level_vertex_id = level_vertex_id;
}

/*IC*/	const CLevelGraph::CVertex *CAI_ObjectLocation::level_vertex() const
{
	VERIFY(ai().level_graph().valid_vertex_id(m_level_vertex_id));
	return				(ai().level_graph().vertex(m_level_vertex_id));
}
//////////////////////////////////////////////////////////////////////////////////


void CGameObject::addFeelTouch( float radius, const luabind::object& lua_object, const luabind::functor<void>& new_delete, const luabind::functor<bool>& contact ) {
  CScriptCallbackEx<bool> feel_touch_contact;
  CScriptCallbackEx<void> feel_touch_new_delete;
  feel_touch_new_delete.set( new_delete, lua_object );
  if ( contact )
    feel_touch_contact.set( contact, lua_object );
  for ( auto& ft : feel_touch_addons ) {
    if (
      ft->feel_touch_new_delete == feel_touch_new_delete
      && ( !contact || ft->feel_touch_contact == feel_touch_contact )
    ) {
      ft->radius = radius;
      return;
    }
  }
  FeelTouchAddon* ft = xr_new<FeelTouchAddon>();
  feel_touch_addons.push_back( ft );
  ft->radius = radius;
  ft->feel_touch_new_delete = feel_touch_new_delete;
  if ( contact )
    ft->feel_touch_contact = feel_touch_contact;
}


void CGameObject::removeFeelTouch( const luabind::object& lua_object, const luabind::functor<void>& new_delete, const luabind::functor<bool>& contact ) {
  CScriptCallbackEx<bool> feel_touch_contact;
  CScriptCallbackEx<void> feel_touch_new_delete;
  feel_touch_new_delete.set( new_delete, lua_object );
  if ( contact )
    feel_touch_contact.set( contact, lua_object );
  if ( feel_touch_processing ) {
    for ( auto& ft : feel_touch_addons ) {
      if (
        ft->feel_touch_new_delete == feel_touch_new_delete
        && ( !contact || ft->feel_touch_contact == feel_touch_contact )
      ) {
        ft->radius = -1;
        feel_touch_changed = true;
        break;
      }
    }
  }
  else {
    feel_touch_addons.erase(
      std::remove_if(
        feel_touch_addons.begin(), feel_touch_addons.end(),
        [&]( auto& ft ) {
          if (
            ft->feel_touch_new_delete == feel_touch_new_delete
            && ( !contact || ft->feel_touch_contact == feel_touch_contact )
          ) {
            xr_delete( ft );
            return true;
          }
          return false;
        }
      ),
      feel_touch_addons.end()
    );
  }
}


void CGameObject::FeelTouchAddonsUpdate() {
  feel_touch_changed    = false;
  feel_touch_processing = true;
  for ( auto& ft : feel_touch_addons ) {
    if ( ft->radius <= 0 ) continue;
    ft->feel_touch.feel_touch_update(
      Position(), ft->radius,
      [&]( const auto O, bool is_new ) {
        CGameObject* GO = smart_cast<CGameObject*>( O );
        ft->feel_touch_new_delete( GO->lua_game_object(), is_new );
      },
      [&]( const auto O ) -> bool {
        CGameObject* GO = smart_cast<CGameObject*>( O );
        if ( !GO ) return false;
        if ( ft->feel_touch_contact ) {
          return ft->feel_touch_contact( GO->lua_game_object() );
        }
        else
          return ( smart_cast<CEntityAlive*>( O ) || smart_cast<CCar*>( O ) ) ? true : false;
      }
    );
  }
  feel_touch_processing = false;
  if ( feel_touch_changed ) {
    feel_touch_addons.erase(
      std::remove_if(
        feel_touch_addons.begin(), feel_touch_addons.end(),
        []( auto& ft ) {
          return ft->radius < 0;
        }
      ),
      feel_touch_addons.end()
    );
    feel_touch_changed = false;
  }
}


void CGameObject::FeelTouchAddonsRelcase( CObject* O ) {
  if ( Level().is_removing_objects() ) return;
  for ( auto& ft : feel_touch_addons ) {
    ft->feel_touch.feel_touch_relcase2(
      O,
      [&]( const auto O, bool is_new ) {
        CGameObject* GO = smart_cast<CGameObject*>( O );
        ft->feel_touch_new_delete( GO->lua_game_object(), is_new );
      }
    );
  }
}
