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

#include "MgcCurve2.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Curve2::Curve2 (Real fTMin, Real fTMax)
{
    m_fTMin = fTMin;
    m_fTMax = fTMax;
}
//----------------------------------------------------------------------------
Curve2::~Curve2 ()
{
}
//----------------------------------------------------------------------------
Real Curve2::GetSpeed (Real fTime) const
{
    Vector2 kVelocity = GetFirstDerivative(fTime);
    Real fSpeed = kVelocity.Length();
    return fSpeed;
}
//----------------------------------------------------------------------------
Real Curve2::GetTotalLength () const
{
    return GetLength(m_fTMin,m_fTMax);
}
//----------------------------------------------------------------------------
Vector2 Curve2::GetTangent (Real fTime) const
{
    Vector2 kVelocity = GetFirstDerivative(fTime);
    kVelocity.Unitize();
    return kVelocity;
}
//----------------------------------------------------------------------------
Vector2 Curve2::GetNormal (Real fTime) const
{
    Vector2 kTangent = GetFirstDerivative(fTime);
    kTangent.Unitize();
    Vector2 kNormal = kTangent.Cross();
    return kNormal;
}
//----------------------------------------------------------------------------
void Curve2::GetFrame (Real fTime, Vector2& rkPosition, Vector2& rkTangent,
    Vector2& rkNormal) const
{
    rkPosition = GetPosition(fTime);
    rkTangent = GetFirstDerivative(fTime);
    rkTangent.Unitize();
    rkNormal = rkTangent.Cross();
}
//----------------------------------------------------------------------------
Real Curve2::GetCurvature (Real fTime) const
{
    Vector2 kDer1 = GetFirstDerivative(fTime);
    Vector2 kDer2 = GetSecondDerivative(fTime);
    Real fSpeedSqr = kDer1.SquaredLength();

    const Real fTolerance = 1e-06f;
    if ( fSpeedSqr >= fTolerance )
    {
        Real fNumer = kDer1.x*kDer2.y - kDer1.y*kDer2.x;
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
void Curve2::SubdivideByTime (int iNumPoints, Vector2*& rakPoint) const
{
    assert( iNumPoints >= 2 );
    rakPoint = new Vector2[iNumPoints];

    Real fDelta = (m_fTMax - m_fTMin)/(iNumPoints-1);

    for (int i = 0; i < iNumPoints; i++)
    {
        Real fTime = m_fTMin + fDelta*i;
        rakPoint[i] = GetPosition(fTime);
    }
}
//----------------------------------------------------------------------------
void Curve2::SubdivideByLength (int iNumPoints, Vector2*& rakPoint) const
{
    assert( iNumPoints >= 2 );
    rakPoint = new Vector2[iNumPoints];

    Real fDelta = GetTotalLength()/(iNumPoints-1);

    for (int i = 0; i < iNumPoints; i++)
    {
        Real fLength = fDelta*i;
        Real fTime = GetTime(fLength);
        rakPoint[i] = GetPosition(fTime);
    }
}
//----------------------------------------------------------------------------
void Curve2::SubdivideByVariation (Real fT0, const Vector2& rkP0,
    Real fT1, const Vector2& rkP1, Real fMinVariation,
    int iLevel, int& riNumPoints, PointList*& rpkList) const
{
    if ( iLevel > 0 && GetVariation(fT0,fT1,&rkP0,&rkP1) > fMinVariation )
    {
        // too much variation, subdivide interval
        iLevel--;
        Real fTMid = 0.5f*(fT0+fT1);
        Vector2 kPMid = GetPosition(fTMid);

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
void Curve2::SubdivideByVariation (Real fMinVariation, int iMaxLevel,
    int& riNumPoints, Vector2*& rakPoint) const
{
    // compute end points of curve
    Vector2 kPMin = GetPosition(m_fTMin);
    Vector2 kPMax = GetPosition(m_fTMax);

    // add left end point to list
    PointList* pkList = new PointList(kPMin,0);
    riNumPoints = 1;

    // binary subdivision, leaf nodes add right end point of subinterval
    SubdivideByVariation(m_fTMin,kPMin,m_fTMax,kPMax,fMinVariation,
        iMaxLevel,riNumPoints,pkList->m_kNext);

    // repackage points in an array
    assert( riNumPoints >= 2 );
    rakPoint = new Vector2[riNumPoints];
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


