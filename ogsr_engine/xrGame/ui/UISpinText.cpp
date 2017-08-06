#include "StdAfx.h"
#include "UISpinText.h"
#include "UILines.h"
#include "../string_table.h"

CUISpinText::CUISpinText(){
    m_curItem = -1;
}

void CUISpinText::AddItem_(const char* item, int id)
{
	SInfo			_info;
	_info._orig		= item;
	_info._transl	= CStringTable().translate(item);
	_info._id		= id;

	m_list.push_back( _info );
	if (-1 == m_curItem)
	{
		m_curItem		= 0;
		SetItem			();
	}
}

void CUISpinText::SetItem()
{
	R_ASSERT			(m_curItem != -1);
	m_pLines->SetText	(m_list[m_curItem]._transl.c_str());
}

LPCSTR CUISpinText::GetTokenText()
{
	R_ASSERT			(m_curItem != -1);
	return				m_list[m_curItem]._orig.c_str();
}

void CUISpinText::SetCurrentValue(){
	xr_token* tok = GetOptToken();

	while (tok->name){
		AddItem_(tok->name, tok->id);
		tok++;
	}
	xr_string val = GetOptTokenValue();

	for (u32 i = 0; i < m_list.size(); i++)
		if (val == m_list[i]._orig.c_str())
		{
			m_curItem	= i;
			break;
		}

	SetItem();
}

void CUISpinText::SaveValue()
{
	CUIOptionsItem::SaveValue		();
	SaveOptTokenValue				(m_list[m_curItem]._orig.c_str());
}

bool CUISpinText::IsChanged()
{
	return 0 != xr_strcmp(GetOptTokenValue(), m_list[m_curItem]._orig.c_str());
}

void CUISpinText::OnBtnUpClick()
{
	if (CanPressUp())
	{
		m_curItem		++;
		SetItem			();
	}

	CUICustomSpin::OnBtnUpClick();
}

void CUISpinText::OnBtnDownClick()
{
	if (CanPressDown())
	{
		m_curItem--;
		SetItem		();
	}

	CUICustomSpin::OnBtnDownClick();
}

bool CUISpinText::CanPressUp()
{
	return m_curItem < (int)m_list.size() - 1;
}

bool CUISpinText::CanPressDown()
{
	return m_curItem > 0;
}