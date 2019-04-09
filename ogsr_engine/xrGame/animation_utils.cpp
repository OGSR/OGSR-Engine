#include "stdafx.h"

#include "animation_utils.h"
#include "..\xr_3da\skeletonanimated.h"
#include "..\xr_3da\bone.h"
#include "game_object_space.h"

anim_bone_fix::anim_bone_fix() : bone(nullptr), parent(nullptr), matrix(Fmatrix().identity())
{
}
anim_bone_fix::~anim_bone_fix()
{
	VERIFY(!bone);
	VERIFY(!parent);
}
void anim_bone_fix::callback(CBoneInstance *BI)
{
	anim_bone_fix* fix = (anim_bone_fix*)BI->callback_param();
	VERIFY(fix->bone);
	VERIFY(fix->parent);
	BI->mTransform.mul_43(fix->parent->mTransform, fix->matrix);

	R_ASSERT2(_valid(BI->mTransform), "anim_bone_fix::	callback");
}

void anim_bone_fix::fix(u16 bone_id, CKinematics &K)
{
	//return;
	VERIFY(&K);
	VERIFY(K.LL_GetBoneRoot() != bone_id);

	CBoneInstance &bi = K.LL_GetBoneInstance(bone_id);

	VERIFY(!bi.callback());

	bone = &bi;
	CBoneData &bd = K.LL_GetData(bone_id);

	parent = &K.LL_GetBoneInstance(bd.GetParentID());
	matrix.mul_43(Fmatrix().invert(parent->mTransform), bi.mTransform);
	bi.set_callback(bctCustom, callback, this, TRUE);
}
void	anim_bone_fix::refix()
{
	//return;
	bone->set_callback(bctCustom, callback, this, TRUE);
}
void	anim_bone_fix::deinit()
{
	release();
	bone = NULL;
	parent = NULL;
}
void anim_bone_fix::release()
{
	//return;
	VERIFY(bone->callback() == callback);
	VERIFY(bone->callback_param() == this);
	bone->reset_callback();
}

bool find_in_parents(const u16 bone_to_find, const u16 from_bone, CKinematics &ca)
{
	const u16 root = ca.LL_GetBoneRoot();
	
	for (u16 bi = from_bone; bi != root && bi != BI_NONE; )
	{
		const CBoneData &bd = ca.LL_GetData(bi);
		if (bi == bone_to_find)
			return true;
		bi = bd.GetParentID();
	}
	return false;
}
