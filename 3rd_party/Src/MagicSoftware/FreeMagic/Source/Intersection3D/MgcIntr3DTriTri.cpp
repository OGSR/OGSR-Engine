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

#include "MgcIntr3DTriTri.h"
using namespace Mgc;

//----------------------------------------------------------------------------
static void Projection (const Vector3& rkD, const Vector3 akV[3],
    Real& rfMin, Real& rfMax)
{
    Real afDot[3] = { rkD.Dot(akV[0]), rkD.Dot(akV[1]), rkD.Dot(akV[2]) };

    rfMin = afDot[0];
    rfMax = rfMin;

    if ( afDot[1] < rfMin )
        rfMin = afDot[1];
    else if ( afDot[1] > rfMax )
        rfMax = afDot[1];

    if ( afDot[2] < rfMin )
        rfMin = afDot[2];
    else if ( afDot[2] > rfMax )
        rfMax = afDot[2];
}
//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Vector3 akU[3], const Vector3 akV[3])
{
    Vector3 kDir;
    Real fUMin, fUMax, fVMin, fVMax;
    int i0, i1;

    // direction N
    Vector3 akE[3] = { akU[1]-akU[0], akU[2]-akU[1], akU[0]-akU[2] };
    Vector3 kN = akE[0].Cross(akE[1]);
    Real fNdU0 = kN.Dot(akU[0]);
    Projection(kN,akV,fVMin,fVMax);
    if ( fNdU0 < fVMin || fNdU0 > fVMax )
        return false;

    // direction M
    Vector3 akF[3] = { akV[1]-akV[0], akV[2]-akV[1], akV[0]-akV[2] };
    Vector3 kM = akF[0].Cross(akF[1]);

    const Real fEpsilon = 1e-06f;  // cutoff: sin(Angle(N,M)) < epsilon
    Vector3 kNxM = kN.Cross(kM);
    if ( kNxM.Dot(kNxM) >= fEpsilon*(kN.Dot(kN)*kM.Dot(kM)) )
    {
        // triangles are not parallel
        Real fMdV0 = kM.Dot(akV[0]);
        Projection(kM,akU,fUMin,fUMax);
        if ( fMdV0 < fUMin || fMdV0 > fUMax )
            return false;

        // directions E[i0]xF[i1]
        for (i1 = 0; i1 < 3; i1++)
        {
            for (i0 = 0; i0 < 3; i0++)
            {
                kDir = akE[i0].Cross(akF[i1]);
                Projection(kDir,akU,fUMin,fUMax);
                Projection(kDir,akV,fVMin,fVMax);
                if ( fUMax < fVMin || fVMax < fUMin )
                    return false;
            }
        }
    }
    else  // triangles are parallel (and, in fact, coplanar)
    {
        // directions NxE[i0]
        for (i0 = 0; i0 < 3; i0++)
        {
            kDir = kN.Cross(akE[i0]);
            Projection(kDir,akU,fUMin,fUMax);
            Projection(kDir,akV,fVMin,fVMax);
            if ( fUMax < fVMin || fVMax < fUMin )
                return false;
        }

        // directions NxF[i1]
        for (i1 = 0; i1 < 3; i1++)
        {
            kDir = kM.Cross(akF[i1]);
            Projection(kDir,akU,fUMin,fUMax);
            Projection(kDir,akV,fVMin,fVMax);
            if ( fUMax < fVMin || fVMax < fUMin )
                return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------

