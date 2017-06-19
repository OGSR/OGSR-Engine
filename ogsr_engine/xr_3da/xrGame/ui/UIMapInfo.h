#pragma once

#include "UIWindow.h"
#include "../script_export_space.h"

class CUIScrollView;

class CUIMapInfo : public CUIWindow {
public:
	CUIMapInfo();
	~CUIMapInfo();
    virtual void Init(float x, float y, float width, float height);
			void InitMap(const char* map_name);
	const char*	 GetLargeDesc();
	DECLARE_SCRIPT_REGISTER_FUNCTION
protected:
	CUIScrollView*	m_view;
	shared_str		m_large_desc;
};

add_to_type_list(CUIMapInfo)
#undef script_type_list
#define script_type_list save_type_list(CUIMapInfo)
