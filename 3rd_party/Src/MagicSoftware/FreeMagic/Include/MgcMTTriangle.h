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

#ifndef MGCMTTRIANGLE_H
#define MGCMTTRIANGLE_H

#include "MagicFMLibType.h"

namespace Mgc {

class MAGICFM MTTriangle
{
public:
    MTTriangle (int iLabel = -1);
    MTTriangle (const MTTriangle& rkT);
    virtual ~MTTriangle ();

    MTTriangle& operator= (const MTTriangle& rkT);

    int GetLabel () const;
    int& Label ();

    int GetVertex (int i) const;
    int& Vertex (int i);
    bool ReplaceVertex (int iVOld, int iVNew);

    int GetEdge (int i) const;
    int& Edge (int i);
    bool ReplaceEdge (int iEOld, int iENew);

    int GetAdjacent (int i) const;
    int& Adjacent (int i);
    bool ReplaceAdjacent (int iAOld, int iANew);

    bool operator== (const MTTriangle& rkT) const;

protected:
    int m_iLabel;
    int m_aiVertex[3];
    int m_aiEdge[3];
    int m_aiAdjacent[3];
};

#include "MgcMTTriangle.inl"

} // namespace Mgc

#endif
