#pragma once


class CBlender_dof : public IBlender
{
public:
	virtual LPCSTR getComment() { return "DoF"; }
	virtual BOOL canBeDetailed() { return FALSE; }
	virtual BOOL canBeLMAPped() { return FALSE; }

	virtual void Compile(CBlender_Compile& C);

	CBlender_dof();
	virtual ~CBlender_dof();
};
