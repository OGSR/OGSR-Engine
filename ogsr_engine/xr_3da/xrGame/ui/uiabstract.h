#pragma once

#pragma warning (disable : 4511 )
#pragma warning (disable : 4512 )

#include "../ui_base.h"
//#include <boost/noncopyable.hpp>

typedef CGameFont::EAligment ETextAlignment;

class IUIFontControl{
public:	
	virtual ~IUIFontControl()											 {};
	virtual void			SetTextColor(u32 color)						= 0;
	virtual u32				GetTextColor()								= 0;
	virtual void			SetFont(CGameFont* pFont)					= 0;
	virtual CGameFont*		GetFont()									= 0;
	virtual void			SetTextAlignment(ETextAlignment alignment)	= 0;
	virtual ETextAlignment	GetTextAlignment()							= 0;
};

typedef enum {
	valTop = 0,
	valCenter,
	valBotton
} EVTextAlignment;

class IUITextControl : public IUIFontControl{
public:
	virtual ~IUITextControl()											 {};
	virtual void SetText(LPCSTR text)								= 0;
	virtual LPCSTR GetText()										= 0;
};


// Texture controls
class IUISimpleTextureControl{
public:
	virtual ~IUISimpleTextureControl() {}
	virtual void		CreateShader(const char* tex, const char* sh = "hud\\default")	= 0;
	virtual void		SetShader(const ref_shader& sh)									= 0;
	virtual void		SetTextureColor(u32 color)										= 0;
	virtual u32			GetTextureColor()										const	= 0;
	virtual void		SetOriginalRect(const Frect& r)									= 0;
	virtual void		SetOriginalRectEx(const Frect& r)								= 0;
};

class IUIMultiTextureOwner{
public:
	virtual ~IUIMultiTextureOwner() {}	
	virtual void		InitTexture(const char* texture)								= 0;
	virtual bool		GetTextureAvailability()										= 0;
	virtual void		SetTextureVisible(bool vis)										= 0;
	virtual bool		GetTextureVisible()												= 0;
};

class CUIMultiTextureOwner : public IUIMultiTextureOwner{
public:
	CUIMultiTextureOwner(){m_bTextureAvailable = false; m_bTextureVisible = false;}
	virtual bool		GetTextureAvailability()	{return m_bTextureAvailable;}
	virtual void		SetTextureVisible(bool vis)	{m_bTextureVisible = true;}
	virtual bool		GetTextureVisible()			{return m_bTextureVisible;}
protected:
	bool m_bTextureAvailable;
	bool m_bTextureVisible;
};

class IUISingleTextureOwner : public CUIMultiTextureOwner, public IUISimpleTextureControl{
public:	
	virtual void		InitTextureEx(const char* texture, const char* shader)			= 0;
	virtual void		SetStretchTexture(bool stretch)									= 0;
	virtual bool		GetStretchTexture()												= 0;	
};

class CUISingleTextureOwner : public IUISingleTextureOwner{
public:
	virtual void		SetStretchTexture(bool stretch)	{m_bStretchTexture = stretch;}
	virtual bool		GetStretchTexture()				{return m_bStretchTexture;}
protected:
	bool m_bStretchTexture;
};

// Window
enum EWindowAlignment{
	waNone		=0,
	waLeft		=1,
	waRight		=2,
	waTop		=4,
	waBottom	=8,
	waCenter	=16
};

class IUISimpleWindow/* :public boost::noncopyable*/{
public:
						IUISimpleWindow						()			{};		
						IUISimpleWindow(const IUISimpleWindow&) = delete;
						void operator=(const IUISimpleWindow&) = delete;
	virtual				~IUISimpleWindow					()			{};
    
	virtual void		Init(float x, float y, float width, float height)= 0;
	virtual void		Draw()											= 0;
	virtual void		Draw(float x, float y)							= 0;
	virtual void		Update()										= 0;
	virtual void		SetWndPos(const Fvector2& pos)					= 0;
	virtual void		SetWndPos(float x, float y)						= 0;
	virtual void		SetWndSize(const Fvector2& size)				= 0;
	virtual void		SetWndRect(const Frect& rect)					= 0;
	virtual void		SetHeight(float height)							= 0;
	virtual void		SetWidth(float width)							= 0;
/*
private:
						IUISimpleWindow									(const IUISimpleWindow& other);
	IUISimpleWindow&	operator =										( const IUISimpleWindow& other );
*/
};

class CUISimpleWindow : public IUISimpleWindow {
public:
							CUISimpleWindow()							{m_alignment=waNone; m_wndPos.set(0,0); m_wndSize.set(0,0);}
	virtual void			Init(float x, float y, float width, float height)	{m_wndPos.set(x,y);m_wndSize.set(width, height);}
	virtual void			SetWndPos(const Fvector2& pos)				{m_wndPos.set(pos.x,pos.y);}
	virtual void			SetWndPos(float x, float y)					{m_wndPos.set(x,y);}
	IC		Fvector2		GetWndPos()							const	{return m_wndPos;}
	virtual void			SetWndSize(const Fvector2& size)			{m_wndSize = size;}
	IC		Fvector2		GetWndSize()						const	{return m_wndSize;}
	virtual void			SetHeight(float height)						{m_wndSize.y = height;}
	IC		float			GetHeight()							const	{return m_wndSize.y;}
	virtual void			SetWidth(float width)						{m_wndSize.x = width;}
	IC		float			GetWidth()							const	{return m_wndSize.x;}
	IC		void			SetVisible(bool vis)						{m_bShowMe = vis;}
	IC		bool			GetVisible()						const	{return m_bShowMe;}
	IC		void			SetAlignment(EWindowAlignment al)			{m_alignment = al;};
	virtual void			SetWndRect(float x, float y, float width, float height) {
																						m_wndPos.set(x,y); 
																						m_wndSize.set(width,height); }
	virtual void			SetWndRect(const Frect& rect)				{SetWndRect(rect.lt.x, rect.lt.y, rect.width(), rect.height());}
	IC		Frect			GetWndRect()						const	{Frect r; GetWndRect(r); return r;}
	IC		void			GetWndRect(Frect& res)				const
	{
		switch (m_alignment){
			case waNone:
				res.set(m_wndPos.x,m_wndPos.y,m_wndPos.x+m_wndSize.x,m_wndPos.y+m_wndSize.y);
				break;
			case waCenter:{
					float half_w = m_wndSize.x/2.0f;
					float half_h = m_wndSize.y/2.0f;
					res.set(m_wndPos.x - half_w,
							m_wndPos.y - half_h,
							m_wndPos.x + half_w,
							m_wndPos.y + half_h);
				}break;
			default:
				NODEFAULT;
		};
	}
				void			MoveWndDelta		(float dx, float dy)				{m_wndPos.x+=dx;m_wndPos.y+=dy;}
				void			MoveWndDelta		(const Fvector2& d)					{ MoveWndDelta(d.x, d.y);	};

protected:
	bool					m_bShowMe;
	Fvector2				m_wndPos;
	Fvector2				m_wndSize;
	EWindowAlignment		m_alignment;
};
class CUISelectable{
protected:
	bool m_bSelected;
public:
	CUISelectable():m_bSelected(false)		{}
	bool			GetSelected	() const	{return m_bSelected;}
	virtual void	SetSelected	(bool b)	{m_bSelected = b;};
};