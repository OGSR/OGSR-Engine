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

#include "MgcIntr3DLinBox.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Segment3& rkSegment, const Box3& rkBox)
{
    Real fAWdU[3], fADdU[3], fAWxDdU[3], fRhs;
    Vector3 kSDir = 0.5f*rkSegment.Direction();
    Vector3 kSCen = rkSegment.Origin() + kSDir;

    Vector3 kDiff = kSCen - rkBox.Center();

    fAWdU[0] = Math::FAbs(kSDir.Dot(rkBox.Axis(0)));
    fADdU[0] = Math::FAbs(kDiff.Dot(rkBox.Axis(0)));
    fRhs = rkBox.Extent(0) + fAWdU[0];
    if ( fADdU[0] > fRhs )
        return false;

    fAWdU[1] = Math::FAbs(kSDir.Dot(rkBox.Axis(1)));
    fADdU[1] = Math::FAbs(kDiff.Dot(rkBox.Axis(1)));
    fRhs = rkBox.Extent(1) + fAWdU[1];
    if ( fADdU[1] > fRhs )
        return false;

    fAWdU[2] = Math::FAbs(kSDir.Dot(rkBox.Axis(2)));
    fADdU[2] = Math::FAbs(kDiff.Dot(rkBox.Axis(2)));
    fRhs = rkBox.Extent(2) + fAWdU[2];
    if ( fADdU[2] > fRhs )
        return false;

    Vector3 kWxD = kSDir.Cross(kDiff);

    fAWxDdU[0] = Math::FAbs(kWxD.Dot(rkBox.Axis(0)));
    fRhs = rkBox.Extent(1)*fAWdU[2] + rkBox.Extent(2)*fAWdU[1];
    if ( fAWxDdU[0] > fRhs )
        return false;

    fAWxDdU[1] = Math::FAbs(kWxD.Dot(rkBox.Axis(1)));
    fRhs = rkBox.Extent(0)*fAWdU[2] + rkBox.Extent(2)*fAWdU[0];
    if ( fAWxDdU[1] > fRhs )
        return false;

    fAWxDdU[2] = Math::FAbs(kWxD.Dot(rkBox.Axis(2)));
    fRhs = rkBox.Extent(0)*fAWdU[1] + rkBox.Extent(1)*fAWdU[0];
    if ( fAWxDdU[2] > fRhs )
        return false;

    return true;
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Ray3& rkRay, const Box3& rkBox)
{
    Real fWdU[3], fAWdU[3], fDdU[3], fADdU[3], fAWxDdU[3], fRhs;

    Vector3 kDiff = rkRay.Origin() - rkBox.Center();

    fWdU[0] = rkRay.Direction().Dot(rkBox.Axis(0));
    fAWdU[0] = Math::FAbs(fWdU[0]);
    fDdU[0] = kDiff.Dot(rkBox.Axis(0));
    fADdU[0] = Math::FAbs(fDdU[0]);
    if ( fADdU[0] > rkBox.Extent(0) && fDdU[0]*fWdU[0] >= 0.0f )
        return false;

    fWdU[1] = rkRay.Direction().Dot(rkBox.Axis(1));
    fAWdU[1] = Math::FAbs(fWdU[1]);
    fDdU[1] = kDiff.Dot(rkBox.Axis(1));
    fADdU[1] = Math::FAbs(fDdU[1]);
    if ( fADdU[1] > rkBox.Extent(1) && fDdU[1]*fWdU[1] >= 0.0f )
        return false;

    fWdU[2] = rkRay.Direction().Dot(rkBox.Axis(2));
    fAWdU[2] = Math::FAbs(fWdU[2]);
    fDdU[2] = kDiff.Dot(rkBox.Axis(2));
    fADdU[2] = Math::FAbs(fDdU[2]);
    if ( fADdU[2] > rkBox.Extent(2) && fDdU[2]*fWdU[2] >= 0.0f )
        return false;

    Vector3 kWxD = rkRay.Direction().Cross(kDiff);

    fAWxDdU[0] = Math::FAbs(kWxD.Dot(rkBox.Axis(0)));
    fRhs = rkBox.Extent(1)*fAWdU[2] + rkBox.Extent(2)*fAWdU[1];
    if ( fAWxDdU[0] > fRhs )
        return false;

    fAWxDdU[1] = Math::FAbs(kWxD.Dot(rkBox.Axis(1)));
    fRhs = rkBox.Extent(0)*fAWdU[2] + rkBox.Extent(2)*fAWdU[0];
    if ( fAWxDdU[1] > fRhs )
        return false;

    fAWxDdU[2] = Math::FAbs(kWxD.Dot(rkBox.Axis(2)));
    fRhs = rkBox.Extent(0)*fAWdU[1] + rkBox.Extent(1)*fAWdU[0];
    if ( fAWxDdU[2] > fRhs )
        return false;

    return true;
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Line3& rkLine, const Box3& rkBox)
{
    Real fAWdU[3], fAWxDdU[3], fRhs;

    Vector3 kDiff = rkLine.Origin() - rkBox.Center();
    Vector3 kWxD = rkLine.Direction().Cross(kDiff);

    fAWdU[1] = Math::FAbs(rkLine.Direction().Dot(rkBox.Axis(1)));
    fAWdU[2] = Math::FAbs(rkLine.Direction().Dot(rkBox.Axis(2)));
    fAWxDdU[0] = Math::FAbs(kWxD.Dot(rkBox.Axis(0)));
    fRhs = rkBox.Extent(1)*fAWdU[2] + rkBox.Extent(2)*fAWdU[1];
    if ( fAWxDdU[0] > fRhs )
        return false;

    fAWdU[0] = Math::FAbs(rkLine.Direction().Dot(rkBox.Axis(0)));
    fAWxDdU[1] = Math::FAbs(kWxD.Dot(rkBox.Axis(1)));
    fRhs = rkBox.Extent(0)*fAWdU[2] + rkBox.Extent(2)*fAWdU[0];
    if ( fAWxDdU[1] > fRhs )
        return false;

    fAWxDdU[2] = Math::FAbs(kWxD.Dot(rkBox.Axis(2)));
    fRhs = rkBox.Extent(0)*fAWdU[1] + rkBox.Extent(1)*fAWdU[0];
    if ( fAWxDdU[2] > fRhs )
        return false;

    return true;
}
//----------------------------------------------------------------------------
static bool Clip (Real fDenom, Real fNumer, Real& rfT0, Real& rfT1)
{
    // Return value is 'true' if line segment intersects the current test
    // plane.  Otherwise 'false' is returned in which case the line segment
    // is entirely clipped.

    if ( fDenom > 0.0f )
    {
        if ( fNumer > fDenom*rfT1 )
            return false;
        if ( fNumer > fDenom*rfT0 )
            rfT0 = fNumer/fDenom;
        return true;
    }
    else if ( fDenom < 0.0f )
    {
        if ( fNumer > fDenom*rfT0 )
            return false;
        if ( fNumer > fDenom*rfT1 )
            rfT1 = fNumer/fDenom;
        return true;
    }
    else
    {
        return fNumer <= 0.0f;
    }
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Vector3& rkOrigin,
    const Vector3& rkDirection, const Real afExtent[3], Real& rfT0,
    Real& rfT1)
{
    Real fSaveT0 = rfT0, fSaveT1 = rfT1;

    bool bNotEntirelyClipped =
        Clip(+rkDirection.x,-rkOrigin.x-afExtent[0],rfT0,rfT1) &&
        Clip(-rkDirection.x,+rkOrigin.x-afExtent[0],rfT0,rfT1) &&
        Clip(+rkDirection.y,-rkOrigin.y-afExtent[1],rfT0,rfT1) &&
        Clip(-rkDirection.y,+rkOrigin.y-afExtent[1],rfT0,rfT1) &&
        Clip(+rkDirection.z,-rkOrigin.z-afExtent[2],rfT0,rfT1) &&
        Clip(-rkDirection.z,+rkOrigin.z-afExtent[2],rfT0,rfT1);

    return bNotEntirelyClipped && ( rfT0 != fSaveT0 || rfT1 != fSaveT1 );
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Segment3& rkSegment, const Box3& rkBox,
    int& riQuantity, Vector3 akPoint[2])
{
    // convert segment to box coordinates
    Vector3 kDiff = rkSegment.Origin() - rkBox.Center();
    Vector3 kOrigin(
        kDiff.Dot(rkBox.Axis(0)),
        kDiff.Dot(rkBox.Axis(1)),
        kDiff.Dot(rkBox.Axis(2))
    );
    Vector3 kDirection(
        rkSegment.Direction().Dot(rkBox.Axis(0)),
        rkSegment.Direction().Dot(rkBox.Axis(1)),
        rkSegment.Direction().Dot(rkBox.Axis(2))
    );

    Real fT0 = 0.0f, fT1 = 1.0f;
    bool bIntersects = FindIntersection(kOrigin,kDirection,rkBox.Extents(),
        fT0,fT1);

    if ( bIntersects )
    {
        if ( fT0 > 0.0f )
        {
            if ( fT1 < 1.0f )
            {
                riQuantity = 2;
                akPoint[0] = rkSegment.Origin() + fT0*rkSegment.Direction();
                akPoint[1] = rkSegment.Origin() + fT1*rkSegment.Direction();
            }
            else
            {
                riQuantity = 1;
                akPoint[0] = rkSegment.Origin() + fT0*rkSegment.Direction();
            }
        }
        else  // fT0 == 0
        {
            if ( fT1 < 1.0f )
            {
                riQuantity = 1;
                akPoint[0] = rkSegment.Origin() + fT1*rkSegment.Direction();
            }
            else  // fT1 == 1
            {
                // segment entirely in box
                riQuantity = 0;
            }
        }
    }
    else
    {
        riQuantity = 0;
    }

    return bIntersects;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Ray3& rkRay, const Box3& rkBox,
    int& riQuantity, Vector3 akPoint[2])
{
    // convert ray to box coordinates
    Vector3 kDiff = rkRay.Origin() - rkBox.Center();
    Vector3 kOrigin(
        kDiff.Dot(rkBox.Axis(0)),
        kDiff.Dot(rkBox.Axis(1)),
        kDiff.Dot(rkBox.Axis(2))
    );
    Vector3 kDirection(
        rkRay.Direction().Dot(rkBox.Axis(0)),
        rkRay.Direction().Dot(rkBox.Axis(1)),
        rkRay.Direction().Dot(rkBox.Axis(2))
    );

    Real fT0 = 0.0f, fT1 = Math::MAX_REAL;
    bool bIntersects = FindIntersection(kOrigin,kDirection,rkBox.Extents(),
        fT0,fT1);

    if ( bIntersects )
    {
        if ( fT0 > 0.0f )
        {
            riQuantity = 2;
            akPoint[0] = rkRay.Origin() + fT0*rkRay.Direction();
            akPoint[1] = rkRay.Origin() + fT1*rkRay.Direction();
        }
        else  // fT0 == 0
        {
            riQuantity = 1;
            akPoint[0] = rkRay.Origin() + fT1*rkRay.Direction();
        }
    }
    else
    {
        riQuantity = 0;
    }

    return bIntersects;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Line3& rkLine, const Box3& rkBox,
    int& riQuantity, Vector3 akPoint[2])
{
    // convert line to box coordinates
    Vector3 kDiff = rkLine.Origin() - rkBox.Center();
    Vector3 kOrigin(
        kDiff.Dot(rkBox.Axis(0)),
        kDiff.Dot(rkBox.Axis(1)),
        kDiff.Dot(rkBox.Axis(2))
    );
    Vector3 kDirection(
        rkLine.Direction().Dot(rkBox.Axis(0)),
        rkLine.Direction().Dot(rkBox.Axis(1)),
        rkLine.Direction().Dot(rkBox.Axis(2))
    );

    Real fT0 = -Math::MAX_REAL, fT1 = Math::MAX_REAL;
    bool bIntersects = FindIntersection(kOrigin,kDirection,rkBox.Extents(),
        fT0,fT1);

    if ( bIntersects )
    {
        if ( fT0 != fT1 )
        {
            riQuantity = 2;
            akPoint[0] = rkLine.Origin() + fT0*rkLine.Direction();
            akPoint[1] = rkLine.Origin() + fT1*rkLine.Direction();
        }
        else
        {
            riQuantity = 1;
            akPoint[0] = rkLine.Origin() + fT0*rkLine.Direction();
        }
    }
    else
    {
        riQuantity = 0;
    }

    return bIntersects;
}
//----------------------------------------------------------------------------


