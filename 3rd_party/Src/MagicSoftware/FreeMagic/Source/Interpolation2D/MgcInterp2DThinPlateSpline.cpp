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

#include "MgcInterp2DThinPlateSpline.h"
#include "MgcLinearSystem.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp2DThinPlateSpline::Interp2DThinPlateSpline (int iQuantity, Real* afX,
    Real* afY, Real* afF, Real fSmooth)
{
    assert( iQuantity >= 3 && afX && afY && afF && fSmooth >= 0.0f );

    m_bInitialized = false;
    m_iQuantity = iQuantity;
    m_afX = new Real[m_iQuantity];
    m_afY = new Real[m_iQuantity];
    m_afA = new Real[m_iQuantity];

    int i, iRow, iCol;

    // map input (x,y) to unit square
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

    // compute matrix A = E+smooth*I [NxN matrix]
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
                Real fT = Math::Sqrt(fDx*fDx+fDy*fDy);
                aafA[iRow][iCol] = Kernel(fT);
            }
        }
    }

    // compute matrix B [Nx3 matrix]
    Real** aafB = new Real*[m_iQuantity];
    for (i = 0; i < m_iQuantity; i++)
        aafB[i] = new Real[3];
    for (iRow = 0; iRow < m_iQuantity; iRow++)
    {
        aafB[iRow][0] = 1.0f;
        aafB[iRow][1] = m_afX[iRow];
        aafB[iRow][2] = m_afY[iRow];
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

    // compute P = B^t A^{-1}  [3xN matrix]
    Real** aafP = new Real*[3];
    for (iRow = 0; iRow < 3; iRow++)
    {
        aafP[iRow] = new Real[m_iQuantity];
        for (iCol = 0; iCol < m_iQuantity; iCol++)
        {
            aafP[iRow][iCol] = 0.0f;
            for (i = 0; i < m_iQuantity; i++)
                aafP[iRow][iCol] += aafB[i][iRow]*aafA[i][iCol];
        }
    }

    // compute Q = P B = B^t A^{-1} B  [3x3 matrix]
    Real** aafQ = new Real*[3];
    for (iRow = 0; iRow < 3; iRow++)
    {
        aafQ[iRow] = new Real[3];
        for (iCol = 0; iCol < 3; iCol++)
        {
            aafQ[iRow][iCol] = 0.0f;
            for (i = 0; i < m_iQuantity; i++)
                aafQ[iRow][iCol] += aafP[iRow][i]*aafB[i][iCol];
        }
    }

    // compute Q^{-1}, result stored in-place in Q
    if ( !LinearSystem::Inverse(3,aafQ) )
    {
        for (iRow = 0; iRow < m_iQuantity; iRow++)
        {
            delete[] aafA[iRow];
            delete[] aafB[iRow];
        }

        for (iRow = 0; iRow < 3; iRow++)
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

    // compute P*z
    Real afProd[3];
    for (iRow = 0; iRow < 3; iRow++)
    {
        afProd[iRow] = 0.0f;
        for (i = 0; i < m_iQuantity; i++)
            afProd[iRow] += aafP[iRow][i]*afF[i];
    }

    // compute 'b' vector for smooth thin plate spline
    for (iRow = 0; iRow < 3; iRow++)
    {
        m_afB[iRow] = 0.0f;
        for (i = 0; i < 3; i++)
            m_afB[iRow] += aafQ[iRow][i]*afProd[i];
    }

    // compute z-B*b
    Real* afTmp = new Real[m_iQuantity];
    for (iRow = 0; iRow < m_iQuantity; iRow++)
    {
        afTmp[iRow] = afF[iRow];
        for (i = 0; i < 3; i++)
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

    for (iRow = 0; iRow < 3; iRow++)
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
Interp2DThinPlateSpline::~Interp2DThinPlateSpline ()
{
    delete[] m_afX;
    delete[] m_afY;
    delete[] m_afA;
}
//----------------------------------------------------------------------------
bool Interp2DThinPlateSpline::IsInitialized () const
{
    return m_bInitialized;
}
//----------------------------------------------------------------------------
Real Interp2DThinPlateSpline::operator() (Real fX, Real fY)
{
    // map (x,y) to the unit square
    fX = (fX - m_fXMin)*m_fXInvRange;
    fY = (fY - m_fYMin)*m_fYInvRange;

    Real fResult = m_afB[0] + m_afB[1]*fX + m_afB[2]*fY;
    for (int i = 0; i < m_iQuantity; i++)
    {
        Real fDx = fX - m_afX[i];
        Real fDy = fY - m_afY[i];
        Real fT = Math::Sqrt(fDx*fDx+fDy*fDy);
        fResult += m_afA[i]*Kernel(fT);
    }
    return fResult;
}
//----------------------------------------------------------------------------
Real Interp2DThinPlateSpline::Kernel (Real fT)
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


