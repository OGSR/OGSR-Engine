// File:        UI3tButton.cpp
// Description: Button with 3 texutres (for <enabled>, <disabled> and <touched> states)
// Created:     07.12.2004
// Author:      Serhiy 0. Vynnychenk0
// Mail:        narrator@gsc-game.kiev.ua
//
// copyright 2004 GSC Game World
//

#pragma once
#include "UIButton.h"
#include "UI_IB_Static.h"

class CUI3tButton : public CUIButton 
{
	friend class CUIXmlInit;
	using CUIButton::SetTextColor;
public:
					CUI3tButton					();
	virtual			~CUI3tButton				();
	// appearance
	using CUIButton::Init;

	virtual void 	Init						(float x, float y, float width, float height);
	virtual void 	InitTexture					(LPCSTR tex_name);
	virtual void 	InitTexture					(LPCSTR tex_enabled, LPCSTR tex_disabled, LPCSTR tex_touched, LPCSTR tex_highlighted);	

			void 	SetTextColor				(u32 color);
			void 	SetTextColorH				(u32 color);
			void 	SetTextColorD				(u32 color);
			void 	SetTextColorT				(u32 color);
	virtual void 	SetTextureOffset			(float x, float y);	
	virtual void 	SetWidth					(float width);
	virtual void 	SetHeight					(float height);
			void 	InitSoundH					(LPCSTR sound_file);
			void 	InitSoundT					(LPCSTR sound_file);

	virtual void 	OnClick						();
	virtual void 	OnFocusReceive				();
	virtual void	OnFocusLost					();

	// check button
	bool			GetCheck					() {return m_eButtonState == BUTTON_PUSHED;}
	void			SetCheck					(bool ch) {m_eButtonState = ch ? BUTTON_PUSHED : BUTTON_NORMAL;}
	
	// behavior
	virtual void	DrawTexture					();
	virtual void	Update						();
	
	//virtual void Enable(bool bEnable);	
	virtual bool 	OnMouse						(float x, float y, EUIMessages mouse_action);
	virtual bool 	OnMouseDown					(int mouse_btn);
			void 	SetCheckMode				(bool mode) {m_bCheckMode = mode;}


	CUIStatic			m_hint;
	CUI_IB_Static		m_background;
protected:
	bool				m_bCheckMode;
private:	
			void		PlaySoundH					();
			void		PlaySoundT					();

	ref_sound			m_sound_h;
	ref_sound			m_sound_t;	
};