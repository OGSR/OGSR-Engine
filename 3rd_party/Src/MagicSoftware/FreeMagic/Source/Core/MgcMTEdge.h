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

#ifndef MGCMTEDGE_H
#define MGCMTEDGE_H

#include "MagicFMLibType.h"

namespace Mgc {

class MAGICFM MTEdge
{
public:
    MTEdge (int iLabel = -1);
    MTEdge (const MTEdge& rkE);
    virtual ~MTEdge ();

    MTEdge& operator= (const MTEdge& rkE);

    int GetLabel () const;
    int& Label ();

    int GetVertex (int i) const;
    int& Vertex (int i);
    bool ReplaceVertex (int iVOld, int iVNew);

    int GetTriangle (int i) const;
    int& Triangle (int i);
    bool ReplaceTriangle (int iTOld, int iTNew);

    bool operator== (const MTEdge& rkE) const;

protected:
    int m_iLabel;
    int m_aiVertex[2];
    int m_aiTriangle[2];
};

#include "MgcMTEdge.inl"

} // namespace Mgc

#endif
