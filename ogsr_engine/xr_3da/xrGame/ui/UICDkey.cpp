
#include "stdafx.h"
#include "UICDkey.h"
#include "UIColorAnimatorWrapper.h"
#include "../../xr_IOConsole.h"
#include "../RegistryFuncs.h"

extern string64	gsCDKey;

CUICDkey::CUICDkey(){
	CreateCDKeyEntry();
	SetCurrentValue();
}

void CUICDkey::OnFocusLost()
{
	CUIWindow::OnFocusLost();
	if(m_bInputFocus)
	{
		m_bInputFocus = false;
		m_iKeyPressAndHold = 0;
		GetMessageTarget()->SendMessage(this,EDIT_TEXT_COMMIT,NULL);
	}
}

void CUICDkey::Draw()
{
	Frect						rect;
	GetAbsoluteRect				(rect);
	Fvector2					outXY;

	outXY.y						= (m_wndSize.y - m_lines.m_pFont->CurrentHeight_())/2.0f;
	outXY.x						= 0;
	m_lines.m_pFont->SetColor	(m_lines.GetTextColor());

	Fvector2					pos;
	pos.set						(rect.left+outXY.x, rect.top+outXY.y);
	UI()->ClientToScreenScaled	(pos);

	if(m_bInputFocus)
	{		
		m_lines.m_pFont->Out	( pos.x, pos.y, "%s" ,AddHyphens(m_lines.m_text.c_str()) );
		
		float _h				= m_lines.m_pFont->CurrentHeight_();
		UI()->ClientToScreenScaledHeight(_h);
		
		outXY.y					= rect.top + (m_wndSize.y - _h)/2.0f;
		
		float	_w_tmp			=0.0f;

		string256				buff;
		int i					= m_lines.m_iCursorPos;
		strncpy					(buff,m_lines.m_text.c_str(),i);
		buff					[i]=0;
		_w_tmp					= m_lines.m_pFont->SizeOf_(buff);
		UI()->ClientToScreenScaledWidth(_w_tmp);
		outXY.x					= rect.left+_w_tmp;
		
		_w_tmp					= m_lines.m_pFont->SizeOf_("-");
		UI()->ClientToScreenScaledWidth(_w_tmp);
		
		if(i>3)
			outXY.x	+= _w_tmp;
		if(i>7)
			outXY.x	+= _w_tmp;
		if(i>11)
			outXY.x	+= _w_tmp;

		UI()->ClientToScreenScaled	(outXY);
		m_lines.m_pFont->Out		(outXY.x, outXY.y, "_");
	}
	else
	{
		string64 tmp = "xxxxxxxxxxxxxxxx";
		tmp[m_lines.m_text.size()] = 0;

		m_lines.m_pFont->Out(pos.x, pos.y, "%s" ,AddHyphens(tmp) );
	}
}

const char* CUICDkey::GetText			()
{
	return AddHyphens(CUIEditBox::GetText());
}


LPCSTR CUICDkey::AddHyphens(LPCSTR c){
	static string32 buf;

	int sz = xr_strlen(c);
	int j = 0; 

	for (int i = 1; i<=3; i++)
		buf[i*5 - 1]='-';

	for (int i = 0; i<sz; i++)
	{
		j = i + iFloor(i/4.0f);
		buf[j] = c[i];		
	}
	buf[sz + iFloor(sz/4.0f)] = 0;

	return buf;
}

LPCSTR CUICDkey::DelHyphens(LPCSTR c){
	static string32 buf;

	int sz = xr_strlen(c);
	int j = 0; 

	for (int i = 0; i<sz - _min(iFloor(sz/4.0f),3); i++)
	{
		j = i + iFloor(i/4.0f);
		buf[i] = c[j];		
	}
	buf[sz - _min(iFloor(sz/4.0f),3)] = 0;

	return buf;
}

void CUICDkey::AddChar(char c){
	if (m_lines.m_text.length() < 16)
	{
		CUIEditBox::AddChar(c);
		SaveValue();
	}
}

void CUICDkey::SetCurrentValue(){
	char CDKeyStr[64];
	CDKeyStr[0] = 0;
	GetCDKey(CDKeyStr);
	m_lines.SetText(DelHyphens(CDKeyStr));

}
extern string64	gsCDKey;

void CUICDkey::SaveValue(){
	CUIOptionsItem::SaveValue();

//	char NewCDKey[32];
//	HKEY KeyCDKey = 0;

//	string256 tmp;
	sprintf_s(gsCDKey,"%s",AddHyphens(m_lines.GetText()));
//	sprintf_s(tmp,"cdkey %s",AddHyphens(m_lines.GetText()));
//	Console->Execute(tmp);

}

bool CUICDkey::IsChanged(){
	string64	tmpCDKeyStr;
	GetCDKey	(tmpCDKeyStr);
	return 0 != xr_strcmp(tmpCDKeyStr, m_lines.GetText());
}

void CUICDkey::CreateCDKeyEntry(){

}

void GetCDKey(char* CDKeyStr){
	ReadRegistry_StrValue(REGISTRY_VALUE_GSCDKEY, CDKeyStr);
}