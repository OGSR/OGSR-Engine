#pragma once
#include "../xr_level_controller.h"
class CUIWindow;

#include "UIMessages.h"
#include "../script_export_space.h"
#include "uiabstract.h"


class CUIWindow  : public CUISimpleWindow
{
public:
	using CUISimpleWindow::Init;

				CUIWindow						();
	virtual		~CUIWindow						();

	////////////////////////////////////
	//инициализация
	virtual void			Init				(Frect* pRect);

	////////////////////////////////////
	//работа с дочерними и родительскими окнами
	virtual void			AttachChild			(CUIWindow* pChild);
	virtual void			DetachChild			(CUIWindow* pChild, bool from_destructor = false);
	virtual void			DoDetachChild			(CUIWindow* pChild, bool from_destructor = false);
	virtual bool			IsChild				(CUIWindow* pChild) const;
	virtual void			DetachAll			();
	int						GetChildNum			()								{return m_ChildWndList.size();} 

	void					DetachFromParent	();

	void					SetParent			(CUIWindow* pNewParent);
	CUIWindow*				GetParent			()	const							{return m_pParentWnd;}
	
	//получить окно самого верхнего уровня
	CUIWindow*				GetTop				()								{if(m_pParentWnd == NULL) return  this; 
																				else return  m_pParentWnd->GetTop();}
	CUIWindow*				GetCurrentMouseHandler();
	CUIWindow*				GetChildMouseHandler();


	//поднять на вершину списка выбранное дочернее окно
	bool					BringToTop			(CUIWindow* pChild);

	//поднять на вершину списка всех родителей окна и его самого
	void					BringAllToTop		();
	


	virtual bool 			OnMouse				(float x, float y, EUIMessages mouse_action);
	virtual void 			OnMouseMove			();
	virtual void 			OnMouseScroll		(float iDirection);
	virtual bool 			OnDbClick			();
	virtual bool 			OnMouseDown			(int mouse_btn);
	virtual void 			OnMouseUp			(int mouse_btn);
	virtual void 			OnFocusReceive		();
	virtual void 			OnFocusLost			();
	virtual void UpdateFocus( bool = false );
	virtual void CommitFocus( bool = false );
	virtual bool CapturesFocusToo();
			bool 			HasChildMouseHandler();

	//захватить/освободить мышь окном
	//сообщение посылается дочерним окном родительскому
	void					SetMouseCapture			(CUIWindow* pChildWindow, bool capture_status);
	CUIWindow*				GetMouseCapturer();

	//окошко, которому пересылаются сообщения,
	//если NULL, то шлем на GetParent()
	void					SetMessageTarget	(CUIWindow* pWindow)								{m_pMessageTarget = pWindow;}
	CUIWindow*				GetMessageTarget	();

	//реакция на клавиатуру
	virtual bool			OnKeyboard			(int dik, EUIMessages keyboard_action);
	virtual bool			OnKeyboardHold		(int dik);
	virtual void			SetKeyboardCapture	(CUIWindow* pChildWindow, bool capture_status);

	
	
	//обработка сообщений не предусмотреных стандартными обработчиками
	//ф-ция должна переопределяться
	//pWnd - указатель на окно, которое послало сообщение
	//pData - указатель на дополнительные данные, которые могут понадобиться
	virtual void			SendMessage			(CUIWindow* pWnd, s16 msg, void* pData = NULL);
	
	

	//запрещение/разрешение на ввод с клавиатуры
	virtual void			Enable				(bool status)									{m_bIsEnabled=status;}
	virtual bool			IsEnabled			()												{return m_bIsEnabled;}

	//убрать/показать окно и его дочерние окна
	virtual void			Show				(bool status)									{SetVisible(status); Enable(status); }
	IC		bool			IsShown				()												{return this->GetVisible();}
			void			ShowChildren		(bool show);
	
	//абсолютные координаты
	IC void					GetAbsoluteRect		(Frect& r) ;
	IC void					GetAbsolutePos		(Fvector2& p) 	{Frect abs; GetAbsoluteRect(abs); p.set(abs.x1,abs.y1);}


			void			GetWndRect_script(Frect &rect)										{CUISimpleWindow::GetWndRect(rect);}
			void			SetWndRect_script(float x, float y, float width, float height)		{CUISimpleWindow::SetWndRect(x,y,width,height);}
			void			SetWndRect_script(Frect rect)										{CUISimpleWindow::SetWndRect(rect);}

			IC		float			GetPosLeft()							const { return m_wndPos.x; }
			IC		float			GetPosTop()							const { return m_wndPos.y; }

	//прорисовка окна
	virtual void			Draw				();
	virtual void			Draw				(float x, float y);
	//обновление окна передпрорисовкой
	virtual void			Update				();


			void			SetPPMode			();
			void			ResetPPMode			();
	IC		bool			GetPPMode			()		{return m_bPP;};
	//для перевода окна и потомков в исходное состояние
	virtual void			Reset				();
			void			ResetAll			();


	//временно!!!! (а может уже и нет)
	virtual void			SetFont				(CGameFont* pFont)			{ m_pFont = pFont;}
	CGameFont*				GetFont				()							{if(m_pFont) return m_pFont;
																				if(m_pParentWnd== NULL)	
																					return  m_pFont;
																				else
																					return  m_pParentWnd->GetFont();}

	using WINDOW_LIST = std::list<CUIWindow*>;
	using WINDOW_LIST_it = WINDOW_LIST::iterator;

	WINDOW_LIST&			GetChildWndList		()							{return m_ChildWndList; }


	IC bool					IsAutoDelete		()							{return m_bAutoDelete;}
	IC void					SetAutoDelete		(bool auto_delete)			{m_bAutoDelete = auto_delete;}

	// Name of the window
	const shared_str		WindowName			() const;
	void					SetWindowName		(LPCSTR wn, BOOL ifnset = FALSE);					
	LPCSTR					WindowName_script	()							{return *m_windowName;}
	CUIWindow*				FindChild			(const shared_str name,  u32 max_nested = 15);
	CUIWindow*				FindChild			(LPCSTR s)					{return FindChild(shared_str(s) );}

	IC bool					CursorOverWindow	() const					{ return m_bCursorOverWindow; }

protected:

	shared_str				m_windowName;
	//список дочерних окон
	WINDOW_LIST				m_ChildWndList;
	
	//указатель на родительское окно
	CUIWindow*				m_pParentWnd;

	//дочернее окно которое, захватило ввод мыши
	CUIWindow* m_pMouseCapturer;
	
	//кто изначально иницировал
	//захват фокуса, только он теперь
	//может весь фокус и освободить
	CUIWindow*				m_pOrignMouseCapturer;

	//дочернее окно которое, захватило ввод клавиатуры
	CUIWindow*				m_pKeyboardCapturer;

	//кому шлем сообщения
	CUIWindow*				m_pMessageTarget;


	CGameFont*				m_pFont;

	// Последняя позиция мышки
	Fvector2 cursor_pos;

	u32						m_dwFocusReceiveTime;

	//флаг автоматического удаления во время вызова деструктора
	bool					m_bAutoDelete;

	// Флаг разрешающий/запрещающий генерацию даблклика
	bool					m_bPP;
	//разрешен ли ввод пользователя
	bool					m_bIsEnabled;

	// Если курсор над окном
	bool					m_bCursorOverWindow;
	bool					m_bCursorOverWindowChanged;
	bool					m_bClickable;

#ifdef DEBUG
	int m_dbg_id;
	Flags32					m_dbg_flag;
#endif

public:
	inline float			GetMousePosX		() const					{ return cursor_pos.x;}
	inline float			GetMousePosY		() const					{ return cursor_pos.y;}

	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIWindow)
#undef script_type_list
#define script_type_list save_type_list(CUIWindow)
