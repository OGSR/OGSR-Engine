#pragma once


#include "uiwindow.h"
#include "../uistaticitem.h"
#include "../script_export_space.h"

class CUIFrameWindow;
class CLAItem;
class CUIXml;
class CUILines;

struct lanim_cont{
	CLAItem*				m_lanim;
	float					m_lanim_start_time;
	float					m_lanim_delay_time;
	Flags8					m_lanimFlags;

	void					set_defaults		();
};

class CUIStatic : public CUIWindow, public CUISingleTextureOwner, public IUITextControl  
{
	friend class CUIXmlInit;
	friend class CUI3tButton;
private:
	typedef CUIWindow inherited;
	lanim_cont				m_lanim_clr;
	lanim_cont				m_lanim_xform;
	void					EnableHeading_int		(bool b)				{m_bHeading = b;}

	shared_str				m_texture;
	shared_str				m_shader;
public:
	using CUISimpleWindow::SetWndRect;

					CUIStatic				();
	virtual			~CUIStatic				();

	// IUISimpleWindow--------------------------------------------------------------------------------------
	virtual void	Init					(float x, float y, float width, float height);
	virtual void	Draw					();
	virtual void	Update					();
	//
			void	RescaleRelative2Rect(const Frect& r);	//need to save proportions of texture			

	// IUISingleTextureOwner--------------------------------------------------------------------------------
	virtual void		CreateShader				(const char* tex, const char* sh = "hud\\default");
	virtual ref_shader& GetShader					();
	virtual void		SetTextureColor				(u32 color);
	virtual u32			GetTextureColor				() const;
	virtual void		SetOriginalRect				(const Frect& r)			{m_UIStaticItem.SetOriginalRect(r);}
	virtual void		SetOriginalRectEx			(const Frect& r)			{m_UIStaticItem.SetOriginalRectEx(r);}

	virtual Frect		GetOriginalRect				() const					{ return m_UIStaticItem.GetOriginalRect(); }
	//
			void		SetVTextAlignment(EVTextAlignment al);
	virtual void		SetColor					(u32 color)					{ m_UIStaticItem.SetColor(color);		}
	u32					GetColor					() const					{ return m_UIStaticItem.GetColor();		}
	u32&				GetColorRef					()							{ return m_UIStaticItem.GetColorRef();	}
	virtual void		InitTexture					(LPCSTR tex_name);
	virtual void		InitTextureEx				(LPCSTR tex_name, LPCSTR sh_name="hud\\default");
	CUIStaticItem*		GetStaticItem				()							{return &m_UIStaticItem;}
			void		SetOriginalRect				(float x, float y, float width, float height)	{m_UIStaticItem.SetOriginalRect(x,y,width,height);}
			void		SetHeadingPivot				(const Fvector2& p, const Fvector2& offset, bool fixedLT) { m_UIStaticItem.SetHeadingPivot(p, offset, fixedLT); }
			void		ResetHeadingPivot			()							{ m_UIStaticItem.ResetHeadingPivot(); }

			void		SetMask						(CUIFrameWindow *pMask);
	virtual void		SetTextureOffset			(float x, float y)			{ m_TextureOffset.set(x, y); }
			Fvector2	GetTextureOffeset			() const					{ return m_TextureOffset; }
			void		TextureOn					()							{ m_bTextureEnable = true; }
			void		TextureOff					()							{ m_bTextureEnable = false; }
			void		TextureAvailable			(bool value)				{ m_bAvailableTexture = value; }
			bool		TextureAvailable			()							{ return m_bAvailableTexture;}

			const  LPCSTR	GetTextureName			()	const					{ return m_texture.c_str(); }
			const  LPCSTR	GetShaderName			()	const					{ return m_shader.c_str(); }
	// own
	virtual void		SetHighlightColor			(const u32 uColor)	{ m_HighlightColor = uColor; }
			void		EnableTextHighlighting		(bool value)		{ m_bEnableTextHighlighting = value; }
			void		SetClrLightAnim				(LPCSTR lanim, bool bCyclic, bool bOnlyAlpha, bool bTextColor, bool bTextureColor);
			void		SetXformLightAnim			(LPCSTR lanim, bool bCyclic);
			void		ResetClrAnimation			();
			void		ResetXformAnimation			();
			bool		IsClrAnimStoped				();
			void		SetClrAnimDelay				(float delay);
	virtual void		Init						(LPCSTR tex_name, float x, float y, float width, float height);	
			void		InitEx						(LPCSTR tex_name, LPCSTR sh_name, float x, float y, float width, float height);

	virtual void		DrawTexture					();
	virtual void		DrawText					();
	virtual void		DrawHighlightedText			();

	virtual void		OnFocusReceive				();
	virtual void		OnFocusLost					();

	//IUITextControl
	virtual void			SetText					(LPCSTR str);
			void			SetTextST				(LPCSTR str_id);
	virtual LPCSTR			GetText					();
	virtual void			SetTextColor			(u32 color);
	virtual u32				GetTextColor			();
	virtual void			SetFont					(CGameFont* pFont);
	virtual CGameFont*		GetFont					();
	virtual void			SetTextAlignment		(ETextAlignment alignment);
	virtual ETextAlignment	GetTextAlignment		();

	// text additional
			void	SetTextComplexMode			(bool md);
			void	SetTextAlign_script			(u32 align);
			u32		GetTextAlign_script			();
			void	SetTextColor_script			(int a, int r, int g, int b){SetTextColor(color_argb(a,r,g,b));}
			u32&	GetTextColorRef				();
//#pragma todo("Satan->Satan : delete next two functions")
//	virtual void			SetTextAlign		(CGameFont::EAligment align);
//	CGameFont::EAligment	GetTextAlign		();
			void AdjustHeightToText			();
			void AdjustWidthToText			();
			void HighlightText(bool bHighlight) {m_bEnableTextHighlighting = bHighlight;}
	virtual bool IsHighlightText();

	virtual void ClipperOn					();
	virtual void ClipperOff					();
	virtual void ClipperOff					(CUIStaticItem& UIStaticItem);
	virtual bool GetClipperState			()								{return m_bClipper;}
	void TextureClipper						(float offset_x = 0, float offset_y = 0,Frect* pClipRect = NULL);
	void TextureClipper						(float offset_x, float offset_y, Frect* pClipRect, CUIStaticItem& UIStaticItem);

	
	void			SetShader				(const ref_shader& sh);
	CUIStaticItem&	GetUIStaticItem			()						{return m_UIStaticItem;}

	virtual	void SetTextX					(float text_x)			{m_TextOffset.x = text_x;}
	virtual	void SetTextY					(float text_y)			{m_TextOffset.y = text_y;}
	virtual	void SetTextPos					(float x, float y)		{SetTextX(x); SetTextY(y);}
			float GetTextX					()						{return m_TextOffset.x;}
			float GetTextY					()						{return m_TextOffset.y;}

	void		SetStretchTexture			(bool stretch_texture)	{m_bStretchTexture = stretch_texture;}
	bool		GetStretchTexture			()						{return m_bStretchTexture;}

	void		SetClipRect					(Frect r);
	Frect		GetSelfClipRect				();
	Frect		GetClipperRect				();	

	// Анализируем текст на помещаемость его по длинне в заданную ширину, и если нет, то всталяем 
	// "\n" реализуем таким образом wordwrap
//	static void PreprocessText				(STRING &str, float width, CGameFont *pFont);
	enum EElipsisPosition
	{
		eepNone,
		eepBegin,
		eepEnd,
		eepCenter
	};

	void SetElipsis							(EElipsisPosition pos, int indent);
	
	void	SetHeading						(float f)				{m_fHeading = f;};
	float	GetHeading						()						{return m_fHeading;}
	bool	Heading							()						{return m_bHeading;}
	void	EnableHeading					(bool b)				{m_bHeading = b;m_lanim_xform.m_lanimFlags.set((1<<4),b);}

	// will be need by CUI3tButton
	// Don't change order!!!!!
	typedef enum {
		E, // enabled
		D, // disabled
		T, // touched
		H  // highlighted
	} E4States;

	void SetTextColor(u32 color, E4States state);

	CUILines*				m_pLines;
protected:
	bool			m_bEnableTextHighlighting;
		// Цвет подсветки
	u32				m_HighlightColor;

	// this array of color will be useful in CUI3tButton class
	// but we really need to declare it directly there because it must be initialized in CUIXmlInit::InitStatic
	u32  m_dwTextColor[4];
	bool m_bUseTextColor[4]; // note: 0 index will be ignored

	bool m_bClipper;
	bool m_bStretchTexture;
	bool m_bAvailableTexture;
	bool m_bTextureEnable;
	CUIStaticItem m_UIStaticItem;

	
	Fvector2		m_TextOffset;

	bool			m_bHeading;
	float			m_fHeading;

    // Для вывода текстуры с обрезанием по маске используем CUIFrameWindow
	CUIFrameWindow	*m_pMask;
	Fvector2		m_TextureOffset;

	// Обрезка надписи
	EElipsisPosition	m_ElipsisPos;
	void Elipsis(const Frect &rect, EElipsisPosition elipsisPos);
	int	m_iElipsisIndent;
	Frect	m_ClipRect;

private:
	Frect	m_xxxRect; // need by RescaleRelative2Rect(Frect& r). it is initializes only once in Init(x,y,width,height)

public:
	DECLARE_SCRIPT_REGISTER_FUNCTION
};

add_to_type_list(CUIStatic)
#undef script_type_list
#define script_type_list save_type_list(CUIStatic)
