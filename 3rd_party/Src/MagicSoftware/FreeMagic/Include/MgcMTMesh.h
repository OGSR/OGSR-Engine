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

#ifndef MGCMTMESH_H
#define MGCMTMESH_H

// Manifold Triangle Mesh.  Each edge has 1 or 2 triangles sharing it.

#include "MgcMTIVertex.h"
#include "MgcMTIEdge.h"
#include "MgcMTITriangle.h"
#include "MgcMTVertex.h"
#include "MgcMTEdge.h"
#include "MgcMTTriangle.h"
#include <fstream>
#include <map>

namespace Mgc {

class MAGICFM MTMesh
{
public:
    MTMesh (int iVQuantity = 0, int iEQuantity = 0, int iTQuantity = 0);
    MTMesh (const MTMesh& rkMesh);
    virtual ~MTMesh ();

    void Reset (int iVQuantity = 0, int iEQuantity = 0, int iTQuantity = 0);
    MTMesh& operator= (const MTMesh& rkMesh);

    int GetVQuantity () const;
    int V (int iLabel) const;
    const MTVertex& GetVertex (int iVIndex) const;
    int GetVLabel (int iVIndex) const;

    int GetEQuantity () const;
    int E (int iLabel0, int iLabel1) const;
    const MTEdge& GetEdge (int iEIndex) const;
    int GetELabel (int iEIndex) const;
    int& ELabel (int iEIndex);

    int GetTQuantity () const;
    int T (int iLabel0, int iLabel1, int iLabel2) const;
    const MTTriangle& GetTriangle (int iTIndex) const;
    int GetTLabel (int iTIndex) const;
    int& TLabel (int iTIndex);

    int& InitialELabel ();
    int& InitialTLabel ();

    bool Insert (int iLabel0, int iLabel1, int iLabel2);
    bool Remove (int iLabel0, int iLabel1, int iLabel2);

    bool SubdivideCentroid (int iLabel0, int iLabel1, int iLabel2,
        int& riNextLabel);
    bool SubdivideCentroidAll (int& riNextLabel);

    bool SubdivideEdge (int iLabel0, int iLabel1, int& riNextLabel);

    virtual void Print (std::ofstream& rkOStr) const;
    virtual bool Print (const char* acFilename) const;

protected:
    void AttachTriangleToEdge (int iT, MTTriangle& rkT, int i, int iE,
        MTEdge& rkE);
    int InsertVertex (int iLabel);
    int InsertEdge (int iLabel0, int iLabel1);
    int InsertTriangle (int iLabel0, int iLabel1, int iLabel2);

    void DetachTriangleFromEdge (int iT, MTTriangle& rkT, int i, int iE,
        MTEdge& rkE);
    void RemoveVertex (int iLabel);
    void RemoveEdge (int iLabel0, int iLabel1);
    void RemoveTriangle (int iLabel0, int iLabel1, int iLabel2);

    TUnorderedSet<MTVertex> m_akVertex;
    TUnorderedSet<MTEdge> m_akEdge;
    TUnorderedSet<MTTriangle> m_akTriangle;

    int m_iInitialELabel;
    int m_iInitialTLabel;

    typedef std::map<MTIVertex,int> VMap;
    typedef std::map<MTIEdge,int> EMap;
    typedef std::map<MTITriangle,int> TMap;
    typedef VMap::iterator VIter;
    typedef EMap::iterator EIter;
    typedef TMap::iterator TIter;
    typedef VMap::const_iterator VCIter;
    typedef EMap::const_iterator ECIter;
    typedef TMap::const_iterator TCIter;

    VMap m_kVMap;
    EMap m_kEMap;
    TMap m_kTMap;
};

#include "MgcMTMesh.inl"

} // namespace Mgc

#endif
