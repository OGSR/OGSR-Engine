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

#include "MgcMatrix3.h"
#include "MgcSurface.h"

namespace Mgc {


class MAGICFM ImplicitSurface : public Surface
{
public:
    // Surface is defined by F(x,y,z) = 0.  In all member functions it is
    // the application's responsibility to ensure that (x,y,z) is a solution
    // to F = 0.

    typedef Real (*Function)(Real,Real,Real);

    ImplicitSurface (
        Function oF,       // F(x,y,z) = 0 is the surface
        Function aoDF[3],  // (Fx,Fy,Fz)
        Function aoD2F[6]  // (Fxx,Fxy,Fxz,Fyy,Fyz,Fzz)
    );

    // verify point is on surface
    bool IsOnSurface (Real fX, Real fY, Real fZ,
        Real fTolerance = 1e-06f) const;

    // derivatives up to second order
    Vector3 GetGradient (Real fX, Real fY, Real fZ) const;
    Matrix3 GetHessian (Real fX, Real fY, Real fZ) const;

    // coordinate frame values
    void GetFrame (Real fX, Real fY, Real fZ, Vector3& kTangent0,
        Vector3& kTangent1, Vector3& kNormal) const;

    // differential geometric quantities
    bool ComputePrincipalCurvatureInfo (Real fX, Real fY, Real fZ,
        Real& rfCurv0, Real& rfCurv1, Vector3& rkDir0, Vector3& rkDir1);

protected:
	Function m_oF;
	Function m_aoDF[3];
	Function m_aoD2F[6];
};

} // namespace Mgc

#endif

