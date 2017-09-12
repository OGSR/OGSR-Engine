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

#include "MgcAppr2DEllipseFit.h"
#include "MgcCont2DBox.h"
#include "MgcDist2DVecElp.h"
#include "MgcMinimizeND.h"
using namespace Mgc;


class PointArray
{
public:
    PointArray (int iQuantity, const Vector2* akPoint)
        :
        m_akPoint(akPoint)
    {
        m_iQuantity = iQuantity;
        m_akTemp = new Vector2[iQuantity];
    }

    ~PointArray ()
    {
        delete[] m_akTemp;
    }

    int m_iQuantity;
    const Vector2* m_akPoint;
    Vector2* m_akTemp;
};

//----------------------------------------------------------------------------
static Real Energy (const Real* afV, void* pvUserData)
{
    int iQuantity = ((PointArray*)pvUserData)->m_iQuantity;
    const Vector2* akPoint = ((PointArray*)pvUserData)->m_akPoint;
    Vector2* akTemp = ((PointArray*)pvUserData)->m_akTemp;

    Real fEnergy = 0.0f;

    // build rotation matrix
    Matrix2 kRot;
    kRot.FromAngle(afV[4]);

    EllipseStandard2 kEllipse;
    kEllipse.Extent(0) = afV[0];
    kEllipse.Extent(1) = afV[1];
    Vector2 kClosest;

    // transform the points to the coordinate system of U and R
    for (int i = 0; i < iQuantity; i++)
    {
        Vector2 kDiff(akPoint[i].x - afV[2],akPoint[i].y - afV[3]);
        akTemp[i] = kDiff*kRot;
        Real fDist = Distance(kEllipse,akTemp[i],kClosest);
        fEnergy += fDist;
    }

    return fEnergy;
}
//----------------------------------------------------------------------------
static void InitialGuess (int iQuantity, const Vector2* akPoint,
    Vector2& rkU, Matrix2& rkR, Real afD[2])
{
    Box2 kBox = ContOrientedBox(iQuantity,akPoint);

    rkU = kBox.Center();
    rkR[0][0] = kBox.Axis(0).x;
    rkR[0][1] = kBox.Axis(0).y;
    rkR[1][0] = kBox.Axis(1).x;
    rkR[1][1] = kBox.Axis(1).y;
    afD[0] = kBox.Extent(0);
    afD[1] = kBox.Extent(1);
}
//----------------------------------------------------------------------------
Real Mgc::EllipseFit (int iQuantity, const Vector2* akPoint, Vector2& rkU,
    Matrix2& rkR, Real afD[2])
{
    // Energy function is E : R^5 -> R where
    // V = (V0,V1,V2,V3,V4)
    //   = (D[0],D[1],U.x,U,y,atan2(R[1][0],R[1][1])).

    int iMaxLevel = 8;
    int iMaxBracket = 8;
    int iMaxIterations = 32;
    PointArray kPA(iQuantity,akPoint);
    MinimizeND kMinimizer(5,Energy,iMaxLevel,iMaxBracket,iMaxIterations,&kPA);

    InitialGuess(iQuantity,akPoint,rkU,rkR,afD);
    Real fAngle = Math::ACos(rkR[0][0]);
    Real fE0 = afD[0]*Math::FAbs(rkR[0][0]) + afD[1]*Math::FAbs(rkR[0][1]);
    Real fE1 = afD[0]*Math::FAbs(rkR[1][0]) + afD[1]*Math::FAbs(rkR[1][1]);

    Real afV0[5] =
    {
        0.5f*afD[0],
        0.5f*afD[1],
        rkU.x - fE0,
        rkU.y - fE1,
        0.0f
    };

    Real afV1[5] =
    {
        2.0f*afD[0],
        2.0f*afD[1],
        rkU.x + fE0,
        rkU.y + fE1,
        Math::PI
    };

    Real afVInitial[5] =
    {
        afD[0],
        afD[1],
        rkU.x,
        rkU.y,
        fAngle
    };

    Real afVMin[5], fEMin;
    kMinimizer.GetMinimum(afV0,afV1,afVInitial,afVMin,fEMin);

    afD[0] = afVMin[0];
    afD[1] = afVMin[1];
    rkU.x = afVMin[2];
    rkU.y = afVMin[3];
    rkR.FromAngle(afVMin[4]);

    return fEMin;
}
//----------------------------------------------------------------------------


