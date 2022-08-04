// File:		UI_IB_Static.h
// Description:	Inheritance of UIInteractiveBackground template class with some
//				CUIStatic features
// Created:		09.02.2005
// Author:		Serge Vynnychenko
// Mail:		narrator@gsc-game.kiev.ua

// Copyright 2005 GSC Game World

#include "StdAfx.h"
#include "UI_IB_Static.h"

void CUI_IB_Static::SetTextureOffset(float x, float y)
{
    if (m_stateCurrent)
        m_stateCurrent->SetTextureOffset(x, y);

    if (m_stateEnabled)
        m_stateEnabled->SetTextureOffset(x, y);

    if (m_stateDisabled)
        m_stateDisabled->SetTextureOffset(x, y);

    if (m_stateHighlighted)
        m_stateHighlighted->SetTextureOffset(x, y);

    if (m_stateTouched)
        m_stateTouched->SetTextureOffset(x, y);
}

void CUI_IB_Static::SetStretchTexture(bool stretch_texture)
{
    if (m_stateCurrent)
        m_stateCurrent->SetStretchTexture(stretch_texture);

    if (m_stateEnabled)
        m_stateEnabled->SetStretchTexture(stretch_texture);

    if (m_stateDisabled)
        m_stateDisabled->SetStretchTexture(stretch_texture);

    if (m_stateHighlighted)
        m_stateHighlighted->SetStretchTexture(stretch_texture);

    if (m_stateTouched)
        m_stateTouched->SetStretchTexture(stretch_texture);
}

void CUI_IB_Static::EnableHeading(bool b)
{
    if (m_stateCurrent)
        m_stateCurrent->EnableHeading(b);

    if (m_stateEnabled)
        m_stateEnabled->EnableHeading(b);

    if (m_stateDisabled)
        m_stateDisabled->EnableHeading(b);

    if (m_stateHighlighted)
        m_stateHighlighted->EnableHeading(b);

    if (m_stateTouched)
        m_stateTouched->EnableHeading(b);
}