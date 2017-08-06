#include "pch_script.h"
#include "UIButton.h"
#include "UI3tButton.h"
#include "UICheckButton.h"
#include "UIRadioButton.h"
#include "UISpinNum.h"
#include "UISpinText.h"
#include "UITrackBar.h"

using namespace luabind;

#pragma optimize("s",on)
void CUIButton::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUIButton, CUIStatic>("CUIButton")
		.def(							constructor<>())
		.def("Init",					(void(CUIButton::*)(float,float,float,float))&CUIButton::Init )
		.def("Init",					(void(CUIButton::*)(LPCSTR,float,float,float,float))&CUIButton::Init )
		.def("SetHighlightColor",		&CUIButton::SetHighlightColor)
		.def("EnableTextHighlighting",	&CUIButton::EnableTextHighlighting)
/*
		.def("SetPushOffsetX",			&CUIButton::SetPushOffsetX)
		.def("SetPushOffsetY",			&CUIButton::SetPushOffsetY)
		.def("GetPushOffsetX",			&CUIButton::GetPushOffsetX)
		.def("GetPushOffsetY",			&CUIButton::GetPushOffsetY)
*/		
		,

		class_<CUI3tButton, CUIButton>("CUI3tButton")
		.def(							constructor<>())
//.		.def("Init",					(void(CUI3tButton::*)(float,float,float,float))CUI3tButton::Init)
//.		.def("Init",					(void(CUI3tButton::*)(LPCSTR,float,float,float,float))CUI3tButton::Init)
//.		.def("InitTexture",				(void(CUI3tButton::*)(LPCSTR))CUI3tButton::InitTexture)
//.		.def("InitTexture",				(void(CUI3tButton::*)(LPCSTR, LPCSTR, LPCSTR, LPCSTR))CUI3tButton::InitTexture)
//.		.def("InitTextureE",			&CUI3tButton::InitTextureEnabled)
//.		.def("InitTextureD",			&CUI3tButton::InitTextureDisabled)
//.		.def("InitTextureT",			&CUI3tButton::InitTextureTouched)
//.		.def("InitTextureH",			&CUI3tButton::InitTextureHighlighted)
//.		.def("SetTextColorD",			&CUI3tButton::SetTextColorD)
//.		.def("SetTextColorT",			&CUI3tButton::SetTextColorT)
//.		.def("SetTextColorH",			&CUI3tButton::SetTextColorH)
//.		.def("SetColor",				(void(CUI3tButton::*)(int, int, int)) &CUI3tButton::SetColor)
		,


		class_<CUICheckButton, CUI3tButton>("CUICheckButton")
		.def(							constructor<>())
		.def("GetCheck",				&CUICheckButton::GetCheck)
		.def("SetCheck",				&CUICheckButton::SetCheck)
		.def("SetDependControl",		&CUICheckButton::SetDependControl),

		class_<CUICustomSpin, CUIWindow>("CUICustomSpin")
		.def("Init",				&CUICustomSpin::Init)
		.def("GetText",				&CUICustomSpin::GetText),

		class_<CUISpinNum, CUICustomSpin>("CUISpinNum")
		.def(							constructor<>()),

		class_<CUISpinFlt, CUICustomSpin>("CUISpinFlt")
		.def(							constructor<>()),

		class_<CUISpinText, CUICustomSpin>("CUISpinText")
		.def(							constructor<>()),

		class_<CUITrackBar, CUIWindow>("CUITrackBar")
		.def(							constructor<>())
		.def("GetCheck",				&CUITrackBar::GetCheck)
		.def("SetCheck",				&CUITrackBar::SetCheck)
	];
}