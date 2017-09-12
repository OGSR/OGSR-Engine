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

#ifndef MGCTCBSPLINE3_H
#define MGCTCBSPLINE3_H

#include "MgcMultipleCurve3.h"

namespace Mgc {


class MAGICFM TCBSpline3 : public MultipleCurve3
{
public:
    // Construction and destruction.  TCBSpline3 accepts responsibility for
    // deleting the input arrays.
    TCBSpline3 (int iSegments, Real* afTime, Vector3* akPoint,
        Real* afTension, Real* afContinuity, Real* afBias);

    virtual ~TCBSpline3 ();

    const Vector3* GetPoints () const;
    const Real* GetTensions () const;
    const Real* GetContinuities () const;
    const Real* GetBiases () const;

    virtual Vector3 GetPosition (Real fTime) const;
    virtual Vector3 GetFirstDerivative (Real fTime) const;
    virtual Vector3 GetSecondDerivative (Real fTime) const;
    virtual Vector3 GetThirdDerivative (Real fTime) const;

protected:
    void ComputePoly (int i0, int i1, int i2, int i3);

    virtual Real GetSpeedKey (int iKey, Real fTime) const;
    virtual Real GetLengthKey (int iKey, Real fT0, Real fT1) const;
    virtual Real GetVariationKey (int iKey, Real fT0, Real fT1,
        const Vector3& rkA, const Vector3& rkB) const;

    Vector3* m_akPoint;
    Real* m_afTension;
    Real* m_afContinuity;
    Real* m_afBias;
    Vector3* m_akA;
    Vector3* m_akB;
    Vector3* m_akC;
    Vector3* m_akD;
};

#include "MgcTCBSpline3.inl"

} // namespace Mgc

#endif


