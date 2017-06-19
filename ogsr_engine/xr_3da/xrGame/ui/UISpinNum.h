// file:		UISpinNum.h
// description:	Spin Button with numerical data (unlike text data)
// created:		15.06.2005
// author:		Serge Vynnychenko
//

#include "UICustomSpin.h"

class CUISpinNum : public CUICustomSpin 
{
public:
					CUISpinNum			();

	virtual void	Init				(float x, float y, float width, float height);

	// CUIOptionsItem
	virtual void	SetCurrentValue		();
	virtual void	SaveValue			();
	virtual bool	IsChanged			();

    virtual void	OnBtnUpClick		();
	virtual void	OnBtnDownClick		();

			void	SetMax				(int max)	{m_iMax=max;};
			void	SetMin				(int min)	{m_iMin=min;};
			int		Value				() const {return m_iVal;}
protected:
			void	SetValue			();
	virtual bool	CanPressUp			();
	virtual bool	CanPressDown		();
	virtual void		IncVal			();
	virtual void		DecVal			();



	int		m_iMax;
	int		m_iMin;
	int		m_iStep;
	int		m_iVal;
};

class CUISpinFlt : public CUICustomSpin 
{
public:
	CUISpinFlt();

	virtual void	Init				(float x, float y, float width, float height);

	// CUIOptionsItem
	virtual void	SetCurrentValue		();
	virtual void	SaveValue			();
	virtual bool	IsChanged			();

    virtual void	OnBtnUpClick		();
	virtual void	OnBtnDownClick		();

			void	SetMax				(float max);
			void	SetMin				(float min);
protected:
			void	SetValue			();
	virtual bool	CanPressUp			();
	virtual bool	CanPressDown		();
	virtual void	IncVal				();
	virtual void	DecVal				();



	float		m_fMax;
	float		m_fMin;
	float		m_fStep;
	float		m_fVal;
};

