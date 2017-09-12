// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2002.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

#include "MgcMTEdge.h"
using namespace Mgc;

//----------------------------------------------------------------------------
MTEdge::MTEdge (int iLabel)
{
    m_iLabel = iLabel;

    for (int i = 0; i < 2; i++)
    {
        m_aiVertex[i] = -1;
        m_aiTriangle[i] = -1;
    }
}
//----------------------------------------------------------------------------
MTEdge::MTEdge (const MTEdge& rkE)
{
    m_iLabel = rkE.m_iLabel;

    for (int i = 0; i < 2; i++)
    {
        m_aiVertex[i] = rkE.m_aiVertex[i];
        m_aiTriangle[i] = rkE.m_aiTriangle[i];
    }
}
//----------------------------------------------------------------------------
MTEdge::~MTEdge ()
{
}
//----------------------------------------------------------------------------
MTEdge& MTEdge::operator= (const MTEdge& rkE)
{
    m_iLabel = rkE.m_iLabel;

    for (int i = 0; i < 2; i++)
    {
        m_aiVertex[i] = rkE.m_aiVertex[i];
        m_aiTriangle[i] = rkE.m_aiTriangle[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
bool MTEdge::ReplaceVertex (int iVOld, int iVNew)
{
    for (int i = 0; i < 2; i++)
    {
        if ( m_aiVertex[i] == iVOld )
        {
            m_aiVertex[i] = iVNew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTEdge::ReplaceTriangle (int iTOld, int iTNew)
{
    for (int i = 0; i < 2; i++)
    {
        if ( m_aiTriangle[i] == iTOld )
        {
            m_aiTriangle[i] = iTNew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTEdge::operator== (const MTEdge& rkE) const
{
    return
        (m_aiVertex[0] == rkE.m_aiVertex[0] &&
         m_aiVertex[1] == rkE.m_aiVertex[1]) ||
        (m_aiVertex[0] == rkE.m_aiVertex[1] &&
         m_aiVertex[1] == rkE.m_aiVertex[0]);
}
//----------------------------------------------------------------------------
