#pragma once


#include "uiframewindow.h"
#include "uilistbox.h"

#include "../script_export_space.h"

class CUIPropertiesBox: public CUIFrameWindow
{
private:
	typedef CUIFrameWindow inherited; 
public:
						CUIPropertiesBox					();
	virtual				~CUIPropertiesBox					();

	virtual void		Init								(float x, float y, float width, float height);



	virtual void		SendMessage							(CUIWindow *pWnd, s16 msg, void *pData);
	virtual bool		OnMouse								(float x, float y, EUIMessages mouse_action);
	virtual bool		OnKeyboard							(int dik, EUIMessages keyboard_action);

	bool				AddItem								(const char*  str, void* pData = NULL, u32 tag_value = 0);
	bool				AddItem_script						(const char*  str){return AddItem(str);};
	u32					GetItemsCount						() {return m_UIListWnd.GetSize();};
	void				RemoveItemByTAG						(u32 tag_value);
	void				RemoveAll							();

	virtual void		Show								(const Frect& parent_rect, const Fvector2& point);
	virtual void		Hide								();

	virtual void		Update								();
	virtual void		Draw								();

	CUIListBoxItem*		GetClickedItem						();

	void				AutoUpdateSize						();

protected:
	CUIListBox			m_UIListWnd;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIPropertiesBox)
#undef script_type_list
#define script_type_list save_type_list(CUIPropertiesBox)
