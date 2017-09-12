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

#ifndef VERTEX3_H
#define VERTEX3_H

#include "MagicFMLibType.h"
#include <map>
#include <vector>

class Vertex3
{
public:
    Vertex3 (float fX = -1.0f, float fY = -1.0f, float fZ = -1.0f);
    bool operator< (const Vertex3& rkV) const;

    float x, y, z;
};

typedef std::vector<Vertex3> V3Array;
typedef std::map<Vertex3,int> V3Map;
typedef V3Map::iterator V3MapIterator;

#endif
