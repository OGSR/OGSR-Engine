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
inline int Interp1DAkima::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
inline const Real* Interp1DAkima::GetF () const
{
    return m_afF;
}
//----------------------------------------------------------------------------
inline const Interp1DAkima::Polynomial* Interp1DAkima::GetPolynomials () const
{
    return m_akPoly;
}
//----------------------------------------------------------------------------
inline const Interp1DAkima::Polynomial& Interp1DAkima::GetPolynomial (int i)
    const
{
    assert( i < m_iQuantity-1 );
    return m_akPoly[i];
}
//----------------------------------------------------------------------------


