void CPHSimpleCharacter::UpdateStaticDamage(dContact* c, SGameMtl* tri_material, bool bo1)
{
	const	dReal	*v = dBodyGetLinearVel(m_body);
	dReal	norm_prg = dFabs(dDOT(v, c->geom.normal));
	dReal	smag = dDOT(v, v);
	dReal	plane_pgr = _sqrt(smag - norm_prg * norm_prg);
	dReal	mag = 0.f;
	if (tri_material->Flags.test(SGameMtl::flPassable))
	{
		mag = _sqrt(smag)*tri_material->fBounceDamageFactor;
	}
	else
	{
		float				vel_prg; vel_prg = std::max(plane_pgr*tri_material->fPHFriction, norm_prg);
		mag = (vel_prg)*tri_material->fBounceDamageFactor;
	}
	if (mag > m_collision_damage_info.m_contact_velocity)
	{
		m_collision_damage_info.m_contact_velocity = mag;
		m_collision_damage_info.m_dmc_signum = bo1 ? 1.f : -1.f;
		m_collision_damage_info.m_dmc_type = SCollisionDamageInfo::ctStatic;
		m_collision_damage_info.m_damege_contact = *c;

		m_collision_damage_info.m_obj_id = u16(-1);
	}
}

void CPHSimpleCharacter::UpdateDynamicDamage(dContact* c, u16 obj_material_idx, dBodyID b, bool bo1)
{
	const dReal* vel = dBodyGetLinearVel(m_body);
	dReal c_vel;
	dMass m;
	dBodyGetMass(b, &m);

	const dReal* obj_vel = dBodyGetLinearVel(b);
	const dReal* norm = c->geom.normal;
	dReal norm_vel = dDOT(vel, norm);
	dReal norm_obj_vel = dDOT(obj_vel, norm);

	if ((bo1&&norm_vel > norm_obj_vel) || (!bo1&&norm_obj_vel > norm_vel))
		return;

	dVector3 Pc = { vel[0] * m_mass + obj_vel[0] * m.mass,vel[1] * m_mass + obj_vel[1] * m.mass,vel[2] * m_mass + obj_vel[2] * m.mass };

	dReal Kself = norm_vel * norm_vel*m_mass / 2.f;
	dReal Kobj = norm_obj_vel * norm_obj_vel*m.mass / 2.f;

	dReal Pcnorm = dDOT(Pc, norm);
	dReal KK = Pcnorm * Pcnorm / (m_mass + m.mass) / 2.f;
	dReal accepted_energy = Kself * m_collision_damage_factor + Kobj * object_damage_factor - KK;

	if (accepted_energy > 0.f)
	{
		SGameMtl	*obj_material = GMLib.GetMaterialByIdx(obj_material_idx);
		c_vel = dSqrt(accepted_energy / m_mass * 2.f)*obj_material->fBounceDamageFactor;
	}
	else c_vel = 0.f;
#ifdef DEBUG
	if (debug_output().ph_dbg_draw_mask().test(phDbgDispObjCollisionDammage) && c_vel > debug_output().dbg_vel_collid_damage_to_display())
	{
		float dbg_my_norm_vell = norm_vel;
		float dbg_obj_norm_vell = norm_obj_vel;
		float dbg_my_kinetic_e = Kself;
		float dbg_obj_kinetic_e = Kobj;
		float dbg_my_effective_e = Kself * m_collision_damage_factor;
		float dbg_obj_effective_e = Kobj * object_damage_factor;
		float dbg_free_energy = KK;
		LPCSTR name = PhysicsRefObject()->ObjectName();

		Msg("-----------------------------------------------------------------------------------------");
		Msg("cd %s -effective vell %f", name, c_vel);
		Msg("cd %s -my_norm_vell %f", name, dbg_my_norm_vell);
		Msg("cd %s -obj_norm_vell %f", name, dbg_obj_norm_vell);
		Msg("cd %s -my_kinetic_e %f", name, dbg_my_kinetic_e);
		Msg("cd %s -obj_kinetic_e %f", name, dbg_obj_kinetic_e);
		Msg("cd %s -my_effective_e %f", name, dbg_my_effective_e);
		Msg("cd %s -obj_effective_e %f", name, dbg_obj_effective_e);
		Msg("cd %s -effective_acceted_e %f", name, accepted_energy);
		Msg("cd %s -real_acceted_e %f", name, Kself + Kobj - KK);
		Msg("cd %s -free_energy %f", name, dbg_free_energy);
		Msg("-----------------------------------------------------------------------------------------");
	}
#endif
	if (c_vel > m_collision_damage_info.m_contact_velocity)
	{
		CPhysicsShellHolder* obj = bo1 ? retrieveRefObject(c->geom.g2) : retrieveRefObject(c->geom.g1);
		VERIFY(obj);
		if (obj && !obj->ObjectGetDestroy())
		{
			m_collision_damage_info.m_contact_velocity = c_vel;
			m_collision_damage_info.m_dmc_signum = bo1 ? 1.f : -1.f;
			m_collision_damage_info.m_dmc_type = SCollisionDamageInfo::ctObject;
			m_collision_damage_info.m_damege_contact = *c;
			m_collision_damage_info.m_hit_callback = obj->ObjectGetCollisionHitCallback();
			m_collision_damage_info.m_obj_id = obj->ObjectID();
		}
	}
}

IC		void	CPHSimpleCharacter::foot_material_update(u16	contact_material_idx,u16	foot_material_idx)
{
	if(*p_lastMaterialIDX!=u16(-1)&&GMLib.GetMaterialByIdx( *p_lastMaterialIDX)->Flags.test(SGameMtl:: flPassable)&&!b_foot_mtl_check)	return			;
	b_foot_mtl_check					=false											;
	if(GMLib.GetMaterialByIdx(contact_material_idx)->Flags.test(SGameMtl:: flPassable))
								*p_lastMaterialIDX=contact_material_idx					;
	else	
								*p_lastMaterialIDX=foot_material_idx					;

}