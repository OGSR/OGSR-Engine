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

#include "MgcIntr3DTetrTetr.h"
using namespace Mgc;
using namespace std;

//----------------------------------------------------------------------------
static void SplitAndDecompose (Tetrahedron kTetra, const Plane& rkPlane,
    vector<Tetrahedron>& rkInside)
{
    // determine on which side of the plane the points of the tetrahedron lie
    Real afC[4];
    int i, aiP[4], aiN[4], aiZ[4];
    int iPositive = 0, iNegative = 0, iZero = 0;

    for (i = 0; i < 4; i++)
    {
        afC[i] = rkPlane.DistanceTo(kTetra[i]);
        if ( afC[i] > 0.0f )
            aiP[iPositive++] = i;
        else if ( afC[i] < 0.0f )
            aiN[iNegative++] = i;
        else
            aiZ[iZero++] = i;
    }

    // For a split to occur, one of the c_i must be positive and one must
    // be negative.

    if ( iNegative == 0 )
    {
        // tetrahedron is completely on the positive side of plane, full clip
        return;
    }

    if ( iPositive == 0 )
    {
        // tetrahedron is completely on the negative side of plane
        rkInside.push_back(kTetra);
        return;
    }

    // Tetrahedron is split by plane.  Determine how it is split and how to
    // decompose the negative-side portion into tetrahedra (6 cases).
    Real fW0, fW1, fInvCDiff;
    Vector3 akIntp[4];

    if ( iPositive == 3 )
    {
        // +++-
        for (i = 0; i < iPositive; i++)
        {
            fInvCDiff = 1.0f/(afC[aiP[i]] - afC[aiN[0]]);
            fW0 = -afC[aiN[0]]*fInvCDiff;
            fW1 = +afC[aiP[i]]*fInvCDiff;
            kTetra[aiP[i]] = fW0*kTetra[aiP[i]] + fW1*kTetra[aiN[0]];
        }
        rkInside.push_back(kTetra);
    }
    else if ( iPositive == 2 )
    {
        if ( iNegative == 2 )
        {
            // ++--
            for (i = 0; i < iPositive; i++)
            {
                fInvCDiff = 1.0f/(afC[aiP[i]]-afC[aiN[0]]);
                fW0 = -afC[aiN[0]]*fInvCDiff;
                fW1 = +afC[aiP[i]]*fInvCDiff;
                akIntp[i] = fW0*kTetra[aiP[i]] + fW1*kTetra[aiN[0]];
            }
            for (i = 0; i < iNegative; i++)
            {
                fInvCDiff = 1.0f/(afC[aiP[i]]-afC[aiN[1]]);
                fW0 = -afC[aiN[1]]*fInvCDiff;
                fW1 = +afC[aiP[i]]*fInvCDiff;
                akIntp[i+2] = fW0*kTetra[aiP[i]] + fW1*kTetra[aiN[1]];
            }

            kTetra[aiP[0]] = akIntp[2];
            kTetra[aiP[1]] = akIntp[1];
            rkInside.push_back(kTetra);

            rkInside.push_back(Tetrahedron(kTetra[aiN[1]],akIntp[3],akIntp[2],
                akIntp[1]));

            rkInside.push_back(Tetrahedron(kTetra[aiN[0]],akIntp[0],akIntp[1],
                akIntp[2]));
        }
        else
        {
            // ++-0
            for (i = 0; i < iPositive; i++)
            {
                fInvCDiff = 1.0f/(afC[aiP[i]]-afC[aiN[0]]);
                fW0 = -afC[aiN[0]]*fInvCDiff;
                fW1 = +afC[aiP[i]]*fInvCDiff;
                kTetra[aiP[i]] = fW0*kTetra[aiP[i]] + fW1*kTetra[aiN[0]];
            }
            rkInside.push_back(kTetra);
        }
    }
    else if ( iPositive == 1 )
    {
        if ( iNegative == 3 )
        {
            // +---
            for (i = 0; i < iNegative; i++)
            {
                fInvCDiff = 1.0f/(afC[aiP[0]]-afC[aiN[i]]);
                fW0 = -afC[aiN[i]]*fInvCDiff;
                fW1 = +afC[aiP[0]]*fInvCDiff;
                akIntp[i] = fW0*kTetra[aiP[0]] + fW1*kTetra[aiN[i]];
            }

            kTetra[aiP[0]] = akIntp[0];
            rkInside.push_back(kTetra);

            rkInside.push_back(Tetrahedron(akIntp[0],kTetra[aiN[1]],
                kTetra[aiN[2]],akIntp[1]));

            rkInside.push_back(Tetrahedron(kTetra[aiN[2]],akIntp[1],akIntp[2],
                akIntp[0]));
        }
        else if ( iNegative == 2 )
        {
            // +--0
            for (i = 0; i < iNegative; i++)
            {
                fInvCDiff = 1.0f/(afC[aiP[0]]-afC[aiN[i]]);
                fW0 = -afC[aiN[i]]*fInvCDiff;
                fW1 = +afC[aiP[0]]*fInvCDiff;
                akIntp[i] = fW0*kTetra[aiP[0]] + fW1*kTetra[aiN[i]];
            }

            kTetra[aiP[0]] = akIntp[0];
            rkInside.push_back(kTetra);

            rkInside.push_back(Tetrahedron(akIntp[1],kTetra[aiZ[0]],
                kTetra[aiN[1]],akIntp[0]));
        }
        else
        {
            // +-00
            fInvCDiff = 1.0f/(afC[aiP[0]]-afC[aiN[0]]);
            fW0 = -afC[aiN[0]]*fInvCDiff;
            fW1 = +afC[aiP[0]]*fInvCDiff;
            kTetra[aiP[0]] = fW0*kTetra[aiP[0]] + fW1*kTetra[aiN[0]];
            rkInside.push_back(kTetra);
        }
    }
}
//----------------------------------------------------------------------------
void Mgc::FindIntersection (const Tetrahedron& rkT0, const Tetrahedron& rkT1,
    vector<Tetrahedron>& rkIntr)
{
    // build planar faces of T0
    Plane akPlane[4];
    rkT0.GetPlanes(akPlane);

    // initial object to clip is T1
    rkIntr.clear();
    rkIntr.push_back(rkT1);

    // clip T1 against planes of T0
    for (int iP = 0; iP < 4; iP++)
    {
        vector<Tetrahedron> kInside;
        for (int iT = 0; iT < (int)rkIntr.size(); iT++)
            SplitAndDecompose(rkIntr[iT],akPlane[iP],kInside);
        rkIntr = kInside;
    }
}
//----------------------------------------------------------------------------

#if 0

// Test program.  The first example illustrates when the minimum number of
// tetrahedra in an intersection (1).  The second example illustrates the
// maximum number of tetrahedra in an intersection (19).

#include <fstream>
int main ()
{
    vector<Tetrahedron> kIntr;
    Tetrahedron kT0, kT1;

    kT0[0] = Vector3(0.0f,0.0f,0.0f);
    kT0[1] = Vector3(1.0f,0.0f,0.0f);
    kT0[2] = Vector3(0.0f,1.0f,0.0f);
    kT0[3] = Vector3(0.0f,0.0f,1.0f);

    kT1[0] = Vector3(0.0f,0.0f,0.0f);
    kT1[1] = Vector3(1.0f,1.0f,0.0f);
    kT1[2] = Vector3(0.0f,1.0f,1.0f);
    kT1[3] = Vector3(1.0f,0.0f,1.0f);

    FindIntersection(kT0,kT1,kIntr);

    // kIntr[0]
    // (0.0,0.0,0.0)
    // (0.5,0.5,0.0)
    // (0.0,0.5,0.5)
    // (0.5,0.0,0.5)

    kT1[0] = Vector3(0.4f,0.4f,0.4f);
    kT1[1] = Vector3(-0.1f,0.25f,0.25f);
    kT1[2] = Vector3(0.25f,-0.1f,0.25f);
    kT1[3] = Vector3(0.25f,0.25f,-0.1f);

    FindIntersection(kT0,kT1,kIntr);

    // kIntr[0]
    // (0.275000,0.362500,0.362500)
    // (0.000000,0.280000,0.280000)
    // (0.280000,0.000000,0.280000)
    // (0.280000,0.280000,0.000000)
    //
    // kIntr[1]
    // (0.275000,0.362500,0.362500)
    // (0.280000,0.000000,0.280000)
    // (0.280000,0.280000,0.000000)
    // (0.362500,0.275000,0.362500)
    //
    // kIntr[2]
    // (0.280000,0.280000,0.000000)
    // (0.362500,0.275000,0.362500)
    // (0.362500,0.362500,0.275000)
    // (0.275000,0.362500,0.362500)
    //
    // kIntr[3]
    // (0.000000,0.280000,0.280000)
    // (0.280000,0.000000,0.280000)
    // (0.280000,0.280000,0.000000)
    // (0.000000,0.184211,0.257895)
    //
    // kIntr[4]
    // (0.280000,0.280000,0.000000)
    // (0.000000,0.184211,0.257895)
    // (0.000000,0.257895,0.184211)
    // (0.000000,0.280000,0.280000)
    //
    // kIntr[5]
    // (0.280000,0.000000,0.280000)
    // (0.000000,0.184211,0.257895)
    // (0.280000,0.280000,0.000000)
    // (0.150000,0.000000,0.250000)
    //
    // kIntr[6]
    // (0.000000,0.184211,0.257895)
    // (0.280000,0.280000,0.000000)
    // (0.150000,0.000000,0.250000)
    // (0.000000,0.257895,0.184211)
    //
    // kIntr[7]
    // (0.150000,0.000000,0.250000)
    // (0.000000,0.257895,0.184211)
    // (0.000000,0.150000,0.250000)
    // (0.000000,0.184211,0.257895)
    //
    // kIntr[8]
    // (0.280000,0.280000,0.000000)
    // (0.150000,0.000000,0.250000)
    // (0.257895,0.000000,0.184211)
    // (0.280000,0.000000,0.280000)
    //
    // kIntr[9]
    // (0.280000,0.280000,0.000000)
    // (0.000000,0.257895,0.184211)
    // (0.257895,0.000000,0.184211)
    // (0.150000,0.250000,0.000000)
    //
    // kIntr[10]
    // (0.000000,0.257895,0.184211)
    // (0.257895,0.000000,0.184211)
    // (0.150000,0.250000,0.000000)
    // (0.000000,0.180147,0.231618)
    //
    // kIntr[11]
    // (0.150000,0.250000,0.000000)
    // (0.000000,0.180147,0.231618)
    // (0.000000,0.250000,0.150000)
    // (0.000000,0.257895,0.184211)
    //
    // kIntr[12]
    // (0.257895,0.000000,0.184211)
    // (0.000000,0.180147,0.231618)
    // (0.150000,0.250000,0.000000)
    // (0.150000,0.000000,0.250000)
    //
    // kIntr[13]
    // (0.000000,0.180147,0.231618)
    // (0.150000,0.250000,0.000000)
    // (0.150000,0.000000,0.250000)
    // (0.000000,0.250000,0.150000)
    //
    // kIntr[14]
    // (0.150000,0.000000,0.250000)
    // (0.000000,0.250000,0.150000)
    // (0.000000,0.150000,0.250000)
    // (0.000000,0.180147,0.231618)
    //
    // kIntr[15]
    // (0.150000,0.250000,0.000000)
    // (0.150000,0.000000,0.250000)
    // (0.221429,0.000000,0.178571)
    // (0.257895,0.000000,0.184211)
    //
    // kIntr[16]
    // (0.221429,0.000000,0.178571)
    // (0.150000,0.250000,0.000000)
    // (0.250000,0.150000,0.000000)
    // (0.280000,0.280000,0.000000)
    //
    // kIntr[17]
    // (0.221429,0.000000,0.178571)
    // (0.250000,0.150000,0.000000)
    // (0.280000,0.280000,0.000000)
    // (0.250000,0.000000,0.150000)
    //
    // kIntr[18]
    // (0.280000,0.280000,0.000000)
    // (0.250000,0.000000,0.150000)
    // (0.257895,0.000000,0.184211)
    // (0.221429,0.000000,0.178571)

    return 0;
}

#endif
