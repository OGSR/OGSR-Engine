// Blender_fog_volume.h: interface for the CBlender_fog_volume class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_FOG_VOLUME_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
#define AFX_BLENDER_FOG_VOLUME_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3
#pragma once

class CBlender_fog_volume : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "LEVEL: fog volumes"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_fog_volume();
	virtual ~CBlender_fog_volume();
};
#endif // !defined(AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
