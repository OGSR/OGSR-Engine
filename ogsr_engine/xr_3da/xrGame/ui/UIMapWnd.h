#pragma once

#include "UIWindow.h"
#include "UIWndCallback.h"


class CUICustomMap;
class CUIGlobalMap;
class CUIFrameWindow;
class CUIScrollBar;
class CUIFrameLineWnd;
class CMapActionPlanner;
class CUITabControl;
class CUIStatic;
class CUI3tButton;
class CUILevelMap;
class CUIMapHint;
class CMapLocation;

DEFINE_MAP(shared_str,CUICustomMap*,GameMaps,GameMapsPairIt);


class CUIMapWnd: public CUIWindow, public CUIWndCallback
{
	typedef CUIWindow inherited;
	enum EMapToolBtn{	eGlobalMap=0,
						eZoomIn,
						eZoomOut,
//.						eAddSpot,
//.						eRemoveSpot,
						eActor,
//.						eHighlightSpot,
						eMaxBtn};
public:
	enum lmFlags{	//. lmUserSpotAdd	= (1<<1),
					//. lmUserSpotRemove= (1<<2),
					lmZoomIn		= (1<<3),
					lmZoomOut		= (1<<4),
					lmFirst			= (1<<5),
//.					lmHighlightSpot = (1<<6),
				};
	Flags32						m_flags;
private:
	float						m_currentZoom;
	CUIGlobalMap*				m_GlobalMap;
	GameMaps					m_GameMaps;
	
	CUIFrameWindow*				m_UIMainFrame;
	CUIScrollBar*				m_UIMainScrollV;
	CUIScrollBar*				m_UIMainScrollH;
	CUIWindow*					m_UILevelFrame;
	CMapActionPlanner*			m_ActionPlanner;
	CUIFrameLineWnd*			UIMainMapHeader;
	CUI3tButton*				m_ToolBar[eMaxBtn];
	CUIMapHint*					m_hint;
//.	CMapLocation*				m_selected_location;
	CUIStatic*					m_text_hint;

	void __stdcall				OnScrollV				(CUIWindow*, void*);
	void __stdcall				OnScrollH				(CUIWindow*, void*);
	void __stdcall				OnToolGlobalMapClicked	(CUIWindow*, void*);
//.	void						OnToolHighlightSpotClicked(CUIWindow*, void*);
	void __stdcall				OnToolActorClicked		(CUIWindow*, void*);
	void						OnToolNextMapClicked	(CUIWindow*, void*);
	void						OnToolPrevMapClicked	(CUIWindow*, void*);
	void __stdcall				OnToolZoomInClicked		(CUIWindow*, void*);
	void __stdcall				OnToolZoomOutClicked	(CUIWindow*, void*);
//.	void						OnToolAddSpotClicked	(CUIWindow*, void*);
//.	void						OnToolRemoveSpotClicked	(CUIWindow*, void*);
	void						ValidateToolBar			();

//.	void						RemoveSpot				();
//.	void						HighlightSpot			();
	void						ResetActionPlanner		();


public:
	CUICustomMap*				m_tgtMap;
	Fvector2					m_tgtCenter;
public:
								CUIMapWnd				();
	virtual						~CUIMapWnd				();

	virtual void				Init					(LPCSTR xml_name, LPCSTR start_from);
	virtual void				Show					(bool status);
	virtual void				Draw					();
	virtual void				Reset					();
	virtual void				Update					();
			float				GetZoom					()	{return m_currentZoom;}
			void				SetZoom					(float value);

			void				ShowHint				(CUIWindow* parent, LPCSTR text);
			void				HideHint				(CUIWindow* parent);
			void				Hint					(const shared_str& text);
	virtual bool				OnMouse					(float x, float y, EUIMessages mouse_action);
	virtual bool				OnKeyboard				(int dik, EUIMessages keyboard_action);
	virtual bool				OnKeyboardHold			(int dik);

	virtual void				SendMessage				(CUIWindow* pWnd, s16 msg, void* pData = NULL);

	void						SetTargetMap			(CUICustomMap* m, bool bZoomIn = false);
	void						SetTargetMap			(CUICustomMap* m, const Fvector2& pos, bool bZoomIn = false);
	void						SetTargetMap			(const shared_str& name, const Fvector2& pos, bool bZoomIn = false);
	void						SetTargetMap			(const shared_str& name, bool bZoomIn = false);
//.	void						AddUserSpot				(CUILevelMap*);
//.	void						Select					(CMapLocation* ml);

	Frect						ActiveMapRect			()		{Frect r; m_UILevelFrame->GetAbsoluteRect(r); return r;};
	void						AddMapToRender			(CUICustomMap*);
	void						RemoveMapToRender		(CUICustomMap*);
	CUIGlobalMap*				GlobalMap				()		{return m_GlobalMap;};
	const GameMaps&				GameMaps				()		{return m_GameMaps;};	
	CUICustomMap*				GetMapByIdx				(u16 idx);
	u16							GetIdxByName			(const shared_str& map_name);
	void						UpdateScroll			();
	shared_str					cName					() const	{return "ui_map_wnd";};
};
