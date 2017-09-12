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

//----------------------------------------------------------------------------
inline ConvexClipper::Vertex::Vertex ()
{
    // m_kPoint and m_iOccurs are uninitialized
    m_fDistance = 0.0f;
    m_bVisible = true;
}
//----------------------------------------------------------------------------
inline ConvexClipper::Edge::Edge ()
{
    // m_aiVertex and m_aiFace are uninitialized
    m_bVisible = true;
}
//----------------------------------------------------------------------------
inline ConvexClipper::Face::Face ()
{
    // m_kPlane is uninitialized, m_akEdge is empty
    m_bVisible = true;
}
//----------------------------------------------------------------------------
inline ConvexClipper::EdgePlus::EdgePlus ()
{
}
//----------------------------------------------------------------------------
inline ConvexClipper::EdgePlus::EdgePlus (int iE, const Edge& rkE)
{
    m_iE = iE;
    m_iF0 = rkE.m_aiFace[0];
    m_iF1 = rkE.m_aiFace[1];

    if ( rkE.m_aiVertex[0] < rkE.m_aiVertex[1] )
    {
        m_iV0 = rkE.m_aiVertex[0];
        m_iV1 = rkE.m_aiVertex[1];
    }
    else
    {
        m_iV0 = rkE.m_aiVertex[1];
        m_iV1 = rkE.m_aiVertex[0];
    }
}
//----------------------------------------------------------------------------
inline bool ConvexClipper::EdgePlus::operator< (const EdgePlus& rkE) const
{
    if ( m_iV1 < rkE.m_iV1 )
        return true;

    if ( m_iV1 == rkE.m_iV1 )
        return m_iV0 < rkE.m_iV0;

    return false;
}
//----------------------------------------------------------------------------
inline bool ConvexClipper::EdgePlus::operator== (const EdgePlus& rkE) const
{
    return m_iV0 == rkE.m_iV0 && m_iV1 == rkE.m_iV1;
}
//----------------------------------------------------------------------------
inline bool ConvexClipper::EdgePlus::operator!= (const EdgePlus& rkE) const
{
    return m_iV0 != rkE.m_iV0 || m_iV1 != rkE.m_iV1;
}
//----------------------------------------------------------------------------
