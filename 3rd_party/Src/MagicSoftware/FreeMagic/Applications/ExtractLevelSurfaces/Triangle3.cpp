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

#include "Triangle3.h"

//----------------------------------------------------------------------------
Triangle3::Triangle3 (int j0, int j1, int j2)
{
    if ( j0 < j1 )
    {
        if ( j0 < j2 )
        {
            i0 = j0;
            i1 = j1;
            i2 = j2;
        }
        else
        {
            i0 = j2;
            i1 = j0;
            i2 = j1;
        }
    }
    else
    {
        if ( j1 < j2 )
        {
            i0 = j1;
            i1 = j2;
            i2 = j0;
        }
        else
        {
            i0 = j2;
            i1 = j0;
            i2 = j1;
        }
    }
}
//----------------------------------------------------------------------------
bool Triangle3::operator< (const Triangle3& rkT) const
{
    if ( i0 < rkT.i0 ) return true;
    if ( i0 > rkT.i0 ) return false;
    if ( i1 < rkT.i1 ) return true;
    if ( i1 > rkT.i1 ) return false;
    return i2 < rkT.i2;
}
//----------------------------------------------------------------------------
