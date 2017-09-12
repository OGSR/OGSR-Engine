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

#include "MgcAppr2DEllipseByArcs.h"
#include "MgcCont2DCircleScribe.h"

//----------------------------------------------------------------------------
void Mgc::ApproximateEllipseByArcs (Real fA, Real fB, int iNumArcs,
    Vector2*& rakPoint, Vector2*& rakCenter, Real*& rafRadius)
{
    // allocate arrays
    assert( iNumArcs >= 2 );
    if ( iNumArcs < 2 )
    {
        rakPoint = NULL;
        rakCenter = NULL;
        rafRadius = NULL;
        return;
    }

    rakPoint = new Vector2[iNumArcs+1];
    rakCenter = new Vector2[iNumArcs];
    rafRadius = new Real[iNumArcs];

    // intermediate ellipse quantities
    Real fA2 = fA*fA, fB2 = fB*fB, fAB = fA*fB, fInvB2mA2 = 1.0f/(fB2-fA2);

    // End points of ellipse in first quadrant.  Points are generated in
    // counterclockwise order.
    rakPoint[0] = Vector2(fA,0.0f);
    rakPoint[iNumArcs] = Vector2(0.0f,fB);

    // curvature at end points, store curvature for computing arcs
    Real fK0 = fA/fB2;
    Real fK1 = fB/fA2;

    // select ellipse points based on curvature properties
    Real fInvNumArcs = 1.0f/iNumArcs;
    int i;
    for (i = 1; i < iNumArcs; i++)
    {
        // curvature at new point is weighted average of curvature at ends
        Real fW1 = i*fInvNumArcs, fW0 = 1.0f - fW1;
        Real fK = fW0*fK0 + fW1*fK1;

        // compute point having this curvature
        Real fTmp = Math::Pow(fAB/fK,0.6666667f);
        rakPoint[i].x = fA*Math::Sqrt(Math::FAbs((fTmp-fA2)*fInvB2mA2));
        rakPoint[i].y = fB*Math::Sqrt(Math::FAbs((fTmp-fB2)*fInvB2mA2));
    }

    // compute arc at (a,0)
    Circle2 kCircle;
    Circumscribe(Vector2(rakPoint[1].x,-rakPoint[1].y),rakPoint[0],
        rakPoint[1],kCircle);
    rakCenter[0] = kCircle.Center();
    rafRadius[0] = kCircle.Radius();

    // compute arc at (0,b)
    int iLast = iNumArcs-1;
    Circumscribe(Vector2(-rakPoint[iLast].x,rakPoint[iLast].y),
        rakPoint[iNumArcs],rakPoint[iLast],kCircle);
    rakCenter[iLast] = kCircle.Center();
    rafRadius[iLast] = kCircle.Radius();

    // compute arcs at intermediate points between (a,0) and (0,b)
    int iM, iP;
    for (iM = 0, i = 1, iP = 2; i < iLast; iM++, i++, iP++)
    {
        Circumscribe(rakPoint[iM],rakPoint[i],rakPoint[iP],kCircle);
        rakCenter[i] = kCircle.Center();
        rafRadius[i] = kCircle.Radius();
    }
}
//----------------------------------------------------------------------------


