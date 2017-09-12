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

#ifndef IMAGEINTERP3D_H
#define IMAGEINTERP3D_H

#include "MgcImages.h"
#include "MgcVector3.h"
using namespace Mgc;

#include <map>
#include <set>
using namespace std;

class ImageInterp3D : public ImageInt3D
{
public:
    ImageInterp3D (int iXBound, int iYBound, int iZBound, int* aiData);
    ImageInterp3D (const char* acFilename);

    // linear interpolation for F and grad(F)
    float GetFunction (const Vector3& rkP) const;
    Vector3 GetGradient (const Vector3& rkP) const;

    class Vertex
    {
    public:
        Vertex (int iXNumer = 0, int iXDenom = 0, int iYNumer = 0,
            int iYDenom = 0, int iZNumer = 0, int iZDenom = 0);

        // for STL map
        bool operator< (const Vertex& rkVertex) const;
        bool operator== (const Vertex& rkVertex) const;
        bool operator!= (const Vertex& rkVertex) const;

        void GetTriple (Vector3& rkMeshVertex) const;

        // vertices stored as rational numbers
        //   (xnumer/xdenom,ynumer/ydenom,znumer/zdenom)
        int m_iXNumer, m_iXDenom, m_iYNumer, m_iYDenom, m_iZNumer, m_iZDenom;
    };

    class Edge
    {
    public:
        Edge (int i0 = -1, int i1 = -1);

        // for STL map
        bool operator< (const Edge& rkEdge) const;
        bool operator== (const Edge& rkEdge) const;
        bool operator!= (const Edge& rkEdge) const;

        // indices into the vertex table
        int m_i0, m_i1;
    };

    class Triangle
    {
    public:
        Triangle (int i0 = -1, int i1 = -1, int i2 = -1);

        // for STL map
        bool operator< (const Triangle& rkTriangle) const;
        bool operator== (const Triangle& rkTriangle) const;
        bool operator!= (const Triangle& rkTriangle) const;

        // indices into the vertex table
        int m_i0, m_i1, m_i2;
    };

    // The extraction assumes linear interpolation (decomposition of image
    // domain into tetrahedra).  The triangle ordering is selected so that
    // the triangle is counterclockwise with respect to the triangle normal
    // chosen to form an acute angle with the negative gradient of the
    // image at the centroid of the triangle.
    void ExtractLevelSetLinear (int iLevel, int& riVertexQuantity,
        Vertex*& rakVertex, int& riEdgeQuantity, Edge*& rakEdge,
        int& riTriangleQuantity, Triangle*& rakTriangle);

protected:
    // support for extraction of level sets
    typedef map<Vertex,int> VMap;
    typedef map<Vertex,int>::iterator VIterator;
    typedef set<Edge> ESet;
    typedef set<Edge>::iterator EIterator;
    typedef set<Triangle> TSet;
    typedef set<Triangle>::iterator TIterator;

    int AddVertex (VMap& rkVMap, int iXNumer, int iXDenom, int iYNumer,
        int iYDenom, int iZNumer, int iZDenom);

    void AddEdge (VMap& rkVMap, ESet& rkESet, int iXNumer0,
        int iXDenom0, int iYNumer0, int iYDenom0, int iZNumer0, int iZDenom0,
        int iXNumer1, int iXDenom1, int iYNumer1, int iYDenom1, int iZNumer1,
        int iZDenom1);

    void AddTriangle (VMap& rkVMap, ESet& rkESet, TSet& rkTSet,
        int iXNumer0, int iXDenom0, int iYNumer0, int iYDenom0, int iZNumer0,
        int iZDenom0, int iXNumer1, int iXDenom1, int iYNumer1, int iYDenom1,
        int iZNumer1, int iZDenom1, int iXNumer2, int iXDenom2, int iYNumer2,
        int iYDenom2, int iZNumer2, int iZDenom2);

    // support for extraction with linear interpolation
    void ProcessTetrahedron (int iLevel, VMap& rkVM, ESet& rkES, TSet& rkTS,
        int iX0, int iY0, int iZ0, int iF0, int iX1, int iY1, int iZ1,
        int iF1, int iX2, int iY2, int iZ2, int iF2, int iX3, int iY3,
        int iZ3, int iF3);

    // bound(0)*bound(1), for image indexing
    int m_iXYProduct;

    // for unique indexing of vertices
    int m_iNextIndex;
};

#include "ImageInterp3D.inl"

#endif


