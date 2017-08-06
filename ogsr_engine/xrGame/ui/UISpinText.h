// file:		UISpinNum.h
// description:	Spin Button with text data (unlike numerical data)
// created:		15.06.2005
// author:		Serge Vynnychenko
//

#include "UICustomSpin.h"

class CUISpinText : public CUICustomSpin{
public:
	CUISpinText();
	// CUIOptionsItem
	virtual void	SetCurrentValue();
	virtual void	SaveValue();
	virtual bool	IsChanged();	

	// own
	virtual void	OnBtnUpClick();
	virtual void	OnBtnDownClick();

			void	AddItem_(const char* item, int id);
			LPCSTR	GetTokenText();
protected:
	virtual bool	CanPressUp		();
	virtual bool	CanPressDown	();
	virtual void	IncVal			(){};
	virtual void	DecVal			(){};
			void	SetItem();
			struct SInfo{
				shared_str	_orig;
				shared_str	_transl;
				int			_id;
			};
	typedef xr_vector< SInfo >		Items;
	typedef Items::iterator			Items_it;

    Items	m_list;
	int		m_curItem;
};