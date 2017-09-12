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

#ifndef MGCMTVERTEX_H
#define MGCMTVERTEX_H

#include "MagicFMLibType.h"
#include "MgcTUnorderedSet.h"

namespace Mgc {

class MAGICFM MTVertex
{
public:
    MTVertex (int iLabel = -1, int iEGrow = 0, int iTGrow = 0);
    MTVertex (const MTVertex& rkV);
    virtual ~MTVertex ();

    MTVertex& operator= (const MTVertex& rkV);

    // Vertex labels are read-only since they are used for maps in the MTMesh
    // class for inverse look-up.
    int GetLabel () const;

    int GetEdgeQuantity () const;
    int GetEdge (int i) const;
    bool InsertEdge (int iE);
    bool RemoveEdge (int iE);
    bool ReplaceEdge (int iEOld, int iENew);

    int GetTriangleQuantity () const;
    int GetTriangle (int i) const;
    bool InsertTriangle (int iT);
    bool RemoveTriangle (int iT);
    bool ReplaceTriangle(int iTOld, int iTNew);

    bool operator== (const MTVertex& rkV) const;

protected:
    int m_iLabel;
    TUnorderedSet<int> m_kESet, m_kTSet;
};

#include "MgcMTVertex.inl"

} // namespace Mgc

#endif
