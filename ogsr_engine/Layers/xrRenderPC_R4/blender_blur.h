#pragma once


class CBlender_blur : public IBlender
{
public:
	virtual LPCSTR getComment() { return "Blur generation"; }

	virtual void Compile(CBlender_Compile& C);

	CBlender_blur();
	virtual ~CBlender_blur();
};
