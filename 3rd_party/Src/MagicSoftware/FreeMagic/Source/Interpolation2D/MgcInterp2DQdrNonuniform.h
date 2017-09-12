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

#ifndef MGCINTERP2DQUADRATICNONUNIFORM_H
#define MGCINTERP2DQUADRATICNONUNIFORM_H

// Quadratic interpolation of a network of triangles whose vertices are of
// the form (x,y,f(x,y)).  This code is an implementation of the algorithm
// found in
//
//   Zoltan J. Cendes and Steven H. Wong,
//   C1 quadratic interpolation over arbitrary point sets,
//   IEEE Computer Graphics & Applications,
//   pp. 8-16, 1987

#include "MgcDelaunay2D.h"

namespace Mgc {


class MAGICFM Interp2DQdrNonuniform : public Delaunay2D
{
public:
    // Construction and destruction.
    //
    // The first two constructors implicitly create the triangle network from
    // the input vertices.  Each constructor accepts ownership of the input
    // arrays and will delete them during destruction.  The underlying
    // triangle network object also will be deleted.  The application must
    // specify the function F and its derivatives Fx and Fy at the spatial
    // locations in the first constructor.  In the second constructor, only
    // F is specified.  The derivatives Fx and Fy are estimated at the sample
    // points.
    //
    // The last two constructors share the input triangle network.  Each
    // constructor accepts ownership of the input function array, but does
    // not delete the triangle network on destruction.  The idea is that the
    // network was shared, either from an explicitly created one by the
    // application or from one created by another interpolator.  The
    // application must specify the function F at the spatial locations.  The
    // derivatives Fx and Fy will be estimated.

    Interp2DQdrNonuniform (int iVertexQuantity, Vector2* akVertex, Real* afF,
        Real* afFx, Real* afFy);

    Interp2DQdrNonuniform (int iVertexQuantity, Vector2* akVertex,
        Real* afF);

    Interp2DQdrNonuniform (Delaunay2D& rkNet, Real* afF, Real* afFx,
        Real* afFy);

    Interp2DQdrNonuniform (Delaunay2D& rkNet, Real* afF);

    virtual ~Interp2DQdrNonuniform ();

    // Quadratic interpolation.  The return value is 'true' if and only if the
    // input point is in the convex hull of the input vertices, in which case
    // the interpolation is valid.
    bool Evaluate (const Vector2& rkPoint, Real& rfF, Real& rfFx,
        Real& rfFy);

protected:
    class MAGICFM TriangleData
    {
    public:
        Vector2 m_kCenter;
        Vector2 m_akIntersect[3];
        Real m_afCoeff[19];
    };

    class MAGICFM Jet
    {
    public:
        Real m_fF, m_fFx, m_fFy;
    };

    Real* m_afF;
    Real* m_afFx;
    Real* m_afFy;
    TriangleData* m_akTData;  // triangle data
    Vector2* m_akECenter;  // extra triangle centers

    void EstimateDerivatives ();
    void ProcessTriangles ();
    void ComputeCrossEdgeIntersections (int iT);
    void ComputeCoefficients (int iT);
};

} // namespace Mgc

#endif


