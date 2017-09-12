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

#include "MgcInterp2DAkimaUniform.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp2DAkimaUniform::Interp2DAkimaUniform (int iXBound, int iYBound,
    Real fXMin, Real fXSpacing, Real fYMin, Real fYSpacing, Real** aafF)
{
    // At least a 3x3 block of data points are needed to construct the
    // estimates of the boundary derivatives.
    assert( iXBound >= 3 && iYBound >= 3 && aafF );
    assert( fXSpacing > 0.0f && fYSpacing > 0.0f );

    m_iXBound = iXBound;
    m_iYBound = iYBound;
    m_fXMin = fXMin;
    m_fXSpacing = fXSpacing;
    m_fYMin = fYMin;
    m_fYSpacing = fYSpacing;
    m_aafF = aafF;

    int iXBoundM1 = iXBound - 1, iYBoundM1 = iYBound - 1;
    m_iQuantity = iXBound*iYBound;
    m_fXMax = fXMin + fXSpacing*iXBoundM1;
    m_fYMax = fYMin + fYSpacing*iYBoundM1;

    // compute slopes
    Real fInvDX = 1.0f/fXSpacing, fInvDY = 1.0f/fYSpacing;
    Real fInvDXDY = fInvDX*fInvDY;
    Real** aafXSlope;  // xslope[y][x]
    Real** aafYSlope;  // yslope[x][y]
    Allocate(iXBound+3,iYBound,aafXSlope);
    Allocate(iYBound+3,iXBound,aafYSlope);
    int iX, iY;

    for (iY = 0; iY < iYBound; iY++)
    {
        for (iX = 0; iX < iXBoundM1; iX++)
            aafXSlope[iY][iX+2] = (aafF[iY][iX+1]-aafF[iY][iX])*fInvDX;

        aafXSlope[iY][1] = 2.0f*aafXSlope[iY][2] - aafXSlope[iY][3];
        aafXSlope[iY][0] = 2.0f*aafXSlope[iY][1] - aafXSlope[iY][2];
        aafXSlope[iY][iXBound+1] = 2.0f*aafXSlope[iY][iXBound] -
            aafXSlope[iY][iXBound-1];
        aafXSlope[iY][iXBound+2] = 2.0f*aafXSlope[iY][iXBound+1] -
            aafXSlope[iY][iXBound];
    }

    for (iX = 0; iX < iXBound; iX++)
    {
        for (iY = 0; iY < iYBoundM1; iY++)
            aafYSlope[iX][iY+2] = (aafF[iY+1][iX]-aafF[iY][iX])*fInvDY;

        aafYSlope[iX][1] = 2.0f*aafYSlope[iX][2] - aafYSlope[iX][3];
        aafYSlope[iX][0] = 2.0f*aafYSlope[iX][1] - aafYSlope[iX][2];
        aafYSlope[iX][iYBound+1] = 2.0f*aafYSlope[iX][iYBound] -
            aafYSlope[iX][iYBound-1];
        aafYSlope[iX][iYBound+2] = 2.0f*aafYSlope[iX][iYBound+1] -
            aafYSlope[iX][iYBound];
    }

    // construct first-order derivatives
    Real** aafFX;
    Real** aafFY;
    Allocate(iXBound,iYBound,aafFX);
    Allocate(iXBound,iYBound,aafFY);

    for (iY = 0; iY < iYBound; iY++)
    {
        for (iX = 0; iX < iXBound; iX++)
            aafFX[iY][iX] = ComputeDerivative(aafXSlope[iY]+iX);
    }

    for (iX = 0; iX < iXBound; iX++)
    {
        for (iY = 0; iY < iYBound; iY++)
            aafFY[iY][iX] = ComputeDerivative(aafYSlope[iX]+iY);
    }

    // construct second-order derivatives
    Real** aafFXY;
    Allocate(iXBound,iYBound,aafFXY);

    unsigned int iX0 = iXBoundM1, iX1 = iX0-1,  iX2 = iX1-1;
    unsigned int iY0 = iYBoundM1, iY1 = iY0-1,  iY2 = iY1-1;

    // corners
    aafFXY[0][0] = 0.25f*fInvDXDY*(
          9.0f*aafF[0][0] - 12.0f*aafF[0][1] + 3.0f*aafF[0][2]
        -12.0f*aafF[1][0] + 16.0f*aafF[1][1] - 4.0f*aafF[1][2]
         +3.0f*aafF[2][0] -  4.0f*aafF[2][1] +      aafF[2][2]);

    aafFXY[0][iXBoundM1] = 0.25f*fInvDXDY*(
          9.0f*aafF[0][iX0] - 12.0f*aafF[0][iX1] + 3.0f*aafF[0][iX2]
        -12.0f*aafF[1][iX0] + 16.0f*aafF[1][iX1] - 4.0f*aafF[1][iX2]
         +3.0f*aafF[2][iX0] -  4.0f*aafF[2][iX1] +      aafF[2][iX2]);

    aafFXY[iYBoundM1][0] = 0.25f*fInvDXDY*(
          9.0f*aafF[iY0][0] - 12.0f*aafF[iY0][1] + 3.0f*aafF[iY0][2]
        -12.0f*aafF[iY1][0] + 16.0f*aafF[iY1][1] - 4.0f*aafF[iY1][2]
         +3.0f*aafF[iY2][0] -  4.0f*aafF[iY2][1] +      aafF[iY2][2]);

    aafFXY[iYBoundM1][iXBoundM1] = 0.25f*fInvDXDY*(
          9.0f*aafF[iY0][iX0] - 12.0f*aafF[iY0][iX1] + 3.0f*aafF[iY0][iX2]
        -12.0f*aafF[iY1][iX0] + 16.0f*aafF[iY1][iX1] - 4.0f*aafF[iY1][iX2]
         +3.0f*aafF[iY2][iX0] -  4.0f*aafF[iY2][iX1] +      aafF[iY2][iX2]);

    // x-edges
    for (iX = 1; iX < iXBoundM1; iX++)
    {
        aafFXY[0][iX] = 0.25f*fInvDXDY*(
            3.0f*(aafF[0][iX-1] - aafF[0][iX+1])
           -4.0f*(aafF[1][iX-1] - aafF[1][iX+1])
           +     (aafF[2][iX-1] - aafF[2][iX+1]));

        aafFXY[iYBoundM1][iX] = 0.25f*fInvDXDY*(
            3.0f*(aafF[iY0][iX-1] - aafF[iY0][iX+1])
           -4.0f*(aafF[iY1][iX-1] - aafF[iY1][iX+1])
           +     (aafF[iY2][iX-1] - aafF[iY2][iX+1]));
    }

    // y-edges
    for (iY = 1; iY < iYBoundM1; iY++)
    {
        aafFXY[iY][0] = 0.25f*fInvDXDY*(
            3.0f*(aafF[iY-1][0] - aafF[iY+1][0])
           -4.0f*(aafF[iY-1][1] - aafF[iY+1][1])
           +     (aafF[iY-1][2] - aafF[iY+1][2]));

        aafFXY[iY][iXBoundM1] = 0.25f*fInvDXDY*(
            3.0f*(aafF[iY-1][iX0] - aafF[iY+1][iX0])
           -4.0f*(aafF[iY-1][iX1] - aafF[iY+1][iX1])
           +     (aafF[iY-1][iX2] - aafF[iY+1][iX2]));
    }

    // interior
    for (iY = 1; iY < iYBoundM1; iY++)
    {
        for (iX = 1; iX < iXBoundM1; iX++)
        {
            aafFXY[iY][iX] = 0.25f*fInvDXDY*(aafF[iY-1][iX-1] -
                aafF[iY-1][iX+1] - aafF[iY+1][iX-1] + aafF[iY+1][iX+1]);
        }
    }

    // construct polynomials
    Allocate(iXBoundM1,iYBoundM1,m_aakPoly);
    for (iY = 0; iY < iYBoundM1; iY++)
    {
        for (iX = 0; iX < iXBoundM1; iX++)
        {
            // Note the 'transposing' of the 2x2 blocks (to match notation
            // used in the polynomial definition).
            Real aafG[2][2] =
            {
                aafF[iY][iX], aafF[iY+1][iX],
                aafF[iY][iX+1], aafF[iY+1][iX+1]
            };

            Real aafGX[2][2] =
            {
                aafFX[iY][iX], aafFX[iY+1][iX],
                aafFX[iY][iX+1], aafFX[iY+1][iX+1]
            };

            Real aafGY[2][2] =
            {
                aafFY[iY][iX], aafFY[iY+1][iX],
                aafFY[iY][iX+1], aafFY[iY+1][iX+1]
            };

            Real aafGXY[2][2] =
            {
                aafFXY[iY][iX], aafFXY[iY+1][iX],
                aafFXY[iY][iX+1], aafFXY[iY+1][iX+1]
            };

            Construct(m_aakPoly[iY][iX],aafG,aafGX,aafGY,aafGXY);
        }
    }

    Deallocate(aafXSlope);
    Deallocate(aafYSlope);
    Deallocate(aafFX);
    Deallocate(aafFY);
    Deallocate(aafFXY);
}
//----------------------------------------------------------------------------
Interp2DAkimaUniform::~Interp2DAkimaUniform ()
{
    Deallocate(m_aakPoly);
}
//----------------------------------------------------------------------------
Real Interp2DAkimaUniform::ComputeDerivative (Real* afSlope) const
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
void Interp2DAkimaUniform::Construct (Polynomial& rkPoly, Real aafF[2][2],
    Real aafFX[2][2], Real aafFY[2][2], Real aafFXY[2][2])
{
    Real fDX = m_fXSpacing;
    Real fDY = m_fYSpacing;
    Real fInvDX = 1.0f/fDX, fInvDX2 = fInvDX*fInvDX;
    Real fInvDY = 1.0f/fDY, fInvDY2 = fInvDY*fInvDY;
    Real fB0, fB1, fB2, fB3;

    rkPoly.A(0,0) = aafF[0][0];
    rkPoly.A(1,0) = aafFX[0][0];
    rkPoly.A(0,1) = aafFY[0][0];
    rkPoly.A(1,1) = aafFXY[0][0];

    fB0 = (aafF[1][0] - rkPoly(0,0,fDX,0.0f))*fInvDX2;
    fB1 = (aafFX[1][0] - rkPoly(1,0,fDX,0.0f))*fInvDX;
    rkPoly.A(2,0) = 3.0f*fB0 - fB1;
    rkPoly.A(3,0) = (-2.0f*fB0 + fB1)*fInvDX;

    fB0 = (aafF[0][1] - rkPoly(0,0,0.0f,fDY))*fInvDY2;
    fB1 = (aafFY[0][1] - rkPoly(0,1,0.0f,fDY))*fInvDY;
    rkPoly.A(0,2) = 3.0f*fB0 - fB1;
    rkPoly.A(0,3) = (-2.0f*fB0 + fB1)*fInvDY;

    fB0 = (aafFY[1][0] - rkPoly(0,1,fDX,0.0f))*fInvDX2;
    fB1 = (aafFXY[1][0] - rkPoly(1,1,fDX,0.0f))*fInvDX;
    rkPoly.A(2,1) = 3.0f*fB0 - fB1;
    rkPoly.A(3,1) = (-2.0f*fB0 + fB1)*fInvDX;

    fB0 = (aafFX[0][1] - rkPoly(1,0,0.0f,fDY))*fInvDY2;
    fB1 = (aafFXY[0][1] - rkPoly(1,1,0.0f,fDY))*fInvDY;
    rkPoly.A(1,2) = 3.0f*fB0 - fB1;
    rkPoly.A(1,3) = (-2.0f*fB0 + fB1)*fInvDY;

    fB0 = (aafF[1][1] - rkPoly(0,0,fDX,fDY))*fInvDX2*fInvDY2;
    fB1 = (aafFX[1][1] - rkPoly(1,0,fDX,fDY))*fInvDX*fInvDY2;
    fB2 = (aafFY[1][1] - rkPoly(0,1,fDX,fDY))*fInvDX2*fInvDY;
    fB3 = (aafFXY[1][1] - rkPoly(1,1,fDX,fDY))*fInvDX*fInvDY;
    rkPoly.A(2,2) = 9.0f*fB0 - 3.0f*fB1 - 3.0f*fB2 + fB3;
    rkPoly.A(3,2) = (-6.0f*fB0 + 3.0f*fB1 + 2.0f*fB2 - fB3)*fInvDX;
    rkPoly.A(2,3) = (-6.0f*fB0 + 2.0f*fB1 + 3.0f*fB2 - fB3)*fInvDY;
    rkPoly.A(3,3) = (4.0f*fB0 - 2.0f*fB1 - 2.0f*fB2 + fB3)*fInvDX*fInvDY;
}
//----------------------------------------------------------------------------
bool Interp2DAkimaUniform::XLookup (Real fX, int& riXIndex, Real& rfDX)
    const
{
    if ( fX >= m_fXMin )
    {
        if ( fX <= m_fXMax )
        {
            for (riXIndex = 0; riXIndex+1 < m_iXBound; riXIndex++)
            {
                if ( fX < m_fXMin + m_fXSpacing*(riXIndex+1) )
                {
                    rfDX = fX - (m_fXMin + m_fXSpacing*riXIndex);
                    return true;
                }
            }

            riXIndex--;
            rfDX = fX - (m_fXMin + m_fXSpacing*riXIndex);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool Interp2DAkimaUniform::YLookup (Real fY, int& riYIndex, Real& rfDY)
    const
{
    if ( fY >= m_fYMin )
    {
        if ( fY <= m_fYMax )
        {
            for (riYIndex = 0; riYIndex+1 < m_iYBound; riYIndex++)
            {
                if ( fY < m_fYMin + m_fYSpacing*(riYIndex+1) )
                {
                    rfDY = fY - (m_fYMin + m_fYSpacing*riYIndex);
                    return true;
                }
            }

            riYIndex--;
            rfDY = fY - (m_fYMin + m_fYSpacing*riYIndex);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
Real Interp2DAkimaUniform::operator() (Real fX, Real fY) const
{
    int iX, iY;
    Real fDX, fDY;

    if ( XLookup(fX,iX,fDX) && YLookup(fY,iY,fDY) )
        return m_aakPoly[iY][iX](fDX,fDY);
    else
        return Math::MAX_REAL;
}
//----------------------------------------------------------------------------
Real Interp2DAkimaUniform::operator() (int iXOrder, int iYOrder, Real fX,
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
void Interp2DAkimaUniform::Allocate (int iXSize, int iYSize,
    Real**& raafArray)
{
    raafArray = new Real*[iYSize];
    raafArray[0] = new Real[iXSize*iYSize];
    for (int iY = 1; iY < iYSize; iY++)
        raafArray[iY] = &raafArray[0][iXSize*iY];
}
//----------------------------------------------------------------------------
void Interp2DAkimaUniform::Deallocate (Real** aafArray)
{
    delete[] aafArray[0];
    delete[] aafArray;
}
//----------------------------------------------------------------------------
void Interp2DAkimaUniform::Allocate (int iXSize, int iYSize,
    Polynomial**& raakArray)
{
    raakArray = new Polynomial*[iYSize];
    raakArray[0] = new Polynomial[iXSize*iYSize];
    for (int iY = 1; iY < iYSize; iY++)
        raakArray[iY] = &raakArray[0][iXSize*iY];
}
//----------------------------------------------------------------------------
void Interp2DAkimaUniform::Deallocate (Polynomial** aakArray)
{
    delete[] aakArray[0];
    delete[] aakArray;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// polynomial members
//----------------------------------------------------------------------------
Interp2DAkimaUniform::Polynomial::Polynomial ()
{
    memset(m_aafA[0],0,16*sizeof(Real));
}
//----------------------------------------------------------------------------
Real Interp2DAkimaUniform::Polynomial::operator() (Real fX, Real fY) const
{
    Real afB[4];
    for (int i = 0; i < 4; i++)
    {
        afB[i] = m_aafA[i][0] + fY*(m_aafA[i][1] + fY*(m_aafA[i][2] +
            fY*m_aafA[i][3]));
    }

    return afB[0] + fX*(afB[1] + fX*(afB[2] + fX*afB[3]));
}
//----------------------------------------------------------------------------
Real Interp2DAkimaUniform::Polynomial::operator() (int iXOrder, int iYOrder,
    Real fX, Real fY) const
{
    // TO DO.  optimize this function
    Real afXPow[4], afYPow[4];

    switch ( iXOrder )
    {
    case 0:
        afXPow[0] = 1.0f;
        afXPow[1] = fX;
        afXPow[2] = fX*fX;
        afXPow[3] = fX*fX*fX;
        break;
    case 1:
        afXPow[0] = 0.0f;
        afXPow[1] = 1.0f;
        afXPow[2] = 2.0f*fX;
        afXPow[3] = 3.0f*fX*fX;
        break;
    case 2:
        afXPow[0] = 0.0f;
        afXPow[1] = 0.0f;
        afXPow[2] = 2.0f;
        afXPow[3] = 6.0f*fX;
        break;
    case 3:
        afXPow[0] = 0.0f;
        afXPow[1] = 0.0f;
        afXPow[2] = 0.0f;
        afXPow[3] = 6.0f;
        break;
    default:
        return 0.0f;
    }

    switch ( iYOrder )
    {
    case 0:
        afYPow[0] = 1.0f;
        afYPow[1] = fY;
        afYPow[2] = fY*fY;
        afYPow[3] = fY*fY*fY;
        break;
    case 1:
        afYPow[0] = 0.0f;
        afYPow[1] = 1.0f;
        afYPow[2] = 2.0f*fY;
        afYPow[3] = 3.0f*fY*fY;
        break;
    case 2:
        afYPow[0] = 0.0f;
        afYPow[1] = 0.0f;
        afYPow[2] = 2.0f;
        afYPow[3] = 6.0f*fY;
        break;
    case 3:
        afYPow[0] = 0.0f;
        afYPow[1] = 0.0f;
        afYPow[2] = 0.0f;
        afYPow[3] = 6.0f;
        break;
    default:
        return 0.0f;
    }

    Real fP = 0.0f;

    for (int iY = 0; iY <= 3; iY++)
    {
        for (int iX = 0; iX <= 3; iX++)
            fP += m_aafA[iX][iY]*afXPow[iX]*afYPow[iY];
    }

    return fP;
}
//----------------------------------------------------------------------------


