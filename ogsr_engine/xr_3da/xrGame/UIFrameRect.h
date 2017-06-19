#ifndef __XR_UIFRAMERECT_H__
#define __XR_UIFRAMERECT_H__
#pragma once

#include "uistaticitem.h"
#include "ui/uiabstract.h"

class CUIFrameRect: public CUISimpleWindow, CUIMultiTextureOwner //public CUICustomItem
{
public:
	enum EFramePart{
		fmBK=0,
		fmL, fmR, fmT, fmB, fmLT, fmRB, fmRT, fmLB, fmMax
	};
	Flags16		m_itm_mask;

	friend class CUIFrameWindow;
	using CUISimpleWindow::Init;

						CUIFrameRect	();
	virtual void		Init			(LPCSTR base_name, float x, float y, float w, float h);//, DWORD align);
	virtual void		InitTexture		(const char* texture);
	virtual void		Draw			();
	virtual void		Draw			(float x, float y);
	virtual void		SetWndPos		(float x, float y);
	virtual void		SetWndPos		(const Fvector2& pos);
	virtual void		SetWndSize		(const Fvector2& size);
	virtual void		SetWndRect		(const Frect& rect);
	virtual void		SetWidth		(float width);
	virtual void		SetHeight		(float height);
	virtual void		Update			();
			void		SetTextureColor	(u32 cl);
			void		SetVisiblePart	(EFramePart p, BOOL b)	{m_itm_mask.set(u16(1<<p), b);};
protected:
	CUIStaticItem	frame[fmMax];

	enum {
		flValidSize	= (1<<0),
		flSingleTex	= (1<<1),
	};
	Flags8			uFlags;
	void			UpdateSize		();
};

#endif  //__XR_UIFRAMERECT_H__