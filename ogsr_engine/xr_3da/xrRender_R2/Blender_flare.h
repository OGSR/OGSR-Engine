// Blender_flare.h: interface for the CBlender_flare class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_FLARE_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
#define AFX_BLENDER_FLARE_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3
#pragma once

class CBlender_flare : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "LEVEL: flare"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_flare();
	virtual ~CBlender_flare();
};

#endif // !defined(AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
