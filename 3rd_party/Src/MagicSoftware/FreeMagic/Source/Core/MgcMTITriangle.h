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

#ifndef MGCMTITRIANGLE_H
#define MGCMTITRIANGLE_H

#include "MagicFMLibType.h"
#include <cassert>

namespace Mgc {

class MAGICFM MTITriangle
{
public:
    MTITriangle (int iLabel0, int iLabel1, int iLabel2);
    bool operator< (const MTITriangle& rkT) const;
    bool operator== (const MTITriangle& rkT) const;
    bool operator!= (const MTITriangle& rkT) const;

    int GetLabel (int i) const;

protected:
    int m_aiLabel[3];
};

#include "MgcMTITriangle.inl"

} // namespace Mgc

#endif

