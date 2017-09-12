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

#include "MgcPolynomial.h"
#include "MgcRTLib.h"
using namespace Mgc;

#include <vector>
using namespace std;

int Polynomial::DIGITS_ACCURACY = 6;
Real Polynomial::ZERO_TOLERANCE = 1e-06f;
Real Polynomial::REMAINDER_EPSILON = 1e-06f;
Real Polynomial::STURM_EPSILON = 1e-04f;

#if 0
// Visual C++ 6.0 SP5 seems not to be calling the math functions in this
// block during pre-main initialization.  In GetAllRoots, ms_fInvLog2,
// ms_fLog10, and ms_fSqrt3 all show up as zero.
const Real Polynomial::ms_fInvLog2 = 1.0f/Math::Log(2.0f);
const Real Polynomial::ms_fLog10 = Math::Log(10.0f);
const Real Polynomial::ms_fThird = 1.0f/3.0f;
const Real Polynomial::ms_fSqrt3 = Math::Sqrt(3.0f);
const Real Polynomial::ms_fTwentySeventh = 1.0f/27.0f;
#else
const Real Polynomial::ms_fInvLog2       = 1.442695041f;
const Real Polynomial::ms_fLog10         = 2.302585093f;
const Real Polynomial::ms_fThird         = 0.333333333f;
const Real Polynomial::ms_fSqrt3         = 1.732050808f;
const Real Polynomial::ms_fTwentySeventh = 0.037037037f;
#endif

//----------------------------------------------------------------------------
Polynomial::Polynomial (int iDegree)
{
    if ( iDegree >= 0 )
    {
        m_iDegree = iDegree;
        m_afCoeff = new Real[m_iDegree+1];
    }
    else
    {
        // default creation
        m_iDegree = -1;
        m_afCoeff = NULL;
    }
}
//----------------------------------------------------------------------------
Polynomial::Polynomial (const Polynomial& rkPoly)
{
    m_iDegree = rkPoly.m_iDegree;
    m_afCoeff = new Real[m_iDegree+1];
    for (int i = 0; i <= m_iDegree; i++)
        m_afCoeff[i] = rkPoly.m_afCoeff[i];
}
//----------------------------------------------------------------------------
Polynomial::~Polynomial ()
{
    delete[] m_afCoeff;
}
//----------------------------------------------------------------------------
Real& Polynomial::operator[] (int i) const
{
    assert( 0 <= i && i <= m_iDegree );
    return ((Real*)m_afCoeff)[i];
}
//----------------------------------------------------------------------------
Polynomial& Polynomial::operator= (const Polynomial& rkPoly)
{
    delete[] m_afCoeff;
    m_iDegree = rkPoly.m_iDegree;

    if ( m_iDegree >= 0 )
    {
        m_afCoeff = new Real[m_iDegree+1];
        for (int i = 0; i <= m_iDegree; i++)
            m_afCoeff[i] = rkPoly.m_afCoeff[i];
    }

    return *this;
}
//----------------------------------------------------------------------------
void Polynomial::SetDegree (int iDegree)
{
    m_iDegree = iDegree;
    delete[] m_afCoeff;

    if ( m_iDegree >= 0 )
        m_afCoeff = new Real[m_iDegree+1];
    else
        m_afCoeff = NULL;
}
//----------------------------------------------------------------------------
int Polynomial::GetDegree () const
{
    return m_iDegree;
}
//----------------------------------------------------------------------------
Real Polynomial::operator() (Real fT) const
{
    assert( m_iDegree >= 0 );

    Real fResult = m_afCoeff[m_iDegree];
    for (int i = m_iDegree-1; i >= 0; i--)
    {
        fResult *= fT;
        fResult += m_afCoeff[i];
    }
    return fResult;
}
//----------------------------------------------------------------------------
Polynomial Polynomial::GetDerivative () const
{
    if ( m_iDegree > 0 )
    {
        Polynomial kDeriv(m_iDegree-1);
        for (int i0 = 0, i1 = 1; i0 < m_iDegree; i0++, i1++)
            kDeriv[i0] = i1*m_afCoeff[i1];
        return kDeriv;
    }
    else
    {
        return Polynomial(-1);
    }
}
//----------------------------------------------------------------------------
Polynomial Polynomial::GetInversion () const
{
    Polynomial kInvPoly(m_iDegree);
    for (int i = 0; i <= m_iDegree; i++)
        kInvPoly[i] = m_afCoeff[m_iDegree-i];
    return kInvPoly;
}
//----------------------------------------------------------------------------
void Polynomial::Compress (Real fEpsilon)
{
	int i;
	for (i = m_iDegree; i >= 0; i--)
	{
        if ( Math::FAbs(m_afCoeff[i]) <= fEpsilon )
			m_iDegree--;
		else
			break;
	}

	for (i = 0; i <= m_iDegree; i++)
		m_afCoeff[i] /= m_afCoeff[m_iDegree];
}
//----------------------------------------------------------------------------
Polynomial Polynomial::operator+ (const Polynomial& rkPoly) const
{
    assert( m_iDegree >= 0 && rkPoly.m_iDegree >= 0 );

    Polynomial kSum(-1);
    int i;

    if ( m_iDegree > rkPoly.m_iDegree )
    {
        kSum.SetDegree(m_iDegree);
        for (i = 0; i <= rkPoly.m_iDegree; i++)
            kSum[i] = m_afCoeff[i] + rkPoly.m_afCoeff[i];
        for (i = rkPoly.m_iDegree+1; i <= m_iDegree; i++)
            kSum[i] = m_afCoeff[i];

    }
    else
    {
        kSum.SetDegree(rkPoly.m_iDegree);
        for (i = 0; i <= m_iDegree; i++)
            kSum[i] = m_afCoeff[i] + rkPoly.m_afCoeff[i];
        for (i = m_iDegree+1; i <= m_iDegree; i++)
            kSum[i] = rkPoly.m_afCoeff[i];
    }

    return kSum;
}
//----------------------------------------------------------------------------
Polynomial Polynomial::operator- (const Polynomial& rkPoly) const
{
    assert( m_iDegree >= 0 && rkPoly.m_iDegree >= 0 );

    Polynomial kDiff(-1);
    int i;

    if ( m_iDegree > rkPoly.m_iDegree )
    {
        kDiff.SetDegree(m_iDegree);
        for (i = 0; i <= rkPoly.m_iDegree; i++)
            kDiff[i] = m_afCoeff[i] - rkPoly.m_afCoeff[i];
        for (i = rkPoly.m_iDegree+1; i <= m_iDegree; i++)
            kDiff[i] = m_afCoeff[i];

    }
    else
    {
        kDiff.SetDegree(rkPoly.m_iDegree);
        for (i = 0; i <= m_iDegree; i++)
            kDiff[i] = m_afCoeff[i] - rkPoly.m_afCoeff[i];
        for (i = m_iDegree+1; i <= m_iDegree; i++)
            kDiff[i] = -rkPoly.m_afCoeff[i];
    }

    return kDiff;
}
//----------------------------------------------------------------------------
Polynomial Polynomial::operator* (const Polynomial& rkPoly) const
{
    assert( m_iDegree >= 0 && rkPoly.m_iDegree >= 0 );

    Polynomial kProd(m_iDegree + rkPoly.m_iDegree);

    memset(kProd.m_afCoeff,0,(kProd.m_iDegree+1)*sizeof(Real));
    for (int i0 = 0; i0 <= m_iDegree; i0++)
    {
        for (int i1 = 0; i1 <= rkPoly.m_iDegree; i1++)
        {
            int i2 = i0 + i1;
            kProd.m_afCoeff[i2] += m_afCoeff[i0]*rkPoly.m_afCoeff[i1];
        }
    }

    return kProd;
}
//----------------------------------------------------------------------------
Polynomial Polynomial::operator* (Real fScalar) const
{
    assert( m_iDegree >= 0 );

    Polynomial kProd(m_iDegree);
    for (int i = 0; i <= m_iDegree; i++)
        kProd[i] = fScalar*m_afCoeff[i];

    return kProd;
}
//----------------------------------------------------------------------------
Polynomial Polynomial::operator- () const
{
    assert( m_iDegree >= 0 );

    Polynomial kNeg(m_iDegree);
    for (int i = 0; i <= m_iDegree; i++)
        kNeg[i] = -m_afCoeff[i];

    return kNeg;
}
//----------------------------------------------------------------------------
Polynomial Mgc::operator* (Real fScalar, const Polynomial& rkPoly)
{
    assert( rkPoly.m_iDegree >= 0 );

    Polynomial kProd(rkPoly.m_iDegree);
    for (int i = 0; i <= rkPoly.m_iDegree; i++)
        kProd[i] = fScalar*rkPoly.m_afCoeff[i];

    return kProd;
}
//----------------------------------------------------------------------------
void Polynomial::Divide (const Polynomial& rkDiv, Polynomial& rkQuot,
    Polynomial& rkRem)
{
    int iQuotDegree = m_iDegree - rkDiv.m_iDegree;
    if ( iQuotDegree >= 0 )
    {
        rkQuot.SetDegree(iQuotDegree);

        // temporary storage for the remainder
        Polynomial kTmp = *this;

        // do the division (Euclidean algorithm)
        Real fInv = 1.0f/rkDiv[rkDiv.m_iDegree];
        for (int iQ = iQuotDegree; iQ >= 0; iQ--)
        {
            int iR = rkDiv.m_iDegree + iQ;
            rkQuot[iQ] = fInv*kTmp[iR];
            for (iR--; iR >= iQ; iR--)
                kTmp[iR] -= rkQuot[iQ]*rkDiv[iR-iQ];
        }

        // calculate the correct degree for the remainder
        int iRemDeg = rkDiv.m_iDegree - 1;
        while ( iRemDeg > 0 && Math::FAbs(kTmp[iRemDeg]) < REMAINDER_EPSILON )
            iRemDeg--;

        if ( iRemDeg == 0 && Math::FAbs(kTmp[0]) < REMAINDER_EPSILON )
            kTmp[0] = 0.0f;

        rkRem.SetDegree(iRemDeg);
        memcpy(rkRem.m_afCoeff,kTmp.m_afCoeff,(iRemDeg+1)*sizeof(Real));
    }
    else
    {
        rkQuot.SetDegree(0);
        rkQuot[0] = 0.0f;
        rkRem = *this;
    }
}
//----------------------------------------------------------------------------
bool Polynomial::Bisection (Real fXMin, Real fXMax,
    Real& rfRoot) const
{
    Real fP0 = (*this)(fXMin);
    Real fP1 = (*this)(fXMax);

    // check for endpoint zeros
    if ( Math::FAbs(fP0) <= ZERO_TOLERANCE )
    {
        rfRoot = fXMin;
        return true;
    }
    if ( Math::FAbs(fP1) <= ZERO_TOLERANCE )
    {
        rfRoot = fXMax;
        return true;
    }

    if ( fP0*fP1 > 0.0f )
        return false;

    // determine number of iterations to get 'digits' accuracy.
    Real fTmp0 = Math::Log(fXMax-fXMin);
    Real fTmp1 = DIGITS_ACCURACY*ms_fLog10;
    Real fArg = (fTmp0 + fTmp1)*ms_fInvLog2;
    int iMaxIter = (int)Math::Ceil(fArg);
    
    for (int i = 0; i < iMaxIter; i++)
    {
        rfRoot = 0.5f*(fXMin + fXMax);
        Real fP = (*this)(rfRoot);
        if ( Math::FAbs(fP) <= ZERO_TOLERANCE )
            return true;
        
        if ( fP*fP0 < 0.0f )
        {
            fXMax = rfRoot;
            fP1 = fP;
        }
        else
        {
            fXMin = rfRoot;
            fP0 = fP;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void Polynomial::GetRootsOnInterval (Real fXMin, Real fXMax,
    int& riCount, Real* afRoot) const
{
    Real fRoot;

    if ( m_iDegree == 1 )
    {
        if ( Bisection(fXMin,fXMax,fRoot) )
        {
            riCount = 1;
            afRoot[0] = fRoot;
        }
        else
        {
            riCount = 0;
        }
        return;
    }

    // get roots of derivative polynomial
    Polynomial kDeriv = GetDerivative();
    kDeriv.GetRootsOnInterval(fXMin,fXMax,riCount,afRoot);

    int i, iNewCount = 0;
    Real* afNewRoot = new Real[riCount+1];

    if ( riCount > 0 )
    {
        // find root on [xmin,root[0]]
        if ( Bisection(fXMin,afRoot[0],fRoot) )
            afNewRoot[iNewCount++] = fRoot;

        // find root on [root[i],root[i+1]] for 0 <= i <= count-2
        for (i = 0; i <= riCount-2; i++)
        {
            if ( Bisection(afRoot[i],afRoot[i+1],fRoot) )
                afNewRoot[iNewCount++] = fRoot;
        }

        // find root on [root[count-1],xmax]
        if ( Bisection(afRoot[riCount-1],fXMax,fRoot) )
            afNewRoot[iNewCount++] = fRoot;
    }
    else
    {
        // polynomial is monotone on [xmin,xmax], has at most one root
        if ( Bisection(fXMin,fXMax,fRoot) )
            afNewRoot[iNewCount++] = fRoot;
    }

    // copy to old buffer
    if ( iNewCount > 0 )
    {
        riCount = 1;
        afRoot[0] = afNewRoot[0];
        for (i = 1; i < iNewCount; i++)
        {
            if ( Math::FAbs(afNewRoot[i]-afNewRoot[i-1]) > ZERO_TOLERANCE )
                afRoot[riCount++] = afNewRoot[i];
        }
    }
    else
    {
        riCount = 0;
    }

    delete[] afNewRoot;
}
//----------------------------------------------------------------------------
void Polynomial::GetAllRoots (int& riCount, Real* afRoot) const
{
    Real fBound, fTmp;
    int i;

    Real fAbs0 = Math::FAbs(m_afCoeff[0]);
    Real fAbsD = Math::FAbs(m_afCoeff[m_iDegree]);

    if ( fAbsD >= fAbs0 )
    {
        fBound = fAbs0;
        for (i = 0; i < m_iDegree; i++)
        {
            fTmp = Math::FAbs(m_afCoeff[i]) + fAbsD;
            if ( fBound < fTmp )
                fBound = fTmp;
        }
        fBound /= fAbsD;

        GetRootsOnInterval(-fBound,fBound,riCount,afRoot);
    }
    else
    {
        fBound = fAbsD;
        for (i = 0; i < m_iDegree; i++)
        {
            fTmp = Math::FAbs(m_afCoeff[i]) + fAbs0;
            if ( fBound < fTmp )
                fBound = fTmp;
        }
        fBound /= fAbs0;

        // use inverted polynomial to find inverse roots
        Real* afInvRoot = new Real[m_iDegree];
        Polynomial kInvPoly = GetInversion();
        kInvPoly.GetRootsOnInterval(-fBound,fBound,riCount,afInvRoot);

        // Adjust the tolerance to your liking.  Any inverse root whose
        // magnitude is smaller than the tolerance is assumed to be zero.  In
        // this case the original polynomial has a "root at infinity", but
        // only finite roots are included in the final results.
        const Real fEpsilon = 1e-08f;
        int iFiniteRoots = 0;
        for (i = 0; i < riCount; i++)
        {
            Real fTmp = afInvRoot[riCount-1-i];
            if ( Math::FAbs(fTmp) >= fEpsilon )
            {
                afRoot[i] = 1.0f/afInvRoot[riCount-1-i];
                iFiniteRoots++;
            }
        }
        delete[] afInvRoot;
        riCount = iFiniteRoots;
    }
}
//----------------------------------------------------------------------------
Real Polynomial::GetRootBound () const
{
    Real fBound, fTmp;
    int i;

    Real fAbs0 = Math::FAbs(m_afCoeff[0]);
    Real fAbsD = Math::FAbs(m_afCoeff[m_iDegree]);

    if ( fAbsD >= fAbs0 )
    {
        fBound = fAbs0;
        for (i = 0; i < m_iDegree; i++)
        {
            fTmp = Math::FAbs(m_afCoeff[i]) + fAbsD;
            if ( fBound < fTmp )
                fBound = fTmp;
        }
        fBound /= fAbsD;
    }
    else
    {
        fBound = fAbsD;
        for (i = 0; i < m_iDegree; i++)
        {
            fTmp = Math::FAbs(m_afCoeff[i]) + fAbs0;
            if ( fBound < fTmp )
                fBound = fTmp;
        }
        fBound /= fAbs0;
    }

    return fBound;
}
//----------------------------------------------------------------------------
bool Polynomial::AllRealPartsNegative (int iDegree, Real* afCoeff)
{
    // assert:  afCoeff[iDegree] = 1

    if ( afCoeff[iDegree-1] <= 0.0f )
        return false;
    if ( iDegree == 1 )
        return true;

    Real* afTmpCoeff = new Real[iDegree];
    afTmpCoeff[0] = 2.0f*afCoeff[0]*afCoeff[iDegree-1];
    int i;
    for (i = 1; i <= iDegree-2; i++) 
    {
        afTmpCoeff[i] = afCoeff[iDegree-1]*afCoeff[i];
        if ( ((iDegree-i) % 2) == 0 )
            afTmpCoeff[i] -= afCoeff[i-1];
        afTmpCoeff[i] *= 2.0f;
    }
    afTmpCoeff[iDegree-1] = 2.0f*afCoeff[iDegree-1]*afCoeff[iDegree-1];

    int iNextDegree;
    for (iNextDegree = iDegree-1; iNextDegree >= 0; iNextDegree--)
    {
        if ( afTmpCoeff[iNextDegree] != 0.0f )
            break;
    }
    for (i = 0; i <= iNextDegree-1; i++)
        afCoeff[i] = afTmpCoeff[i]/afTmpCoeff[iNextDegree];
    delete[] afTmpCoeff;

    return AllRealPartsNegative(iNextDegree,afCoeff);
}
//----------------------------------------------------------------------------
bool Polynomial::AllRealPartsNegative () const
{
    // make a copy of coefficients, later calls will change the copy
    Real* afCoeff = new Real[m_iDegree+1];
    memcpy(afCoeff,m_afCoeff,(m_iDegree+1)*sizeof(Real));

    // make polynomial monic
    if ( afCoeff[m_iDegree] != 1.0f )
    {
        Real fInv = 1.0f/afCoeff[m_iDegree];
        for (int i = 0; i < m_iDegree; i++)
            afCoeff[i] *= fInv;
        afCoeff[m_iDegree] = 1.0f;
    }

    return AllRealPartsNegative(m_iDegree,afCoeff);
}
//----------------------------------------------------------------------------
bool Polynomial::AllRealPartsPositive () const
{
    // make a copy of coefficients, later calls will change the copy
    Real* afCoeff = new Real[m_iDegree+1];
    memcpy(afCoeff,m_afCoeff,(m_iDegree+1)*sizeof(Real));

    // make polynomial monic
    int i;
    if ( afCoeff[m_iDegree] != 1.0f )
    {
        Real fInv = 1.0f/afCoeff[m_iDegree];
        for (i = 0; i < m_iDegree; i++)
            afCoeff[i] *= fInv;
        afCoeff[m_iDegree] = 1.0f;
    }

    // reflect z -> -z
    int iSign = -1;
    for (i = m_iDegree-1; i >= 0; i--, iSign = -iSign)
        afCoeff[i] *= iSign;

    return AllRealPartsNegative(m_iDegree,afCoeff);
}
//----------------------------------------------------------------------------
bool Polynomial::RootsDegree2 (int& riCount, Real afRoot[2]) const
{
    // compute real roots to x^2+c[1]*x+c[0] = 0
    if ( m_iDegree != 2 )
        return false;

    // make polynomial monic
    Real afCoeff[2] = { m_afCoeff[0], m_afCoeff[1] };
    if ( m_afCoeff[2] != 1.0f )
    {
        Real fInv = 1.0f/m_afCoeff[2];
        afCoeff[0] *= fInv;
        afCoeff[1] *= fInv;
    }

    Real fDiscr = afCoeff[1]*afCoeff[1]-4.0f*afCoeff[0];
    if ( Math::FAbs(fDiscr) <= ZERO_TOLERANCE )
        fDiscr = 0.0f;

    if ( fDiscr >= 0.0f )
    {
        fDiscr = Math::Sqrt(fDiscr);
        afRoot[0] = 0.5f*(-afCoeff[1]-fDiscr);
        afRoot[1] = 0.5f*(-afCoeff[1]+fDiscr);
        riCount = 2;
    }
    else
    {
        riCount = 0;
    }

    return true;
}
//----------------------------------------------------------------------------
bool Polynomial::RootsDegree3 (int& riCount, Real afRoot[3]) const
{
    // compute real roots to x^3+c[2]*x^2+c[1]*x+c[0] = 0
    if ( m_iDegree != 3 )
        return false;

    // make polynomial monic
    Real afCoeff[3] = { m_afCoeff[0], m_afCoeff[1], m_afCoeff[2] };
    if ( m_afCoeff[3] != 1.0f )
    {
        Real fInv = 1.0f/m_afCoeff[3];
        afCoeff[0] *= fInv;
        afCoeff[1] *= fInv;
        afCoeff[2] *= fInv;
    }

    // convert to y^3+a*y+b = 0 by x = y-c[2]/3 and
    Real fA = ms_fThird*(3.0f*afCoeff[1]-afCoeff[2]*afCoeff[2]);
    Real fB = ms_fTwentySeventh*(2.0f*afCoeff[2]*afCoeff[2]*afCoeff[2] -
        9.0f*afCoeff[1]*afCoeff[2]+27.0f*afCoeff[0]);
    Real fOffset = ms_fThird*afCoeff[2];

    Real fDiscr = 0.25f*fB*fB + ms_fTwentySeventh*fA*fA*fA;
    if ( Math::FAbs(fDiscr) <= ZERO_TOLERANCE )
        fDiscr = 0.0f;

    Real fHalfB = 0.5f*fB;
    if ( fDiscr > 0.0f )  // 1 real, 2 complex roots
    {
        fDiscr = Math::Sqrt(fDiscr);
        Real fTemp = -fHalfB + fDiscr;
        if ( fTemp >= 0.0f )
            afRoot[0] = Math::Pow(fTemp,ms_fThird);
        else
            afRoot[0] = -Math::Pow(-fTemp,ms_fThird);
        fTemp = -fHalfB - fDiscr;
        if ( fTemp >= 0.0f )
            afRoot[0] += Math::Pow(fTemp,ms_fThird);
        else
            afRoot[0] -= Math::Pow(-fTemp,ms_fThird);
        afRoot[0] -= fOffset;
        riCount = 1;
    }
    else if ( fDiscr < 0.0f ) 
    {
        Real fDist = Math::Sqrt(-ms_fThird*fA);
        Real fAngle = ms_fThird*Math::ATan2(Math::Sqrt(-fDiscr),-fHalfB);
        Real fCos = Math::Cos(fAngle);
        Real fSin = Math::Sin(fAngle);
        afRoot[0] = 2.0f*fDist*fCos-fOffset;
        afRoot[1] = -fDist*(fCos+ms_fSqrt3*fSin)-fOffset;
        afRoot[2] = -fDist*(fCos-ms_fSqrt3*fSin)-fOffset;
        riCount = 3;
    }
    else 
    {
        Real fTemp;
        if ( fHalfB >= 0.0f )
            fTemp = -Math::Pow(fHalfB,ms_fThird);
        else
            fTemp = Math::Pow(-fHalfB,ms_fThird);
        afRoot[0] = 2.0f*fTemp-fOffset;
        afRoot[1] = -fTemp-fOffset;
        afRoot[2] = afRoot[1];
        riCount = 3;
    }

    return true;
}
//----------------------------------------------------------------------------
bool Polynomial::RootsDegree4 (int& riCount, Real afRoot[4]) const
{
    // compute real roots to x^4+c[3]*x^3+c[2]*x^2+c[1]*x+c[0] = 0
    if ( m_iDegree != 4 )
        return false;

    // make polynomial monic
    Real afCoeff[4] = { m_afCoeff[0], m_afCoeff[1], m_afCoeff[2],
        m_afCoeff[3] };
    if ( m_afCoeff[4] != 1.0f )
    {
        Real fInv = 1.0f/m_afCoeff[4];
        afCoeff[0] *= fInv;
        afCoeff[1] *= fInv;
        afCoeff[2] *= fInv;
        afCoeff[3] *= fInv;
    }

    // reduction to resolvent cubic polynomial
    Polynomial kResolve(3);
    kResolve[3] = 1.0f;
    kResolve[2] = -afCoeff[2];
    kResolve[1] = afCoeff[3]*afCoeff[1]-4.0f*afCoeff[0];
    kResolve[0] = -afCoeff[3]*afCoeff[3]*afCoeff[0] +
        4.0f*afCoeff[2]*afCoeff[0]-afCoeff[1]*afCoeff[1];
    int iResolveCount;
    Real afResolveRoot[3];
    kResolve.RootsDegree3(iResolveCount,afResolveRoot);
    Real fY = afResolveRoot[0];

    riCount = 0;
    Real fDiscr = 0.25f*afCoeff[3]*afCoeff[3]-afCoeff[2]+fY;
    if ( Math::FAbs(fDiscr) <= ZERO_TOLERANCE )
        fDiscr = 0.0f;

    if ( fDiscr > 0.0f ) 
    {
        Real fR = Math::Sqrt(fDiscr);
        Real fT1 = 0.75f*afCoeff[3]*afCoeff[3]-fR*fR-2.0f*afCoeff[2];
        Real fT2 = (4.0f*afCoeff[3]*afCoeff[2]-8.0f*afCoeff[1]-
            afCoeff[3]*afCoeff[3]*afCoeff[3])/(4.0f*fR);

        Real fTplus = fT1+fT2;
        Real fTminus = fT1-fT2;
        if ( Math::FAbs(fTplus) <= ZERO_TOLERANCE ) 
            fTplus = 0.0f;
        if ( Math::FAbs(fTminus) <= ZERO_TOLERANCE ) 
            fTminus = 0.0f;

        if ( fTplus >= 0.0f )
        {
            Real fD = Math::Sqrt(fTplus);
            afRoot[0] = -0.25f*afCoeff[3]+0.5f*(fR+fD);
            afRoot[1] = -0.25f*afCoeff[3]+0.5f*(fR-fD);
            riCount += 2;
        }
        if ( fTminus >= 0.0f )
        {
            Real fE = Math::Sqrt(fTminus);
            afRoot[riCount++] = -0.25f*afCoeff[3]+0.5f*(fE-fR);
            afRoot[riCount++] = -0.25f*afCoeff[3]-0.5f*(fE+fR);
        }
    }
    else if ( fDiscr < 0.0f )
    {
        riCount = 0;
    }
    else
    {
        Real fT2 = fY*fY-4.0f*afCoeff[0];
        if ( fT2 >= -ZERO_TOLERANCE ) 
        {
            if ( fT2 < 0.0f ) // round to zero
                fT2 = 0.0f;
            fT2 = 2.0f*Math::Sqrt(fT2);
            Real fT1 = 0.75f*afCoeff[3]*afCoeff[3]-2.0f*afCoeff[2];
            if ( fT1+fT2 >= ZERO_TOLERANCE ) 
            {
                Real fD = Math::Sqrt(fT1+fT2);
                afRoot[0] = -0.25f*afCoeff[3]+0.5f*fD;
                afRoot[1] = -0.25f*afCoeff[3]-0.5f*fD;
                riCount += 2;
            }
            if ( fT1-fT2 >= ZERO_TOLERANCE ) 
            {
                Real fE = Math::Sqrt(fT1-fT2);
                afRoot[riCount++] = -0.25f*afCoeff[3]+0.5f*fE;
                afRoot[riCount++] = -0.25f*afCoeff[3]-0.5f*fE;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------
Real Polynomial::SpecialCubeRoot (Real fA, Real fB, Real fC)
{
    // Solve A*r^3 + B*r = C where A > 0 and B > 0.
    //
    // Let r = D*sinh(u) where D = sqrt(4*B/(3*A)).  Then
    // sinh(3*u) = 4*[sinh(u)]^3+3*sinh(u) = E where E = 4*C/(A*D^3).
    // sinh(3*u) = E has solution u = (1/3)*log(E+sqrt(E^2+1)).  This
    // leads to sinh(u) = ((E+sqrt(E^2+1))^{1/3}-(E+sqrt(E^2+1))^{-1/3})/2.
    // Therefore,
    //
    //     r = D*((E+sqrt(E^2+1))^{1/3}-(E+sqrt(E^2+1))^{-1/3})/2.

    Real fD = Math::Sqrt(4.0f*ms_fThird*fB/fA);
    Real fE = 4.0f*fC/(fA*fD*fD*fD);
    Real fF = Math::Pow(fE+Math::Sqrt(fE*fE+1.0f),ms_fThird);
    Real fRoot = 0.5f*fD*(fF-1.0f/fF);

    return fRoot;
}
//----------------------------------------------------------------------------
int Polynomial::GetRootCount (Real fT0, Real fT1)
{
    if ( m_iDegree == 0 )
    {
        // polynomial is constant on the interval
        if ( m_afCoeff[0] != 0.0f )
            return 0;
        else
            return -1;  // to indicate "infinitely many"
    }

    // generate the Sturm sequence
    vector<Polynomial*> kSturm;
    Polynomial* pkF0 = new Polynomial(*this);
    Polynomial* pkF1 = new Polynomial(pkF0->GetDerivative());
    kSturm.push_back(pkF0);
    kSturm.push_back(pkF1);

    while ( pkF1->GetDegree() > 0 )
    {
        Polynomial* pkF2 = new Polynomial;
        Polynomial kQuot;
        pkF0->Divide(*pkF1,kQuot,*pkF2);
        *pkF2 = -(*pkF2);
        kSturm.push_back(pkF2);
        pkF0 = pkF1;
        pkF1 = pkF2;
    }

    int i;
    Real fValue0, fValue1;

    // count the sign changes at t0
    int iSignChanges0 = 0;
    if ( fT0 == -Math::MAX_REAL )
    {
        pkF0 = kSturm[0];
        if ( pkF0->GetDegree() & 1 )
            fValue0 = -(*pkF0)[pkF0->GetDegree()];
        else
            fValue0 = (*pkF0)[pkF0->GetDegree()];

        if ( Math::FAbs(fValue0) < STURM_EPSILON )
            fValue0 = 0.0f;

        for (i = 1; i < (int)kSturm.size(); i++)
        {
            pkF1 = kSturm[i];

            if ( pkF1->GetDegree() & 1 )
                fValue1 = -(*pkF1)[pkF1->GetDegree()];
            else
                fValue1 = (*pkF1)[pkF1->GetDegree()];

            if ( Math::FAbs(fValue1) < STURM_EPSILON )
                fValue1 = 0.0f;

            if ( fValue0*fValue1 < 0.0f || fValue0 == 0.0f )
                iSignChanges0++;

            fValue0 = fValue1;
            pkF0 = pkF1;
        }
    }
    else
    {
        pkF0 = kSturm[0];
        fValue0 = (*pkF0)(fT0);
        if ( Math::FAbs(fValue0) < STURM_EPSILON )
            fValue0 = 0.0f;

        for (i = 1; i < (int)kSturm.size(); i++)
        {
            pkF1 = kSturm[i];
            fValue1 = (*pkF1)(fT0);
            if ( Math::FAbs(fValue1) < STURM_EPSILON )
                fValue1 = 0.0f;

            if ( fValue0*fValue1 < 0.0f || fValue0 == 0.0f )
                iSignChanges0++;

            fValue0 = fValue1;
            pkF0 = pkF1;
        }
    }

    // count the sign changes at t1
    int iSignChanges1 = 0;
    if ( fT1 == Math::MAX_REAL )
    {
        pkF0 = kSturm[0];
        fValue0 = (*pkF0)[pkF0->GetDegree()];
        if ( Math::FAbs(fValue0) < STURM_EPSILON )
            fValue0 = 0.0f;

        for (i = 1; i < (int)kSturm.size(); i++)
        {
            pkF1 = kSturm[i];
            fValue1 = (*pkF1)[pkF1->GetDegree()];
            if ( Math::FAbs(fValue1) < STURM_EPSILON )
                fValue1 = 0.0f;

            if ( fValue0*fValue1 < 0.0f || fValue0 == 0.0f )
                iSignChanges1++;

            fValue0 = fValue1;
            pkF0 = pkF1;
        }
    }
    else
    {
        pkF0 = kSturm[0];
        fValue0 = (*pkF0)(fT1);
        if ( Math::FAbs(fValue0) < STURM_EPSILON )
            fValue0 = 0.0f;

        for (i = 1; i < (int)kSturm.size(); i++)
        {
            pkF1 = kSturm[i];
            fValue1 = (*pkF1)(fT1);
            if ( Math::FAbs(fValue1) < STURM_EPSILON )
                fValue1 = 0.0f;

            if ( fValue0*fValue1 < 0.0f || fValue0 == 0.0f )
                iSignChanges1++;

            fValue0 = fValue1;
            pkF0 = pkF1;
        }
    }

    // clean up
    for (i = 0; i < (int)kSturm.size(); i++)
        delete kSturm[i];

    if ( iSignChanges0 >= iSignChanges1 )
        return iSignChanges0 - iSignChanges1;

    // theoretically we should not get here
    assert( false );
    return 0;
}
//----------------------------------------------------------------------------


