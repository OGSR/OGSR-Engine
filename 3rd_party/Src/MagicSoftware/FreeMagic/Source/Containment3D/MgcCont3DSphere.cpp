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

#include "MgcCont3DSphere.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Sphere Mgc::ContSphereOfAABB (int iQuantity, const Vector3* akPoint)
{
    Vector3 kMin = akPoint[0], kMax = kMin;
    for (int i = 1; i < iQuantity; i++)
    {
        if ( akPoint[i].x < kMin.x )
            kMin.x = akPoint[i].x;
        else if ( akPoint[i].x > kMax.x )
            kMax.x = akPoint[i].x;

        if ( akPoint[i].y < kMin.y )
            kMin.y = akPoint[i].y;
        else if ( akPoint[i].y > kMax.y )
            kMax.y = akPoint[i].y;

        if ( akPoint[i].z < kMin.z )
            kMin.z = akPoint[i].z;
        else if ( akPoint[i].z > kMax.z )
            kMax.z = akPoint[i].z;
    }

    Sphere kSphere;
    kSphere.Center() = 0.5f*(kMax + kMin);
    Vector3 kHalfDiagonal = 0.5f*(kMax - kMin);
    kSphere.Radius() = kHalfDiagonal.Length();

    return kSphere;
}
//----------------------------------------------------------------------------
Sphere Mgc::ContSphereAverage (int iQuantity, const Vector3* akPoint)
{
    Vector3 kCenter = akPoint[0];
    int i;
    for (i = 1; i < iQuantity; i++)
        kCenter += akPoint[i];
    Real fInvQuantity = 1.0f/iQuantity;
    kCenter *= fInvQuantity;

    Real fMaxRadiusSqr = 0.0f;
    for (i = 0; i < iQuantity; i++)
    {
        Vector3 kDiff = akPoint[i] - kCenter;
        Real fRadiusSqr = kDiff.SquaredLength();
        if ( fRadiusSqr > fMaxRadiusSqr )
            fMaxRadiusSqr = fRadiusSqr;
    }

    Sphere kSphere;
    kSphere.Center() = kCenter;
    kSphere.Radius() = Math::Sqrt(fMaxRadiusSqr);

    return kSphere;
}
//----------------------------------------------------------------------------
bool Mgc::ContSphereOfAABB (int iQuantity, const Vector3* akPoint,
    const bool* abValid, Sphere& rkSphere)
{
    Vector3 kMin, kMax;
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            kMin = akPoint[i];
            kMax = kMin;
            break;
        }
    }
    if ( i == iQuantity )
        return false;

    for (i++; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            if ( akPoint[i].x < kMin.x )
                kMin.x = akPoint[i].x;
            else if ( akPoint[i].x > kMax.x )
                kMax.x = akPoint[i].x;

            if ( akPoint[i].y < kMin.y )
                kMin.y = akPoint[i].y;
            else if ( akPoint[i].y > kMax.y )
                kMax.y = akPoint[i].y;

            if ( akPoint[i].z < kMin.z )
                kMin.z = akPoint[i].z;
            else if ( akPoint[i].z > kMax.z )
                kMax.z = akPoint[i].z;
        }
    }

    rkSphere.Center() = 0.5f*(kMax + kMin);
    Vector3 kHalfDiagonal = 0.5f*(kMax - kMin);
    rkSphere.Radius() = kHalfDiagonal.Length();

    return true;
}
//----------------------------------------------------------------------------
bool Mgc::ContSphereAverage (int iQuantity, const Vector3* akPoint,
    const bool* abValid, Sphere& rkSphere)
{
    Vector3 kCenter = Vector3::ZERO;
    int i, iValidQuantity = 0;
    for (i = 0; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            kCenter += akPoint[i];
            iValidQuantity++;
        }
    }
    if ( iValidQuantity == 0 )
        return false;

    Real fInvQuantity = 1.0f/iValidQuantity;
    kCenter *= fInvQuantity;

    Real fMaxRadiusSqr = 0.0f;
    for (i = 0; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            Vector3 kDiff = akPoint[i] - kCenter;
            Real fRadiusSqr = kDiff.SquaredLength();
            if ( fRadiusSqr > fMaxRadiusSqr )
                fMaxRadiusSqr = fRadiusSqr;
        }
    }

    rkSphere.Center() = kCenter;
    rkSphere.Radius() = Math::Sqrt(fMaxRadiusSqr);

    return true;
}
//----------------------------------------------------------------------------
bool Mgc::InSphere (const Vector3& rkPoint, const Sphere& rkSphere,
    Real fEpsilon)
{
    Real fRSqr = rkSphere.Radius()*rkSphere.Radius();
    Vector3 kDiff = rkPoint - rkSphere.Center();
    Real fSqrDist = kDiff.SquaredLength();
    return fSqrDist <= fRSqr + fEpsilon;
}
//----------------------------------------------------------------------------
Sphere Mgc::MergeSpheres (const Sphere& rkSphere0, const Sphere& rkSphere1)
{
    Vector3 kCDiff = rkSphere1.Center() - rkSphere0.Center();
    Real fLSqr = kCDiff.SquaredLength();
    Real fRDiff = rkSphere1.Radius() - rkSphere0.Radius();
    Real fRDiffSqr = fRDiff*fRDiff;

    if ( fRDiffSqr >= fLSqr )
        return ( fRDiff >= 0.0f ? rkSphere1 : rkSphere0 );

    Real fLength = Math::Sqrt(fLSqr);
    const Real fTolerance = 1e-06f;
    Sphere kSphere;

    if ( fLength > fTolerance )
    {
        Real fCoeff = (fLength + fRDiff)/(2.0f*fLength);
        kSphere.Center() = rkSphere0.Center() + fCoeff*kCDiff;
    }
    else
    {
        kSphere.Center() = rkSphere0.Center();
    }

    kSphere.Radius() = 0.5f*(fLength + rkSphere0.Radius() +
        rkSphere1.Radius());

    return kSphere;
}
//----------------------------------------------------------------------------


