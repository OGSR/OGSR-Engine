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

#include "MgcInterp2DBicubic.h"
using namespace Mgc;

static Real gs_aafBlend[4][4] =  // blending matrix M
{
    { 1.0f/6.0f, -3.0f/6.0f,  3.0f/6.0f, -1.0f/6.0f },
    { 4.0f/6.0f,  0.0f/6.0f, -6.0f/6.0f,  3.0f/6.0f },
    { 1.0f/6.0f,  3.0f/6.0f,  3.0f/6.0f, -3.0f/6.0f },
    { 0.0f/6.0f,  0.0f/6.0f,  0.0f/6.0f,  1.0f/6.0f }
};

//----------------------------------------------------------------------------
Interp2DBicubic::Interp2DBicubic (int iXBound, int iYBound, Real fXMin,
    Real fXSpacing, Real fYMin, Real fYSpacing, Real** aafF)
{
    // At least a 3x3 block of data points are needed to construct the
    // estimates of the boundary derivatives.
    assert( iXBound >= 3 && iYBound >= 3 && aafF );
    assert( fXSpacing > 0.0f && fYSpacing > 0.0f );

    m_iXBound = iXBound;
    m_iYBound = iYBound;
    m_iQuantity = iXBound*iYBound;

    m_fXMin = fXMin;
    m_fXSpacing = fXSpacing;
    m_fInvXSpacing = 1.0f/fXSpacing;
    m_fYMin = fYMin;
    m_fYSpacing = fYSpacing;
    m_fInvYSpacing = 1.0f/fYSpacing;
    m_fXMax = fXMin + fXSpacing*(iXBound-1);
    m_fYMax = fYMin + fYSpacing*(iYBound-1);

    m_aafF = aafF;
}
//----------------------------------------------------------------------------
Real Interp2DBicubic::operator() (Real fX, Real fY) const
{
    // compute x-index and clamp to image
    Real fXIndex = (fX - m_fXMin)*m_fInvXSpacing;
    int iX = (int)fXIndex;
    if ( iX < 0 || iX > m_iXBound - 1 )
        return Math::MAX_REAL;

    // compute y-index and clamp to image
    Real fYIndex = (fY - m_fYMin)*m_fInvYSpacing;
    int iY = (int)fYIndex;
    if ( iY < 0 || iY > m_iYBound - 1 )
        return Math::MAX_REAL;

    Real afU[4];
    afU[0] = 1.0f;
    afU[1] = fXIndex - iX;
    afU[2] = afU[1]*afU[1];
    afU[3] = afU[1]*afU[2];

    Real afV[4];
    afV[0] = 1.0f;
    afV[1] = fYIndex - iY;
    afV[2] = afV[1]*afV[1];
    afV[3] = afV[1]*afV[2];

    // compute P = M*U and Q = M*V
    Real afP[4], afQ[4];
    int iRow, iCol;
    for (iRow = 0; iRow < 4; iRow++)
    {
        afP[iRow] = 0.0f;
        afQ[iRow] = 0.0f;
        for (iCol = 0; iCol < 4; iCol++)
        {
            afP[iRow] += gs_aafBlend[iRow][iCol]*afU[iCol];
            afQ[iRow] += gs_aafBlend[iRow][iCol]*afV[iCol];
        }
    }

    // compute (M*U)^t D (M*V) where D is the 4x4 subimage containing (x,y)
    iX--;
    iY--;
    Real fResult = 0.0f;
    for (iRow = 0; iRow < 4; iRow++)
    {
        int iYClamp = iY + iRow;
        if ( iYClamp < 0 )
            iYClamp = 0;
        else if ( iYClamp > m_iYBound - 1 )
            iYClamp = m_iYBound - 1;

        for (iCol = 0; iCol < 4; iCol++)
        {
            int iXClamp = iX + iCol;
            if ( iXClamp < 0 )
                iXClamp = 0;
            else if ( iXClamp > m_iXBound - 1 )
                iXClamp = m_iXBound - 1;

            fResult += afQ[iRow]*m_aafF[iYClamp][iXClamp]*afP[iCol];
        }
    }

    return fResult;
}
//----------------------------------------------------------------------------
#if 0
// TO DO.  Implement XLookup and YLookup.
Real Interp2DBicubic::operator() (int iXOrder, int iYOrder, Real fX,
    Real fY) const
{
    int iX, iY;
    Real fDX, fDY;

    if ( XLookup(fX,iX,fDX) && YLookup(fY,iY,fDY) )
        return m_aakPoly[iY][iX](iXOrder,iYOrder,fDX,fDY);
    else
        return Math::MAX_REAL;
}
//----------------------------------------------------------------------------
#endif
//----------------------------------------------------------------------------
void Interp2DBicubic::Allocate (int iXSize, int iYSize, Real**& raafArray)
{
    raafArray = new Real*[iYSize];
    raafArray[0] = new Real[iXSize*iYSize];
    for (int iY = 1; iY < iYSize; iY++)
        raafArray[iY] = &raafArray[0][iXSize*iY];
}
//----------------------------------------------------------------------------
void Interp2DBicubic::Deallocate (Real** aafArray)
{
    delete[] aafArray[0];
    delete[] aafArray;
}
//----------------------------------------------------------------------------


