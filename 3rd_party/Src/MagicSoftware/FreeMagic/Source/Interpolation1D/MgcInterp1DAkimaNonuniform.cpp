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

#include "MgcInterp1DAkimaNonuniform.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp1DAkimaNonuniform::Interp1DAkimaNonuniform (int iQuantity, Real* afX,
    Real* afF)
    :
    Interp1DAkima(iQuantity,afF)
{
    m_afX = afX;

    // compute slopes
    Real* afSlope = new Real[iQuantity+3];
    int i, iP1, iP2;
    for (i = 0, iP1 = 1, iP2 = 2; i < iQuantity-1; i++, iP1++, iP2++)
    {
        Real fDX = afX[iP1] - afX[i];
        Real fDF = afF[iP1] - afF[i];
        afSlope[iP2] = fDF/fDX;
    }

    afSlope[1] = 2.0f*afSlope[2] - afSlope[3];
    afSlope[0] = 2.0f*afSlope[1] - afSlope[2];
    afSlope[iQuantity+1] = 2.0f*afSlope[iQuantity] - afSlope[iQuantity-1];
    afSlope[iQuantity+2] = 2.0f*afSlope[iQuantity+1] - afSlope[iQuantity];

    // construct derivatives
    Real* afFDer = new Real[iQuantity];
    for (i = 0; i < iQuantity; i++)
        afFDer[i] = ComputeDerivative(afSlope+i);

    // construct polynomials
    for (i = 0, iP1 = 1; i < iQuantity-1; i++, iP1++)
    {
        Polynomial& rkPoly = m_akPoly[i];

        Real fF0 = afF[i], fF1 = afF[iP1];
        Real fFDer0 = afFDer[i], fFDer1 = afFDer[iP1];
        Real fDF = fF1 - fF0;
        Real fDX = afX[iP1] - afX[i];
        Real fDX2 = fDX*fDX, fDX3 = fDX2*fDX;

        rkPoly[0] = fF0;
        rkPoly[1] = fFDer0;
        rkPoly[2] = (3.0f*fDF - fDX*(fFDer1 + 2.0f*fFDer0))/fDX2;
        rkPoly[3] = (fDX*(fFDer0 + fFDer1) - 2.0f*fDF)/fDX3;
    }

    delete[] afSlope;
    delete[] afFDer;
}
//----------------------------------------------------------------------------
Interp1DAkimaNonuniform::~Interp1DAkimaNonuniform ()
{
}
//----------------------------------------------------------------------------
bool Interp1DAkimaNonuniform::Lookup (Real fX, int& riIndex, Real& rfDX)
    const
{
    if ( fX >= m_afX[0] )
    {
        if ( fX <= m_afX[m_iQuantity-1] )
        {
            for (riIndex = 0; riIndex+1 < m_iQuantity; riIndex++)
            {
                if ( fX < m_afX[riIndex+1] )
                {
                    rfDX = fX - m_afX[riIndex];
                    return true;
                }
            }

            riIndex--;
            rfDX = fX - m_afX[riIndex];
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------


