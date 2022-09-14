////////////////////////////////////////////////////////////////////////////
//  Created     : 19.06.2018
//  Authors     : Xottab_DUTY (OpenXRay project)
//                FozeSt
//                Unfainthful
//
//  Copyright (C) GSC Game World - 2018
////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "UILoadingScreen.h"
#include "../../xr_3da/GameFont.h"
#include "UIHelper.h"
#include "xrUIXmlParser.h"
#include "UIXmlInit.h"
#include "string_table.h"

UILoadingScreen::UILoadingScreen()
    : loadingProgress(nullptr), loadingProgressPercent(nullptr), loadingLogo(nullptr), loadingStage(nullptr), loadingHeader(nullptr), loadingTipNumber(nullptr), loadingTip(nullptr), maxTip(100)
{
    UILoadingScreen::Initialize();
}

void UILoadingScreen::Initialize()
{
    CUIXml uiXml;
    R_ASSERT(uiXml.Init(CONFIG_PATH, UI_PATH, "ui_mm_loading_screen.xml"));

    const auto loadProgressBar = [&]() { loadingProgress = UIHelper::CreateProgressBar(uiXml, "loading_progress", this); };

    const auto loadBackground = [&] { UIHelper::CreateStatic(uiXml, "background", this, false); };

    const auto node = uiXml.NavigateToNodeWithAttribute("loading_progress", "under_background", "0");
    if (node)
    {
        loadBackground();
        loadProgressBar();
    }
    else
    {
        loadProgressBar();
        loadBackground();
    }

    loadingLogo = UIHelper::CreateStatic(uiXml, "loading_logo", this, false);
    loadingProgressPercent = UIHelper::CreateStatic(uiXml, "loading_progress_percent", this, false);
    loadingStage = UIHelper::CreateStatic(uiXml, "loading_stage", this, false);

    loadingHeader = UIHelper::CreateStatic(uiXml, "loading_header", this, false);

    loadingTipNumber = UIHelper::CreateStatic(uiXml, "loading_tip_number", this, false);
    loadingTip = UIHelper::CreateStatic(uiXml, "loading_tip", this, false);

    maxTip = uiXml.ReadAttribInt("loading_tip", 0, "number_of_tips", maxTip);
}

void UILoadingScreen::Update(const int stagesCompleted, const int stagesTotal)
{
    std::scoped_lock<decltype(loadingLock)> lock(loadingLock);

    const float progress = float(stagesCompleted) / stagesTotal * loadingProgress->GetRange_max();

    if (loadingProgress->GetProgressPos() < progress)
        loadingProgress->SetProgressPos(progress);

    if (loadingProgressPercent)
    {
        char buf[5];
        xr_sprintf(buf, "%.0f%%", loadingProgress->GetProgressPos());
        loadingProgressPercent->SetText(buf);
    }

    CUIWindow::Update();
    Draw();
}

void UILoadingScreen::ForceDrop()
{
    std::scoped_lock<decltype(loadingLock)> lock(loadingLock);

    const float prev = loadingProgress->m_inertion;
    const float maximal = loadingProgress->GetRange_max();

    loadingProgress->m_inertion = 0.0f;
    loadingProgress->SetProgressPos(loadingProgress->GetRange_min());

    for (int i = 0; i < int(maximal); ++i)
    {
        loadingProgress->Update();
    }

    loadingProgress->m_inertion = prev;
}

void UILoadingScreen::ForceFinish()
{
    std::scoped_lock<decltype(loadingLock)> lock(loadingLock);

    const float prev = loadingProgress->m_inertion;
    const float maximal = loadingProgress->GetRange_max();

    loadingProgress->m_inertion = 0.0f;
    loadingProgress->SetProgressPos(maximal);

    for (int i = 0; i < int(maximal); ++i)
    {
        loadingProgress->Update();
    }

    loadingProgress->m_inertion = prev;
}

void UILoadingScreen::SetLevelLogo(const char* name)
{
    std::scoped_lock<decltype(loadingLock)> lock(loadingLock);

    if (loadingLogo)
        loadingLogo->InitTexture(name);
}

void UILoadingScreen::SetStageTitle(const char* title)
{
    std::scoped_lock<decltype(loadingLock)> lock(loadingLock);

    loadingStage->SetText(title);
}

void UILoadingScreen::SetStageTip()
{
    std::scoped_lock<decltype(loadingLock)> lock(loadingLock);

    u8 tip_num = Random.randI(1, maxTip);

    string512 buff;

    if (loadingHeader)
    {
        loadingHeader->SetText(CStringTable().translate("ls_header").c_str());
    }
    if (loadingTipNumber)
    {
        xr_sprintf(buff, "%s%d:", CStringTable().translate("ls_tip_number").c_str(), tip_num);
        shared_str tipNumber = buff;
        loadingTipNumber->SetText(tipNumber.c_str());
    }
    if (loadingTip)
    {
        xr_sprintf(buff, "ls_tip_%d", tip_num);
        loadingTip->SetText(CStringTable().translate(buff).c_str());
    }
}

void UILoadingScreen::Show(bool status) { CUIWindow::Show(status); }

bool UILoadingScreen::IsShown() { return CUIWindow::IsShown(); }
