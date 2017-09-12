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

#ifndef VERTEXCOLLAPSE_H
#define VERTEXCOLLAPSE_H

#include "MgcVector3.h"
using namespace Mgc;


class VertexCollapse
{
public:
    VertexCollapse (int iQuantity, Vector3*& rakVertex, bool bClosed,
        int*& raiMap, int& riEQuantity, int*& raiEdge);

    ~VertexCollapse ();

protected:
    // construct all the records to go into the heap
    void InitializeHeap (int iVQuantity, Vector3* akVertex, bool bClosed);

    // Reorganize records to form a heap, an O(N log N) operation
    void BuildHeap ();

    // Remove the root of the heap, an O(log N) operation.  The weights of
    // the vertices affected by the removal are updated.  The returned value
    // is the index of the removed vertex.
    int RemoveRoot (Vector3* akVertex);

    // The weight of the heap element at index HIndex has changed.  The heap
    // array needs to be updated to correctly represent a heap.  The update is
    // an O(log N) operation.
    void Update (int iHIndex, float fWeight);

    // weight calculation for vertex triple <V[iM],V[iZ],V[iP]>
    float GetWeight (int iM, int iZ, int iP, Vector3* akVertex);

    // create data structures for the polyline
    void CollapseVertices (int iVQuantity, Vector3* akVertex,
        int* aiCollapse);
    void ComputeEdges (int iVQuantity, bool bClosed, int* aiCollapse,
        int* aiMap, int iEQuantity, int* aiEdge);

    void ReorderVertices (int iVQuantity, Vector3*& rakVertex,
        int* aiCollapse, int iEQuantity, int* aiEdge);


    // Binary tree for heap stored in an array.  If P is a parent index, then
    // the two child indices are C0 = 2*P+1 and C1 = 2*P+2.  If C is a child
    // index, then the parent is P = (C-1)/2.
    class Record
    {
    public:
        int m_iVIndex;      // index of the represented vertex
        float m_fWeight;  // weight associated with the vertex
        int m_iHIndex;      // index of this element in the heap array
        Record* m_pkLAdj;   // heap element for the left-adjacent vertex
        Record* m_pkRAdj;   // heap element for the right-adjacent vertex
    };

    int m_iHQuantity;
    Record* m_akRecord;
    Record** m_apkHeap;

    // for debugging
    bool IsValid (int iStart, int iFinal);
    bool IsValid ();
};

#endif


