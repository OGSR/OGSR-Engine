// Blender_reflections.h: interface for the CBlender_reflections class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_REFLECTIONS_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
#define AFX_BLENDER_REFLECTIONS_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3
#pragma once

class CBlender_reflections : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "INTERNAL: reflections"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_reflections();
	virtual ~CBlender_reflections();
};

#endif // !defined(AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
