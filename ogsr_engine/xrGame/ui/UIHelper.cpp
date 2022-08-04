////////////////////////////////////////////////////////////////////////////
//	Module 		: UIHelper.cpp
//	Created 	: 17.01.2008
//	Author		: Evgeniy Sokolov
//	Description : UI Helper class implementation
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UIHelper.h"
#include "UIXmlInit.h"

#include "UIProgressBar.h"
#include "UIFrameLineWnd.h"
#include "UIFrameWindow.h"
#include "UI3tButton.h"
#include "UICheckButton.h"
#include "UIScrollView.h"
#include "UIEditBox.h"
#include "UIDragDropListEx.h"

CUIWindow* UIHelper::CreateNormalWindow(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical /*= true*/)
{
    // If it's not critical element, then don't crash if it doesn't exist
    if (!critical && !xml.NavigateToNode(ui_path, 0))
        return nullptr;

    auto ui = xr_new<CUIWindow>();
    if (parent)
    {
        parent->AttachChild(ui);
        ui->SetAutoDelete(true);
    }
    CUIXmlInit::InitWindow(xml, ui_path, 0, ui);
    return ui;
}

CUIStatic* UIHelper::CreateStatic(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical)
{
    // If it's not critical element, then don't crash if it doesn't exist
    if (!critical && !xml.NavigateToNode(ui_path, 0))
        return nullptr;

    auto ui = xr_new<CUIStatic>();
    if (parent)
    {
        parent->AttachChild(ui);
        ui->SetAutoDelete(true);
    }
    CUIXmlInit::InitStatic(xml, ui_path, 0, ui);
    return ui;
}

CUIScrollView* UIHelper::CreateScrollView(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical /*= true*/)
{
    // If it's not critical element, then don't crash if it doesn't exist
    if (!critical && !xml.NavigateToNode(ui_path, 0))
        return nullptr;

    auto ui = xr_new<CUIScrollView>();
    if (parent)
    {
        parent->AttachChild(ui);
        ui->SetAutoDelete(true);
    }
    CUIXmlInit::InitScrollView(xml, ui_path, 0, ui);
    return ui;
}

CUIEditBox* UIHelper::CreateEditBox(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical)
{
    // If it's not critical element, then don't crash if it doesn't exist
    if (!critical && !xml.NavigateToNode(ui_path, 0))
        return nullptr;

    auto ui = xr_new<CUIEditBox>();
    if (parent)
    {
        parent->AttachChild(ui);
        ui->SetAutoDelete(true);
    }
    CUIXmlInit::InitEditBox(xml, ui_path, 0, ui);
    return ui;
}

CUIProgressBar* UIHelper::CreateProgressBar(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical)
{
    // If it's not critical element, then don't crash if it doesn't exist
    if (!critical && !xml.NavigateToNode(ui_path, 0))
        return nullptr;

    auto ui = xr_new<CUIProgressBar>();
    parent->AttachChild(ui);
    ui->SetAutoDelete(true);
    CUIXmlInit::InitProgressBar(xml, ui_path, 0, ui);
    return ui;
}

CUIFrameLineWnd* UIHelper::CreateFrameLine(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical)
{
    // If it's not critical element, then don't crash if it doesn't exist
    if (!critical && !xml.NavigateToNode(ui_path, 0))
        return nullptr;

    auto ui = xr_new<CUIFrameLineWnd>();
    if (parent)
    {
        parent->AttachChild(ui);
        ui->SetAutoDelete(true);
    }
    CUIXmlInit::InitFrameLine(xml, ui_path, 0, ui);
    return ui;
}

CUIFrameWindow* UIHelper::CreateFrameWindow(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical)
{
    // If it's not critical element, then don't crash if it doesn't exist
    if (!critical && !xml.NavigateToNode(ui_path, 0))
        return nullptr;

    auto ui = xr_new<CUIFrameWindow>();
    if (parent)
    {
        parent->AttachChild(ui);
        ui->SetAutoDelete(true);
    }
    CUIXmlInit::InitFrameWindow(xml, ui_path, 0, ui);
    return ui;
}

CUI3tButton* UIHelper::Create3tButton(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical)
{
    // If it's not critical element, then don't crash if it doesn't exist
    if (!critical && !xml.NavigateToNode(ui_path, 0))
        return nullptr;

    auto ui = xr_new<CUI3tButton>();
    parent->AttachChild(ui);
    ui->SetAutoDelete(true);
    CUIXmlInit::Init3tButton(xml, ui_path, 0, ui);
    return ui;
}

CUICheckButton* UIHelper::CreateCheck(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical)
{
    // If it's not critical element, then don't crash if it doesn't exist
    if (!critical && !xml.NavigateToNode(ui_path, 0))
        return nullptr;

    auto ui = xr_new<CUICheckButton>();
    parent->AttachChild(ui);
    ui->SetAutoDelete(true);
    CUIXmlInit::InitCheck(xml, ui_path, 0, ui);
    return ui;
}

CUIDragDropListEx* UIHelper::CreateDragDropListEx(CUIXml& xml, LPCSTR ui_path, CUIWindow* parent, bool critical)
{
    // If it's not critical element, then don't crash if it doesn't exist
    if (!critical && !xml.NavigateToNode(ui_path, 0))
        return nullptr;

    auto ui = xr_new<CUIDragDropListEx>();
    parent->AttachChild(ui);
    ui->SetAutoDelete(true);
    CUIXmlInit::InitDragDropListEx(xml, ui_path, 0, ui);
    return ui;
}
