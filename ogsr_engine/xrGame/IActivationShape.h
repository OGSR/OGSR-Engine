#pragma once
class CPhysicsShellHolder;
void ActivateShapeExplosive(CPhysicsShellHolder* self_obj, const Fvector &size, Fvector &out_size, Fvector &in_out_pos);
void ActivateShapePhysShellHolder(CPhysicsShellHolder *obj, const Fmatrix &in_xform, const Fvector &in_size, Fvector &in_pos, Fvector &out_pos);
bool ActivateShapeCharacterPhysicsSupport(Fvector &out_pos, const Fvector &vbox, const Fvector &activation_pos, const Fmatrix &mXFORM, bool not_collide_characters, bool set_rotation, CPhysicsShellHolder *m_EntityAlife);