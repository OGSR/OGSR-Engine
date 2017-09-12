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

#include "MgcDist3DLinLin.h"
#include "MgcDist3DVecRct.h"
#include "MgcDist3DLinRct.h"
using namespace Mgc;

static const Real gs_fTolerance = 1e-05f;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Line3& rkLine, const Rectangle3& rkRct,
    Real* pfLinP, Real* pfRctP0, Real* pfRctP1)
{
    Vector3 kDiff = rkRct.Origin() - rkLine.Origin();
    Real fA00 = rkLine.Direction().SquaredLength();
    Real fA01 = -rkLine.Direction().Dot(rkRct.Edge0());
    Real fA02 = -rkLine.Direction().Dot(rkRct.Edge1());
    Real fA11 = rkRct.Edge0().SquaredLength();
    Real fA22 = rkRct.Edge1().SquaredLength();
    Real fB0  = -kDiff.Dot(rkLine.Direction());
    Real fB1  = kDiff.Dot(rkRct.Edge0());
    Real fB2  = kDiff.Dot(rkRct.Edge1());
    Real fCof00 = fA11*fA22;
    Real fCof01 = -fA01*fA22;
    Real fCof02 = -fA02*fA11;
    Real fDet = fA00*fCof00+fA01*fCof01+fA02*fCof02;

    Segment3 kSegPgm;
    Vector3 kPt;
    Real fSqrDist, fSqrDist0, fR, fS, fT, fR0, fS0, fT0;

    if ( Math::FAbs(fDet) >= gs_fTolerance )
    {
        Real fCof11 = fA00*fA22-fA02*fA02;
        Real fCof12 = fA02*fA01;
        Real fCof22 = fA00*fA11-fA01*fA01;
        Real fInvDet = 1.0f/fDet;
        Real fRhs0 = -fB0*fInvDet;
        Real fRhs1 = -fB1*fInvDet;
        Real fRhs2 = -fB2*fInvDet;

        fR = fCof00*fRhs0+fCof01*fRhs1+fCof02*fRhs2;
        fS = fCof01*fRhs0+fCof11*fRhs1+fCof12*fRhs2;
        fT = fCof02*fRhs0+fCof12*fRhs1+fCof22*fRhs2;

        if ( fS < 0.0f )
        {
            if ( fT < 0.0f )  // region 6
            {
                // min on face s=0 or t=0
                kSegPgm.Origin() = rkRct.Origin();
                kSegPgm.Direction() = rkRct.Edge1();
                fSqrDist = SqrDistance(rkLine,kSegPgm,NULL,&fT);
                fS = 0.0f;
                kSegPgm.Origin() = rkRct.Origin();
                kSegPgm.Direction() = rkRct.Edge0();
                fSqrDist0 = SqrDistance(rkLine,kSegPgm,NULL,&fS0);
                fT0 = 0.0f;
                if ( fSqrDist0 < fSqrDist )
                {
                    fSqrDist = fSqrDist0;
                    fS = fS0;
                    fT = fT0;
                }
            }
            else if ( fT <= 1.0f )  // region 5
            {
                // min on face s=0
                kSegPgm.Origin() = rkRct.Origin();
                kSegPgm.Direction() = rkRct.Edge1();
                fSqrDist = SqrDistance(rkLine,kSegPgm,NULL,&fT);
                fS = 0.0f;
            }
            else  // region 4
            {
                // min on face s=0 or t=1
                kSegPgm.Origin() = rkRct.Origin();
                kSegPgm.Direction() = rkRct.Edge1();
                fSqrDist = SqrDistance(rkLine,kSegPgm,NULL,&fT);
                fS = 0.0f;
                kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                kSegPgm.Direction() = rkRct.Edge0();
                fSqrDist0 = SqrDistance(rkLine,kSegPgm,NULL,&fS0);
                fT0 = 1.0f;
                if ( fSqrDist0 < fSqrDist )
                {
                    fSqrDist = fSqrDist0;
                    fS = fS0;
                    fT = fT0;
                }
            }
        }
        else if ( fS <= 1.0f )
        {
            if ( fT < 0.0f )  // region 7
            {
                // min on face t=0
                kSegPgm.Origin() = rkRct.Origin();
                kSegPgm.Direction() = rkRct.Edge0();
                fSqrDist = SqrDistance(rkLine,kSegPgm,NULL,&fS);
                fT = 0.0f;
            }
            else if ( fT <= 1.0f )  // region 0
            {
                // line intersects rectangle
                fSqrDist = 0.0f;
            }
            else  // region 3
            {
                // min on face t=1
                kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                kSegPgm.Direction() = rkRct.Edge0();
                fSqrDist = SqrDistance(rkLine,kSegPgm,NULL,&fS);
                fT = 1.0f;
            }
        }
        else
        {
            if ( fT < 0.0f )  // region 8
            {
                // min on face s=1 or t=0
                kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                kSegPgm.Direction() = rkRct.Edge1();
                fSqrDist = SqrDistance(rkLine,kSegPgm,NULL,&fT);
                fS = 1.0f;
                kSegPgm.Origin() = rkRct.Origin();
                kSegPgm.Direction() = rkRct.Edge0();
                fSqrDist0 = SqrDistance(rkLine,kSegPgm,NULL,&fS0);
                fT0 = 0.0f;
                if ( fSqrDist0 < fSqrDist )
                {
                    fSqrDist = fSqrDist0;
                    fS = fS0;
                    fT = fT0;
                }
            }
            else if ( fT <= 1.0f )  // region 1
            {
                // min on face s=1
                kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                kSegPgm.Direction() = rkRct.Edge1();
                fSqrDist = SqrDistance(rkLine,kSegPgm,NULL,&fT);
                fS = 1.0f;
            }
            else  // region 2
            {
                // min on face s=1 or t=1
                kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                kSegPgm.Direction() = rkRct.Edge1();
                fSqrDist = SqrDistance(rkLine,kSegPgm,NULL,&fT);
                fS = 1.0f;
                kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                kSegPgm.Direction() = rkRct.Edge0();
                fSqrDist0 = SqrDistance(rkLine,kSegPgm,NULL,&fS0);
                fT0 = 1.0f;
                if ( fSqrDist0 < fSqrDist )
                {
                    fSqrDist = fSqrDist0;
                    fS = fS0;
                    fT = fT0;
                }
            }
        }
    }
    else
    {
        // line and rectangle are parallel
        kSegPgm.Origin() = rkRct.Origin();
        kSegPgm.Direction() = rkRct.Edge0();
        fSqrDist = SqrDistance(rkLine,kSegPgm,&fR,&fS);
        fT = 0.0f;

        kSegPgm.Direction() = rkRct.Edge1();
        fSqrDist0 = SqrDistance(rkLine,kSegPgm,&fR0,&fT0);
        fS0 = 0.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
        kSegPgm.Direction() = rkRct.Edge0();
        fSqrDist0 = SqrDistance(rkLine,kSegPgm,&fR0,&fS0);
        fT0 = 1.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
        kSegPgm.Direction() = rkRct.Edge1();
        fSqrDist0 = SqrDistance(rkLine,kSegPgm,&fR0,&fT0);
        fS0 = 1.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }
    }

    if ( pfLinP )
        *pfLinP = fR;

    if ( pfRctP0 )
        *pfRctP0 = fS;

    if ( pfRctP1 )
        *pfRctP1 = fT;

    return Math::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Ray3& rkRay, const Rectangle3& rkRct,
    Real* pfRayP, Real* pfRctP0, Real* pfRctP1)
{
    Vector3 kDiff = rkRct.Origin() - rkRay.Origin();
    Real fA00 = rkRay.Direction().SquaredLength();
    Real fA01 = -rkRay.Direction().Dot(rkRct.Edge0());
    Real fA02 = -rkRay.Direction().Dot(rkRct.Edge1());
    Real fA11 = rkRct.Edge0().SquaredLength();
    Real fA22 = rkRct.Edge1().SquaredLength();
    Real fB0  = -kDiff.Dot(rkRay.Direction());
    Real fB1  = kDiff.Dot(rkRct.Edge0());
    Real fB2  = kDiff.Dot(rkRct.Edge1());
    Real fCof00 = fA11*fA22;
    Real fCof01 = -fA01*fA22;
    Real fCof02 = -fA02*fA11;
    Real fDet = fA00*fCof00+fA01*fCof01+fA02*fCof02;

    Segment3 kSegPgm;
    Vector3 kPt;
    Real fSqrDist, fSqrDist0, fR, fS, fT, fR0, fS0, fT0;

    if ( Math::FAbs(fDet) >= gs_fTolerance )
    {
        Real fCof11 = fA00*fA22-fA02*fA02;
        Real fCof12 = fA02*fA01;
        Real fCof22 = fA00*fA11-fA01*fA01;
        Real fInvDet = 1.0f/fDet;
        Real fRhs0 = -fB0*fInvDet;
        Real fRhs1 = -fB1*fInvDet;
        Real fRhs2 = -fB2*fInvDet;

        fR = fCof00*fRhs0+fCof01*fRhs1+fCof02*fRhs2;
        fS = fCof01*fRhs0+fCof11*fRhs1+fCof12*fRhs2;
        fT = fCof02*fRhs0+fCof12*fRhs1+fCof22*fRhs2;

        if ( fR <= 0.0f )
        {
            if ( fS < 0.0f )
            {
                if ( fT < 0.0f )  // region 6m
                {
                    // min on face s=0 or t=0 or r=0
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,&fR,&fT);
                    fS = 0.0f;
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkRay,kSegPgm,&fR0,&fS0);
                    fT0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    fSqrDist0 = SqrDistance(rkRay.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT <= 1.0f )  // region 5m
                {
                    // min on face s=0 or r=0
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,&fR,&fT);
                    fS = 0.0f;
                    fSqrDist0 = SqrDistance(rkRay.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 4m
                {
                    // min on face s=0 or t=1 or r=0
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,&fR,&fT);
                    fS = 0.0f;
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkRay,kSegPgm,&fR0,&fS0);
                    fT0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    fSqrDist0 = SqrDistance(rkRay.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
            else if ( fS <= 1.0f )
            {
                if ( fT < 0.0f )  // region 7m
                {
                    // min on face t=0 or r=0
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,&fR,&fS);
                    fT = 0.0f;
                    fSqrDist0 = SqrDistance(rkRay.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT <= 1.0f )  // region 0m
                {
                    // min on face r=0
                    fSqrDist = SqrDistance(rkRay.Origin(),rkRct,&fS,&fT);
                    fR = 0.0f;
                }
                else  // region 3m
                {
                    // min on face t=1 or r=0
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,&fR,&fS);
                    fT = 1.0f;
                    fSqrDist0 = SqrDistance(rkRay.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
            else
            {
                if ( fT < 0.0f )  // region 8m
                {
                    // min on face s=1 or t=0 or r=0
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,&fR,&fT);
                    fS = 1.0f;
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkRay,kSegPgm,&fR0,&fS0);
                    fT0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    fSqrDist0 = SqrDistance(rkRay.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT <= 1.0f )  // region 1m
                {
                    // min on face s=1 or r=0
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,&fR,&fT);
                    fS = 1.0f;
                    fSqrDist0 = SqrDistance(rkRay.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 2m
                {
                    // min on face s=1 or t=1 or r=0
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,&fR,&fT);
                    fS = 1.0f;
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkRay,kSegPgm,&fR0,&fS0);
                    fT0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    fSqrDist0 = SqrDistance(rkRay.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
        }
        else  // fR > 0
        {
            if ( fS < 0.0f )
            {
                if ( fT < 0.0f )  // region 6p
                {
                    // min on face s=0 or t=0
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,NULL,&fT);
                    fS = 0.0f;
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkRay,kSegPgm,NULL,&fS0);
                    fT0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT <= 1.0f )  // region 5p
                {
                    // min on face s=0
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,NULL,&fT);
                    fS = 0.0f;
                }
                else  // region 4p
                {
                    // min on face s=0 or t=1
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,NULL,&fT);
                    fS = 0.0f;
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkRay,kSegPgm,NULL,&fS0);
                    fT0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
            else if ( fS <= 1.0f )
            {
                if ( fT < 0.0f )  // region 7p
                {
                    // min on face t=0
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,NULL,&fS);
                    fT = 0.0f;
                }
                else if ( fT <= 1.0f )  // region 0p
                {
                    // ray intersects the rectangle
                    fSqrDist = 0.0f;
                }
                else  // region 3p
                {
                    // min on face t=1
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,NULL,&fS);
                    fT = 1.0f;
                }
            }
            else
            {
                if ( fT < 0.0f )  // region 8p
                {
                    // min on face s=1 or t=0
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,NULL,&fT);
                    fS = 1.0f;
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkRay,kSegPgm,NULL,&fS0);
                    fT0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT <= 1.0f )  // region 1p
                {
                    // min on face s=1
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,NULL,&fT);
                    fS = 1.0f;
                }
                else  // region 2p
                {
                    // min on face s=1 or t=1
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkRay,kSegPgm,NULL,&fT);
                    fS = 1.0f;
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkRay,kSegPgm,NULL,&fS0);
                    fT0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
        }
    }
    else
    {
        // ray and rectangle are parallel
        kSegPgm.Origin() = rkRct.Origin();
        kSegPgm.Direction() = rkRct.Edge0();
        fSqrDist = SqrDistance(rkRay,kSegPgm,&fR,&fS);
        fT = 0.0f;

        kSegPgm.Direction() = rkRct.Edge1();
        fSqrDist0 = SqrDistance(rkRay,kSegPgm,&fR0,&fT0);
        fS0 = 0.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
        kSegPgm.Direction() = rkRct.Edge0();
        fSqrDist0 = SqrDistance(rkRay,kSegPgm,&fR0,&fS0);
        fT0 = 1.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
        kSegPgm.Direction() = rkRct.Edge1();
        fSqrDist0 = SqrDistance(rkRay,kSegPgm,&fR0,&fT0);
        fS0 = 1.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        fSqrDist0 = SqrDistance(rkRay.Origin(),rkRct,&fS0,&fT0);
        fR0 = 0.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }
    }

    if ( pfRayP )
        *pfRayP = fR;

    if ( pfRctP0 )
        *pfRctP0 = fS;

    if ( pfRctP1 )
        *pfRctP1 = fT;

    return Math::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Segment3& rkSeg, const Rectangle3& rkRct,
    Real* pfSegP, Real* pfRctP0, Real* pfRctP1)
{
    Vector3 kDiff = rkRct.Origin() - rkSeg.Origin();
    Real fA00 = rkSeg.Direction().SquaredLength();
    Real fA01 = -rkSeg.Direction().Dot(rkRct.Edge0());
    Real fA02 = -rkSeg.Direction().Dot(rkRct.Edge1());
    Real fA11 = rkRct.Edge0().SquaredLength();
    Real fA22 = rkRct.Edge1().SquaredLength();
    Real fB0  = -kDiff.Dot(rkSeg.Direction());
    Real fB1  = kDiff.Dot(rkRct.Edge0());
    Real fB2  = kDiff.Dot(rkRct.Edge1());
    Real fCof00 = fA11*fA22;
    Real fCof01 = -fA01*fA22;
    Real fCof02 = -fA02*fA11;
    Real fDet = fA00*fCof00+fA01*fCof01+fA02*fCof02;

    Segment3 kSegPgm;
    Vector3 kPt;
    Real fSqrDist, fSqrDist0, fR, fS, fT, fR0, fS0, fT0;

    if ( Math::FAbs(fDet) >= gs_fTolerance )
    {
        Real fCof11 = fA00*fA22-fA02*fA02;
        Real fCof12 = fA02*fA01;
        Real fCof22 = fA00*fA11-fA01*fA01;
        Real fInvDet = 1.0f/fDet;
        Real fRhs0 = -fB0*fInvDet;
        Real fRhs1 = -fB1*fInvDet;
        Real fRhs2 = -fB2*fInvDet;

        fR = fCof00*fRhs0+fCof01*fRhs1+fCof02*fRhs2;
        fS = fCof01*fRhs0+fCof11*fRhs1+fCof12*fRhs2;
        fT = fCof02*fRhs0+fCof12*fRhs1+fCof22*fRhs2;

        if ( fR < 0.0f )
        {
            if ( fS < 0.0f )
            {
                if ( fT < 0.0f )  // region 6m
                {
                    // min on face s=0 or t=0 or r=0
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 0.0f;
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fS0);
                    fT0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    fSqrDist0 = SqrDistance(rkSeg.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT <= 1.0f )  // region 5m
                {
                    // min on face s=0 or r=0
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 0.0f;
                    fSqrDist0 = SqrDistance(rkSeg.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 4m
                {
                    // min on face s=0 or t=1 or r=0
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 0.0f;
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fS0);
                    fT0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    fSqrDist0 = SqrDistance(rkSeg.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
            else if ( fS <= 1.0f )
            {
                if ( fT < 0.0f )  // region 7m
                {
                    // min on face t=0 or r=0
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fS);
                    fT = 0.0f;
                    fSqrDist0 = SqrDistance(rkSeg.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT <= 1.0f )  // region 0m
                {
                    // min on face r=0
                    fSqrDist = SqrDistance(rkSeg.Origin(),rkRct,&fS,&fT);
                    fR = 0.0f;
                }
                else  // region 3m
                {
                    // min on face t=1 or r=0
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fS);
                    fT = 1.0f;
                    fSqrDist0 = SqrDistance(rkSeg.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
            else
            {
                if ( fT < 0.0f )  // region 8m
                {
                    // min on face s=1 or t=0 or r=0
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 1.0f;
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fS0);
                    fT0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    fSqrDist0 = SqrDistance(rkSeg.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT <= 1.0f )  // region 1m
                {
                    // min on face s=1 or r=0
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 1.0f;
                    fSqrDist0 = SqrDistance(rkSeg.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 2m
                {
                    // min on face s=1 or t=1 or r=0
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 1.0f;
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fS0);
                    fT0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    fSqrDist0 = SqrDistance(rkSeg.Origin(),rkRct,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
        }
        else if ( fR <= 1.0f )
        {
            if ( fS < 0.0f )
            {
                if ( fT < 0.0f )  // region 6
                {
                    // min on face s=0 or t=0
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 0.0f;
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fS0);
                    fT0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT <= 1.0f )  // region 5
                {
                    // min on face s=0
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 0.0f;
                }
                else // region 4
                {
                    // min on face s=0 or t=1
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 0.0f;
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fS0);
                    fT0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
            else if ( fS <= 1.0f )
            {
                if ( fT < 0.0f )  // region 7
                {
                    // min on face t=0
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fS);
                    fT = 0.0f;
                }
                else if ( fT <= 1.0f )  // region 0
                {
                    // global minimum is interior
                    fSqrDist = fR*(fA00*fR+fA01*fS+fA02*fT+2.0f*fB0)
                          +fS*(fA01*fR+fA11*fS+2.0f*fB1)
                          +fT*(fA02*fR+fA22*fT+2.0f*fB2)
                          +kDiff.SquaredLength();
                }
                else  // region 3
                {
                    // min on face t=1
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fS);
                    fT = 1.0f;
                }
            }
            else
            {
                if ( fT < 0.0 )  // region 8
                {
                    // min on face s=1 or t=0
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 1.0f;
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fS0);
                    fT0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT <= 1.0f )  // region 1
                {
                    // min on face s=1
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 1.0f;
                }
                else  // region 2
                {
                    // min on face s=1 or t=1
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 1.0f;
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fS0);
                    fT0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
        }
        else
        {
            if ( fS < 0.0f )
            {
                if ( fT < 0.0f )  // region 6p
                {
                    // min on face s=0 or t=0 or r=1
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 0.0f;
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fS0);
                    fT0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    kPt = rkSeg.Origin()+rkSeg.Direction();
                    fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT <= 1.0f )  // region 5p
                {
                    // min on face s=0 or r=1
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 0.0f;
                    fSqrDist0 = SqrDistance(rkSeg.Origin(),rkRct,&fS0,&fT0);
                    kPt = rkSeg.Origin()+rkSeg.Direction();
                    fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 4p
                {
                    // min on face s=0 or t=1 or r=1
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 0.0f;
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fS0);
                    fT0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    kPt = rkSeg.Origin()+rkSeg.Direction();
                    fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
            else if ( fS <= 1.0f )
            {
                if ( fT < 0.0f )  // region 7p
                {
                    // min on face t=0 or r=1
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fS);
                    fT = 0.0f;
                    kPt = rkSeg.Origin()+rkSeg.Direction();
                    fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT <= 1.0f )  // region 0p
                {
                    // min on face r=1
                    kPt = rkSeg.Origin()+rkSeg.Direction();
                    fSqrDist = SqrDistance(kPt,rkRct,&fS,&fT);
                    fR = 1.0f;
                }
                else  // region 3p
                {
                    // min on face t=1 or r=1
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fS);
                    fT = 1.0f;
                    kPt = rkSeg.Origin()+rkSeg.Direction();
                    fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
            else
            {
                if ( fT < 0.0f )  // region 8p
                {
                    // min on face s=1 or t=0 or r=1
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 1.0f;
                    kSegPgm.Origin() = rkRct.Origin();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fS0);
                    fT0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    kPt = rkSeg.Origin()+rkSeg.Direction();
                    fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT <= 1.0f )  // region 1p
                {
                    // min on face s=1 or r=1
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 1.0f;
                    kPt = rkSeg.Origin()+rkSeg.Direction();
                    fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 2p
                {
                    // min on face s=1 or t=1 or r=1
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
                    kSegPgm.Direction() = rkRct.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fT);
                    fS = 1.0f;
                    kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
                    kSegPgm.Direction() = rkRct.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fS0);
                    fT0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    kPt = rkSeg.Origin()+rkSeg.Direction();
                    fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
            }
        }
    }
    else
    {
        // segment and rectangle are parallel
        kSegPgm.Origin() = rkRct.Origin();
        kSegPgm.Direction() = rkRct.Edge0();
        fSqrDist = SqrDistance(rkSeg,kSegPgm,&fR,&fS);
        fT = 0.0f;

        kSegPgm.Direction() = rkRct.Edge1();
        fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fT0);
        fS0 = 0.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge1();
        kSegPgm.Direction() = rkRct.Edge0();
        fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fS0);
        fT0 = 1.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        kSegPgm.Origin() = rkRct.Origin()+rkRct.Edge0();
        kSegPgm.Direction() = rkRct.Edge1();
        fSqrDist0 = SqrDistance(rkSeg,kSegPgm,&fR0,&fT0);
        fS0 = 1.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        fSqrDist0 = SqrDistance(rkSeg.Origin(),rkRct,&fS0,&fT0);
        fR0 = 0.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        kPt = rkSeg.Origin()+rkSeg.Direction();
        fSqrDist0 = SqrDistance(kPt,rkRct,&fS0,&fT0);
        fR0 = 1.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }
    }

    if ( pfSegP )
        *pfSegP = fR;

    if ( pfRctP0 )
        *pfRctP0 = fS;

    if ( pfRctP1 )
        *pfRctP1 = fT;

    return Math::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Line3& rkLine, const Rectangle3& rkRct,
    Real* pfLinP, Real* pfRctP0, Real* pfRctP1)
{
    return Math::Sqrt(SqrDistance(rkLine,rkRct,pfLinP,pfRctP0,pfRctP1));
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Ray3& rkRay, const Rectangle3& rkRct,
    Real* pfRayP, Real* pfRctP0, Real* pfRctP1)
{
    return Math::Sqrt(SqrDistance(rkRay,rkRct,pfRayP,pfRctP0,pfRctP1));
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Segment3& rkSeg, const Rectangle3& rkRct,
    Real* pfSegP, Real* pfRctP0, Real* pfRctP1)
{
    return Math::Sqrt(SqrDistance(rkSeg,rkRct,pfSegP,pfRctP0,pfRctP1));
}
//----------------------------------------------------------------------------


