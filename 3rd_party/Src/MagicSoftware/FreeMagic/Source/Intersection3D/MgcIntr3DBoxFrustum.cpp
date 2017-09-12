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

#include "MgcIntr3DBoxFrustum.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Box3& rkBox, const Frustum& rkFrustum)
{
    // for convenience
    const Vector3* akA = rkBox.Axes();
    const Real* afE = rkBox.Extents();

    Vector3 kDiff = rkBox.Center() - rkFrustum.Origin();

    Real afA[3], afB[3], afC[3], afD[3];
    Real afNA[3], afNB[3], afNC[3], afND[3];
    Real afNApLC[3], afNAmLC[3], afNBpUC[3], afNBmUC[3];
    Real afLC[3], afLD[3], afUC[3], afUD[3], afLBpUA[3], afLBmUA[3];
    Real fDdD, fR, fP, fMin, fMax, fMTwoLF, fMTwoUF, fLB, fUA, fTmp;
    int i, j;

    // M = D
    afD[2] = kDiff.Dot(rkFrustum.DVector());
    for (i = 0; i < 3; i++)
        afC[i] = akA[i].Dot(rkFrustum.DVector());
    fR = afE[0]*Math::FAbs(afC[0]) +
         afE[1]*Math::FAbs(afC[1]) +
         afE[2]*Math::FAbs(afC[2]);
    if ( afD[2] + fR < rkFrustum.DMin() || afD[2] - fR > rkFrustum.DMax() )
        return false;

    // M = n*L - l*D
    for (i = 0; i < 3; i++)
    {
        afA[i] = akA[i].Dot(rkFrustum.LVector());
        afLC[i] = rkFrustum.LBound()*afC[i];
        afNA[i] = rkFrustum.DMin()*afA[i];
        afNAmLC[i] = afNA[i] - afLC[i];
    }
    afD[0] = kDiff.Dot(rkFrustum.LVector());
    fR = afE[0]*Math::FAbs(afNAmLC[0]) +
         afE[1]*Math::FAbs(afNAmLC[1]) +
         afE[2]*Math::FAbs(afNAmLC[2]);
    afND[0] = rkFrustum.DMin()*afD[0];
    afLD[2] = rkFrustum.LBound()*afD[2];
    fDdD = afND[0] - afLD[2];
    fMTwoLF = rkFrustum.GetMTwoLF();
    if ( fDdD + fR < fMTwoLF || fDdD > fR )
        return false;

    // M = -n*L - l*D
    for (i = 0; i < 3; i++)
        afNApLC[i] = afNA[i] + afLC[i];
    fR = afE[0]*Math::FAbs(afNApLC[0]) +
         afE[1]*Math::FAbs(afNApLC[1]) +
         afE[2]*Math::FAbs(afNApLC[2]);
    fDdD = -(afND[0] + afLD[2]);
    if ( fDdD + fR < fMTwoLF || fDdD > fR )
        return false;

    // M = n*U - u*D
    for (i = 0; i < 3; i++)
    {
        afB[i] = akA[i].Dot(rkFrustum.UVector());
        afUC[i] = rkFrustum.UBound()*afC[i];
        afNB[i] = rkFrustum.DMin()*afB[i];
        afNBmUC[i] = afNB[i] - afUC[i];
    }
    afD[1] = kDiff.Dot(rkFrustum.UVector());
    fR = afE[0]*Math::FAbs(afNBmUC[0]) +
         afE[1]*Math::FAbs(afNBmUC[1]) +
         afE[2]*Math::FAbs(afNBmUC[2]);
    afND[1] = rkFrustum.DMin()*afD[1];
    afUD[2] = rkFrustum.UBound()*afD[2];
    fDdD = afND[1] - afUD[2];
    fMTwoUF = rkFrustum.GetMTwoUF();
    if ( fDdD + fR < fMTwoUF || fDdD > fR )
        return false;

    // M = -n*U - u*D
    for (i = 0; i < 3; i++)
        afNBpUC[i] = afNB[i] + afUC[i];
    fR = afE[0]*Math::FAbs(afNBpUC[0]) +
         afE[1]*Math::FAbs(afNBpUC[1]) +
         afE[2]*Math::FAbs(afNBpUC[2]);
    fDdD = -(afND[1] + afUD[2]);
    if ( fDdD + fR < fMTwoUF || fDdD > fR )
        return false;

    // M = A[i]
    for (i = 0; i < 3; i++)
    {
        fP = rkFrustum.LBound()*Math::FAbs(afA[i]) +
             rkFrustum.UBound()*Math::FAbs(afB[i]);
        afNC[i] = rkFrustum.DMin()*afC[i];
        fMin = afNC[i] - fP;
        if ( fMin < 0.0f )
            fMin *= rkFrustum.GetDRatio();
        fMax = afNC[i] + fP;
        if ( fMax > 0.0f )
            fMax *= rkFrustum.GetDRatio();
        fDdD = afA[i]*afD[0] + afB[i]*afD[1] + afC[i]*afD[2];
        if ( fDdD + afE[i] < fMin || fDdD - afE[i] > fMax )
            return false;
    }

    // M = Cross(L,A[i])
    for (i = 0; i < 3; i++)
    {
        fP = rkFrustum.UBound()*Math::FAbs(afC[i]);
        fMin = afNB[i] - fP;
        if ( fMin < 0.0f )
            fMin *= rkFrustum.GetDRatio();
        fMax = afNB[i] + fP;
        if ( fMax > 0.0f )
            fMax *= rkFrustum.GetDRatio();
        fDdD = -afC[i]*afD[1] + afB[i]*afD[2];
        fR = afE[0]*Math::FAbs(afB[i]*afC[0]-afB[0]*afC[i]) +
             afE[1]*Math::FAbs(afB[i]*afC[1]-afB[1]*afC[i]) +
             afE[2]*Math::FAbs(afB[i]*afC[2]-afB[2]*afC[i]);
        if ( fDdD + fR < fMin || fDdD - fR > fMax )
            return false;
    }

    // M = Cross(U,A[i])
    for (i = 0; i < 3; i++)
    {
        fP = rkFrustum.LBound()*Math::FAbs(afC[i]);
        fMin = -afNA[i] - fP;
        if ( fMin < 0.0f )
            fMin *= rkFrustum.GetDRatio();
        fMax = -afNA[i] + fP;
        if ( fMax > 0.0f )
            fMax *= rkFrustum.GetDRatio();
        fDdD = afC[i]*afD[0] - afA[i]*afD[2];
        fR = afE[0]*Math::FAbs(afA[i]*afC[0]-afA[0]*afC[i]) +
             afE[1]*Math::FAbs(afA[i]*afC[1]-afA[1]*afC[i]) +
             afE[2]*Math::FAbs(afA[i]*afC[2]-afA[2]*afC[i]);
        if ( fDdD + fR < fMin || fDdD - fR > fMax )
            return false;
    }

    // M = Cross(n*D+l*L+u*U,A[i])
    for (i = 0; i < 3; i++)
    {
        fLB = rkFrustum.LBound()*afB[i];
        fUA = rkFrustum.UBound()*afA[i];
        afLBpUA[i] = fLB + fUA;
        afLBmUA[i] = fLB - fUA;
    }
    for (i = 0; i < 3; i++)
    {
        fP = rkFrustum.LBound()*Math::FAbs(afNBmUC[i]) +
             rkFrustum.UBound()*Math::FAbs(afNAmLC[i]);
        fTmp = rkFrustum.DMin()*afLBmUA[i];
        fMin = fTmp - fP;
        if ( fMin < 0.0f )
            fMin *= rkFrustum.GetDRatio();
        fMax = fTmp + fP;
        if ( fMax > 0.0f )
            fMax *= rkFrustum.GetDRatio();
        fDdD = -afD[0]*afNBmUC[i] + afD[1]*afNAmLC[i] + afD[2]*afLBmUA[i];
        fR = 0.0f;
        for (j = 0; j < 3; j++)
        {
            fR += afE[j]*Math::FAbs(-afA[j]*afNBmUC[i]+ afB[j]*afNAmLC[i]
                + afC[j]*afLBmUA[i]);
        }
        if ( fDdD + fR < fMin || fDdD - fR > fMax )
            return false;
    }

    // M = Cross(n*D+l*L-u*U,A[i])
    for (i = 0; i < 3; i++)
    {
        fP = rkFrustum.LBound()*Math::FAbs(afNBpUC[i]) +
             rkFrustum.UBound()*Math::FAbs(afNAmLC[i]);
        fTmp = rkFrustum.DMin()*afLBpUA[i];
        fMin = fTmp - fP;
        if ( fMin < 0.0f )
            fMin *= rkFrustum.GetDRatio();
        fMax = fTmp + fP;
        if ( fMax > 0.0f )
            fMax *= rkFrustum.GetDRatio();
        fDdD = -afD[0]*afNBpUC[i] + afD[1]*afNAmLC[i] + afD[2]*afLBpUA[i];
        fR = 0.0f;
        for (j = 0; j < 3; j++)
        {
            fR += afE[j]*Math::FAbs(-afA[j]*afNBpUC[i]+ afB[j]*afNAmLC[i]
                + afC[j]*afLBpUA[i]);
        }
        if ( fDdD + fR < fMin || fDdD - fR > fMax )
            return false;
    }

    // M = Cross(n*D-l*L+u*U,A[i])
    for (i = 0; i < 3; i++)
    {
        fP = rkFrustum.LBound()*Math::FAbs(afNBmUC[i]) +
             rkFrustum.UBound()*Math::FAbs(afNApLC[i]);
        fTmp = -rkFrustum.DMin()*afLBpUA[i];
        fMin = fTmp - fP;
        if ( fMin < 0.0f )
            fMin *= rkFrustum.GetDRatio();
        fMax = fTmp + fP;
        if ( fMax > 0.0f )
            fMax *= rkFrustum.GetDRatio();
        fDdD = -afD[0]*afNBmUC[i] + afD[1]*afNApLC[i] - afD[2]*afLBpUA[i];
        fR = 0.0f;
        for (j = 0; j < 3; j++)
        {
            fR += afE[j]*Math::FAbs(-afA[j]*afNBmUC[i]+ afB[j]*afNApLC[i]
                - afC[j]*afLBpUA[i]);
        }
        if ( fDdD + fR < fMin || fDdD - fR > fMax )
            return false;
    }

    // M = Cross(n*D-l*L-u*U,A[i])
    for (i = 0; i < 3; i++)
    {
        fP = rkFrustum.LBound()*Math::FAbs(afNBpUC[i]) +
             rkFrustum.UBound()*Math::FAbs(afNApLC[i]);
        fTmp = -rkFrustum.DMin()*afLBmUA[i];
        fMin = fTmp - fP;
        if ( fMin < 0.0f )
            fMin *= rkFrustum.GetDRatio();
        fMax = fTmp + fP;
        if ( fMax > 0.0f )
            fMax *= rkFrustum.GetDRatio();
        fDdD = -afD[0]*afNBpUC[i] + afD[1]*afNApLC[i] - afD[2]*afLBmUA[i];
        fR = 0.0f;
        for (j = 0; j < 3; j++)
        {
            fR += afE[j]*Math::FAbs(-afA[j]*afNBpUC[i]+ afB[j]*afNApLC[i]
                - afC[j]*afLBmUA[i]);
        }
        if ( fDdD + fR < fMin || fDdD - fR > fMax )
            return false;
    }

    return true;
}
//----------------------------------------------------------------------------


