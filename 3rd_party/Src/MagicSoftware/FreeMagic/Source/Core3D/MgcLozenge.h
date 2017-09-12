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

#ifndef MGCLOZENGE_H
#define MGCLOZENGE_H

#include "MgcRectangle3.h"

namespace Mgc {


class MAGICFM Lozenge
{
public:
    Lozenge ();

    Vector3& Origin ();
    const Vector3& Origin () const;

    Vector3& Edge0 ();
    const Vector3& Edge0 () const;

    Vector3& Edge1 ();
    const Vector3& Edge1 () const;

    Real& Radius ();
    const Real& Radius () const;

    Rectangle3& Rectangle ();
    const Rectangle3& Rectangle () const;

protected:
    Rectangle3 m_kRectangle;
    Real m_fRadius;
};

#include "MgcLozenge.inl"

} // namespace Mgc

#endif


