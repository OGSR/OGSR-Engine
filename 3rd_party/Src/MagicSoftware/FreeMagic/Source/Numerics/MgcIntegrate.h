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

#ifndef MGCINTEGRATE_H
#define MGCINTEGRATE_H

#include "MgcMath.h"

namespace Mgc {


class MAGICFM Integrate
{
public:
    // last parameter is for user-defined data
    typedef Real (*Function)(Real,void*);

    // Romberg integration
    static void SetOrder (int iOrder);
    static int GetOrder ();
    static Real RombergIntegral (Real fA, Real fB, Function oF,
        void* pvUserData = 0);

    // Gaussian quadrature
    static Real GaussianQuadrature (Real fA, Real fB, Function oF,
        void* pvUserData = 0);

protected:
    // parameters for Romberg integration
    static int ms_iOrder;
    static Real* ms_apfRom[2];

    // pre-main initialization and post-main termination
    static void Initialize ();
    static void Terminate ();
    friend class _IntegrateInitTerm;
};

} // namespace Mgc

#endif

