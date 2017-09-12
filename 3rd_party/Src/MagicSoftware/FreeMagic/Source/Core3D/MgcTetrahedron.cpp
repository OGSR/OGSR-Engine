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

#include "MgcTetrahedron.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Tetrahedron::Tetrahedron (const Vector3& rkV0, const Vector3& rkV1,
    const Vector3& rkV2, const Vector3& rkV3)
{
    m_akVertex[0] = rkV0;
    m_akVertex[1] = rkV1;
    m_akVertex[2] = rkV2;
    m_akVertex[3] = rkV3;
}
//----------------------------------------------------------------------------
Tetrahedron::Tetrahedron (const Vector3 akV[4])
{
    memcpy(m_akVertex,akV,4*sizeof(Vector3));
}
//----------------------------------------------------------------------------
void Tetrahedron::GetPlanes (Plane akPlane[4]) const
{
    Vector3 kEdge10 = m_akVertex[1] - m_akVertex[0];
    Vector3 kEdge20 = m_akVertex[2] - m_akVertex[0];
    Vector3 kEdge30 = m_akVertex[3] - m_akVertex[0];
    Vector3 kEdge21 = m_akVertex[2] - m_akVertex[1];
    Vector3 kEdge31 = m_akVertex[3] - m_akVertex[1];

    akPlane[0].Normal() = kEdge20.Cross(kEdge10);  // <v0,v2,v1>
    akPlane[1].Normal() = kEdge10.Cross(kEdge30);  // <v0,v1,v3>
    akPlane[2].Normal() = kEdge30.Cross(kEdge20);  // <v0,v3,v2>
    akPlane[3].Normal() = kEdge21.Cross(kEdge31);  // <v1,v2,v3>

    Real fDet = kEdge10.Dot(akPlane[3].Normal());
    int i;
    if ( fDet < 0.0f )
    {
        // normals are inner pointing, reverse their directions
        for (i = 0; i < 4; i++)
            akPlane[i].Normal() *= -1.0f;
    }

    for (i = 0; i < 4; i++)
        akPlane[i].Constant() = m_akVertex[i].Dot(akPlane[i].Normal());
}
//----------------------------------------------------------------------------
