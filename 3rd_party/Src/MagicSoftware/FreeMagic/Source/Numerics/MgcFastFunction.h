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

#ifndef MGCFASTFUNCTION_H
#define MGCFASTFUNCTION_H

#include "MgcMath.h"

namespace Mgc {


class MAGICFM FastFunction
{
public:
    static Real Sin0 (Real fT);  // 0 <= t <= PI/2
    static Real Sin1 (Real fT);  // 0 <= t <= PI/2
    static Real Cos0 (Real fT);  // 0 <= t <= PI/2
    static Real Cos1 (Real fT);  // 0 <= t <= PI/2
    static Real Tan0 (Real fT);  // 0 <= t <= PI/4
    static Real Tan1 (Real fT);  // 0 <= t <= PI/4

    static Real InvSin0 (Real fT);  // 0 <= t <= 1
    static Real InvCos0 (Real fT);  // 0 <= t <= 1
    static Real InvTan0 (Real fT);  // |t| <= 1
    static Real InvTan1 (Real fT);  // |t| <= 1
    static Real InvTan2 (Real fT);  // |t| <= 1
};

} // namespace Mgc

#endif

