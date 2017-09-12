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

#ifndef MGCELLIPSE2_H
#define MGCELLIPSE2_H

#include "MagicFMLibType.h"
#include "MgcMatrix2.h"

namespace Mgc {


class MAGICFM EllipseStandard2
{
public:
    // (x/a)^2 + (y/b)^2 = 1

    EllipseStandard2 ();

    Real& Extent (int i);
    const Real& Extent (int i) const;
    Real* Extents ();
    const Real* Extents () const;

protected:
    Real m_afExtent[2];
};


class MAGICFM Ellipse2
{
public:
    // center-matrix form, (X-C)^T A (X-C) = 1, where A is a positive
    // definite matrix

    Ellipse2 ();

    Vector2& Center ();
    const Vector2& Center () const;

    Matrix2& A ();
    const Matrix2& A () const;

protected:
    Vector2 m_kCenter;
    Matrix2 m_kA;
};

#include "MgcEllipse2.inl"

} // namespace Mgc

#endif


