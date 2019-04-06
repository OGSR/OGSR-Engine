// Blender_DOF.h: interface for the CBlender_DOF class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLENDER_DOF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
#define AFX_BLENDER_DOF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3
#pragma once

class CBlender_dof : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "OGSE: depth of field"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_dof();
	virtual ~CBlender_dof();
};

#endif // !defined(AFX_BLENDER_VERTEX_AREF_H__07141B30_A968_407E_86F8_D12702FE0B9B__INCLUDED_3)
