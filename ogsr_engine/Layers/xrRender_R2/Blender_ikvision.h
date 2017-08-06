// Blender_ikvision.h: interface for the CBlender_ikvision class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_IKVISION_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
#define AFX_BLENDER_IKVISION_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3
#pragma once

class CBlender_ikvision : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "OGSE: ikvision"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_ikvision();
	virtual ~CBlender_ikvision();
};

#endif // !defined(AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
