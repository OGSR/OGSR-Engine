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

#include "MgcPlane.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Plane::Plane (const Vector3& rkPoint0, const Vector3& rkPoint1,
    const Vector3& rkPoint2)
{
    Vector3 kEdge1 = rkPoint1 - rkPoint0;
    Vector3 kEdge2 = rkPoint2 - rkPoint0;
    m_kNormal = kEdge1.UnitCross(kEdge2);
    m_fConstant = m_kNormal.Dot(rkPoint0);
}
//----------------------------------------------------------------------------
Plane::Side Plane::WhichSide (const Vector3& rkPoint) const
{
    Real fDistance = DistanceTo(rkPoint);

    if ( fDistance < 0.0f )
        return Plane::NEGATIVE_SIDE;

    if ( fDistance > 0.0f )
        return Plane::POSITIVE_SIDE;

    return Plane::NO_SIDE;
}
//----------------------------------------------------------------------------
void Plane::Normalize ()
{
    // assert:  |N| > 0
    Real fInvLength = 1.0f/m_kNormal.Length();
    m_kNormal *= fInvLength;
    m_fConstant *= fInvLength;
}
//----------------------------------------------------------------------------


