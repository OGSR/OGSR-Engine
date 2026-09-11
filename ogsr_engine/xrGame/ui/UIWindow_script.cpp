#include "stdafx.h"
#include "UIWindow.h"
#include "UIFrameWindow.h"
#include "UIFrameLineWnd.h"
#include "UIDialogWnd.h"
#include "UIDialogHolder.h"
#include "../GamePersistent.h"
#include "UILabel.h"
#include "UIMMShniaga.h"
#include "UITextureMaster.h"
#include "UIScrollView.h"

const Fvector2* get_wnd_pos(CUIWindow* w) { return &w->GetWndPos(); }
using namespace luabind;

void CUIWindow::script_register(lua_State* L)
{
    module(L)[
        def("GetARGB", &color_argb),
        def("GetFontSmall", [] { return UI().Font().pFontStat; }),
        def("GetFontMedium", [] { return UI().Font().pFontMedium; }),
        def("GetFontDI", [] { return UI().Font().pFontDI; }),
        def("GetFontGraffiti19Russian", [] { return UI().Font().pFontGraffiti19Russian; }),
        def("GetFontGraffiti22Russian", [] { return UI().Font().pFontGraffiti22Russian; }),
        def("GetFontLetterica16Russian",[] { return UI().Font().pFontLetterica16Russian; }),
        def("GetFontLetterica18Russian", [] { return UI().Font().pFontLetterica18Russian; }),
        def("GetFontGraffiti32Russian", [] { return UI().Font().pFontGraffiti32Russian; }),
        def("GetFontGraffiti50Russian", [] { return UI().Font().pFontGraffiti50Russian; }),
		def("GetFontArial14", [] {return UI().Font().pFontArial14; }),
        def("GetFontLetterica25", [] { return UI().Font().pFontLetterica25; }),
        def("GetFontCustom", [](const char* sect) { return UI().Font().InitializeCustomFont(sect); }),

           class_<CUIWindow>("CUIWindow")
               .def(constructor<>())
               .def("AttachChild", &CUIWindow::AttachChild, adopt<2>())
               .def("DetachChild", &CUIWindow::DetachChild)
               .def("SetAutoDelete", &CUIWindow::SetAutoDelete)
               .def("IsAutoDelete", &CUIWindow::IsAutoDelete)

               .def("SetWndRect", (void (CUIWindow::*)(Frect))&CUIWindow::SetWndRect_script)
               .def("SetWndPos", (void (CUIWindow::*)(Fvector2))&CUIWindow::SetWndPos_script)
               .def("SetWndSize", (void (CUIWindow::*)(Fvector2))&CUIWindow::SetWndSize_script)
               .def("GetWndPos", &get_wnd_pos)
               .def("GetWidth", &CUIWindow::GetWidth)
               .def("SetWidth", &CUIWindow::SetWidth)
               .def("GetHeight", &CUIWindow::GetHeight)
               .def("SetHeight", &CUIWindow::SetHeight)

               .def("Enable", &CUIWindow::Enable)
               .def("IsEnabled", &CUIWindow::IsEnabled)
               .def("Show", &CUIWindow::Show)
               .def("IsShown", &CUIWindow::IsShown)
               .def("SetFont", &CUIWindow::SetFont)
               .def("GetFont", &CUIWindow::GetFont)

               .def("WindowName", &CUIWindow::WindowName_script)
               .def("SetWindowName", &CUIWindow::SetWindowName)
               .def("SetPPMode", &CUIWindow::SetPPMode)
               .def("ResetPPMode", &CUIWindow::ResetPPMode)

               .def("GetMousePosX", [](CUIWindow* self) { return self->cursor_pos.x; })
               .def("GetMousePosY", [](CUIWindow* self) { return self->cursor_pos.y; })
           ,
           class_<CDialogHolder>("CDialogHolder").def("AddDialogToRender", &CDialogHolder::AddDialogToRender).def("RemoveDialogToRender", &CDialogHolder::RemoveDialogToRender),

           class_<CUIDialogWnd, CUIWindow>("CUIDialogWnd")
               .def("ShowDialog", &CUIDialogWnd::ShowDialog)
               .def("HideDialog", &CUIDialogWnd::HideDialog)
               .def("GetHolder", &CUIDialogWnd::GetHolder),

           class_<CUIFrameWindow, CUIWindow>("CUIFrameWindow")
               .def(constructor<>())
               .def("SetWidth", &CUIFrameWindow::SetWidth)
               .def("SetHeight", &CUIFrameWindow::SetHeight)
               .def("SetColor", &CUIFrameWindow::SetTextureColor),

           class_<CUIFrameLineWnd, CUIWindow>("CUIFrameLineWnd")
               .def(constructor<>())
               .def("SetWidth", &CUIFrameLineWnd::SetWidth)
               .def("SetHeight", &CUIFrameLineWnd::SetHeight)
               .def("SetColor", &CUIFrameLineWnd::SetTextureColor),

           class_<CUIMMShniaga, CUIWindow>("CUIMMShniaga")
               .enum_("enum_page_id")[value("epi_main", CUIMMShniaga::epi_main), value("epi_new_game", CUIMMShniaga::epi_new_game)]
               .def("SetVisibleMagnifier", &CUIMMShniaga::SetVisibleMagnifier)
               .def("SetPage", &CUIMMShniaga::SetPage)
               .def("ShowPage", &CUIMMShniaga::ShowPage),

           class_<CUIScrollView, CUIWindow>("CUIScrollView")
               .def(constructor<>())
               .def("AddWindow", &CUIScrollView::AddWindow)
               .def("RemoveWindow", &CUIScrollView::RemoveWindow)
               .def("Clear", &CUIScrollView::Clear)
               .def("ScrollToBegin", &CUIScrollView::ScrollToBegin)
               .def("ScrollToEnd", &CUIScrollView::ScrollToEnd)
               .def("GetMinScrollPos", &CUIScrollView::GetMinScrollPos)
               .def("GetMaxScrollPos", &CUIScrollView::GetMaxScrollPos)
               .def("GetCurrentScrollPos", &CUIScrollView::GetCurrentScrollPos)
               .def("SetScrollPos", &CUIScrollView::SetScrollPos)
               .def("ForceUpdate", &CUIScrollView::ForceUpdate),

           class_<enum_exporter<EUIMessages>>("ui_events")
               .enum_("events")[
                   // CUIWindow
                   value("WINDOW_LBUTTON_DOWN", int(WINDOW_LBUTTON_DOWN)), value("WINDOW_RBUTTON_DOWN", int(WINDOW_RBUTTON_DOWN)),
                   value("WINDOW_LBUTTON_UP", int(WINDOW_LBUTTON_UP)), value("WINDOW_RBUTTON_UP", int(WINDOW_RBUTTON_UP)), value("WINDOW_MOUSE_MOVE", int(WINDOW_MOUSE_MOVE)),
                   value("WINDOW_LBUTTON_DB_CLICK", int(WINDOW_LBUTTON_DB_CLICK)), value("WINDOW_KEY_PRESSED", int(WINDOW_KEY_PRESSED)),
                   value("WINDOW_KEY_RELEASED", int(WINDOW_KEY_RELEASED)), value("WINDOW_KEYBOARD_CAPTURE_LOST", int(WINDOW_KEYBOARD_CAPTURE_LOST)),
                   value("STATIC_FOCUS_RECEIVED", int(STATIC_FOCUS_RECEIVED)), value("STATIC_FOCUS_LOST", int(STATIC_FOCUS_LOST)),

                   // CUIButton
                   value("BUTTON_CLICKED", int(BUTTON_CLICKED)), value("BUTTON_DOWN", int(BUTTON_DOWN)),

                   // CUITabControl
                   value("TAB_CHANGED", int(TAB_CHANGED)),

                   // CUICheckButton
                   value("CHECK_BUTTON_SET", int(CHECK_BUTTON_SET)), value("CHECK_BUTTON_RESET", int(CHECK_BUTTON_RESET)),

                   // CUIRadioButton
                   value("RADIOBUTTON_SET", int(RADIOBUTTON_SET)),

                   // CUIScrollBox
                   value("SCROLLBOX_MOVE", int(SCROLLBOX_MOVE)),

                   // CUIScrollBar
                   value("SCROLLBAR_VSCROLL", int(SCROLLBAR_VSCROLL)), value("SCROLLBAR_HSCROLL", int(SCROLLBAR_HSCROLL)),

                   // CUIListWnd
                   value("LIST_ITEM_CLICKED", int(LIST_ITEM_CLICKED)), value("LIST_ITEM_SELECT", int(LIST_ITEM_SELECT)),

                   // UIPropertiesBox
                   value("PROPERTY_CLICKED", int(PROPERTY_CLICKED)),

                   // CUIMessageBox
                   value("MESSAGE_BOX_OK_CLICKED", int(MESSAGE_BOX_OK_CLICKED)), value("MESSAGE_BOX_YES_CLICKED", int(MESSAGE_BOX_YES_CLICKED)),
                   value("MESSAGE_BOX_NO_CLICKED", int(MESSAGE_BOX_NO_CLICKED)), value("MESSAGE_BOX_CANCEL_CLICKED", int(MESSAGE_BOX_CANCEL_CLICKED)),
                   value("MESSAGE_BOX_COPY_CLICKED", int(MESSAGE_BOX_COPY_CLICKED)), value("MESSAGE_BOX_QUIT_GAME_CLICKED", int(MESSAGE_BOX_QUIT_GAME_CLICKED)),
                   value("MESSAGE_BOX_QUIT_WIN_CLICKED", int(MESSAGE_BOX_QUIT_WIN_CLICKED)),

                   value("EDIT_TEXT_COMMIT", int(EDIT_TEXT_COMMIT)),
                   // CMainMenu
                   value("MAIN_MENU_RELOADED", int(MAIN_MENU_RELOADED))]];
}
