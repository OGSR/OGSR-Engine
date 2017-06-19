#pragma once

#include "UIScrollView.h"
#include "UIStatsPlayerList.h"

class CUIXml;
class CUIFrameWindow;

class CUIStats : public CUIScrollView {
public:
	using CUIScrollView::Init;

	CUIStats();
	virtual ~CUIStats();
	CUIWindow* Init(CUIXml& xml_doc, LPCSTR path,int team);
};