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
using namespace Mgc;

static const Real gs_fTolerance = 1e-05f;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Line3& rkLine0, const Line3& rkLine1,
    Real* pfLinP0, Real* pfLinP1)
{
    Vector3 kDiff = rkLine0.Origin() - rkLine1.Origin();
    Real fA00 = rkLine0.Direction().SquaredLength();
    Real fA01 = -rkLine0.Direction().Dot(rkLine1.Direction());
    Real fA11 = rkLine1.Direction().SquaredLength();
    Real fB0 = kDiff.Dot(rkLine0.Direction());
    Real fC = kDiff.SquaredLength();
    Real fDet = Math::FAbs(fA00*fA11-fA01*fA01);
    Real fB1, fS, fT, fSqrDist;

    if ( fDet >= gs_fTolerance )
    {
        // lines are not parallel
        fB1 = -kDiff.Dot(rkLine1.Direction());
        Real fInvDet = 1.0f/fDet;
        fS = (fA01*fB1-fA11*fB0)*fInvDet;
        fT = (fA01*fB0-fA00*fB1)*fInvDet;
        fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
            fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
    }
    else
    {
        // lines are parallel, select any closest pair of points
        fS = -fB0/fA00;
        fT = 0.0f;
        fSqrDist = fB0*fS+fC;
    }

    if ( pfLinP0 )
        *pfLinP0 = fS;

    if ( pfLinP1 )
        *pfLinP1 = fT;

    return Math::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Line3& rkLine, const Ray3& rkRay, Real* pfLinP,
    Real* pfRayP)
{
    Vector3 kDiff = rkLine.Origin() - rkRay.Origin();
    Real fA00 = rkLine.Direction().SquaredLength();
    Real fA01 = -rkLine.Direction().Dot(rkRay.Direction());
    Real fA11 = rkRay.Direction().SquaredLength();
    Real fB0 = kDiff.Dot(rkLine.Direction());
    Real fC = kDiff.SquaredLength();
    Real fDet = Math::FAbs(fA00*fA11-fA01*fA01);
    Real fB1, fS, fT, fSqrDist;

    if ( fDet >= gs_fTolerance )
    {
        fB1 = -kDiff.Dot(rkRay.Direction());
        fT = fA01*fB0-fA00*fB1;

        if ( fT >= 0.0f )
        {
            // two interior points are closest, one on line and one on ray
            Real fInvDet = 1.0f/fDet;
            fS = (fA01*fB1-fA11*fB0)*fInvDet;
            fT *= fInvDet;
            fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
        }
        else
        {
            // end point of ray and interior point of line are closest
            fS = -fB0/fA00;
            fT = 0.0f;
            fSqrDist = fB0*fS+fC;
        }
    }
    else
    {
        // lines are parallel, closest pair with one point at ray origin
        fS = -fB0/fA00;
        fT = 0.0f;
        fSqrDist = fB0*fS+fC;
    }

    if ( pfLinP )
        *pfLinP = fS;

    if ( pfRayP )
        *pfRayP = fT;

    return Math::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Line3& rkLine, const Segment3& rkSeg,
    Real* pfLinP, Real* pfSegP)
{
    Vector3 kDiff = rkLine.Origin() - rkSeg.Origin();
    Real fA00 = rkLine.Direction().SquaredLength();
    Real fA01 = -rkLine.Direction().Dot(rkSeg.Direction());
    Real fA11 = rkSeg.Direction().SquaredLength();
    Real fB0 = kDiff.Dot(rkLine.Direction());
    Real fC = kDiff.SquaredLength();
    Real fDet = Math::FAbs(fA00*fA11-fA01*fA01);
    Real fB1, fS, fT, fSqrDist;

    if ( fDet >= gs_fTolerance )
    {
        fB1 = -kDiff.Dot(rkSeg.Direction());
        fT = fA01*fB0-fA00*fB1;

        if ( fT >= 0.0f )
        {
            if ( fT <= fDet )
            {
                // two interior points are closest, one on line and one on
                // segment
                Real fInvDet = 1.0f/fDet;
                fS = (fA01*fB1-fA11*fB0)*fInvDet;
                fT *= fInvDet;
                fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                    fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
            }
            else
            {
                // end point e1 of segment and interior point of line are
                // closest
                Real fTmp = fA01+fB0;
                fS = -fTmp/fA00;
                fT = 1.0f;
                fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
            }
        }
        else
        {
            // end point e0 of segment and interior point of line are closest
            fS = -fB0/fA00;
            fT = 0.0f;
            fSqrDist = fB0*fS+fC;
        }
    }
    else
    {
        // lines are parallel, closest pair with one point at segment origin
        fS = -fB0/fA00;
        fT = 0.0f;
        fSqrDist = fB0*fS+fC;
    }

    if ( pfLinP )
        *pfLinP = fS;

    if ( pfSegP )
        *pfSegP = fT;

    return Math::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Ray3& rkRay0, const Ray3& rkRay1,
    Real* pfRayP0, Real* pfRayP1)
{
    Vector3 kDiff = rkRay0.Origin() - rkRay1.Origin();
    Real fA00 = rkRay0.Direction().SquaredLength();
    Real fA01 = -rkRay0.Direction().Dot(rkRay1.Direction());
    Real fA11 = rkRay1.Direction().SquaredLength();
    Real fB0 = kDiff.Dot(rkRay0.Direction());
    Real fC = kDiff.SquaredLength();
    Real fDet = Math::FAbs(fA00*fA11-fA01*fA01);
    Real fB1, fS, fT, fSqrDist;

    if ( fDet >= gs_fTolerance )
    {
        // rays are not parallel
        fB1 = -kDiff.Dot(rkRay1.Direction());
        fS = fA01*fB1-fA11*fB0;
        fT = fA01*fB0-fA00*fB1;

        if ( fS >= 0.0f )
        {
            if ( fT >= 0.0f )  // region 0 (interior)
            {
                // minimum at two interior points of rays
                Real fInvDet = 1.0f/fDet;
                fS *= fInvDet;
                fT *= fInvDet;
                fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                    fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
            }
            else  // region 3 (side)
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
        }
        else
        {
            if ( fT >= 0.0f )  // region 1 (side)
            {
                fS = 0.0f;
                if ( fB1 >= 0.0f )
                {
                    fT = 0.0f;
                    fSqrDist = fC;
                }
                else
                {
                    fT = -fB1/fA11;
                    fSqrDist = fB1*fT+fC;
                }
            }
            else  // region 2 (corner)
            {
                if ( fB0 < 0.0f )
                {
                    fS = -fB0/fA00;
                    fT = 0.0f;
                    fSqrDist = fB0*fS+fC;
                }
                else
                {
                    fS = 0.0;
                    if ( fB1 >= 0.0f )
                    {
                        fT = 0.0f;
                        fSqrDist = fC;
                    }
                    else
                    {
                        fT = -fB1/fA11;
                        fSqrDist = fB1*fT+fC;
                    }
                }
            }
        }
    }
    else
    {
        // rays are parallel
        if ( fA01 > 0.0f )
        {
            // opposite direction vectors
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
            // same direction vectors
            if ( fB0 >= 0.0f )
            {
                fB1 = -kDiff.Dot(rkRay1.Direction());
                fS = 0.0f;
                fT = -fB1/fA11;
                fSqrDist = fB1*fT+fC;
            }
            else
            {
                fS = -fB0/fA00;
                fT = 0.0f;
                fSqrDist = fB0*fS+fC;
            }
        }
    }

    if ( pfRayP0 )
        *pfRayP0 = fS;

    if ( pfRayP1 )
        *pfRayP1 = fT;

    return Math::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Ray3& rkRay, const Segment3& rkSeg,
    Real* pfRayP, Real* pfSegP)
{
    Vector3 kDiff = rkRay.Origin() - rkSeg.Origin();
    Real fA00 = rkRay.Direction().SquaredLength();
    Real fA01 = -rkRay.Direction().Dot(rkSeg.Direction());
    Real fA11 = rkSeg.Direction().SquaredLength();
    Real fB0 = kDiff.Dot(rkRay.Direction());
    Real fC = kDiff.SquaredLength();
    Real fDet = Math::FAbs(fA00*fA11-fA01*fA01);
    Real fB1, fS, fT, fSqrDist, fTmp;

    if ( fDet >= gs_fTolerance )
    {
        // ray and segment are not parallel
        fB1 = -kDiff.Dot(rkSeg.Direction());
        fS = fA01*fB1-fA11*fB0;
        fT = fA01*fB0-fA00*fB1;

        if ( fS >= 0.0f )
        {
            if ( fT >= 0.0f )
            {
                if ( fT <= fDet )  // region 0
                {
                    // minimum at interior points of ray and segment
                    Real fInvDet = 1.0f/fDet;
                    fS *= fInvDet;
                    fT *= fInvDet;
                    fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                        fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
                }
                else  // region 1
                {
                    fT = 1.0f;
                    if ( fB0 >= -fA01 )
                    {
                        fS = 0.0f;
                        fSqrDist = fA11+2.0f*fB1+fC;
                    }
                    else
                    {
                        fTmp = fA01 + fB0;
                        fS = -fTmp/fA00;
                        fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
                    }
                }
            }
            else  // region 5
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
        }
        else
        {
            if ( fT <= 0.0f )  // region 4
            {
                if ( fB0 < 0.0f )
                {
                    fS = -fB0/fA00;
                    fT = 0.0f;
                    fSqrDist = fB0*fS+fC;
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
            else if ( fT <= fDet )  // region 3
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
            else  // region 2
            {
                fTmp = fA01+fB0;
                if ( fTmp < 0.0f )
                {
                    fS = -fTmp/fA00;
                    fT = 1.0f;
                    fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
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
        }
    }
    else
    {
        // ray and segment are parallel
        if ( fA01 > 0.0f )
        {
            // opposite direction vectors
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
            // same direction vectors
            fB1 = -kDiff.Dot(rkSeg.Direction());
            fT = 1.0f;
            fTmp = fA01+fB0;
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
    }

    if ( pfRayP )
        *pfRayP = fS;

    if ( pfSegP )
        *pfSegP = fT;

    return Math::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Segment3& rkSeg0, const Segment3& rkSeg1,
    Real* pfSegP0, Real* pfSegP1)
{
    Vector3 kDiff = rkSeg0.Origin() - rkSeg1.Origin();
    Real fA00 = rkSeg0.Direction().SquaredLength();
    Real fA01 = -rkSeg0.Direction().Dot(rkSeg1.Direction());
    Real fA11 = rkSeg1.Direction().SquaredLength();
    Real fB0 = kDiff.Dot(rkSeg0.Direction());
    Real fC = kDiff.SquaredLength();
    Real fDet = Math::FAbs(fA00*fA11-fA01*fA01);
    Real fB1, fS, fT, fSqrDist, fTmp;

    if ( fDet >= gs_fTolerance )
    {
        // line segments are not parallel
        fB1 = -kDiff.Dot(rkSeg1.Direction());
        fS = fA01*fB1-fA11*fB0;
        fT = fA01*fB0-fA00*fB1;
        
        if ( fS >= 0.0f )
        {
            if ( fS <= fDet )
            {
                if ( fT >= 0.0f )
                {
                    if ( fT <= fDet )  // region 0 (interior)
                    {
                        // minimum at two interior points of 3D lines
                        Real fInvDet = 1.0f/fDet;
                        fS *= fInvDet;
                        fT *= fInvDet;
                        fSqrDist = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                            fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
                    }
                    else  // region 3 (side)
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
                            fSqrDist = fA00+fA11+fC+2.0f*(fB1+fTmp);
                        }
                        else
                        {
                            fS = -fTmp/fA00;
                            fSqrDist = fTmp*fS+fA11+2.0f*fB1+fC;
                        }
                    }
                }
                else  // region 7 (side)
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
            }
            else
            {
                if ( fT >= 0.0 )
                {
                    if ( fT <= fDet )  // region 1 (side)
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
                            fSqrDist = fA00+fA11+fC+2.0f*(fB0+fTmp);
                        }
                        else
                        {
                            fT = -fTmp/fA11;
                            fSqrDist = fTmp*fT+fA00+2.0f*fB0+fC;
                        }
                    }
                    else  // region 2 (corner)
                    {
                        fTmp = fA01+fB0;
                        if ( -fTmp <= fA00 )
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
                                fSqrDist = fA00+fA11+fC+2.0f*(fB0+fTmp);
                            }
                            else
                            {
                                fT = -fTmp/fA11;
                                fSqrDist = fTmp*fT+fA00+2.0f*fB0+fC;
                            }
                        }
                    }
                }
                else  // region 8 (corner)
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
                            fSqrDist = fA00+fA11+fC+2.0f*(fB0+fTmp);
                        }
                        else
                        {
                            fT = -fTmp/fA11;
                            fSqrDist = fTmp*fT+fA00+2.0f*fB0+fC;
                        }
                    }
                }
            }
        }
        else 
        {
            if ( fT >= 0.0f )
            {
                if ( fT <= fDet )  // region 5 (side)
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
                else  // region 4 (corner)
                {
                    fTmp = fA01+fB0;
                    if ( fTmp < 0.0f )
                    {
                        fT = 1.0f;
                        if ( -fTmp >= fA00 )
                        {
                            fS = 1.0f;
                            fSqrDist = fA00+fA11+fC+2.0f*(fB1+fTmp);
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
                            fSqrDist = fA11+2.0f*fB1+fC;
                        }
                        else
                        {
                            fT = -fB1/fA11;
                            fSqrDist = fB1*fT+fC;
                        }
                    }
                }
            }
            else   // region 6 (corner)
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
        }
    }
    else
    {
        // line segments are parallel
        if ( fA01 > 0.0f )
        {
            // direction vectors form an obtuse angle
            if ( fB0 >= 0.0f )
            {
                fS = 0.0f;
                fT = 0.0f;
                fSqrDist = fC;
            }
            else if ( -fB0 <= fA00 )
            {
                fS = -fB0/fA00;
                fT = 0.0f;
                fSqrDist = fB0*fS+fC;
            }
            else
            {
                fB1 = -kDiff.Dot(rkSeg1.Direction());
                fS = 1.0f;
                fTmp = fA00+fB0;
                if ( -fTmp >= fA01 )
                {
                    fT = 1.0f;
                    fSqrDist = fA00+fA11+fC+2.0f*(fA01+fB0+fB1);
                }
                else
                {
                    fT = -fTmp/fA01;
                    fSqrDist = fA00+2.0f*fB0+fC+fT*(fA11*fT+2.0f*(fA01+fB1));
                }
            }
        }
        else
        {
            // direction vectors form an acute angle
            if ( -fB0 >= fA00 )
            {
                fS = 1.0f;
                fT = 0.0f;
                fSqrDist = fA00+2.0f*fB0+fC;
            }
            else if ( fB0 <= 0.0f )
            {
                fS = -fB0/fA00;
                fT = 0.0f;
                fSqrDist = fB0*fS+fC;
            }
            else
            {
                fB1 = -kDiff.Dot(rkSeg1.Direction());
                fS = 0.0f;
                if ( fB0 >= -fA01 )
                {
                    fT = 1.0f;
                    fSqrDist = fA11+2.0f*fB1+fC;
                }
                else
                {
                    fT = -fB0/fA01;
                    fSqrDist = fC+fT*(2.0f*fB1+fA11*fT);
                }
            }
        }
    }

    if ( pfSegP0 )
        *pfSegP0 = fS;

    if ( pfSegP1 )
        *pfSegP1 = fT;

    return Math::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Line3& rkLine0, const Line3& rkLine1,
    Real* pfLinP0, Real* pfLinP1)
{
    return Math::Sqrt(SqrDistance(rkLine0,rkLine1,pfLinP0,pfLinP1));
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Line3& rkLine, const Ray3& rkRay, Real* pfLinP,
    Real* pfRayP)
{
    return Math::Sqrt(SqrDistance(rkLine,rkRay,pfLinP,pfRayP));
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Line3& rkLine, const Segment3& rkSeg,
    Real* pfLinP, Real* pfSegP)
{
    return Math::Sqrt(SqrDistance(rkLine,rkSeg,pfLinP,pfSegP));
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Ray3& rkRay0, const Ray3& rkRay1, Real* pfRayP0,
    Real* pfRayP1)
{
    return Math::Sqrt(SqrDistance(rkRay0,rkRay1,pfRayP0,pfRayP1));
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Ray3& rkRay, const Segment3& rkSeg, Real* pfRayP,
    Real* pfSegP)
{
    return Math::Sqrt(SqrDistance(rkRay,rkSeg,pfRayP,pfSegP));
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Segment3& rkSeg0, const Segment3& rkSeg1,
    Real* pfSegP0, Real* pfSegP1)
{
    return Math::Sqrt(SqrDistance(rkSeg0,rkSeg1,pfSegP0,pfSegP1));
}
//----------------------------------------------------------------------------


