#pragma once

#include "UIWindow.h"

class CUIAnimatedStatic;
class CUIStatic;
class CUIXml;
class CUIScrollView;
class CMMSound;

class CUIMMShniaga :	public CUIWindow,
						public CDeviceResetNotifier
{
public:
	using CUIWindow::Init;

						CUIMMShniaga			();
	virtual				~CUIMMShniaga			();

			void 		Init					(CUIXml& xml_doc, LPCSTR path);
	virtual void 		Update					();
	virtual void 		Draw					();

	virtual bool 		OnMouse					(float x, float y, EUIMessages mouse_action);
	virtual bool 		OnKeyboard				(int dik, EUIMessages keyboard_action);
	virtual void 		SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = 0);
			void 		SetVisibleMagnifier		(bool f);
	virtual void		OnDeviceReset			();
protected:
	typedef enum {
		E_Begin = 0,
		E_Update,
		E_Finilize,
		E_Stop
	} EVENT;

	void 				SelectBtn				(int btn);
	void 				SelectBtn				(CUIWindow* btn);
	int	 				BtnCount				();
	void 				OnBtnClick				();

	void 				ProcessEvent			(EVENT ev);

	bool 				IsButton				(CUIWindow* st);
	void 				CreateList				(xr_vector<CUIStatic*>& lst, CUIXml& xml_doc, LPCSTR path);
	void 				ShowMain				();
	void 				ShowNewGame				();
	float				pos						(float x1, float x2, u32 t);

    CUIStatic*			m_shniaga;
	CUIStatic*			m_magnifier;
	CUIStatic*			m_anims[2];
	CUIStatic*			m_gratings[2];
	CUIScrollView*		m_view;

    u32					m_start_time;
	u32					m_run_time;
    float				m_origin;
	float				m_destination;
	float				m_mag_pos;
	float				m_offset;

	xr_vector<CUIStatic*>	m_buttons;
	xr_vector<CUIStatic*>	m_buttons_new;
	int						m_selected_btn;
	int						m_page;
    CUIWindow*				m_selected;
	CMMSound*				m_sound;
	Fvector2				m_wheel_size[2];
	enum {fl_SoundFinalized	= 1, fl_MovingStoped = 2	};

	Flags32				m_flags;	
};