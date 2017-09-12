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

#include "MgcInterp3DAkimaUniform.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Interp3DAkimaUniform::Interp3DAkimaUniform (int iXBound, int iYBound,
    int iZBound, Real fXMin, Real fXSpacing, Real fYMin, Real fYSpacing,
    Real fZMin, Real fZSpacing, Real*** aaafF)
{
    // At least a 3x3x3 block of data points are needed to construct the
    // estimates of the boundary derivatives.
    assert( iXBound >= 3 && iYBound >= 3 && iZBound >= 3 && aaafF );
    assert( fXSpacing > 0.0f && fYSpacing > 0.0f && fZSpacing > 0.0f );

    m_iXBound = iXBound;
    m_iYBound = iYBound;
    m_iZBound = iZBound;
    m_fXMin = fXMin;
    m_fXSpacing = fXSpacing;
    m_fYMin = fYMin;
    m_fYSpacing = fYSpacing;
    m_fZMin = fZMin;
    m_fZSpacing = fZSpacing;
    m_aaafF = aaafF;

    int iXBoundM1 = iXBound - 1;
    int iYBoundM1 = iYBound - 1;
    int iZBoundM1 = iZBound - 1;
    m_iQuantity = iXBound*iYBound*iZBound;
    m_fXMax = fXMin + fXSpacing*iXBoundM1;
    m_fYMax = fYMin + fYSpacing*iYBoundM1;
    m_fZMax = fZMin + fZSpacing*iZBoundM1;

    // compute slopes
    Real fInvDX = 1.0f/fXSpacing;
    Real fInvDY = 1.0f/fYSpacing;
    Real fInvDZ = 1.0f/fZSpacing;
    Real fInvDXDY = fInvDX*fInvDY;
    Real fInvDXDZ = fInvDX*fInvDZ;
    Real fInvDYDZ = fInvDY*fInvDZ;
    Real fInvDXDYDZ = fInvDX*fInvDYDZ;
    Real*** aaafXSlope;  // xslope[z][y][x]
    Real*** aaafYSlope;  // yslope[z][x][y]
    Real*** aaafZSlope;  // zslope[y][x][z]
    Allocate(iXBound+3,iYBound,iZBound,aaafXSlope);
    Allocate(iYBound+3,iXBound,iZBound,aaafYSlope);
    Allocate(iZBound+3,iXBound,iYBound,aaafZSlope);
    int iX, iY, iZ;

    for (iZ = 0; iZ < iZBound; iZ++)
    {
        for (iY = 0; iY < iYBound; iY++)
        {
            for (iX = 0; iX < iXBoundM1; iX++)
            {
                aaafXSlope[iZ][iY][iX+2] = (aaafF[iZ][iY][iX+1] - 
                    aaafF[iZ][iY][iX])*fInvDX;
            }

            aaafXSlope[iZ][iY][1] = 2.0f*aaafXSlope[iZ][iY][2] -
                aaafXSlope[iZ][iY][3];
            aaafXSlope[iZ][iY][0] = 2.0f*aaafXSlope[iZ][iY][1] -
                aaafXSlope[iZ][iY][2];
            aaafXSlope[iZ][iY][iXBound+1] =
                2.0f*aaafXSlope[iZ][iY][iXBound] -
                aaafXSlope[iZ][iY][iXBound-1];
            aaafXSlope[iZ][iY][iXBound+2] =
                2.0f*aaafXSlope[iZ][iY][iXBound+1] -
                aaafXSlope[iZ][iY][iXBound];
        }
    }

    for (iZ = 0; iZ < iZBound; iZ++)
    {
        for (iX = 0; iX < iXBound; iX++)
        {
            for (iY = 0; iY < iYBoundM1; iY++)
            {
                aaafYSlope[iZ][iX][iY+2] = (aaafF[iZ][iY+1][iX] -
                    aaafF[iZ][iY][iX])*fInvDY;
            }

            aaafYSlope[iZ][iX][1] = 2.0f*aaafYSlope[iZ][iX][2] -
                aaafYSlope[iZ][iX][3];
            aaafYSlope[iZ][iX][0] = 2.0f*aaafYSlope[iZ][iX][1] -
                aaafYSlope[iZ][iX][2];
            aaafYSlope[iZ][iX][iYBound+1] =
                2.0f*aaafYSlope[iZ][iX][iYBound] -
                aaafYSlope[iZ][iX][iYBound-1];
            aaafYSlope[iZ][iX][iYBound+2] =
                2.0f*aaafYSlope[iZ][iX][iYBound+1] -
                aaafYSlope[iZ][iX][iYBound];
        }
    }

    for (iY = 0; iY < iYBound; iY++)
    {
        for (iX = 0; iX < iXBound; iX++)
        {
            for (iZ = 0; iZ < iZBoundM1; iZ++)
            {
                aaafZSlope[iY][iX][iZ+2] = (aaafF[iZ+1][iY][iX] -
                    aaafF[iZ][iY][iX])*fInvDZ;
            }

            aaafZSlope[iY][iX][1] = 2.0f*aaafZSlope[iY][iX][2] -
                aaafZSlope[iY][iX][3];
            aaafZSlope[iY][iX][0] = 2.0f*aaafZSlope[iY][iX][1] -
                aaafZSlope[iY][iX][2];
            aaafZSlope[iY][iX][iZBound+1] =
                2.0f*aaafZSlope[iY][iX][iZBound] -
                aaafZSlope[iY][iX][iZBound-1];
            aaafZSlope[iY][iX][iZBound+2] =
                2.0f*aaafZSlope[iY][iX][iZBound+1] -
                aaafZSlope[iY][iX][iZBound];
        }
    }

    // construct first-order derivatives
    Real*** aaafFX;
    Real*** aaafFY;
    Real*** aaafFZ;
    Allocate(iXBound,iYBound,iZBound,aaafFX);
    Allocate(iXBound,iYBound,iZBound,aaafFY);
    Allocate(iXBound,iYBound,iZBound,aaafFZ);

    for (iZ = 0; iZ < iZBound; iZ++)
    {
        for (iY = 0; iY < iYBound; iY++)
        {
            for (iX = 0; iX < iXBound; iX++)
            {
                aaafFX[iZ][iY][iX] = ComputeDerivative(
                   aaafXSlope[iZ][iY]+iX);
            }
        }
    }

    for (iZ = 0; iZ < iZBound; iZ++)
    {
        for (iX = 0; iX < iXBound; iX++)
        {
            for (iY = 0; iY < iYBound; iY++)
            {
                aaafFY[iZ][iY][iX] = ComputeDerivative(
                    aaafYSlope[iZ][iX]+iY);
            }
        }
    }

    for (iY = 0; iY < iYBound; iY++)
    {
        for (iX = 0; iX < iXBound; iX++)
        {
            for (iZ = 0; iZ < iZBound; iZ++)
            {
                aaafFZ[iZ][iY][iX] = ComputeDerivative(
                    aaafZSlope[iY][iX]+iZ);
            }
        }
    }

    // construct second-order derivatives
    Real*** aaafFXY;
    Real*** aaafFXZ;
    Real*** aaafFYZ;
    Allocate(iXBound,iYBound,iZBound,aaafFXY);
    Allocate(iXBound,iYBound,iZBound,aaafFXZ);
    Allocate(iXBound,iYBound,iZBound,aaafFYZ);

    int iX0 = iXBoundM1, iX1 = iX0-1,  iX2 = iX1-1;
    int iY0 = iYBoundM1, iY1 = iY0-1,  iY2 = iY1-1;
    int iZ0 = iZBoundM1, iZ1 = iZ0-1,  iZ2 = iZ1-1;

    for (iZ = 0; iZ < iZBound; iZ++)
    {
        // corners of z-slice
        aaafFXY[iZ][0][0] = 0.25f*fInvDXDY*(
            9.0f*aaafF[iZ][0][0] - 12.0f*aaafF[iZ][0][1] +
            3.0f*aaafF[iZ][0][2] - 12.0f*aaafF[iZ][1][0] +
            16.0f*aaafF[iZ][1][1] - 4.0f*aaafF[iZ][1][2] +
            3.0f*aaafF[iZ][2][0] - 4.0f*aaafF[iZ][2][1] +
            aaafF[iZ][2][2]);

        aaafFXY[iZ][0][iXBoundM1] = 0.25f*fInvDXDY*(
            9.0f*aaafF[iZ][0][iX0] - 12.0f*aaafF[iZ][0][iX1] +
            3.0f*aaafF[iZ][0][iX2] - 12.0f*aaafF[iZ][1][iX0] +
            16.0f*aaafF[iZ][1][iX1] - 4.0f*aaafF[iZ][1][iX2] +
            3.0f*aaafF[iZ][2][iX0] - 4.0f*aaafF[iZ][2][iX1] +
            aaafF[iZ][2][iX2]);

        aaafFXY[iZ][iYBoundM1][0] = 0.25f*fInvDXDY*(
            9.0f*aaafF[iZ][iY0][0] - 12.0f*aaafF[iZ][iY0][1] +
            3.0f*aaafF[iZ][iY0][2] - 12.0f*aaafF[iZ][iY1][0] +
            16.0f*aaafF[iZ][iY1][1] - 4.0f*aaafF[iZ][iY1][2] +
            3.0f*aaafF[iZ][iY2][0] - 4.0f*aaafF[iZ][iY2][1] +
            aaafF[iZ][iY2][2]);

        aaafFXY[iZ][iYBoundM1][iXBoundM1] = 0.25f*fInvDXDY*(
          9.0f*aaafF[iZ][iY0][iX0] - 12.0f*aaafF[iZ][iY0][iX1] +
          3.0f*aaafF[iZ][iY0][iX2] - 12.0f*aaafF[iZ][iY1][iX0] +
          16.0f*aaafF[iZ][iY1][iX1] - 4.0f*aaafF[iZ][iY1][iX2] +
          3.0f*aaafF[iZ][iY2][iX0] -  4.0f*aaafF[iZ][iY2][iX1] +
          aaafF[iZ][iY2][iX2]);

        // x-edges of z-slice
        for (iX = 1; iX < iXBoundM1; iX++)
        {
            aaafFXY[iZ][0][iX] = 0.25f*fInvDXDY*(
                3.0f*(aaafF[iZ][0][iX-1] - aaafF[iZ][0][iX+1]) -
                4.0f*(aaafF[iZ][1][iX-1] - aaafF[iZ][1][iX+1]) +
                (aaafF[iZ][2][iX-1] - aaafF[iZ][2][iX+1]));

            aaafFXY[iZ][iYBoundM1][iX] = 0.25f*fInvDXDY*(
                3.0f*(aaafF[iZ][iY0][iX-1] - aaafF[iZ][iY0][iX+1]) -
                4.0f*(aaafF[iZ][iY1][iX-1] - aaafF[iZ][iY1][iX+1]) +
                (aaafF[iZ][iY2][iX-1] - aaafF[iZ][iY2][iX+1]));
        }

        // y-edges of z-slice
        for (iY = 1; iY < iYBoundM1; iY++)
        {
            aaafFXY[iZ][iY][0] = 0.25f*fInvDXDY*(
                3.0f*(aaafF[iZ][iY-1][0] - aaafF[iZ][iY+1][0]) -
                4.0f*(aaafF[iZ][iY-1][1] - aaafF[iZ][iY+1][1]) +
                (aaafF[iZ][iY-1][2] - aaafF[iZ][iY+1][2]));

            aaafFXY[iZ][iY][iXBoundM1] = 0.25f*fInvDXDY*(
                3.0f*(aaafF[iZ][iY-1][iX0] - aaafF[iZ][iY+1][iX0]) -
                4.0f*(aaafF[iZ][iY-1][iX1] - aaafF[iZ][iY+1][iX1]) +
                (aaafF[iZ][iY-1][iX2] - aaafF[iZ][iY+1][iX2]));
        }

        // interior of z-slice
        for (iY = 1; iY < iYBoundM1; iY++)
        {
            for (iX = 1; iX < iXBoundM1; iX++)
            {
                aaafFXY[iZ][iY][iX] = 0.25f*fInvDXDY*(
                    aaafF[iZ][iY-1][iX-1] - aaafF[iZ][iY-1][iX+1] -
                    aaafF[iZ][iY+1][iX-1] + aaafF[iZ][iY+1][iX+1]);
            }
        }
    }

    for (iY = 0; iY < iYBound; iY++)
    {
        // corners of z-slice
        aaafFXZ[0][iY][0] = 0.25f*fInvDXDZ*(
            9.0f*aaafF[0][iY][0] - 12.0f*aaafF[0][iY][1] +
            3.0f*aaafF[0][iY][2] - 12.0f*aaafF[1][iY][0] +
            16.0f*aaafF[1][iY][1] - 4.0f*aaafF[1][iY][2] +
            3.0f*aaafF[2][iY][0] - 4.0f*aaafF[2][iY][1] +
            aaafF[2][iY][2]);

        aaafFXZ[0][iY][iXBoundM1] = 0.25f*fInvDXDZ*(
            9.0f*aaafF[0][iY][iX0] - 12.0f*aaafF[0][iY][iX1] +
            3.0f*aaafF[0][iY][iX2] - 12.0f*aaafF[1][iY][iX0] +
            16.0f*aaafF[1][iY][iX1] - 4.0f*aaafF[1][iY][iX2] +
            3.0f*aaafF[2][iY][iX0] - 4.0f*aaafF[2][iY][iX1] +
            aaafF[2][iY][iX2]);

        aaafFXZ[iZBoundM1][iY][0] = 0.25f*fInvDXDZ*(
            9.0f*aaafF[iZ0][iY][0] - 12.0f*aaafF[iZ0][iY][1] +
            3.0f*aaafF[iZ0][iY][2] - 12.0f*aaafF[iZ1][iY][0] +
            16.0f*aaafF[iZ1][iY][1] - 4.0f*aaafF[iZ1][iY][2] +
            3.0f*aaafF[iZ2][iY][0] - 4.0f*aaafF[iZ2][iY][1] +
            aaafF[iZ2][iY][2]);

        aaafFXZ[iZBoundM1][iY][iXBoundM1] = 0.25f*fInvDXDZ*(
          9.0f*aaafF[iZ0][iY][iX0] - 12.0f*aaafF[iZ0][iY][iX1] +
          3.0f*aaafF[iZ0][iY][iX2] - 12.0f*aaafF[iZ1][iY][iX0] +
          16.0f*aaafF[iZ1][iY][iX1] - 4.0f*aaafF[iZ1][iY][iX2] +
          3.0f*aaafF[iZ2][iY][iX0] -  4.0f*aaafF[iZ2][iY][iX1] +
          aaafF[iZ2][iY][iX2]);

        // x-edges of y-slice
        for (iX = 1; iX < iXBoundM1; iX++)
        {
            aaafFXZ[0][iY][iX] = 0.25f*fInvDXDZ*(
                3.0f*(aaafF[0][iY][iX-1] - aaafF[0][iY][iX+1]) -
                4.0f*(aaafF[1][iY][iX-1] - aaafF[1][iY][iX+1]) +
                (aaafF[2][iY][iX-1] - aaafF[2][iY][iX+1]));

            aaafFXZ[iZBoundM1][iY][iX] = 0.25f*fInvDXDZ*(
                3.0f*(aaafF[iZ0][iY][iX-1] - aaafF[iZ0][iY][iX+1]) -
                4.0f*(aaafF[iZ1][iY][iX-1] - aaafF[iZ1][iY][iX+1]) +
                (aaafF[iZ2][iY][iX-1] - aaafF[iZ2][iY][iX+1]));
        }

        // z-edges of y-slice
        for (iZ = 1; iZ < iZBoundM1; iZ++)
        {
            aaafFXZ[iZ][iY][0] = 0.25f*fInvDXDZ*(
                3.0f*(aaafF[iZ-1][iY][0] - aaafF[iZ+1][iY][0]) -
                4.0f*(aaafF[iZ-1][iY][1] - aaafF[iZ+1][iY][1]) +
                (aaafF[iZ-1][iY][2] - aaafF[iZ+1][iY][2]));

            aaafFXZ[iZ][iY][iXBoundM1] = 0.25f*fInvDXDZ*(
                3.0f*(aaafF[iZ-1][iY][iX0] - aaafF[iZ+1][iY][iX0]) -
                4.0f*(aaafF[iZ-1][iY][iX1] - aaafF[iZ+1][iY][iX1]) +
                (aaafF[iZ-1][iY][iX2] - aaafF[iZ+1][iY][iX2]));
        }

        // interior of y-slice
        for (iZ = 1; iZ < iZBoundM1; iZ++)
        {
            for (iX = 1; iX < iXBoundM1; iX++)
            {
                aaafFXZ[iZ][iY][iX] = 0.25f*fInvDXDZ*(
                    aaafF[iZ-1][iY][iX-1] - aaafF[iZ-1][iY][iX+1] -
                    aaafF[iZ+1][iY][iX-1] + aaafF[iZ+1][iY][iX+1]);
            }
        }
    }

    for (iX = 0; iX < iXBound; iX++)
    {
        // corners of x-slice
        aaafFYZ[0][0][iX] = 0.25f*fInvDYDZ*(
            9.0f*aaafF[0][0][iX] - 12.0f*aaafF[0][1][iX] +
            3.0f*aaafF[0][2][iX] - 12.0f*aaafF[1][0][iX] +
            16.0f*aaafF[1][1][iX] - 4.0f*aaafF[1][2][iX] +
            3.0f*aaafF[2][0][iX] - 4.0f*aaafF[2][1][iX] +
            aaafF[2][2][iX]);

        aaafFYZ[0][iYBoundM1][iX] = 0.25f*fInvDYDZ*(
            9.0f*aaafF[0][iY0][iX] - 12.0f*aaafF[0][iY1][iX] +
            3.0f*aaafF[0][iY2][iX] - 12.0f*aaafF[1][iY0][iX] +
            16.0f*aaafF[1][iY1][iX] - 4.0f*aaafF[1][iY2][iX] +
            3.0f*aaafF[2][iY0][iX] - 4.0f*aaafF[2][iY1][iX] +
            aaafF[2][iY2][iX]);

        aaafFYZ[iZBoundM1][0][iX] = 0.25f*fInvDYDZ*(
            9.0f*aaafF[iZ0][0][iX] - 12.0f*aaafF[iZ0][1][iX] +
            3.0f*aaafF[iZ0][2][iX] - 12.0f*aaafF[iZ1][0][iX] +
            16.0f*aaafF[iZ1][1][iX] - 4.0f*aaafF[iZ1][2][iX] +
            3.0f*aaafF[iZ2][0][iX] - 4.0f*aaafF[iZ2][1][iX] +
            aaafF[iZ2][2][iX]);

        aaafFYZ[iZBoundM1][iYBoundM1][iX] = 0.25f*fInvDYDZ*(
          9.0f*aaafF[iZ0][iY0][iX] - 12.0f*aaafF[iZ0][iY1][iX] +
          3.0f*aaafF[iZ0][iY2][iX] - 12.0f*aaafF[iZ1][iY0][iX] +
          16.0f*aaafF[iZ1][iY1][iX] - 4.0f*aaafF[iZ1][iY2][iX] +
          3.0f*aaafF[iZ2][iY0][iX] -  4.0f*aaafF[iZ2][iY1][iX] +
          aaafF[iZ2][iY2][iX]);

        // y-edges of x-slice
        for (iY = 1; iY < iYBoundM1; iY++)
        {
            aaafFYZ[0][iY][iX] = 0.25f*fInvDYDZ*(
                3.0f*(aaafF[0][iY-1][iX] - aaafF[0][iY+1][iX]) -
                4.0f*(aaafF[1][iY-1][iX] - aaafF[1][iY+1][iX]) +
                (aaafF[2][iY-1][iX] - aaafF[2][iY+1][iX]));

            aaafFYZ[iZBoundM1][iY][iX] = 0.25f*fInvDYDZ*(
                3.0f*(aaafF[iZ0][iY-1][iX] - aaafF[iZ0][iY+1][iX]) -
                4.0f*(aaafF[iZ1][iY-1][iX] - aaafF[iZ1][iY+1][iX]) +
                (aaafF[iZ2][iY-1][iX] - aaafF[iZ2][iY+1][iX]));
        }

        // z-edges of x-slice
        for (iZ = 1; iZ < iZBoundM1; iZ++)
        {
            aaafFYZ[iZ][0][iX] = 0.25f*fInvDYDZ*(
                3.0f*(aaafF[iZ-1][0][iX] - aaafF[iZ+1][0][iX]) -
                4.0f*(aaafF[iZ-1][1][iX] - aaafF[iZ+1][1][iX]) +
                (aaafF[iZ-1][2][iX] - aaafF[iZ+1][2][iX]));

            aaafFYZ[iZ][iYBoundM1][iX] = 0.25f*fInvDYDZ*(
                3.0f*(aaafF[iZ-1][iY0][iX] - aaafF[iZ+1][iY0][iX]) -
                4.0f*(aaafF[iZ-1][iY1][iX] - aaafF[iZ+1][iY1][iX]) +
                (aaafF[iZ-1][iY2][iX] - aaafF[iZ+1][iY2][iX]));
        }

        // interior of x-slice
        for (iZ = 1; iZ < iZBoundM1; iZ++)
        {
            for (iY = 1; iY < iYBoundM1; iY++)
            {
                aaafFYZ[iZ][iY][iX] = 0.25f*fInvDYDZ*(
                    aaafF[iZ-1][iY-1][iX] - aaafF[iZ-1][iY+1][iX] -
                    aaafF[iZ+1][iY-1][iX] + aaafF[iZ+1][iY+1][iX]);
            }
        }
    }

    // construct third-order derivatives
    Real*** aaafFXYZ;
    Allocate(iXBound,iYBound,iZBound,aaafFXYZ);

    // convolution masks
    Real afCDer[3] = { -0.5f, 0.0f, 0.5f };  // centered difference, O(h^2)
    Real afODer[3] = { -1.5f, 2.0f, -0.5f };  // one-sided difference, O(h^2)
    Real fMask;

    // corners
    aaafFXYZ[0][0][0] = 0.0f;
    aaafFXYZ[0][0][iXBoundM1] = 0.0f;
    aaafFXYZ[0][iYBoundM1][0] = 0.0f;
    aaafFXYZ[0][iYBoundM1][iXBoundM1] = 0.0f;
    aaafFXYZ[iZBoundM1][0][0] = 0.0f;
    aaafFXYZ[iZBoundM1][0][iXBoundM1] = 0.0f;
    aaafFXYZ[iZBoundM1][iYBoundM1][0] = 0.0f;
    aaafFXYZ[iZBoundM1][iYBoundM1][iXBoundM1] = 0.0f;
    for (iZ = 0; iZ <= 2; iZ++)
    {
        for (iY = 0; iY <= 2; iY++)
        {
            for (iX = 0; iX <= 2; iX++)
            {
                fMask = fInvDXDYDZ*afODer[iX]*afODer[iY]*afODer[iZ];

                aaafFXYZ[0][0][0] += fMask*
                    aaafF[iZ][iY][iX];

                aaafFXYZ[0][0][iXBoundM1] += fMask*
                    aaafF[iZ][iY][iXBoundM1-iX];

                aaafFXYZ[0][iYBoundM1][0] += fMask*
                    aaafF[iZ][iYBoundM1-iY][iX];

                aaafFXYZ[0][iYBoundM1][iXBoundM1] += fMask*
                    aaafF[iZ][iYBoundM1-iY][iXBoundM1-iX];

                aaafFXYZ[iZBoundM1][0][0] += fMask*
                    aaafF[iZBoundM1-iZ][iY][iX];

                aaafFXYZ[iZBoundM1][0][iXBoundM1] += fMask*
                    aaafF[iZBoundM1-iZ][iY][iXBoundM1-iX];

                aaafFXYZ[iZBoundM1][iYBoundM1][0] += fMask*
                    aaafF[iZBoundM1-iZ][iYBoundM1-iY][iX];

                aaafFXYZ[iZBoundM1][iYBoundM1][iXBoundM1] += fMask*
                    aaafF[iZBoundM1-iZ][iYBoundM1-iY][iXBoundM1-iX];
            }
        }
    }

    // x-edges
    for (iX0 = 1; iX0 < iXBoundM1; iX0++)
    {
        aaafFXYZ[0][0][iX0] = 0.0f;
        aaafFXYZ[0][iYBoundM1][iX0] = 0.0f;
        aaafFXYZ[iZBoundM1][0][iX0] = 0.0f;
        aaafFXYZ[iZBoundM1][iYBoundM1][iX0] = 0.0f;
        for (iZ = 0; iZ <= 2; iZ++)
        {
            for (iY = 0; iY <= 2; iY++)
            {
                for (iX = 0; iX <= 2; iX++)
                {
                    fMask = fInvDXDYDZ*afCDer[iX]*afODer[iY]*afODer[iZ];

                    aaafFXYZ[0][0][iX0] += fMask*
                        aaafF[iZ][iY][iX0+iX-1];

                    aaafFXYZ[0][iYBoundM1][iX0] += fMask*
                        aaafF[iZ][iYBoundM1-iY][iX0+iX-1];

                    aaafFXYZ[iZBoundM1][0][iX0] += fMask*
                        aaafF[iZBoundM1-iZ][iY][iX0+iX-1];

                    aaafFXYZ[iZBoundM1][iYBoundM1][iX0] += fMask*
                        aaafF[iZBoundM1-iZ][iYBoundM1-iY][iX0+iX-1];
                }
            }
        }
    }

    // y-edges
    for (iY0 = 1; iY0 < iYBoundM1; iY0++)
    {
        aaafFXYZ[0][iY0][0] = 0.0f;
        aaafFXYZ[0][iY0][iXBoundM1] = 0.0f;
        aaafFXYZ[iZBoundM1][iY0][0] = 0.0f;
        aaafFXYZ[iZBoundM1][iY0][iXBoundM1] = 0.0f;
        for (iZ = 0; iZ <= 2; iZ++)
        {
            for (iY = 0; iY <= 2; iY++)
            {
                for (iX = 0; iX <= 2; iX++)
                {
                    fMask = fInvDXDYDZ*afODer[iX]*afCDer[iY]*afODer[iZ];

                    aaafFXYZ[0][iY0][0] += fMask*
                        aaafF[iZ][iY0+iY-1][iX];

                    aaafFXYZ[0][iY0][iXBoundM1] += fMask*
                        aaafF[iZ][iY0+iY-1][iXBoundM1-iX];

                    aaafFXYZ[iZBoundM1][iY0][0] += fMask*
                        aaafF[iZBoundM1-iZ][iY0+iY-1][iX];

                    aaafFXYZ[iZBoundM1][iY0][iXBoundM1] += fMask*
                        aaafF[iZBoundM1-iZ][iY0+iY-1][iXBoundM1-iX];
                }
            }
        }
    }

    // z-edges
    for (iZ0 = 1; iZ0 < iZBoundM1; iZ0++)
    {
        aaafFXYZ[iZ0][0][0] = 0.0f;
        aaafFXYZ[iZ0][0][iXBoundM1] = 0.0f;
        aaafFXYZ[iZ0][iYBoundM1][0] = 0.0f;
        aaafFXYZ[iZ0][iYBoundM1][iXBoundM1] = 0.0f;
        for (iZ = 0; iZ <= 2; iZ++)
        {
            for (iY = 0; iY <= 2; iY++)
            {
                for (iX = 0; iX <= 2; iX++)
                {
                    fMask = fInvDXDYDZ*afODer[iX]*afODer[iY]*afCDer[iZ];

                    aaafFXYZ[iZ0][0][0] += fMask*
                        aaafF[iZ0+iZ-1][iY][iX];

                    aaafFXYZ[iZ0][0][iXBoundM1] += fMask*
                        aaafF[iZ0+iZ-1][iY][iXBoundM1-iX];

                    aaafFXYZ[iZ0][iYBoundM1][0] += fMask*
                        aaafF[iZ0+iZ-1][iYBoundM1-iY][iX];

                    aaafFXYZ[iZ0][iYBoundM1][iXBoundM1] += fMask*
                        aaafF[iZ0+iZ-1][iYBoundM1-iY][iXBoundM1-iX];
                }
            }
        }
    }

    // xy-faces
    for (iY0 = 1; iY0 < iYBoundM1; iY0++)
    {
        for (iX0 = 1; iX0 < iXBoundM1; iX0++)
        {
            aaafFXYZ[0][iY0][iX0] = 0.0f;
            aaafFXYZ[iZBoundM1][iY0][iX0] = 0.0f;
            for (iZ = 0; iZ <= 2; iZ++)
            {
                for (iY = 0; iY <= 2; iY++)
                {
                    for (iX = 0; iX <= 2; iX++)
                    {
                        fMask = fInvDXDYDZ*afCDer[iX]*afCDer[iY]*afODer[iZ];

                        aaafFXYZ[0][iY0][iX0] += fMask*
                            aaafF[iZ][iY0+iY-1][iX0+iX-1];

                        aaafFXYZ[iZBoundM1][iY0][iX0] += fMask*
                            aaafF[iZBoundM1-iZ][iY0+iY-1][iX0+iX-1];
                    }
                }
            }
        }
    }

    // xz-faces
    for (iZ0 = 1; iZ0 < iZBoundM1; iZ0++)
    {
        for (iX0 = 1; iX0 < iXBoundM1; iX0++)
        {
            aaafFXYZ[iZ0][0][iX0] = 0.0f;
            aaafFXYZ[iZ0][iYBoundM1][iX0] = 0.0f;
            for (iZ = 0; iZ <= 2; iZ++)
            {
                for (iY = 0; iY <= 2; iY++)
                {
                    for (iX = 0; iX <= 2; iX++)
                    {
                        fMask = fInvDXDYDZ*afCDer[iX]*afODer[iY]*afCDer[iZ];

                        aaafFXYZ[iZ0][0][iX0] += fMask*
                            aaafF[iZ0+iZ-1][iY][iX0+iX-1];

                        aaafFXYZ[iZ0][iYBoundM1][iX0] += fMask*
                            aaafF[iZ0+iZ-1][iYBoundM1-iY][iX0+iX-1];
                    }
                }
            }
        }
    }

    // yz-faces
    for (iZ0 = 1; iZ0 < iZBoundM1; iZ0++)
    {
        for (iY0 = 1; iY0 < iYBoundM1; iY0++)
        {
            aaafFXYZ[iZ0][iY0][0] = 0.0f;
            aaafFXYZ[iZ0][iY0][iXBoundM1] = 0.0f;
            for (iZ = 0; iZ <= 2; iZ++)
            {
                for (iY = 0; iY <= 2; iY++)
                {
                    for (iX = 0; iX <= 2; iX++)
                    {
                        fMask = fInvDXDYDZ*afODer[iX]*afCDer[iY]*afCDer[iZ];

                        aaafFXYZ[iZ0][iY0][0] += fMask*
                            aaafF[iZ0+iZ-1][iY0+iY-1][iX];

                        aaafFXYZ[iZ0][iY0][iXBoundM1] += fMask*
                            aaafF[iZ0+iZ-1][iY0+iY-1][iXBoundM1-iX];
                    }
                }
            }
        }
    }

    // interiors
    for (iZ0 = 1; iZ0 < iZBoundM1; iZ0++)
    {
        for (iY0 = 1; iY0 < iYBoundM1; iY0++)
        {
            for (iX0 = 1; iX0 < iXBoundM1; iX0++)
            {
                aaafFXYZ[iZ0][iY0][iX0] = 0.0f;

                for (iZ = 0; iZ <= 2; iZ++)
                {
                    for (iY = 0; iY <= 2; iY++)
                    {
                        for (iX = 0; iX <= 2; iX++)
                        {
                            fMask = fInvDXDYDZ*afCDer[iX]*afCDer[iY]*
                                afCDer[iZ];

                            aaafFXYZ[iZ0][iY0][iX0] += fMask*
                                aaafF[iZ0+iZ-1][iY0+iY-1][iX0+iX-1];
                        }
                    }
                }
            }
        }
    }

    // construct polynomials
    Allocate(iXBoundM1,iYBoundM1,iZBoundM1,m_aaakPoly);
    for (iZ = 0; iZ < iZBoundM1; iZ++)
    {
        for (iY = 0; iY < iYBoundM1; iY++)
        {
            for (iX = 0; iX < iXBoundM1; iX++)
            {
                // Note the 'transposing' of the 2x2x2 blocks (to match
                // notation used in the polynomial definition).
                Real aaafG[2][2][2] =
                {
                    aaafF[iZ][iY][iX],
                    aaafF[iZ+1][iY][iX],
                    aaafF[iZ][iY+1][iX],
                    aaafF[iZ+1][iY+1][iX],
                    aaafF[iZ][iY][iX+1],
                    aaafF[iZ+1][iY][iX+1],
                    aaafF[iZ][iY+1][iX+1],
                    aaafF[iZ+1][iY+1][iX+1],
                };

                Real aaafGX[2][2][2] =
                {
                    aaafFX[iZ][iY][iX],
                    aaafFX[iZ+1][iY][iX],
                    aaafFX[iZ][iY+1][iX],
                    aaafFX[iZ+1][iY+1][iX],
                    aaafFX[iZ][iY][iX+1],
                    aaafFX[iZ+1][iY][iX+1],
                    aaafFX[iZ][iY+1][iX+1],
                    aaafFX[iZ+1][iY+1][iX+1],
                };

                Real aaafGY[2][2][2] =
                {
                    aaafFY[iZ][iY][iX],
                    aaafFY[iZ+1][iY][iX],
                    aaafFY[iZ][iY+1][iX],
                    aaafFY[iZ+1][iY+1][iX],
                    aaafFY[iZ][iY][iX+1],
                    aaafFY[iZ+1][iY][iX+1],
                    aaafFY[iZ][iY+1][iX+1],
                    aaafFY[iZ+1][iY+1][iX+1],
                };

                Real aaafGZ[2][2][2] =
                {
                    aaafFZ[iZ][iY][iX],
                    aaafFZ[iZ+1][iY][iX],
                    aaafFZ[iZ][iY+1][iX],
                    aaafFZ[iZ+1][iY+1][iX],
                    aaafFZ[iZ][iY][iX+1],
                    aaafFZ[iZ+1][iY][iX+1],
                    aaafFZ[iZ][iY+1][iX+1],
                    aaafFZ[iZ+1][iY+1][iX+1],
                };

                Real aaafGXY[2][2][2] =
                {
                    aaafFXY[iZ][iY][iX],
                    aaafFXY[iZ+1][iY][iX],
                    aaafFXY[iZ][iY+1][iX],
                    aaafFXY[iZ+1][iY+1][iX],
                    aaafFXY[iZ][iY][iX+1],
                    aaafFXY[iZ+1][iY][iX+1],
                    aaafFXY[iZ][iY+1][iX+1],
                    aaafFXY[iZ+1][iY+1][iX+1],
                };

                Real aaafGXZ[2][2][2] =
                {
                    aaafFXZ[iZ][iY][iX],
                    aaafFXZ[iZ+1][iY][iX],
                    aaafFXZ[iZ][iY+1][iX],
                    aaafFXZ[iZ+1][iY+1][iX],
                    aaafFXZ[iZ][iY][iX+1],
                    aaafFXZ[iZ+1][iY][iX+1],
                    aaafFXZ[iZ][iY+1][iX+1],
                    aaafFXZ[iZ+1][iY+1][iX+1],
                };

                Real aaafGYZ[2][2][2] =
                {
                    aaafFYZ[iZ][iY][iX],
                    aaafFYZ[iZ+1][iY][iX],
                    aaafFYZ[iZ][iY+1][iX],
                    aaafFYZ[iZ+1][iY+1][iX],
                    aaafFYZ[iZ][iY][iX+1],
                    aaafFYZ[iZ+1][iY][iX+1],
                    aaafFYZ[iZ][iY+1][iX+1],
                    aaafFYZ[iZ+1][iY+1][iX+1],
                };

                Real aaafGXYZ[2][2][2] =
                {
                    aaafFXYZ[iZ][iY][iX],
                    aaafFXYZ[iZ+1][iY][iX],
                    aaafFXYZ[iZ][iY+1][iX],
                    aaafFXYZ[iZ+1][iY+1][iX],
                    aaafFXYZ[iZ][iY][iX+1],
                    aaafFXYZ[iZ+1][iY][iX+1],
                    aaafFXYZ[iZ][iY+1][iX+1],
                    aaafFXYZ[iZ+1][iY+1][iX+1],
                };

                Construct(m_aaakPoly[iZ][iY][iX],aaafG,aaafGX,aaafGY,
                    aaafGZ,aaafGXY,aaafGXZ,aaafGYZ,aaafGXYZ);
            }
        }
    }

    Deallocate(iYBound,iZBound,aaafXSlope);
    Deallocate(iXBound,iZBound,aaafYSlope);
    Deallocate(iXBound,iYBound,aaafZSlope);
    Deallocate(iYBound,iZBound,aaafFX);
    Deallocate(iYBound,iZBound,aaafFY);
    Deallocate(iYBound,iZBound,aaafFZ);
    Deallocate(iYBound,iZBound,aaafFXY);
    Deallocate(iYBound,iZBound,aaafFXZ);
    Deallocate(iYBound,iZBound,aaafFYZ);
    Deallocate(iYBound,iZBound,aaafFXYZ);
}
//----------------------------------------------------------------------------
Interp3DAkimaUniform::~Interp3DAkimaUniform ()
{
    Deallocate(m_iYBound-1,m_iZBound-1,m_aaakPoly);
}
//----------------------------------------------------------------------------
Real Interp3DAkimaUniform::ComputeDerivative (Real* afSlope) const
{
    if ( afSlope[1] != afSlope[2] )
    {
        if ( afSlope[0] != afSlope[1] )
        {
            if ( afSlope[2] != afSlope[3] )
            {
                Real fAD0 = Math::FAbs(afSlope[3] - afSlope[2]);
                Real fAD1 = Math::FAbs(afSlope[0] - afSlope[1]);
                return (fAD0*afSlope[1]+fAD1*afSlope[2])/(fAD0+fAD1);
            }
            else
            {
                return afSlope[2];
            }
        }
        else
        {
            if ( afSlope[2] != afSlope[3] )
            {
                return afSlope[1];
            }
            else
            {
                return 0.5f*(afSlope[1]+afSlope[2]);
            }
        }
    }
    else
    {
        return afSlope[1];
    }
}
//----------------------------------------------------------------------------
void Interp3DAkimaUniform::Construct (Polynomial& rkPoly,
    Real aaafF[2][2][2], Real aaafFX[2][2][2], Real aaafFY[2][2][2],
    Real aaafFZ[2][2][2], Real aaafFXY[2][2][2], Real aaafFXZ[2][2][2],
    Real aaafFYZ[2][2][2], Real aaafFXYZ[2][2][2])
{
    Real fDX = m_fXSpacing, fDY = m_fYSpacing, fDZ = m_fZSpacing;
    Real fInvDX = 1.0f/fDX, fInvDX2 = fInvDX*fInvDX;
    Real fInvDY = 1.0f/fDY, fInvDY2 = fInvDY*fInvDY;
    Real fInvDZ = 1.0f/fDZ, fInvDZ2 = fInvDZ*fInvDZ;
    Real fB0, fB1, fB2, fB3, fB4, fB5, fB6, fB7;

    rkPoly.A(0,0,0) = aaafF[0][0][0];
    rkPoly.A(1,0,0) = aaafFX[0][0][0];
    rkPoly.A(0,1,0) = aaafFY[0][0][0];
    rkPoly.A(0,0,1) = aaafFZ[0][0][0];
    rkPoly.A(1,1,0) = aaafFXY[0][0][0];
    rkPoly.A(1,0,1) = aaafFXZ[0][0][0];
    rkPoly.A(0,1,1) = aaafFYZ[0][0][0];
    rkPoly.A(1,1,1) = aaafFXYZ[0][0][0];

    // solve for Aij0
    fB0 = (aaafF[1][0][0] - rkPoly(0,0,0,fDX,0.0f,0.0f))*fInvDX2;
    fB1 = (aaafFX[1][0][0] - rkPoly(1,0,0,fDX,0.0f,0.0f))*fInvDX;
    rkPoly.A(2,0,0) = 3.0f*fB0 - fB1;
    rkPoly.A(3,0,0) = (-2.0f*fB0 + fB1)*fInvDX;

    fB0 = (aaafF[0][1][0] - rkPoly(0,0,0,0.0f,fDY,0.0f))*fInvDY2;
    fB1 = (aaafFY[0][1][0] - rkPoly(0,1,0,0.0f,fDY,0.0f))*fInvDY;
    rkPoly.A(0,2,0) = 3.0f*fB0 - fB1;
    rkPoly.A(0,3,0) = (-2.0f*fB0 + fB1)*fInvDY;

    fB0 = (aaafFY[1][0][0] - rkPoly(0,1,0,fDX,0.0f,0.0f))*fInvDX2;
    fB1 = (aaafFXY[1][0][0] - rkPoly(1,1,0,fDX,0.0f,0.0f))*fInvDX;
    rkPoly.A(2,1,0) = 3.0f*fB0 - fB1;
    rkPoly.A(3,1,0) = (-2.0f*fB0 + fB1)*fInvDX;

    fB0 = (aaafFX[0][1][0] - rkPoly(1,0,0,0.0f,fDY,0.0f))*fInvDY2;
    fB1 = (aaafFXY[0][1][0] - rkPoly(1,1,0,0.0f,fDY,0.0f))*fInvDY;
    rkPoly.A(1,2,0) = 3.0f*fB0 - fB1;
    rkPoly.A(1,3,0) = (-2.0f*fB0 + fB1)*fInvDY;

    fB0 = (aaafF[1][1][0] - rkPoly(0,0,0,fDX,fDY,0.0f))*fInvDX2*fInvDY2;
    fB1 = (aaafFX[1][1][0] - rkPoly(1,0,0,fDX,fDY,0.0f))*fInvDX*fInvDY2;
    fB2 = (aaafFY[1][1][0] - rkPoly(0,1,0,fDX,fDY,0.0f))*fInvDX2*fInvDY;
    fB3 = (aaafFXY[1][1][0] - rkPoly(1,1,0,fDX,fDY,0.0f))*fInvDX*fInvDY;
    rkPoly.A(2,2,0) = 9.0f*fB0 - 3.0f*fB1 - 3.0f*fB2 + fB3;
    rkPoly.A(3,2,0) = (-6.0f*fB0 + 3.0f*fB1 + 2.0f*fB2 - fB3)*fInvDX;
    rkPoly.A(2,3,0) = (-6.0f*fB0 + 2.0f*fB1 + 3.0f*fB2 - fB3)*fInvDY;
    rkPoly.A(3,3,0) = (4.0f*fB0 - 2.0f*fB1 - 2.0f*fB2 + fB3)*fInvDX*fInvDY;

    // solve for Ai0k
    fB0 = (aaafF[0][0][1] - rkPoly(0,0,0,0.0f,0.0f,fDZ))*fInvDZ2;
    fB1 = (aaafFZ[0][0][1] - rkPoly(0,0,1,0.0f,0.0f,fDZ))*fInvDZ;
    rkPoly.A(0,0,2) = 3.0f*fB0 - fB1;
    rkPoly.A(0,0,3) = (-2.0f*fB0 + fB1)*fInvDZ;

    fB0 = (aaafFZ[1][0][0] - rkPoly(0,0,1,fDX,0.0f,0.0f))*fInvDX2;
    fB1 = (aaafFXZ[1][0][0] - rkPoly(1,0,1,fDX,0.0f,0.0f))*fInvDX;
    rkPoly.A(2,0,1) = 3.0f*fB0 - fB1;
    rkPoly.A(3,0,1) = (-2.0f*fB0 + fB1)*fInvDX;

    fB0 = (aaafFX[0][0][1] - rkPoly(1,0,0,0.0f,0.0f,fDZ))*fInvDZ2;
    fB1 = (aaafFXZ[0][0][1] - rkPoly(1,0,1,0.0f,0.0f,fDZ))*fInvDZ;
    rkPoly.A(1,0,2) = 3.0f*fB0 - fB1;
    rkPoly.A(1,0,3) = (-2.0f*fB0 + fB1)*fInvDZ;

    fB0 = (aaafF[1][0][1] - rkPoly(0,0,0,fDX,0.0f,fDZ))*fInvDX2*fInvDZ2;
    fB1 = (aaafFX[1][0][1] - rkPoly(1,0,0,fDX,0.0f,fDZ))*fInvDX*fInvDZ2;
    fB2 = (aaafFZ[1][0][1] - rkPoly(0,0,1,fDX,0.0f,fDZ))*fInvDX2*fInvDZ;
    fB3 = (aaafFXZ[1][0][1] - rkPoly(1,0,1,fDX,0.0f,fDZ))*fInvDX*fInvDZ;
    rkPoly.A(2,0,2) = 9.0f*fB0 - 3.0f*fB1 - 3.0f*fB2 + fB3;
    rkPoly.A(3,0,2) = (-6.0f*fB0 + 3.0f*fB1 + 2.0f*fB2 - fB3)*fInvDX;
    rkPoly.A(2,0,3) = (-6.0f*fB0 + 2.0f*fB1 + 3.0f*fB2 - fB3)*fInvDZ;
    rkPoly.A(3,0,3) = (4.0f*fB0 - 2.0f*fB1 - 2.0f*fB2 + fB3)*fInvDX*fInvDZ;

    // solve for A0jk
    fB0 = (aaafFZ[0][1][0] - rkPoly(0,0,1,0.0f,fDY,0.0f))*fInvDY2;
    fB1 = (aaafFYZ[0][1][0] - rkPoly(0,1,1,0.0f,fDY,0.0f))*fInvDY;
    rkPoly.A(0,2,1) = 3.0f*fB0 - fB1;
    rkPoly.A(0,3,1) = (-2.0f*fB0 + fB1)*fInvDY;

    fB0 = (aaafFY[0][0][1] - rkPoly(0,1,0,0.0f,0.0f,fDZ))*fInvDZ2;
    fB1 = (aaafFYZ[0][0][1] - rkPoly(0,1,1,0.0f,0.0f,fDZ))*fInvDZ;
    rkPoly.A(0,1,2) = 3.0f*fB0 - fB1;
    rkPoly.A(0,1,3) = (-2.0f*fB0 + fB1)*fInvDZ;

    fB0 = (aaafF[0][1][1] - rkPoly(0,0,0,0.0f,fDY,fDZ))*fInvDY2*fInvDZ2;
    fB1 = (aaafFY[0][1][1] - rkPoly(0,1,0,0.0f,fDY,fDZ))*fInvDY*fInvDZ2;
    fB2 = (aaafFZ[0][1][1] - rkPoly(0,0,1,0.0f,fDY,fDZ))*fInvDY2*fInvDZ;
    fB3 = (aaafFYZ[0][1][1] - rkPoly(0,1,1,0.0f,fDY,fDZ))*fInvDY*fInvDZ;
    rkPoly.A(0,2,2) = 9.0f*fB0 - 3.0f*fB1 - 3.0f*fB2 + fB3;
    rkPoly.A(0,3,2) = (-6.0f*fB0 + 3.0f*fB1 + 2.0f*fB2 - fB3)*fInvDY;
    rkPoly.A(0,2,3) = (-6.0f*fB0 + 2.0f*fB1 + 3.0f*fB2 - fB3)*fInvDZ;
    rkPoly.A(0,3,3) = (4.0f*fB0 - 2.0f*fB1 - 2.0f*fB2 + fB3)*fInvDY*fInvDZ;

    // solve for Aij1
    fB0 = (aaafFYZ[1][0][0] - rkPoly(0,1,1,fDX,0.0f,0.0f))*fInvDX2;
    fB1 = (aaafFXYZ[1][0][0] - rkPoly(1,1,1,fDX,0.0f,0.0f))*fInvDX;
    rkPoly.A(2,1,1) = 3.0f*fB0 - fB1;
    rkPoly.A(3,1,1) = (-2.0f*fB0 + fB1)*fInvDX;

    fB0 = (aaafFXZ[0][1][0] - rkPoly(1,0,1,0.0f,fDY,0.0f))*fInvDY2;
    fB1 = (aaafFXYZ[0][1][0] - rkPoly(1,1,1,0.0f,fDY,0.0f))*fInvDY;
    rkPoly.A(1,2,1) = 3.0f*fB0 - fB1;
    rkPoly.A(1,3,1) = (-2.0f*fB0 + fB1)*fInvDY;

    fB0 = (aaafFZ[1][1][0] - rkPoly(0,0,1,fDX,fDY,0.0f))*fInvDX2*fInvDY2;
    fB1 = (aaafFXZ[1][1][0] - rkPoly(1,0,1,fDX,fDY,0.0f))*fInvDX*fInvDY2;
    fB2 = (aaafFYZ[1][1][0] - rkPoly(0,1,1,fDX,fDY,0.0f))*fInvDX2*fInvDY;
    fB3 = (aaafFXYZ[1][1][0] - rkPoly(1,1,1,fDX,fDY,0.0f))*fInvDX*fInvDY;
    rkPoly.A(2,2,1) = 9.0f*fB0 - 3.0f*fB1 - 3.0f*fB2 + fB3;
    rkPoly.A(3,2,1) = (-6.0f*fB0 + 3.0f*fB1 + 2.0f*fB2 - fB3)*fInvDX;
    rkPoly.A(2,3,1) = (-6.0f*fB0 + 2.0f*fB1 + 3.0f*fB2 - fB3)*fInvDY;
    rkPoly.A(3,3,1) = (4.0f*fB0 - 2.0f*fB1 - 2.0f*fB2 + fB3)*fInvDX*fInvDY;

    // solve for Ai1k
    fB0 = (aaafFXY[0][0][1] - rkPoly(1,1,0,0.0f,0.0f,fDZ))*fInvDZ2;
    fB1 = (aaafFXYZ[0][0][1] - rkPoly(1,1,1,0.0f,0.0f,fDZ))*fInvDZ;
    rkPoly.A(1,1,2) = 3.0f*fB0 - fB1;
    rkPoly.A(1,1,3) = (-2.0f*fB0 + fB1)*fInvDZ;

    fB0 = (aaafFY[1][0][1] - rkPoly(0,1,0,fDX,0.0f,fDZ))*fInvDX2*fInvDZ2;
    fB1 = (aaafFXY[1][0][1] - rkPoly(1,1,0,fDX,0.0f,fDZ))*fInvDX*fInvDZ2;
    fB2 = (aaafFYZ[1][0][1] - rkPoly(0,1,1,fDX,0.0f,fDZ))*fInvDX2*fInvDZ;
    fB3 = (aaafFXYZ[1][0][1] - rkPoly(1,1,1,fDX,0.0f,fDZ))*fInvDX*fInvDZ;
    rkPoly.A(2,1,2) = 9.0f*fB0 - 3.0f*fB1 - 3.0f*fB2 + fB3;
    rkPoly.A(3,1,2) = (-6.0f*fB0 + 3.0f*fB1 + 2.0f*fB2 - fB3)*fInvDX;
    rkPoly.A(2,1,3) = (-6.0f*fB0 + 2.0f*fB1 + 3.0f*fB2 - fB3)*fInvDZ;
    rkPoly.A(3,1,3) = (4.0f*fB0 - 2.0f*fB1 - 2.0f*fB2 + fB3)*fInvDX*fInvDZ;

    // solve for A1jk
    fB0 = (aaafFX[0][1][1] - rkPoly(1,0,0,0.0f,fDY,fDZ))*fInvDY2*fInvDZ2;
    fB1 = (aaafFXY[0][1][1] - rkPoly(1,1,0,0.0f,fDY,fDZ))*fInvDY*fInvDZ2;
    fB2 = (aaafFXZ[0][1][1] - rkPoly(1,0,1,0.0f,fDY,fDZ))*fInvDY2*fInvDZ;
    fB3 = (aaafFXYZ[0][1][1] - rkPoly(1,1,1,0.0f,fDY,fDZ))*fInvDY*fInvDZ;
    rkPoly.A(1,2,2) = 9.0f*fB0 - 3.0f*fB1 - 3.0f*fB2 + fB3;
    rkPoly.A(1,3,2) = (-6.0f*fB0 + 3.0f*fB1 + 2.0f*fB2 - fB3)*fInvDY;
    rkPoly.A(1,2,3) = (-6.0f*fB0 + 2.0f*fB1 + 3.0f*fB2 - fB3)*fInvDZ;
    rkPoly.A(1,3,3) = (4.0f*fB0 - 2.0f*fB1 - 2.0f*fB2 + fB3)*fInvDY*fInvDZ;

    // solve for remaining Aijk with i >= 2, j >= 2, k >= 2
    fB0 = (aaafF[1][1][1]-rkPoly(0,0,0,fDX,fDY,fDZ))*fInvDX2*fInvDY2*fInvDZ2;
    fB1 = (aaafFX[1][1][1]-rkPoly(1,0,0,fDX,fDY,fDZ))*fInvDX*fInvDY2*fInvDZ2;
    fB2 = (aaafFY[1][1][1]-rkPoly(0,1,0,fDX,fDY,fDZ))*fInvDX2*fInvDY*fInvDZ2;
    fB3 = (aaafFZ[1][1][1]-rkPoly(0,0,1,fDX,fDY,fDZ))*fInvDX2*fInvDY2*fInvDZ;
    fB4 = (aaafFXY[1][1][1]-rkPoly(1,1,0,fDX,fDY,fDZ))*fInvDX*fInvDY*fInvDZ2;
    fB5 = (aaafFXZ[1][1][1]-rkPoly(1,0,1,fDX,fDY,fDZ))*fInvDX*fInvDY2*fInvDZ;
    fB6 = (aaafFYZ[1][1][1]-rkPoly(0,1,1,fDX,fDY,fDZ))*fInvDX2*fInvDY*fInvDZ;
    fB7 = (aaafFXYZ[1][1][1]-rkPoly(1,1,1,fDX,fDY,fDZ))*fInvDX*fInvDY*fInvDZ;
    rkPoly.A(2,2,2) = 27.0f*fB0 - 9.0f*fB1 - 9.0f*fB2 - 9.0f*fB3 + 3.0f*fB4 +
        3.0f*fB5 + 3.0f*fB6 - fB7;
    rkPoly.A(3,2,2) = (-18.0f*fB0 + 9.0f*fB1 + 6.0f*fB2 + 6.0f*fB3 - 3.0f*fB4
        - 3.0f*fB5 - 2.0f*fB6 + fB7)*fInvDX;
    rkPoly.A(2,3,2) = (-18.0f*fB0 + 6.0f*fB1 + 9.0f*fB2 + 6.0f*fB3 - 3.0f*fB4
        - 2.0f*fB5 - 3.0f*fB6 + fB7)*fInvDY;
    rkPoly.A(2,2,3) = (-18.0f*fB0 + 6.0f*fB1 + 6.0f*fB2 + 9.0f*fB3 - 2.0f*fB4
        - 3.0f*fB5 - 3.0f*fB6 + fB7)*fInvDZ;
    rkPoly.A(3,3,2) = (12.0f*fB0 - 6.0f*fB1 - 6.0f*fB2 - 4.0f*fB3 + 3.0f*fB4
        + 2.0f*fB5 + 2.0f*fB6 - fB7)*fInvDX*fInvDY;
    rkPoly.A(3,2,3) = (12.0f*fB0 - 6.0f*fB1 - 4.0f*fB2 - 6.0f*fB3 + 2.0f*fB4
        + 3.0f*fB5 + 2.0f*fB6 - fB7)*fInvDX*fInvDZ;
    rkPoly.A(2,3,3) = (12.0f*fB0 - 4.0f*fB1 - 6.0f*fB2 - 6.0f*fB3 + 2.0f*fB4
        + 2.0f*fB5 + 3.0f*fB6 - fB7)*fInvDY*fInvDZ;
    rkPoly.A(3,3,3) = (-8.0f*fB0 + 4.0f*fB1 + 4.0f*fB2 + 4.0f*fB3 - 2.0f*fB4
        - 2.0f*fB5 - 2.0f*fB6 + fB7)*fInvDX*fInvDY*fInvDZ;
}
//----------------------------------------------------------------------------
bool Interp3DAkimaUniform::XLookup (Real fX, int& riXIndex, Real& rfDX)
    const
{
    if ( fX >= m_fXMin )
    {
        if ( fX <= m_fXMax )
        {
            for (riXIndex = 0; riXIndex+1 < m_iXBound; riXIndex++)
            {
                if ( fX < m_fXMin + m_fXSpacing*(riXIndex+1) )
                {
                    rfDX = fX - (m_fXMin + m_fXSpacing*riXIndex);
                    return true;
                }
            }

            riXIndex--;
            rfDX = fX - (m_fXMin + m_fXSpacing*riXIndex);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool Interp3DAkimaUniform::YLookup (Real fY, int& riYIndex, Real& rfDY)
    const
{
    if ( fY >= m_fYMin )
    {
        if ( fY <= m_fYMax )
        {
            for (riYIndex = 0; riYIndex+1 < m_iYBound; riYIndex++)
            {
                if ( fY < m_fYMin + m_fYSpacing*(riYIndex+1) )
                {
                    rfDY = fY - (m_fYMin + m_fYSpacing*riYIndex);
                    return true;
                }
            }

            riYIndex--;
            rfDY = fY - (m_fYMin + m_fYSpacing*riYIndex);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
bool Interp3DAkimaUniform::ZLookup (Real fZ, int& riZIndex, Real& rfDZ)
    const
{
    if ( fZ >= m_fZMin )
    {
        if ( fZ <= m_fZMax )
        {
            for (riZIndex = 0; riZIndex+1 < m_iZBound; riZIndex++)
            {
                if ( fZ < m_fZMin + m_fZSpacing*(riZIndex+1) )
                {
                    rfDZ = fZ - (m_fZMin + m_fZSpacing*riZIndex);
                    return true;
                }
            }

            riZIndex--;
            rfDZ = fZ - (m_fZMin + m_fZSpacing*riZIndex);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
Real Interp3DAkimaUniform::operator() (Real fX, Real fY, Real fZ) const
{
    int iX, iY, iZ;
    Real fDX, fDY, fDZ;

    if ( XLookup(fX,iX,fDX) && YLookup(fY,iY,fDY) && ZLookup(fZ,iZ,fDZ) )
        return m_aaakPoly[iZ][iY][iX](fDX,fDY,fDZ);
    else
        return Math::MAX_REAL;
}
//----------------------------------------------------------------------------
Real Interp3DAkimaUniform::operator() (int iXOrder, int iYOrder, int iZOrder,
    Real fX, Real fY, Real fZ) const
{
    int iX, iY, iZ;
    Real fDX, fDY, fDZ;

    if ( XLookup(fX,iX,fDX) && YLookup(fY,iY,fDY) && ZLookup(fZ,iZ,fDZ) )
        return m_aaakPoly[iZ][iY][iX](iXOrder,iYOrder,iZOrder,fDX,fDY,fDZ);
    else
        return Math::MAX_REAL;
}
//----------------------------------------------------------------------------
void Interp3DAkimaUniform::Allocate (int iXSize, int iYSize, int iZSize,
    Real***& raaafArray)
{
    raaafArray = new Real**[iZSize];
    for (int iZ = 0; iZ < iZSize; iZ++)
    {
        raaafArray[iZ] = new Real*[iYSize];
        for (int iY = 0; iY < iYSize; iY++)
            raaafArray[iZ][iY] = new Real[iXSize];
    }
}
//----------------------------------------------------------------------------
void Interp3DAkimaUniform::Deallocate (int iYSize, int iZSize,
    Real*** aaafArray)
{
    for (int iZ = 0; iZ < iZSize; iZ++)
    {
        for (int iY = 0; iY < iYSize; iY++)
            delete[] aaafArray[iZ][iY];
        delete[] aaafArray[iZ];
    }
    delete[] aaafArray;
}
//----------------------------------------------------------------------------
void Interp3DAkimaUniform::Allocate (int iXSize, int iYSize, int iZSize,
    Polynomial***& raaakArray)
{
    raaakArray = new Polynomial**[iZSize];
    for (int iZ = 0; iZ < iZSize; iZ++)
    {
        raaakArray[iZ] = new Polynomial*[iYSize];
        for (int iY = 0; iY < iYSize; iY++)
            raaakArray[iZ][iY] = new Polynomial[iXSize];
    }
}
//----------------------------------------------------------------------------
void Interp3DAkimaUniform::Deallocate (int iYSize, int iZSize,
    Polynomial*** aaakArray)
{
    for (int iZ = 0; iZ < iZSize; iZ++)
    {
        for (int iY = 0; iY < iYSize; iY++)
            delete[] aaakArray[iZ][iY];
        delete[] aaakArray[iZ];
    }
    delete[] aaakArray;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// polynomial members
//----------------------------------------------------------------------------
Interp3DAkimaUniform::Polynomial::Polynomial ()
{
    memset(m_aaafA[0][0],0,64*sizeof(Real));
}
//----------------------------------------------------------------------------
Real Interp3DAkimaUniform::Polynomial::operator() (Real fX, Real fY,
    Real fZ) const
{
    // TO DO.  optimize this function
    Real afXPow[4] = { 1.0f, fX, fX*fX, fX*fX*fX };
    Real afYPow[4] = { 1.0f, fY, fY*fY, fY*fY*fY };
    Real afZPow[4] = { 1.0f, fZ, fZ*fZ, fZ*fZ*fZ };

    Real fP = 0.0f;
    for (int iZ = 0; iZ <= 3; iZ++)
    {
        for (int iY = 0; iY <= 3; iY++)
        {
            for (int iX = 0; iX <= 3; iX++)
                fP += m_aaafA[iX][iY][iZ]*afXPow[iX]*afYPow[iY]*afZPow[iZ];
        }
    }

    return fP;
}
//----------------------------------------------------------------------------
Real Interp3DAkimaUniform::Polynomial::operator() (int iXOrder, int iYOrder,
    int iZOrder, Real fX, Real fY, Real fZ) const
{
    // TO DO.  optimize this function
    Real afXPow[4], afYPow[4], afZPow[4];

    switch ( iXOrder )
    {
    case 0:
        afXPow[0] = 1.0f;
        afXPow[1] = fX;
        afXPow[2] = fX*fX;
        afXPow[3] = fX*fX*fX;
        break;
    case 1:
        afXPow[0] = 0.0f;
        afXPow[1] = 1.0f;
        afXPow[2] = 2.0f*fX;
        afXPow[3] = 3.0f*fX*fX;
        break;
    case 2:
        afXPow[0] = 0.0f;
        afXPow[1] = 0.0f;
        afXPow[2] = 2.0f;
        afXPow[3] = 6.0f*fX;
        break;
    case 3:
        afXPow[0] = 0.0f;
        afXPow[1] = 0.0f;
        afXPow[2] = 0.0f;
        afXPow[3] = 6.0f;
        break;
    default:
        return 0.0f;
    }

    switch ( iYOrder )
    {
    case 0:
        afYPow[0] = 1.0f;
        afYPow[1] = fY;
        afYPow[2] = fY*fY;
        afYPow[3] = fY*fY*fY;
        break;
    case 1:
        afYPow[0] = 0.0f;
        afYPow[1] = 1.0f;
        afYPow[2] = 2.0f*fY;
        afYPow[3] = 3.0f*fY*fY;
        break;
    case 2:
        afYPow[0] = 0.0f;
        afYPow[1] = 0.0f;
        afYPow[2] = 2.0f;
        afYPow[3] = 6.0f*fY;
        break;
    case 3:
        afYPow[0] = 0.0f;
        afYPow[1] = 0.0f;
        afYPow[2] = 0.0f;
        afYPow[3] = 6.0f;
        break;
    default:
        return 0.0f;
    }

    switch ( iZOrder )
    {
    case 0:
        afZPow[0] = 1.0f;
        afZPow[1] = fZ;
        afZPow[2] = fZ*fZ;
        afZPow[3] = fZ*fZ*fZ;
        break;
    case 1:
        afZPow[0] = 0.0f;
        afZPow[1] = 1.0f;
        afZPow[2] = 2.0f*fZ;
        afZPow[3] = 3.0f*fZ*fZ;
        break;
    case 2:
        afZPow[0] = 0.0f;
        afZPow[1] = 0.0f;
        afZPow[2] = 2.0f;
        afZPow[3] = 6.0f*fZ;
        break;
    case 3:
        afZPow[0] = 0.0f;
        afZPow[1] = 0.0f;
        afZPow[2] = 0.0f;
        afZPow[3] = 6.0f;
        break;
    default:
        return 0.0f;
    }

    Real fP = 0.0f;

    for (int iZ = 0; iZ <= 3; iZ++)
    {
        for (int iY = 0; iY <= 3; iY++)
        {
            for (int iX = 0; iX <= 3; iX++)
                fP += m_aaafA[iX][iY][iZ]*afXPow[iX]*afYPow[iY]*afZPow[iZ];
        }
    }

    return fP;
}
//----------------------------------------------------------------------------


