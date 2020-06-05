////////////////////////////////////////////////////////////////////////////
//  Created     : 19.06.2018
//  Authors     : Xottab_DUTY (OpenXRay project)
//                FozeSt
//                Unfainthful
//
//  Copyright (C) GSC Game World - 2018
////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../xr_3da/ILoadingScreen.h"
#include "UIStatic.h"
#include "UIWindow.h"
#include "UIProgressBar.h"

class UILoadingScreen : public ILoadingScreen, public CUIWindow
{
	std::recursive_mutex loadingLock;

	CUIProgressBar* loadingProgress;
	CUIStatic* loadingProgressPercent;
	CUIStatic* loadingLogo;

	CUIStatic* loadingStage;
	CUIStatic* loadingHeader;
	CUIStatic* loadingTipNumber;
	CUIStatic* loadingTip;

public:
	UILoadingScreen();

	void Initialize() override;

	void Show(bool status) override;
	bool IsShown() override;

	void Update(const int stagesCompleted, const int stagesTotal) override;
	void ForceDrop() override;
	void ForceFinish() override;

	void SetLevelLogo(const char* name) override;
	void SetStageTitle(const char* title) override;
	void SetStageTip(const char* header, const char* tipNumber, const char* tip) override;
};
