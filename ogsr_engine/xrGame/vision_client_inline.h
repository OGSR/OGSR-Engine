////////////////////////////////////////////////////////////////////////////
//	Module 		: vision_client_inline.h
//	Created 	: 11.06.2007
//  Modified 	: 11.06.2007
//	Author		: Dmitriy Iassenev
//	Description : vision client inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC CVisualMemoryManager& vision_client::visual() const
{
    VERIFY(m_visual);
    return (*m_visual);
}