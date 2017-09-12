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

#ifndef MGCNATURALSPLINE2_H
#define MGCNATURALSPLINE2_H

#include "MgcMultipleCurve2.h"

namespace Mgc {


class MAGICFM NaturalSpline2 : public MultipleCurve2
{
public:
    enum BoundaryType
    {
        BT_FREE,
        BT_CLAMPED,
        BT_CLOSED
    };

    // Construction and destruction.  NaturalSpline2 accepts responsibility
    // for deleting the input arrays.
    NaturalSpline2 (BoundaryType eType, int iSegments, Real* afTime,
        Vector2* akPoint);

    virtual ~NaturalSpline2 ();


    const Vector2* GetPoints () const;

    virtual Vector2 GetPosition (Real fTime) const;
    virtual Vector2 GetFirstDerivative (Real fTime) const;
    virtual Vector2 GetSecondDerivative (Real fTime) const;
    virtual Vector2 GetThirdDerivative (Real fTime) const;

protected:
    void CreateFreeSpline ();
    void CreateClampedSpline ();
    void CreateClosedSpline ();

    virtual Real GetSpeedKey (int iKey, Real fTime) const;
    virtual Real GetLengthKey (int iKey, Real fT0, Real fT1) const;
    virtual Real GetVariationKey (int iKey, Real fT0, Real fT1,
        const Vector2& rkA, const Vector2& rkB) const;

    Vector2* m_akA;
    Vector2* m_akB;
    Vector2* m_akC;
    Vector2* m_akD;
};

#include "MgcNaturalSpline2.inl"

} // namespace Mgc

#endif


