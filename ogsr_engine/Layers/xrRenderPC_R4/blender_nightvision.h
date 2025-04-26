#pragma once


class CBlender_nightvision : public IBlender
{
public:
	virtual LPCSTR getComment() { return "nightvision"; }

	virtual void Compile(CBlender_Compile& C);

	CBlender_nightvision();
	virtual ~CBlender_nightvision();
};
