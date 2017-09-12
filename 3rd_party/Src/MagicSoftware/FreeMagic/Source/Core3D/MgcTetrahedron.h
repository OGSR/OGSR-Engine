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

#ifndef MGCTETRAHEDRON_H
#define MGCTETRAHEDRON_H

#include "MgcPlane.h"

namespace Mgc {

class MAGICFM Tetrahedron
{
public:
    Tetrahedron ();
    Tetrahedron (const Vector3& rkV0, const Vector3& rkV1,
        const Vector3& rkV2, const Vector3& rkV3);
    Tetrahedron (const Vector3 akV[4]);

    Vector3& operator[] (int i);
    const Vector3& operator[] (int i) const;

    // Construct the planes of the faces.  The planes have outer pointing
    // normal vectors, not necessarily unit length.
    void GetPlanes (Plane akPlane[4]) const;

protected:
    Vector3 m_akVertex[4];
};

#include "MgcTetrahedron.inl"

} // namespace Mgc

#endif


