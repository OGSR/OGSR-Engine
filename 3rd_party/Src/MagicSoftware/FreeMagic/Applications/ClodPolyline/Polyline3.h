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

#ifndef POLYLINE3_H
#define POLYLINE3_H

#include "MgcVector3.h"
using namespace Mgc;


class Polyline3
{
public:
    // Constructor and destructor.  The quantity Q must be 2 or larger.
    // Polyline3 accepts responsibility for deleting the input array.  The
    // vertices are assumed to be ordered.  For an open polyline, the segments
    // are <V[i],V[i+1]> for 0 <= i <= Q-2.  If the polyline is closed, an
    // additional segment is <V[Q-1],V[0]>.
    Polyline3 (int iVQuantity, Vector3* akVertex, bool bClosed);
    ~Polyline3 ();

    // accessors to vertex data
    int GetVertexQuantity () const;
    const Vector3* GetVertices () const;
    bool GetClosed () const;

    // accessors to edge data
    int GetEdgeQuantity () const;
    const int* GetEdges () const;

    // accessors to level of detail  (MinLOD <= LOD <= MaxLOD is required)
    int GetMinLevelOfDetail () const;
    int GetMaxLevelOfDetail () const;
    void SetLevelOfDetail (int iLOD);
    int GetLevelOfDetail () const;

protected:
    // the polyline vertices
    int m_iVQuantity;
    Vector3* m_akVertex;
    bool m_bClosed;

    // the polyline edges
    int m_iEQuantity;
    int* m_aiEdge;

    // the level of detail information
    int m_iVMin, m_iVMax;
    int* m_aiMap;
};

#include "Polyline3.inl"

#endif


