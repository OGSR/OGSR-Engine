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

#include "MgcBinary3D.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Binary3D::Binary3D (int iXBound, int iYBound, int iZBound, Eint* atData)
    :
    ImageInt3D(iXBound,iYBound,iZBound,atData)
{
}
//----------------------------------------------------------------------------
Binary3D::Binary3D (const Binary3D& rkImage)
    :
    ImageInt3D(rkImage)
{
}
//----------------------------------------------------------------------------
Binary3D::Binary3D (const char* acFilename)
    :
    ImageInt3D(acFilename)
{
}
//----------------------------------------------------------------------------
void Binary3D::GetComponents (int& riQuantity, ImageInt3D& rkComponents) const
{
    // Create a temporary copy of image to store intermediate information
    // during component labeling.  The original image is embedded in an image
    // with two more slices, two more rows, and two more columns so that the
    // image boundary pixels are properly handled.  This copy is initially
    // zero.
    ImageInt3D kTemp(GetBound(0)+2,GetBound(1)+2,GetBound(2)+2);
    int iX, iY, iZ, iXP, iYP, iZP;
    for (iZ = 0, iZP = 1; iZ < GetBound(2); iZ++, iZP++)
    {
        for (iY = 0, iYP = 1; iY < GetBound(1); iY++, iYP++)
        {
            for (iX = 0, iXP = 1; iX < GetBound(0); iX++, iXP++)
                kTemp(iXP,iYP,iZP) = ( (*this)(iX,iY,iZ) ? 1 : 0 );
        }
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

    // Merge equivalent components.  Voxel (x,y,z) has previous neighbors
    // (x-1,y-1,z-1), (x,y-1,z-1), (x+1,y-1,z-1), (x-1,y,z-1), (x,y,z-1),
    // (x+1,y,z-1), (x-1,y+1,z-1), (x,y+1,z-1), (x+1,y+1,z-1), (x-1,y-1,z),
    // (x,y-1,z), (x+1,y-1,z), (x-1,y,z) [13 of 26 voxels visited before
    // (x,y,z) is visited, get component labels from them].
    for (iZ = 1; iZ < kTemp.GetBound(2)-1; iZ++)
    {
        for (iY = 1; iY < kTemp.GetBound(1)-1; iY++)
        {
            for (iX = 1; iX < kTemp.GetBound(0)-1; iX++)
            {
                int iValue = kTemp(iX,iY,iZ);
                if ( iValue > 0 )
                {
                    AddToAssociative(iValue,kTemp(iX-1,iY-1,iZ-1),aiAssoc);
                    AddToAssociative(iValue,kTemp(iX,  iY-1,iZ-1),aiAssoc);
                    AddToAssociative(iValue,kTemp(iX+1,iY-1,iZ-1),aiAssoc);
                    AddToAssociative(iValue,kTemp(iX-1,iY  ,iZ-1),aiAssoc);
                    AddToAssociative(iValue,kTemp(iX  ,iY  ,iZ-1),aiAssoc);
                    AddToAssociative(iValue,kTemp(iX+1,iY  ,iZ-1),aiAssoc);
                    AddToAssociative(iValue,kTemp(iX-1,iY+1,iZ-1),aiAssoc);
                    AddToAssociative(iValue,kTemp(iX  ,iY+1,iZ-1),aiAssoc);
                    AddToAssociative(iValue,kTemp(iX+1,iY+1,iZ-1),aiAssoc);
                    AddToAssociative(iValue,kTemp(iX-1,iY-1,iZ  ),aiAssoc);
                    AddToAssociative(iValue,kTemp(iX  ,iY-1,iZ  ),aiAssoc);
                    AddToAssociative(iValue,kTemp(iX+1,iY-1,iZ  ),aiAssoc);
                    AddToAssociative(iValue,kTemp(iX-1,iY  ,iZ  ),aiAssoc);
                }
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
    for (iZ = 0, iZP = 1; iZ < rkComponents.GetBound(2); iZ++, iZP++)
    {
        for (iY = 0, iYP = 1; iY < rkComponents.GetBound(1); iY++, iYP++)
        {
            for (iX = 0, iXP = 1; iX < rkComponents.GetBound(0); iX++, iXP++)
                rkComponents(iX,iY,iZ) = aiAssoc[kTemp(iXP,iYP,iZP)];
        }
    }

    delete[] aiAssoc;
}
//----------------------------------------------------------------------------
void Binary3D::AddToAssociative (int i0, int i1, int* aiAssoc) const
{
    // Adjacent voxels have labels i0 and i1. Associate them so that they
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


