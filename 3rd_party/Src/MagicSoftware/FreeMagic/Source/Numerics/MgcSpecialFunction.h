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

#ifndef MGCSPECIALFUNCTION_H
#define MGCSPECIALFUNCTION_H

#include "MgcMath.h"

namespace Mgc {


class MAGICFM SpecialFunction
{
public:
    // gamma and related functions
    static Real LogGamma (Real fX);
    static Real Gamma (Real fX);
    static Real IncompleteGamma (Real fA, Real fX);

    // error functions
    static Real Erf (Real fX);   // polynomial approximation
    static Real Erfc (Real fX);  // erfc(x) = 1-erf(x)

    // modified Bessel functions of order 0 and 1
    static Real ModBessel0 (Real fX);
    static Real ModBessel1 (Real fX);

protected:
    // series form (used when fX < 1+fA)
    static Real IncompleteGammaS (Real fA, Real fX);

    // continued fraction form (used when fX >= 1+fA)
    static Real IncompleteGammaCF (Real fA, Real fX);
};

} // namespace Mgc

#endif

