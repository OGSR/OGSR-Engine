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

#include "Vertex3.h"

//----------------------------------------------------------------------------
Vertex3::Vertex3 (float fX, float fY, float fZ)
{
    x = fX;
    y = fY;
    z = fZ;
}
//----------------------------------------------------------------------------
bool Vertex3::operator< (const Vertex3& rkV) const
{
    if ( x < rkV.x ) return true;
    if ( x > rkV.x ) return false;
    if ( y < rkV.y ) return true;
    if ( y > rkV.y ) return false;
    return z < rkV.z;
}
//----------------------------------------------------------------------------
