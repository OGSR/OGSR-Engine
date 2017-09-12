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

#include "MgcMTTriangle.h"
using namespace Mgc;

//----------------------------------------------------------------------------
MTTriangle::MTTriangle (int iLabel)
{
    m_iLabel = iLabel;

    for (int i = 0; i < 3; i++)
    {
        m_aiVertex[i] = -1;
        m_aiEdge[i] = -1;
        m_aiAdjacent[i] = -1;
    }
}
//----------------------------------------------------------------------------
MTTriangle::MTTriangle (const MTTriangle& rkT)
{
    m_iLabel = rkT.m_iLabel;

    for (int i = 0; i < 3; i++)
    {
        m_aiVertex[i] = rkT.m_aiVertex[i];
        m_aiEdge[i] = rkT.m_aiEdge[i];
        m_aiAdjacent[i] = rkT.m_aiAdjacent[i];
    }
}
//----------------------------------------------------------------------------
MTTriangle::~MTTriangle ()
{
}
//----------------------------------------------------------------------------
MTTriangle& MTTriangle::operator= (const MTTriangle& rkT)
{
    m_iLabel = rkT.m_iLabel;

    for (int i = 0; i < 3; i++)
    {
        m_aiVertex[i] = rkT.m_aiVertex[i];
        m_aiEdge[i] = rkT.m_aiEdge[i];
        m_aiAdjacent[i] = rkT.m_aiAdjacent[i];
    }
    return *this;
}
//----------------------------------------------------------------------------
bool MTTriangle::ReplaceVertex (int iVOld, int iVNew)
{
    for (int i = 0; i < 3; i++)
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
bool MTTriangle::ReplaceEdge (int iEOld, int iENew)
{
    for (int i = 0; i < 3; i++)
    {
        if ( m_aiEdge[i] == iEOld )
        {
            m_aiEdge[i] = iENew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTTriangle::ReplaceAdjacent (int iAOld, int iANew)
{
    for (int i = 0; i < 3; i++)
    {
        if ( m_aiAdjacent[i] == iAOld )
        {
            m_aiAdjacent[i] = iANew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTTriangle::operator== (const MTTriangle& rkT) const
{
    if ( m_aiVertex[0] == rkT.m_aiVertex[0] )
    {
        return m_aiVertex[1] == rkT.m_aiVertex[1]
            && m_aiVertex[2] == rkT.m_aiVertex[2];
    }

    if ( m_aiVertex[0] == rkT.m_aiVertex[1] )
    {
        return m_aiVertex[1] == rkT.m_aiVertex[2]
            && m_aiVertex[2] == rkT.m_aiVertex[0];
    }

    if ( m_aiVertex[0] == rkT.m_aiVertex[2] )
    {
        return m_aiVertex[1] == rkT.m_aiVertex[0]
            && m_aiVertex[2] == rkT.m_aiVertex[1];
    }

    return false;
}
//----------------------------------------------------------------------------
