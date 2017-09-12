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

#ifndef MGCBINARY2D_H
#define MGCBINARY2D_H

#include "MgcImages.h"
#include "MgcMath.h"
#include <vector>

namespace Mgc {


class MAGICFM Binary2D : public ImageInt2D
{
public:
    // Construction and destruction.  Binary2D accepts responsibility for
    // deleting the input data array.
    Binary2D (int iXBound, int iYBound, Eint* atData = 0);
    Binary2D (const Binary2D& rkImage);
    Binary2D (const char* acFilename);

    // Extract boundaries from blobs in a binary image.  The input image is
    // treated as binary in the sense that pixels are either zero or not zero.
    // The blobs correspond to the non-zero values.  The boundaries are
    // treated as 8-connected (neighbors can occur in any of 8 directions from
    // the given pixel).  The blob interiors are treated as 4-connected.
    //
    // For sake of naming, let the returned array be called Boundary.  If the
    // image is identically zero, there are no blobs, so Boundary = null.  If
    // there are blobs, the boundaries are packed into Boundary as follows.
    //
    // Boundary[0] is the number of blobs (connected components).  Boundary[1]
    // is the number of pixels in the first boundary.  Boundary[2] through
    // Boundary[Boundary[1]+1] are the pixel locations stored as unsigned
    // integers (see below).  The next boundary (if any) is stored in the
    // same format:  number of pixel locations followed by the array of pixel
    // locations.
    //
    // The pixel locations are stored as unsigned integers.  If I is the index
    // and (x,y) is the pixel location, then I = x + XDIM*y where the image is
    // XDIM-by-YDIM.  You can use rkImage.GetCoordinates(I,x,y) to extract the
    // (x,y) values for a given I.
    //
    // The application is responsible for deleting the returned array.
    int* GetBoundaries () const;


    // Compute the connected components of a binary image.  The components in
    // the returned image are labeled with positive integer values.  If the
    // image is identically zero, then the components image is identically
    // zero and the returned quantity is zero.
    void GetComponents (int& riQuantity, ImageInt2D& rkComponents) const;


    // Compute the L1 distance transform.  Given a pixel (x,y), the neighbors
    // (x+1,y), (x-1,y), (x,y+1), and (x,y-1) are 1 unit of distance from
    // (x,y).  The neighbors (x+1,y+1), (x+1,y-1), (x-1,y+1), and (x-1,y-1)
    // are 2 units of distance from (x,y).
    void GetL1Distance (int& riMaxDistance, ImageInt2D& rkTransform) const;

    // Compute the L2 distance transform (Euclidean distance transform).  The
    // distances are exact as long as they are smaller than 100 (see the
    // comments in the source code).
    void GetL2Distance (Real& rfMaxDistance,
        ImageDouble2D& rkTransform) const;

    // Compute a skeleton of the image.  Pixels are trimmed from outside to
    // inside using L1 distance.  Connectivity and cycles of the original
    // blobs are preserved.
    void GetSkeleton (ImageInt2D& rkSkeleton) const;

protected:
    // helper for boundary extraction
    typedef std::vector<int> BoundaryList;
    BoundaryList* ExtractBoundary (int iX0, int iY0,
        ImageInt2D& rkTemp) const;

    // helper for component labeling
    void AddToAssociative (int i0, int i1, int* aiAssoc) const;

    // helper for L1 distance (no embedding in larger image, done in-place)
    void GetL1DistanceZeroBoundary (int& riMaxDistance,
        ImageInt2D& rkTemp) const;

    // helpers for L2 distance
    void L2Initialize (ImageInt2D& rkXNear, ImageInt2D& rkYNear,
        ImageInt2D& rkDist) const;

    void L2Check (int iX, int iY, int iDx, int iDy, ImageInt2D& rkXNear,
        ImageInt2D& rkYNear, ImageInt2D& rkDist) const;

    void L2XpYp (ImageInt2D& rkXNear, ImageInt2D& rkYNear,
        ImageInt2D& rkDist) const;

    void L2XpYm (ImageInt2D& rkXNear, ImageInt2D& rkYNear,
        ImageInt2D& rkDist) const;

    void L2XmYp (ImageInt2D& rkXNear, ImageInt2D& rkYNear,
        ImageInt2D& rkDist) const;

    void L2XmYm (ImageInt2D& rkXNear, ImageInt2D& rkYNear,
        ImageInt2D& rkDist) const;

    void L2Finalize (const ImageInt2D& rkDist, Real& rfMaxDistance,
        ImageDouble2D& rkTransform) const;

    // helpers for skeletonization
    typedef bool (*InteriorFunction)(ImageInt2D&,int,int);
    static bool Interior4 (ImageInt2D& rkImage, int iX, int iY);
    static bool Interior3 (ImageInt2D& rkImage, int iX, int iY);
    static bool Interior2 (ImageInt2D& rkImage, int iX, int iY);
    static bool MarkInterior (ImageInt2D& rkImage, int iValue,
        InteriorFunction oIFunction);

    static bool IsArticulation (ImageInt2D& rkImage, int iX, int iY);
    static bool ClearInteriorAdjacent (ImageInt2D& rkImage, int iValue);
    static void Trim4 (ImageInt2D& rkImage);
    static void Trim3 (ImageInt2D& rkImage);
    static void Trim2 (ImageInt2D& rkImage);
};

} // namespace Mgc

#endif


