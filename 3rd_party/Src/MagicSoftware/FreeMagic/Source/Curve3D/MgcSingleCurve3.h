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

#ifndef MGCSINGLECURVE3_H
#define MGCSINGLECURVE3_H

#include "MgcCurve3.h"

namespace Mgc {


class MAGICFM SingleCurve3 : public Curve3
{
public:
    // abstract base class
    SingleCurve3 (Real fTMin, Real fTMax);

    // length-from-time and time-from-length
    virtual Real GetLength (Real fT0, Real fT1) const;
    virtual Real GetTime (Real fLength, int iIterations = 32,
        Real fTolerance = 1e-06f) const;

protected:
    static Real GetSpeedWithData (Real fTime, void* pvData);
};

} // namespace Mgc

#endif


