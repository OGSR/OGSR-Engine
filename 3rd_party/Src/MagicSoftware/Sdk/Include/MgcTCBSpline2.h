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

#ifndef MGCTCBSPLINE2_H
#define MGCTCBSPLINE2_H

#include "MgcMultipleCurve2.h"

namespace Mgc {


class MAGICFM TCBSpline2 : public MultipleCurve2
{
public:
    // Construction and destruction.  TCBSpline2 accepts responsibility for
    // deleting the input arrays.
    TCBSpline2 (int iSegments, Real* afTime, Vector2* akPoint,
        Real* afTension, Real* afContinuity, Real* afBias);

    virtual ~TCBSpline2 ();


    const Vector2* GetPoints () const;
    const Real* GetTensions () const;
    const Real* GetContinuities () const;
    const Real* GetBiases () const;

    virtual Vector2 GetPosition (Real fTime) const;
    virtual Vector2 GetFirstDerivative (Real fTime) const;
    virtual Vector2 GetSecondDerivative (Real fTime) const;
    virtual Vector2 GetThirdDerivative (Real fTime) const;

protected:
    void ComputePoly (int i0, int i1, int i2, int i3);

    virtual Real GetSpeedKey (int iKey, Real fTime) const;
    virtual Real GetLengthKey (int iKey, Real fT0, Real fT1) const;
    virtual Real GetVariationKey (int iKey, Real fT0, Real fT1,
        const Vector2& rkA, const Vector2& rkB) const;

    Vector2* m_akPoint;
    Real* m_afTension;
    Real* m_afContinuity;
    Real* m_afBias;
    Vector2* m_akA;
    Vector2* m_akB;
    Vector2* m_akC;
    Vector2* m_akD;
};

#include "MgcTCBSpline2.inl"

} // namespace Mgc

#endif


