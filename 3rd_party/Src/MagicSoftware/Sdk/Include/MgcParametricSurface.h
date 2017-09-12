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

#ifndef MGCPARAMETRICSURFACE_H
#define MGCPARAMETRICSURFACE_H

#include "MgcSurface.h"
#include "MgcVector3.h"

namespace Mgc {


class MAGICFM ParametricSurface : public Surface
{
public:
    // position and derivatives up to second order
    virtual Vector3 GetPosition (Real fU, Real fV) const = 0;
    virtual Vector3 GetDerivativeU (Real fU, Real fV) const = 0;
    virtual Vector3 GetDerivativeV (Real fU, Real fV) const = 0;
    virtual Vector3 GetDerivativeUU (Real fU, Real fV) const = 0;
    virtual Vector3 GetDerivativeUV (Real fU, Real fV) const = 0;
    virtual Vector3 GetDerivativeVV (Real fU, Real fV) const = 0;

    // coordinate frame values
    Vector3 GetTangent0 (Real fU, Real fV) const;
    Vector3 GetTangent1 (Real fU, Real fV) const;
    Vector3 GetNormal (Real fU, Real fV) const;
    void GetFrame (Real fU, Real fV, Vector3& kPosition, Vector3& kTangent0,
        Vector3& kTangent1, Vector3& kNormal) const;

    // differential geometric quantities
    void ComputePrincipalCurvatureInfo (Real fU, Real fV, Real& rfCurv0,
        Real& rfCurv1, Vector3& rkDir0, Vector3& rkDir1);
};

} // namespace Mgc

#endif

