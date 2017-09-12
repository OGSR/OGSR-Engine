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

#ifndef MGCRECTANGLE3_H
#define MGCRECTANGLE3_H

#include "MgcVector3.h"

namespace Mgc {


class MAGICFM Rectangle3
{
public:
    // Points are rct(s,t) = b+s*e0+t*e1 where Dot(e0,e1) = 0, 0 <= s <= 1,
    // and 0 <= t <= 1.

    Rectangle3 ();

    Vector3& Origin ();
    const Vector3& Origin () const;

    Vector3& Edge0 ();
    const Vector3& Edge0 () const;

    Vector3& Edge1 ();
    const Vector3& Edge1 () const;

protected:
    Vector3 m_kOrigin;
    Vector3 m_kEdge0;
    Vector3 m_kEdge1;
};

#include "MgcRectangle3.inl"

} // namespace Mgc

#endif


