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

#include <cassert>
#include "Polyline3.h"
#include "VertexCollapse.h"

//----------------------------------------------------------------------------
Polyline3::Polyline3 (int iVQuantity, Vector3* akVertex, bool bClosed)
{
    assert( akVertex  && (bClosed ? iVQuantity >= 3 : iVQuantity >= 2) );

    m_iVQuantity = iVQuantity;
    m_akVertex = akVertex;
    m_bClosed = bClosed;

    // compute the vertex collapses
    VertexCollapse(m_iVQuantity,m_akVertex,m_bClosed,m_aiMap,m_iEQuantity,
        m_aiEdge);

    // polyline initially at full level of detail
    m_iVMin = ( m_bClosed ? 3 : 2 );
    m_iVMax = m_iVQuantity;
}
//----------------------------------------------------------------------------
Polyline3::~Polyline3 ()
{
    delete[] m_akVertex;
    delete[] m_aiEdge;
    delete[] m_aiMap;
}
//----------------------------------------------------------------------------
void Polyline3::SetLevelOfDetail (int iVQuantity)
{
    if ( iVQuantity < m_iVMin || iVQuantity > m_iVMax )
        return;

    // decrease level of detail
    while ( m_iVQuantity > iVQuantity )
    {
        m_iVQuantity--;
        m_aiEdge[m_aiMap[m_iVQuantity]] = m_aiEdge[2*m_iEQuantity-1];
        m_iEQuantity--;
    }

    // increase level of detail
    while ( m_iVQuantity < iVQuantity )
    {
        m_iEQuantity++;
        m_aiEdge[m_aiMap[m_iVQuantity]] = m_iVQuantity;
        m_iVQuantity++;
    }
}
//----------------------------------------------------------------------------


