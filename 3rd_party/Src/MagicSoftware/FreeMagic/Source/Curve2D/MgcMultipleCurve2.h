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

#ifndef MGCMULTIPLECURVE2_H
#define MGCMULTIPLECURVE2_H

#include "MgcCurve2.h"

namespace Mgc {


class MAGICFM MultipleCurve2 : public Curve2
{
public:
    // Construction and destruction for abstract base class.  MultipleCurve2
    // accepts responsibility for deleting the input array.
    MultipleCurve2 (int iSegments, Real* afTime);
    virtual ~MultipleCurve2 ();

    // member access
    int GetSegments () const;
    const Real* GetTimes () const;

    // length-from-time and time-from-length
    virtual Real GetLength (Real fT0, Real fT1) const;
    virtual Real GetTime (Real fLength, int iIterations = 32,
        Real fTolerance = 1e-06f) const;

    // support for subdivision
    virtual Real GetVariation (Real fT0, Real fT1, const Vector2* pkP0 = 0,
        const Vector2* pkP1 = 0) const;

protected:
    int m_iSegments;
    Real* m_afTime;

    // These quantities are allocated by GetLength when they are needed the
    // first time.  The allocations occur in InitializeLength (called by
    // GetLength), so this member function must be 'const'. In order to
    // allocate the arrays in a 'const' function, they must be declared as
    // 'mutable'.
    mutable Real* m_afLength;
    mutable Real* m_afAccumLength;

    void GetKeyInfo (Real fTime, int& riKey, Real& rfDt) const;

    void InitializeLength () const;
    virtual Real GetSpeedKey (int iKey, Real fTime) const = 0;
    virtual Real GetLengthKey (int iKey, Real fT0, Real fT1) const = 0;
    virtual Real GetVariationKey (int iKey, Real fT0, Real fT1,
        const Vector2& rkA, const Vector2& rkB) const = 0;

    static Real GetSpeedWithData (Real fTime, void* pvData);
};

} // namespace Mgc

#endif



