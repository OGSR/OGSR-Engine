#include "stdafx.h"
#include "UIStatic.h"
#include "UILines.h"

using namespace luabind;

#pragma optimize("s", on)
void CUIStatic::script_register(lua_State* L)
{
    module(L)[class_<CUIStatic, CUIWindow>("CUIStatic")
                  .def(constructor<>())

                  .def("SetText", (void(CUIStatic::*)(LPCSTR))(&CUIStatic::SetText))
                  .def("SetTextST", (void(CUIStatic::*)(LPCSTR))(&CUIStatic::SetTextST))
                  .def("GetText", &CUIStatic::GetText)
                  .def("IsMultibyteFont", [](CUIStatic* self) -> bool { return self->m_pLines->GetFont()->IsMultibyte(); })

                  .def("SetTextX", &CUIStatic::SetTextX)
                  .def("SetTextY", &CUIStatic::SetTextY)
                  .def("GetTextX", &CUIStatic::GetTextX)
                  .def("GetTextY", &CUIStatic::GetTextY)

                  .def("SetColor", &CUIStatic::SetColor)
                  .def("GetColor", &CUIStatic::GetColor)
                  .def("SetColorA", [](CUIStatic* self, u8 alpha) { self->SetColor(subst_alpha(self->GetColor(), alpha)); })
                  .def("SetTextColor", &CUIStatic::SetTextColor_script)
                  .def("Init", (void(CUIStatic::*)(float, float, float, float)) & CUIStatic::Init)
                  .def("Init", (void(CUIStatic::*)(LPCSTR, float, float, float, float)) & CUIStatic::Init)
                  .def("InitTexture", &CUIStatic::InitTexture)
                  .def("SetTextureOffset", &CUIStatic::SetTextureOffset)

                  .def("GetOriginalRect", &CUIStatic::GetOriginalRect)
                  .def("SetOriginalRect", (void(CUIStatic::*)(float, float, float, float)) & CUIStatic::SetOriginalRect)
                  .def("ResetOriginalRect", &CUIStatic::ResetOriginalRect)
                  .def("SetNoShaderCache", &CUIStatic::SetNoShaderCache)
                  .def("SetStretchTexture", &CUIStatic::SetStretchTexture)
                  .def("GetStretchTexture", &CUIStatic::GetStretchTexture)

                  .def("SetTextAlign", &CUIStatic::SetTextAlign_script)
                  .def("GetTextAlign", &CUIStatic::GetTextAlign_script)

                  .def("SetHeading", &CUIStatic::SetHeading)
                  .def("GetHeading", &CUIStatic::GetHeading)
                  .def("EnableHeading", &CUIStatic::EnableHeading)

                  .def("ClipperOn", &CUIStatic::ClipperOn)
                  .def("ClipperOff", (void(CUIStatic::*)(void)) & CUIStatic::ClipperOff)
                  .def("GetClipperState", &CUIStatic::GetClipperState)
                  .def("SetClipRect", &CUIStatic::SetClipRect)
                  .def("GetClipRect", &CUIStatic::GetClipperRect)

                  .def("SetTextComplexMode", &CUIStatic::SetTextComplexMode)
                  .def("AdjustWidthToText", &CUIStatic::AdjustWidthToText)
                  .def("AdjustHeightToText", &CUIStatic::AdjustHeightToText)
                  .def("SetVTextAlign", &CUIStatic::SetVTextAlignment)
                  .def("SetTextPos", &CUIStatic::SetTextPos)];
}
