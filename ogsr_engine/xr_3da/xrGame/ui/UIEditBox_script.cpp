#include "pch_script.h"
#include "UIEditBox.h"
#include "UIEditBoxEx.h"

using namespace luabind;

#pragma optimize("s",on)
void CUIEditBox::script_register(lua_State *L)
{
	module(L)
	[
		class_<CUICustomEdit, CUIWindow>("CUICustomEdit")
		.def("SetText",				&CUICustomEdit::SetText)
		.def("GetText",				&CUICustomEdit::GetText)
		.def("SetTextColor",		&CUICustomEdit::SetTextColor)
		.def("GetTextColor",		&CUICustomEdit::GetTextColor)
		.def("SetFont",				&CUICustomEdit::SetFont)
		.def("GetFont",				&CUICustomEdit::GetFont)
		.def("SetTextAlignment",	&CUICustomEdit::SetTextAlignment)
		.def("GetTextAlignment",	&CUICustomEdit::GetTextAlignment)
		.def("SetTextPosX",			&CUICustomEdit::SetTextPosX)
		.def("SetTextPosY",			&CUICustomEdit::SetTextPosY)
		.def("SetNumbersOnly",		&CUICustomEdit::SetNumbersOnly),

		class_<CUIEditBox, CUICustomEdit>("CUIEditBox")
		.def(						constructor<>())
		.def("InitTexture",			&CUIEditBox::InitTexture),

		class_<CUIEditBoxEx, CUICustomEdit>("CUIEditBoxEx")
		.def(						constructor<>())
		.def("InitTexture",			&CUIEditBoxEx::InitTexture)
	];
}