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

#include "MgcIntervalIntersection.h"
#include "MgcRTLib.h"
using namespace Mgc;

//----------------------------------------------------------------------------
void Mgc::FindIntersection (Real fU0, Real fU1, Real fV0, Real fV1,
    int& riQuantity, Real& rfW0, Real& rfW1)
{
    assert( fU0 < fU1 && fV0 < fV1 );

    if ( fU1 < fV0 || fU0 > fV1 )
    {
        riQuantity = 0;
    }
    else if ( fU1 > fV0 )
    {
        if ( fU0 < fV1 )
        {
            riQuantity = 2;
            rfW0 = ( fU0 < fV0 ? fV0 : fU0 );
            rfW1 = ( fU1 > fV1 ? fV1 : fU1 );
        }
        else  // fU0 == fV1
        {
            riQuantity = 1;
            rfW0 = fU0;
        }
    }
    else  // fU1 == fV0
    {
        riQuantity = 1;
        rfW0 = fU1;
    }
}
//----------------------------------------------------------------------------
int Mgc::FindIntersection (Real fU0, Real fU1, Real& rfV0, Real& rfV1)
{
    assert( fU0 < fU1 && rfV0 < rfV1 );

    if ( fU1 < rfV0 || fU0 > rfV1 )
    {
        return 0;
    }
    else if ( fU1 > rfV0 )
    {
        if ( fU0 < rfV1 )
        {
            if ( fU0 > rfV0 )
                rfV0 = fU0;
            if ( fU1 < rfV1 )
                rfV1 = fU1;
            return 2;
        }
        else  // fU0 == fV1
        {
            rfV0 = fU0;
            return 1;
        }
    }
    else  // fU1 == fV0
    {
        rfV0 = fU1;
        return 1;
    }
}
//----------------------------------------------------------------------------


