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
inline bool ImageInterp3D::Vertex::operator== (const Vertex& rkV) const
{
    return ( m_iXNumer*rkV.m_iXDenom == m_iXDenom*rkV.m_iXNumer )
        && ( m_iYNumer*rkV.m_iYDenom == m_iYDenom*rkV.m_iYNumer )
        && ( m_iZNumer*rkV.m_iZDenom == m_iZDenom*rkV.m_iZNumer );
}
//----------------------------------------------------------------------------
inline bool ImageInterp3D::Vertex::operator!= (const Vertex& rkV) const
{
    return !operator==(rkV);
}
//----------------------------------------------------------------------------
inline bool ImageInterp3D::Edge::operator== (const Edge& rkE) const
{
    return m_i0 == rkE.m_i0 && m_i1 == rkE.m_i1;
}
//----------------------------------------------------------------------------
inline bool ImageInterp3D::Edge::operator!= (const Edge& rkE) const
{
    return !operator==(rkE);
}
//----------------------------------------------------------------------------
inline bool ImageInterp3D::Triangle::operator== (const Triangle& rkT) const
{
    return (m_i0 == rkT.m_i0) &&
          ((m_i1 == rkT.m_i1 && m_i2 == rkT.m_i2) ||
           (m_i1 == rkT.m_i2 && m_i2 == rkT.m_i1));
}
//----------------------------------------------------------------------------
inline bool ImageInterp3D::Triangle::operator!= (const Triangle& rkT) const
{
    return !operator==(rkT);
}
//----------------------------------------------------------------------------


