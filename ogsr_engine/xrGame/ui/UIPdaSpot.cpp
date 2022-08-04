#include "stdafx.h"
#include "UIPdaSpot.h"
#include <dinput.h>
#include "Level.h"
#include "map_manager.h"
#include "map_location.h"
#include "UIEditBoxEx.h"
#include "UIStatic.h"
#include "UIXmlInit.h"
#include "UI3tButton.h"
#include "string_table.h"

CUIPdaSpot::CUIPdaSpot()
{
    m_mainWnd = false;
    m_levelName = nullptr;
    m_position = Fvector();

    m_spotID = u16(-1);
    m_spotType = READ_IF_EXISTS(pSettings, r_string, "user_spots", "spot_type", "red_location");

    InitControls();
}

void CUIPdaSpot::Init(u16 spot_id, LPCSTR level_name, Fvector pos, bool main_wnd)
{
    m_mainWnd = main_wnd;
    m_levelName = level_name;
    m_position = pos;
    m_spotID = m_mainWnd ? u16(-1) : spot_id;

    if (!m_mainWnd)
    {
        CMapLocation* ml = Level().MapManager().GetMapLocation(m_spotType, m_spotID);
        if (!ml)
            return;
        m_editBox->SetText(ml->GetHint());
        ml->HighlightSpot(true, Fcolor().set(255.f, 36.f, 0.f, 255.f));
    }

    m_editBox->CaptureFocus(true);
}

void CUIPdaSpot::InitControls()
{
    this->SetWndRect(Frect().set(0.0f, 0.0f, 1024.f, 768.f));

    CUIXml uiXml;
    bool xml_result = uiXml.Init(CONFIG_PATH, UI_PATH, "pda_spot.xml");
    R_ASSERT3(xml_result, "xml file not found", "pda_spot.xml");

    CUIXmlInit xml_init;

    m_background = xr_new<CUIStatic>();
    AttachChild(m_background);
    m_background->SetAutoDelete(true);
    xml_init.InitStatic(uiXml, "background", 0, m_background);

    m_editBox = xr_new<CUIEditBoxEx>();
    AttachChild(m_editBox);
    m_editBox->SetAutoDelete(true);
    xml_init.InitEditBoxEx(uiXml, "spot_name_edit", 0, m_editBox);
    m_editBox->CaptureFocus(true);

    m_btn_ok = xr_new<CUI3tButton>();
    AttachChild(m_btn_ok);
    m_btn_ok->SetAutoDelete(true);
    xml_init.Init3tButton(uiXml, "btn_apply", 0, m_btn_ok);
    m_btn_ok->SetWindowName("btn_apply");
    Register(m_btn_ok);
    AddCallback("btn_apply", BUTTON_CLICKED, fastdelegate::MakeDelegate(this, &CUIPdaSpot::OnApply));

    m_btn_cancel = xr_new<CUI3tButton>();
    AttachChild(m_btn_cancel);
    m_btn_cancel->SetAutoDelete(true);
    xml_init.Init3tButton(uiXml, "btn_cancel", 0, m_btn_cancel);
    m_btn_cancel->SetWindowName("btn_cancel");
    Register(m_btn_cancel);
    AddCallback("btn_cancel", BUTTON_CLICKED, fastdelegate::MakeDelegate(this, &CUIPdaSpot::OnExit));
}

void CUIPdaSpot::OnAdd(CUIWindow* ui, void* d)
{
    // MsgDbg("--[%s] adding user location: type: [%s], lname: [%s], pos: [%.5f, %.5f, %.5f]. Text: '%s'", __FUNCTION__, m_spotType.c_str(), m_levelName, m_position.x,
    // m_position.y, m_position.z, m_editBox->GetText());

    CMapLocation* ml = Level().MapManager().AddUserLocation(m_spotType, m_levelName, m_position);
    ml->SetHint(m_editBox->GetText());
    ml->SetSerializable(true);

    OnExit(ui, d);
}

void CUIPdaSpot::OnApply(CUIWindow* ui, void* d)
{
    if (m_mainWnd)
    {
        OnAdd(ui, d);
        return;
    }

    CMapLocation* ml = Level().MapManager().GetMapLocation(m_spotType, m_spotID);
    if (!ml)
        return;

    if (m_editBox->GetText() != ml->GetHint())
        ml->SetHint(m_editBox->GetText());

    OnExit(ui, d);
}

void CUIPdaSpot::OnExit(CUIWindow* w, void* d) { Exit(); }

void CUIPdaSpot::Exit()
{
    if (!m_mainWnd)
    {
        CMapLocation* ml = Level().MapManager().GetMapLocation(m_spotType, m_spotID);
        if (!ml)
            return;
        ml->HighlightSpot(false, Fcolor().set(0.f, 0.f, 0.f, 0.f));
    }

    m_mainWnd = false;
    m_levelName = nullptr;
    m_position = Fvector();
    m_spotID = u16(-1);
    m_editBox->SetText("");

    this->Hide();
}

bool CUIPdaSpot::OnKeyboard(int dik, EUIMessages keyboard_action)
{
    if (base_class::OnKeyboard(dik, keyboard_action))
        return true;

    switch (dik)
    {
    case DIK_RETURN:
    case DIK_NUMPADENTER: {
        if (IsShown())
        {
            OnApply(nullptr, nullptr);
            return true;
        }
    }
    break;
    case DIK_ESCAPE: {
        if (IsShown())
        {
            Exit();
            return true;
        }
    }
    break;
    }

    return false;
}

void CUIPdaSpot::SendMessage(CUIWindow* pWnd, s16 msg, void* pData) { CUIWndCallback::OnEvent(pWnd, msg, pData); }
