// File:		UITimeWnd.cpp
// Description:	Timer window for Inventory window
// Created:		01.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua
//
// Copyright 2005 GSC Game World
//

#include "StdAfx.h"
#include "UITimeWnd.h"
#include "UIInventoryUtilities.h"

#define TIMER_HEIGHT		37
#define TIMER_WIDTH			185
#define CLOCK_FACE_INDENT_X	117
#define CLOCK_FACE_INDENT_Y	9

CUITimeWnd::CUITimeWnd(){
	AttachChild(&this->m_staticString);
	AttachChild(&this->m_staticTime);
}

CUITimeWnd::~CUITimeWnd(){

}

void CUITimeWnd::SetTextColor(u32 color){
	this->m_staticString.SetTextColor(color);
}

void CUITimeWnd::SetFont(CGameFont* pFont){
	this->m_staticString.SetFont(pFont);
}

void CUITimeWnd::SetText(LPCSTR str){
	this->m_staticString.SetText(str);
}

void CUITimeWnd::SetTextX(float x){
	this->m_staticString.SetTextX(x);
}

void CUITimeWnd::SetTextY(float y){
	this->m_staticString.SetTextY(y);
}

void CUITimeWnd::Init(float x, float y, float widht, float height){
    Init(x,y);	
}

void CUITimeWnd::Init(float x, float y){
	CUIStatic::Init(x, y, TIMER_WIDTH, TIMER_HEIGHT);
	CUIStatic::InitTexture("ui\\ui_dg_time");

	m_staticString.Init(0, 0, CLOCK_FACE_INDENT_X, TIMER_HEIGHT);	
	m_staticTime.Init(CLOCK_FACE_INDENT_X, CLOCK_FACE_INDENT_Y, TIMER_WIDTH - CLOCK_FACE_INDENT_X, TIMER_HEIGHT);	
}

void CUITimeWnd::Update(){
	this->m_staticTime.SetText(*InventoryUtilities::GetGameTimeAsString(InventoryUtilities::etpTimeToMinutes));
}

