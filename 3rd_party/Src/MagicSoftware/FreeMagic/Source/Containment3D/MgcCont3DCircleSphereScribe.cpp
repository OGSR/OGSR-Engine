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

#include "MgcCont3DCircleSphereScribe.h"
#include "MgcLinearSystem.h"
using namespace Mgc;

static const Real gs_fTolerance = 1e-06f;

//----------------------------------------------------------------------------
bool Mgc::Circumscribe (const Vector3& rkV0, const Vector3& rkV1,
    const Vector3& rkV2, Circle3& rkCircle)
{
    Vector3 kE02 = rkV0 - rkV2;
    Vector3 kE12 = rkV1 - rkV2;
    Real fE02E02 = kE02.Dot(kE02);
    Real fE02E12 = kE02.Dot(kE12);
    Real fE12E12 = kE12.Dot(kE12);
    Real fDet = fE02E02*fE12E12 - fE02E12*fE02E12;
    if ( Math::FAbs(fDet) < gs_fTolerance )
        return false;

    Real fHalfInvDet = 0.5f/fDet;
    Real fU0 = fHalfInvDet*fE12E12*(fE02E02 - fE02E12);
    Real fU1 = fHalfInvDet*fE02E02*(fE12E12 - fE02E12);
    Vector3 kTmp = fU0*kE02 + fU1*kE12;

    rkCircle.Center() = rkV2 + kTmp;
    rkCircle.Radius() = kTmp.Length();

    rkCircle.N() = kE02.UnitCross(kE12);
    Vector3& rkN = rkCircle.N();
    Vector3& rkU = rkCircle.U();
    if ( Math::FAbs(rkN.x) >= Math::FAbs(rkN.y)
    &&   Math::FAbs(rkN.x) >= Math::FAbs(rkN.z) )
    {
        rkU.x = -rkN.y;
        rkU.y = rkN.x;
        rkU.z = 0.0f;
    }
    else
    {
        rkU.x = 0.0f;
        rkU.y = rkN.z;
        rkU.z = -rkN.y;
    }

    rkU.Unitize();
    rkCircle.V() = rkCircle.N().Cross(rkCircle.U());

    return true;
}
//----------------------------------------------------------------------------
bool Mgc::Circumscribe (const Vector3& rkV0, const Vector3& rkV1,
    const Vector3& rkV2, const Vector3& rkV3, Sphere& rkSphere)
{
    Vector3 kE10 = rkV1 - rkV0;
    Vector3 kE20 = rkV2 - rkV0;
    Vector3 kE30 = rkV3 - rkV0;

    Real aafA[3][3] =
    {
        kE10.x, kE10.y, kE10.z,
        kE20.x, kE20.y, kE20.z,
        kE30.x, kE30.y, kE30.z
    };

    Real afB[3] =
    {
        0.5f*kE10.SquaredLength(),
        0.5f*kE20.SquaredLength(),
        0.5f*kE30.SquaredLength()
    };

    Vector3 kSol;
    if ( LinearSystem::Solve3(aafA,afB,(Real*)&kSol) )
    {
        rkSphere.Center() = rkV0 + kSol;
        rkSphere.Radius() = kSol.Length();
        return true;
    }
    else
    {
        return false;
    }
}
//----------------------------------------------------------------------------
bool Mgc::Inscribe (const Vector3& rkV0, const Vector3& rkV1,
    const Vector3& rkV2, Circle3& rkCircle)
{
    // edges
    Vector3 kE0 = rkV1 - rkV0;
    Vector3 kE1 = rkV2 - rkV1;
    Vector3 kE2 = rkV0 - rkV2;

    // plane normal
    rkCircle.N() = kE1.Cross(kE0);

    // edge normals within the plane
    Vector3 kN0 = rkCircle.N().UnitCross(kE0);
    Vector3 kN1 = rkCircle.N().UnitCross(kE1);
    Vector3 kN2 = rkCircle.N().UnitCross(kE2);

    Real fA0 = kN1.Dot(kE0);
    if ( Math::FAbs(fA0) < gs_fTolerance )
        return false;

    Real fA1 = kN2.Dot(kE1);
    if ( Math::FAbs(fA1) < gs_fTolerance )
        return false;

    Real fA2 = kN0.Dot(kE2);
    if ( Math::FAbs(fA2) < gs_fTolerance )
        return false;

    Real fInvA0 = 1.0f/fA0;
    Real fInvA1 = 1.0f/fA1;
    Real fInvA2 = 1.0f/fA2;

    rkCircle.Radius() = 1.0f/(fInvA0 + fInvA1 + fInvA2);
    rkCircle.Center() = rkCircle.Radius()*(fInvA0*rkV0 + fInvA1*rkV1 +
        fInvA2*rkV2);

    rkCircle.U() = kN0;
    rkCircle.V() = rkCircle.N().Cross(rkCircle.U());

    return true;
}
//----------------------------------------------------------------------------
bool Mgc::Inscribe (const Vector3& rkV0, const Vector3& rkV1,
    const Vector3& rkV2, const Vector3& rkV3, Sphere& rkSphere)
{
    // edges
    Vector3 kE10 = rkV1 - rkV0;
    Vector3 kE20 = rkV2 - rkV0;
    Vector3 kE30 = rkV3 - rkV0;
    Vector3 kE21 = rkV2 - rkV1;
    Vector3 kE31 = rkV3 - rkV1;

    // normals
    Vector3 kN0 = kE31.Cross(kE21);
    Vector3 kN1 = kE20.Cross(kE30);
    Vector3 kN2 = kE30.Cross(kE10);
    Vector3 kN3 = kE10.Cross(kE20);

    // unitize normals
    if ( Math::FAbs(kN0.Unitize()) < gs_fTolerance )
        return false;
    if ( Math::FAbs(kN1.Unitize()) < gs_fTolerance )
        return false;
    if ( Math::FAbs(kN2.Unitize()) < gs_fTolerance )
        return false;
    if ( Math::FAbs(kN3.Unitize()) < gs_fTolerance )
        return false;

    Real aafA[3][3] =
    {
        kN1.x-kN0.x, kN1.y-kN0.y, kN1.z-kN0.z,
        kN2.x-kN0.x, kN2.y-kN0.y, kN2.z-kN0.z,
        kN3.x-kN0.x, kN3.y-kN0.y, kN3.z-kN0.z
    };

    Real afB[3] =
    {
        0.0f,
        0.0f,
        -kN3.Dot(kE30)
    };

    Vector3 kSol;
    if ( LinearSystem::Solve3(aafA,afB,(Real*)&kSol) )
    {
        rkSphere.Center() = rkV3 + kSol;
        rkSphere.Radius() = Math::FAbs(kN0.Dot(kSol));
        return true;
    }
    else
    {
        return false;
    }
}
//----------------------------------------------------------------------------


