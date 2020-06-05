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

UILoadingScreen::UILoadingScreen()
	: loadingProgress(nullptr), loadingProgressPercent(nullptr), loadingLogo(nullptr),
	loadingStage(nullptr), loadingHeader(nullptr), loadingTipNumber(nullptr), loadingTip(nullptr)
{
	UILoadingScreen::Initialize();
}

void UILoadingScreen::Initialize()
{
	CUIXml uiXml;
	R_ASSERT(uiXml.Init(CONFIG_PATH, UI_PATH, "ui_mm_loading_screen.xml"));

	const auto loadProgressBar = [&]()
	{
		loadingProgress = UIHelper::CreateProgressBar(uiXml, "loading_progress", this);
	};

	const auto loadBackground = [&]
	{
		CUIXmlInit::InitWindow(uiXml, "background", 0, this);
	};

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

void UILoadingScreen::SetStageTip(const char* header, const char* tipNumber, const char* tip)
{
	std::scoped_lock<decltype(loadingLock)> lock(loadingLock);

	loadingHeader->SetText(header);
	loadingTipNumber->SetText(tipNumber);
	loadingTip->SetText(tip);
}

void UILoadingScreen::Show(bool status)
{
	CUIWindow::Show(status);
}

bool UILoadingScreen::IsShown()
{
	return CUIWindow::IsShown();
}
