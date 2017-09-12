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

#ifndef MGCBINARY3D_H
#define MGCBINARY3D_H

#include "MgcImages.h"
#include <vector>

namespace Mgc {


class MAGICFM Binary3D : public ImageInt3D
{
public:
    // Construction and destruction.  Binary3D accepts responsibility for
    // deleting the input data array.
    Binary3D (int iXBound, int iYBound, int iZBound, Eint* atData = 0);
    Binary3D (const Binary3D& rkImage);
    Binary3D (const char* acFilename);

    // Compute the connected components of a binary image.  The components in
    // the returned image are labeled with positive integer values.  If the
    // image is identically zero, then the components image is identically
    // zero and the returned quantity is zero.

    void GetComponents (int& riQuantity, ImageInt3D& rkComponents) const;

protected:
    // helper for component labeling
    void AddToAssociative (int i0, int i1, int* aiAssoc) const;
};

} // namespace Mgc

#endif


