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

#ifndef MGCFRUSTUM_H
#define MGCFRUSTUM_H

#include "MgcVector3.h"

namespace Mgc {


class MAGICFM Frustum
{
public:
    // Orthogonal frustum.  Let E be the origin, L be the left vector, U be
    // the up vector, and D be the direction vector.  Let l > 0 and u > 0 be
    // the extents in the L and U directions, respectively.  Let n and f be
    // the extents in the D direction with 0 < n < f.  The four corners of the
    // frustum in the near plane are E + s0*l*L + s1*u*U + n*D where |s0| =
    // |s1| = 1 (four choices).  The four corners of the frustum in the far
    // plane are E + (f/n)*(s0*l*L + s1*u*U) where |s0| = |s1| = 1 (four
    // choices).

    Frustum ();

    Vector3& Origin ();
    const Vector3& Origin () const;

    Vector3& LVector ();
    const Vector3& LVector () const;
    Vector3& UVector ();
    const Vector3& UVector () const;
    Vector3& DVector ();
    const Vector3& DVector () const;

    Real& LBound ();
    const Real& LBound () const;
    Real& UBound ();
    const Real& UBound () const;
    Real& DMin ();
    const Real& DMin () const;
    Real& DMax ();
    const Real& DMax () const;

    void Update ();
    Real GetDRatio () const;
    Real GetMTwoLF () const;
    Real GetMTwoUF () const;

    void ComputeVertices (Vector3 akVertex[8]) const;

protected:
    Vector3 m_kOrigin, m_kLVector, m_kUVector, m_kDVector;
    Real m_fLBound, m_fUBound, m_fDMin, m_fDMax;

    // derived quantities
    Real m_fDRatio;
    Real m_fMTwoLF, m_fMTwoUF;
};

#include "MgcFrustum.inl"

} // namespace Mgc

#endif


