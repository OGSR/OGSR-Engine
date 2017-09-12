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

#include "MgcImages.h"
using namespace Mgc;

// Instantiations to support dynamic link libraries.  This appears to be
// necessary on the SGI running IRIX and using the MIPSPRO CC compiler.

template class Mgc::TImage<Echar>;
template class Mgc::TImage<Euchar>;
template class Mgc::TImage<Eshort>;
template class Mgc::TImage<Eushort>;
template class Mgc::TImage<Eint>;
template class Mgc::TImage<Euint>;
template class Mgc::TImage<Elong>;
template class Mgc::TImage<Eulong>;
template class Mgc::TImage<Efloat>;
template class Mgc::TImage<Edouble>;
template class Mgc::TImage<Ergb5>;
template class Mgc::TImage<Ergb8>;

template class Mgc::TImage2D<Echar>;
template class Mgc::TImage2D<Euchar>;
template class Mgc::TImage2D<Eshort>;
template class Mgc::TImage2D<Eushort>;
template class Mgc::TImage2D<Eint>;
template class Mgc::TImage2D<Euint>;
template class Mgc::TImage2D<Elong>;
template class Mgc::TImage2D<Eulong>;
template class Mgc::TImage2D<Efloat>;
template class Mgc::TImage2D<Edouble>;
template class Mgc::TImage2D<Ergb5>;
template class Mgc::TImage2D<Ergb8>;

template class Mgc::TImage3D<Echar>;
template class Mgc::TImage3D<Euchar>;
template class Mgc::TImage3D<Eshort>;
template class Mgc::TImage3D<Eushort>;
template class Mgc::TImage3D<Eint>;
template class Mgc::TImage3D<Euint>;
template class Mgc::TImage3D<Elong>;
template class Mgc::TImage3D<Eulong>;
template class Mgc::TImage3D<Efloat>;
template class Mgc::TImage3D<Edouble>;
template class Mgc::TImage3D<Ergb5>;
template class Mgc::TImage3D<Ergb8>;


