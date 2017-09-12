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
#include "MgcDist3DVecTri.h"
#include "MgcDist3DLinTri.h"
using namespace Mgc;

static const Real gs_fTolerance = 1e-06f;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Line3& rkLine, const Triangle3& rkTri,
    Real* pfLinP, Real* pfTriP0, Real* pfTriP1)
{
    Vector3 kDiff = rkTri.Origin() - rkLine.Origin();
    Real fA00 = rkLine.Direction().SquaredLength();
    Real fA01 = -rkLine.Direction().Dot(rkTri.Edge0());
    Real fA02 = -rkLine.Direction().Dot(rkTri.Edge1());
    Real fA11 = rkTri.Edge0().SquaredLength();
    Real fA12 = rkTri.Edge0().Dot(rkTri.Edge1());
    Real fA22 = rkTri.Edge1().Dot(rkTri.Edge1());
    Real fB0  = -kDiff.Dot(rkLine.Direction());
    Real fB1  = kDiff.Dot(rkTri.Edge0());
    Real fB2  = kDiff.Dot(rkTri.Edge1());

    Segment3 kTriSeg;
    Vector3 kPt;
    Real fSqrDist, fSqrDist0, fR, fS, fT, fR0, fS0, fT0;

    // Set up for a relative error test on the angle between ray direction
    // and triangle normal to determine parallel/nonparallel status.
    Vector3 kN = rkTri.Edge0().Cross(rkTri.Edge1());
    float fNSqrLen = kN.SquaredLength();
    float fDot = rkLine.Direction().Dot(kN);
    bool bNotParallel = (fDot*fDot >= gs_fTolerance*fA00*fNSqrLen);

    if ( bNotParallel )
    {
        Real fCof00 = fA11*fA22-fA12*fA12;
        Real fCof01 = fA02*fA12-fA01*fA22;
        Real fCof02 = fA01*fA12-fA02*fA11;
        Real fCof11 = fA00*fA22-fA02*fA02;
        Real fCof12 = fA02*fA01-fA00*fA12;
        Real fCof22 = fA00*fA11-fA01*fA01;
        Real fInvDet = 1.0f/(fA00*fCof00+fA01*fCof01+fA02*fCof02);
        Real fRhs0 = -fB0*fInvDet;
        Real fRhs1 = -fB1*fInvDet;
        Real fRhs2 = -fB2*fInvDet;

        fR = fCof00*fRhs0+fCof01*fRhs1+fCof02*fRhs2;
        fS = fCof01*fRhs0+fCof11*fRhs1+fCof12*fRhs2;
        fT = fCof02*fRhs0+fCof12*fRhs1+fCof22*fRhs2;

        if ( fS+fT <= 1.0f )
        {
            if ( fS < 0.0f )
            {
                if ( fT < 0.0f )  // region 4
                {
                    // min on face s=0 or t=0
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge1();
                    fSqrDist = SqrDistance(rkLine,kTriSeg,NULL,&fT);
                    fS = 0.0f;
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge0();
                    fSqrDist0 = SqrDistance(rkLine,kTriSeg,NULL,&fS0);
                    fT0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 3
                {
                    // min on face s=0
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge1();
                    fSqrDist = SqrDistance(rkLine,kTriSeg,NULL,&fT);
                    fS = 0.0f;
                }
            }
            else if ( fT < 0.0f )  // region 5
            {
                // min on face t=0
                kTriSeg.Origin() = rkTri.Origin();
                kTriSeg.Direction() = rkTri.Edge0();
                fSqrDist = SqrDistance(rkLine,kTriSeg,NULL,&fS);
                fT = 0.0f;
            }
            else  // region 0
            {
                // line intersects triangle
                fSqrDist = 0.0f;
            }
        }
        else
        {
            if ( fS < 0.0f )  // region 2
            {
                // min on face s=0 or s+t=1
                kTriSeg.Origin() = rkTri.Origin();
                kTriSeg.Direction() = rkTri.Edge1();
                fSqrDist = SqrDistance(rkLine,kTriSeg,NULL,&fT);
                fS = 0.0f;
                kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                fSqrDist0 = SqrDistance(rkLine,kTriSeg,NULL,&fT0);
                fS0 = 1.0f-fT0;
                if ( fSqrDist0 < fSqrDist )
                {
                    fSqrDist = fSqrDist0;
                    fS = fS0;
                    fT = fT0;
                }
            }
            else if ( fT < 0.0f )  // region 6
            {
                // min on face t=0 or s+t=1
                kTriSeg.Origin() = rkTri.Origin();
                kTriSeg.Direction() = rkTri.Edge0();
                fSqrDist = SqrDistance(rkLine,kTriSeg,NULL,&fS);
                fT = 0.0f;
                kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                fSqrDist0 = SqrDistance(rkLine,kTriSeg,NULL,&fT0);
                fS0 = 1.0f-fT0;
                if ( fSqrDist0 < fSqrDist )
                {
                    fSqrDist = fSqrDist0;
                    fS = fS0;
                    fT = fT0;
                }
            }
            else  // region 1
            {
                // min on face s+t=1
                kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                fSqrDist = SqrDistance(rkLine,kTriSeg,NULL,&fT);
                fS = 1.0f-fT;
            }
        }
    }
    else
    {
        // line and triangle are parallel
        kTriSeg.Origin() = rkTri.Origin();
        kTriSeg.Direction() = rkTri.Edge0();
        fSqrDist = SqrDistance(rkLine,kTriSeg,&fR,&fS);
        fT = 0.0f;

        kTriSeg.Direction() = rkTri.Edge1();
        fSqrDist0 = SqrDistance(rkLine,kTriSeg,&fR0,&fT0);
        fS0 = 0.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        kTriSeg.Origin() = rkTri.Origin() + rkTri.Edge0();
        kTriSeg.Direction() = rkTri.Edge1() - rkTri.Edge0();
        fSqrDist0 = SqrDistance(rkLine,kTriSeg,&fR0,&fT0);
        fS0 = 1.0f-fT0;
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

    if ( pfTriP0 )
        *pfTriP0 = fS;

    if ( pfTriP1 )
        *pfTriP1 = fT;

    return fSqrDist;
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Ray3& rkRay, const Triangle3& rkTri,
    Real* pfRayP, Real* pfTriP0, Real* pfTriP1)
{
    Vector3 kDiff = rkTri.Origin() - rkRay.Origin();
    Real fA00 = rkRay.Direction().SquaredLength();
    Real fA01 = -rkRay.Direction().Dot(rkTri.Edge0());
    Real fA02 = -rkRay.Direction().Dot(rkTri.Edge1());
    Real fA11 = rkTri.Edge0().SquaredLength();
    Real fA12 = rkTri.Edge0().Dot(rkTri.Edge1());
    Real fA22 = rkTri.Edge1().Dot(rkTri.Edge1());
    Real fB0  = -kDiff.Dot(rkRay.Direction());
    Real fB1  = kDiff.Dot(rkTri.Edge0());
    Real fB2  = kDiff.Dot(rkTri.Edge1());

    Segment3 kTriSeg;
    Vector3 kPt;
    Real fSqrDist, fSqrDist0, fR, fS, fT, fR0, fS0, fT0;

    // Set up for a relative error test on the angle between ray direction
    // and triangle normal to determine parallel/nonparallel status.
    Vector3 kN = rkTri.Edge0().Cross(rkTri.Edge1());
    float fNSqrLen = kN.SquaredLength();
    float fDot = rkRay.Direction().Dot(kN);
    bool bNotParallel = (fDot*fDot >= gs_fTolerance*fA00*fNSqrLen);

    if ( bNotParallel )
    {
        Real fCof00 = fA11*fA22-fA12*fA12;
        Real fCof01 = fA02*fA12-fA01*fA22;
        Real fCof02 = fA01*fA12-fA02*fA11;
        Real fCof11 = fA00*fA22-fA02*fA02;
        Real fCof12 = fA02*fA01-fA00*fA12;
        Real fCof22 = fA00*fA11-fA01*fA01;
        Real fInvDet = 1.0f/(fA00*fCof00+fA01*fCof01+fA02*fCof02);
        Real fRhs0 = -fB0*fInvDet;
        Real fRhs1 = -fB1*fInvDet;
        Real fRhs2 = -fB2*fInvDet;

        fR = fCof00*fRhs0+fCof01*fRhs1+fCof02*fRhs2;
        fS = fCof01*fRhs0+fCof11*fRhs1+fCof12*fRhs2;
        fT = fCof02*fRhs0+fCof12*fRhs1+fCof22*fRhs2;

        if ( fR <= 0.0f )
        {
            if ( fS+fT <= 1.0f )
            {
                if ( fS < 0.0f )
                {
                    if ( fT < 0.0f )  // region 4m
                    {
                        // min on face s=0 or t=0 or r=0
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge1();
                        fSqrDist = SqrDistance(rkRay,kTriSeg,&fR,&fT);
                        fS = 0.0f;
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge0();
                        fSqrDist0 = SqrDistance(rkRay,kTriSeg,&fR0,&fS0);
                        fT0 = 0.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fR = fR0;
                            fS = fS0;
                            fT = fT0;
                        }
                        fSqrDist0 = SqrDistance(rkRay.Origin(),rkTri,&fS0,
                            &fT0);
                        fR0 = 0.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fR = fR0;
                            fS = fS0;
                            fT = fT0;
                        }
                    }
                    else  // region 3m
                    {
                        // min on face s=0 or r=0
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge1();
                        fSqrDist = SqrDistance(rkRay,kTriSeg,&fR,&fT);
                        fS = 0.0f;
                        fSqrDist0 = SqrDistance(rkRay.Origin(),rkTri,&fS0,
                            &fT0);
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
                else if ( fT < 0.0f )  // region 5m
                {
                    // min on face t=0 or r=0
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge0();
                    fSqrDist = SqrDistance(rkRay,kTriSeg,&fR,&fS);
                    fT = 0.0f;
                    fSqrDist0 = SqrDistance(rkRay.Origin(),rkTri,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 0m
                {
                    // min on face r=0
                    fSqrDist = SqrDistance(rkRay.Origin(),rkTri,&fS,&fT);
                    fR = 0.0f;
                }
            }
            else
            {
                if ( fS < 0.0f )  // region 2m
                {
                    // min on face s=0 or s+t=1 or r=0
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge1();
                    fSqrDist = SqrDistance(rkRay,kTriSeg,&fR,&fT);
                    fS = 0.0f;
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist0 = SqrDistance(rkRay,kTriSeg,&fR0,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    fSqrDist0 = SqrDistance(rkRay.Origin(),rkTri,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT < 0.0f )  // region 6m
                {
                    // min on face t=0 or s+t=1 or r=0
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge0();
                    fSqrDist = SqrDistance(rkRay,kTriSeg,&fR,&fS);
                    fT = 0.0f;
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist0 = SqrDistance(rkRay,kTriSeg,&fR0,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    fSqrDist0 = SqrDistance(rkRay.Origin(),rkTri,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 1m
                {
                    // min on face s+t=1 or r=0
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist = SqrDistance(rkRay,kTriSeg,&fR,&fT);
                    fS = 1.0f-fT;
                    fSqrDist0 = SqrDistance(rkRay.Origin(),rkTri,&fS0,&fT0);
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
            if ( fS+fT <= 1.0f )
            {
                if ( fS < 0.0f )
                {
                    if ( fT < 0.0f )  // region 4p
                    {
                        // min on face s=0 or t=0
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge1();
                        fSqrDist = SqrDistance(rkRay,kTriSeg,NULL,&fT);
                        fS = 0.0f;
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge0();
                        fSqrDist0 = SqrDistance(rkRay,kTriSeg,NULL,&fS0);
                        fT0 = 0.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fS = fS0;
                            fT = fT0;
                        }
                    }
                    else  // region 3p
                    {
                        // min on face s=0
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge1();
                        fSqrDist = SqrDistance(rkRay,kTriSeg,NULL,&fT);
                        fS = 0.0f;
                    }
                }
                else if ( fT < 0.0f )  // region 5p
                {
                    // min on face t=0
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge0();
                    fSqrDist = SqrDistance(rkRay,kTriSeg,NULL,&fS);
                    fT = 0.0f;
                }
                else  // region 0p
                {
                    // ray intersects triangle
                    fSqrDist = 0.0f;
                }
            }
            else
            {
                if ( fS < 0.0f )  // region 2p
                {
                    // min on face s=0 or s+t=1
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge1();
                    fSqrDist = SqrDistance(rkRay,kTriSeg,NULL,&fT);
                    fS = 0.0f;
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist0 = SqrDistance(rkRay,kTriSeg,NULL,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT < 0.0f )  // region 6p
                {
                    // min on face t=0 or s+t=1
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge0();
                    fSqrDist = SqrDistance(rkRay,kTriSeg,NULL,&fS);
                    fT = 0.0f;
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist0 = SqrDistance(rkRay,kTriSeg,NULL,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 1p
                {
                    // min on face s+t=1
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist = SqrDistance(rkRay,kTriSeg,NULL,&fT);
                    fS = 1.0f-fT;
                }
            }
        }
    }
    else
    {
        // ray and triangle are parallel
        kTriSeg.Origin() = rkTri.Origin();
        kTriSeg.Direction() = rkTri.Edge0();
        fSqrDist = SqrDistance(rkRay,kTriSeg,&fR,&fS);
        fT = 0.0f;

        kTriSeg.Direction() = rkTri.Edge1();
        fSqrDist0 = SqrDistance(rkRay,kTriSeg,&fR0,&fT0);
        fS0 = 0.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        kTriSeg.Origin() = rkTri.Origin() + rkTri.Edge0();
        kTriSeg.Direction() = rkTri.Edge1() - rkTri.Edge0();
        fSqrDist0 = SqrDistance(rkRay,kTriSeg,&fR0,&fT0);
        fS0 = 1.0f-fT0;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        fSqrDist0 = SqrDistance(rkRay.Origin(),rkTri,&fS0,&fT0);
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

    if ( pfTriP0 )
        *pfTriP0 = fS;

    if ( pfTriP1 )
        *pfTriP1 = fT;

    return fSqrDist;
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Segment3& rkSeg, const Triangle3& rkTri,
    Real* pfSegP, Real* pfTriP0, Real* pfTriP1)
{
    Vector3 kDiff = rkTri.Origin() - rkSeg.Origin();
    Real fA00 = rkSeg.Direction().SquaredLength();
    Real fA01 = -rkSeg.Direction().Dot(rkTri.Edge0());
    Real fA02 = -rkSeg.Direction().Dot(rkTri.Edge1());
    Real fA11 = rkTri.Edge0().SquaredLength();
    Real fA12 = rkTri.Edge0().Dot(rkTri.Edge1());
    Real fA22 = rkTri.Edge1().Dot(rkTri.Edge1());
    Real fB0  = -kDiff.Dot(rkSeg.Direction());
    Real fB1  = kDiff.Dot(rkTri.Edge0());
    Real fB2  = kDiff.Dot(rkTri.Edge1());

    Segment3 kTriSeg;
    Vector3 kPt;
    Real fSqrDist, fSqrDist0, fR, fS, fT, fR0, fS0, fT0;

    // Set up for a relative error test on the angle between ray direction
    // and triangle normal to determine parallel/nonparallel status.
    Vector3 kN = rkTri.Edge0().Cross(rkTri.Edge1());
    Real fNSqrLen = kN.SquaredLength();
    Real fDot = rkSeg.Direction().Dot(kN);
    bool bNotParallel = (fDot*fDot >= gs_fTolerance*fA00*fNSqrLen);

    if ( bNotParallel )
    {
        Real fCof00 = fA11*fA22-fA12*fA12;
        Real fCof01 = fA02*fA12-fA01*fA22;
        Real fCof02 = fA01*fA12-fA02*fA11;
        Real fCof11 = fA00*fA22-fA02*fA02;
        Real fCof12 = fA02*fA01-fA00*fA12;
        Real fCof22 = fA00*fA11-fA01*fA01;
        Real fInvDet = 1.0f/(fA00*fCof00+fA01*fCof01+fA02*fCof02);
        Real fRhs0 = -fB0*fInvDet;
        Real fRhs1 = -fB1*fInvDet;
        Real fRhs2 = -fB2*fInvDet;

        fR = fCof00*fRhs0+fCof01*fRhs1+fCof02*fRhs2;
        fS = fCof01*fRhs0+fCof11*fRhs1+fCof12*fRhs2;
        fT = fCof02*fRhs0+fCof12*fRhs1+fCof22*fRhs2;

        if ( fR < 0.0f )
        {
            if ( fS+fT <= 1.0f )
            {
                if ( fS < 0.0f )
                {
                    if ( fT < 0.0f )  // region 4m
                    {
                        // min on face s=0 or t=0 or r=0
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge1();
                        fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fT);
                        fS = 0.0f;
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge0();
                        fSqrDist0 = SqrDistance(rkSeg,kTriSeg,&fR0,&fS0);
                        fT0 = 0.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fR = fR0;
                            fS = fS0;
                            fT = fT0;
                        }
                        fSqrDist0 = SqrDistance(rkSeg.Origin(),rkTri,&fS0,
                            &fT0);
                        fR0 = 0.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fR = fR0;
                            fS = fS0;
                            fT = fT0;
                        }
                    }
                    else  // region 3m
                    {
                        // min on face s=0 or r=0
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge1();
                        fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fT);
                        fS = 0.0f;
                        fSqrDist0 = SqrDistance(rkSeg.Origin(),rkTri,&fS0,
                            &fT0);
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
                else if ( fT < 0.0f )  // region 5m
                {
                    // min on face t=0 or r=0
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fS);
                    fT = 0.0f;
                    fSqrDist0 = SqrDistance(rkSeg.Origin(),rkTri,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 0m
                {
                    // min on face r=0
                    fSqrDist = SqrDistance(rkSeg.Origin(),rkTri,&fS,&fT);
                    fR = 0.0f;
                }
            }
            else
            {
                if ( fS < 0.0f )  // region 2m
                {
                    // min on face s=0 or s+t=1 or r=0
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fT);
                    fS = 0.0f;
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kTriSeg,&fR0,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    fSqrDist0 = SqrDistance(rkSeg.Origin(),rkTri,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT < 0.0f )  // region 6m
                {
                    // min on face t=0 or s+t=1 or r=0
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fS);
                    fT = 0.0f;
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kTriSeg,&fR0,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    fSqrDist0 = SqrDistance(rkSeg.Origin(),rkTri,&fS0,&fT0);
                    fR0 = 0.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 1m
                {
                    // min on face s+t=1 or r=0
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fT);
                    fS = 1.0f-fT;
                    fSqrDist0 = SqrDistance(rkSeg.Origin(),rkTri,&fS0,&fT0);
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
            if ( fS+fT <= 1.0f )
            {
                if ( fS < 0.0f )
                {
                    if ( fT < 0.0f )  // region 4
                    {
                        // min on face s=0 or t=0
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge1();
                        fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fT);
                        fS = 0.0f;
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge0();
                        fSqrDist0 = SqrDistance(rkSeg,kTriSeg,&fR0,&fS0);
                        fT0 = 0.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fR = fR0;
                            fS = fS0;
                            fT = fT0;
                        }
                    }
                    else  // region 3
                    {
                        // min on face s=0
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge1();
                        fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fT);
                        fS = 0.0f;
                    }
                }
                else if ( fT < 0.0f )  // region 5
                {
                    // min on face t=0
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fS);
                    fT = 0.0f;
                }
                else  // region 0
                {
                    // global minimum is interior, done
                    fSqrDist = 0.0f;
                }
            }
            else
            {
                if ( fS < 0.0f )  // region 2
                {
                    // min on face s=0 or s+t=1
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fT);
                    fS = 0.0f;
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kTriSeg,&fR0,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT < 0.0f )  // region 6
                {
                    // min on face t=0 or s+t=1
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fS);
                    fT = 0.0f;
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kTriSeg,&fR0,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 1
                {
                    // min on face s+t=1
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fT);
                    fS = 1.0f-fT;
                }
            }
        }
        else  // fR > 1
        {
            if ( fS+fT <= 1.0f )
            {
                if ( fS < 0.0f )
                {
                    if ( fT < 0.0f )  // region 4p
                    {
                        // min on face s=0 or t=0 or r=1
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge1();
                        fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fT);
                        fS = 0.0f;
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge0();
                        fSqrDist0 = SqrDistance(rkSeg,kTriSeg,&fR0,&fS0);
                        fT0 = 0.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fR = fR0;
                            fS = fS0;
                            fT = fT0;
                        }
                        kPt = rkSeg.Origin()+rkSeg.Direction();
                        fSqrDist0 = SqrDistance(kPt,rkTri,&fS0,&fT0);
                        fR0 = 1.0f;
                        if ( fSqrDist0 < fSqrDist )
                        {
                            fSqrDist = fSqrDist0;
                            fR = fR0;
                            fS = fS0;
                            fT = fT0;
                        }
                    }
                    else  // region 3p
                    {
                        // min on face s=0 or r=1
                        kTriSeg.Origin() = rkTri.Origin();
                        kTriSeg.Direction() = rkTri.Edge1();
                        fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fT);
                        fS = 0.0f;
                        kPt = rkSeg.Origin()+rkSeg.Direction();
                        fSqrDist0 = SqrDistance(kPt,rkTri,&fS0,&fT0);
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
                else if ( fT < 0.0f )  // region 5p
                {
                    // min on face t=0 or r=1
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fS);
                    fT = 0.0f;
                    kPt = rkSeg.Origin()+rkSeg.Direction();
                    fSqrDist0 = SqrDistance(kPt,rkTri,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 0p
                {
                    // min face on r=1
                    kPt = rkSeg.Origin()+rkSeg.Direction();
                    fSqrDist = SqrDistance(kPt,rkTri,&fS,&fT);
                    fR = 1.0f;
                }
            }
            else
            {
                if ( fS < 0.0f )  // region 2p
                {
                    // min on face s=0 or s+t=1 or r=1
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge1();
                    fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fT);
                    fS = 0.0f;
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kTriSeg,&fR0,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    kPt = rkSeg.Origin()+rkSeg.Direction();
                    fSqrDist0 = SqrDistance(kPt,rkTri,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else if ( fT < 0.0f )  // region 6p
                {
                    // min on face t=0 or s+t=1 or r=1
                    kTriSeg.Origin() = rkTri.Origin();
                    kTriSeg.Direction() = rkTri.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fS);
                    fT = 0.0f;
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist0 = SqrDistance(rkSeg,kTriSeg,&fR0,&fT0);
                    fS0 = 1.0f-fT0;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                    kPt = rkSeg.Origin()+rkSeg.Direction();
                    fSqrDist0 = SqrDistance(kPt,rkTri,&fS0,&fT0);
                    fR0 = 1.0f;
                    if ( fSqrDist0 < fSqrDist )
                    {
                        fSqrDist = fSqrDist0;
                        fR = fR0;
                        fS = fS0;
                        fT = fT0;
                    }
                }
                else  // region 1p
                {
                    // min on face s+t=1 or r=1
                    kTriSeg.Origin() = rkTri.Origin()+rkTri.Edge0();
                    kTriSeg.Direction() = rkTri.Edge1()-rkTri.Edge0();
                    fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fT);
                    fS = 1.0f-fT;
                    kPt = rkSeg.Origin()+rkSeg.Direction();
                    fSqrDist0 = SqrDistance(kPt,rkTri,&fS0,&fT0);
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
        // segment and triangle are parallel
        kTriSeg.Origin() = rkTri.Origin();
        kTriSeg.Direction() = rkTri.Edge0();
        fSqrDist = SqrDistance(rkSeg,kTriSeg,&fR,&fS);
        fT = 0.0f;

        kTriSeg.Direction() = rkTri.Edge1();
        fSqrDist0 = SqrDistance(rkSeg,kTriSeg,&fR0,&fT0);
        fS0 = 0.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        kTriSeg.Origin() = rkTri.Origin() + rkTri.Edge0();
        kTriSeg.Direction() = rkTri.Edge1() - rkTri.Edge0();
        fSqrDist0 = SqrDistance(rkSeg,kTriSeg,&fR0,&fT0);
        fS0 = 1.0f-fT0;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        fSqrDist0 = SqrDistance(rkSeg.Origin(),rkTri,&fS0,&fT0);
        fR0 = 0.0f;
        if ( fSqrDist0 < fSqrDist )
        {
            fSqrDist = fSqrDist0;
            fR = fR0;
            fS = fS0;
            fT = fT0;
        }

        kPt = rkSeg.Origin()+rkSeg.Direction();
        fSqrDist0 = SqrDistance(kPt,rkTri,&fS0,&fT0);
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

    if ( pfTriP0 )
        *pfTriP0 = fS;

    if ( pfTriP1 )
        *pfTriP1 = fT;

    return fSqrDist;
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Line3& rkLine, const Triangle3& rkTri,
    Real* pfLinP, Real* pfTriP0, Real* pfTriP1)
{
    return Math::Sqrt(SqrDistance(rkLine,rkTri,pfLinP,pfTriP0,pfTriP1));
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Ray3& rkRay, const Triangle3& rkTri,
    Real* pfRayP, Real* pfTriP0, Real* pfTriP1)
{
    return Math::Sqrt(SqrDistance(rkRay,rkTri,pfRayP,pfTriP0,pfTriP1));
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Segment3& rkSeg, const Triangle3& rkTri,
    Real* pfSegP, Real* pfTriP0, Real* pfTriP1)
{
    return Math::Sqrt(SqrDistance(rkSeg,rkTri,pfSegP,pfTriP0,pfTriP1));
}
//----------------------------------------------------------------------------


