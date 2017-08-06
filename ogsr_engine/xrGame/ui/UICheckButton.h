#pragma once

#include "ui3tbutton.h"
#include "UIOptionsItem.h"

class CUICheckButton : public CUI3tButton, public CUIOptionsItem {
public:
	CUICheckButton(void);
	virtual ~CUICheckButton(void);

	virtual void Update();

	// CUIOptionsItem
	virtual void	SetCurrentValue();
	virtual void	SaveValue();
	virtual bool	IsChanged();
	virtual void 	SeveBackUpValue			();
	virtual void 	Undo					();

	virtual void Init(float x, float y, float width, float height);
	virtual void SetTextX(float x) {/*do nothing*/}

	//состояние кнопки
	bool GetCheck()					{return m_eButtonState == BUTTON_PUSHED;}
	void SetCheck(bool ch)			{m_eButtonState = ch ? BUTTON_PUSHED : BUTTON_NORMAL;}

	void SetDependControl(CUIWindow* pWnd);

private:
	bool			b_backup_val;
	void InitTexture();
	CUIWindow* m_pDependControl;
};