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

#ifndef MGCCAPSULE_H
#define MGCCAPSULE_H

#include "MgcSegment3.h"

namespace Mgc {


class MAGICFM Capsule
{
public:
    Capsule ();

    Vector3& Origin ();
    const Vector3& Origin () const;

    Vector3& Direction ();
    const Vector3& Direction () const;

    Real& Radius ();
    const Real& Radius () const;

    Segment3& Segment ();
    const Segment3& Segment () const;

protected:
    Segment3 m_kSegment;
    Real m_fRadius;
};

#include "MgcCapsule.inl"

} // namespace Mgc

#endif


