// Blender_rain_drops.h: interface for the CBlender_rain_drops class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_RAINDROPS_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
#define AFX_BLENDER_RAINDROPS_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3
#pragma once

class CBlender_rain_drops : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "OGSE: rain drops"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_rain_drops();
	virtual ~CBlender_rain_drops();
};

#endif // !defined(AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
