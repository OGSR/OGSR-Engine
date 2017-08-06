// File:		UILine.cpp
// Description:	Single text line
// Created:		05.04.2005
// Author:		Serge Vynnycheko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World


#include "StdAfx.h"
#include "UILine.h"
#include "uilinestd.h"
#include "UIColorAnimatorWrapper.h"

CUIColorAnimatorWrapper CUILine::m_animation;

//#define LOG_ALL_LINES
#ifdef LOG_ALL_LINES
	int ListLinesCount = 0;
	struct DBGList{
		CUILine*		wnd;
		int				num;
	};
	xr_vector<DBGList>	dbg_list_lines;
	void dump_list_lines(){
		Msg("------Total  Lines %d",dbg_list_lines.size());
		xr_vector<DBGList>::iterator _it = dbg_list_lines.begin();
		for(;_it!=dbg_list_lines.end();++_it)
			Msg("--leak detected ---- Line = %d",(*_it).num);
	}
#else
	void dump_list_lines(){}
#endif

CUILine::CUILine(){
	m_tmpLine = NULL;
	m_animation.SetColorAnimation("ui_map_area_anim");
	m_animation.Cyclic(true);

#ifdef LOG_ALL_LINES
	ListLinesCount++;
	dbg_list_lines.push_back(DBGList());
	dbg_list_lines.back().wnd = this;
	dbg_list_lines.back().num = ListLinesCount;
#endif
}

CUILine::~CUILine(){
	xr_delete(m_tmpLine);

#ifdef LOG_ALL_LINES
	xr_vector<DBGList>::iterator _it = dbg_list_lines.begin();
	bool bOK = false;
	for(;_it!=dbg_list_lines.end();++_it){
		if((*_it).wnd == this){
			bOK = true;
			dbg_list_lines.erase(_it);
			break;
		}
	}
	if(!bOK)
		Msg("CUILine::~CUILine()!!!!!!!!!!!!!!!!!!!!!!! cannot find window in list");
#endif
}

CUILine::CUILine(const CUILine& other){
	m_subLines = other.m_subLines;
	m_tmpLine = NULL;
#ifdef LOG_ALL_LINES
	ListLinesCount++;
	dbg_list_lines.push_back(DBGList());
	dbg_list_lines.back().wnd = this;
	dbg_list_lines.back().num = ListLinesCount;
#endif
}

CUILine& CUILine::operator =(const CUILine& other){
	m_subLines = other.m_subLines;
	xr_delete(m_tmpLine);
	return (*this);
}

void CUILine::AddSubLine(const xr_string& str, u32 color){
	CUISubLine sline;
	sline.m_color = color;
	sline.m_text = str;
	m_subLines.push_back(sline);
}

void CUILine::AddSubLine(LPCSTR str, u32 color){
	CUISubLine sline;
	sline.m_color = color;
	sline.m_text = str;
	m_subLines.push_back(sline);
}

void CUILine::AddSubLine(const CUISubLine* subLine){
	m_subLines.push_back(*subLine);
}

void CUILine::Clear(){
	m_subLines.clear();
}

void CUILine::ProcessNewLines(){
#pragma todo("SATAN->SATAN: sometimes working badly (leaves empty lines)")
	for (u32 i=0; i < m_subLines.size(); i++){
		StrSize pos = m_subLines[i].m_text.find("\\n");
		if (pos != npos)
		{
			CUISubLine sbLine;
			if (pos)
                sbLine = *m_subLines[i].Cut2Pos((int)pos-1);
			sbLine.m_last_in_line = true;
			m_subLines.insert(m_subLines.begin()+i, sbLine);
			m_subLines[i+1].m_text.erase(0,2);
			if (m_subLines[i+1].m_text.empty()){
				m_subLines.erase(m_subLines.begin()+i+1);
			}
		}
	}
}

void CUILine::Draw(CGameFont* pFont, float x, float y) const{
	float length = 0;
	int size = m_subLines.size();

	for (int i=0; i<size; i++)
	{
		m_subLines[i].Draw(pFont, x+length, y);
		float ll = pFont->SizeOf_(m_subLines[i].m_text.c_str()); //. all ok
		UI()->ClientToScreenScaledWidth(ll);
		length	+= ll;
	}
}

int CUILine::GetSize(){
	int sz = 0;
	int size = m_subLines.size();
	for (int i=0; i<size; i++)
		sz += (int)m_subLines[i].m_text.size();

	return sz;
}

const CUILine* CUILine::GetEmptyLine(){
	xr_delete(m_tmpLine);
	m_tmpLine = xr_new<CUILine>();

    return m_tmpLine;
}

const CUILine* CUILine::CutByLength(CGameFont* pFont, float length, BOOL cut_word)
{
	R_ASSERT(GetSize() > 0);
	// if first sub line is void then delete it

	Position pos;
	InitPos(pos);

	if (!pos.word_1.exist()) // void string
	{
		if (m_subLines[0].m_last_in_line)
		{
			m_subLines.erase(m_subLines.begin());
			return GetEmptyLine();
		}
		else
		{
			m_subLines.erase(m_subLines.begin());
			InitPos(pos);
		}
	}


	float len2w1 = GetLength_inclusiveWord_1(pos, pFont);
//.	* scale; bacause of our fonts not scaled

    if (!pos.word_2.exist())
	{
		if (cut_word && len2w1 > length)
			return CutWord(pFont, length);		
		else
			return Cut2Pos(pos);
	}

	
	float len2w2 = GetLength_inclusiveWord_2(pos, pFont);

	if (len2w1 > length)
	{
		if (cut_word)
            return CutWord(pFont, length);		
		else
            return Cut2Pos(pos);
	}
	else if (len2w1 <= length && len2w2 > length)
	{
		// cut whole first word
		return Cut2Pos(pos);  // all right :)
	}
	else // if (len2w1 > length && len2w2 > length)
	{
		while (IncPos(pos))
		{
			len2w1 = GetLength_inclusiveWord_1(pos, pFont);
			if(!pos.word_2.exist())
			{
				return Cut2Pos(pos);
			}
			len2w2 = GetLength_inclusiveWord_2(pos, pFont);
			if (len2w1 <= length && len2w2 > length)
				return Cut2Pos(pos);
		}
     
		return Cut2Pos(pos, false);
	}

}

bool CUILine::GetWord(Word& w, const xr_string& text, int begin) const{

	if (text.empty())
		return false;

	StrSize first, last, lastsp/*last space*/;
	first  = text.find_first_not_of(' ', begin);
	last   = text.find_first_of(' ', first);
	
	if( npos==last && npos==first )
		return false;

	if( npos==last && npos!=first )
	{
		w.pos		= (int)first;
		w.len		= (int)(text.length()-first);
		w.len_full	= w.len;
		return		true;
	}

	lastsp			= text.find_first_not_of(' ', last);

	if (npos == lastsp && npos == first) // maybe we have string only with spaces
	{
		first		= text.find_first_of(' ',begin);
		last		= text.find_last_of(' ',begin);
		
		if (npos == first) //suxxx it is empty string
			return	false;

		w.pos		= (int)first;
		w.len		= (int)(last - first + 1);
		w.len_full	= w.len;
		return		true;
	}
	

	if (npos == lastsp)
		lastsp = last;
	else
		--lastsp;

	if (npos == last && npos != first)
		last = text.size() - 1;
	else
		--last;

	if (npos == lastsp)
		lastsp = last;

	first = begin;

	w.pos		= (int) first;
	w.len		= (int)(last - first + 1);
	w.len_full	= (int)(lastsp - first + 1);

#ifdef DEBUG
	if (npos != first && (npos == last || npos == lastsp ))
		R_ASSERT2(false,"CUILine::InitPos -- impossible match");
#endif

	return true;        
}

bool CUILine::InitPos(Position& pos) const
{
	Word w;
	pos.curr_subline = 0;
	if (GetWord(w, m_subLines[0].m_text, 0))
		pos.word_1 = w;
	else
		return false;

	if (GetWord(w, m_subLines[0].m_text, w.last_space() + 1))
		pos.word_2 = w;
	else if (m_subLines.size() > 1 && GetWord(w, m_subLines[1].m_text, 0))
		pos.word_2 = w;

	return true;
}

bool CUILine::IncPos(Position& pos) const{
	u32 totalLinesCount = m_subLines.size();
	if (totalLinesCount < pos.curr_subline)
		return false;

	Word w;
	u32 curLine = pos.curr_subline;

	if ( ! pos.is_separated() )
	{
		if (GetWord(w, m_subLines[curLine].m_text, pos.word_2.last_space() + 1))
		{
			pos.word_1 = pos.word_2;
			pos.word_2 = w;
			return true;
		}
		else if (curLine + 1 <= totalLinesCount - 1)
		{
			if (GetWord(w, m_subLines[curLine + 1].m_text, 0))
			{
				pos.word_1 = pos.word_2;
				pos.word_2 = w;
				return true;
			}
		}
		else
			return false;
	}
	else if (curLine + 1 <= totalLinesCount -1)
	{
		if (GetWord(w, m_subLines[curLine + 1].m_text, pos.word_2.last_space() + 1))
		{
			pos.word_1 = pos.word_2;
			pos.word_2 = w;
			pos.curr_subline = curLine + 1;
			return true;
		}
		else if (curLine + 2 <= totalLinesCount -1)
			if (GetWord(w, m_subLines[curLine + 2].m_text, 0))
			{
				pos.word_1 = pos.word_2;
				pos.word_2 = w;
				pos.curr_subline = curLine + 1;
				return true;
			}
			return false;
	}
	return false;
}

const CUILine* CUILine::Cut2Pos(Position& pos, bool to_first){
	xr_delete(m_tmpLine);
	m_tmpLine = xr_new<CUILine>();

	int last;

	if (to_first || !pos.is_separated())
		last = pos.curr_subline - 1;
	else
		last = pos.curr_subline;

	for (int i = 0; i<= last; i++)
	{
		m_tmpLine->AddSubLine(&m_subLines[i]);

		if (m_subLines[i].m_last_in_line) // check if this subline must be last in line
		{
			for (int j = 0; j<= i; j++)
				m_subLines.erase(m_subLines.begin());
			return m_tmpLine;
		}
	}

	if (to_first)
		m_tmpLine->AddSubLine(m_subLines[last + 1].Cut2Pos(pos.word_1.last_space()));
	else
		m_tmpLine->AddSubLine(m_subLines[last + 1].Cut2Pos(pos.word_2.last_space()));

	for (int i = 0; i<= last; i++)
        m_subLines.erase(m_subLines.begin());

    return m_tmpLine;
}

const CUILine* CUILine::CutWord(CGameFont* pFont, float length){
	xr_delete(m_tmpLine);
	m_tmpLine = xr_new<CUILine>();

	float len = 0;

	for (u32 i= 0; i<m_subLines[0].m_text.length(); i++)
	{
		float ll = pFont->SizeOf_(m_subLines[0].m_text[i]);
		UI()->ClientToScreenScaledWidth(ll);
		len += ll;

		if (len>length){
			m_tmpLine->AddSubLine(m_subLines[0].Cut2Pos((i?i:1)-1));
			return m_tmpLine;
		}
	}

	R_ASSERT2(false, "meaningless call of CUILine::CutWord() ):");

	return m_tmpLine;
}

float  CUILine::GetLength_inclusiveWord_1(Position& pos, CGameFont* pFont) const{
	R_ASSERT(pos.word_1.exist());
	float len = 0;

	for (u32 i = 0; i < pos.curr_subline; ++i)
	{
		float ll = pFont->SizeOf_(m_subLines[i].m_text.c_str());
		UI()->ClientToScreenScaledWidth(ll);
		len	+= ll;
	}

	xr_string str;
	str.assign(m_subLines[pos.curr_subline].m_text, 0, pos.word_1.pos + pos.word_1.len);

	
	float ll2 = pFont->SizeOf_(str.c_str());
	UI()->ClientToScreenScaledWidth(ll2);
	len += ll2;

	return len;
}

float  CUILine::GetLength_inclusiveWord_2(Position& pos, CGameFont* pFont) const{
	R_ASSERT(pos.word_2.exist());
	float len = 0;
	int last;

	if (!pos.is_separated())
		last = pos.curr_subline - 1;
	else
		last = pos.curr_subline;

	for (int i = 0; i <= last; i++)
	{
		float ll = pFont->SizeOf_(m_subLines[i].m_text.c_str());
		UI()->ClientToScreenScaledWidth(ll);
		len += ll;
	}

	xr_string str;
	str.assign(m_subLines[last + 1].m_text, 0, pos.word_2.pos + pos.word_2.len);

	float ll2	= pFont->SizeOf_(str.c_str());
	UI()->ClientToScreenScaledWidth(ll2);
	len			+= ll2;

	return len;
}
