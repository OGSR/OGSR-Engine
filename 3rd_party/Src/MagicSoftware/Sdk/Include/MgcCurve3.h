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

#ifndef MGCCURVE3_H
#define MGCCURVE3_H

#include "MgcVector3.h"

namespace Mgc {


class MAGICFM Curve3
{
public:
    // abstract base class
    Curve3 (Real fTMin, Real fTMax);
    virtual ~Curve3 ();

    // interval on which curve parameter is defined
    Real GetMinTime () const;
    Real GetMaxTime () const;

    // position and derivatives
    virtual Vector3 GetPosition (Real fTime) const = 0;
    virtual Vector3 GetFirstDerivative (Real fTime) const = 0;
    virtual Vector3 GetSecondDerivative (Real fTime) const = 0;
    virtual Vector3 GetThirdDerivative (Real fTime) const = 0;

    // differential geometric quantities
    Real GetSpeed (Real fTime) const;
    virtual Real GetLength (Real fT0, Real fT1) const = 0;
    Real GetTotalLength () const;
    Vector3 GetTangent (Real fTime) const;
    Vector3 GetNormal (Real fTime) const;
    Vector3 GetBinormal (Real fTime) const;
    void GetFrame (Real fTime, Vector3& rkPosition, Vector3& rkTangent,
        Vector3& rkNormal, Vector3& rkBinormal) const;
    Real GetCurvature (Real fTime) const;
    Real GetTorsion (Real fTime) const;

    // inverse mapping of s = Length(t) given by t = Length^{-1}(s)
    virtual Real GetTime (Real fLength, int iIterations = 32,
        Real fTolerance = 1e-06f) const = 0;

    // subdivision
    void SubdivideByTime (int iNumPoints, Vector3*& rakPoint) const;
    void SubdivideByLength (int iNumPoints, Vector3*& rakPoint) const;

    // Subdivision by variation. The pointers pkP0 and pkP1 correspond to the
    // curve points at fT0 and fT1.  If the pointer values are not null, the
    // assumption is that the caller has passed in the curve points.
    // Otherwise, the function computes the curve points.
    virtual Real GetVariation (Real fT0, Real fT1, const Vector3* pkP0 = 0,
        const Vector3* pkP1 = 0) const = 0;
    void SubdivideByVariation (Real fMinVariation, int iMaxLevel,
        int& riNumPoints, Vector3*& rakPoint) const;

protected:
    // curve parameter is t where tmin <= t <= tmax
    Real m_fTMin, m_fTMax;

    // subdivision
    class MAGICFM PointList
    {
    public:
        PointList (const Vector3& rkPoint, PointList* pkNext)
        {
            m_kPoint = rkPoint;
            m_kNext = pkNext;
        }

        Vector3 m_kPoint;
        PointList* m_kNext;
    };

    void SubdivideByVariation (Real fT0, const Vector3& rkP0, Real fT1,
        const Vector3& rkP1, Real kMinVariation, int iLevel,
        int& riNumPoints, PointList*& rpkList) const;
};

#include "MgcCurve3.inl"

} // namespace Mgc

#endif


