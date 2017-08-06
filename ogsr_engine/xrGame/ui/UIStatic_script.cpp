#include "pch_script.h"
#include "UIStatic.h"

using namespace luabind;

#pragma optimize("s",on)
void CUIStatic::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUIStatic, CUIWindow>("CUIStatic")
		.def(						constructor<>())

		.def("SetText",				(void (CUIStatic::*)(LPCSTR)) (&CUIStatic::SetText) )
		.def("SetTextST",			(void (CUIStatic::*)(LPCSTR)) (&CUIStatic::SetTextST) )
		.def("GetText",				&CUIStatic::GetText)

		.def("SetTextX",				&CUIStatic::SetTextX)
		.def("SetTextY",				&CUIStatic::SetTextY)
		.def("GetTextX",				&CUIStatic::GetTextX)
		.def("GetTextY",				&CUIStatic::GetTextY)
		
		.def("SetColor",			&CUIStatic::SetColor)
		.def("GetColor",			&CUIStatic::GetColor)
		.def("SetTextColor",		&CUIStatic::SetTextColor_script)
		.def("Init",				(void(CUIStatic::*)(float,float,float,float))&CUIStatic::Init )
		.def("Init",				(void(CUIStatic::*)(LPCSTR,float,float,float,float))&CUIStatic::Init )
		.def("InitTexture",			&CUIStatic::InitTexture )
		.def("SetTextureOffset",	&CUIStatic::SetTextureOffset )


		.def("SetOriginalRect",		(void(CUIStatic::*)(float,float,float,float))&CUIStatic::SetOriginalRect)
		.def("SetStretchTexture",	&CUIStatic::SetStretchTexture)
		.def("GetStretchTexture",	&CUIStatic::GetStretchTexture)

		.def("SetTextAlign",		&CUIStatic::SetTextAlign_script)
		.def("GetTextAlign",		&CUIStatic::GetTextAlign_script)

		.def("SetHeading",			&CUIStatic::SetHeading)
		.def("GetHeading",			&CUIStatic::GetHeading)
	
		.def("ClipperOn",			&CUIStatic::ClipperOn)
		.def("ClipperOff",			(void(CUIStatic::*)(void))&CUIStatic::ClipperOff )
		.def("GetClipperState",		&CUIStatic::GetClipperState)

		.def("SetTextComplexMode", &CUIStatic::SetTextComplexMode)
		.def("AdjustWidthToText",	&CUIStatic::AdjustWidthToText)
		.def("AdjustHeightToText", &CUIStatic::AdjustHeightToText)
		.def("SetVTextAlign",		&CUIStatic::SetVTextAlignment)
		.def("SetTextPos",			&CUIStatic::SetTextPos)
	];
}