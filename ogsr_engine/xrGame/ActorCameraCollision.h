#pragma once
class CPhysicsShell;
class CCameraBase;
class CPhysicsShellHolder;
extern CPhysicsShell* actor_camera_shell;
/*
#ifdef DEBUG
extern BOOL dbg_draw_camera_collision;
extern float	camera_collision_character_skin_depth ;
extern float	camera_collision_character_shift_z ;
#endif
*/
bool test_camera_box(const Fvector& box_size, const Fmatrix& xform, CPhysicsShellHolder* l_actor);
void collide_camera(CCameraBase& camera, float _viewport_near, CPhysicsShellHolder* l_actor, bool on_ladder);