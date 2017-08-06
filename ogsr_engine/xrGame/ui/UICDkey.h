//
#pragma once
#include "UIEditBox.h"

class CUICDkey : public CUIEditBox {
public:
						CUICDkey		();
	virtual	void		SetText			(LPCSTR str) {}
	virtual	const char* GetText			();
	// CUIOptionsItem
	virtual void	SetCurrentValue();
	virtual void	SaveValue();
	virtual bool	IsChanged();
	
			void	CreateCDKeyEntry();			

	virtual void	Draw();
	virtual void	AddChar(char c);
	virtual void	OnFocusLost		();

private:
			LPCSTR	AddHyphens(LPCSTR str);
			LPCSTR	DelHyphens(LPCSTR str);

};

extern	void	GetCDKey(char* CDKeyStr);