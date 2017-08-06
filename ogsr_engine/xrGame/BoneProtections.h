#pragma once

struct SBoneProtections{
	struct BoneProtection {
		float		koeff;
		float		armour;
		BOOL		BonePassBullet;
	};
	float				m_fHitFrac;
	typedef xr_map<s16,BoneProtection>		storage_type;
	typedef storage_type::iterator	storage_it;
						SBoneProtections	()								{m_default.koeff = 1.0f; m_default.armour = 0;}
	BoneProtection		m_default;
	storage_type		m_bones_koeff;
	void				reload				(const shared_str& outfit_section, CKinematics* kinematics);
	float				getBoneProtection	(s16 bone_id);
	float				getBoneArmour		(s16 bone_id);
	BOOL				getBonePassBullet	(s16 bone_id);
};

