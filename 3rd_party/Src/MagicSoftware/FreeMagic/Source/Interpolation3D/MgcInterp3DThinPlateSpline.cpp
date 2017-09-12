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

#include "MgcInterp3DThinPlateSpline.h"
#include "MgcLinearSystem.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp3DThinPlateSpline::Interp3DThinPlateSpline (int iQuantity,
    Real* afX, Real* afY, Real* afZ, Real* afF, Real fSmooth)
{
    assert( iQuantity >= 4 && afX && afY && afZ && afF && fSmooth >= 0.0f );

    m_bInitialized = false;
    m_iQuantity = iQuantity;
    m_afX = new Real[m_iQuantity];
    m_afY = new Real[m_iQuantity];
    m_afZ = new Real[m_iQuantity];
    m_afA = new Real[m_iQuantity];

    int i, iRow, iCol;

    // map input (x,y,z) to unit cube
    m_fXMin = afX[0];
    m_fXMax = m_fXMin;
    for (i = 1; i < m_iQuantity; i++)
    {
        if ( afX[i] < m_fXMin )
            m_fXMin = afX[i];
        if ( afX[i] > m_fXMax )
            m_fXMax = afX[i];
    }
    assert( m_fXMax > m_fXMin );
    m_fXInvRange = 1.0f/(m_fXMax - m_fXMin);
    for (i = 0; i < m_iQuantity; i++)
        m_afX[i] = (afX[i] - m_fXMin)*m_fXInvRange;

    m_fYMin = afY[0];
    m_fYMax = m_fYMin;
    for (i = 1; i < m_iQuantity; i++)
    {
        if ( afY[i] < m_fYMin )
            m_fYMin = afY[i];
        if ( afY[i] > m_fYMax )
            m_fYMax = afY[i];
    }
    assert( m_fYMax > m_fYMin );
    m_fYInvRange = 1.0f/(m_fYMax - m_fYMin);
    for (i = 0; i < m_iQuantity; i++)
        m_afY[i] = (afY[i] - m_fYMin)*m_fYInvRange;

    m_fZMin = afZ[0];
    m_fZMax = m_fZMin;
    for (i = 1; i < m_iQuantity; i++)
    {
        if ( afZ[i] < m_fZMin )
            m_fZMin = afZ[i];
        if ( afZ[i] > m_fZMax )
            m_fZMax = afZ[i];
    }
    assert( m_fZMax > m_fZMin );
    m_fZInvRange = 1.0f/(m_fZMax - m_fZMin);
    for (i = 0; i < m_iQuantity; i++)
        m_afZ[i] = (afZ[i] - m_fZMin)*m_fZInvRange;

    // compute matrix A = E + smooth*I [NxN matrix]
    Real** aafA = new Real*[m_iQuantity];
    for (iRow = 0; iRow < m_iQuantity; iRow++)
    {
        aafA[iRow] = new Real[m_iQuantity];
        for (iCol = 0; iCol < m_iQuantity; iCol++)
        {
            if ( iRow == iCol )
            {
                aafA[iRow][iCol] = fSmooth;
            }
            else
            {
                Real fDx = m_afX[iRow] - m_afX[iCol];
                Real fDy = m_afY[iRow] - m_afY[iCol];
                Real fDz = m_afZ[iRow] - m_afZ[iCol];
                Real fT = Math::Sqrt(fDx*fDx+fDy*fDy+fDz*fDz);
                aafA[iRow][iCol] = Kernel(fT);
            }
        }
    }

    // compute matrix B [Nx4 matrix]
    Real** aafB = new Real*[m_iQuantity];
    for (i = 0; i < m_iQuantity; i++)
        aafB[i] = new Real[4];
    for (iRow = 0; iRow < m_iQuantity; iRow++)
    {
        aafB[iRow][0] = 1.0f;
        aafB[iRow][1] = m_afX[iRow];
        aafB[iRow][2] = m_afY[iRow];
        aafB[iRow][3] = m_afZ[iRow];
    }

    // compute A^{-1}, result stored in-place in A
    if ( !LinearSystem::Inverse(m_iQuantity,aafA) )
    {
        for (iRow = 0; iRow < m_iQuantity; iRow++)
        {
            delete[] aafA[iRow];
            delete[] aafB[iRow];
        }
        delete[] aafA;
        delete[] aafB;
        return;
    }

    // compute P = B^t A^{-1} [4xN matrix]
    Real** aafP = new Real*[4];
    for (iRow = 0; iRow < 4; iRow++)
    {
        aafP[iRow] = new Real[m_iQuantity];
        for (iCol = 0; iCol < m_iQuantity; iCol++)
        {
            aafP[iRow][iCol] = 0.0f;
            for (i = 0; i < m_iQuantity; i++)
                aafP[iRow][iCol] += aafB[i][iRow]*aafA[i][iCol];
        }
    }

    // compute Q = P B = B^t A^{-1} B  [4x4 matrix]
    Real** aafQ = new Real*[4];
    for (iRow = 0; iRow < 4; iRow++)
    {
        aafQ[iRow] = new Real[4];
        for (iCol = 0; iCol < 4; iCol++)
        {
            aafQ[iRow][iCol] = 0.0f;
            for (i = 0; i < m_iQuantity; i++)
                aafQ[iRow][iCol] += aafP[iRow][i]*aafB[i][iCol];
        }
    }

    // compute Q^{-1}, result stored in-place in Q
    if ( !LinearSystem::Inverse(4,aafQ) )
    {
        for (iRow = 0; iRow < m_iQuantity; iRow++)
        {
            delete[] aafA[iRow];
            delete[] aafB[iRow];
        }

        for (iRow = 0; iRow < 4; iRow++)
        {
            delete[] aafP[iRow];
            delete[] aafQ[iRow];
        }

        delete[] aafA;
        delete[] aafB;
        delete[] aafP;
        delete[] aafQ;

        return;
    }

    // compute P*w
    Real afProd[4];
    for (iRow = 0; iRow < 4; iRow++)
    {
        afProd[iRow] = 0.0f;
        for (i = 0; i < m_iQuantity; i++)
            afProd[iRow] += aafP[iRow][i]*afF[i];
    }

    // compute 'b' vector for smooth thin plate spline
    for (iRow = 0; iRow < 4; iRow++)
    {
        m_afB[iRow] = 0.0f;
        for (i = 0; i < 4; i++)
            m_afB[iRow] += aafQ[iRow][i]*afProd[i];
    }

    // compute w-B*b
    Real* afTmp = new Real[m_iQuantity];
    for (iRow = 0; iRow < m_iQuantity; iRow++)
    {
        afTmp[iRow] = afF[iRow];
        for (i = 0; i < 4; i++)
            afTmp[iRow] -= aafB[iRow][i]*m_afB[i];
    }

    // compute 'a' vector for smooth thin plate spline
    for (iRow = 0; iRow < m_iQuantity; iRow++)
    {
        m_afA[iRow] = 0.0f;
        for (i = 0; i < m_iQuantity; i++)
            m_afA[iRow] += aafA[iRow][i]*afTmp[i];
    }
    delete[] afTmp;

    // clean up
    for (iRow = 0; iRow < m_iQuantity; iRow++)
    {
        delete[] aafA[iRow];
        delete[] aafB[iRow];
    }

    for (iRow = 0; iRow < 4; iRow++)
    {
        delete[] aafP[iRow];
        delete[] aafQ[iRow];
    }

    delete[] aafA;
    delete[] aafB;
    delete[] aafP;
    delete[] aafQ;

    m_bInitialized = true;
}
//----------------------------------------------------------------------------
Interp3DThinPlateSpline::~Interp3DThinPlateSpline ()
{
    delete[] m_afX;
    delete[] m_afY;
    delete[] m_afZ;
    delete[] m_afA;
}
//----------------------------------------------------------------------------
bool Interp3DThinPlateSpline::IsInitialized () const
{
    return m_bInitialized;
}
//----------------------------------------------------------------------------
Real Interp3DThinPlateSpline::operator() (Real fX, Real fY,
    Real fZ)
{
    // map (x,y,z) to the unit cube
    fX = (fX - m_fXMin)*m_fXInvRange;
    fY = (fY - m_fYMin)*m_fYInvRange;
    fZ = (fZ - m_fZMin)*m_fZInvRange;

    Real fResult = m_afB[0] + m_afB[1]*fX + m_afB[2]*fY + m_afB[3]*fZ;
    for (int i = 0; i < m_iQuantity; i++)
    {
        Real fDx = fX - m_afX[i];
        Real fDy = fY - m_afY[i];
        Real fDz = fZ - m_afZ[i];
        Real fT = Math::Sqrt(fDx*fDx+fDy*fDy+fDz*fDz);
        fResult += m_afA[i]*Kernel(fT);
    }
    return fResult;
}
//----------------------------------------------------------------------------
Real Interp3DThinPlateSpline::Kernel (Real fT)
{
    if ( fT > 0.0f )
    {
        Real fT2 = fT*fT;
        return fT2*Math::Log(fT2);
    }
    else
    {
        return 0.0f;
    }
}
//----------------------------------------------------------------------------


