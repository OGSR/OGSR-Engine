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

#include "MgcAppr3DGaussPointsFit.h"
#include "MgcCont3DLozenge.h"
#include "MgcDist3DVecLin.h"
#include "MgcDist3DVecRct.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Lozenge Mgc::ContLozenge (int iQuantity, const Vector3* akPoint)
{
    Lozenge kLozenge;

    // Fit with Gaussian.  Axis(0) corresponds to the smallest eigenvalue.
    Vector3 kCenter, akAxis[3];
    Real afExtent[3];
    GaussPointsFit(iQuantity,akPoint,kCenter,akAxis,afExtent);

    Vector3 kDiff = akPoint[0] - kCenter;
    Real fWMin = akAxis[0].Dot(kDiff), fWMax = fWMin, fW;
    int i;
    for (i = 1; i < iQuantity; i++)
    {
        kDiff = akPoint[i] - kCenter;
        fW = akAxis[0].Dot(kDiff);
        if ( fW < fWMin )
            fWMin = fW;
        else if ( fW > fWMax )
            fWMax = fW;
    }

    Real fRadius = 0.5f*(fWMax - fWMin);
    Real fRSqr = fRadius*fRadius;
    kCenter += (0.5f*(fWMax + fWMin))*akAxis[0];

    Real fAMin = Math::MAX_REAL, fAMax = -Math::MAX_REAL;
    Real fBMin = Math::MAX_REAL, fBMax = -Math::MAX_REAL;
    Real fDiscr, fRadical, fU, fV, fTest;
    for (i = 0; i < iQuantity; i++)
    {
        kDiff = akPoint[i] - kCenter;
        fU = akAxis[2].Dot(kDiff);
        fV = akAxis[1].Dot(kDiff);
        fW = akAxis[0].Dot(kDiff);
        fDiscr = fRSqr - fW*fW;
        fRadical = Math::Sqrt(Math::FAbs(fDiscr));

        fTest = fU + fRadical;
        if ( fTest < fAMin )
            fAMin = fTest;

        fTest = fU - fRadical;
        if ( fTest > fAMax )
            fAMax = fTest;

        fTest = fV + fRadical;
        if ( fTest < fBMin )
            fBMin = fTest;

        fTest = fV - fRadical;
        if ( fTest > fBMax )
            fBMax = fTest;
    }

    // enclosing region might be a capsule or a sphere
    if ( fAMin >= fAMax )
    {
        fTest = 0.5f*(fAMin + fAMax);
        fAMin = fTest;
        fAMax = fTest;
    }
    if ( fBMin >= fBMax )
    {
        fTest = 0.5f*(fBMin + fBMax);
        fBMin = fTest;
        fBMax = fTest;
    }

    // Make correction for points inside mitered corner but outside quarter
    // sphere.
    for (i = 0; i < iQuantity; i++)
    {
        kDiff = akPoint[i] - kCenter;
        fU = akAxis[2].Dot(kDiff);
        fV = akAxis[1].Dot(kDiff);

        Real* pfAExtreme = NULL;
        Real* pfBExtreme = NULL;

        if ( fU > fAMax )
        {
            if ( fV > fBMax )
            {
                pfAExtreme = &fAMax;
                pfBExtreme = &fBMax;
            }
            else if ( fV < fBMin )
            {
                pfAExtreme = &fAMax;
                pfBExtreme = &fBMin;
            }
        }
        else if ( fU < fAMin )
        {
            if ( fV > fBMax )
            {
                pfAExtreme = &fAMin;
                pfBExtreme = &fBMax;
            }
            else if ( fV < fBMin )
            {
                pfAExtreme = &fAMin;
                pfBExtreme = &fBMin;
            }
        }

        if ( pfAExtreme )
        {
            Real fDeltaU = fU - *pfAExtreme;
            Real fDeltaV = fV - *pfBExtreme;
            Real fDeltaSumSqr = fDeltaU*fDeltaU + fDeltaV*fDeltaV;
            fW = akAxis[0].Dot(kDiff);
            Real fWSqr = fW*fW;
            fTest = fDeltaSumSqr + fWSqr;
            if ( fTest > fRSqr )
            {
                fDiscr = (fRSqr - fWSqr)/fDeltaSumSqr;
                Real fT = -Math::Sqrt(Math::FAbs(fDiscr));
                *pfAExtreme = fU + fT*fDeltaU;
                *pfBExtreme = fV + fT*fDeltaV;
            }
        }
    }

    if ( fAMin < fAMax )
    {
        if ( fBMin < fBMax )
        {
            kLozenge.Origin() = kCenter + fAMin*akAxis[2] + fBMin*akAxis[1];
            kLozenge.Edge0() = (fAMax - fAMin)*akAxis[2];
            kLozenge.Edge1() = (fBMax - fBMin)*akAxis[1];
        }
        else
        {
            // enclosing lozenge is really a capsule
            kLozenge.Origin() = kCenter + fAMin*akAxis[2] +
                (0.5f*(fBMin+fBMax))*akAxis[1];
            kLozenge.Edge0() = (fAMax - fAMin)*akAxis[2];
            kLozenge.Edge1() = Vector3::ZERO;
        }
    }
    else
    {
        if ( fBMin < fBMax )
        {
            // enclosing lozenge is really a capsule
            kLozenge.Origin() = kCenter + (0.5f*(fAMin+fAMax))*akAxis[2] +
                fBMin*akAxis[1];
            kLozenge.Edge0() = Vector3::ZERO;
            kLozenge.Edge1() = (fBMax - fBMin)*akAxis[1];
        }
        else
        {
            // enclosing lozenge is really a sphere
            kLozenge.Origin() = kCenter + (0.5f*(fAMin+fAMax))*akAxis[2] +
                (0.5f*(fBMin+fBMax))*akAxis[1];
            kLozenge.Edge0() = Vector3::ZERO;
            kLozenge.Edge1() = Vector3::ZERO;
        }
    }

    kLozenge.Radius() = fRadius;

    return kLozenge;
}
//----------------------------------------------------------------------------
bool Mgc::ContLozenge (int iQuantity, const Vector3* akPoint,
    const bool* abValid, Lozenge& rkLozenge)
{
    // Fit with Gaussian.  Axis(0) corresponds to the smallest eigenvalue.
    Vector3 kCenter, akAxis[3];
    Real afExtent[3];
    if ( !GaussPointsFit(iQuantity,akPoint,abValid,kCenter,akAxis,afExtent) )
        return false;

    Vector3 kDiff;
    Real fWMin, fWMax, fW;
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            kDiff = akPoint[i] - kCenter;
            fWMin = akAxis[0].Dot(kDiff);
            fWMax = fWMin;
            break;
        }
    }

    for (i++; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            kDiff = akPoint[i] - kCenter;
            fW = akAxis[0].Dot(kDiff);
            if ( fW < fWMin )
                fWMin = fW;
            else if ( fW > fWMax )
                fWMax = fW;
        }
    }

    Real fRadius = 0.5f*(fWMax - fWMin);
    Real fRSqr = fRadius*fRadius;
    kCenter += (0.5f*(fWMax + fWMin))*akAxis[0];

    Real fAMin = Math::MAX_REAL, fAMax = -Math::MAX_REAL;
    Real fBMin = Math::MAX_REAL, fBMax = -Math::MAX_REAL;
    Real fDiscr, fRadical, fU, fV, fTest;
    for (i = 0; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            kDiff = akPoint[i] - kCenter;
            fU = akAxis[2].Dot(kDiff);
            fV = akAxis[1].Dot(kDiff);
            fW = akAxis[0].Dot(kDiff);
            fDiscr = fRSqr - fW*fW;
            fRadical = Math::Sqrt(Math::FAbs(fDiscr));

            fTest = fU + fRadical;
            if ( fTest < fAMin )
                fAMin = fTest;

            fTest = fU - fRadical;
            if ( fTest > fAMax )
                fAMax = fTest;

            fTest = fV + fRadical;
            if ( fTest < fBMin )
                fBMin = fTest;

            fTest = fV - fRadical;
            if ( fTest > fBMax )
                fBMax = fTest;
        }
    }

    // enclosing region might be a capsule or a sphere
    if ( fAMin >= fAMax )
    {
        fTest = 0.5f*(fAMin + fAMax);
        fAMin = fTest;
        fAMax = fTest;
    }
    if ( fBMin >= fBMax )
    {
        fTest = 0.5f*(fBMin + fBMax);
        fBMin = fTest;
        fBMax = fTest;
    }

    // Make correction for points inside mitered corner but outside quarter
    // sphere.
    for (i = 0; i < iQuantity; i++)
    {
        if ( abValid[i] )
        {
            kDiff = akPoint[i] - kCenter;
            fU = akAxis[2].Dot(kDiff);
            fV = akAxis[1].Dot(kDiff);

            Real* pfAExtreme = NULL;
            Real* pfBExtreme = NULL;

            if ( fU > fAMax )
            {
                if ( fV > fBMax )
                {
                    pfAExtreme = &fAMax;
                    pfBExtreme = &fBMax;
                }
                else if ( fV < fBMin )
                {
                    pfAExtreme = &fAMax;
                    pfBExtreme = &fBMin;
                }
            }
            else if ( fU < fAMin )
            {
                if ( fV > fBMax )
                {
                    pfAExtreme = &fAMin;
                    pfBExtreme = &fBMax;
                }
                else if ( fV < fBMin )
                {
                    pfAExtreme = &fAMin;
                    pfBExtreme = &fBMin;
                }
            }

            if ( pfAExtreme )
            {
                Real fDeltaU = fU - *pfAExtreme;
                Real fDeltaV = fV - *pfBExtreme;
                Real fDeltaSumSqr = fDeltaU*fDeltaU + fDeltaV*fDeltaV;
                fW = akAxis[0].Dot(kDiff);
                Real fWSqr = fW*fW;
                fTest = fDeltaSumSqr + fWSqr;
                if ( fTest > fRSqr )
                {
                    fDiscr = (fRSqr - fWSqr)/fDeltaSumSqr;
                    Real fT = -Math::Sqrt(Math::FAbs(fDiscr));
                    *pfAExtreme = fU + fT*fDeltaU;
                    *pfBExtreme = fV + fT*fDeltaV;
                }
            }
        }
    }

    if ( fAMin < fAMax )
    {
        if ( fBMin < fBMax )
        {
            rkLozenge.Origin() = kCenter + fAMin*akAxis[2] + fBMin*akAxis[1];
            rkLozenge.Edge0() = (fAMax - fAMin)*akAxis[2];
            rkLozenge.Edge1() = (fBMax - fBMin)*akAxis[1];
        }
        else
        {
            // enclosing lozenge is really a capsule
            rkLozenge.Origin() = kCenter + fAMin*akAxis[2] +
                (0.5f*(fBMin+fBMax))*akAxis[1];
            rkLozenge.Edge0() = (fAMax - fAMin)*akAxis[2];
            rkLozenge.Edge1() = Vector3::ZERO;
        }
    }
    else
    {
        if ( fBMin < fBMax )
        {
            // enclosing lozenge is really a capsule
            rkLozenge.Origin() = kCenter + (0.5f*(fAMin+fAMax))*akAxis[2] +
                fBMin*akAxis[1];
            rkLozenge.Edge0() = Vector3::ZERO;
            rkLozenge.Edge1() = (fBMax - fBMin)*akAxis[1];
        }
        else
        {
            // enclosing lozenge is really a sphere
            rkLozenge.Origin() = kCenter + (0.5f*(fAMin+fAMax))*akAxis[2] +
                (0.5f*(fBMin+fBMax))*akAxis[1];
            rkLozenge.Edge0() = Vector3::ZERO;
            rkLozenge.Edge1() = Vector3::ZERO;
        }
    }

    rkLozenge.Radius() = fRadius;

    return true;
}
//----------------------------------------------------------------------------
bool Mgc::InLozenge (const Vector3& rkPoint, const Lozenge& rkLozenge,
    Real fEpsilon)
{
    Real fRSqr = rkLozenge.Radius()*rkLozenge.Radius();
    Real fSqrDist = SqrDistance(rkPoint,rkLozenge.Rectangle());
    return fSqrDist <= fRSqr + fEpsilon;
}
//----------------------------------------------------------------------------


