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

#ifndef MGCPOLYNOMIAL_H
#define MGCPOLYNOMIAL_H

#include "MagicFMLibType.h"
#include "MgcMath.h"

namespace Mgc {

class MAGICFM Polynomial
{
public:
    // construction and destruction
    Polynomial (int iDegree = -1);
    Polynomial (const Polynomial& rkPoly);
    ~Polynomial ();

    // assignment
    Polynomial& operator= (const Polynomial& rkPoly);

    // coefficient access
    void SetDegree (int iDegree);
    int GetDegree () const;
    Real& operator[] (int i) const;

    // evaluation
    Real operator() (Real fT) const;

    // derivation
    Polynomial GetDerivative () const;

    // inversion ( invpoly[i] = poly[degree-i] for 0 <= i <= degree )
    Polynomial GetInversion () const;

    // Reduce degree by eliminating all (nearly) zero leading coefficients
    // and by making the leading coefficient one.  The input parameter is
    // the threshold for specifying that a coefficient is effectively zero.
    void Compress (Real fEpsilon = 1e-08f);

    // arithmetic
    Polynomial operator+ (const Polynomial& rkPoly) const;
    Polynomial operator- (const Polynomial& rkPoly) const;
    Polynomial operator* (const Polynomial& rkPoly) const;
    Polynomial operator* (Real fScalar) const;
    Polynomial operator- () const;
    MAGICFM friend Polynomial operator* (Real fScalar,
        const Polynomial& rkPoly);

    // If 'this' is P(t) and the divisor is D(t) with degree(P) >= degree(D),
    // then P(t) = Q(t)*D(t)+R(t) where Q(t) is the quotient with
    // degree(Q) = degree(P) - degree(D) and R(t) is the remainder with
    // degree(R) < degree(D).  If this routine is called with
    // degree(P) < degree(D), then Q = 0 and R = P are returned.  The value
    // of REMAINDER_EPSILON is used as a threshold on the coefficients of
    // the remainder polynomial.  If smaller, the coefficient is assumed to
    // be zero.
    static Real REMAINDER_EPSILON;
    void Divide (const Polynomial& rkDiv, Polynomial& rkQuot,
        Polynomial& rkRem);

    // root finding
    static int DIGITS_ACCURACY;
    static Real ZERO_TOLERANCE;
    bool Bisection (Real fXMin, Real fXMax, Real& rfRoot) const;
    void GetRootsOnInterval (Real fXMin, Real fXMax,
        int& riCount, Real* afRoot) const;
    void GetAllRoots (int& riCount, Real* afRoot) const;

    Real GetRootBound () const;  // real roots must be in [-bound,bound]
    bool AllRealPartsNegative () const;
    bool AllRealPartsPositive () const;

    // low-degree root finding (degree must be correct)
    bool RootsDegree2 (int& riCount, Real afRoot[2]) const;
    bool RootsDegree3 (int& riCount, Real afRoot[3]) const;
    bool RootsDegree4 (int& riCount, Real afRoot[4]) const;

    // solve A*r^3 + B*r = C where A > 0 and B > 0
    Real SpecialCubeRoot (Real fA, Real fB, Real fC);

    // Count the number of roots on [t0,t1].  Uses Sturm sequences to do the
    // counting.  It is allowed to pass in t0 = -MgcMath::MAXREAL or
    // t1 = +MgcMath::MAXREAL.  The value of STURM_EPSILON is used as a
    // threshold on the value of a sturm polynomial at an end point.  If
    // smaller, that value is assumed to be zero.  The return value is the
    // number of roots.  If there are infinitely many, -1 is returned.
    static Real STURM_EPSILON;
    int GetRootCount (Real fT0, Real fT1);

protected:
    int m_iDegree;
    Real* m_afCoeff;

    // root finding
    static const Real ms_fInvLog2;
    static const Real ms_fLog10;
    static const Real ms_fThird;
    static const Real ms_fSqrt3;
    static const Real ms_fTwentySeventh;
    static bool AllRealPartsNegative (int iDegree, Real* afCoeff);
};

} // namespace Mgc

#endif


