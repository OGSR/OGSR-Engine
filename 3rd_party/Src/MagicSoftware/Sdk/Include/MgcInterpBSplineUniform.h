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

#ifndef MGCINTERPBSPLINEUNIFORM_H
#define MGCINTERPBSPLINEUNIFORM_H

#include "MgcMath.h"

namespace Mgc {

class MAGICFM InterpBSplineUniform
{
public:
    // Construction and destruction.  InterpBSplineUniform accepts
    // responsibility for deleting the input array afData.  The input array
    // aiDim is copied.
    InterpBSplineUniform (int iDims, int iDegree, const int* aiDim,
        Real* afData);
    virtual ~InterpBSplineUniform ();

    int GetDimension () const;
    int GetDegree () const;
    Real GetDomainMin (int i) const;
    Real GetDomainMax (int i) const;
    int GetGridMin (int i) const;
    int GetGridMax (int i) const;

    // spline evaluation for function interpolation (no derivatives)
    virtual Real operator() (Real* afX) = 0;

    // spline evaluation, derivative counts given in aiDx[]
    virtual Real operator() (int* aiDx, Real* afX) = 0;

protected:
    int m_iDims;         // N, number of dimensions
    int m_iDegree;       // D, degree of polynomial spline
    int m_iDp1;          // D+1
    int m_iDp1ToN;       // power(D+1,N)
    int m_iDp1To2N;      // power(D+1,2N)
    int* m_aiDim;        // dimension sizes dim[0] through dim[N-1]
    Real* m_afData;      // N-dimensional array of data
    Real* m_afDomMin;    // minimum allowed value of spline input vector
    Real* m_afDomMax;    // maximum allowed value of spline input vector
    int* m_aiGridMin;    // minimum allowed value for current local grid
    int* m_aiGridMax;    // maximum allowed value for current local grid
    int* m_aiBase;       // base indices for grid of local control points
    int* m_aiOldBase;    // old base indices for grid of local control points
    Real** m_aafMatrix;  // (D+1)x(D+1) blending matrix
    Real* m_afCache;     // cache for subblock of data
    Real* m_afInter;     // intermediate product of data with blending matrix
    Real** m_aafPoly;    // poly[N][D+1], store polynomials and derivatives
    Real** m_aafCoeff;   // coefficients for polynomial construction
    Real* m_afProduct;   // outer tensor product of m with itself N times
    int* m_aiSkip;       // for skipping zero values of mtensor

    Real (*m_oEvaluateCallback)(int);

    virtual void EvaluateUnknownData () = 0;
    virtual void ComputeIntermediate () = 0;
    void SetPolynomial (int iOrder, Real fDiff, Real* afPoly);

    static int Choose (int iN, int iK);  // n choose k
    static Real** BlendMatrix (int iDegree);
};

#include "MgcInterpBSplineUniform.inl"

} // namespace Mgc

#endif
