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

//----------------------------------------------------------------------------
void GetBoundaries (const Binary2D& rkImage)
{
    int* aiBoundaries = rkImage.GetBoundaries();
    if ( aiBoundaries )
    {
        ImageInt2D kOutput(rkImage.GetBound(0),rkImage.GetBound(1));
        int iIndex = 1;
        for (int iB = 0; iB < aiBoundaries[0]; iB++)
        {
            int iQuantity = aiBoundaries[iIndex++];
            for (int i = 0; i < iQuantity; i++)
                kOutput[aiBoundaries[iIndex++]] = 1;
        }

        kOutput.Save("boundaries.im");
        delete[] aiBoundaries;
    }
}
//----------------------------------------------------------------------------
void GetComponents (const Binary2D& rkImage)
{
    int iQuantity;
    ImageInt2D kComponents(rkImage.GetBound(0),rkImage.GetBound(1));
    rkImage.GetComponents(iQuantity,kComponents);
    kComponents.Save("components.im");
}
//----------------------------------------------------------------------------
void GetL1Distance (const Binary2D& rkImage)
{
    int iMaxDistance;
    ImageInt2D kDistance(rkImage.GetBound(0),rkImage.GetBound(1));
    rkImage.GetL1Distance(iMaxDistance,kDistance);
    kDistance.Save("L1distance.im");
}
//----------------------------------------------------------------------------
void GetL2Distance (const Binary2D& rkImage)
{
    float fMaxDistance;
    ImageDouble2D kDistance(rkImage.GetBound(0),rkImage.GetBound(1));
    rkImage.GetL2Distance(fMaxDistance,kDistance);
    kDistance.Save("L2distance.im");
}
//----------------------------------------------------------------------------
void GetSkeleton (const Binary2D& rkImage)
{
    ImageInt2D kSkeleton(rkImage.GetBound(0),rkImage.GetBound(1));
    rkImage.GetSkeleton(kSkeleton);
    kSkeleton.Save("skeleton.im");
}
//----------------------------------------------------------------------------
int main ()
{
    Binary2D kImage("binary.im");

    GetBoundaries(kImage);
    GetComponents(kImage);
    GetL1Distance(kImage);
    GetL2Distance(kImage);
    GetSkeleton(kImage);

    return 0;
}
//----------------------------------------------------------------------------


