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

#ifndef MGCINTERP1DAKIMAUNIFORM_H
#define MGCINTERP1DAKIMAUNIFORM_H

#include "MgcInterp1DAkima.h"

namespace Mgc {


class MAGICFM Interp1DAkimaUniform : public Interp1DAkima
{
public:
    // Construction and destruction.  Interp1DAkimaUniform accepts
    // responsibility for deleting the input array.  The interpolator is for
    // uniformly spaced x-values.
    Interp1DAkimaUniform (int iQuantity, Real fXMin, Real fXSpacing,
        Real* afF);

    virtual ~Interp1DAkimaUniform ();

    virtual Real GetXMin () const;
    virtual Real GetXMax () const;
    Real GetXSpacing () const;

protected:
    virtual bool Lookup (Real fX, int& riIndex, Real& rfDX) const;

    Real m_fXMin, m_fXMax, m_fXSpacing;
};

#include "MgcInterp1DAkimaUniform.inl"

} // namespace Mgc

#endif


