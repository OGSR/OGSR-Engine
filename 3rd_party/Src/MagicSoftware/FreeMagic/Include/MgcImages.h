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

#ifndef MGCIMAGES_H
#define MGCIMAGES_H

#include "MgcElement.h"
#include "MgcTImage.h"
#include "MgcTImage2D.h"
#include "MgcTImage3D.h"

namespace Mgc {

typedef TImage<Echar>     ImageChar;
typedef TImage<Euchar>    ImageUChar;
typedef TImage<Eshort>    ImageShort;
typedef TImage<Eushort>   ImageUShort;
typedef TImage<Eint>      ImageInt;
typedef TImage<Euint>     ImageUInt;
typedef TImage<Elong>     ImageLong;
typedef TImage<Eulong>    ImageULong;
typedef TImage<Efloat>    ImageFloat;
typedef TImage<Edouble>   ImageDouble;
typedef TImage<Ergb5>     ImageRGB5;
typedef TImage<Ergb8>     ImageRGB8;

typedef TImage2D<Echar>   ImageChar2D;
typedef TImage2D<Euchar>  ImageUChar2D;
typedef TImage2D<Eshort>  ImageShort2D;
typedef TImage2D<Eushort> ImageUShort2D;
typedef TImage2D<Eint>    ImageInt2D;
typedef TImage2D<Euint>   ImageUInt2D;
typedef TImage2D<Elong>   ImageLong2D;
typedef TImage2D<Eulong>  ImageULong2D;
typedef TImage2D<Efloat>  ImageFloat2D;
typedef TImage2D<Edouble> ImageDouble2D;
typedef TImage2D<Ergb5>   ImageRGB52D;
typedef TImage2D<Ergb8>   ImageRGB82D;

typedef TImage3D<Echar>   ImageChar3D;
typedef TImage3D<Euchar>  ImageUChar3D;
typedef TImage3D<Eshort>  ImageShort3D;
typedef TImage3D<Eushort> ImageUShort3D;
typedef TImage3D<Eint>    ImageInt3D;
typedef TImage3D<Euint>   ImageUInt3D;
typedef TImage3D<Elong>   ImageLong3D;
typedef TImage3D<Eulong>  ImageULong3D;
typedef TImage3D<Efloat>  ImageFloat3D;
typedef TImage3D<Edouble> ImageDouble3D;
typedef TImage3D<Ergb5>   ImageRGB53D;
typedef TImage3D<Ergb8>   ImageRGB83D;

} // namespace Mgc

#endif


