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

#include "MgcInterp1DAkimaUniform.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp1DAkimaUniform::Interp1DAkimaUniform (int iQuantity, Real fXMin,
    Real fXSpacing, Real* afF)
    :
    Interp1DAkima(iQuantity,afF)
{
    assert( fXSpacing > 0.0f );

    m_fXMin = fXMin;
    m_fXMax = fXMin + fXSpacing*(iQuantity-1);
    m_fXSpacing = fXSpacing;

    // compute slopes
    Real fInvDX = 1.0f/fXSpacing;
    Real* afSlope = new Real[iQuantity+3];
    int i, iP1, iP2;
    for (i = 0, iP1 = 1, iP2 = 2; i < iQuantity-1; i++, iP1++, iP2++)
        afSlope[iP2] = (afF[iP1] - afF[i])*fInvDX;

    afSlope[1] = 2.0f*afSlope[2] - afSlope[3];
    afSlope[0] = 2.0f*afSlope[1] - afSlope[2];
    afSlope[iQuantity+1] = 2.0f*afSlope[iQuantity] - afSlope[iQuantity-1];
    afSlope[iQuantity+2] = 2.0f*afSlope[iQuantity+1] - afSlope[iQuantity];

    // construct derivatives
    Real* afFDer = new Real[iQuantity];
    for (i = 0; i < iQuantity; i++)
        afFDer[i] = ComputeDerivative(afSlope+i);

    // construct polynomials
    Real fInvDX2 = 1.0f/(fXSpacing*fXSpacing);
    Real fInvDX3 = fInvDX2/fXSpacing;
    for (i = 0, iP1 = 1; i < iQuantity-1; i++, iP1++)
    {
        Polynomial& rkPoly = m_akPoly[i];

        Real fF0 = afF[i], fF1 = afF[iP1], fDF = fF1 - fF0;
        Real fFDer0 = afFDer[i], fFDer1 = afFDer[iP1];

        rkPoly[0] = fF0;
        rkPoly[1] = fFDer0;
        rkPoly[2] = (3.0f*fDF-fXSpacing*(fFDer1+2.0f*fFDer0))*fInvDX2;
        rkPoly[3] = (fXSpacing*(fFDer0+fFDer1)-2.0f*fDF)*fInvDX3;
    }

    delete[] afSlope;
    delete[] afFDer;
}
//----------------------------------------------------------------------------
Interp1DAkimaUniform::~Interp1DAkimaUniform ()
{
}
//----------------------------------------------------------------------------
bool Interp1DAkimaUniform::Lookup (Real fX, int& riIndex, Real& rfDX) const
{
    if ( fX >= m_fXMin )
    {
        if ( fX <= m_fXMax )
        {
            for (riIndex = 0; riIndex+1 < m_iQuantity; riIndex++)
            {
                if ( fX < m_fXMin + m_fXSpacing*(riIndex+1) )
                {
                    rfDX = fX - (m_fXMin + m_fXSpacing*riIndex);
                    return true;
                }
            }

            riIndex--;
            rfDX = fX - (m_fXMin + m_fXSpacing*riIndex);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------


