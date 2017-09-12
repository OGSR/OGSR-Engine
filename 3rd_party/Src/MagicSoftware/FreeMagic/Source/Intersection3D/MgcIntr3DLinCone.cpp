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

#include "MgcIntr3DLinCone.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::FindIntersection (const Line3& rkLine, const Cone3& rkCone,
    int& riQuantity, Vector3 akPoint[2])
{
    Real fEpsilon = 1e-06f;

    // set up quadratic Q(t) = c2*t^2 + 2*c1*t + c0
    Real fAdD = rkCone.Axis().Dot(rkLine.Direction());
    Real fDdD = rkLine.Direction().Dot(rkLine.Direction());
    Real fCosSqr = rkCone.CosAngle()*rkCone.CosAngle();
    Vector3 kE = rkLine.Origin() - rkCone.Vertex();
    Real fAdE = rkCone.Axis().Dot(kE);
    Real fDdE = rkLine.Direction().Dot(kE);
    Real fEdE = kE.Dot(kE);
    Real fC2 = fAdD*fAdD - fCosSqr*fDdD;
    Real fC1 = fAdD*fAdE - fCosSqr*fDdE;
    Real fC0 = fAdE*fAdE - fCosSqr*fEdE;

    // Solve the quadratic.  Keep only those X for which Dot(A,X-V) > 0.
    if ( Math::FAbs(fC2) >= fEpsilon )
    {
        // c2 != 0
        Real fDiscr = fC1*fC1 - fC0*fC2;
        if ( fDiscr < 0.0f )
        {
            // no real roots
            riQuantity = 0;
            return false;
        }
        else if ( fDiscr > 0.0f )
        {
            // two distinct real roots
            Real fRoot = Math::Sqrt(fDiscr);
            Real fInvC2 = 1.0f/fC2;
            riQuantity = 0;

            Real fT = (-fC1 - fRoot)*fInvC2;
            akPoint[riQuantity] = rkLine.Origin() + fT*rkLine.Direction();
            kE = akPoint[riQuantity] - rkCone.Vertex();
            if ( kE.Dot(rkCone.Axis()) > 0.0f )
                riQuantity++;

            fT = (-fC1 + fRoot)*fInvC2;
            akPoint[riQuantity] = rkLine.Origin() + fT*rkLine.Direction();
            kE = akPoint[riQuantity] - rkCone.Vertex();
            if ( kE.Dot(rkCone.Axis()) > 0.0f )
                riQuantity++;

            return true;
        }
        else
        {
            // one repeated real root
            akPoint[0] = rkLine.Origin() - (fC1/fC2)*rkLine.Direction();
            kE = akPoint[0] - rkCone.Vertex();
            if ( kE.Dot(rkCone.Axis()) > 0.0f )
                riQuantity = 1;
            else
                riQuantity = 0;

            return true;
        }
    }
    else if ( Math::FAbs(fC1) >= fEpsilon )
    {
        // c2 = 0, c1 != 0
        akPoint[0] = rkLine.Origin() - (0.5f*fC0/fC1)*rkLine.Direction();
        kE = akPoint[0] - rkCone.Vertex();
        if ( kE.Dot(rkCone.Axis()) > 0.0f )
            riQuantity = 1;
        else
            riQuantity = 0;

        return true;
    }
    else if ( Math::FAbs(fC0) >= fEpsilon )
    {
        // c2 = c1 = 0, c0 != 0
        return false;
    }
    else
    {
        // c2 = c1 = c0 = 0, cone contains ray V+t*D where V is cone vertex
        // and D is the line direction.
        riQuantity = -1;
        return true;
    }
}
//----------------------------------------------------------------------------


