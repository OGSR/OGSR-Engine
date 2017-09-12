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

#include "MgcXFormInterp.h"
using namespace Mgc;

//----------------------------------------------------------------------------
void Mgc::XFormInterp (const Matrix3& rkM0, const Vector3& rkT0,
    const Matrix3& rkM1, const Vector3& rkT1, Real fTime, Matrix3& rkM,
    Vector3& rkT)
{
    if ( fTime <= 0.0f )
    {
        rkM = rkM0;
        rkT = rkT0;
        return;
    }

    if ( fTime >= 1.0f )
    {
        rkM = rkM1;
        rkT = rkT1;
        return;
    }

    // factor M0 = L0*S0*R0
    Matrix3 kL0, kR0;
    Vector3 kS0;
    rkM0.SingularValueDecomposition(kL0,kS0,kR0);

    // compute quaternions for L0 and R0
    Quaternion kQL0, kQR0;
    kQL0.FromRotationMatrix(kL0);
    kQR0.FromRotationMatrix(kR0);

    // factor M1 = L1*S1*R1
    Matrix3 kL1, kR1;
    Vector3 kS1;
    rkM1.SingularValueDecomposition(kL1,kS1,kR1);

    // compute quaternions for L1 and R1
    Quaternion kQL1, kQR1;
    kQL1.FromRotationMatrix(kL1);
    kQR1.FromRotationMatrix(kR1);

    // compute M = L*S*R
    Matrix3 kL, kR;
    Vector3 kS;

    // spherical linear interpolation of quaternions for L and R
    Quaternion kQL = Quaternion::Slerp(fTime,kQL0,kQL1);
    Quaternion kQR = Quaternion::Slerp(fTime,kQR0,kQR1);
    kQL.ToRotationMatrix(kL);
    kQR.ToRotationMatrix(kR);

    // generalized geometric means, S = S0^{1-t}*S1^t
    Real fOMTime = 1.0f - fTime;
    kS[0] = Math::Pow(Math::FAbs(kS0[0]),fOMTime) *
        Math::Pow(Math::FAbs(kS1[0]),fTime);
    kS[1] = Math::Pow(Math::FAbs(kS0[1]),fOMTime) *
        Math::Pow(Math::FAbs(kS1[1]),fTime);
    kS[2] = Math::Pow(Math::FAbs(kS0[2]),fOMTime) *
        Math::Pow(Math::FAbs(kS1[2]),fTime);

    // compute the product M = L*S*R
    rkM.SingularValueComposition(kL,kS,kR);

    // linearly interpolate T = (1-t)*T0+t*T1
    rkT = fOMTime*rkT0 + fTime*rkT1;
}
//----------------------------------------------------------------------------


