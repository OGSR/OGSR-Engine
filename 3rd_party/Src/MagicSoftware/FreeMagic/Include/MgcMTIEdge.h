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

#ifndef MGCMTIEDGE_H
#define MGCMTIEDGE_H

#include "MagicFMLibType.h"
#include <cassert>

namespace Mgc {

class MAGICFM MTIEdge
{
public:
    MTIEdge (int iLabel0, int iLabel1);
    bool operator< (const MTIEdge& rkE) const;
    bool operator== (const MTIEdge& rkE) const;
    bool operator!= (const MTIEdge& rkE) const;

    int GetLabel (int i) const;

protected:
    int m_aiLabel[2];
};

#include "MgcMTIEdge.inl"

} // namespace Mgc

#endif

