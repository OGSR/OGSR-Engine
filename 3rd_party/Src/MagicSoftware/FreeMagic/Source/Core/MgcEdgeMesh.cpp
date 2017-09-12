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

#include "MgcEdgeMesh.h"
#include "MgcRTLib.h"
using namespace Mgc;

#include <fstream>
#include <stack>
using namespace std;

//----------------------------------------------------------------------------
EdgeMesh::EdgeMesh ()
{
}
//----------------------------------------------------------------------------
EdgeMesh::~EdgeMesh ()
{
}
//----------------------------------------------------------------------------
void EdgeMesh::InsertEdge (int iV0, int iV1)
{
    Edge kE(iV0,iV1);

    // insert edge
    pair<MEIter,bool> kRE = m_kEMap.insert(make_pair(kE,EdgeAttribute()));

    // insert vertices
    pair<MVIter,bool> kRV0 = m_kVMap.insert(make_pair(iV0,VertexAttribute()));
    kRV0.first->second.m_kESet.insert(kE);

    pair<MVIter,bool> kRV1 = m_kVMap.insert(make_pair(iV1,VertexAttribute()));
    kRV1.first->second.m_kESet.insert(kE);


    // Notify derived classes that mesh components have been inserted.  The
    // notification occurs here to make sure the derived classes have access
    // to the current state of the mesh after the edge insertion.

    OnVertexInsert(iV0,kRV0.second,kRV0.first->second.m_pvData);
    OnVertexInsert(iV1,kRV1.second,kRV1.first->second.m_pvData);
    OnEdgeInsert(kE,kRE.second,kRE.first->second.m_pvData);
}
//----------------------------------------------------------------------------
void EdgeMesh::InsertEdge (const Edge& rkE)
{
    InsertEdge(rkE.m_aiV[0],rkE.m_aiV[1]);
}
//----------------------------------------------------------------------------
void EdgeMesh::RemoveEdge (int iV0, int iV1)
{
    // remove edge
    Edge kE(iV0,iV1);
    MEIter pkE = m_kEMap.find(kE);
    if ( pkE == m_kEMap.end() )
    {
        // edge does not exist, nothing to do
        return;
    }

    // update vertices
    MVIter pkV0 = m_kVMap.find(iV0);
    assert( pkV0 != m_kVMap.end() );
    pkV0->second.m_kESet.erase(kE);

    MVIter pkV1 = m_kVMap.find(iV1);
    assert( pkV1 != m_kVMap.end() );
    pkV1->second.m_kESet.erase(kE);


    // Notify derived classes that mesh components are about to be destroyed.
    // The notification occurs here to make sure the derived classes have
    // access to the current state of the mesh before the edge removal.

    bool bDestroy = pkV0->second.m_kESet.size() == 0;
    OnVertexRemove(iV0,bDestroy,pkV0->second.m_pvData);
    if ( bDestroy )
        m_kVMap.erase(iV0);

    bDestroy = pkV1->second.m_kESet.size() == 0;
    OnVertexRemove(iV1,bDestroy,pkV1->second.m_pvData);
    if ( bDestroy )
        m_kVMap.erase(iV1);

    OnEdgeRemove(kE,true,pkE->second.m_pvData);
    m_kEMap.erase(kE);
}
//----------------------------------------------------------------------------
void EdgeMesh::RemoveEdge (const Edge& rkE)
{
    RemoveEdge(rkE.m_aiV[0],rkE.m_aiV[1]);
}
//----------------------------------------------------------------------------
void EdgeMesh::RemoveAllEdges ()
{
    MEIter pkE = m_kEMap.begin();
    while ( pkE != m_kEMap.end() )
    {
        int iV0 = pkE->first.m_aiV[0];
        int iV1 = pkE->first.m_aiV[1];
        pkE++;

        RemoveEdge(iV0,iV1);
    }
}
//----------------------------------------------------------------------------
void EdgeMesh::Print (const char* acFilename) const
{
    ofstream kOStr(acFilename);

    // print vertices
    kOStr << "vertex quantity = " << m_kVMap.size() << endl;
    for (MVCIter pkVM = m_kVMap.begin(); pkVM != m_kVMap.end(); pkVM++)
    {
        kOStr << "v<" << pkVM->first << "> : e ";

        SECIter pkES = pkVM->second.m_kESet.begin();
        while ( pkES != pkVM->second.m_kESet.end() )
        {
            kOStr << '<' << pkES->m_aiV[0] << ',' << pkES->m_aiV[1] << "> ";
            pkES++;
        }
        kOStr << endl;
    }
    kOStr << endl;

    // print edges
    kOStr << "edge quantity = " << m_kEMap.size() << endl;
    for (MECIter pkEM = m_kEMap.begin(); pkEM != m_kEMap.end(); pkEM++)
    {
        kOStr << "e<" << pkEM->first.m_aiV[0] << ',' << pkEM->first.m_aiV[1];
        kOStr << ">" << endl;
    }
    kOStr << endl;
}
//----------------------------------------------------------------------------
void* EdgeMesh::GetData (int iV)
{
    MVIter pkV = m_kVMap.find(iV);
    return ( pkV != m_kVMap.end() ? pkV->second.m_pvData : NULL );
}
//----------------------------------------------------------------------------
set<EdgeMesh::Edge> EdgeMesh::GetEdges (int iV)
{
    MVIter pkV = m_kVMap.find(iV);
    return ( pkV != m_kVMap.end() ? pkV->second.m_kESet : set<Edge>() );
}
//----------------------------------------------------------------------------
void* EdgeMesh::GetData (int iV0, int iV1)
{
    MEIter pkE = m_kEMap.find(Edge(iV0,iV1));
    return ( pkE != m_kEMap.end() ? pkE->second.m_pvData : NULL );
}
//----------------------------------------------------------------------------
void* EdgeMesh::GetData (const Edge& rkE)
{
    return GetData(rkE.m_aiV[0],rkE.m_aiV[1]);
}
//----------------------------------------------------------------------------
bool EdgeMesh::IsManifold () const
{
    for (MVCIter pkV = m_kVMap.begin(); pkV != m_kVMap.end(); pkV++)
    {
        if ( pkV->second.m_kESet.size() > 2 )
            return false;
    }
    return true;
}
//----------------------------------------------------------------------------
bool EdgeMesh::IsClosed () const
{
    for (MVCIter pkV = m_kVMap.begin(); pkV != m_kVMap.end(); pkV++)
    {
        if ( pkV->second.m_kESet.size() != 2 )
            return false;
    }
    return true;
}
//----------------------------------------------------------------------------
bool EdgeMesh::IsConnected () const
{
    // Do a depth-first search of the mesh.  It is connected if and only if
    // all of the edge are visited on a single search.

    int iESize = m_kEMap.size();
    if ( iESize == 0 )
        return true;

    // for marking visited edges during the traversal
    map<Edge,bool> kVisitedMap;
    MECIter pkE;
    for (pkE = m_kEMap.begin(); pkE != m_kEMap.end(); pkE++)
        kVisitedMap.insert(make_pair(pkE->first,false));

    // start the traversal at any edge in the mesh
    stack<Edge> kStack;
    kStack.push(m_kEMap.begin()->first);
    map<Edge,bool>::iterator pkVI = kVisitedMap.find(m_kEMap.begin()->first);
    assert( pkVI != kVisitedMap.end() );
    pkVI->second = true;
    iESize--;

    while ( !kStack.empty() )
    {
        // start at the current edge
        Edge kE = kStack.top();
        kStack.pop();

        for (int i = 0; i < 2; i++)
        {
            // get a vertex of the current edge
            MVCIter pkV = m_kVMap.find(kE.m_aiV[i]);

            // visit each adjacent edge
            SECIter pkEAdj = pkV->second.m_kESet.begin();
            while ( pkEAdj != pkV->second.m_kESet.end() )
            {
                pkVI = kVisitedMap.find(*pkEAdj);
                assert( pkVI != kVisitedMap.end() );
                if ( pkVI->second == false )
                {
                    // this adjacent edge not yet visited
                    kStack.push(*pkEAdj);
                    pkVI->second = true;
                    iESize--;
                }

                pkEAdj++;
            }
        }
    }

    return iESize == 0;
}
//----------------------------------------------------------------------------
void EdgeMesh::GetComponents (vector<EdgeMesh*>& rkComponents)
{
    // Do a depth-first search of the mesh to find connected components.
    int iESize = m_kEMap.size();
    if ( iESize == 0 )
        return;

    // for marking visited edges during the traversal
    map<Edge,bool> kVisitedMap;
    MECIter pkE;
    for (pkE = m_kEMap.begin(); pkE != m_kEMap.end(); pkE++)
        kVisitedMap.insert(make_pair(pkE->first,false));

    while ( iESize > 0 )
    {
        // find an unvisited edge in the mesh
        stack<Edge> kStack;
        map<Edge,bool>::iterator pkVI = kVisitedMap.begin();
        while ( pkVI != kVisitedMap.end() )
        {
            if ( pkVI->second == false )
            {
                kStack.push(pkVI->first);
                pkVI->second = true;
                iESize--;
                break;
            }
            pkVI++;
        }

        // traverse the connected component of the starting edge
        EdgeMesh* pkComponent = Create();
        while ( !kStack.empty() )
        {
            // start at the current edge
            Edge kE = kStack.top();
            kStack.pop();
            pkComponent->InsertEdge(kE);

            for (int i = 0; i < 2; i++)
            {
                // get a vertex of the current edge
                MVCIter pkV = m_kVMap.find(kE.m_aiV[i]);

                // visit each adjacent edge
                SECIter pkEAdj = pkV->second.m_kESet.begin();
                while ( pkEAdj != pkV->second.m_kESet.end() )
                {
                    pkVI = kVisitedMap.find(*pkEAdj);
                    assert( pkVI != kVisitedMap.end() );
                    if ( pkVI->second == false )
                    {
                        // this adjacent edge not yet visited
                        kStack.push(*pkEAdj);
                        pkVI->second = true;
                        iESize--;
                    }
                    pkEAdj++;
                }
            }
        }
        rkComponents.push_back(pkComponent);
    }
}
//----------------------------------------------------------------------------
bool EdgeMesh::GetConsistentComponents (vector<EdgeMesh*>& rkComponents)
{
    if ( !IsManifold() )
        return false;

    // Do a depth-first search of the mesh to find connected components.
    int iESize = m_kEMap.size();
    if ( iESize == 0 )
        return true;

    // for marking visited edges during the traversal
    map<Edge,bool> kVisitedMap;
    MECIter pkE;
    for (pkE = m_kEMap.begin(); pkE != m_kEMap.end(); pkE++)
        kVisitedMap.insert(make_pair(pkE->first,false));

    while ( iESize > 0 )
    {
        // Find an unvisited edge in the mesh.  Any edge pushed onto
        // the stack is considered to have a consistent ordering.
        stack<Edge> kStack;
        map<Edge,bool>::iterator pkVI = kVisitedMap.begin();
        while ( pkVI != kVisitedMap.end() )
        {
            if ( pkVI->second == false )
            {
                kStack.push(pkVI->first);
                pkVI->second = true;
                iESize--;
                break;
            }
            pkVI++;
        }

        // traverse the connected component of the starting edge
        EdgeMesh* pkComponent = Create();
        while ( !kStack.empty() )
        {
            // start at the current edge
            Edge kE = kStack.top();
            kStack.pop();
            pkComponent->InsertEdge(kE);

            for (int i = 0; i < 2; i++)
            {
                // get a vertex of the current edge
                int iV0 = kE.m_aiV[i], iV1;
                MVCIter pkV = m_kVMap.find(iV0);

                int iSize = pkV->second.m_kESet.size();
                assert( iSize == 1 || iSize == 2 );  // mesh is manifold
                SECIter pkEAdj = pkV->second.m_kESet.begin();
                if ( iSize == 2 )
                {
                    // get the adjacent edge to the current one
                    if ( *pkEAdj == kE )
                        pkEAdj++;

                    pkVI = kVisitedMap.find(*pkEAdj);
                    assert( pkVI != kVisitedMap.end() );
                    if ( pkVI->second == false )
                    {
                        // adjacent edge not yet visited
                        if ( pkEAdj->m_aiV[0] == iV0 )
                        {
                            // adjacent edge must be reordered
                            iV0 = pkEAdj->m_aiV[0];
                            iV1 = pkEAdj->m_aiV[1];
                            kVisitedMap.erase(*pkEAdj);
                            RemoveEdge(iV0,iV1);
                            InsertEdge(iV1,iV0);
                            kVisitedMap.insert(make_pair(Edge(iV1,iV0),
                                false));

                            // refresh the iterators since maps changed
                            pkV = m_kVMap.find(iV0);
                            pkEAdj = pkV->second.m_kESet.begin();
                            if ( *pkEAdj == kE )
                                pkEAdj++;
                            pkVI = kVisitedMap.find(*pkEAdj);
                            assert( pkVI != kVisitedMap.end() );
                        }

                        kStack.push(*pkEAdj);
                        pkVI->second = true;
                        iESize--;
                    }
                }
            }
        }
        rkComponents.push_back(pkComponent);
    }

    return true;
}
//----------------------------------------------------------------------------
EdgeMesh* EdgeMesh::GetReversedOrderMesh () const
{
    EdgeMesh* pkReversed = Create();

    for (MECIter pkE = m_kEMap.begin(); pkE != m_kEMap.end(); pkE++)
        pkReversed->InsertEdge(pkE->first.m_aiV[1],pkE->first.m_aiV[0]);

    return pkReversed;
}
//----------------------------------------------------------------------------


