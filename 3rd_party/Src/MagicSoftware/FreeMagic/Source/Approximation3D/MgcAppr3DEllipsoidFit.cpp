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

#include "MgcAppr3DEllipsoidFit.h"
#include "MgcCont3DBox.h"
#include "MgcDist3DVecElp.h"
#include "MgcMinimizeND.h"
using namespace Mgc;

class PointArray
{
public:
    PointArray (int iQuantity, const Vector3* akPoint)
        :
        m_akPoint(akPoint)
    {
        m_iQuantity = iQuantity;
        m_akTemp = new Vector3[iQuantity];
    }

    ~PointArray ()
    {
        delete[] m_akTemp;
    }

    int m_iQuantity;
    const Vector3* m_akPoint;
    Vector3* m_akTemp;
};

//----------------------------------------------------------------------------
static void MatrixToAngles (const Matrix3& rkR, Real* afAngle)
{
    // rotation axis = (cos(a0)sin(a1),sin(a0)sin(a1),cos(a1))
    // a0 in [-pi,pi], a1 in [0,pi], a2 in [0,pi]

    Vector3 kAxis;
    rkR.ToAxisAngle(kAxis,afAngle[2]);

    if ( -1.0f < kAxis.z )
    {
        if ( kAxis.z < 1.0f )
        {
            afAngle[0] = Math::ATan2(kAxis.y,kAxis.x);
            afAngle[1] = (Real)acos(kAxis.z);
        }
        else
        {
            afAngle[0] = 0.0f;
            afAngle[1] = 0.0f;
        }
    }
    else
    {
        afAngle[0] = 0.0f;
        afAngle[1] = Math::PI;
    }
}
//----------------------------------------------------------------------------
static void AnglesToMatrix (const Real* afAngle, Matrix3& rkR)
{
    // rotation axis = (cos(a0)sin(a1),sin(a0)sin(a1),cos(a1))
    // a0 in [-pi,pi], a1 in [0,pi], a2 in [0,pi]

    Real fCos0 = Math::Cos(afAngle[0]);
    Real fSin0 = Math::Sin(afAngle[0]);
    Real fCos1 = Math::Cos(afAngle[1]);
    Real fSin1 = Math::Sin(afAngle[1]);
    Vector3 kAxis(fCos0*fSin1,fSin0*fSin1,fCos1);
    rkR.FromAxisAngle(kAxis,afAngle[2]);
}
//----------------------------------------------------------------------------
static Real Energy (const Real* afV, void* pvUserData)
{
    int iQuantity = ((PointArray*)pvUserData)->m_iQuantity;
    const Vector3* akPoint = ((PointArray*)pvUserData)->m_akPoint;
    Vector3* akTemp = ((PointArray*)pvUserData)->m_akTemp;

    Real fEnergy = 0.0f;

    // build rotation matrix
    Matrix3 kRot;
    AnglesToMatrix(&afV[6],kRot);

    // Uniformly scale the extents to keep reasonable floating point values
    // in the distance calculations.
    Real fMax = afV[0];
    if ( afV[1] > fMax )
        fMax = afV[1];
    if ( afV[2] > fMax )
        fMax = afV[2];

    Real fInvMax = 1.0f/fMax;
    EllipsoidStandard kEllipsoid;
    kEllipsoid.Extent(0) = fInvMax*afV[0];
    kEllipsoid.Extent(1) = fInvMax*afV[1];
    kEllipsoid.Extent(2) = fInvMax*afV[2];
    Vector3 kClosest;

    // transform the points to the coordinate system of U and R
    for (int i = 0; i < iQuantity; i++)
    {
        Vector3 kDiff(
            akPoint[i].x - afV[3],
            akPoint[i].y - afV[4],
            akPoint[i].z - afV[5]);

        akTemp[i] = fInvMax*(kDiff*kRot);

        Real fDist = Distance(kEllipsoid,akTemp[i],kClosest);
        fEnergy += fMax*fDist;
    }

    return fEnergy;
}
//----------------------------------------------------------------------------
static void InitialGuess (int iQuantity, const Vector3* akPoint, Vector3& rkU,
    Matrix3& rkR, Real afD[3])
{
    Box3 kBox = ContOrientedBox(iQuantity,akPoint);

    rkU = kBox.Center();
    rkR[0][0] = kBox.Axis(0).x;
    rkR[0][1] = kBox.Axis(0).y;
    rkR[0][2] = kBox.Axis(0).z;
    rkR[1][0] = kBox.Axis(1).x;
    rkR[1][1] = kBox.Axis(1).y;
    rkR[1][2] = kBox.Axis(1).z;
    rkR[2][0] = kBox.Axis(2).x;
    rkR[2][1] = kBox.Axis(2).y;
    rkR[2][2] = kBox.Axis(2).z;
    afD[0] = kBox.Extent(0);
    afD[1] = kBox.Extent(1);
    afD[2] = kBox.Extent(2);
}
//----------------------------------------------------------------------------
Real Mgc::EllipsoidFit (int iQuantity, const Vector3* akPoint, Vector3& rkU,
    Matrix3& rkR, Real afD[3])
{
    // Energy function is E : R^9 -> R where
    // V = (V0,V1,V2,V3,V4,V5,V6,V7,V8)
    //   = (D[0],D[1],D[2],U.x,U,y,U.z,A0,A1,A2). 
    // For really scattered data, you might need a search function

    int iMaxLevel = 8;
    int iMaxBracket = 8;
    int iMaxIterations = 32;
    PointArray kPA(iQuantity,akPoint);
    MinimizeND kMinimizer(9,Energy,iMaxLevel,iMaxBracket,iMaxIterations,&kPA);

    InitialGuess(iQuantity,akPoint,rkU,rkR,afD);

    Real afAngle[3];
    MatrixToAngles(rkR,afAngle);

    Real afExtent[3] =
    {
        afD[0]*Math::FAbs(rkR[0][0]) + afD[1]*Math::FAbs(rkR[0][1])
            + afD[2]*Math::FAbs(rkR[0][2]),
        afD[0]*Math::FAbs(rkR[1][0]) + afD[1]*Math::FAbs(rkR[1][1])
            + afD[2]*Math::FAbs(rkR[1][2]),
        afD[0]*Math::FAbs(rkR[2][0]) + afD[1]*Math::FAbs(rkR[2][1])
            + afD[2]*Math::FAbs(rkR[2][2])
    };

    Real afV0[9] =
    {
        0.5f*afD[0],
        0.5f*afD[1],
        0.5f*afD[2],
        rkU.x - afExtent[0],
        rkU.y - afExtent[1],
        rkU.z - afExtent[2],
        -Math::PI,
        0.0f,
        0.0f
    };

    Real afV1[9] =
    {
        2.0f*afD[0],
        2.0f*afD[1],
        2.0f*afD[2],
        rkU.x + afExtent[0],
        rkU.y + afExtent[1],
        rkU.z + afExtent[2],
        Math::PI,
        Math::PI,
        Math::PI
    };

    Real afVInitial[9] =
    {
        afD[0],
        afD[1],
        afD[2],
        rkU.x,
        rkU.y,
        rkU.z,
        afAngle[0],
        afAngle[1],
        afAngle[2]
    };

    Real afVMin[9], fEMin;
    kMinimizer.GetMinimum(afV0,afV1,afVInitial,afVMin,fEMin);

    afD[0] = afVMin[0];
    afD[1] = afVMin[1];
    afD[2] = afVMin[2];
    rkU.x = afVMin[3];
    rkU.y = afVMin[4];
    rkU.z = afVMin[5];

    AnglesToMatrix(&afVMin[6],rkR);

    return fEMin;
}
//----------------------------------------------------------------------------



