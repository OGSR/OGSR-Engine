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

#ifndef MGCCONVEXCLIPPER_H
#define MGCCONVEXCLIPPER_H

#include "MgcPlane.h"
#include <set>
#include <vector>

namespace Mgc {

class ConvexPolyhedron;
class Plane;

class MAGICFM ConvexClipper
{
public:
    class MAGICFM Vertex
    {
    public:
        Vertex ();

        Vector3 m_kPoint;
        Real m_fDistance;
        int m_iOccurs;
        bool m_bVisible;
    };

    class MAGICFM Edge
    {
    public:
        Edge ();

        int m_aiVertex[2];
        int m_aiFace[2];
        bool m_bVisible;
    };

    class MAGICFM Face
    {
    public:
        Face ();

        Plane m_kPlane;
        std::set<int> m_akEdge;
        bool m_bVisible;
    };

    // construction
    ConvexClipper (const ConvexPolyhedron& rkPoly, Real fEpsilon = 1e-06f);

    // Discard the portion of the mesh on the negative side of the plane.
    // This function is valid for any manifold triangle mesh (at most two
    // triangles shared per edge).
    int Clip (const Plane& rkPlane);

    // convert back to a convex polyhedron
    void Convert (ConvexPolyhedron& rkPoly);

    // for debugging
    bool Print (const char* acFilename) const;

protected:
    // support for post-processing faces
    class MAGICFM EdgePlus
    {
    public:
        EdgePlus ();
        EdgePlus (int iE, const Edge& rkE);

        bool operator< (const EdgePlus& rkE) const;
        bool operator== (const EdgePlus& rkE) const;
        bool operator!= (const EdgePlus& rkE) const;

        int m_iE, m_iV0, m_iV1, m_iF0, m_iF1;
    };

    void PostProcess (int iNF, Face& rkNF);

    bool GetOpenPolyline (Face& rkF, int& riVStart, int& riVFinal);
    void OrderVertices (Face& rkF, std::vector<int>& raiOrdered);
    void GetTriangles (std::vector<int>& raiConnect,
        std::vector<Plane>& rakPlane);

    std::vector<Vertex> m_akVertex;
    std::vector<Edge> m_akEdge;
    std::vector<Face> m_akFace;
    Real m_fEpsilon;
};

#include "MgcConvexClipper.inl"

} // namespace Mgc

#endif
