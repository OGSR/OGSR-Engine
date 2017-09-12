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

#include "MgcBinary2D.h"
using namespace Mgc;
using namespace std;

//----------------------------------------------------------------------------
Binary2D::Binary2D (int iXBound, int iYBound, Eint* atData)
    :
    ImageInt2D(iXBound,iYBound,atData)
{
}
//----------------------------------------------------------------------------
Binary2D::Binary2D (const Binary2D& rkImage)
    :
    ImageInt2D(rkImage)
{
}
//----------------------------------------------------------------------------
Binary2D::Binary2D (const char* acFilename)
    :
    ImageInt2D(acFilename)
{
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Extraction of boundary from binary objects in image.
//
// Directions are:  W=0, NW=1, N=2, NE=3, E=4, SE=5, S=6, SW=7.  If a pixel
// was reached from the direction indicated, then its eight neighbors are
// searched in the order shown:
//
//     W      NW     N      NE     E      SE     S      SW
//     0 1 2  7 0 1  6 7 0  5 6 7  4 5 6  3 4 5  2 3 4  1 2 3
//     7 * 3  6 * 2  5 * 1  4 * 0  3 * 7  2 * 6  1 * 5  0 * 4
//     6 5 4  5 4 3  4 3 2  3 2 1  2 1 0  1 0 7  0 7 6  7 6 5
//
// The formula for the next direction is:  newdir = (index+5+olddir) MOD 8
// where index is the number of the neighbor in the search.  For example, in
// the configuration,
//
//   010
//   010
//   001
//
// suppose the center pixel was reached from the North (olddir=2).  Neighbors
// at index=0 and index=1 are zero.  The first nonzero neighbor is at index=2.
// Thus, newdir = (2+5+2) MOD 8 = 1, so the direction from which the next
// pixel is reached is NorthWest.
//
// One-pixel thick structures are treated as having more thickness in the
// sense that they are traversed twice, once per "side".
//----------------------------------------------------------------------------
int* Binary2D::GetBoundaries () const
{
    // Create a temporary copy of image to store intermediate information
    // during boundary extraction.  The original image is embedded in an
    // image with two more rows and two more columns so that the image
    // boundary pixels are properly handled.
    ImageInt2D kTemp(GetBound(0)+2,GetBound(1)+2);  // initially zero
    int iX, iY, iXP, iYP;
    for (iY = 0, iYP = 1; iY < GetBound(1); iY++, iYP++)
    {
        for (iX = 0, iXP = 1; iX < GetBound(0); iX++, iXP++)
            kTemp(iXP,iYP) = ( (*this)(iX,iY) ? 1 : 0 );
    }

    // Label interior pixels as 2.  Exterior pixels are then 0, boundary
    // pixels are then 1.
    for (iY = 1; iY+1 < kTemp.GetBound(1); iY++)
    {
        for (iX = 1; iX+1 < kTemp.GetBound(0); iX++)
        {
            if ( kTemp(iX,iY) && kTemp(iX-1,iY) && kTemp(iX+1,iY)
            &&   kTemp(iX,iY-1) && kTemp(iX,iY+1) )
            {
                kTemp(iX,iY) = 2;
            }
        }
    }

    // Search image for boundary points and extract the full boundaries.
    vector<BoundaryList*> kBoundaries;
    for (iY = 0; iY < kTemp.GetBound(1); iY++)
    {
        for (iX = 0; iX < kTemp.GetBound(0); iX++)
        {
            if ( kTemp(iX,iY) == 1 )
                kBoundaries.push_back(ExtractBoundary(iX,iY,kTemp));
        }
    }

    // Repackage lists into a single array.
    int iSize = 1;  // make room for boundary count
    int i;
    for (i = 0; i < (int)kBoundaries.size(); i++)
        iSize += kBoundaries[i]->size() + 1;

    int* aiPacked = new int[iSize];
    aiPacked[0] = (int)kBoundaries.size();
    int iIndex = 1;
    for (i = 0; i < (int)kBoundaries.size(); i++)
    {
        BoundaryList* pkBoundary = kBoundaries[i];

        aiPacked[iIndex++] = pkBoundary->size();
        for (int j = 0; j < (int)pkBoundary->size(); j++)
            aiPacked[iIndex++] = (*pkBoundary)[j];

        delete pkBoundary;
    }

    return aiPacked;
}
//----------------------------------------------------------------------------
Binary2D::BoundaryList* Binary2D::ExtractBoundary (int iX0, int iY0,
    ImageInt2D& rkTemp) const
{
    static const int s_aiDx[8] = { -1,  0, +1, +1, +1,  0, -1, -1 };
    static const int s_aiDy[8] = { -1, -1, -1,  0, +1, +1, +1,  0 };

    // Create new point list containing first boundary point.  Note that the
    // index for the pixel is computed for the original image, not for the
    // larger temporary image.
    BoundaryList* pkBoundary = new BoundaryList;
    pkBoundary->push_back(GetIndex(iX0-1,iY0-1));

    // Compute the direction from background (0) to boundary pixel (1).
    int iCx = iX0, iCy = iY0;
    int iNx, iNy, iDir;
    for (iDir = 0; iDir < 8; iDir++)
    {
        iNx = iCx + s_aiDx[iDir];
        iNy = iCy + s_aiDy[iDir];
        if ( rkTemp(iNx,iNy) == 0 )
        {
            iDir = (iDir+1)%8;
            break;
        }
    }

    // Traverse boundary in clockwise order.  Mark visited pixels as 3.
    rkTemp(iCx,iCy) = 3;
    while ( true )
    {
        int i, iNbr;
        for (i = 0, iNbr = iDir; i < 8; i++, iNbr = (iNbr+1)%8)
        {
            iNx = iCx + s_aiDx[iNbr];
            iNy = iCy + s_aiDy[iNbr];
            if ( rkTemp(iNx,iNy) )  // next boundary pixel found
                break;
        }

        if ( i == 8 )  // (iCx,iCy) is isolated
            break;

        if ( iNx == iX0 && iNy == iY0 )  // boundary traversal completed
            break;

        // (iNx,iNy) is next boundary point, add point to list.  Note that
        // the index for the pixel is computed for the original image, not
        // for the larger temporary image.
        pkBoundary->push_back(GetIndex(iNx-1,iNy-1));

        // mark visited pixels as 3
        rkTemp(iNx,iNy) = 3;

        // start search for next point
        iCx = iNx;
        iCy = iNy;
        iDir = (i+5+iDir)%8;
    }

    return pkBoundary;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Connected component labeling.
//
// The 1D connected components of each row are labeled first.  The labels
// of row-adjacent components are merged by using an associative memory
// scheme.  The associative memory is stored as an array that initially
// represents the identity permutation M = {0,1,2,...}.  Each association of
// i and j is represented by applying the transposition (i,j) to the array.
// That is, M[i] and M[j] are swapped.  After all associations have been
// applied during the merge step, the array has cycles that represent the
// connected components.  A relabeling is done to give a consecutive set of
// positive component labels.
//
// For example:
//
//     Image       Rows labeled
//     00100100    00100200
//     00110100    00330400
//     00011000    00055000
//     01000100    06000700
//     01000000    08000000
//
// Initially, the associated memory is M[i] = i for 0 <= i <= 8.  Background
// label is 0 and never changes.
// 1. Pass through second row.
//    a. 3 is associated with 1, M = {0,3,2,1,4,5,6,7,8}
//    b. 4 is associated with 2, M = {0,3,4,1,2,5,6,7,8}
// 2. Pass through third row.
//    a. 5 is associated with 3, M = {0,3,4,5,2,1,6,7,8}.  Note that
//       M[5] = 5 and M[3] = 1 have been swapped, not a problem since
//       1 and 3 are already "equivalent labels".
//    b. 5 is associated with 4, M = {0,3,4,5,1,2,6,7,8}
// 3. Pass through fourth row.
//    a. 7 is associated with 5, M = {0,3,4,5,1,7,6,2,8}
// 4. Pass through fifth row.
//    a. 8 is associated with 6, M = {0,3,4,5,1,7,8,2,6}
//
// The M array has two cycles.  Cycle 1 is
//   M[1] = 3, M[3] = 5, M[5] = 7, M[7] = 2, M[2] = 4, M[4] = 1
// and cycle 2 is
//   M[6] = 8, M[8] = 6
// The image is relabeled by replacing each pixel value with the index of
// the cycle containing it.  The result for this example is
//     00100100
//     00110100
//     00011000
//     02000100
//     02000000
//----------------------------------------------------------------------------
void Binary2D::GetComponents (int& riQuantity, ImageInt2D& rkComponents) const
{
    // Create a temporary copy of image to store intermediate information
    // during component labeling.  The original image is embedded in an
    // image with two more rows and two more columns so that the image
    // boundary pixels are properly handled.
    ImageInt2D kTemp(GetBound(0)+2,GetBound(1)+2);  // initially zero
    int iX, iY, iXP, iYP;
    for (iY = 0, iYP = 1; iY < GetBound(1); iY++, iYP++)
    {
        for (iX = 0, iXP = 1; iX < GetBound(0); iX++, iXP++)
            kTemp(iXP,iYP) = ( (*this)(iX,iY) ? 1 : 0 );
    }

    // label connected components in 1D array
    int i, iComponent = 0;
    for (i = 0; i < kTemp.GetQuantity(); i++)
    {
        if ( kTemp[i] )
        {
            iComponent++;
            while ( kTemp[i] )
            {
                // loop terminates since kTemp is zero on its boundaries
                kTemp[i++] = iComponent;
            }
        }
    }

    if ( iComponent == 0 )
    {
        // input image is identically zero
        riQuantity = 0;
        rkComponents = (Eint)0;
        return;
    }

    // associative memory for merging
    int* aiAssoc = new int[iComponent+1];
    for (i = 0; i < iComponent + 1; i++)
        aiAssoc[i] = i;

    // Merge equivalent components.  Pixel (x,y) has previous neighbors
    // (x-1,y-1), (x,y-1), (x+1,y-1), and (x-1,y) [4 of 8 pixels visited
    // before (x,y) is visited, get component labels from them].
    for (iY = 1; iY < kTemp.GetBound(1)-1; iY++)
    {
        for (iX = 1; iX < kTemp.GetBound(0)-1; iX++)
        {
            int iValue = kTemp(iX,iY);
            if ( iValue > 0 )
            {
                AddToAssociative(iValue,kTemp(iX-1,iY-1),aiAssoc);
                AddToAssociative(iValue,kTemp(iX  ,iY-1),aiAssoc);
                AddToAssociative(iValue,kTemp(iX+1,iY-1),aiAssoc);
                AddToAssociative(iValue,kTemp(iX-1,iY  ),aiAssoc);
            }
        }
    }

    // replace each cycle of equivalent labels by a single label
    riQuantity = 0;
    for (i = 1; i <= iComponent; i++)
    {
        if ( i <= aiAssoc[i] )
        {
            riQuantity++;
            int iCurrent = i;
            while ( aiAssoc[iCurrent] != i )
            {
                int iNext = aiAssoc[iCurrent];
                aiAssoc[iCurrent] = riQuantity;
                iCurrent = iNext;
            }
            aiAssoc[iCurrent] = riQuantity;
        }
    }

    // pack a relabeled image in smaller size output
    for (iY = 0, iYP = 1; iY < rkComponents.GetBound(1); iY++, iYP++)
    {
        for (iX = 0, iXP = 1; iX < rkComponents.GetBound(0); iX++, iXP++)
            rkComponents(iX,iY) = aiAssoc[kTemp(iXP,iYP)];
    }

    delete[] aiAssoc;
}
//----------------------------------------------------------------------------
void Binary2D::AddToAssociative (int i0, int i1, int* aiAssoc) const
{
    // Adjacent pixels have labels i0 and i1. Associate them so that they
    // represent the same component.  [assert: i0 > 0]
    if ( i1 == 0 || i1 == i0 )
        return;

    int iSearch = i1;
    do
    {
        iSearch = aiAssoc[iSearch];
    }
    while ( iSearch != i1 && iSearch != i0 );

    if ( iSearch == i1 )
    {
        int iSave = aiAssoc[i0];
        aiAssoc[i0] = aiAssoc[i1];
        aiAssoc[i1] = iSave;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// L1 Distance Transform.
//
// This distance is also known as the "city block" distance or whatever.
// North, South, East, and West neighbors are 1 unit away.  NorthWest,
// NorthEast, SouthWest, and SouthEast neighbors are 2 units away.
//----------------------------------------------------------------------------
void Binary2D::GetL1Distance (int& riMaxDistance,
    ImageInt2D& rkTransform) const
{
    // Create a temporary copy of image to store intermediate information
    // during distance calculations.  The original image is embedded in an
    // image with two more rows and two more columns so that the image
    // boundary pixels are properly handled.
    ImageInt2D kTemp(GetBound(0)+2,GetBound(1)+2);  // initially zero
    int iX, iY, iXP, iYP;
    for (iY = 0, iYP = 1; iY < GetBound(1); iY++, iYP++)
    {
        for (iX = 0, iXP = 1; iX < GetBound(0); iX++, iXP++)
            kTemp(iXP,iYP) = ( (*this)(iX,iY) ? 1 : 0 );
    }

    GetL1DistanceZeroBoundary(riMaxDistance,kTemp);    

    // pack transformed image in smaller size output
    for (iY = 0; iY < rkTransform.GetBound(1); iY++)
    {
        for (iX = 0; iX < rkTransform.GetBound(0); iX++)
            rkTransform(iX,iY) = kTemp(iX+1,iY+1);
    }
}
//----------------------------------------------------------------------------
void Binary2D::GetL1DistanceZeroBoundary (int& riMaxDistance,
    ImageInt2D& rkTemp) const
{
    bool bChangeMade = true;
    int iDistance;
    for (iDistance = 1; bChangeMade; iDistance++)
    {
        bChangeMade = false;
        int iDistanceP1 = iDistance + 1;
        for (int iY = 1; iY+1 < rkTemp.GetBound(1); iY++)
        {
            for (int iX = 1; iX+1 < rkTemp.GetBound(0); iX++)
            {
                if ( rkTemp(iX,iY) == iDistance )
                {
                    if ( rkTemp(iX-1,iY) >= iDistance
                    &&   rkTemp(iX+1,iY) >= iDistance
                    &&   rkTemp(iX,iY-1) >= iDistance
                    &&   rkTemp(iX,iY+1) >= iDistance )
                    {
                        rkTemp(iX,iY) = iDistanceP1;
                        bChangeMade = true;
                    }
                }
            }
        }
    }

    riMaxDistance = iDistance;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// L2 Distance Transform (Euclidean Distance Transform)
//
// This program calculates the Euclidean distance transform of a binary
// input image.  The adaptive algorithm is guaranteed to give exact
// distances for all distances < 100.  Algorithm sent to me by John Gauch.
//
// From: John Gauch <jgauch@tisl.ukans.edu>
// The basic idea is similar to a EDT described recently in PAMI by Laymarie
// from McGill.  By keeping the dx and dy offset to the nearest edge (feature)
// point in the image, we can search to see which dx dy is closest to a given
// point by examining a set of neighbors.  The Laymarie method (and Borgfors)
// look at a fixed 3x3 or 5x5 neighborhood and call it a day.  What we did was
// calculate (painfully) what neighborhoods you need to look at to guarentee
// that the exact distance is obtained.  Thus, you will see in the code, that
// we L2Check the current distance and depending on what we have so far, we
// extend the search region.  Since our algorithm for L2Checking the exactness
// of each neighborhood is on the order N^4, we have only gone to N=100.  In
// theory, you could make this large enough to get all distances exact.  We
// have implemented the algorithm to get all distances < 100 to be exact. 
//----------------------------------------------------------------------------
void Binary2D::GetL2Distance (Real& rfMaxDistance,
    ImageDouble2D& rkTransform) const
{
    ImageInt2D kXNear(GetBound(0),GetBound(1));
    ImageInt2D kYNear(GetBound(0),GetBound(1));
    ImageInt2D kDist(GetBound(0),GetBound(1));

    L2Initialize(kXNear,kYNear,kDist);
    L2XpYp(kXNear,kYNear,kDist);
    L2XmYm(kXNear,kYNear,kDist);
    L2XpYm(kXNear,kYNear,kDist);
    L2XmYp(kXNear,kYNear,kDist);
    L2Finalize(kDist,rfMaxDistance,rkTransform);
}
//----------------------------------------------------------------------------
void Binary2D::L2Initialize (ImageInt2D& rkXNear,
    ImageInt2D& rkYNear, ImageInt2D& rkDist) const
{
    for (int iY = 0; iY < GetBound(1); iY++)
    {
        for (int iX = 0; iX < GetBound(0); iX++)
        {
            if ( (*this)(iX,iY) != 0 )
            {
                rkXNear(iX,iY) = 0;
                rkYNear(iX,iY) = 0;
                rkDist(iX,iY) = INT_MAX;
            }
            else
            {
                rkXNear(iX,iY) = iX;
                rkYNear(iX,iY) = iY;
                rkDist(iX,iY) = 0;
            }
        }
    }
}
//----------------------------------------------------------------------------
void Binary2D::L2Check (int iX, int iY, int iDx, int iDy,
    ImageInt2D& rkXNear, ImageInt2D& rkYNear, ImageInt2D& rkDist) const
{
    int iXp = iX + iDx, iYp = iY + iDy;

    if ( 0 <= iXp && iXp < GetBound(0)
    &&   0 <= iYp && iYp < GetBound(1) )
    {
        if ( rkDist(iXp,iYp) < rkDist(iX,iY) )
        {
            int iDx0 = rkXNear(iXp,iYp) - iX;
            int iDy0 = rkYNear(iXp,iYp) - iY;
            int iNewDist = iDx0*iDx0 + iDy0*iDy0;
            if ( iNewDist < rkDist(iX,iY) )
            {
                rkXNear(iX,iY) = rkXNear(iXp,iYp);
                rkYNear(iX,iY) = rkYNear(iXp,iYp);
                rkDist(iX,iY) = iNewDist;
            }
        }
    }
}
//----------------------------------------------------------------------------
void Binary2D::L2XpYp (ImageInt2D& rkXNear, ImageInt2D& rkYNear,
    ImageInt2D& rkDist) const
{
    const int iK1 = 1;
    const int iK2 = 169;   // 13^2
    const int iK3 = 961;   // 31^2
    const int iK4 = 2401;  // 49^2
    const int iK5 = 5184;  // 72^2

    for (int iY = 0; iY < GetBound(1); iY++)
    {
        for (int iX = 0; iX < GetBound(0); iX++)
        {
            int iDist = rkDist(iX,iY);
            if ( iDist > iK1 )
            { 
                L2Check(iX,iY,-1,0,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-1,-1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,0,-1,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK2 )
            { 
                L2Check(iX,iY,-2,-1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-1,-2,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK3 )
            { 
                L2Check(iX,iY,-3,-1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-3,-2,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-2,-3,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-1,-3,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK4 )
            { 
                L2Check(iX,iY,-4,-1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-4,-3,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-3,-4,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-1,-4,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK5 )
            { 
                L2Check(iX,iY,-5,-1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-5,-2,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-5,-3,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-5,-4,rkXNear,rkYNear,rkDist);
                L2Check(iX,iY,-4,-5,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-2,-5,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-3,-5,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-1,-5,rkXNear,rkYNear,rkDist); 
            }
        }
    }
}
//----------------------------------------------------------------------------
void Binary2D::L2XpYm (ImageInt2D& rkXNear, ImageInt2D& rkYNear,
    ImageInt2D& rkDist) const
{
    const int iK1 = 1;
    const int iK2 = 169;   // 13^2
    const int iK3 = 961;   // 31^2
    const int iK4 = 2401;  // 49^2
    const int iK5 = 5184;  // 72^2

    for (int iY = GetBound(1)-1; iY >= 0; iY--)
    {
        for (int iX = 0; iX < GetBound(0); iX++)
        {
            int iDist = rkDist(iX,iY);
            if ( iDist > iK1 )
            { 
                L2Check(iX,iY,-1,0,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-1,1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,0,1,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK2 )
            { 
                L2Check(iX,iY,-2,1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-1,2,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK3 )
            { 
                L2Check(iX,iY,-3,1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-3,2,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-2,3,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-1,3,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK4 )
            { 
                L2Check(iX,iY,-4,1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-4,3,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-3,4,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-1,4,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK5 )
            { 
                L2Check(iX,iY,-5,1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-5,2,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-5,3,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-5,4,rkXNear,rkYNear,rkDist);
                L2Check(iX,iY,-4,5,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-2,5,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-3,5,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,-1,5,rkXNear,rkYNear,rkDist); 
            }
        }
    }
}
//----------------------------------------------------------------------------
void Binary2D::L2XmYp (ImageInt2D& rkXNear, ImageInt2D& rkYNear,
    ImageInt2D& rkDist) const
{
    const int iK1 = 1;
    const int iK2 = 169;   // 13^2
    const int iK3 = 961;   // 31^2
    const int iK4 = 2401;  // 49^2
    const int iK5 = 5184;  // 72^2

    for (int iY = 0; iY < GetBound(1); iY++)
    {
        for (int iX = GetBound(0)-1; iX >= 0; iX--)
        {
            int iDist = rkDist(iX,iY);
            if ( iDist > iK1 )
            { 
                L2Check(iX,iY,1,0,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,1,-1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,0,-1,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK2 )
            { 
                L2Check(iX,iY,2,-1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,1,-2,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK3 )
            { 
                L2Check(iX,iY,3,-1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,3,-2,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,2,-3,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,1,-3,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK4 )
            { 
                L2Check(iX,iY,4,-1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,4,-3,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,3,-4,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,1,-4,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK5 )
            { 
                L2Check(iX,iY,5,-1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,5,-2,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,5,-3,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,5,-4,rkXNear,rkYNear,rkDist);
                L2Check(iX,iY,4,-5,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,2,-5,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,3,-5,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,1,-5,rkXNear,rkYNear,rkDist); 
            }
        }
    }
}
//----------------------------------------------------------------------------
void Binary2D::L2XmYm (ImageInt2D& rkXNear, ImageInt2D& rkYNear,
    ImageInt2D& rkDist) const
{
    const int iK1 = 1;
    const int iK2 = 169;   // 13^2
    const int iK3 = 961;   // 31^2
    const int iK4 = 2401;  // 49^2
    const int iK5 = 5184;  // 72^2

    for (int iY = GetBound(1)-1; iY >= 0; iY--)
    {
        for (int iX = GetBound(0)-1; iX >= 0; iX--)
        {
            int iDist = rkDist(iX,iY);
            if ( iDist > iK1 )
            { 
                L2Check(iX,iY,1,0,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,1,1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,0,1,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK2 )
            { 
                L2Check(iX,iY,2,1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,1,2,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK3 )
            { 
                L2Check(iX,iY,3,1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,3,2,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,2,3,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,1,3,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK4 )
            { 
                L2Check(iX,iY,4,1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,4,3,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,3,4,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,1,4,rkXNear,rkYNear,rkDist); 
            }
            if ( iDist > iK5 )
            { 
                L2Check(iX,iY,5,1,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,5,2,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,5,3,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,5,4,rkXNear,rkYNear,rkDist);
                L2Check(iX,iY,4,5,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,2,5,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,3,5,rkXNear,rkYNear,rkDist); 
                L2Check(iX,iY,1,5,rkXNear,rkYNear,rkDist); 
            }
        }
    }
}
//----------------------------------------------------------------------------
void Binary2D::L2Finalize (const ImageInt2D& rkDist, Real& rfMaxDistance,
    ImageDouble2D& rkTransform) const
{
    rfMaxDistance = 0.0f;
    for (int iY = 0; iY < GetBound(1); iY++)
    {
        for (int iX = 0; iX < GetBound(0); iX++)
        {
            Real fDist = Math::Sqrt((Real)rkDist(iX,iY));
            if ( fDist > rfMaxDistance )
                rfMaxDistance = fDist;

            rkTransform(iX,iY) = fDist;
        }
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Skeletonization.
//
// Boundary pixels are trimmed from the object one layer at a time based on
// their adjacency to interior pixels.  At each step the connectivity and
// cycles of the object are preserved.
//----------------------------------------------------------------------------
void Binary2D::GetSkeleton (ImageInt2D& rkSkeleton) const
{
    // Create a temporary copy of image to store intermediate information
    // during skeletonization.  The original image is embedded in an
    // image with two more rows and two more columns so that the image
    // boundary pixels are properly handled.
    ImageInt2D kTemp(GetBound(0)+2,GetBound(1)+2);  // initially zero
    int iX, iY, iXP, iYP;
    for (iY = 0, iYP = 1; iY < GetBound(1); iY++, iYP++)
    {
        for (iX = 0, iXP = 1; iX < GetBound(0); iX++, iXP++)
            kTemp(iXP,iYP) = ( (*this)(iX,iY) ? 1 : 0 );
    }

    Trim4(kTemp);
    Trim3(kTemp);
    Trim2(kTemp);

    // pack skeleton in smaller size output
    for (iY = 0, iYP = 1; iY < rkSkeleton.GetBound(1); iY++, iYP++)
    {
        for (iX = 0, iXP = 1; iX < rkSkeleton.GetBound(0); iX++, iXP++)
            rkSkeleton(iX,iY) = ( kTemp(iXP,iYP) ? 1 : 0 );
    }
}
//----------------------------------------------------------------------------
bool Binary2D::Interior2 (ImageInt2D& rkImage, int iX, int iY)
{
    bool b1 = rkImage(iX,iY-1) > 0;
    bool b3 = rkImage(iX+1,iY) > 0;
    bool b5 = rkImage(iX,iY+1) > 0;
    bool b7 = rkImage(iX-1,iY) > 0;
    return (b1 && b3) || (b3 && b5) || (b5 && b7) || (b7 && b1);
}
//----------------------------------------------------------------------------
bool Binary2D::Interior3 (ImageInt2D& rkImage, int iX, int iY)
{
    int iNbrs = 0;
    if ( rkImage(iX-1,iY) > 0 ) iNbrs++;
    if ( rkImage(iX+1,iY) > 0 ) iNbrs++;
    if ( rkImage(iX,iY-1) > 0 ) iNbrs++;
    if ( rkImage(iX,iY+1) > 0 ) iNbrs++;
    return iNbrs == 3;
}
//----------------------------------------------------------------------------
bool Binary2D::Interior4 (ImageInt2D& rkImage, int iX, int iY)
{
    return rkImage(iX-1,iY) > 0
        && rkImage(iX+1,iY) > 0
        && rkImage(iX,iY-1) > 0
        && rkImage(iX,iY+1) > 0;
}
//----------------------------------------------------------------------------
bool Binary2D::MarkInterior (ImageInt2D& rkImage, int iValue,
    InteriorFunction oIsInterior)
{
    bool bNoInterior = true;

    for (int iY = 0; iY < rkImage.GetBound(1); iY++)
    {
        for (int iX = 0; iX < rkImage.GetBound(0); iX++)
        {
            if ( rkImage(iX,iY) > 0 )
            {
                if ( oIsInterior(rkImage,iX,iY) )
                {
                    rkImage(iX,iY) = iValue;
                    bNoInterior = false;
                }
                else
                {
                    rkImage(iX,iY) = 1;
                }
            }
        }
    }

    return bNoInterior;
}
//----------------------------------------------------------------------------
bool Binary2D::IsArticulation (ImageInt2D& rkImage, int iX, int iY)
{
    static int s_aiArticulation[256] =
    {
        0,0,0,0,0,1,0,0,0,1,0,0,0,1,0,0,
        0,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,
        0,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,
        0,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,
        0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        0,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,
        0,1,1,1,1,1,1,1,0,1,0,0,0,1,0,0,
        0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
        1,1,1,1,1,1,1,1,1,1,0,0,1,1,0,0,
        0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,1,1,0,0,1,1,0,0,1,1,0,0,
        1,1,1,1,1,1,1,1,1,1,0,0,1,1,0,0,
        0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,1,1,0,0,0,0,0,0,0,0,0,0
    };

    // Converts 8 neighbors of pixel (x,y) to an 8-bit value, bit = 1 iff
    // pixel is set.
    int iByte = 0;
    if ( rkImage(iX-1,iY-1) )  iByte |= 0x01;
    if ( rkImage(iX,  iY-1) )  iByte |= 0x02;
    if ( rkImage(iX+1,iY-1) )  iByte |= 0x04;
    if ( rkImage(iX+1,iY  ) )  iByte |= 0x08;
    if ( rkImage(iX+1,iY+1) )  iByte |= 0x10;
    if ( rkImage(iX  ,iY+1) )  iByte |= 0x20;
    if ( rkImage(iX-1,iY+1) )  iByte |= 0x40;
    if ( rkImage(iX-1,iY  ) )  iByte |= 0x80;

    return s_aiArticulation[iByte] == 1;
}
//----------------------------------------------------------------------------
bool Binary2D::ClearInteriorAdjacent (ImageInt2D& rkImage, int iValue)
{
    bool bNoRemoval = true;

    for (int iY = 0; iY < rkImage.GetBound(1); iY++)
    {
        for (int iX = 0; iX < rkImage.GetBound(0); iX++)
        {
            if ( rkImage(iX,iY) == 1 )
            {
                bool bInteriorAdjacent =
                    rkImage(iX-1,iY-1) == iValue ||
                    rkImage(iX  ,iY-1) == iValue ||
                    rkImage(iX+1,iY-1) == iValue ||
                    rkImage(iX+1,iY  ) == iValue ||
                    rkImage(iX+1,iY+1) == iValue ||
                    rkImage(iX  ,iY+1) == iValue ||
                    rkImage(iX-1,iY+1) == iValue ||
                    rkImage(iX-1,iY  ) == iValue;

                if ( bInteriorAdjacent && !IsArticulation(rkImage,iX,iY) )
                {
                    rkImage(iX,iY) = 0;
                    bNoRemoval = false;
                }
            }
        }
    }

    return bNoRemoval;
}
//----------------------------------------------------------------------------
void Binary2D::Trim4 (ImageInt2D& rkImage)
{
    while ( true )
    {
        if ( MarkInterior(rkImage,4,Interior4) )
        {
            // No interior pixels, trimmed set is at most 2-pixels thick.
            break;
        }

        if ( ClearInteriorAdjacent(rkImage,4) )
        {
            // All remaining interior pixels are either articulation points
            // or part of blobs whose boundary pixels are all articulation
            // points.  An example of the latter case is shown below.  The
            // background pixels are marked with '.' rather than '0' for
            // readability.  The interior pixels are marked with '4' and the
            // boundary pixels are marked with '1'.
            //
            //   .........
            //   .....1...
            //   ..1.1.1..
            //   .1.141...
            //   ..14441..
            //   ..1441.1.
            //   .1.11.1..
            //   ..1..1...
            //   .........
            //
            // This is a pathological problem where there are many small holes
            // (0-pixel with north, south, west, and east neighbors all
            // 1-pixels) that your application can try to avoid by an initial
            // pass over the image to fill in such holes.  Of course, you do
            // have problems with checkerboard patterns...
            break;
        }
    }
}
//----------------------------------------------------------------------------
void Binary2D::Trim3 (ImageInt2D& rkImage)
{
    while ( true )
    {
        if ( MarkInterior(rkImage,3,Interior3) )
        {
            // No interior pixels, trimmed set is at most 2-pixels thick.
            break;
        }

        if ( ClearInteriorAdjacent(rkImage,3) )
        {
            // All remaining 3-values can be safely removed since they are
            // not articulation points and the removal will not cause new
            // holes.
            for (int iY = 0; iY < rkImage.GetBound(1); iY++)
            {
                for (int iX = 0; iX < rkImage.GetBound(0); iX++)
                {
                    if ( rkImage(iX,iY) == 3
                    &&   !IsArticulation(rkImage,iX,iY) )
                    {
                        rkImage(iX,iY) = 0;
                    }
                }
            }
            break;
        }
    }
}
//----------------------------------------------------------------------------
void Binary2D::Trim2 (ImageInt2D& rkImage)
{
    while ( true )
    {
        if ( MarkInterior(rkImage,2,Interior2) )
        {
            // No interior pixels, trimmed set is at most 1-pixel thick.
            // Call it a skeleton.
            break;
        }

        if ( ClearInteriorAdjacent(rkImage,2) )
        {
            // Removes 2-values that are not articulation points.
            for (int iY = 0; iY < rkImage.GetBound(1); iY++)
            {
                for (int iX = 0; iX < rkImage.GetBound(0); iX++)
                {
                    if ( rkImage(iX,iY) == 2
                    &&   !IsArticulation(rkImage,iX,iY) )
                    {
                        rkImage(iX,iY) = 0;
                    }
                }
            }
            break;
        }
    }
}
//----------------------------------------------------------------------------


