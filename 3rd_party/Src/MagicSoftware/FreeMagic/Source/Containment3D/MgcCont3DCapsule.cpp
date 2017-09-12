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

#include "MgcAppr3DLineFit.h"
#include "MgcCont3DCapsule.h"
#include "MgcDist3DVecLin.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Capsule Mgc::ContCapsule (int iQuantity, const Vector3* akPoint)
{
    Capsule kCapsule;

    Line3 kLine;
    OrthogonalLineFit(iQuantity,akPoint,kLine.Origin(),kLine.Direction());

    Real fMaxRadiusSqr = 0.0f;
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        Real fRadiusSqr = SqrDistance(akPoint[i],kLine);
        if ( fRadiusSqr > fMaxRadiusSqr )
            fMaxRadiusSqr = fRadiusSqr;
    }

    Vector3 kU, kV, kW = kLine.Direction();
    Vector3::GenerateOrthonormalBasis(kU,kV,kW);

    Real fMin = Math::MAX_REAL, fMax = -fMin;
    for (i = 0; i < iQuantity; i++)
    {
        Vector3 kDiff = akPoint[i] - kLine.Origin();
        Real fU = kU.Dot(kDiff);
        Real fV = kV.Dot(kDiff);
        Real fW = kW.Dot(kDiff);
        Real fDiscr = fMaxRadiusSqr - (fU*fU + fV*fV);
        Real fRadical = Math::Sqrt(Math::FAbs(fDiscr));

        Real fTest = fW + fRadical;
        if ( fTest < fMin )
            fMin = fTest;

        fTest = fW - fRadical;
        if ( fTest > fMax )
            fMax = fTest;
    }

    if ( fMin < fMax )
    {
        kCapsule.Origin() = kLine.Origin() + fMin*kLine.Direction();
        kCapsule.Direction() = (fMax-fMin)*kLine.Direction();
    }
    else
    {
        // enclosing capsule is really a sphere
        kCapsule.Origin() = kLine.Origin() +
            (0.5f*(fMin+fMax))*kLine.Direction();
        kCapsule.Direction() = Vector3::ZERO;
    }

    kCapsule.Radius() = Math::Sqrt(fMaxRadiusSqr);

    return kCapsule;
}
//----------------------------------------------------------------------------
bool Mgc::ContCapsule (int iQuantity, const Vector3* akPoint,
    const bool* abValid, Capsule& rkCapsule)
{
    Line3 kLine;
    if ( !OrthogonalLineFit(iQuantity,akPoint,abValid,kLine.Origin(),
         kLine.Direction()) )
    {
        return false;
    }

    Real fMaxRadiusSqr = 0.0f;
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            Real fRadiusSqr = SqrDistance(akPoint[i],kLine);
            if ( fRadiusSqr > fMaxRadiusSqr )
                fMaxRadiusSqr = fRadiusSqr;
        }
    }

    Vector3 kU, kV, kW = kLine.Direction();
    Vector3::GenerateOrthonormalBasis(kU,kV,kW);

    Real fMin = Math::MAX_REAL, fMax = -fMin;
    for (i = 0; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            Vector3 kDiff = akPoint[i] - kLine.Origin();
            Real fU = kU.Dot(kDiff);
            Real fV = kV.Dot(kDiff);
            Real fW = kW.Dot(kDiff);
            Real fDiscr = fMaxRadiusSqr - (fU*fU + fV*fV);
            Real fRadical = Math::Sqrt(Math::FAbs(fDiscr));

            Real fTest = fW + fRadical;
            if ( fTest < fMin )
                fMin = fTest;

            fTest = fW - fRadical;
            if ( fTest > fMax )
                fMax = fTest;
        }
    }

    if ( fMin < fMax )
    {
        rkCapsule.Origin() = kLine.Origin() + fMin*kLine.Direction();
        rkCapsule.Direction() = (fMax-fMin)*kLine.Direction();
    }
    else
    {
        // enclosing capsule is really a sphere
        rkCapsule.Origin() = kLine.Origin() +
            (0.5f*(fMin+fMax))*kLine.Direction();
        rkCapsule.Direction() = Vector3::ZERO;
    }

    rkCapsule.Radius() = Math::Sqrt(fMaxRadiusSqr);

    return true;
}
//----------------------------------------------------------------------------
bool Mgc::InCapsule (const Vector3& rkPoint, const Capsule& rkCapsule,
    Real fEpsilon)
{
    Real fRSqr = rkCapsule.Radius()*rkCapsule.Radius();
    Real fSqrDist = SqrDistance(rkPoint,rkCapsule.Segment());
    return fSqrDist <= fRSqr + fEpsilon;
}
//----------------------------------------------------------------------------
bool Mgc::InCapsule (const Sphere& rkSphere, const Capsule& rkCapsule)
{
    Real fRDiff = rkCapsule.Radius() - rkSphere.Radius();
    if ( fRDiff >= 0.0f )
    {
        return SqrDistance(rkSphere.Center(),rkCapsule.Segment()) <=
            fRDiff*fRDiff;
    }
    else
    {
        return false;
    }
}
//----------------------------------------------------------------------------
bool Mgc::InCapsule (const Capsule& rkTestCapsule,  const Capsule& rkCapsule)
{
    Sphere kSphere0, kSphere1;
    kSphere0.Center() = rkTestCapsule.Origin();
    kSphere0.Radius() = rkTestCapsule.Radius();
    kSphere1.Center() = rkTestCapsule.Origin() + rkTestCapsule.Direction();
    kSphere1.Radius() = rkTestCapsule.Radius();

    return InCapsule(kSphere0,rkCapsule) && InCapsule(kSphere1,rkCapsule);
}
//----------------------------------------------------------------------------
Capsule Mgc::MergeCapsules (const Capsule& rkCapsule0,
    const Capsule& rkCapsule1)
{
    if ( InCapsule(rkCapsule0,rkCapsule1) )
        return rkCapsule1;

    if ( InCapsule(rkCapsule1,rkCapsule0) )
        return rkCapsule0;

    const Vector3& rkP0 = rkCapsule0.Origin();
    const Vector3& rkP1 = rkCapsule1.Origin();
    const Vector3& rkD0 = rkCapsule0.Direction();
    const Vector3& rkD1 = rkCapsule1.Direction();

    // axis of final capsule
    Line3 kLine;

    // axis center is average of input axis centers
    kLine.Origin() = 0.5f*(rkP0 + rkP1) + 0.25f*(rkD0 + rkD1);

    // axis unit direction is average of input axis unit directions
    Vector3 kDirection0 = rkD0;
    Vector3 kDirection1 = rkD1;
    kDirection0.Unitize();
    kDirection1.Unitize();
    Vector3& rkLineDir = (Vector3&) kLine.Direction();
    if ( kDirection0.Dot(kDirection1) >= 0.0f )
        rkLineDir = kDirection0 + kDirection1;
    else
        rkLineDir = kDirection0 - kDirection1;
    rkLineDir.Unitize();

    // Cylinder with axis 'kLine' must contain the spheres centered at the
    // end points of the input capsules.
    Real fRadius = Distance(rkP0,kLine) + rkCapsule0.Radius();

    Real fDist = Distance(rkP1,kLine) + rkCapsule1.Radius();
    if ( fDist > fRadius )
        fRadius = fDist;

    Vector3 kP0D0 = rkP0 + rkD0;
    fDist = Distance(kP0D0,kLine) + rkCapsule0.Radius();
    if ( fDist > fRadius )
        fRadius = fDist;

    Vector3 kP1D1 = rkP1 + rkD1;
    fDist = Distance(kP1D1,kLine) + rkCapsule1.Radius();
    if ( fDist > fRadius )
        fRadius = fDist;

    // process sphere <P0,r0>
    Real fRDiff = fRadius - rkCapsule0.Radius();
    Real fRDiffSqr = fRDiff*fRDiff;
    Vector3 kDiff = kLine.Origin() - rkP0;
    Real fK0 = kDiff.SquaredLength() - fRDiffSqr;
    Real fK1 = kDiff.Dot(kLine.Direction());
    Real fDiscr = fK1*fK1 - fK0;  // assert:  K1*K1-K0 >= 0
    Real fRoot = Math::Sqrt(Math::FAbs(fDiscr));
    Real fTPos = fK1 - fRoot;
    Real fTNeg = fK1 + fRoot;
    Real fTmp;

    // process sphere <P0+D0,r0>
    kDiff = kLine.Origin() - kP0D0;
    fK0 = kDiff.SquaredLength() - fRDiffSqr;
    fK1 = kDiff.Dot(kLine.Direction());
    fDiscr = fK1*fK1 - fK0;  // assert:  K1*K1-K0 >= 0
    fRoot = Math::Sqrt(Math::FAbs(fDiscr));
    fTmp = fK1 - fRoot;
    if ( fTmp > fTPos )
        fTPos = fTmp;
    fTmp = fK1 + fRoot;
    if ( fTmp < fTNeg )
        fTNeg = fTmp;

    // process sphere <P1,r1>
    fRDiff = fRadius - rkCapsule1.Radius();
    fRDiffSqr = fRDiff*fRDiff;
    kDiff = kLine.Origin() - rkP1;
    fK0 = kDiff.SquaredLength() - fRDiffSqr;
    fK1 = kDiff.Dot(kLine.Direction());
    fDiscr = fK1*fK1 - fK0;  // assert:  K1*K1-K0 >= 0
    fRoot = Math::Sqrt(Math::FAbs(fDiscr));
    fTmp = fK1 - fRoot;
    if ( fTmp > fTPos )
        fTPos = fTmp;
    fTmp = fK1 + fRoot;
    if ( fTmp < fTNeg )
        fTNeg = fTmp;

    // process sphere <P1+D1,r1>
    kDiff = kLine.Origin() - kP1D1;
    fK0 = kDiff.SquaredLength() - fRDiffSqr;
    fK1 = kDiff.Dot(kLine.Direction());
    fDiscr = fK1*fK1 - fK0;  // assert:  K1*K1-K0 >= 0
    fRoot = Math::Sqrt(Math::FAbs(fDiscr));
    fTmp = fK1 - fRoot;
    if ( fTmp > fTPos )
        fTPos = fTmp;
    fTmp = fK1 + fRoot;
    if ( fTmp < fTNeg )
        fTNeg = fTmp;

    if ( fTPos < fTNeg )
    {
        Real fAverage = 0.5f*(fTPos + fTNeg);
        fTPos = fAverage;
        fTNeg = fAverage;
    }

    Capsule kCapsule;
    kCapsule.Radius() = fRadius;
    kCapsule.Origin() = kLine.Origin() + fTNeg*kLine.Direction();
    kCapsule.Direction() = (fTPos - fTNeg)*kLine.Direction();
    return kCapsule;
}
//----------------------------------------------------------------------------


