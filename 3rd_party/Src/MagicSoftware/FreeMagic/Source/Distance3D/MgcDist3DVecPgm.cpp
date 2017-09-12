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

#include "MgcDist3DVecPgm.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Vector3& rkPoint, const Parallelogram3& rkPgm,
    Real* pfSParam, Real* pfTParam)
{
    Vector3 kDiff = rkPgm.Origin() - rkPoint;
    Real fA00 = rkPgm.Edge0().SquaredLength();
    Real fA01 = rkPgm.Edge0().Dot(rkPgm.Edge1());
    Real fA11 = rkPgm.Edge1().SquaredLength();
    Real fB0 = kDiff.Dot(rkPgm.Edge0());
    Real fB1 = kDiff.Dot(rkPgm.Edge1());
    Real fC = kDiff.SquaredLength();
    Real fDet = Math::FAbs(fA00*fA11-fA01*fA01);
    Real fS = fA01*fB1-fA11*fB0;
    Real fT = fA01*fB0-fA00*fB1;
    Real fSqrDist, fTmp;

    if ( fS < 0.0f )
    {
        if ( fT < 0.0f )  // region 6
        {
            if ( fB0 < 0.0f )
            {
                fT = 0.0f;
                if ( -fB0 >= fA00 )
                {
                    fS = 1.0f;
                    fSqrDist = fA00+2.0f*fB0+fC;
                }
                else
                {
                    fS = -fB0/fA00;
                    fSqrDist = fB0*fS+fC;
                }
            }
            else
            {
                fS = 0.0f;
                if ( fB1 >= 0.0f )
                {
                    fT = 0.0f;
                    fSqrDist = fC;
                }
                else if ( -fB1 >= fA11 )
                {
                    fT = 1.0f;
                    fSqrDist = fA11+2.0f*fB1+fC;
                }
                else
                {
                    fT = -fB1/fA11;
                    fSqrDist = fB1*fT+fC;
                }
            }
        }
        else if ( fT <= fDet )  // region 5
        {
            fS = 0.0f;
            if ( fB1 >= 0.0f )
            {
                fT = 0.0f;
                fSqrDist = fC;
            }
            else if ( -fB1 >= fA11 )
            {
                fT = 1.0f;
                fSqrDist = fA11+2.0f*fB1+fC;
            }
            else
            {
                fT = -fB1/fA11;
                fSqrDist = fB1*fT+fC;
            }
        }
        else  // region 4
        {
            fTmp = fA01+fB0;
            if ( fTmp < 0.0f )
            {
                fT = 1.0f;
                if ( -fTmp >= fA00 )
                {
                    fS = 1.0f;
                    fSqrDist = fA00+fA11+fC+2.0f*(fA01+fB0+fB1);
                }
                else
                {
                    fS = -fTmp/fA00;
                    fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
                }
            }
            else
            {
                fS = 0.0f;
                if ( fB1 >= 0.0f )
                {
                    fT = 0.0f;
                    fSqrDist = fC;
                }
                else if ( -fB1 >= fA11 )
                {
                    fT = 1.0f;
                    fSqrDist = fA11+2*fB1+fC;
                }
                else
                {
                    fT = -fB1/fA11;
                    fSqrDist = fB1*fT+fC;
                }
            }
        }
    }
    else if ( fS <= fDet )
    {
        if ( fT < 0.0f )  // region 7
        {
            fT = 0.0f;
            if ( fB0 >= 0.0f )
            {
                fS = 0.0f;
                fSqrDist = fC;
            }
            else if ( -fB0 >= fA00 )
            {
                fS = 1.0f;
                fSqrDist = fA00+2.0f*fB0+fC;
            }
            else
            {
                fS = -fB0/fA00;
                fSqrDist = fB0*fS+fC;
            }
        }
        else if ( fT <= fDet )  // region 0
        {
            // minimum at interior point
            Real fInvDet = 1.0f/fDet;
            fS *= fInvDet;
            fT *= fInvDet;
            fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
        }
        else  // region 3
        {
            fT = 1.0f;
            fTmp = fA01+fB0;
            if ( fTmp >= 0.0f )
            {
                fS = 0.0f;
                fSqrDist = fA11+2.0f*fB1+fC;
            }
            else if ( -fTmp >= fA00 )
            {
                fS = 1.0f;
                fSqrDist = fA00+fA11+fC+2.0f*(fA01+fB0+fB1);
            }
            else
            {
                fS = -fTmp/fA00;
                fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
            }
        }
    }
    else
    {
        if ( fT < 0.0f )  // region 8
        {
            if ( -fB0 < fA00 )
            {
                fT = 0.0f;
                if ( fB0 >= 0.0f )
                {
                    fS = 0.0f;
                    fSqrDist = fC;
                }
                else
                {
                    fS = -fB0/fA00;
                    fSqrDist = fB0*fS+fC;
                }
            }
            else
            {
                fS = 1.0f;
                fTmp = fA01+fB1;
                if ( fTmp >= 0.0f )
                {
                    fT = 0.0f;
                    fSqrDist = fA00+2.0f*fB0+fC;
                }
                else if ( -fTmp >= fA11 )
                {
                    fT = 1.0f;
                    fSqrDist = fA00+fA11+fC+2.0f*(fA01+fB0+fB1);
                }
                else
                {
                    fT = -fTmp/fA11;
                    fSqrDist = fTmp*fT+fA00+2.0f*fB0+fC;
                }
            }
        }
        else if ( fT <= fDet )  // region 1
        {
            fS = 1.0;
            fTmp = fA01+fB1;
            if ( fTmp >= 0.0f )
            {
                fT = 0.0f;
                fSqrDist = fA00+2.0f*fB0+fC;
            }
            else if ( -fTmp >= fA11 )
            {
                fT = 1.0f;
                fSqrDist = fA00+fA11+fC+2.0f*(fA01+fB0+fB1);
            }
            else
            {
                fT = -fTmp/fA11;
                fSqrDist = fTmp*fT+fA00+2.0f*fB0+fC;
            }
        }
        else  // region 2
        {
            fTmp = fA01+fB0;
            if ( -fTmp < fA00 )
            {
                fT = 1.0f;
                if ( fTmp >= 0.0f )
                {
                    fS = 0.0f;
                    fSqrDist = fA11+2.0f*fB1+fC;
                }
                else
                {
                    fS = -fTmp/fA00;
                    fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
                }
            }
            else
            {
                fS = 1.0f;
                fTmp = fA01+fB1;
                if ( fTmp >= 0.0f )
                {
                    fT = 0.0f;
                    fSqrDist = fA00+2.0f*fB0+fC;
                }
                else if ( -fTmp >= fA11 )
                {
                    fT = 1.0f;
                    fSqrDist = fA00+fA11+fC+2.0f*(fA01+fB0+fB1);
                }
                else
                {
                    fT = -fTmp/fA11;
                    fSqrDist = fTmp*fT+fA00+2.0f*fB0+fC;
                }
            }
        }
    }

    if ( pfSParam )
        *pfSParam = fS;

    if ( pfTParam )
        *pfTParam = fT;

    return Math::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Vector3& rkPoint, const Parallelogram3& rkPgm,
    Real* pfSParam, Real* pfTParam)
{
    return Math::Sqrt(SqrDistance(rkPoint,rkPgm,pfSParam,pfTParam));
}
//----------------------------------------------------------------------------


