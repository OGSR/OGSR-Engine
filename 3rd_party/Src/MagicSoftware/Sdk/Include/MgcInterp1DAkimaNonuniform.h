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

#ifndef MGCINTERP1DAKIMANONUNIFORM_H
#define MGCINTERP1DAKIMANONUNIFORM_H

#include "MgcInterp1DAkima.h"

namespace Mgc {


class MAGICFM Interp1DAkimaNonuniform : public Interp1DAkima
{
public:
    // Construction and destruction.  Interp1DAkimaNonuniform does not
    // accept responsibility for deleting the input arrays.  The application
    // must do so.  The interpolator is for arbitrarily spaced x-values.
    Interp1DAkimaNonuniform (int iQuantity, Real* afX, Real* afF);
    virtual ~Interp1DAkimaNonuniform ();

    const Real* GetX () const;
    virtual Real GetXMin () const;
    virtual Real GetXMax () const;

protected:
    virtual bool Lookup (Real fX, int& riIndex, Real& rfDX) const;

    Real* m_afX;
};

#include "MgcInterp1DAkimaNonuniform.inl"

} // namespace Mgc

#endif


