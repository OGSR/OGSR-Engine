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

#ifndef MGCCOLORRGB_H
#define MGCCOLORRGB_H

#include "MagicFMLibType.h"
#include "MgcMath.h"

namespace Mgc {

class MAGICFM ColorRGB
{
public:
    // construction (components in range [0,1])
    ColorRGB ();
    ColorRGB (Real fR, Real fG, Real fB);
    ColorRGB (const ColorRGB& rkColor);

    // color channels
    Real r, g, b;

    // access color C as C[0] = C.r, C[1] = C.g, C[2] = C.r
    //
    // WARNING.  These member functions rely on
    // (1) ColorRGB not having virtual functions
    // (2) the data packed in a 3*sizeof(Real) memory block
    Real& operator[] (int i) const;
    operator Real* ();

    void Clamp ();
    void ScaleByMax ();

    ColorRGB& operator= (const ColorRGB& rkColor);
    bool operator== (const ColorRGB& rkColor) const;
    bool operator!= (const ColorRGB& rkColor) const;

    ColorRGB operator+ (const ColorRGB& rkColor) const;
    ColorRGB operator- (const ColorRGB& rkColor) const;
    ColorRGB operator* (const ColorRGB& rkColor) const;
    ColorRGB operator/ (const ColorRGB& rkColor) const;
    ColorRGB operator- () const;
    MAGICFM friend ColorRGB operator* (Real fScalar,
        const ColorRGB& rkColor);
    ColorRGB& operator+= (const ColorRGB& rkColor);
    ColorRGB& operator-= (const ColorRGB& rkColor);
    ColorRGB& operator*= (const ColorRGB& rkColor);
    ColorRGB& operator/= (const ColorRGB& rkColor);

    static const ColorRGB BLACK; // = (0,0,0) 
    static const ColorRGB WHITE; // = (1,1,1)
};

#include "MgcColorRGB.inl"

} // namespace Mgc

#endif


