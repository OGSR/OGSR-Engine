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

#include "MgcMatrix2.h"
using namespace Mgc;

const Real Matrix2::EPSILON = 1e-06f;
const Matrix2 Matrix2::ZERO(0.0f,0.0f,0.0f,0.0f);
const Matrix2 Matrix2::IDENTITY(1.0f,0.0f,0.0f,1.0f);

//----------------------------------------------------------------------------
Matrix2::Matrix2 (const Real aafEntry[2][2])
{
    memcpy(m_aafEntry,aafEntry,4*sizeof(Real));
}
//----------------------------------------------------------------------------
Matrix2::Matrix2 (const Matrix2& rkMatrix)
{
    memcpy(m_aafEntry,rkMatrix.m_aafEntry,4*sizeof(Real));
}
//----------------------------------------------------------------------------
Matrix2::Matrix2 (Real fEntry00, Real fEntry01, Real fEntry10, Real fEntry11)
{
    m_aafEntry[0][0] = fEntry00;
    m_aafEntry[0][1] = fEntry01;
    m_aafEntry[1][0] = fEntry10;
    m_aafEntry[1][1] = fEntry11;
}
//----------------------------------------------------------------------------
Vector2 Matrix2::GetColumn (int iCol) const
{
    assert( 0 <= iCol && iCol < 2 );
    return Vector2(m_aafEntry[0][iCol],m_aafEntry[1][iCol]);
}
//----------------------------------------------------------------------------
Matrix2& Matrix2::operator= (const Matrix2& rkMatrix)
{
    memcpy(m_aafEntry,rkMatrix.m_aafEntry,4*sizeof(Real));
    return *this;
}
//----------------------------------------------------------------------------
bool Matrix2::operator== (const Matrix2& rkMatrix) const
{
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            if ( m_aafEntry[iRow][iCol] != rkMatrix.m_aafEntry[iRow][iCol] )
                return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool Matrix2::operator!= (const Matrix2& rkMatrix) const
{
    return !operator==(rkMatrix);
}
//----------------------------------------------------------------------------
Matrix2 Matrix2::operator+ (const Matrix2& rkMatrix) const
{
    Matrix2 kSum;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            kSum.m_aafEntry[iRow][iCol] = m_aafEntry[iRow][iCol] +
                rkMatrix.m_aafEntry[iRow][iCol];
        }
    }
    return kSum;
}
//----------------------------------------------------------------------------
Matrix2 Matrix2::operator- (const Matrix2& rkMatrix) const
{
    Matrix2 kDiff;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            kDiff.m_aafEntry[iRow][iCol] = m_aafEntry[iRow][iCol] -
                rkMatrix.m_aafEntry[iRow][iCol];
        }
    }
    return kDiff;
}
//----------------------------------------------------------------------------
Matrix2 Matrix2::operator* (const Matrix2& rkMatrix) const
{
    Matrix2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            kProd.m_aafEntry[iRow][iCol] =
                m_aafEntry[iRow][0]*rkMatrix.m_aafEntry[0][iCol] +
                m_aafEntry[iRow][1]*rkMatrix.m_aafEntry[1][iCol];
        }
    }
    return kProd;
}
//----------------------------------------------------------------------------
Vector2 Matrix2::operator* (const Vector2& rkVector) const
{
    Vector2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        kProd[iRow] =
            m_aafEntry[iRow][0]*rkVector[0] +
            m_aafEntry[iRow][1]*rkVector[1];
    }
    return kProd;
}
//----------------------------------------------------------------------------
Vector2 Mgc::operator* (const Vector2& rkVector, const Matrix2& rkMatrix)
{
    Vector2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        kProd[iRow] =
            rkVector[0]*rkMatrix.m_aafEntry[0][iRow] +
            rkVector[1]*rkMatrix.m_aafEntry[1][iRow];
    }
    return kProd;
}
//----------------------------------------------------------------------------
Matrix2 Matrix2::operator- () const
{
    Matrix2 kNeg;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
            kNeg[iRow][iCol] = -m_aafEntry[iRow][iCol];
    }
    return kNeg;
}
//----------------------------------------------------------------------------
Matrix2 Matrix2::operator* (Real fScalar) const
{
    Matrix2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
            kProd[iRow][iCol] = fScalar*m_aafEntry[iRow][iCol];
    }
    return kProd;
}
//----------------------------------------------------------------------------
Matrix2 Mgc::operator* (Real fScalar, const Matrix2& rkMatrix)
{
    Matrix2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
            kProd[iRow][iCol] = fScalar*rkMatrix.m_aafEntry[iRow][iCol];
    }
    return kProd;
}
//----------------------------------------------------------------------------
Matrix2 Matrix2::Transpose () const
{
    Matrix2 kTranspose;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
            kTranspose[iRow][iCol] = m_aafEntry[iCol][iRow];
    }
    return kTranspose;
}
//----------------------------------------------------------------------------
bool Matrix2::Inverse (Matrix2& rkInverse, Real fTolerance) const
{
    Real fDet = m_aafEntry[0][0]*m_aafEntry[1][1] -
        m_aafEntry[0][1]*m_aafEntry[1][0];

    if ( Math::FAbs(fDet) <= fTolerance )
        return false;

    Real fInvDet = 1.0f/fDet;
    rkInverse[0][0] = m_aafEntry[1][1]*fInvDet;
    rkInverse[1][0] = -m_aafEntry[1][0]*fInvDet;
    rkInverse[0][1] = -m_aafEntry[0][1]*fInvDet;
    rkInverse[1][1] = m_aafEntry[0][0]*fInvDet;

    return true;
}
//----------------------------------------------------------------------------
Matrix2 Matrix2::Inverse (Real fTolerance) const
{
    Matrix2 fInverse = Matrix2::ZERO;
    Inverse(fInverse,fTolerance);
    return fInverse;
}
//----------------------------------------------------------------------------
Real Matrix2::Determinant () const
{
    return m_aafEntry[0][0]*m_aafEntry[1][1] -
           m_aafEntry[0][1]*m_aafEntry[1][0];
}
//----------------------------------------------------------------------------
void Matrix2::SingularValueDecomposition (Matrix2& rkL, Vector2& rkS,
    Matrix2& rkR) const
{
    // compute M*M^t and M^t*M where M^t = Transpose(M)
    Real fM00Sqr = m_aafEntry[0][0]*m_aafEntry[0][0];
    Real fM01Sqr = m_aafEntry[0][1]*m_aafEntry[0][1];
    Real fM10Sqr = m_aafEntry[1][0]*m_aafEntry[1][0];
    Real fM11Sqr = m_aafEntry[1][1]*m_aafEntry[1][1];

    Real afMMT[3], afMTM[3];
    afMMT[0] = fM00Sqr + fM01Sqr;
    afMMT[1] = m_aafEntry[0][0]*m_aafEntry[1][0] +
        m_aafEntry[0][1]*m_aafEntry[1][1];
    afMMT[2] = fM10Sqr + fM11Sqr;
    afMTM[0] = fM00Sqr + fM10Sqr;
    afMTM[1] = m_aafEntry[0][0]*m_aafEntry[0][1] +
        m_aafEntry[1][0]*m_aafEntry[1][1];
    afMTM[2] = fM01Sqr + fM11Sqr;

    // factor M*M^t = L*S^2*L^t
    Real fCos, fSin, fInvLen;
    if ( Math::FAbs(afMMT[1]) > 0.0f )
    {
        Real fTrace = afMMT[0] + afMMT[2];
        Real fDet = afMMT[0]*afMMT[2] - afMMT[1]*afMMT[1];
        Real fDiscr = fTrace*fTrace-4.0f*fDet;
        Real fRoot = Math::Sqrt(Math::FAbs(fDiscr));
        rkS[0] = 0.5f*(fTrace + fRoot);
        rkS[1] = 0.5f*(fTrace - fRoot);

        fCos = afMMT[1];
        fSin = rkS[0] - afMMT[0];
        fInvLen = Math::InvSqrt(fCos*fCos + fSin*fSin);
        fCos *= fInvLen;
        fSin *= fInvLen;

        rkL[0][0] = fCos;
        rkL[1][0] = fSin;
        rkL[0][1] = -fSin;
        rkL[1][1] = fCos;
    }
    else
    {
        rkS[0] = afMMT[0];
        rkS[1] = afMMT[2];
        rkL = IDENTITY;
    }

    // factor M^t*M = R^t*S^2*R
    if ( Math::FAbs(afMTM[1]) > 0.0f )
    {
        fCos = afMTM[1];
        fSin = rkS[0] - afMTM[0];
        fInvLen = Math::InvSqrt(fCos*fCos + fSin*fSin);
        fCos *= fInvLen;
        fSin *= fInvLen;

        rkR[0][0] = fCos;
        rkR[1][0] = -fSin;
        rkR[0][1] = +fSin;
        rkR[1][1] = fCos;
    }
    else
    {
        rkR = IDENTITY;
    }

    rkS[0] = Math::Sqrt(Math::FAbs(rkS[0]));
    rkS[1] = Math::Sqrt(Math::FAbs(rkS[1]));

    Vector2 kDiag = rkS;

    Matrix2 kTest;
    kTest.SingularValueComposition(rkL,kDiag,rkR);
    kTest = *this - kTest;
    int i = 0;
    Real fMinNormSqr = kTest.L2NormSqr();

    kDiag[0] = -kDiag[0];
    kTest.SingularValueComposition(rkL,kDiag,rkR);
    kTest = *this - kTest;
    Real fNormSqr = kTest.L2NormSqr();
    if ( fNormSqr < fMinNormSqr )
    {
        i = 1;
        fMinNormSqr = fNormSqr;
    }

    kDiag[1] = -kDiag[1];
    kTest.SingularValueComposition(rkL,kDiag,rkR);
    kTest = *this - kTest;
    fNormSqr = kTest.L2NormSqr();
    if ( fNormSqr < fMinNormSqr )
    {
        i = 2;
        fMinNormSqr = fNormSqr;
    }

    kDiag[0] = -kDiag[0];
    kTest.SingularValueComposition(rkL,kDiag,rkR);
    kTest = *this - kTest;
    fNormSqr = kTest.L2NormSqr();
    if ( fNormSqr < fMinNormSqr )
    {
        i = 3;
        fMinNormSqr = fNormSqr;
    }

    switch ( i )
    {
    case 1:
        rkR[0][0] = -rkR[0][0];
        rkR[0][1] = -rkR[0][1];
        break;
    case 2:
        rkR[0][0] = -rkR[0][0];
        rkR[0][1] = -rkR[0][1];
        rkR[1][0] = -rkR[1][0];
        rkR[1][1] = -rkR[1][1];
        break;
    case 3:
        rkR[1][0] = -rkR[1][0];
        rkR[1][1] = -rkR[1][1];
        break;
    }
}
//----------------------------------------------------------------------------
void Matrix2::SingularValueComposition (const Matrix2& rkL,
    const Vector2& rkS, const Matrix2& rkR)
{
    int iRow, iCol;
    Matrix2 kTmp;

    // product S*R
    for (iRow = 0; iRow < 2; iRow++)
    {
        for (iCol = 0; iCol < 2; iCol++)
            kTmp[iRow][iCol] = rkS[iRow]*rkR[iRow][iCol];
    }

    // product L*S*R
    for (iRow = 0; iRow < 2; iRow++)
    {
        for (iCol = 0; iCol < 2; iCol++)
        {
            m_aafEntry[iRow][iCol] = 0.0f;
            for (int iMid = 0; iMid < 2; iMid++)
                m_aafEntry[iRow][iCol] += rkL[iRow][iMid]*kTmp[iMid][iCol];
        }
    }
}
//----------------------------------------------------------------------------
void Matrix2::Orthonormalize ()
{
    // Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
    // M = [m0|m1], then orthonormal output matrix is Q = [q0|q1],
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.

    // compute q0
    Real fInvLength = Math::InvSqrt(m_aafEntry[0][0]*m_aafEntry[0][0]
        + m_aafEntry[1][0]*m_aafEntry[1][0]);

    m_aafEntry[0][0] *= fInvLength;
    m_aafEntry[1][0] *= fInvLength;

    // compute q1
    Real fDot0 =
        m_aafEntry[0][0]*m_aafEntry[0][1] +
        m_aafEntry[1][0]*m_aafEntry[1][1];

    m_aafEntry[0][1] -= fDot0*m_aafEntry[0][0];
    m_aafEntry[1][1] -= fDot0*m_aafEntry[1][0];

    fInvLength = Math::InvSqrt(m_aafEntry[0][1]*m_aafEntry[0][1] +
        m_aafEntry[1][1]*m_aafEntry[1][1]);

    m_aafEntry[0][1] *= fInvLength;
    m_aafEntry[1][1] *= fInvLength;
}
//----------------------------------------------------------------------------
void Matrix2::QDUDecomposition (Matrix2& rkQ, Vector2& rkD, Real& rkU) const
{
    // Factor M = QR = QDU where Q is orthogonal, D is diagonal, 
    // and U is upper triangular with ones on its diagonal.  Algorithm uses
    // Gram-Schmidt orthogonalization (the QR algorithm).
    //
    // If M = [ m0 | m1 ] and Q = [ q0 | q1 ], then
    //
    //   q0 = m0/|m0| = (a,b)
    //   q1 = (-b,a)
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.  The matrix R has entries
    //
    //   r00 = q0*m0  r01 = q0*m1
    //   r10 = 0      r11 = q1*m1
    //
    // so D = diag(r00,r11) and U has entry u01 = r01/r00.

    // Q = rotation
    // D = scaling
    // U = shear

    // D stores the two diagonal entries r00, r11
    // U stores the entries u01

    // build orthogonal matrix Q
    Real fInvLength = Math::InvSqrt(m_aafEntry[0][0]*m_aafEntry[0][0]);
    rkQ[0][0] = m_aafEntry[0][0]*fInvLength;
    rkQ[1][0] = m_aafEntry[1][0]*fInvLength;

    Real fDot = rkQ[0][0]*m_aafEntry[0][1] + rkQ[1][0]*m_aafEntry[1][1];
    rkQ[0][1] = m_aafEntry[0][1]-fDot*rkQ[0][0];
    rkQ[1][1] = m_aafEntry[1][1]-fDot*rkQ[1][0];
    fInvLength = Math::InvSqrt(rkQ[0][1]*rkQ[0][1] + rkQ[1][1]*rkQ[1][1]);
    rkQ[0][1] *= fInvLength;
    rkQ[1][1] *= fInvLength;

    // guarantee that orthogonal matrix has determinant 1 (no reflections)
    Real fDet = rkQ[0][0]*rkQ[1][1] - rkQ[0][1]*rkQ[1][0];
    if ( fDet < 0.0f )
    {
        rkQ[0][1] = -rkQ[0][1];
        rkQ[1][1] = -rkQ[1][1];
    }

    // build "right" matrix R
    Matrix2 kR;
    kR[0][0] = rkQ[0][0]*m_aafEntry[0][0] + rkQ[1][0]*m_aafEntry[1][0];
    kR[0][1] = rkQ[0][0]*m_aafEntry[0][1] + rkQ[1][0]*m_aafEntry[1][1];
    kR[1][0] = 0.0f;
    kR[1][1] = rkQ[0][1]*m_aafEntry[0][1] + rkQ[1][1]*m_aafEntry[1][1];

    // the scaling component
    rkD[0] = kR[0][0];
    rkD[1] = kR[1][1];

    // the shear component
    rkU = kR[0][1]/rkD[0];
}
//----------------------------------------------------------------------------
Real Matrix2::SpectralNorm () const
{
    Matrix2 kP;
    int iRow, iCol;
    Real fPmax = 0.0f;
    for (iRow = 0; iRow < 2; iRow++)
    {
        for (iCol = 0; iCol < 2; iCol++)
        {
            kP[iRow][iCol] = 0.0f;
            for (int iMid = 0; iMid < 2; iMid++)
            {
                kP[iRow][iCol] +=
                    m_aafEntry[iMid][iRow]*m_aafEntry[iMid][iCol];
            }
            if ( kP[iRow][iCol] > fPmax )
                fPmax = kP[iRow][iCol];
        }
    }

    Real fInvPmax = 1.0f/fPmax;
    for (iRow = 0; iRow < 2; iRow++)
    {
        for (iCol = 0; iCol < 3; iCol++)
            kP[iRow][iCol] *= fInvPmax;
    }

    Real fDet = kP[0][0]*kP[1][1]-kP[0][1]*kP[1][0];
    Real fTrace = -(kP[0][0]+kP[1][1]);
    Real fRoot = 0.5f*(-fTrace +
        Math::Sqrt(Math::FAbs(fTrace*fTrace-4.0f*fDet)));
    Real fNorm = Math::Sqrt(fPmax*fRoot);
    return fNorm;
}
//----------------------------------------------------------------------------
Real Matrix2::L2NormSqr () const
{
    Real fNormSqr = 0.0f;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            Real fSqr = m_aafEntry[iRow][iCol]*m_aafEntry[iRow][iCol];
            fNormSqr += fSqr;
        }
    }
    return fNormSqr;
}
//----------------------------------------------------------------------------
void Matrix2::ToAngle (Real& rfRadians) const
{
    rfRadians = Math::ATan2(m_aafEntry[1][0],m_aafEntry[0][0]);
}
//----------------------------------------------------------------------------
void Matrix2::FromAngle (Real kRadians)
{
    Real fCos = Math::Cos(kRadians);
    Real fSin = Math::Sin(kRadians);
    
    m_aafEntry[0][0] = fCos;
    m_aafEntry[0][1] = -fSin;
    m_aafEntry[1][0] = fSin;
    m_aafEntry[1][1] = fCos;
}
//----------------------------------------------------------------------------
void Matrix2::Tridiagonal (Real afDiag[2], Real afSubDiag[2])
{
    // matrix is already tridiagonal, repackage for QL algorithm
    afDiag[0] = m_aafEntry[0][0];
    afDiag[1] = m_aafEntry[1][1];
    afSubDiag[0] = m_aafEntry[0][1];
    afSubDiag[1] = 0.0f;
    m_aafEntry[0][0] = 1.0f;
    m_aafEntry[0][1] = 0.0f;
    m_aafEntry[1][0] = 0.0f;
    m_aafEntry[1][1] = 1.0f;
}
//----------------------------------------------------------------------------
bool Matrix2::QLAlgorithm (Real afDiag[2], Real afSubDiag[2])
{
    for (int i0 = 0; i0 < 2; i0++)
    {
        const int iMaxIter = 32;
        int iIter;
        for (iIter = 0; iIter < iMaxIter; iIter++)
        {
            int i1;
            for (i1 = i0; i1 <= 0; i1++)
            {
                Real fSum = Math::FAbs(afDiag[i1]) + Math::FAbs(afDiag[i1+1]);
                if ( Math::FAbs(afSubDiag[i1]) + fSum == fSum )
                    break;
            }
            if ( i1 == i0 )
                break;

            Real fTmp0 = (afDiag[i0+1]-afDiag[i0])/(2.0f*afSubDiag[i0]);
            Real fTmp1 = Math::Sqrt(fTmp0*fTmp0 + 1.0f);
            if ( fTmp0 < 0.0f )
                fTmp0 = afDiag[i1]-afDiag[i0]+afSubDiag[i0]/(fTmp0-fTmp1);
            else
                fTmp0 = afDiag[i1]-afDiag[i0]+afSubDiag[i0]/(fTmp0+fTmp1);
            Real fSin = 1.0f;
            Real fCos = 1.0f;
            Real fTmp2 = 0.0f;
            for (int i2 = i1-1; i2 >= i0; i2--)
            {
                Real fTmp3 = fSin*afSubDiag[i2];
                Real fTmp4 = fCos*afSubDiag[i2];
                if ( Math::FAbs(fTmp3) >= Math::FAbs(fTmp0) )
                {
                    fCos = fTmp0/fTmp3;
                    fTmp1 = Math::Sqrt(fCos*fCos + 1.0f);
                    afSubDiag[i2+1] = fTmp3*fTmp1;
                    fSin = 1.0f/fTmp1;
                    fCos *= fSin;
                }
                else
                {
                    fSin = fTmp3/fTmp0;
                    fTmp1 = Math::Sqrt(fSin*fSin+1.0f);
                    afSubDiag[i2+1] = fTmp0*fTmp1;
                    fCos = 1.0f/fTmp1;
                    fSin *= fCos;
                }
                fTmp0 = afDiag[i2+1]-fTmp2;
                fTmp1 = (afDiag[i2]-fTmp0)*fSin + 2.0f*fTmp4*fCos;
                fTmp2 = fSin*fTmp1;
                afDiag[i2+1] = fTmp0+fTmp2;
                fTmp0 = fCos*fTmp1-fTmp4;

                for (int iRow = 0; iRow < 2; iRow++)
                {
                    fTmp3 = m_aafEntry[iRow][i2+1];
                    m_aafEntry[iRow][i2+1] = fSin*m_aafEntry[iRow][i2] +
                        fCos*fTmp3;
                    m_aafEntry[iRow][i2] = fCos*m_aafEntry[iRow][i2] -
                        fSin*fTmp3;
                }
            }
            afDiag[i0] -= fTmp2;
            afSubDiag[i0] = fTmp0;
            afSubDiag[i1] = 0.0f;
        }

        if ( iIter == iMaxIter )
        {
            // should not get here under normal circumstances
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void Matrix2::EigenSolveSymmetric (Real afEigenvalue[2],
    Vector2 akEigenvector[2]) const
{
    Matrix2 kMatrix = *this;
    Real afSubDiag[3];
    kMatrix.Tridiagonal(afEigenvalue,afSubDiag);
    kMatrix.QLAlgorithm(afEigenvalue,afSubDiag);

    for (int i = 0; i < 2; i++)
    {
        akEigenvector[i][0] = kMatrix[0][i];
        akEigenvector[i][1] = kMatrix[1][i];
    }

    // make eigenvectors form a right--handed system
    Real fDet = akEigenvector[0].Dot(akEigenvector[1]);
    if ( fDet < 0.0f )
    {
        akEigenvector[1][0] = - akEigenvector[1][0];
        akEigenvector[1][1] = - akEigenvector[1][1];
    }
}
//----------------------------------------------------------------------------
void Matrix2::TensorProduct (const Vector2& rkU, const Vector2& rkV,
    Matrix2& rkProduct)
{
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
            rkProduct[iRow][iCol] = rkU[iRow]*rkV[iCol];
    }
}
//----------------------------------------------------------------------------


