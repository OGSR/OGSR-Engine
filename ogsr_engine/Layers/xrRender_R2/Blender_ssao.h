// Blender_ssao.h: interface for the CBlender_ssao class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_SSAO_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
#define AFX_BLENDER_SSAO_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3
#pragma once

class CBlender_ssao : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "OGSE: screen space ambient occlusion"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_ssao();
	virtual ~CBlender_ssao();
};

#endif // !defined(AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
