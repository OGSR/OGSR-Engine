#pragma once


class CBlender_lut : public IBlender
{
public:
	virtual		LPCSTR		getComment() { return "LUT Shader"; }
	virtual		BOOL		canBeDetailed() { return FALSE; }
	virtual		BOOL		canBeLMAPped() { return FALSE; }

	virtual		void		Compile(CBlender_Compile& C);

	CBlender_lut();
	virtual ~CBlender_lut();
};