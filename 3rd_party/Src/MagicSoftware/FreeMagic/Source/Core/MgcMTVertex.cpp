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

#include "MgcMTVertex.h"
using namespace Mgc;

//----------------------------------------------------------------------------
MTVertex::MTVertex (int iLabel, int iEGrow, int iTGrow)
    :
    m_kESet(iEGrow,iEGrow),
    m_kTSet(iTGrow,iTGrow)
{
    m_iLabel = iLabel;
}
//----------------------------------------------------------------------------
MTVertex::MTVertex (const MTVertex& rkV)
    :
    m_kESet(rkV.m_kESet),
    m_kTSet(rkV.m_kTSet)
{
    m_iLabel = rkV.m_iLabel;
}
//----------------------------------------------------------------------------
MTVertex::~MTVertex ()
{
}
//----------------------------------------------------------------------------
MTVertex& MTVertex::operator= (const MTVertex& rkV)
{
    m_kESet = rkV.m_kESet;
    m_kTSet = rkV.m_kTSet;
    m_iLabel = rkV.m_iLabel;
    return *this;
}
//----------------------------------------------------------------------------
bool MTVertex::InsertEdge (int iE)
{
    return m_kESet.Insert(iE);
}
//----------------------------------------------------------------------------
bool MTVertex::RemoveEdge (int iE)
{
    return m_kESet.Remove(iE);
}
//----------------------------------------------------------------------------
bool MTVertex::ReplaceEdge (int iEOld, int iENew)
{
    for (int i = 0; i < m_kESet.GetQuantity(); i++)
    {
        if ( m_kESet[i] == iEOld )
        {
            m_kESet[i] = iENew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTVertex::InsertTriangle (int iT)
{
    return m_kTSet.Insert(iT);
}
//----------------------------------------------------------------------------
bool MTVertex::RemoveTriangle (int iT)
{
    return m_kTSet.Remove(iT);
}
//----------------------------------------------------------------------------
bool MTVertex::ReplaceTriangle (int iTOld, int iTNew)
{
    for (int i = 0; i < m_kTSet.GetQuantity(); i++)
    {
        if ( m_kTSet[i] == iTOld )
        {
            m_kTSet[i] = iTNew;
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool MTVertex::operator== (const MTVertex& rkV) const
{
    return m_iLabel == rkV.m_iLabel;
}
//----------------------------------------------------------------------------
