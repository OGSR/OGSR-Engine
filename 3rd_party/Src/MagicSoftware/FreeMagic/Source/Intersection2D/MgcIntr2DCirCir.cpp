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

#include "MgcIntr2DCirCir.h"
using namespace Mgc;

//----------------------------------------------------------------------------
static bool Find (const Vector2& rkC0, Real fR0, const Vector2& rkC1,
    Real fR1, int& riQuantity, Vector2 akPoint[2])
{
    // The two circles are |X-C0| = R0 and |X-C1| = R1.  Define U = C1 - C0
    // and V = Perp(U) where Perp(x,y) = (y,-x).  Note that Dot(U,V) = 0 and
    // |V|^2 = |U|^2.  The intersection points X can be written in the form
    // X = C0+s*U+t*V and X = C1+(s-1)*U+t*V.  Squaring the circle equations
    // and substituting these formulas into them yields
    //   R0^2 = (s^2 + t^2)*|U|^2
    //   R1^2 = ((s-1)^2 + t^2)*|U|^2.
    // Subtracting and solving for s yields
    //   s = ((R0^2-R1^2)/|U|^2 + 1)/2
    // Then replace in the first equation and solve for t^2
    //   t^2 = (R0^2/|U|^2) - s^2.
    // In order for there to be solutions, the right-hand side must be
    // nonnegative.  Some algebra leads to the condition for existence of
    // solutions,
    //   (|U|^2 - (R0+R1)^2)*(|U|^2 - (R0-R1)^2) <= 0.
    // This reduces to
    //   |R0-R1| <= |U| <= |R0+R1|.
    // If |U| = |R0-R1|, then the circles are side-by-side and just tangent.
    // If |U| = |R0+R1|, then the circles are nested and just tangent.
    // If |R0-R1| < |U| < |R0+R1|, then the two circles to intersect in two
    // points.

    Vector2 kU = rkC1 - rkC0;
    Real fUSqrLen = kU.SquaredLength();
    if ( fUSqrLen == 0.0f && fR0 == fR1 )
    {
        // Circles are the same.  TO DO.  Allow an 'epsilon' for the floating
        // point compares?
        riQuantity = -1;
        return true;
    }

    Real fR0pR1 = fR0 + fR1;
    Real fR0pR1Sqr = fR0pR1*fR0pR1;
    if ( fUSqrLen > fR0pR1Sqr )
    {
        riQuantity = 0;
        return false;
    }

    Real fR0mR1 = fR0 - fR1;
    Real fR0mR1Sqr = fR0mR1*fR0mR1;
    if ( fUSqrLen < fR0mR1Sqr )
    {
        riQuantity = 0;
        return false;
    }

    if ( fUSqrLen < fR0pR1Sqr )
    {
        if ( fR0mR1Sqr < fUSqrLen )
        {
            Real fInvUSqrLen = 1.0f/fUSqrLen;
            Real fS = 0.5f*((fR0*fR0-fR1*fR1)*fInvUSqrLen+1.0f);
            Vector2 kTmp = rkC0 + fS*kU;

            Real fT = Math::Sqrt(fR0*fR0*fInvUSqrLen - fS*fS);
            Vector2 kV(kU.y,-kU.x);
            riQuantity = 2;
            akPoint[0] = kTmp - fT*kV;
            akPoint[1] = kTmp + fT*kV;
        }
        else
        {
            // |U| = |R0-R1|, circles are tangent
            riQuantity = 1;
            akPoint[0] = rkC0 + (fR0/fR0mR1)*kU;
        }
    }
    else
    {
        // |U| = |R0+R1|, circles are tangent
        riQuantity = 1;
        akPoint[0] = rkC0 + (fR0/fR0pR1)*kU;
    }

    return true;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Circle2& rkCircle0,
    const Circle2& rkCircle1, int& riQuantity, Vector2 akPoint[2])
{
    return Find(rkCircle0.Center(),rkCircle0.Radius(),rkCircle1.Center(),
        rkCircle1.Radius(),riQuantity,akPoint);
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Circle2& rkCircle, const Arc2& rkArc,
    int& riQuantity, Vector2 akPoint[2])
{
    Vector2 akIntrPoint[2];
    bool bIntersects = Find(rkCircle.Center(),rkCircle.Radius(),
        rkArc.Center(),rkArc.Radius(),riQuantity,akIntrPoint);

    if ( riQuantity == -1 )
    {
        // arc is contained by the circle
        return true;
    }

    if ( bIntersects )
    {
        // test if circle-circle intersection points are on the arc
        int iIntrQuantity = riQuantity;
        riQuantity = 0;
        for (int i = 0; i < iIntrQuantity; i++)
        {
            akPoint[riQuantity] = akIntrPoint[i];
            if ( rkArc.Contains(akPoint[riQuantity]) )
                riQuantity++;
        }
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Arc2& rkArc0, const Arc2& rkArc1,
    int& riQuantity, Vector2 akPoint[2])
{
    Vector2 akIntrPoint[2];
    bool bIntersects = Find(rkArc0.Center(),rkArc0.Radius(),rkArc1.Center(),
        rkArc1.Radius(),riQuantity,akIntrPoint);

    if ( riQuantity == -1 )
    {
        // Arcs are cocircular.  Determine if they overlap.
        if ( rkArc1.Contains(rkArc0.End0()) )
        {
            if ( rkArc1.Contains(rkArc0.End1()) )
            {
                riQuantity = -2;
                akPoint[0] = rkArc0.End0();
                akPoint[1] = rkArc0.End1();
            }
            else
            {
                akPoint[0] = rkArc0.End0();
                if ( rkArc0.End0() != rkArc1.End1() )
                {
                    riQuantity = -2;
                    akPoint[1] = rkArc1.End1();
                }
                else
                {
                    riQuantity = -1;
                }
            }
            return true;
        }

        if ( rkArc1.Contains(rkArc0.End1()) )
        {
            akPoint[0] = rkArc1.End0();
            if ( rkArc0.End1() != rkArc1.End0() )
            {
                riQuantity = -2;
                akPoint[1] = rkArc0.End1();
            }
            else
            {
                riQuantity = -1;
            }
            return true;
        }

        Arc2 kTransArc;
        kTransArc.Center() = rkArc0.Center();
        kTransArc.Radius() = rkArc0.Radius();
        kTransArc.End0() = rkArc0.End0();
        kTransArc.End1() = rkArc1.End0();
        if ( kTransArc.Contains(rkArc0.End1()) )
        {
            riQuantity = 0;
            return false;
        }
        else
        {
            riQuantity = -2;
            akPoint[0] = rkArc1.End0();
            akPoint[1] = rkArc1.End1();
            return true;
        }
    }

    if ( bIntersects )
    {
        // test if circle-circle intersection points are on the arcs
        int iIntrQuantity = riQuantity;
        riQuantity = 0;
        for (int i = 0; i < iIntrQuantity; i++)
        {
            akPoint[riQuantity] = akIntrPoint[i];
            if ( rkArc0.Contains(akPoint[riQuantity])
            &&   rkArc1.Contains(akPoint[riQuantity]) )
            {
                riQuantity++;
            }
        }
    }

    return riQuantity != 0;
}
//----------------------------------------------------------------------------


