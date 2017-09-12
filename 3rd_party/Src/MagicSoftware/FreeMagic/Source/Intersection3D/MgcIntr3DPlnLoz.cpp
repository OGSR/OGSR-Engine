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

#include "MgcIntr3DPlnLoz.h"
using namespace Mgc;

//----------------------------------------------------------------------------
bool Mgc::TestIntersection (const Plane& rkPlane, const Lozenge& rkLozenge,
    bool bUnitNormal)
{
    Vector3 kNormal = rkPlane.Normal();
    Real fConstant = rkPlane.Constant();
    if ( !bUnitNormal )
    {
        Real fLength = kNormal.Unitize();
        fConstant /= fLength;
    }

    Vector3 kC10 = rkLozenge.Origin() + rkLozenge.Edge0();
    Vector3 kC01 = rkLozenge.Origin() + rkLozenge.Edge1();
    Vector3 kC11 = kC10 + rkLozenge.Edge1();

    Real fTmp00 = kNormal.Dot(rkLozenge.Origin()) - fConstant;
    Real fTmp10 = kNormal.Dot(kC10) - fConstant;
    if ( fTmp00*fTmp10 <= 0.0f )
    {
        // two lozenge ends on opposite sides of the plane
        return true;
    }

    Real fTmp01 = kNormal.Dot(kC01) - fConstant;
    if ( fTmp00*fTmp01 <= 0.0f )
    {
        // two lozenge ends on opposite sides of the plane
        return true;
    }

    Real fTmp11 = kNormal.Dot(kC11) - fConstant;
    if ( fTmp10*fTmp11 <= 0.0f )
    {
        // two lozenge ends on opposite sides of the plane
        return true;
    }

    return Math::FAbs(fTmp00) <= rkLozenge.Radius()
        || Math::FAbs(fTmp10) <= rkLozenge.Radius()
        || Math::FAbs(fTmp01) <= rkLozenge.Radius()
        || Math::FAbs(fTmp11) <= rkLozenge.Radius();
}
//----------------------------------------------------------------------------
bool Mgc::Culled (const Plane& rkPlane, const Lozenge& rkLozenge,
    bool bUnitNormal)
{
    Vector3 kNormal = rkPlane.Normal();
    Real fConstant = rkPlane.Constant();
    if ( !bUnitNormal )
    {
        Real fLength = kNormal.Unitize();
        fConstant /= fLength;
    }

    Real fTmp00 = kNormal.Dot(rkLozenge.Origin()) - fConstant;
    if ( fTmp00 < 0.0f )
    {
        Real fDotNE0 = kNormal.Dot(rkLozenge.Edge0());
        Real fTmp10 = fTmp00 + fDotNE0;
        if ( fTmp10 < 0.0f )
        {
            Real fDotNE1 = kNormal.Dot(rkLozenge.Edge1());
            Real fTmp01 = fTmp00 + fDotNE1;
            if ( fTmp01 < 0.0f )
            {
                Real fTmp11 = fTmp10 + fDotNE1;
                if ( fTmp11 < 0.0f )
                {
                    // all four lozenge corners on negative side of plane
                    if ( fTmp00 <= fTmp10 )
                    {
                        if ( fTmp00 <= fTmp01 )
                            return fTmp00 <= -rkLozenge.Radius();
                        else
                            return fTmp01 <= -rkLozenge.Radius();
                    }
                    else
                    {
                        if ( fTmp10 <= fTmp11 )
                            return fTmp10 <= -rkLozenge.Radius();
                        else
                            return fTmp11 <= -rkLozenge.Radius();
                    }
                }
            }
        }
    }

    return false;
}
//----------------------------------------------------------------------------


