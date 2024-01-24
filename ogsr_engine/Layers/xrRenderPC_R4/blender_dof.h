#pragma once


class CBlender_dof : public IBlender
{
public:
	virtual LPCSTR getComment() { return "DoF"; }

	virtual void Compile(CBlender_Compile& C);

	CBlender_dof();
	virtual ~CBlender_dof();
};
