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
inline int Interp2DAkimaUniform::GetXBound () const
{
    return m_iXBound;
}
//----------------------------------------------------------------------------
inline int Interp2DAkimaUniform::GetYBound () const
{
    return m_iYBound;
}
//----------------------------------------------------------------------------
inline int Interp2DAkimaUniform::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
inline Real** Interp2DAkimaUniform::GetF () const
{
    return m_aafF;
}
//----------------------------------------------------------------------------
inline Interp2DAkimaUniform::Polynomial**
    Interp2DAkimaUniform::GetPolynomials () const
{
    return m_aakPoly;
}
//----------------------------------------------------------------------------
inline const Interp2DAkimaUniform::Polynomial&
Interp2DAkimaUniform::GetPolynomial (int iX, int iY) const
{
    assert( iX < m_iXBound-1 && iY < m_iYBound-1 );
    return m_aakPoly[iY][iX];
}
//----------------------------------------------------------------------------
inline Real Interp2DAkimaUniform::GetXMin () const
{
    return m_fXMin;
}
//----------------------------------------------------------------------------
inline Real Interp2DAkimaUniform::GetXMax () const
{
    return m_fXMax;
}
//----------------------------------------------------------------------------
inline Real Interp2DAkimaUniform::GetXSpacing () const
{
    return m_fXSpacing;
}
//----------------------------------------------------------------------------
inline Real Interp2DAkimaUniform::GetYMin () const
{
    return m_fYMin;
}
//----------------------------------------------------------------------------
inline Real Interp2DAkimaUniform::GetYMax () const
{
    return m_fYMax;
}
//----------------------------------------------------------------------------
inline Real Interp2DAkimaUniform::GetYSpacing () const
{
    return m_fYSpacing;
}
//----------------------------------------------------------------------------


