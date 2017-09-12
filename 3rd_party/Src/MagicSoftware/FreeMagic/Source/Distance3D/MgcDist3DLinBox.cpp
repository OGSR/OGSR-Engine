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

#include "MgcDist3DLinBox.h"
#include "MgcDist3DVecBox.h"
using namespace Mgc;

//----------------------------------------------------------------------------
static void Face (int i0, int i1, int i2, Vector3& rkPnt,
    const Vector3& rkDir, const Box3& rkBox, const Vector3& rkPmE,
    Real* pfLParam, Real& rfSqrDistance)
{
    Vector3 kPpE;
    Real fLSqr, fInv, fTmp, fParam, fT, fDelta;

    kPpE[i1] = rkPnt[i1] + rkBox.Extent(i1);
    kPpE[i2] = rkPnt[i2] + rkBox.Extent(i2);
    if ( rkDir[i0]*kPpE[i1] >= rkDir[i1]*rkPmE[i0] )
    {
        if ( rkDir[i0]*kPpE[i2] >= rkDir[i2]*rkPmE[i0] )
        {
            // v[i1] >= -e[i1], v[i2] >= -e[i2] (distance = 0)
            if ( pfLParam )
            {
                rkPnt[i0] = rkBox.Extent(i0);
                fInv = 1.0f/rkDir[i0];
                rkPnt[i1] -= rkDir[i1]*rkPmE[i0]*fInv;
                rkPnt[i2] -= rkDir[i2]*rkPmE[i0]*fInv;
                *pfLParam = -rkPmE[i0]*fInv;
            }
        }
        else
        {
            // v[i1] >= -e[i1], v[i2] < -e[i2]
            fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i2]*rkDir[i2];
            fTmp = fLSqr*kPpE[i1] - rkDir[i1]*(rkDir[i0]*rkPmE[i0] +
                rkDir[i2]*kPpE[i2]);
            if ( fTmp <= 2.0f*fLSqr*rkBox.Extent(i1) )
            {
                fT = fTmp/fLSqr;
                fLSqr += rkDir[i1]*rkDir[i1];
                fTmp = kPpE[i1] - fT;
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*fTmp +
                    rkDir[i2]*kPpE[i2];
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + fTmp*fTmp +
                    kPpE[i2]*kPpE[i2] + fDelta*fParam;

                if ( pfLParam )
                {
                    *pfLParam = fParam;
                    rkPnt[i0] = rkBox.Extent(i0);
                    rkPnt[i1] = fT - rkBox.Extent(i1);
                    rkPnt[i2] = -rkBox.Extent(i2);
                }
            }
            else
            {
                fLSqr += rkDir[i1]*rkDir[i1];
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*rkPmE[i1] +
                    rkDir[i2]*kPpE[i2];
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + rkPmE[i1]*rkPmE[i1] +
                    kPpE[i2]*kPpE[i2] + fDelta*fParam;

                if ( pfLParam )
                {
                    *pfLParam = fParam;
                    rkPnt[i0] = rkBox.Extent(i0);
                    rkPnt[i1] = rkBox.Extent(i1);
                    rkPnt[i2] = -rkBox.Extent(i2);
                }
            }
        }
    }
    else
    {
        if ( rkDir[i0]*kPpE[i2] >= rkDir[i2]*rkPmE[i0] )
        {
            // v[i1] < -e[i1], v[i2] >= -e[i2]
            fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1];
            fTmp = fLSqr*kPpE[i2] - rkDir[i2]*(rkDir[i0]*rkPmE[i0] +
                rkDir[i1]*kPpE[i1]);
            if ( fTmp <= 2.0f*fLSqr*rkBox.Extent(i2) )
            {
                fT = fTmp/fLSqr;
                fLSqr += rkDir[i2]*rkDir[i2];
                fTmp = kPpE[i2] - fT;
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                    rkDir[i2]*fTmp;
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                    fTmp*fTmp + fDelta*fParam;

                if ( pfLParam )
                {
                    *pfLParam = fParam;
                    rkPnt[i0] = rkBox.Extent(i0);
                    rkPnt[i1] = -rkBox.Extent(i1);
                    rkPnt[i2] = fT - rkBox.Extent(i2);
                }
            }
            else
            {
                fLSqr += rkDir[i2]*rkDir[i2];
                fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                    rkDir[i2]*rkPmE[i2];
                fParam = -fDelta/fLSqr;
                rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                    rkPmE[i2]*rkPmE[i2] + fDelta*fParam;

                if ( pfLParam )
                {
                    *pfLParam = fParam;
                    rkPnt[i0] = rkBox.Extent(i0);
                    rkPnt[i1] = -rkBox.Extent(i1);
                    rkPnt[i2] = rkBox.Extent(i2);
                }
            }
        }
        else
        {
            // v[i1] < -e[i1], v[i2] < -e[i2]
            fLSqr = rkDir[i0]*rkDir[i0]+rkDir[i2]*rkDir[i2];
            fTmp = fLSqr*kPpE[i1] - rkDir[i1]*(rkDir[i0]*rkPmE[i0] +
                rkDir[i2]*kPpE[i2]);
            if ( fTmp >= 0.0f )
            {
                // v[i1]-edge is closest
                if ( fTmp <= 2.0f*fLSqr*rkBox.Extent(i1) )
                {
                    fT = fTmp/fLSqr;
                    fLSqr += rkDir[i1]*rkDir[i1];
                    fTmp = kPpE[i1] - fT;
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*fTmp +
                        rkDir[i2]*kPpE[i2];
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + fTmp*fTmp +
                        kPpE[i2]*kPpE[i2] + fDelta*fParam;

                    if ( pfLParam )
                    {
                        *pfLParam = fParam;
                        rkPnt[i0] = rkBox.Extent(i0);
                        rkPnt[i1] = fT - rkBox.Extent(i1);
                        rkPnt[i2] = -rkBox.Extent(i2);
                    }
                }
                else
                {
                    fLSqr += rkDir[i1]*rkDir[i1];
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*rkPmE[i1] +
                        rkDir[i2]*kPpE[i2];
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + rkPmE[i1]*rkPmE[i1]
                        + kPpE[i2]*kPpE[i2] + fDelta*fParam;

                    if ( pfLParam )
                    {
                        *pfLParam = fParam;
                        rkPnt[i0] = rkBox.Extent(i0);
                        rkPnt[i1] = rkBox.Extent(i1);
                        rkPnt[i2] = -rkBox.Extent(i2);
                    }
                }
                return;
            }

            fLSqr = rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1];
            fTmp = fLSqr*kPpE[i2] - rkDir[i2]*(rkDir[i0]*rkPmE[i0] +
                rkDir[i1]*kPpE[i1]);
            if ( fTmp >= 0.0f )
            {
                // v[i2]-edge is closest
                if ( fTmp <= 2.0f*fLSqr*rkBox.Extent(i2) )
                {
                    fT = fTmp/fLSqr;
                    fLSqr += rkDir[i2]*rkDir[i2];
                    fTmp = kPpE[i2] - fT;
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                        rkDir[i2]*fTmp;
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                        fTmp*fTmp + fDelta*fParam;

                    if ( pfLParam )
                    {
                        *pfLParam = fParam;
                        rkPnt[i0] = rkBox.Extent(i0);
                        rkPnt[i1] = -rkBox.Extent(i1);
                        rkPnt[i2] = fT - rkBox.Extent(i2);
                    }
                }
                else
                {
                    fLSqr += rkDir[i2]*rkDir[i2];
                    fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                        rkDir[i2]*rkPmE[i2];
                    fParam = -fDelta/fLSqr;
                    rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                        rkPmE[i2]*rkPmE[i2] + fDelta*fParam;

                    if ( pfLParam )
                    {
                        *pfLParam = fParam;
                        rkPnt[i0] = rkBox.Extent(i0);
                        rkPnt[i1] = -rkBox.Extent(i1);
                        rkPnt[i2] = rkBox.Extent(i2);
                    }
                }
                return;
            }

            // (v[i1],v[i2])-corner is closest
            fLSqr += rkDir[i2]*rkDir[i2];
            fDelta = rkDir[i0]*rkPmE[i0] + rkDir[i1]*kPpE[i1] +
                rkDir[i2]*kPpE[i2];
            fParam = -fDelta/fLSqr;
            rfSqrDistance += rkPmE[i0]*rkPmE[i0] + kPpE[i1]*kPpE[i1] +
                kPpE[i2]*kPpE[i2] + fDelta*fParam;

            if ( pfLParam )
            {
                *pfLParam = fParam;
                rkPnt[i0] = rkBox.Extent(i0);
                rkPnt[i1] = -rkBox.Extent(i1);
                rkPnt[i2] = -rkBox.Extent(i2);
            }
        }
    }
}
//----------------------------------------------------------------------------
static void CaseNoZeros (Vector3& rkPnt, const Vector3& rkDir,
    const Box3& rkBox, Real* pfLParam, Real& rfSqrDistance)
{
    Vector3 kPmE(rkPnt.x - rkBox.Extent(0), rkPnt.y - rkBox.Extent(1),
        rkPnt.z - rkBox.Extent(2));

    Real fProdDxPy, fProdDyPx, fProdDzPx, fProdDxPz, fProdDzPy, fProdDyPz;

    fProdDxPy = rkDir.x*kPmE.y;
    fProdDyPx = rkDir.y*kPmE.x;
    if ( fProdDyPx >= fProdDxPy )
    {
        fProdDzPx = rkDir.z*kPmE.x;
        fProdDxPz = rkDir.x*kPmE.z;
        if ( fProdDzPx >= fProdDxPz )
        {
            // line intersects x = e0
            Face(0,1,2,rkPnt,rkDir,rkBox,kPmE,pfLParam,rfSqrDistance);
        }
        else
        {
            // line intersects z = e2
            Face(2,0,1,rkPnt,rkDir,rkBox,kPmE,pfLParam,rfSqrDistance);
        }
    }
    else
    {
        fProdDzPy = rkDir.z*kPmE.y;
        fProdDyPz = rkDir.y*kPmE.z;
        if ( fProdDzPy >= fProdDyPz )
        {
            // line intersects y = e1
            Face(1,2,0,rkPnt,rkDir,rkBox,kPmE,pfLParam,rfSqrDistance);
        }
        else
        {
            // line intersects z = e2
            Face(2,0,1,rkPnt,rkDir,rkBox,kPmE,pfLParam,rfSqrDistance);
        }
    }
}
//----------------------------------------------------------------------------
static void Case0 (int i0, int i1, int i2, Vector3& rkPnt,
    const Vector3& rkDir, const Box3& rkBox, Real* pfLParam,
    Real& rfSqrDistance)
{
    Real fPmE0 = rkPnt[i0] - rkBox.Extent(i0);
    Real fPmE1 = rkPnt[i1] - rkBox.Extent(i1);
    Real fProd0 = rkDir[i1]*fPmE0;
    Real fProd1 = rkDir[i0]*fPmE1;
    Real fDelta, fInvLSqr, fInv;

    if ( fProd0 >= fProd1 )
    {
        // line intersects P[i0] = e[i0]
        rkPnt[i0] = rkBox.Extent(i0);

        Real fPpE1 = rkPnt[i1] + rkBox.Extent(i1);
        fDelta = fProd0 - rkDir[i0]*fPpE1;
        if ( fDelta >= 0.0f )
        {
            fInvLSqr = 1.0f/(rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1]);
            rfSqrDistance += fDelta*fDelta*fInvLSqr;
            if ( pfLParam )
            {
                rkPnt[i1] = -rkBox.Extent(i1);
                *pfLParam = -(rkDir[i0]*fPmE0+rkDir[i1]*fPpE1)*fInvLSqr;
            }
        }
        else
        {
            if ( pfLParam )
            {
                fInv = 1.0f/rkDir[i0];
                rkPnt[i1] -= fProd0*fInv;
                *pfLParam = -fPmE0*fInv;
            }
        }
    }
    else
    {
        // line intersects P[i1] = e[i1]
        rkPnt[i1] = rkBox.Extent(i1);

        Real fPpE0 = rkPnt[i0] + rkBox.Extent(i0);
        fDelta = fProd1 - rkDir[i1]*fPpE0;
        if ( fDelta >= 0.0f )
        {
            fInvLSqr = 1.0f/(rkDir[i0]*rkDir[i0] + rkDir[i1]*rkDir[i1]);
            rfSqrDistance += fDelta*fDelta*fInvLSqr;
            if ( pfLParam )
            {
                rkPnt[i0] = -rkBox.Extent(i0);
                *pfLParam = -(rkDir[i0]*fPpE0+rkDir[i1]*fPmE1)*fInvLSqr;
            }
        }
        else
        {
            if ( pfLParam )
            {
                fInv = 1.0f/rkDir[i1];
                rkPnt[i0] -= fProd1*fInv;
                *pfLParam = -fPmE1*fInv;
            }
        }
    }

    if ( rkPnt[i2] < -rkBox.Extent(i2) )
    {
        fDelta = rkPnt[i2] + rkBox.Extent(i2);
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i2] = -rkBox.Extent(i2);
    }
    else if ( rkPnt[i2] > rkBox.Extent(i2) )
    {
        fDelta = rkPnt[i2] - rkBox.Extent(i2);
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i2] = rkBox.Extent(i2);
    }
}
//----------------------------------------------------------------------------
static void Case00 (int i0, int i1, int i2, Vector3& rkPnt,
    const Vector3& rkDir, const Box3& rkBox, Real* pfLParam,
    Real& rfSqrDistance)
{
    Real fDelta;

    if ( pfLParam )
        *pfLParam = (rkBox.Extent(i0) - rkPnt[i0])/rkDir[i0];

    rkPnt[i0] = rkBox.Extent(i0);

    if ( rkPnt[i1] < -rkBox.Extent(i1) )
    {
        fDelta = rkPnt[i1] + rkBox.Extent(i1);
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i1] = -rkBox.Extent(i1);
    }
    else if ( rkPnt[i1] > rkBox.Extent(i1) )
    {
        fDelta = rkPnt[i1] - rkBox.Extent(i1);
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i1] = rkBox.Extent(i1);
    }

    if ( rkPnt[i2] < -rkBox.Extent(i2) )
    {
        fDelta = rkPnt[i2] + rkBox.Extent(i2);
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i1] = -rkBox.Extent(i2);
    }
    else if ( rkPnt[i2] > rkBox.Extent(i2) )
    {
        fDelta = rkPnt[i2] - rkBox.Extent(i2);
        rfSqrDistance += fDelta*fDelta;
        rkPnt[i2] = rkBox.Extent(i2);
    }
}
//----------------------------------------------------------------------------
static void Case000 (Vector3& rkPnt, const Box3& rkBox,
    Real& rfSqrDistance)
{
    Real fDelta;

    if ( rkPnt.x < -rkBox.Extent(0) )
    {
        fDelta = rkPnt.x + rkBox.Extent(0);
        rfSqrDistance += fDelta*fDelta;
        rkPnt.x = -rkBox.Extent(0);
    }
    else if ( rkPnt.x > rkBox.Extent(0) )
    {
        fDelta = rkPnt.x - rkBox.Extent(0);
        rfSqrDistance += fDelta*fDelta;
        rkPnt.x = rkBox.Extent(0);
    }

    if ( rkPnt.y < -rkBox.Extent(1) )
    {
        fDelta = rkPnt.y + rkBox.Extent(1);
        rfSqrDistance += fDelta*fDelta;
        rkPnt.y = -rkBox.Extent(1);
    }
    else if ( rkPnt.y > rkBox.Extent(1) )
    {
        fDelta = rkPnt.y - rkBox.Extent(1);
        rfSqrDistance += fDelta*fDelta;
        rkPnt.y = rkBox.Extent(1);
    }

    if ( rkPnt.z < -rkBox.Extent(2) )
    {
        fDelta = rkPnt.z + rkBox.Extent(2);
        rfSqrDistance += fDelta*fDelta;
        rkPnt.z = -rkBox.Extent(2);
    }
    else if ( rkPnt.z > rkBox.Extent(2) )
    {
        fDelta = rkPnt.z - rkBox.Extent(2);
        rfSqrDistance += fDelta*fDelta;
        rkPnt.z = rkBox.Extent(2);
    }
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Line3& rkLine, const Box3& rkBox,
    Real* pfLParam, Real* pfBParam0, Real* pfBParam1, Real* pfBParam2)
{
#ifdef _DEBUG
    // The four parameters pointers are either all non-null or all null.
    if ( pfLParam )
    {
        assert( pfBParam0 && pfBParam1 && pfBParam2 );
    }
    else
    {
        assert( !pfBParam0 && !pfBParam1 && !pfBParam2 );
    }
#endif

    // compute coordinates of line in box coordinate system
    Vector3 kDiff = rkLine.Origin() - rkBox.Center();
    Vector3 kPnt(kDiff.Dot(rkBox.Axis(0)),kDiff.Dot(rkBox.Axis(1)),
        kDiff.Dot(rkBox.Axis(2)));
    Vector3 kDir(rkLine.Direction().Dot(rkBox.Axis(0)),
        rkLine.Direction().Dot(rkBox.Axis(1)),
        rkLine.Direction().Dot(rkBox.Axis(2)));

    // Apply reflections so that direction vector has nonnegative components.
    bool bReflect[3];
    int i;
    for (i = 0; i < 3; i++)
    {
        if ( kDir[i] < 0.0f )
        {
            kPnt[i] = -kPnt[i];
            kDir[i] = -kDir[i];
            bReflect[i] = true;
        }
        else
        {
            bReflect[i] = false;
        }
    }

    Real fSqrDistance = 0.0f;

    if ( kDir.x > 0.0f )
    {
        if ( kDir.y > 0.0f )
        {
            if ( kDir.z > 0.0f )
            {
                // (+,+,+)
                CaseNoZeros(kPnt,kDir,rkBox,pfLParam,fSqrDistance);
            }
            else
            {
                // (+,+,0)
                Case0(0,1,2,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
            }
        }
        else
        {
            if ( kDir.z > 0.0f )
            {
                // (+,0,+)
                Case0(0,2,1,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
            }
            else
            {
                // (+,0,0)
                Case00(0,1,2,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
            }
        }
    }
    else
    {
        if ( kDir.y > 0.0f )
        {
            if ( kDir.z > 0.0f )
            {
                // (0,+,+)
                Case0(1,2,0,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
            }
            else
            {
                // (0,+,0)
                Case00(1,0,2,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
            }
        }
        else
        {
            if ( kDir.z > 0.0f )
            {
                // (0,0,+)
                Case00(2,0,1,kPnt,kDir,rkBox,pfLParam,fSqrDistance);
            }
            else
            {
                // (0,0,0)
                Case000(kPnt,rkBox,fSqrDistance);
                if ( pfLParam )
                    *pfLParam = 0.0f;
            }
        }
    }

    if ( pfLParam )
    {
        // undo reflections
        for (i = 0; i < 3; i++)
        {
            if ( bReflect[i] )
                kPnt[i] = -kPnt[i];
        }

        *pfBParam0 = kPnt.x;
        *pfBParam1 = kPnt.y;
        *pfBParam2 = kPnt.z;
    }

    return fSqrDistance;
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Ray3& rkRay, const Box3& rkBox, Real* pfLParam,
    Real* pfBParam0, Real* pfBParam1, Real* pfBParam2)
{
#ifdef _DEBUG
    // The four parameters pointers are either all non-null or all null.
    if ( pfLParam )
    {
        assert( pfBParam0 && pfBParam1 && pfBParam2 );
    }
    else
    {
        assert( !pfBParam0 && !pfBParam1 && !pfBParam2 );
    }
#endif

    Line3 kLine;
    kLine.Origin() = rkRay.Origin();
    kLine.Direction() = rkRay.Direction();

    Real fLP, fBP0, fBP1, fBP2;
    Real fSqrDistance = SqrDistance(kLine,rkBox,&fLP,&fBP0,&fBP1,&fBP2);
    if ( fLP >= 0.0f )
    {
        if ( pfLParam )
        {
            *pfLParam = fLP;
            *pfBParam0 = fBP0;
            *pfBParam1 = fBP1;
            *pfBParam2 = fBP2;
        }

        return fSqrDistance;
    }
    else
    {
        fSqrDistance = SqrDistance(rkRay.Origin(),rkBox,pfBParam0,
            pfBParam1,pfBParam2);

        if ( pfLParam )
            *pfLParam = 0.0f;

        return fSqrDistance;
    }
}
//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Segment3& rkSeg, const Box3& rkBox,
    Real* pfLParam, Real* pfBParam0, Real* pfBParam1, Real* pfBParam2)
{
#ifdef _DEBUG
    // The four parameters pointers are either all non-null or all null.
    if ( pfLParam )
    {
        assert( pfBParam0 && pfBParam1 && pfBParam2 );
    }
    else
    {
        assert( !pfBParam0 && !pfBParam1 && !pfBParam2 );
    }
#endif

    Line3 kLine;
    kLine.Origin() = rkSeg.Origin();
    kLine.Direction() = rkSeg.Direction();

    Real fLP, fBP0, fBP1, fBP2;
    Real fSqrDistance = SqrDistance(kLine,rkBox,&fLP,&fBP0,&fBP1,&fBP2);
    if ( fLP >= 0.0f )
    {
        if ( fLP <= 1.0f )
        {
            if ( pfLParam )
            {
                *pfLParam = fLP;
                *pfBParam0 = fBP0;
                *pfBParam1 = fBP1;
                *pfBParam2 = fBP2;
            }

            return fSqrDistance;
        }
        else
        {
            fSqrDistance = SqrDistance(rkSeg.Origin()+rkSeg.Direction(),
                rkBox,pfBParam0,pfBParam1,pfBParam2);

            if ( pfLParam )
                *pfLParam = 1.0f;

            return fSqrDistance;
        }
    }
    else
    {
        fSqrDistance = SqrDistance(rkSeg.Origin(),rkBox,pfBParam0,
            pfBParam1,pfBParam2);

        if ( pfLParam )
            *pfLParam = 0.0f;

        return fSqrDistance;
    }
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Line3& rkLine, const Box3& rkBox, Real* pfLParam,
    Real* pfBParam0, Real* pfBParam1, Real* pfBParam2)
{
    return Math::Sqrt(SqrDistance(rkLine,rkBox,pfLParam,pfBParam0,pfBParam1,
        pfBParam2));
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Ray3& rkRay, const Box3& rkBox, Real* pfLParam,
    Real* pfBParam0, Real* pfBParam1, Real* pfBParam2)
{
    return Math::Sqrt(SqrDistance(rkRay,rkBox,pfLParam,pfBParam0,pfBParam1,
        pfBParam2));
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Segment3& rkSeg, const Box3& rkBox,
    Real* pfLParam, Real* pfBParam0, Real* pfBParam1, Real* pfBParam2)
{
    return Math::Sqrt(SqrDistance(rkSeg,rkBox,pfLParam,pfBParam0,pfBParam1,
        pfBParam2));
}
//----------------------------------------------------------------------------


