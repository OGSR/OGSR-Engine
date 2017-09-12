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

#include "MgcMTMesh.h"
#include <fstream>
using namespace Mgc;
using namespace std;

//----------------------------------------------------------------------------
MTMesh::MTMesh (int iVQuantity, int iEQuantity, int iTQuantity)
    :
    m_akVertex(iVQuantity),
    m_akEdge(iEQuantity),
    m_akTriangle(iTQuantity)
{
    m_iInitialELabel = -1;
    m_iInitialTLabel = -1;
}
//----------------------------------------------------------------------------
MTMesh::MTMesh (const MTMesh& rkMesh)
{
    *this = rkMesh;
}
//----------------------------------------------------------------------------
MTMesh::~MTMesh ()
{
}
//----------------------------------------------------------------------------
void MTMesh::Reset (int iVQuantity, int iEQuantity, int iTQuantity)
{
    m_akVertex.Reset(iVQuantity);
    m_akEdge.Reset(iEQuantity);
    m_akTriangle.Reset(iTQuantity);
    m_kVMap.clear();
    m_kEMap.clear();
    m_kTMap.clear();
}
//----------------------------------------------------------------------------
MTMesh& MTMesh::operator= (const MTMesh& rkMesh)
{
    m_akVertex = rkMesh.m_akVertex;
    m_akEdge = rkMesh.m_akEdge;
    m_akTriangle = rkMesh.m_akTriangle;
    m_kVMap = rkMesh.m_kVMap;
    m_kEMap = rkMesh.m_kEMap;
    m_kTMap = rkMesh.m_kTMap;
    m_iInitialELabel = rkMesh.m_iInitialELabel;
    m_iInitialTLabel = rkMesh.m_iInitialTLabel;
    return *this;
}
//----------------------------------------------------------------------------
bool MTMesh::Insert (int iLabel0, int iLabel1, int iLabel2)
{
    // insert triangle
    int iT = InsertTriangle(iLabel0,iLabel1,iLabel2);
    if ( iT == -1 )
    {
        // triangle already exists
        return true;
    }

    // insert vertices
    int iV0 = InsertVertex(iLabel0);
    int iV1 = InsertVertex(iLabel1);
    int iV2 = InsertVertex(iLabel2);

    // insert edges
    int iE0 = InsertEdge(iLabel0,iLabel1);
    int iE1 = InsertEdge(iLabel1,iLabel2);
    int iE2 = InsertEdge(iLabel2,iLabel0);

    // set the connections between components
    MTTriangle& rkT = m_akTriangle[iT];
    MTVertex& rkV0 = m_akVertex[iV0];
    MTVertex& rkV1 = m_akVertex[iV1];
    MTVertex& rkV2 = m_akVertex[iV2];
    MTEdge& rkE0 = m_akEdge[iE0];
    MTEdge& rkE1 = m_akEdge[iE1];
    MTEdge& rkE2 = m_akEdge[iE2];

    // attach edges to vertices
    rkV0.InsertEdge(iE2);
    rkV0.InsertEdge(iE0);
    rkV1.InsertEdge(iE0);
    rkV1.InsertEdge(iE1);
    rkV2.InsertEdge(iE1);
    rkV2.InsertEdge(iE2);
    rkE0.Vertex(0) = iV0;
    rkE0.Vertex(1) = iV1;
    rkE1.Vertex(0) = iV1;
    rkE1.Vertex(1) = iV2;
    rkE2.Vertex(0) = iV2;
    rkE2.Vertex(1) = iV0;

    // attach triangles to vertices
    rkV0.InsertTriangle(iT);
    rkV1.InsertTriangle(iT);
    rkV2.InsertTriangle(iT);
    rkT.Vertex(0) = iV0;
    rkT.Vertex(1) = iV1;
    rkT.Vertex(2) = iV2;

    // attach triangle to edges
    AttachTriangleToEdge(iT,rkT,0,iE0,rkE0);
    AttachTriangleToEdge(iT,rkT,1,iE1,rkE1);
    AttachTriangleToEdge(iT,rkT,2,iE2,rkE2);

    return true;
}
//----------------------------------------------------------------------------
int MTMesh::InsertVertex (int iLabel)
{
    MTIVertex kV(iLabel);
    int iV;

    VIter pkV = m_kVMap.find(kV);
    if ( pkV != m_kVMap.end() )
    {
        // vertex already exists
        iV = pkV->second;
    }
    else
    {
        // create new vertex
        iV = m_akVertex.Append(MTVertex(iLabel));
        m_kVMap.insert(make_pair(kV,iV));
    }

    return iV;
}
//----------------------------------------------------------------------------
int MTMesh::InsertEdge (int iLabel0, int iLabel1)
{
    MTIEdge kE(iLabel0,iLabel1);
    int iE;

    EIter pkE = m_kEMap.find(kE);
    if ( pkE != m_kEMap.end() )
    {
        // edge already exists
        iE = pkE->second;
    }
    else
    {
        // create new edge
        iE = m_akEdge.Append(MTEdge(m_iInitialELabel));
        m_kEMap.insert(make_pair(kE,iE));
    }

    return iE;
}
//----------------------------------------------------------------------------
int MTMesh::InsertTriangle (int iLabel0, int iLabel1, int iLabel2)
{
    MTITriangle kT(iLabel0,iLabel1,iLabel2);
    int iT;

    TIter pkT = m_kTMap.find(kT);
    if ( pkT != m_kTMap.end() )
    {
        // triangle already exists
        iT = -1;
    }
    else
    {
        // create new triangle
        iT = m_akTriangle.Append(MTTriangle(m_iInitialTLabel));
        m_kTMap.insert(make_pair(kT,iT));
    }

    return iT;
}
//----------------------------------------------------------------------------
bool MTMesh::Remove (int iLabel0, int iLabel1, int iLabel2)
{
    MTITriangle kT(iLabel0,iLabel1,iLabel2);
    TIter pkT = m_kTMap.find(kT);
    if ( pkT == m_kTMap.end() )
    {
        // triangle does not exist
        return false;
    }
    int iT = pkT->second;

    MTTriangle& rkT = m_akTriangle[iT];

    // detach triangle from edges
    int iE0 = rkT.Edge(0), iE1 = rkT.Edge(1), iE2 = rkT.Edge(2);
    MTEdge& rkE0 = m_akEdge[iE0];
    MTEdge& rkE1 = m_akEdge[iE1];
    MTEdge& rkE2 = m_akEdge[iE2];
    DetachTriangleFromEdge(iT,rkT,0,iE0,rkE0);
    DetachTriangleFromEdge(iT,rkT,1,iE1,rkE1);
    DetachTriangleFromEdge(iT,rkT,2,iE2,rkE2);

    // detach triangles from vertices
    int iV0 = rkT.Vertex(0);
    MTVertex& rkV0 = m_akVertex[iV0];
    rkV0.RemoveTriangle(iT);

    int iV1 = rkT.Vertex(1);
    MTVertex& rkV1 = m_akVertex[iV1];
    rkV1.RemoveTriangle(iT);

    int iV2 = rkT.Vertex(2);
    MTVertex& rkV2 = m_akVertex[iV2];
    rkV2.RemoveTriangle(iT);

    // detach edges from vertices (only if last edge to reference vertex)
    bool bE0Destroy = (rkE0.Triangle(0) == -1);
    if ( bE0Destroy )
    {
        rkV0.RemoveEdge(iE0);
        rkV1.RemoveEdge(iE0);
    }

    bool bE1Destroy = (rkE1.Triangle(0) == -1);
    if ( bE1Destroy )
    {
        rkV1.RemoveEdge(iE1);
        rkV2.RemoveEdge(iE1);
    }

    bool bE2Destroy = (rkE2.Triangle(0) == -1);
    if ( bE2Destroy )
    {
        rkV0.RemoveEdge(iE2);
        rkV2.RemoveEdge(iE2);
    }

    // Removal of components from the sets and maps starts here.  Be careful
    // using set indices, component references, and map iterators since
    // deletion has side effects.  Deletion of a component might cause another
    // component to be moved within the corresponding set or map.
    bool bV0Destroy = (rkV0.GetEdgeQuantity() == 0);
    bool bV1Destroy = (rkV1.GetEdgeQuantity() == 0);
    bool bV2Destroy = (rkV2.GetEdgeQuantity() == 0);

    // remove edges if no longer used
    if ( bE0Destroy )
        RemoveEdge(iLabel0,iLabel1);

    if ( bE1Destroy )
        RemoveEdge(iLabel1,iLabel2);

    if ( bE2Destroy )
        RemoveEdge(iLabel2,iLabel0);

    // remove vertices if no longer used
    if ( bV0Destroy )
        RemoveVertex(iLabel0);

    if ( bV1Destroy )
        RemoveVertex(iLabel1);

    if ( bV2Destroy )
        RemoveVertex(iLabel2);

    // remove triangle (definitely no longer used)
    RemoveTriangle(iLabel0,iLabel1,iLabel2);
    return true;
}
//----------------------------------------------------------------------------
void MTMesh::RemoveVertex (int iLabel)
{
    // get array location of vertex
    VIter pkV = m_kVMap.find(MTIVertex(iLabel));
    assert( pkV != m_kVMap.end() );
    int iV = pkV->second;

    // remove the vertex from the array and from the map
    int iVOld, iVNew;
    m_akVertex.RemoveAt(iV,&iVOld,&iVNew);
    m_kVMap.erase(pkV);

    if ( iVNew >= 0 )
    {
        // The vertex at the end of the array moved into the slot vacated by
        // the deleted vertex.  Update all the components sharing the moved
        // vertex.
        MTVertex& rkV = m_akVertex[iVNew];
        int i;

        // inform edges about location change
        for (i = 0; i < rkV.GetEdgeQuantity(); i++)
        {
            MTEdge& rkE = m_akEdge[rkV.GetEdge(i)];
            rkE.ReplaceVertex(iVOld,iVNew);
        }

        // inform triangles about location change
        for (i = 0; i < rkV.GetTriangleQuantity(); i++)
        {
            MTTriangle& rkT = m_akTriangle[rkV.GetTriangle(i)];
            rkT.ReplaceVertex(iVOld,iVNew);
        }

        pkV = m_kVMap.find(MTIVertex(rkV.GetLabel()));
        assert( pkV != m_kVMap.end() );
        pkV->second = iVNew;
    }
}
//----------------------------------------------------------------------------
void MTMesh::RemoveEdge (int iLabel0, int iLabel1)
{
    // get array location of edge
    EIter pkE = m_kEMap.find(MTIEdge(iLabel0,iLabel1));
    assert( pkE != m_kEMap.end() );
    int iE = pkE->second;

    // remove the edge from the array and from the map
    int iEOld, iENew;
    m_akEdge.RemoveAt(iE,&iEOld,&iENew);
    m_kEMap.erase(pkE);

    if ( iENew >= 0 )
    {
        // The edge at the end of the array moved into the slot vacated by
        // the deleted edge.  Update all the components sharing the moved
        // edge.
        MTEdge& rkE = m_akEdge[iENew];

        // inform vertices about location change
        MTVertex& rkV0 = m_akVertex[rkE.Vertex(0)];
        MTVertex& rkV1 = m_akVertex[rkE.Vertex(1)];
        rkV0.ReplaceEdge(iEOld,iENew);
        rkV1.ReplaceEdge(iEOld,iENew);

        // inform triangles about location change
        for (int i = 0; i < 2; i++)
        {
            int iT = rkE.GetTriangle(i);
            if ( iT != -1 )
            {
                MTTriangle& rkT = m_akTriangle[iT];
                rkT.ReplaceEdge(iEOld,iENew);
            }
        }

        pkE = m_kEMap.find(MTIEdge(rkV0.GetLabel(),rkV1.GetLabel()));
        assert( pkE != m_kEMap.end() );
        pkE->second = iENew;
    }
}
//----------------------------------------------------------------------------
void MTMesh::RemoveTriangle (int iLabel0, int iLabel1, int iLabel2)
{
    // get array location of triangle
    TIter pkT = m_kTMap.find(MTITriangle(iLabel0,iLabel1,iLabel2));
    assert( pkT != m_kTMap.end() );
    int iT = pkT->second;

    // remove the triangle from the array and from the map
    int iTOld, iTNew;
    m_akTriangle.RemoveAt(iT,&iTOld,&iTNew);
    m_kTMap.erase(pkT);

    if ( iTNew >= 0 )
    {
        // The triangle at the end of the array moved into the slot vacated by
        // the deleted triangle.  Update all the components sharing the moved
        // triangle.
        MTTriangle& rkT = m_akTriangle[iTNew];

        // inform vertices about location change
        MTVertex& rkV0 = m_akVertex[rkT.Vertex(0)];
        MTVertex& rkV1 = m_akVertex[rkT.Vertex(1)];
        MTVertex& rkV2 = m_akVertex[rkT.Vertex(2)];
        rkV0.ReplaceTriangle(iTOld,iTNew);
        rkV1.ReplaceTriangle(iTOld,iTNew);
        rkV2.ReplaceTriangle(iTOld,iTNew);

        // inform edges about location change
        int i;
        for (i = 0; i < 3; i++)
        {
            MTEdge& rkE = m_akEdge[rkT.GetEdge(i)];
            rkE.ReplaceTriangle(iTOld,iTNew);
        }

        // inform adjacents about location change
        for (i = 0; i < 3; i++)
        {
            int iA = rkT.GetAdjacent(i);
            if ( iA != -1 )
            {
                MTTriangle& rkA = m_akTriangle[iA];
                rkA.ReplaceAdjacent(iTOld,iTNew);
            }
        }

        pkT = m_kTMap.find(MTITriangle(rkV0.GetLabel(),rkV1.GetLabel(),
            rkV2.GetLabel()));
        assert( pkT != m_kTMap.end() );
        pkT->second = iTNew;
    }
}
//----------------------------------------------------------------------------
void MTMesh::AttachTriangleToEdge (int iT, MTTriangle& rkT, int i, int iE,
    MTEdge& rkE)
{
    if ( rkE.Triangle(0) == -1 )
    {
        rkE.Triangle(0) = iT;
    }
    else
    {
        int iTAdj = rkE.Triangle(0);
        MTTriangle& rkTAdj = m_akTriangle[iTAdj];
        rkT.Adjacent(i) = iTAdj;
        for (int j = 0; j < 3; j++)
        {
            if ( rkTAdj.Edge(j) == iE )
            {
                rkTAdj.Adjacent(j) = iT;
                break;
            }
        }

        if ( rkE.Triangle(1) == -1 )
        {
            rkE.Triangle(1) = iT;
        }
        else
        {
            // mesh is not manifold
            assert( false );
        }
    }

    rkT.Edge(i) = iE;
}
//----------------------------------------------------------------------------
void MTMesh::DetachTriangleFromEdge (int iT, MTTriangle& rkT, int i, int iE,
    MTEdge& rkE)
{
    // This function leaves T only partially complete.  The edge E is no
    // longer referenced by T, even though the vertices of T reference the
    // end points of E.  If T has an adjacent triangle A that shares E, then
    // A is a complete triangle.

    if ( rkE.Triangle(0) == iT )
    {
        int iTAdj = rkE.Triangle(1);
        if ( iTAdj != -1 )
        {
            // T and TAdj share E, update adjacency information for both
            MTTriangle& rkTAdj = m_akTriangle[iTAdj];
            for (int j = 0; j < 3; j++)
            {
                if ( rkTAdj.Edge(j) == iE )
                {
                    rkTAdj.Adjacent(j) = -1;
                    break;
                }
            }
        }
        rkE.Triangle(0) = iTAdj;
    }
    else if ( rkE.Triangle(1) == iT )
    {
        // T and TAdj share E, update adjacency information for both
        MTTriangle& rkTAdj = m_akTriangle[rkE.Triangle(0)];
        for (int j = 0; j < 3; j++)
        {
            if ( rkTAdj.Edge(j) == iE )
            {
                rkTAdj.Adjacent(j) = -1;
                break;
            }
        }
    }
    else
    {
        // Should not get here.  The specified edge must share the input
        // triangle.
        assert( false );
    }

    rkE.Triangle(1) = -1;
    rkT.Edge(i) = -1;
    rkT.Adjacent(i) = -1;
}
//----------------------------------------------------------------------------
bool MTMesh::SubdivideCentroid (int iLabel0, int iLabel1, int iLabel2,
    int& riNextLabel)
{
    int iT = T(iLabel0,iLabel1,iLabel2);
    if ( iT == -1 )
        return false;

    if ( m_kVMap.find(MTIVertex(riNextLabel)) != m_kVMap.end() )
    {
        // vertex already exists with this label
        return false;
    }

    // subdivide the triangle
    Remove(iLabel0,iLabel1,iLabel2);
    Insert(iLabel0,iLabel1,riNextLabel);
    Insert(iLabel1,iLabel2,riNextLabel);
    Insert(iLabel2,iLabel0,riNextLabel);

    riNextLabel++;
    return true;
}
//----------------------------------------------------------------------------
bool MTMesh::SubdivideCentroidAll (int& riNextLabel)
{
    // verify that the next-label range is valid
    int iT, iTMax = m_akTriangle.GetQuantity();
    int iTempLabel = riNextLabel;
    for (iT = 0; iT < iTMax; iT++, iTempLabel++)
    {
        if ( m_kVMap.find(MTIVertex(iTempLabel)) != m_kVMap.end() )
        {
            // vertex already exists with this label
            return false;
        }
    }

    // Care must be taken when processing the triangles iteratively.  The
    // side of effect of removing the first triangle is that the last triangle
    // in the array is moved into the vacated position.  The first problem is
    // that the moved triangle will be skipped in the iteration.  The second
    // problem is that the insertions cause the triangle array to grow.  To
    // avoid skipping the moved triangle, a different algorithm than the one
    // in SubdivideCentroid(int,int,int,int&) is used.  The triangle to be
    // removed is detached from two edges.  Two of the subtriangles are added
    // to the mesh.  The third subtriangle is calculated in the already
    // existing memory that stored the original triangle.  To avoid the
    // infinite recursion induced by a growing array, the original size of
    // the triangle array is stored int iTMax.  This guarantees that only the
    // original triangles are subdivided and that newly added triangles are
    // not.
    for (iT = 0; iT < iTMax; iT++, riNextLabel++)
    {
        // the triangle to subdivide
        MTTriangle& rkT = m_akTriangle[iT];
        int iLabel0 = GetVLabel(rkT.Vertex(0));
        int iLabel1 = GetVLabel(rkT.Vertex(1));
        int iLabel2 = GetVLabel(rkT.Vertex(2));

        // detach the triangle from two edges
        int iE1 = rkT.Edge(1), iE2 = rkT.Edge(2);
        MTEdge& rkE1 = m_akEdge[iE1];
        MTEdge& rkE2 = m_akEdge[iE2];
        DetachTriangleFromEdge(iT,rkT,1,iE1,rkE1);
        DetachTriangleFromEdge(iT,rkT,2,iE2,rkE2);

        // Insert the two subtriangles that share edges E1 and E2.  A
        // potential side effect is that the triangle array is reallocated
        // to make room for the new triangles.  This will invalidate the
        // reference rkT from the code above, but the index iT into the array
        // is still correct.  A reallocation of the vertex array might also
        // occur.
        Insert(iLabel1,iLabel2,riNextLabel);
        Insert(iLabel2,iLabel0,riNextLabel);

        // stitch the third subtriangle to the other subtriangles.
        MTTriangle& rkTN = m_akTriangle[iT];
        int iSubE1 = E(iLabel1,riNextLabel);
        int iSubE2 = E(iLabel0,riNextLabel);
        MTEdge& rkSubE1 = m_akEdge[iSubE1];
        MTEdge& rkSubE2 = m_akEdge[iSubE2];
        AttachTriangleToEdge(iT,rkTN,1,iSubE1,rkSubE1);
        AttachTriangleToEdge(iT,rkTN,2,iSubE2,rkSubE2);
    }

    return true;
}
//----------------------------------------------------------------------------
bool MTMesh::SubdivideEdge (int iLabel0, int iLabel1, int& riNextLabel)
{
    int iE = E(iLabel0,iLabel1);
    if ( iE == -1 )
        return false;

    if ( m_kVMap.find(MTIVertex(riNextLabel)) != m_kVMap.end() )
    {
        // vertex already exists with this label
        return false;
    }

    // split the triangles sharing the edge
    MTEdge& rkE = m_akEdge[iE];
    int iT0 = rkE.Triangle(0), iT1 = rkE.Triangle(1);
    int iT0L0, iT0L1, iT0L2, iT1L0, iT1L1, iT1L2;
    int iT0E0, iT0E1, iT1E0, iT1E1;
    if ( iT0 >= 0 && iT1 == -1 )
    {
        // edge shared by only T0
        MTTriangle& rkT0 = m_akTriangle[iT0];
        iT0L0 = GetVLabel(rkT0.Vertex(0));
        iT0L1 = GetVLabel(rkT0.Vertex(1));
        iT0L2 = GetVLabel(rkT0.Vertex(2));
        iT0E0 = rkT0.Edge(0);
        iT0E1 = rkT0.Edge(1);

        Remove(iT0L0,iT0L1,iT0L2);
        if ( iT0E0 == iE )
        {
            Insert(iT0L0,riNextLabel,iT0L2);
            Insert(riNextLabel,iT0L1,iT0L2);
        }
        else if ( iT0E1 == iE )
        {
            Insert(iT0L1,riNextLabel,iT0L0);
            Insert(riNextLabel,iT0L2,iT0L0);
        }
        else
        {
            Insert(iT0L2,riNextLabel,iT0L1);
            Insert(riNextLabel,iT0L0,iT0L1);
        }
    }
    else if ( iT1 >= 0 && iT0 == -1 )
    {
        // Edge shared by only T1.  The Remove(int,int,int) call is not
        // factored outside the conditional statements to avoid potential
        // reallocation side effects that would invalidate the reference rkT1.
        MTTriangle& rkT1 = m_akTriangle[iT1];
        iT1L0 = GetVLabel(rkT1.Vertex(0));
        iT1L1 = GetVLabel(rkT1.Vertex(1));
        iT1L2 = GetVLabel(rkT1.Vertex(2));
        iT1E0 = rkT1.Edge(0);
        iT1E1 = rkT1.Edge(1);

        Remove(iT1L0,iT1L1,iT1L2);
        if ( iT1E0 == iE )
        {
            Insert(iT1L0,riNextLabel,iT1L2);
            Insert(riNextLabel,iT1L1,iT1L2);
        }
        else if ( iT1E1 == iE )
        {
            Insert(iT1L1,riNextLabel,iT1L0);
            Insert(riNextLabel,iT1L2,iT1L0);
        }
        else
        {
            Insert(iT1L2,riNextLabel,iT1L1);
            Insert(riNextLabel,iT1L0,iT1L1);
        }
    }
    else
    {
        assert( iT0 >= 0 && iT1 >= 0 );

        // Edge shared by both T0 and T1.  The Remove(int,int,int) call is not
        // factored outside the conditional statements to avoid potential
        // reallocation side effects that would invalidate the references
        // rkT0 and rkT1.
        MTTriangle& rkT0 = m_akTriangle[iT0];
        iT0L0 = GetVLabel(rkT0.Vertex(0));
        iT0L1 = GetVLabel(rkT0.Vertex(1));
        iT0L2 = GetVLabel(rkT0.Vertex(2));
        iT0E0 = rkT0.Edge(0);
        iT0E1 = rkT0.Edge(1);

        MTTriangle& rkT1 = m_akTriangle[iT1];
        iT1L0 = GetVLabel(rkT1.Vertex(0));
        iT1L1 = GetVLabel(rkT1.Vertex(1));
        iT1L2 = GetVLabel(rkT1.Vertex(2));
        iT1E0 = rkT1.Edge(0);
        iT1E1 = rkT1.Edge(1);

        // Both triangles must be removed before the insertions to guarantee
        // that the common edge is deleted from the mesh first.
        Remove(iT0L0,iT0L1,iT0L2);
        Remove(iT1L0,iT1L1,iT1L2);

        if ( iT0E0 == iE )
        {
            Insert(iT0L0,riNextLabel,iT0L2);
            Insert(riNextLabel,iT0L1,iT0L2);
        }
        else if ( iT0E1 == iE )
        {
            Insert(iT0L1,riNextLabel,iT0L0);
            Insert(riNextLabel,iT0L2,iT0L0);
        }
        else
        {
            Insert(iT0L2,riNextLabel,iT0L1);
            Insert(riNextLabel,iT0L0,iT0L1);
        }

        if ( iT1E0 == iE )
        {
            Insert(iT1L0,riNextLabel,iT1L2);
            Insert(riNextLabel,iT1L1,iT1L2);
        }
        else if ( iT1E1 == iE )
        {
            Insert(iT1L1,riNextLabel,iT1L0);
            Insert(riNextLabel,iT1L2,iT1L0);
        }
        else
        {
            Insert(iT1L2,riNextLabel,iT1L1);
            Insert(riNextLabel,iT1L0,iT1L1);
        }
    }

    riNextLabel++;
    return true;
}
//----------------------------------------------------------------------------
void MTMesh::Print (ofstream& rkOStr) const
{
    int i;

    // print vertices
    rkOStr << "vertex quantity = " << m_akVertex.GetQuantity() << endl;
    for (int iV = 0; iV < m_akVertex.GetQuantity(); iV++)
    {
        const MTVertex& rkV = m_akVertex.Get(iV);

        rkOStr << "vertex<" << iV << ">" << endl;
        rkOStr << "    l: " << rkV.GetLabel() << endl;
        rkOStr << "    e: ";
        for (i = 0; i < rkV.GetEdgeQuantity(); i++)
            rkOStr << rkV.GetEdge(i) << ' ';
        rkOStr << endl;
        rkOStr << "    t: ";
        for (i = 0; i < rkV.GetTriangleQuantity(); i++)
            rkOStr << rkV.GetTriangle(i) << ' ';
        rkOStr << endl;
    }
    rkOStr << endl;

    // print edges
    rkOStr << "edge quantity = " << m_akEdge.GetQuantity() << endl;
    for (int iE = 0; iE < m_akEdge.GetQuantity(); iE++)
    {
        const MTEdge& rkE = m_akEdge.Get(iE);

        rkOStr << "edge<" << iE << ">" << endl;
        rkOStr << "    v: "
              << rkE.GetVertex(0) << ' '
              << rkE.GetVertex(1) << endl;
        rkOStr << "    t: " << rkE.GetTriangle(0) << ' ' << rkE.GetTriangle(1)
              << endl;
    }
    rkOStr << endl;

    // print triangles
    rkOStr << "triangle quantity = " << m_akTriangle.GetQuantity() << endl;
    for (int iT = 0; iT < m_akTriangle.GetQuantity(); iT++)
    {
        const MTTriangle& rkT = m_akTriangle.Get(iT);

        rkOStr << "triangle<" << iT << ">" << endl;
        rkOStr << "    v: "
              << rkT.GetVertex(0) << ' '
              << rkT.GetVertex(1) << ' '
              << rkT.GetVertex(2) << endl;
        rkOStr << "    e: " << rkT.GetEdge(0) << ' ' << rkT.GetEdge(1)
              << ' ' << rkT.GetEdge(2) << endl;
        rkOStr << "    a: " << rkT.GetAdjacent(0) << ' ' << rkT.GetAdjacent(1)
              << ' ' << rkT.GetAdjacent(2) << endl;
    }
    rkOStr << endl;
}
//----------------------------------------------------------------------------
bool MTMesh::Print (const char* acFilename) const
{
    ofstream kOStr(acFilename);
    if ( !kOStr )
        return false;

    Print(kOStr);
    return true;
}
//----------------------------------------------------------------------------

