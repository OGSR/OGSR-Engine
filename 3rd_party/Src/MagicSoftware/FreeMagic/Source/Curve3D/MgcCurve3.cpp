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

#include "MgcCurve3.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Curve3::Curve3 (Real fTMin, Real fTMax)
{
    m_fTMin = fTMin;
    m_fTMax = fTMax;
}
//----------------------------------------------------------------------------
Curve3::~Curve3 ()
{
}
//----------------------------------------------------------------------------
Real Curve3::GetSpeed (Real fTime) const
{
    Vector3 kVelocity = GetFirstDerivative(fTime);
    Real fSpeed = kVelocity.Length();
    return fSpeed;
}
//----------------------------------------------------------------------------
Real Curve3::GetTotalLength () const
{
    return GetLength(m_fTMin,m_fTMax);
}
//----------------------------------------------------------------------------
Vector3 Curve3::GetTangent (Real fTime) const
{
    Vector3 kVelocity = GetFirstDerivative(fTime);
    kVelocity.Unitize();
    return kVelocity;
}
//----------------------------------------------------------------------------
Vector3 Curve3::GetNormal (Real fTime) const
{
    Vector3 kVelocity = GetFirstDerivative(fTime);
    Vector3 kAcceleration = GetSecondDerivative(fTime);
    Real fVDotV = kVelocity.Dot(kVelocity);
    Real fVDotA = kVelocity.Dot(kAcceleration);
    Vector3 kNormal = fVDotV*kAcceleration - fVDotA*kVelocity;
    kNormal.Unitize();
    return kNormal;
}
//----------------------------------------------------------------------------
Vector3 Curve3::GetBinormal (Real fTime) const
{
    Vector3 kVelocity = GetFirstDerivative(fTime);
    Vector3 kAcceleration = GetSecondDerivative(fTime);
    Real fVDotV = kVelocity.Dot(kVelocity);
    Real fVDotA = kVelocity.Dot(kAcceleration);
    Vector3 kNormal = fVDotV*kAcceleration - fVDotA*kVelocity;
    kNormal.Unitize();
    kVelocity.Unitize();
    Vector3 kBinormal = kVelocity.Cross(kNormal);
    return kBinormal;
}
//----------------------------------------------------------------------------
void Curve3::GetFrame (Real fTime, Vector3& rkPosition, Vector3& rkTangent,
    Vector3& rkNormal, Vector3& rkBinormal) const
{
    rkPosition = GetPosition(fTime);
    Vector3 kVelocity = GetFirstDerivative(fTime);
    Vector3 kAcceleration = GetSecondDerivative(fTime);
    Real fVDotV = kVelocity.Dot(kVelocity);
    Real fVDotA = kVelocity.Dot(kAcceleration);
    rkNormal = fVDotV*kAcceleration - fVDotA*kVelocity;
    rkNormal.Unitize();
    rkTangent = kVelocity;
    rkTangent.Unitize();
    rkBinormal = rkTangent.Cross(rkNormal);
}
//----------------------------------------------------------------------------
Real Curve3::GetCurvature (Real fTime) const
{
    Vector3 kVelocity = GetFirstDerivative(fTime);
    Real fSpeedSqr = kVelocity.SquaredLength();

    const Real fTolerance = 1e-06f;
    if ( fSpeedSqr >= fTolerance )
    {
        Vector3 kAcceleration = GetSecondDerivative(fTime);
        Vector3 kCross = kVelocity.Cross(kAcceleration);
        Real fNumer = kCross.Length();
        Real fDenom = Math::Pow(fSpeedSqr,1.5f);
        return fNumer/fDenom;
    }
    else
    {
        // curvature is indeterminate, just return 0
        return 0.0f;
    }
}
//----------------------------------------------------------------------------
Real Curve3::GetTorsion (Real fTime) const
{
    Vector3 kVelocity = GetFirstDerivative(fTime);
    Vector3 kAcceleration = GetSecondDerivative(fTime);
    Vector3 kCross = kVelocity.Cross(kAcceleration);
    Real fDenom = kCross.SquaredLength();

    const Real fTolerance = 1e-06f;
    if ( fDenom >= fTolerance )
    {
        Vector3 kJerk = GetThirdDerivative(fTime);
        Real fNumer = kCross.Dot(kJerk);
        return fNumer/fDenom;
    }
    else
    {
        // torsion is indeterminate, just return 0
        return 0.0f;
    }
}
//----------------------------------------------------------------------------
void Curve3::SubdivideByTime (int iNumPoints, Vector3*& rakPoint) const
{
    assert( iNumPoints >= 2 );
    rakPoint = new Vector3[iNumPoints];

    Real fDelta = (m_fTMax - m_fTMin)/(iNumPoints-1);

    for (int i = 0; i < iNumPoints; i++)
    {
        Real fTime = m_fTMin + fDelta*i;
        rakPoint[i] = GetPosition(fTime);
    }
}
//----------------------------------------------------------------------------
void Curve3::SubdivideByLength (int iNumPoints, Vector3*& rakPoint)
    const
{
    assert( iNumPoints >= 2 );
    rakPoint = new Vector3[iNumPoints];

    Real fDelta = GetTotalLength()/(iNumPoints-1);

    for (int i = 0; i < iNumPoints; i++)
    {
        Real fLength = fDelta*i;
        Real fTime = GetTime(fLength);
        rakPoint[i] = GetPosition(fTime);
    }
}
//----------------------------------------------------------------------------
void Curve3::SubdivideByVariation (Real fT0, const Vector3& rkP0,
    Real fT1, const Vector3& rkP1, Real fMinVariation, int iLevel,
    int& riNumPoints, PointList*& rpkList) const
{
    if ( iLevel > 0 && GetVariation(fT0,fT1,&rkP0,&rkP1) > fMinVariation )
    {
        // too much variation, subdivide interval
        iLevel--;
        Real fTMid = 0.5f*(fT0+fT1);
        Vector3 kPMid = GetPosition(fTMid);

        SubdivideByVariation(fT0,rkP0,fTMid,kPMid,fMinVariation,iLevel,
            riNumPoints,rpkList);

        SubdivideByVariation(fTMid,kPMid,fT1,rkP1,fMinVariation,iLevel,
            riNumPoints,rpkList);
    }
    else
    {
        // add right end point, left end point was added by neighbor
        rpkList = new PointList(rkP1,rpkList);
        riNumPoints++;
    }
}
//----------------------------------------------------------------------------
void Curve3::SubdivideByVariation (Real fMinVariation, int iMaxLevel,
    int& riNumPoints, Vector3*& rakPoint) const
{
    // compute end points of curve
    Vector3 kPMin = GetPosition(m_fTMin);
    Vector3 kPMax = GetPosition(m_fTMax);

    // add left end point to list
    PointList* pkList = new PointList(kPMin,0);
    riNumPoints = 1;

    // binary subdivision, leaf nodes add right end point of subinterval
    SubdivideByVariation(m_fTMin,kPMin,m_fTMax,kPMax,fMinVariation,
        iMaxLevel,riNumPoints,pkList->m_kNext);

    // repackage points in an array
    assert( riNumPoints >= 2 );
    rakPoint = new Vector3[riNumPoints];
    for (int i = 0; i < riNumPoints; i++)
    {
        assert( pkList );
        rakPoint[i] = pkList->m_kPoint;

        PointList* pkSave = pkList;
        pkList = pkList->m_kNext;
        delete pkSave;
    }
    assert( pkList == 0 );
}
//----------------------------------------------------------------------------


