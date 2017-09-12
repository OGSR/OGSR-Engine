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

#include "MgcInterp1DAkima.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp1DAkima::Interp1DAkima (int iQuantity, Real* afF)
{
    // At least three data points are needed to construct the estimates of
    // the boundary derivatives.
    assert( iQuantity >= 3 && afF );

    m_iQuantity = iQuantity;
    m_afF = afF;
    m_akPoly = new Polynomial[iQuantity-1];
}
//----------------------------------------------------------------------------
Interp1DAkima::~Interp1DAkima ()
{
    delete[] m_akPoly;
}
//----------------------------------------------------------------------------
Real Interp1DAkima::ComputeDerivative (Real* afSlope) const
{
    if ( afSlope[1] != afSlope[2] )
    {
        if ( afSlope[0] != afSlope[1] )
        {
            if ( afSlope[2] != afSlope[3] )
            {
                Real fAD0 = Math::FAbs(afSlope[3] - afSlope[2]);
                Real fAD1 = Math::FAbs(afSlope[0] - afSlope[1]);
                return (fAD0*afSlope[1]+fAD1*afSlope[2])/(fAD0+fAD1);
            }
            else
            {
                return afSlope[2];
            }
        }
        else
        {
            if ( afSlope[2] != afSlope[3] )
            {
                return afSlope[1];
            }
            else
            {
                return 0.5f*(afSlope[1]+afSlope[2]);
            }
        }
    }
    else
    {
        return afSlope[1];
    }
}
//----------------------------------------------------------------------------
Real Interp1DAkima::operator() (Real fX) const
{
    int iIndex;
    Real fDX;

    if ( Lookup(fX,iIndex,fDX) )
        return m_akPoly[iIndex](fDX);
    else
        return Math::MAX_REAL;
}
//----------------------------------------------------------------------------
Real Interp1DAkima::operator() (int iOrder, Real fX) const
{
    int iIndex;
    Real fDX;

    if ( Lookup(fX,iIndex,fDX) )
        return m_akPoly[iIndex](iOrder,fDX);
    else
        return Math::MAX_REAL;
}
//----------------------------------------------------------------------------


