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

#ifndef MGCBEZIERCURVE2_H
#define MGCBEZIERCURVE2_H

#include "MgcSingleCurve2.h"

namespace Mgc {


class MAGICFM BezierCurve2 : public SingleCurve2
{
public:
    // Construction and destruction.  BezierCurve2 accepts responsibility for
    // deleting the input array.
    BezierCurve2 (int iDegree, Vector2* akCtrlPoint);
    virtual ~BezierCurve2 ();

    int GetDegree () const;
    const Vector2* GetControlPoints () const;

    virtual Vector2 GetPosition (Real fTime) const;
    virtual Vector2 GetFirstDerivative (Real fTime) const;
    virtual Vector2 GetSecondDerivative (Real fTime) const;
    virtual Vector2 GetThirdDerivative (Real fTime) const;

    virtual Real GetVariation (Real fT0, Real fT1, const Vector2* pkP0 = 0,
        const Vector2* pkP1 = 0) const;

protected:
    int m_iDegree;
    int m_iNumCtrlPoints;
    Vector2* m_akCtrlPoint;
    Vector2* m_akDer1CtrlPoint;
    Vector2* m_akDer2CtrlPoint;
    Vector2* m_akDer3CtrlPoint;
    Real** m_aafChoose;

    // variation support
    int m_iTwoDegree;
    int m_iTwoDegreePlusOne;
    Real* m_afSigma;
    Real* m_afRecip;
    Real* m_afPowT0;
    Real* m_afPowOmT0;
    Real* m_afPowT1;
    Real* m_afPowOmT1;
};

#include "MgcBezierCurve2.inl"

} // namespace Mgc

#endif


