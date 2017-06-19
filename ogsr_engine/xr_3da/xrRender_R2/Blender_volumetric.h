// Blender_volumetric.h: interface for the CBlender_volumetric class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_VOLUMETRIC_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
#define AFX_BLENDER_VOLUMETRIC_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3
#pragma once

class CBlender_volumetric : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "INTERNAL: accumulate volumetric light"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_volumetric();
	virtual ~CBlender_volumetric();
};

#endif // !defined(AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
