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
inline int Interp3DAkimaUniform::GetXBound () const
{
    return m_iXBound;
}
//----------------------------------------------------------------------------
inline int Interp3DAkimaUniform::GetYBound () const
{
    return m_iYBound;
}
//----------------------------------------------------------------------------
inline int Interp3DAkimaUniform::GetZBound () const
{
    return m_iZBound;
}
//----------------------------------------------------------------------------
inline int Interp3DAkimaUniform::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
inline Real*** Interp3DAkimaUniform::GetF () const
{
    return m_aaafF;
}
//----------------------------------------------------------------------------
inline Interp3DAkimaUniform::Polynomial***
Interp3DAkimaUniform::GetPolynomials () const
{
    return m_aaakPoly;
}
//----------------------------------------------------------------------------
inline const Interp3DAkimaUniform::Polynomial&
Interp3DAkimaUniform::GetPolynomial (int iX, int iY, int iZ) const
{
    assert( iX < m_iXBound-1 && iY < m_iYBound-1 && iZ < m_iZBound-1 );
    return m_aaakPoly[iZ][iY][iX];
}
//----------------------------------------------------------------------------
inline Real Interp3DAkimaUniform::GetXMin () const
{
    return m_fXMin;
}
//----------------------------------------------------------------------------
inline Real Interp3DAkimaUniform::GetXMax () const
{
    return m_fXMax;
}
//----------------------------------------------------------------------------
inline Real Interp3DAkimaUniform::GetXSpacing () const
{
    return m_fXSpacing;
}
//----------------------------------------------------------------------------
inline Real Interp3DAkimaUniform::GetYMin () const
{
    return m_fYMin;
}
//----------------------------------------------------------------------------
inline Real Interp3DAkimaUniform::GetYMax () const
{
    return m_fYMax;
}
//----------------------------------------------------------------------------
inline Real Interp3DAkimaUniform::GetYSpacing () const
{
    return m_fYSpacing;
}
//----------------------------------------------------------------------------
inline Real Interp3DAkimaUniform::GetZMin () const
{
    return m_fZMin;
}
//----------------------------------------------------------------------------
inline Real Interp3DAkimaUniform::GetZMax () const
{
    return m_fZMax;
}
//----------------------------------------------------------------------------
inline Real Interp3DAkimaUniform::GetZSpacing () const
{
    return m_fZSpacing;
}
//----------------------------------------------------------------------------


