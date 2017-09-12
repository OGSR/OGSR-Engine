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

#include "MgcDist3DVecBox.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Vector3& rkPoint, const Box3& rkBox,
    Real* pfBParam0, Real* pfBParam1, Real* pfBParam2)
{
#ifdef _DEBUG
    // The three parameters pointers are either all non-null or all null.
    if ( pfBParam0 )
    {
        assert( pfBParam1 && pfBParam2 );
    }
    else
    {
        assert( !pfBParam1 && !pfBParam2 );
    }
#endif

    // compute coordinates of point in box coordinate system
    Vector3 kDiff = rkPoint - rkBox.Center();
    Vector3 kClosest(kDiff.Dot(rkBox.Axis(0)),kDiff.Dot(rkBox.Axis(1)),
        kDiff.Dot(rkBox.Axis(2)));

    // project test point onto box
    Real fSqrDistance = 0.0f;
    Real fDelta;

    if ( kClosest.x < -rkBox.Extent(0) )
    {
        fDelta = kClosest.x + rkBox.Extent(0);
        fSqrDistance += fDelta*fDelta;
        kClosest.x = -rkBox.Extent(0);
    }
    else if ( kClosest.x > rkBox.Extent(0) )
    {
        fDelta = kClosest.x - rkBox.Extent(0);
        fSqrDistance += fDelta*fDelta;
        kClosest.x = rkBox.Extent(0);
    }

    if ( kClosest.y < -rkBox.Extent(1) )
    {
        fDelta = kClosest.y + rkBox.Extent(1);
        fSqrDistance += fDelta*fDelta;
        kClosest.y = -rkBox.Extent(1);
    }
    else if ( kClosest.y > rkBox.Extent(1) )
    {
        fDelta = kClosest.y - rkBox.Extent(1);
        fSqrDistance += fDelta*fDelta;
        kClosest.y = rkBox.Extent(1);
    }

    if ( kClosest.z < -rkBox.Extent(2) )
    {
        fDelta = kClosest.z + rkBox.Extent(2);
        fSqrDistance += fDelta*fDelta;
        kClosest.z = -rkBox.Extent(2);
    }
    else if ( kClosest.z > rkBox.Extent(2) )
    {
        fDelta = kClosest.z - rkBox.Extent(2);
        fSqrDistance += fDelta*fDelta;
        kClosest.z = rkBox.Extent(2);
    }

    if ( pfBParam0 )
    {
        *pfBParam0 = kClosest.x;
        *pfBParam1 = kClosest.y;
        *pfBParam2 = kClosest.z;
    }

    return fSqrDistance;
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Vector3& rkPoint, const Box3& rkBox,
    Real* pfBParam0, Real* pfBParam1, Real* pfBParam2)
{
    return Math::Sqrt(SqrDistance(rkPoint,rkBox,pfBParam0,pfBParam1,
        pfBParam2));
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Vector3& rkPoint, float fXMin, float fXMax,
    float fYMin, float fYMax, float fZMin, float fZMax, Vector3* pkClosest)
{
    Real fSqrDistance = 0.0f;
    Real fDelta;

    if ( pkClosest )
    {
        if ( rkPoint.x < fXMin )
        {
            fDelta = rkPoint.x - fXMin;
            fSqrDistance += fDelta*fDelta;
            pkClosest->x = fXMin;
        }
        else if ( rkPoint.x > fXMax )
        {
            fDelta = rkPoint.x - fXMax;
            fSqrDistance += fDelta*fDelta;
            pkClosest->x = fXMax;
        }
        else
        {
            pkClosest->x = rkPoint.x;
        }

        if ( rkPoint.y < fYMin )
        {
            fDelta = rkPoint.y - fYMin;
            fSqrDistance += fDelta*fDelta;
            pkClosest->y = fYMin;
        }
        else if ( rkPoint.y > fYMax )
        {
            fDelta = rkPoint.y - fYMax;
            fSqrDistance += fDelta*fDelta;
            pkClosest->y = fYMax;
        }
        else
        {
            pkClosest->y = rkPoint.y;
        }

        if ( rkPoint.z < fZMin )
        {
            fDelta = rkPoint.z - fZMin;
            fSqrDistance += fDelta*fDelta;
            pkClosest->z = fZMin;
        }
        else if ( rkPoint.z > fZMax )
        {
            fDelta = rkPoint.z - fZMax;
            fSqrDistance += fDelta*fDelta;
            pkClosest->z = fZMax;
        }
        else
        {
            pkClosest->z = rkPoint.z;
        }
    }
    else
    {
        if ( rkPoint.x < fXMin )
        {
            fDelta = rkPoint.x - fXMin;
            fSqrDistance += fDelta*fDelta;
        }
        else if ( rkPoint.x > fXMax )
        {
            fDelta = rkPoint.x - fXMax;
            fSqrDistance += fDelta*fDelta;
        }

        if ( rkPoint.y < fYMin )
        {
            fDelta = rkPoint.y - fYMin;
            fSqrDistance += fDelta*fDelta;
        }
        else if ( rkPoint.y > fYMax )
        {
            fDelta = rkPoint.y - fYMax;
            fSqrDistance += fDelta*fDelta;
        }

        if ( rkPoint.z < fZMin )
        {
            fDelta = rkPoint.z - fZMin;
            fSqrDistance += fDelta*fDelta;
        }
        else if ( rkPoint.z > fZMax )
        {
            fDelta = rkPoint.z - fZMax;
            fSqrDistance += fDelta*fDelta;
        }
    }

    return fSqrDistance;
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Vector3& rkPoint, float fXMin, float fXMax,
    float fYMin, float fYMax, float fZMin, float fZMax, Vector3* pkClosest)
{
    return Math::Sqrt(SqrDistance(rkPoint,fXMin,fXMax,fYMin,fYMax,fZMin,
        fZMax,pkClosest));
}
//----------------------------------------------------------------------------
