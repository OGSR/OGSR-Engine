#pragma once

#include "UIStatic.h"
#include "../script_export_space.h"


class CUIButton : public CUIStatic
{
private:
	typedef			CUIStatic				inherited;
public:
					CUIButton				();
	virtual			~CUIButton				();

	virtual bool	OnMouse					(float x, float y, EUIMessages mouse_action);
	virtual void	OnClick					();

	//прорисовка окна
	virtual void	DrawTexture				();
	virtual void	DrawText				();
	virtual void	DrawHighlightedText		();

	virtual void	Update					();
	virtual void	Enable					(bool status);
	virtual bool	OnKeyboard				(int dik, EUIMessages keyboard_action);
	virtual void	OnFocusLost				();

	//режимы в которых можно нажимать кнопку
	typedef enum{NORMAL_PRESS, //кнопка нажимается при 
							   //нажатии и отпускании на ней мыши
				 DOWN_PRESS    //сразу при нажатии
			} E_PRESS_MODE;


	//заново подготовить состояние
    virtual void	Reset					();


	//подсвечен ли текст на кнопке
	// принудительная подсветка
	virtual void	HighlightItem			(bool bHighlight)			{m_bCursorOverWindow = bHighlight; }

	//состояния в которых находится кнопка
	typedef enum{BUTTON_NORMAL, //кнопка никак не затрагивается
		BUTTON_PUSHED, //в нажатом сотоянии
		BUTTON_UP      //при удерживаемой кнопки мыши 
	} E_BUTTON_STATE;

	// Установка состояния кнопки: утоплена, не утоплена
	void				SetButtonMode			(E_BUTTON_STATE eBtnState)	{ m_eButtonState = eBtnState; }
	E_BUTTON_STATE		GetButtonsState			()							{ return m_eButtonState;}

	// Поведение кнопки как переключателя реализовано пока только в режиме NORMAL_PRESS
	void				SetButtonAsSwitch		(bool bAsSwitch)			{ m_bIsSwitch = bAsSwitch; }

	// Работа с акселератором
	// Код акселератора берется из файла dinput.h, из DirectX SDK.
	// Например: кнопка A - код 0x1E(DIK_A)
	void				SetAccelerator			(int iAccel, int idx)	{VERIFY(idx==0||idx==1); m_uAccelerator[idx] = iAccel; }
	const int			GetAccelerator			(int idx) const			{VERIFY(idx==0||idx==1); return m_uAccelerator[idx]; }
	IC bool				IsAccelerator			(int iAccel) const		{return (m_uAccelerator[0]==iAccel)||(m_uAccelerator[1]==iAccel) ;}

	void				SetPressMode			(E_PRESS_MODE ePressMode)	{m_ePressMode = ePressMode;}
	E_PRESS_MODE		GetPressMode			()							{return m_ePressMode;}
	
	void				SetPushOffset			(const Fvector2& offset)	{m_PushOffset = offset;}
	Fvector2			GetPushOffset			()							{return m_PushOffset;}
	void				SetShadowOffset			(const Fvector2& offset) { m_ShadowOffset = offset; }
	shared_str			m_hint_text;
protected:
	
	E_BUTTON_STATE		m_eButtonState;
	bool				m_bIsSwitch;
	bool				m_bButtonClicked;
	E_PRESS_MODE		m_ePressMode;
	Fvector2			m_PushOffset;
	int					m_uAccelerator[2];
	Fvector2			m_ShadowOffset;

	DECLARE_SCRIPT_REGISTER_FUNCTION
};


add_to_type_list(CUIButton)
#undef script_type_list
#define script_type_list save_type_list(CUIButton)
