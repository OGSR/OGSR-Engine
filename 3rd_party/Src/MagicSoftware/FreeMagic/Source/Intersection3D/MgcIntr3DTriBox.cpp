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

#include "MgcIntr3DTriBox.h"
using namespace Mgc;

//----------------------------------------------------------------------------
static void ProjectTriangle (const Vector3& rkD, const Vector3* apkTri[3],
    Real& rfMin, Real& rfMax)
{
    rfMin = rkD.Dot(*apkTri[0]);
    rfMax = rfMin;

    Real fDot = rkD.Dot(*apkTri[1]);
    if ( fDot < rfMin )
        rfMin = fDot;
    else if ( fDot > rfMax )
        rfMax = fDot;

    fDot = rkD.Dot(*apkTri[2]);
    if ( fDot < rfMin )
        rfMin = fDot;
    else if ( fDot > rfMax )
        rfMax = fDot;
}
//----------------------------------------------------------------------------
static void ProjectBox (const Vector3& rkD, const Box3& rkBox, Real& rfMin,
    Real& rfMax)
{
    Real fDdC = rkD.Dot(rkBox.Center());
    Real fR =
        rkBox.Extent(0)*Math::FAbs(rkD.Dot(rkBox.Axis(0))) +
        rkBox.Extent(1)*Math::FAbs(rkD.Dot(rkBox.Axis(1))) +
        rkBox.Extent(2)*Math::FAbs(rkD.Dot(rkBox.Axis(2)));
    rfMin = fDdC - fR;
    rfMax = fDdC + fR;
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Vector3* apkTri[3], const Box3& rkBox)
{
    Real fMin0, fMax0, fMin1, fMax1;
    Vector3 kD, akE[3];

    // test direction of triangle normal
    akE[0] = (*apkTri[1]) - (*apkTri[0]);
    akE[1] = (*apkTri[2]) - (*apkTri[0]);
    kD = akE[0].Cross(akE[1]);
    fMin0 = kD.Dot(*apkTri[0]);
    fMax0 = fMin0;
    ProjectBox(kD,rkBox,fMin1,fMax1);
    if ( fMax1 < fMin0 || fMax0 < fMin1 )
        return false;

    // test direction of box faces
    for (int i = 0; i < 3; i++)
    {
        kD = rkBox.Axis(i);
        ProjectTriangle(kD,apkTri,fMin0,fMax0);
        Real fDdC = kD.Dot(rkBox.Center());
        fMin1 = fDdC - rkBox.Extent(i);
        fMax1 = fDdC + rkBox.Extent(i);
        if ( fMax1 < fMin0 || fMax0 < fMin1 )
            return false;
    }

    // test direction of triangle-box edge cross products
    akE[2] = akE[1] - akE[0];
    for (int i0 = 0; i0 < 3; i0++)
    {
        for (int i1 = 0; i1 < 3; i1++)
        {
            kD = akE[i0].Cross(rkBox.Axis(i1));
            ProjectTriangle(kD,apkTri,fMin0,fMax0);
            ProjectBox(kD,rkBox,fMin1,fMax1);
            if ( fMax1 < fMin0 || fMax0 < fMin1 )
                return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
static bool NoIntersect (Real fTMax, Real fSpeed, Real fMin0, Real fMax0,
    Real fMin1, Real fMax1, Real& rfTFirst, Real& rfTLast)
{
    Real fInvSpeed, fT;

    if ( fMax1 < fMin0 )  // C1 initially on left of C0
    {
        if ( fSpeed <= 0.0f )
        {
            // intervals moving apart
            return true;
        }

        fInvSpeed = 1.0f/fSpeed;

        fT = (fMin0 - fMax1)*fInvSpeed;
        if ( fT > rfTFirst )
            rfTFirst = fT;
        if ( rfTFirst > fTMax )
            return true;

        fT = (fMax0 - fMin1)*fInvSpeed;
        if ( fT < rfTLast )
            rfTLast = fT;
        if ( rfTFirst > rfTLast )
            return true;
    }
    else if ( fMax0 < fMin1 )  // C1 initially on right of C0
    {
        if ( fSpeed >= 0.0f )
        {
            // intervals moving apart
            return true;
        }

        fInvSpeed = 1.0f/fSpeed;

        fT = (fMax0 - fMin1)*fInvSpeed;
        if ( fT > rfTFirst )
            rfTFirst = fT;
        if ( rfTFirst > fTMax )
            return true;

        fT = (fMin0 - fMax1)*fInvSpeed;
        if ( fT < rfTLast )
            rfTLast = fT;
        if ( rfTFirst > rfTLast )
            return true;
    }
    else  // C0 and C1 overlap
    {
        if ( fSpeed > 0.0f )
        {
            fT = (fMax0 - fMin1)/fSpeed;
            if ( fT < rfTLast )
                rfTLast = fT;
            if ( rfTFirst > rfTLast )
                return true;
        }
        else if ( fSpeed < 0.0f )
        {
            fT = (fMin0 - fMax1)/fSpeed;
            if ( fT < rfTLast )
                rfTLast = fT;
            if ( rfTFirst > rfTLast )
                return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Vector3* apkTri[3], const Vector3& rkTriVel,
    const Box3& rkBox, const Vector3& rkBoxVel, Real fTMax, Real& rfTFirst,
    Real& rfTLast)
{
    Real fMin0, fMax0, fMin1, fMax1, fSpeed;
    Vector3 kD, akE[3];

    // process as if triangle is stationary, box is moving
    Vector3 kW = rkBoxVel - rkTriVel;
    rfTFirst = 0.0f;
    rfTLast = Math::MAX_REAL;

    // test direction of triangle normal
    akE[0] = (*apkTri[1]) - (*apkTri[0]);
    akE[1] = (*apkTri[2]) - (*apkTri[0]);
    kD = akE[0].Cross(akE[1]);
    fMin0 = kD.Dot(*apkTri[0]);
    fMax0 = fMin0;
    ProjectBox(kD,rkBox,fMin1,fMax1);
    fSpeed = kD.Dot(kW);
    if ( NoIntersect(fTMax,fSpeed,fMin0,fMax0,fMin1,fMax1,rfTFirst,rfTLast) )
        return false;

    // test direction of box faces
    for (int i = 0; i < 3; i++)
    {
        kD = rkBox.Axis(i);
        ProjectTriangle(kD,apkTri,fMin0,fMax0);
        Real fDdC = kD.Dot(rkBox.Center());
        fMin1 = fDdC - rkBox.Extent(i);
        fMax1 = fDdC + rkBox.Extent(i);
        fSpeed = kD.Dot(kW);
        if ( NoIntersect(fTMax,fSpeed,fMin0,fMax0,fMin1,fMax1,rfTFirst,
             rfTLast) )
        {
            return false;
        }
    }

    // test direction of triangle-box edge cross products
    akE[2] = akE[1] - akE[0];
    for (int i0 = 0; i0 < 3; i0++)
    {
        for (int i1 = 0; i1 < 3; i1++)
        {
            kD = akE[i0].Cross(rkBox.Axis(i1));
            ProjectTriangle(kD,apkTri,fMin0,fMax0);
            ProjectBox(kD,rkBox,fMin1,fMax1);
            fSpeed = kD.Dot(kW);
            if ( NoIntersect(fTMax,fSpeed,fMin0,fMax0,fMin1,fMax1,rfTFirst,
                 rfTLast) )
            {
                return false;
            }
        }
    }

    return true;
}
//----------------------------------------------------------------------------

