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
// http://www.magic-software.com/License.html/free.pdf

//----------------------------------------------------------------------------
inline ImageInterp2D::Vertex::Vertex (int iXNumer, int iXDenom, int iYNumer,
    int iYDenom)
{
    if ( iXDenom > 0 )
    {
        m_iXNumer = iXNumer;
        m_iXDenom = iXDenom;
    }
    else
    {
        m_iXNumer = -iXNumer;
        m_iXDenom = -iXDenom;
    }

    if ( iYDenom > 0 )
    {
        m_iYNumer = iYNumer;
        m_iYDenom = iYDenom;
    }
    else
    {
        m_iYNumer = -iYNumer;
        m_iYDenom = -iYDenom;
    }
}
//----------------------------------------------------------------------------
inline void ImageInterp2D::Vertex::GetPair (float& rfX, float& rfY) const
{
    rfX = float(m_iXNumer)/float(m_iXDenom);
    rfY = float(m_iYNumer)/float(m_iYDenom);
}
//----------------------------------------------------------------------------
inline void ImageInterp2D::Vertex::GetPair (double& rdX, double& rdY) const
{
    rdX = double(m_iXNumer)/double(m_iXDenom);
    rdY = double(m_iYNumer)/double(m_iYDenom);
}
//----------------------------------------------------------------------------
inline ImageInterp2D::Edge::Edge (int i0, int i1)
{
    m_i0 = i0;
    m_i1 = i1;
}
//----------------------------------------------------------------------------


