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

#include "MgcBox2.h"
using namespace Mgc;

//----------------------------------------------------------------------------
void Box2::ComputeVertices (Vector2 akVertex[4]) const
{
    Vector2 akEAxis[2] =
    {
        m_afExtent[0]*m_akAxis[0],
        m_afExtent[1]*m_akAxis[1]
    };

    akVertex[0] = m_kCenter - akEAxis[0] - akEAxis[1];
    akVertex[1] = m_kCenter + akEAxis[0] - akEAxis[1];
    akVertex[2] = m_kCenter + akEAxis[0] + akEAxis[1];
    akVertex[3] = m_kCenter - akEAxis[0] + akEAxis[1];
}
//----------------------------------------------------------------------------


