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

#ifndef MGCBEZIERCURVE3_H
#define MGCBEZIERCURVE3_H

#include "MgcSingleCurve3.h"

namespace Mgc {


class MAGICFM BezierCurve3 : public SingleCurve3
{
public:
    // Construction and destruction.  BezierCurve3 accepts responsibility for
    // deleting the input arrays.
    BezierCurve3 (int iDegree, Vector3* akCtrlPoint);
    virtual ~BezierCurve3 ();

    int GetDegree () const;
    const Vector3* GetControlPoints () const;

    virtual Vector3 GetPosition (Real fTime) const;
    virtual Vector3 GetFirstDerivative (Real fTime) const;
    virtual Vector3 GetSecondDerivative (Real fTime) const;
    virtual Vector3 GetThirdDerivative (Real fTime) const;

    virtual Real GetVariation (Real fT0, Real fT1, const Vector3* pkP0 = 0,
        const Vector3* pkP1 = 0) const;

protected:
    int m_iDegree;
    int m_iNumCtrlPoints;
    Vector3* m_akCtrlPoint;
    Vector3* m_akDer1CtrlPoint;
    Vector3* m_akDer2CtrlPoint;
    Vector3* m_akDer3CtrlPoint;
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

#include "MgcBezierCurve3.inl"

} // namespace Mgc

#endif


