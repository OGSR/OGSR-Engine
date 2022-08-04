////////////////////////////////////////////////////////////////////////////
//	Module 		: UIHelper.h
//	Created 	: 17.01.2008
//	Author		: Evgeniy Sokolov
//	Description : UI Helper class
////////////////////////////////////////////////////////////////////////////

#pragma once

class CUIXml;
class CUIWindow;
class CUIStatic;
class CUIScrollView;
class CUIProgressBar;
class CUIFrameLineWnd;
class CUIFrameWindow;
class CUI3tButton;
class CUICheckButton;
class CUIDragDropListEx;
class CUIEditBox;

class UIHelper
{
public:
    UIHelper() = default;
    ~UIHelper() = default;

    static CUIWindow* CreateNormalWindow(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical = true);
    static CUIStatic* CreateStatic(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical = true);
    static CUIScrollView* CreateScrollView(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical = true);
    static CUIProgressBar* CreateProgressBar(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical = true);
    static CUIFrameLineWnd* CreateFrameLine(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical = true);
    static CUIFrameWindow* CreateFrameWindow(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical = true);
    static CUI3tButton* Create3tButton(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical = true);
    static CUICheckButton* CreateCheck(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical = true);
    static CUIEditBox* CreateEditBox(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical = true);

    static CUIDragDropListEx* CreateDragDropListEx(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical = true);
};
