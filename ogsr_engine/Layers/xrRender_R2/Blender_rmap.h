// Blender_rmap.h: interface for the CBlender_rmap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_RMAP_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
#define AFX_BLENDER_RMAP_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3
#pragma once

class CBlender_rmap : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "INTERNAL: rain map"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_rmap();
	virtual ~CBlender_rmap();
};

#endif // !defined(AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
