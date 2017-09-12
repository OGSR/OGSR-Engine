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

#ifndef MGCINTERP2DSPHERE_H
#define MGCINTERP2DSPHERE_H

// Interpolation of a scalar-valued function defined on a sphere.  Although
// the sphere lives in 3D, the interpolation is a 2D method whose input
// points are angles (theta,phi) from spherical coordinates.  The domains of
// the angles are -PI <= theta <= PI and 0 <= phi <= PI.

#include "MgcInterp2DQdrNonuniform.h"

namespace Mgc {


class MAGICFM Interp2DSphere
{
public:
    // Construction and destruction.  Interp2DSphere accepts ownership of
    // the input arrays and will delete them during destruction.  For complete
    // spherical coverage, include the two antipodal (theta,phi) points
    // (-PI,0,F(-PI,0)) and (-PI,PI,F(-PI,PI)) in the input data.  These
    // correspond to the sphere poles x = 0, y = 0, and |z| = 1.
    Interp2DSphere (int iVertexQuantity, Real* afTheta, Real* afPhi,
        Real* afF);

    ~Interp2DSphere ();

    // Spherical coordinates are
    //   x = cos(theta)*sin(phi)
    //   y = sin(theta)*sin(phi)
    //   z = cos(phi)
    // for -PI <= theta <= PI, 0 <= phi <= PI.  The application can use this
    // function to convert unit length vectors (x,y,z) to (theta,phi).
    static void GetSphericalCoords (Real fX, Real fY, Real fZ,
        Real& rfTheta, Real& rfPhi);

    bool Evaluate (Real fTheta, Real fPhi, Real& rfF);

protected:
    Interp2DQdrNonuniform* m_pkInterp;
};

} // namespace Mgc

#endif


