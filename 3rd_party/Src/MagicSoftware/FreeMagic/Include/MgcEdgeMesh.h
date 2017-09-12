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

#ifndef MGCEDGEMESH_H
#define MGCEDGEMESH_H

#include "MagicFMLibType.h"
#include <map>
#include <set>
#include <vector>

namespace Mgc {

class MAGICFM EdgeMesh
{
public:
    // vertex is <v>
    // edge is <v0,v1> where v0 = min(v0,v1)

    class Edge
    {
    public:
        Edge (int iV0, int iV1);
        bool operator< (const Edge& rkE) const;
        bool operator== (const Edge& rkE) const;
        bool operator!= (const Edge& rkE) const;

        int m_aiV[2];
    };


    EdgeMesh ();
    virtual ~EdgeMesh ();

    // Used for operations that create new meshes from the current one.  This
    // allows derived class construction within the base class operations.
    virtual EdgeMesh* Create () const;

    // Callbacks that are executed when vertices or edges are inserted or
    // removed during edge insertion, edge removal, or vertex collapsing.  The
    // default behavior for the creation is to return NULL pointers.  A
    // derived class may override the creation and return data to be attached
    // to the specific mesh component.  The default behavior for the
    // destruction is to do nothing.  A derived class may override the
    // destruction and handle the data that was detached from the specific
    // mesh component before its destruction.
    virtual void OnVertexInsert (int iV, bool bCreate, void*& rpvData);
    virtual void OnVertexRemove (int iV, bool bDestroy, void* pvData);
    virtual void OnEdgeInsert (const Edge& rkE, bool bCreate, void*& rpvData);
    virtual void OnEdgeRemove (const Edge& rkE, bool bDestroy, void* pvData);

    // Insert and remove edges.  The functions are virtual in case a
    // derived class wants to implement pre- and post-operation semantics.
    virtual void InsertEdge (int iV0, int iV1);
    virtual void InsertEdge (const Edge& rkE);
    virtual void RemoveEdge (int iV0, int iV1);
    virtual void RemoveEdge (const Edge& rkE);

    // This should be called before Mesh destruction if a derived class has
    // allocated vertex or edge data and attached it to the mesh components.
    // Since the creation and destruction callbacks are virtual, any
    // insert/remove operations in the base Mesh destructor will only
    // call the base virtual callbacks, not any derived-class ones.  An
    // alternative to calling this is that the derived class maintain enough
    // information to know which data objects to destroy during its own
    // destructor call.
    virtual void RemoveAllEdges ();

    // write the mesh to an ASCII file
    void Print (const char* acFilename) const;

    // vertex attributes
    void* GetData (int iV);
    std::set<Edge> GetEdges (int iV);

    // edge attributes
    void* GetData (int iV0, int iV1);
    void* GetData (const Edge& rkE);

    // The mesh is manifold if each vertex has at most two adjacent edges.
    // It is possible that the mesh has multiple connected components.
    bool IsManifold () const;

    // The mesh is closed if each vertex has exactly two adjacent edges.
    // It is possible that the mesh has multiple connected components.
    bool IsClosed () const;

    // The mesh is connected if each edge can be reached from any other
    // edge by a traversal.
    bool IsConnected () const;

    // Extract the connected components from the mesh.
    void GetComponents (std::vector<EdgeMesh*>& rkComponents);

    // Extract the connected components from the mesh, but each component has
    // a consistent ordering across all edges of that component.  The
    // mesh must be manifold.  The return value is 'true' if and only if the
    // mesh is manifold.  If the mesh has multiple components, each component
    // will have a consistent ordering.  However, the mesh knows nothing about
    // the mesh geometry, so it is possible that ordering across components is
    // not consistent.  For example, if the mesh has two disjoint closed
    // manifold components, one of them could have an ordering that implies
    // outward pointing normals and the other inward pointing normals.
    bool GetConsistentComponents (std::vector<EdgeMesh*>& rkComponents);

    // Reverse the ordering of all edges in the mesh.
    EdgeMesh* GetReversedOrderMesh () const;

protected:
    class VertexAttribute
    {
    public:
        void* m_pvData;
        std::set<Edge> m_kESet;
    };

    class EdgeAttribute
    {
    public:
        void* m_pvData;
    };


    std::map<int,VertexAttribute> m_kVMap;
    std::map<Edge,EdgeAttribute> m_kEMap;

    // for readability of the code
    typedef std::map<int,VertexAttribute>::iterator MVIter;
    typedef std::map<int,VertexAttribute>::const_iterator MVCIter;
    typedef std::map<Edge,EdgeAttribute>::iterator MEIter;
    typedef std::map<Edge,EdgeAttribute>::const_iterator MECIter;
    typedef std::set<int>::iterator SVIter;
    typedef std::set<int>::const_iterator SVCIter;
    typedef std::set<Edge>::iterator SEIter;
    typedef std::set<Edge>::const_iterator SECIter;
};

#include "MgcEdgeMesh.inl"

} // namespace Mgc

#endif


