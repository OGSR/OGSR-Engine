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

#ifndef MGCDIST3DLINBOX_H
#define MGCDIST3DLINBOX_H

#include "MgcBox3.h"
#include "MgcLine3.h"
#include "MgcRay3.h"
#include "MgcSegment3.h"

namespace Mgc {

// Requirement: Output pointers must be either all non-null or all null.


// squared distance measurements

MAGICFM Real SqrDistance (const Line3& rkLine, const Box3& rkBox,
    Real* pfLParam = NULL, Real* pfBParam0 = NULL, Real* pfBParam1 = NULL,
    Real* pfBParam2 = NULL);

MAGICFM Real SqrDistance (const Ray3& rkRay, const Box3& rkBox,
    Real* pfLParam = NULL, Real* pfBParam0 = NULL, Real* pfBParam1 = NULL,
    Real* pfBParam2 = NULL);

MAGICFM Real SqrDistance (const Segment3& rkSeg, const Box3& rkBox,
    Real* pfLParam = NULL, Real* pfBParam0 = NULL, Real* pfBParam1 = NULL,
    Real* pfBParam2 = NULL);


// distance measurements

MAGICFM Real Distance (const Line3& rkLine, const Box3& rkBox,
    Real* pfLParam = NULL, Real* pfBParam0 = NULL, Real* pfBParam1 = NULL,
    Real* pfBParam2 = NULL);

MAGICFM Real Distance (const Ray3& rkRay, const Box3& rkBox,
    Real* pfLParam = NULL, Real* pfBParam0 = NULL, Real* pfBParam1 = NULL,
    Real* pfBParam2 = NULL);

MAGICFM Real Distance (const Segment3& rkSeg, const Box3& rkBox,
    Real* pfLParam = NULL, Real* pfBParam0 = NULL, Real* pfBParam1 = NULL,
    Real* pfBParam2 = NULL);

} // namespace Mgc

#endif


