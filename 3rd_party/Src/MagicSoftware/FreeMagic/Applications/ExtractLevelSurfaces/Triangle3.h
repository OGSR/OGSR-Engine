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

#ifndef TRIANGLE3_H
#define TRIANGLE3_H

#include "MagicFMLibType.h"
#include <map>
#include <vector>

class Triangle3
{
public:
    Triangle3 (int j0 = -1, int j1 = -1, int j2 = -1);
    bool operator< (const Triangle3& rkT) const;

    int i0, i1, i2;
};

typedef std::vector<Triangle3> T3Array;
typedef std::map<Triangle3,int> T3Map;
typedef T3Map::iterator T3MapIterator;

#endif
