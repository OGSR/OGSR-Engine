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

#ifndef MGCQUADRICSURFACE_H
#define MGCQUADRICSURFACE_H

#include "MgcMatrix3.h"
#include "MgcSurface.h"

namespace Mgc {


class MAGICFM QuadricSurface : public Surface
{
public:
    // Quadric surfaces are defined implicitly by x^T A x + b^T x + c = 0
    // where A is symmetric 3x3, b and x are 3x1, and c is a scalar.
    QuadricSurface (const Matrix3& rkA, const Vector3& rkB, Real fC);

    enum Type
    {
        QST_NONE,  // the implicit equation has no solution or is a tautology
        QST_POINT,
        QST_LINE,
        QST_PLANE,
        QST_TWO_PLANES,
        QST_PARABOLIC_CYLINDER,
        QST_ELLIPTIC_CYLINDER,
        QST_HYPERBOLIC_CYLINDER,
        QST_ELLIPTIC_PARABOLOID,
        QST_HYPERBOLIC_PARABOLOID,
        QST_ELLIPTIC_CONE,
        QST_HYPERBOLOID_ONE_SHEET,
        QST_HYPERBOLOID_TWO_SHEETS,
        QST_ELLIPSOID,
        QST_MAX_TYPE
    };

    // The returned array contains the eigenvalues of A, the entries of the
    // diagonal matrix D in y^T D y + e^T x + c = 0.
    void GetCharacterization (Type& eType, Real afD[3]) const;


    // Tessellation of a sphere using a 'seed' inscribed convex polyhedron.
    class Edge;
    class Triangle;
    class MAGICFM Vertex
    {
    public:
        Vector3* m_pkPoint;
        int m_iNumEdges;
        class Edge** m_apkEdge;
    };

    class MAGICFM Edge
    {
    public:
        Vertex* m_apkVertex[2];
        Triangle* m_apkTriangle[2];

        // For testing purposes, but not necessary for the algorithm.  This
        // allows the display program to show the subdivision structure.
        int m_iStep;
    };

    class MAGICFM Triangle
    {
    public:
        // pointers to triangle vertices (counterclockwise order)
        Vertex* m_apkVertex[3];

        // triangle edges: E0 = <V0,V1>, E1 = <V1,V2>, E2 = <V2,V0>
        Edge* m_apkEdge[3];

        // adjacent triangles:  adjacent[i] shares edge[i]
        Triangle* m_apkAdjacent[3];
    };

    class MAGICFM ConvexPolyhedron
    {
    public:
        int m_iNumVertices;
        Vertex* m_apkVertex;

        int m_iNumEdges;
        Edge* m_apkEdge;

        int m_iNumTriangles;
        Triangle* m_apkTriangle;

        // temporary storage, average of polyhedron vertices
        Vector3 m_kCentroid;
    };

    static void TessellateSphere (int iSteps, ConvexPolyhedron& rkPoly);
    static void DeletePolyhedron (ConvexPolyhedron& rkPoly);

protected:
    Matrix3 m_kA;
    Vector3 m_kB;
    Real m_fC;

    // support for sphere tessellation
    static int VertexIndex (const ConvexPolyhedron& rkPoly,
        const Vertex* pkV);
    static int EdgeIndex (const ConvexPolyhedron& rkPoly, const Edge* pkE);
    static int TriangleIndex (const ConvexPolyhedron& rkPoly,
        const Triangle* pkT);
    static int AdjacentOrient (const Triangle* pkT, const Triangle* pkA);
    static void ComputeCentroid (ConvexPolyhedron& rkPoly, int iNumVertices);
    static void RayIntersectSphere (const Vector3& rkCen,
        const Vector3& rkMid, Vector3* pkIntersect);
    static void Expand (int iSteps, ConvexPolyhedron& rkPoly);
};

} // namespace Mgc

#endif

