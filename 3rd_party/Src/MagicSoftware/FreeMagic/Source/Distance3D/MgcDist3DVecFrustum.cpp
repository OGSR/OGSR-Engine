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

#include "MgcDist3DVecLin.h"
#include "MgcDist3DVecFrustum.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Vector3& rkPoint, const Frustum& rkFrustum,
    Vector3* pkClosest)
{
    // compute coordinates of point with respect to frustum coordinate system
    Vector3 kDiff = rkPoint - rkFrustum.Origin();
    Vector3 kTest = Vector3(
        kDiff.Dot(rkFrustum.LVector()),
        kDiff.Dot(rkFrustum.UVector()),
        kDiff.Dot(rkFrustum.DVector()));

    // perform calculations in octant with nonnegative L and U coordinates
    bool bLSignChange;
    if ( kTest.x < 0.0f )
    {
        bLSignChange = true;
        kTest.x = -kTest.x;
    }
    else
    {
        bLSignChange = false;
    }

    bool bUSignChange;
    if ( kTest.y < 0.0f )
    {
        bUSignChange = true;
        kTest.y = -kTest.y;
    }
    else
    {
        bUSignChange = false;
    }

    // frustum derived parameters
    Real fLMin = rkFrustum.LBound(), fLMax = rkFrustum.GetDRatio()*fLMin;
    Real fUMin = rkFrustum.UBound(), fUMax = rkFrustum.GetDRatio()*fUMin;
    Real fDMin = rkFrustum.DMin(), fDMax = rkFrustum.DMax();
    Real fLMinSqr = fLMin*fLMin;
    Real fUMinSqr = fUMin*fUMin;
    Real fDMinSqr = fDMin*fDMin;
    Real fMinLDDot = fLMinSqr + fDMinSqr;
    Real fMinUDDot = fUMinSqr + fDMinSqr;
    Real fMinLUDDot = fLMinSqr + fMinUDDot;
    Real fMaxLDDot = rkFrustum.GetDRatio()*fMinLDDot;
    Real fMaxUDDot = rkFrustum.GetDRatio()*fMinUDDot;
    Real fMaxLUDDot = rkFrustum.GetDRatio()*fMinLUDDot;

    // Algorithm computes closest point in all cases by determining in which
    // Voronoi region of the vertices, edges, and faces of the frustum that
    // the test point lives.
    Vector3 kClosest;
    Real fLDot, fUDot, fLDDot, fUDDot, fLUDDot, fLEdgeDot, fUEdgeDot, fT;

    if ( kTest.z >= fDMax )
    {
        if ( kTest.x <= fLMax )
        {
            if ( kTest.y <= fUMax )
            {
                // F-face
                kClosest.x = kTest.x;
                kClosest.y = kTest.y;
                kClosest.z = fDMax;
            }
            else
            {
                // UF-edge
                kClosest.x = kTest.x;
                kClosest.y = fUMax;
                kClosest.z = fDMax;
            }
        }
        else
        {
            if ( kTest.y <= fUMax )
            {
                // LF-edge
                kClosest.x = fLMax;
                kClosest.y = kTest.y;
                kClosest.z = fDMax;
            }
            else
            {
                // LUF-vertex
                kClosest.x = fLMax;
                kClosest.y = fUMax;
                kClosest.z = fDMax;
            }
        }
    }
    else if ( kTest.z <= fDMin )
    {
        if ( kTest.x <= fLMin )
        {
            if ( kTest.y <= fUMin )
            {
                // N-face
                kClosest.x = kTest.x;
                kClosest.y = kTest.y;
                kClosest.z = fDMin;
            }
            else
            {
                fUDDot = fUMin*kTest.y + fDMin*kTest.z;
                if ( fUDDot >= fMaxUDDot )
                {
                    // UF-edge
                    kClosest.x = kTest.x;
                    kClosest.y = fUMax;
                    kClosest.z = fDMax;
                }
                else if ( fUDDot >= fMinUDDot )
                {
                    // U-face
                    fUDot = fDMin*kTest.y - fUMin*kTest.z;
                    fT = fUDot/fMinUDDot;
                    kClosest.x = kTest.x;
                    kClosest.y = kTest.y - fT*fDMin;
                    kClosest.z = kTest.z + fT*fUMin;
                }
                else
                {
                    // UN-edge
                    kClosest.x = kTest.x;
                    kClosest.y = fUMin;
                    kClosest.z = fDMin;
                }
            }
        }
        else
        {
            if ( kTest.y <= fUMin )
            {
                fLDDot = fLMin*kTest.x + fDMin*kTest.z;
                if ( fLDDot >= fMaxLDDot )
                {
                    // LF-edge
                    kClosest.x = fLMax;
                    kClosest.y = kTest.y;
                    kClosest.z = fDMax;
                }
                else if ( fLDDot >= fMinLDDot )
                {
                    // L-face
                    fLDot = fDMin*kTest.x - fLMin*kTest.z;
                    fT = fLDot/fMinLDDot;
                    kClosest.x = kTest.x - fT*fDMin;
                    kClosest.y = kTest.y;
                    kClosest.z = kTest.z + fT*fLMin;
                }
                else
                {
                    // LN-edge
                    kClosest.x = fLMin;
                    kClosest.y = kTest.y;
                    kClosest.z = fDMin;
                }
            }
            else
            {
                fLUDDot = fLMin*kTest.x + fUMin*kTest.y + fDMin*kTest.z;
                fLEdgeDot = fUMin*fLUDDot - fMinLUDDot*kTest.y;
                if ( fLEdgeDot >= 0.0f )
                {
                    fLDDot = fLMin*kTest.x + fDMin*kTest.z;
                    if ( fLDDot >= fMaxLDDot )
                    {
                        // LF-edge
                        kClosest.x = fLMax;
                        kClosest.y = kTest.y;
                        kClosest.z = fDMax;
                    }
                    else if ( fLDDot >= fMinLDDot )
                    {
                        // L-face
                        fLDot = fDMin*kTest.x - fLMin*kTest.z;
                        fT = fLDot/fMinLDDot;
                        kClosest.x = kTest.x - fT*fDMin;
                        kClosest.y = kTest.y;
                        kClosest.z = kTest.z + fT*fLMin;
                    }
                    else
                    {
                        // LN-edge
                        kClosest.x = fLMin;
                        kClosest.y = kTest.y;
                        kClosest.z = fDMin;
                    }
                }
                else
                {
                    fUEdgeDot = fLMin*fLUDDot - fMinLUDDot*kTest.x;
                    if ( fUEdgeDot >= 0.0f )
                    {
                        fUDDot = fUMin*kTest.y + fDMin*kTest.z;
                        if ( fUDDot >= fMaxUDDot )
                        {
                            // UF-edge
                            kClosest.x = kTest.x;
                            kClosest.y = fUMax;
                            kClosest.z = fDMax;
                        }
                        else if ( fUDDot >= fMinUDDot )
                        {
                            // U-face
                            fUDot = fDMin*kTest.y - fUMin*kTest.z;
                            fT = fUDot/fMinUDDot;
                            kClosest.x = kTest.x;
                            kClosest.y = kTest.y - fT*fDMin;
                            kClosest.z = kTest.z + fT*fUMin;
                        }
                        else
                        {
                            // UN-edge
                            kClosest.x = kTest.x;
                            kClosest.y = fUMin;
                            kClosest.z = fDMin;
                        }
                    }
                    else
                    {
                        if ( fLUDDot >= fMaxLUDDot )
                        {
                            // LUF-vertex
                            kClosest.x = fLMax;
                            kClosest.y = fUMax;
                            kClosest.z = fDMax;
                        }
                        else if ( fLUDDot >= fMinLUDDot )
                        {
                            // LU-edge
                            fT = fLUDDot/fMinLUDDot;
                            kClosest.x = fT*fLMin;
                            kClosest.y = fT*fUMin;
                            kClosest.z = fT*fDMin;
                        }
                        else
                        {
                            // LUN-vertex
                            kClosest.x = fLMin;
                            kClosest.y = fUMin;
                            kClosest.z = fDMin;
                        }
                    }
                }
            }
        }
    }
    else
    {
        fLDot = fDMin*kTest.x - fLMin*kTest.z;
        fUDot = fDMin*kTest.y - fUMin*kTest.z;
        if ( fLDot <= 0.0f )
        {
            if ( fUDot <= 0.0f )
            {
                // point inside frustum
                kClosest = kTest;
            }
            else
            {
                fUDDot = fUMin*kTest.y + fDMin*kTest.z;
                if ( fUDDot >= fMaxUDDot )
                {
                    // UF-edge
                    kClosest.x = kTest.x;
                    kClosest.y = fUMax;
                    kClosest.z = fDMax;
                }
                else
                {
                    // U-face
                    fUDot = fDMin*kTest.y - fUMin*kTest.z;
                    fT = fUDot/fMinUDDot;
                    kClosest.x = kTest.x;
                    kClosest.y = kTest.y - fT*fDMin;
                    kClosest.z = kTest.z + fT*fUMin;
                }
            }
        }
        else
        {
            if ( fUDot <= 0.0f )
            {
                fLDDot = fLMin*kTest.x + fDMin*kTest.z;
                if ( fLDDot >= fMaxLDDot )
                {
                    // LF-edge
                    kClosest.x = fLMax;
                    kClosest.y = kTest.y;
                    kClosest.z = fDMax;
                }
                else
                {
                    // L-face
                    fLDot = fDMin*kTest.x - fLMin*kTest.z;
                    fT = fLDot/fMinLDDot;
                    kClosest.x = kTest.x - fT*fDMin;
                    kClosest.y = kTest.y;
                    kClosest.z = kTest.z + fT*fLMin;
                }
            }
            else
            {
                fLUDDot = fLMin*kTest.x + fUMin*kTest.y + fDMin*kTest.z;
                fLEdgeDot = fUMin*fLUDDot - fMinLUDDot*kTest.y;
                if ( fLEdgeDot >= 0.0f )
                {
                    fLDDot = fLMin*kTest.x + fDMin*kTest.z;
                    if ( fLDDot >= fMaxLDDot )
                    {
                        // LF-edge
                        kClosest.x = fLMax;
                        kClosest.y = kTest.y;
                        kClosest.z = fDMax;
                    }
                    else // assert( fLDDot >= fMinLDDot ) from geometry
                    {
                        // L-face
                        fLDot = fDMin*kTest.x - fLMin*kTest.z;
                        fT = fLDot/fMinLDDot;
                        kClosest.x = kTest.x - fT*fDMin;
                        kClosest.y = kTest.y;
                        kClosest.z = kTest.z + fT*fLMin;
                    }
                }
                else
                {
                    fUEdgeDot = fLMin*fLUDDot - fMinLUDDot*kTest.x;
                    if ( fUEdgeDot >= 0.0f )
                    {
                        fUDDot = fUMin*kTest.y + fDMin*kTest.z;
                        if ( fUDDot >= fMaxUDDot )
                        {
                            // UF-edge
                            kClosest.x = kTest.x;
                            kClosest.y = fUMax;
                            kClosest.z = fDMax;
                        }
                        else // assert( fUDDot >= fMinUDDot ) from geometry
                        {
                            // U-face
                            fUDot = fDMin*kTest.y - fUMin*kTest.z;
                            fT = fUDot/fMinUDDot;
                            kClosest.x = kTest.x;
                            kClosest.y = kTest.y - fT*fDMin;
                            kClosest.z = kTest.z + fT*fUMin;
                        }
                    }
                    else
                    {
                        if ( fLUDDot >= fMaxLUDDot )
                        {
                            // LUF-vertex
                            kClosest.x = fLMax;
                            kClosest.y = fUMax;
                            kClosest.z = fDMax;
                        }
                        else // assert( fLUDDot >= fMinLUDDot ) from geometry
                        {
                            // LU-edge
                            fT = fLUDDot/fMinLUDDot;
                            kClosest.x = fT*fLMin;
                            kClosest.y = fT*fUMin;
                            kClosest.z = fT*fDMin;
                        }
                    }
                }
            }
        }
    }

    kDiff = kTest - kClosest;

    // convert back to original quadrant
    if ( bLSignChange )
        kClosest.x = -kClosest.x;

    if ( bUSignChange )
        kClosest.y = -kClosest.y;

    if ( pkClosest )
    {
        // caller wants closest point, convert back to world coordinates
        *pkClosest = rkFrustum.Origin() + kClosest.x*rkFrustum.LVector() +
            kClosest.y*rkFrustum.UVector() + kClosest.z*rkFrustum.DVector();
    }

    // compute and return squared distance
    return kDiff.SquaredLength();
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Vector3& rkPoint, const Frustum& rkFrustum,
    Vector3* pkClosest)
{
    return Math::Sqrt(SqrDistance(rkPoint,rkFrustum,pkClosest));
}
//----------------------------------------------------------------------------


