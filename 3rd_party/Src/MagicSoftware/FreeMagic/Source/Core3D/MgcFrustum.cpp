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

#include "MgcFrustum.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Frustum::Frustum ()
    :
    m_kOrigin(Vector3::ZERO),
    m_kLVector(Vector3::UNIT_X),
    m_kUVector(Vector3::UNIT_Y),
    m_kDVector(Vector3::UNIT_Z)
{
    m_fLBound = 1.0f;
    m_fUBound = 1.0f;
    m_fDMin = 1.0f;
    m_fDMax = 2.0f;

    Update();
}
//----------------------------------------------------------------------------
void Frustum::ComputeVertices (Vector3 akVertex[8]) const
{
    Vector3 kDScaled = m_fDMin*m_kDVector;
    Vector3 kLScaled = m_fLBound*m_kLVector;
    Vector3 kUScaled = m_fUBound*m_kUVector;

    akVertex[0] = kDScaled - kLScaled - kUScaled;
    akVertex[1] = kDScaled - kLScaled + kUScaled;
    akVertex[2] = kDScaled + kLScaled + kUScaled;
    akVertex[3] = kDScaled + kLScaled - kUScaled;

    for (int i = 0, ip = 4; i < 4; i++, ip++)
    {
        akVertex[ip] = m_kOrigin + m_fDRatio*akVertex[i];
        akVertex[i] += m_kOrigin;
    }
}
//----------------------------------------------------------------------------
void Frustum::Update ()
{
    m_fDRatio = m_fDMax/m_fDMin;
    m_fMTwoLF = -2.0f*m_fLBound*m_fDMax;
    m_fMTwoUF = -2.0f*m_fUBound*m_fDMax;
}
//----------------------------------------------------------------------------


