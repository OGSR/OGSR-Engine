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

#ifndef MGCCONVEXPOLYHEDRON_H
#define MGCCONVEXPOLYHEDRON_H

#include "MgcPlane.h"
#include "MgcMTMesh.h"
#include "MgcVector2.h"
#include <set>
#include <vector>

namespace Mgc {

class MAGICFM ConvexPolyhedron : public MTMesh
{
public:
    // construction
    ConvexPolyhedron ();
    ConvexPolyhedron (const std::vector<Vector3>& rakPoint,
        const std::vector<int>& raiConnect);
    ConvexPolyhedron (const std::vector<Vector3>& rakPoint,
        const std::vector<int>& raiConnect,
        const std::vector<Plane>& rakPlane);
    ConvexPolyhedron (const ConvexPolyhedron& rkPoly);

    void Create (const std::vector<Vector3>& rakPoint,
        const std::vector<int>& raiConnect);
    void Create (const std::vector<Vector3>& rakPoint,
        const std::vector<int>& raiConnect,
        const std::vector<Plane>& rakPlane);

    ConvexPolyhedron& operator= (const ConvexPolyhedron& rkPoly);

    // read points and planes
    const std::vector<Vector3>& GetPoints () const;
    const Vector3& GetPoint (int iV) const;
    const std::vector<Plane>& GetPlanes () const;
    const Plane& GetPlane (int iT) const;

    // Allow vertex modification.  The caller is responsible for preserving
    // the convexity.  After modifying the vertices, call UpdatePlanes to
    // recompute the planes of the polyhedron faces.
    int AddPoint (const Vector3& rkPoint);
    std::vector<Vector3>& Points ();
    Vector3& Point (int iV);
    void UpdatePlanes ();

    // Test for convexity:  Assuming the application has guaranteed that the
    // mesh is manifold and closed, this function will iterate over the faces
    // of the polyhedron and verify for each that the polyhedron vertices are
    // all on the nonnegative side of the plane.  The threshold is the value
    // that the plane distance d is compared to, d < 0.  In theory the
    // distances should all be nonegative.  Floating point round-off errors
    // can cause some small distances, so you might set fThreshold to a small
    // negative number.
    bool ValidateHalfSpaceProperty (Real fThreshold = 0.0f) const;
    void ComputeCentroid ();
    const Vector3& GetCentroid () const;

    // discard the portion of the mesh on the negative side of the plane
    bool Clip (const Plane& rkPlane, ConvexPolyhedron& rkIntr) const;

    // compute the polyhedron of intersection
    bool FindIntersection (const ConvexPolyhedron& rkPoly,
        ConvexPolyhedron& rkIntr) const;

    static void FindAllIntersections (int iQuantity, ConvexPolyhedron* akPoly,
        int& riCombos, ConvexPolyhedron**& rapkIntr);

    Real GetSurfaceArea () const;
    Real GetVolume () const;
    bool ContainsPoint (const Vector3& rkP) const;

    // The eye point must be outside the polyhedron.  The output is the
    // terminator, an ordered list of vertices forming a simple closed
    // polyline that separates the visible from invisible faces of the
    // polyhedron.
    void ComputeTerminator (const Vector3& rkEye,
        std::vector<Vector3>& rkTerminator);

    // If projection plane is Dot(N,X) = c where N is unit length, then the
    // application must ensure that Dot(N,eye) > c.  That is, the eye point is
    // on the side of the plane to which N points.  The application must also
    // specify two vectors U and V in the projection plane so that {U,V,N} is
    // a right-handed and orthonormal set (the matrix [U V N] is orthonormal
    // with determinant 1).  The origin of the plane is computed internally as
    // the closest point to the eye point (an orthogonal pyramid for the
    // perspective projection).  If all vertices P on the terminator satisfy
    // Dot(N,P) < Dot(N,eye), then the polyhedron is completely visible (in
    // the sense of perspective projection onto the viewing plane).  In this
    // case the silhouette is computed by projecting the terminator points
    // onto the viewing plane.  The return value of the function is 'true'
    // when this happens.  However, if at least one terminator point P
    // satisfies Dot(N,P) >= Dot(N,eye), then the silhouette is unbounded in
    // the view plane.  It is not computed and the function returns 'false'.
    // A silhouette point (x,y) is extracted from the point Q that is the
    // intersection of the ray whose origin is the eye point and that contains
    // a terminator point, Q = K+x*U+y*V+z*N where K is the origin of the
    // plane.
    bool ComputeSilhouette (const Vector3& rkEye, const Plane& rkPlane,
        const Vector3& rkU, const Vector3& rkV,
        std::vector<Vector2>& rkSilhouette);

    bool ComputeSilhouette (std::vector<Vector3>& rkTerminator,
        const Vector3& rkEye, const Plane& rkPlane, const Vector3& rkU,
        const Vector3& rkV, std::vector<Vector2>& rkSilhouette);

    // Create an egg-shaped object that is axis-aligned and centered at
    // (xc,yc,zc).  The input bounds are all positive and represent the
    // distances from the center to the six extreme points on the egg.
    static void CreateEggShape (const Vector3& rkCenter, Real fX0, Real fX1,
        Real fY0, Real fY1, Real fZ0, Real fZ1, int iMaxSteps,
        ConvexPolyhedron& rkEgg);

    // debugging support
    virtual void Print (std::ofstream& rkOStr) const;
    virtual bool Print (const char* acFilename) const;

protected:
    // support for intersection testing
    static ConvexPolyhedron* FindSolidIntersection (
        const ConvexPolyhedron& rkPoly0, const ConvexPolyhedron& rkPoly1);
    static int GetHighBit (int i);

    // support for computing surface area
    Real GetTriangleArea (const Vector3& rkN, const Vector3& rkV0,
        const Vector3& rkV1, const Vector3& rkV2) const;

    // support for computing the terminator and silhouette
    Real GetDistance (const Vector3& rkEye, int iT,
        std::vector<Real>& rafDistance) const;
    static bool IsNegativeProduct (Real fDist0, Real fDist1);

    std::vector<Vector3> m_akPoint;
    std::vector<Plane> m_akPlane;
    Vector3 m_kCentroid;
};

#include "MgcConvexPolyhedron.inl"

} // namespace Mgc

#endif
