#include "stdafx.h"
#include "BoneProtections.h"
#include "../skeletonanimated.h"

float SBoneProtections::getBoneProtection	(s16 bone_id)
{
	storage_it it = m_bones_koeff.find(bone_id);
	if( it != m_bones_koeff.end() )
		return it->second.koeff;
	else
		return m_default.koeff;
}

float SBoneProtections::getBoneArmour	(s16 bone_id)
{
	storage_it it = m_bones_koeff.find(bone_id);
	if( it != m_bones_koeff.end() )
		return it->second.armour;
	else
		return m_default.armour;
}

BOOL SBoneProtections::getBonePassBullet(s16 bone_id)
{
	storage_it it = m_bones_koeff.find(bone_id);
	if( it != m_bones_koeff.end() )
		return it->second.BonePassBullet;
	else
		return m_default.BonePassBullet;
}

void SBoneProtections::reload(const shared_str& bone_sect, CKinematics* kinematics)
{
	VERIFY(kinematics);
	m_bones_koeff.clear();

	m_fHitFrac = READ_IF_EXISTS(pSettings, r_float, bone_sect, "hit_fraction",	0.1f);

	m_default.koeff		= 1.0f;
	m_default.armour	= 0.0f;
	m_default.BonePassBullet = FALSE;

	CInifile::Sect	&protections = pSettings->r_section(bone_sect);
	for (CInifile::SectCIt i=protections.Data.begin(); protections.Data.end() != i; ++i) {
		string256 buffer;
		float Koeff = (float)atof( _GetItem(*(*i).second, 0, buffer) );
		float Armour = (float)atof( _GetItem(*(*i).second, 1, buffer) );
		BOOL BonePassBullet = (BOOL) (atoi( _GetItem(*(*i).second, 2, buffer) )>0.5f);
		
		BoneProtection	BP;
		BP.koeff = Koeff;
		BP.armour = Armour;
		BP.BonePassBullet = BonePassBullet;


		if (!xr_strcmp(*(*i).first,"default"))
		{
			m_default = BP;
		}
		else 
		{
			if (!xr_strcmp(*(*i).first,"hit_fraction")) continue;

			s16	bone_id				= kinematics->LL_BoneID(i->first);
			R_ASSERT2				(BI_NONE != bone_id, *(*i).first);			
			m_bones_koeff.insert(mk_pair(bone_id,BP));
		}
	}

}
