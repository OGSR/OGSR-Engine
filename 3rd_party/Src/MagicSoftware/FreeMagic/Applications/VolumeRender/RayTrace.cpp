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

#include <cmath>
#include "RayTrace.h"

//----------------------------------------------------------------------------
RayTrace::RayTrace (ImageUChar3D* pkImage, float fGamma)
{
    // gamma correction default value
    m_fGamma = fGamma;

    // get maximum dimension size
    int iBMax = pkImage->GetBound(0);
    if ( pkImage->GetBound(1) > iBMax )
        iBMax = pkImage->GetBound(1);
    if ( pkImage->GetBound(2) > iBMax )
        iBMax = pkImage->GetBound(2);

    // Construct density in range [0,1/bmax].  The accumulator values cannot
    // exceed 1.
    m_pkDensity = new ImageFloat3D(pkImage->GetBound(0),pkImage->GetBound(1),
        pkImage->GetBound(2));
    unsigned char ucMin = (*pkImage)[0], ucMax = ucMin;
    int i;
    for (i = 1; i < pkImage->GetQuantity(); i++)
    {
        if ( (*pkImage)[i] < ucMin )
            ucMin = (*pkImage)[i];
        else if ( (*pkImage)[i] > ucMax )
            ucMax = (*pkImage)[i];
    }
    float fInvRange = 1.0f/(((float)(ucMax)-(float)(ucMin))*iBMax);
    for (i = 1; i < pkImage->GetQuantity(); i++)
        (*m_pkDensity)[i] = ((*pkImage)[i]-(float)(ucMin))*fInvRange;

    // center point of 3D image
    m_fXCenter = float((m_pkDensity->GetBound(0)-1)/2);
    m_fYCenter = float((m_pkDensity->GetBound(1)-1)/2);
    m_fZCenter = float((m_pkDensity->GetBound(2)-1)/2);

    // determine image bounds and allocate images
    m_iBound = int(ceilf(2.0f*iBMax));
    if ( m_iBound % 2 )
        m_iBound++;
    m_iBoundM1 = m_iBound-1;
    m_iHBound = m_iBound/2;
    m_pkAccum = new ImageFloat2D(m_iBound,m_iBound);
    m_pkRender = new ImageUShort2D(m_iBound,m_iBound);

    // initialize eyepoint frame field
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            m_aafFrame[iRow][iCol] = ( iRow == iCol ? 1.0f : 0.0f );
    }
}
//----------------------------------------------------------------------------
RayTrace::~RayTrace ()
{
    delete m_pkDensity;
    delete m_pkAccum;
    delete m_pkRender;
}
//----------------------------------------------------------------------------
bool RayTrace::Clipped (float fP, float fQ, float& rfU0, float& rfU1)
{
    float fR;

    if ( fP < 0.0f )
    {
        fR = fQ/fP;
        if ( fR > rfU1 )
            return false;
        if ( fR > rfU0 )
            rfU0 = fR;
        return true;
    }

    if ( fP > 0.0f )
    {
        fR = fQ/fP;
        if ( fR < rfU0 )
            return false;
        if ( fR < rfU1 )
            rfU1 = fR;
        return true;
    }

    // fP == 0
    return fQ >= 0.0f;
}
//----------------------------------------------------------------------------
bool RayTrace::Clip3D (float& rfX0, float& rfY0, float& rfZ0, float& rfX1,
    float& rfY1, float& rfZ1)
{
    // clipping region
    const float fXMin = -m_fXCenter, fXMax = m_fXCenter;
    const float fYMin = -m_fYCenter, fYMax = m_fYCenter;
    const float fZMin = -m_fZCenter, fZMax = m_fZCenter;

    float fU0 = 0.0f, fU1 = 1.0f;

    float fDx = rfX1 - rfX0;
    if ( !Clipped(-fDx,rfX0-fXMin,fU0,fU1)
    ||   !Clipped(+fDx,fXMax-rfX0,fU0,fU1) )
    {
        return false;
    }

    float fDy = rfY1 - rfY0;
    if ( !Clipped(-fDy,rfY0-fYMin,fU0,fU1)
    ||   !Clipped(+fDy,fYMax-rfY0,fU0,fU1) )
    {
        return false;
    }

    float fDz = rfZ1 - rfZ0;
    if ( !Clipped(-fDz,rfZ0-fZMin,fU0,fU1)
    ||   !Clipped(+fDz,fZMax-rfZ0,fU0,fU1) )
    {
        return false;
    }

    if ( fU1 < 1.0f )
    {
        rfX1 = rfX0 + fU1*fDx;
        rfY1 = rfY0 + fU1*fDy;
        rfZ1 = rfZ0 + fU1*fDz;
    }
    if ( fU0 > 0.0f )
    {
        rfX0 = rfX0 + fU0*fDx;
        rfY0 = rfY0 + fU0*fDy;
        rfZ0 = rfZ0 + fU0*fDz;
    }

    return true;
}
//----------------------------------------------------------------------------
void RayTrace::Line3D (int iJ0, int iJ1, int iX0, int iY0, int iZ0, int iX1,
    int iY1, int iZ1)
{
    // starting point of line
    int iX = iX0, iY = iY0, iZ = iZ0;

    // direction of line
    int iDx = iX1 - iX0, iDy = iY1 - iY0, iDz = iZ1 - iZ0;

    // increment or decrement depending on direction of line
    int iSx = (iDx > 0 ? 1 : (iDx < 0 ? -1 : 0));
    int iSy = (iDy > 0 ? 1 : (iDy < 0 ? -1 : 0));
    int iSz = (iDz > 0 ? 1 : (iDz < 0 ? -1 : 0));

    // decision parameters for voxel selection
    iDx = abs(iDx); iDy = abs(iDy); iDz = abs(iDz);
    int iAx = 2*iDx, iAy = 2*iDy, iAz = 2*iDz;
    int iDecX, iDecY, iDecZ;

    // determine largest direction component, single-step related variable
    int iMax = iDx, iVar = 0;
    if ( iDy > iMax ) { iMax = iDy; iVar = 1; }
    if ( iDz > iMax ) { iMax = iDz; iVar = 2; }

    // traverse Bresenham line and accumulate density values
    int iIndex = iJ0 + m_pkAccum->GetBound(0)*iJ1;
    switch ( iVar )
    {
    case 0:  // single-step in x-direction
        iDecY = iAy - iDx;
        iDecZ = iAz - iDx;
        for (/**/; /**/; iX += iSx, iDecY += iAy, iDecZ += iAz)
        {
            // accumulate the density value
            (*m_pkAccum)[iIndex] = (*m_pkAccum)[iIndex] +
                (*m_pkDensity)(iX,iY,iZ);

            // take Bresenham step
            if ( iX == iX1 ) break;
            if ( iDecY >= 0 ) { iDecY -= iAx; iY += iSy; }
            if ( iDecZ >= 0 ) { iDecZ -= iAx; iZ += iSz; }
        }
        break;
    case 1:  // single-step in y-direction
        iDecX = iAx - iDy;
        iDecZ = iAz - iDy;
        for (/**/; /**/; iY += iSy, iDecX += iAx, iDecZ += iAz)
        {
            // accumulate the density value
            (*m_pkAccum)[iIndex] = (*m_pkAccum)[iIndex] +
                (*m_pkDensity)(iX,iY,iZ);

            // take Bresenham step
            if ( iY == iY1 ) break;
            if ( iDecX >= 0 ) { iDecX -= iAy; iX += iSx; }
            if ( iDecZ >= 0 ) { iDecZ -= iAy; iZ += iSz; }
        }
        break;
    case 2:  // single-step in z-direction
        iDecX = iAx - iDz;
        iDecY = iAy - iDz;
        for (/**/; /**/; iZ += iSz, iDecX += iAx, iDecY += iAy)
        {
            // accumulate the density value
            (*m_pkAccum)[iIndex] = (*m_pkAccum)[iIndex] +
                (*m_pkDensity)(iX,iY,iZ);

            // take Bresenham step
            if ( iZ == iZ1 ) break;
            if ( iDecX >= 0 ) { iDecX -= iAz; iX += iSx; }
            if ( iDecY >= 0 ) { iDecY -= iAz; iY += iSy; }
        }
        break;
    }

    unsigned int uiGray = (unsigned int)(31.0f*powf((*m_pkAccum)[iIndex],
        m_fGamma));

    (*m_pkRender)[iIndex] = uiGray + (uiGray << 5) + (uiGray << 10);
}
//----------------------------------------------------------------------------
void RayTrace::Trace (int iSpacing)
{
    int iI0, iI1, iJ0, iJ1;

    *m_pkAccum = 0.0f;
    *m_pkRender = (unsigned short)0;

    float fLw0 = m_iHBound*m_aafFrame[0][2];
    float fLw1 = m_iHBound*m_aafFrame[1][2];
    float fLw2 = m_iHBound*m_aafFrame[2][2];
    for (iI1 = -m_iHBound; iI1 < m_iHBound; iI1 += iSpacing)
    {
        iJ1 = iI1 + m_iHBound;
        for (iI0 = -m_iHBound; iI0 < m_iHBound; iI0 += iSpacing)
        {
            iJ0 = iI0 + m_iHBound;

            float fA0 = iI0*m_aafFrame[0][0] + iI1*m_aafFrame[0][1];
            float fA1 = iI0*m_aafFrame[1][0] + iI1*m_aafFrame[1][1];
            float fA2 = iI0*m_aafFrame[2][0] + iI1*m_aafFrame[2][1];

            float fX0 = fA0-fLw0, fY0 = fA1-fLw1, fZ0 = fA2-fLw2;
            float fX1 = fA0+fLw0, fY1 = fA1+fLw1, fZ1 = fA2+fLw2;

            if ( Clip3D(fX0,fY0,fZ0,fX1,fY1,fZ1) )
            {
                int iX0 = (int)(fX0+m_fXCenter);
                int iY0 = (int)(fY0+m_fYCenter);
                int iZ0 = (int)(fZ0+m_fZCenter);
                int iX1 = (int)(fX1+m_fXCenter);
                int iY1 = (int)(fY1+m_fYCenter);
                int iZ1 = (int)(fZ1+m_fZCenter);
                Line3D(iJ0,iJ1,iX0,iY0,iZ0,iX1,iY1,iZ1);
                if ( iSpacing > 1 )
                {
                    unsigned int uiValue = (*m_pkRender)(iJ0,iJ1);
                    for (int iY = 0; iY < iSpacing; iY++)
                    {
                        for (int iX = 0; iX < iSpacing; iX++)
                            (*m_pkRender)(iJ0+iX,iJ1+iY) = uiValue;
                    }
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void RayTrace::DrawWireFrame ()
{
    // calculate orientation of cube corners relative to u and v
    float fUDot0 =  m_aafFrame[0][0] + m_aafFrame[1][0] + m_aafFrame[2][0];
    float fUDot1 =  m_aafFrame[0][0] + m_aafFrame[1][0] - m_aafFrame[2][0];
    float fUDot2 = -m_aafFrame[0][0] + m_aafFrame[1][0] - m_aafFrame[2][0];
    float fUDot3 = -m_aafFrame[0][0] + m_aafFrame[1][0] + m_aafFrame[2][0];
    float fVDot0 =  m_aafFrame[0][1] + m_aafFrame[1][1] + m_aafFrame[2][1];
    float fVDot1 =  m_aafFrame[0][1] + m_aafFrame[1][1] - m_aafFrame[2][1];
    float fVDot2 = -m_aafFrame[0][1] + m_aafFrame[1][1] - m_aafFrame[2][1];
    float fVDot3 = -m_aafFrame[0][1] + m_aafFrame[1][1] + m_aafFrame[2][1];

    // corner locations
    int aiX[8], aiY[8];

    // (-1,-1,-1)
    aiX[0] = (int)(m_iBoundM1*(1.0f-0.5f*fUDot0)*0.5f);
    aiY[0] = (int)(m_iBoundM1*(1.0f-0.5f*fVDot0)*0.5f);

    // (1,-1,-1)
    aiX[1] = (int)(m_iBoundM1*(1.0f-0.5f*fUDot3)*0.5f);
    aiY[1] = (int)(m_iBoundM1*(1.0f-0.5f*fVDot3)*0.5f);

    // (1,1,-1)
    aiX[2] = (int)(m_iBoundM1*(1.0f+0.5f*fUDot1)*0.5f);
    aiY[2] = (int)(m_iBoundM1*(1.0f+0.5f*fVDot1)*0.5f);

    // (-1,1,-1)
    aiX[3] = (int)(m_iBoundM1*(1.0f+0.5f*fUDot2)*0.5f);
    aiY[3] = (int)(m_iBoundM1*(1.0f+0.5f*fVDot2)*0.5f);

    // (-1,-1,1)
    aiX[4] = (int)(m_iBoundM1*(1.0f-0.5f*fUDot1)*0.5f);
    aiY[4] = (int)(m_iBoundM1*(1.0f-0.5f*fVDot1)*0.5f);

    // (1,-1,1)
    aiX[5] = (int)(m_iBoundM1*(1.0f-0.5f*fUDot2)*0.5f);
    aiY[5] = (int)(m_iBoundM1*(1.0f-0.5f*fVDot2)*0.5f);

    // (1,1,1)
    aiX[6] = (int)(m_iBoundM1*(1.0f+0.5f*fUDot0)*0.5f);
    aiY[6] = (int)(m_iBoundM1*(1.0f+0.5f*fVDot0)*0.5f);

    // (-1,1,1)
    aiX[7] = (int)(m_iBoundM1*(1.0f+0.5f*fUDot3)*0.5f);
    aiY[7] = (int)(m_iBoundM1*(1.0f+0.5f*fVDot3)*0.5f);

    // draw back faces
    if ( m_aafFrame[0][2] < 0.0f )
    {
        // face x = -1
        Line2D(false,aiX[0],aiY[0],aiX[3],aiY[3]);
        Line2D(false,aiX[3],aiY[3],aiX[7],aiY[7]);
        Line2D(false,aiX[7],aiY[7],aiX[4],aiY[4]);
        Line2D(false,aiX[4],aiY[4],aiX[0],aiY[0]);
    }
    else if ( m_aafFrame[0][2] > 0.0f )
    {
        // face x = +1
        Line2D(false,aiX[1],aiY[1],aiX[2],aiY[2]);
        Line2D(false,aiX[2],aiY[2],aiX[6],aiY[6]);
        Line2D(false,aiX[6],aiY[6],aiX[5],aiY[5]);
        Line2D(false,aiX[5],aiY[5],aiX[1],aiY[1]);
    }

    if ( m_aafFrame[1][2] < 0.0f )
    {
        // face y = -1
        Line2D(false,aiX[0],aiY[0],aiX[1],aiY[1]);
        Line2D(false,aiX[1],aiY[1],aiX[5],aiY[5]);
        Line2D(false,aiX[5],aiY[5],aiX[4],aiY[4]);
        Line2D(false,aiX[4],aiY[4],aiX[0],aiY[0]);
    }
    else if ( m_aafFrame[1][2] > 0.0f )
    {
        // face y = +1
        Line2D(false,aiX[2],aiY[2],aiX[3],aiY[3]);
        Line2D(false,aiX[3],aiY[3],aiX[7],aiY[7]);
        Line2D(false,aiX[7],aiY[7],aiX[6],aiY[6]);
        Line2D(false,aiX[6],aiY[6],aiX[2],aiY[2]);
    }

    if ( m_aafFrame[2][2] < 0.0f )
    {
        // face z = -1
        Line2D(false,aiX[0],aiY[0],aiX[1],aiY[1]);
        Line2D(false,aiX[1],aiY[1],aiX[2],aiY[2]);
        Line2D(false,aiX[2],aiY[2],aiX[3],aiY[3]);
        Line2D(false,aiX[3],aiY[3],aiX[0],aiY[0]);
    }
    else if ( m_aafFrame[2][2] > 0.0f )
    {
        // face z = +1
        Line2D(false,aiX[4],aiY[4],aiX[5],aiY[5]);
        Line2D(false,aiX[5],aiY[5],aiX[6],aiY[6]);
        Line2D(false,aiX[6],aiY[6],aiX[7],aiY[7]);
        Line2D(false,aiX[7],aiY[7],aiX[4],aiY[4]);
    }

    // draw front faces
    if ( m_aafFrame[0][2] < 0.0f )
    {
        // face x = +1
        Line2D(true,aiX[1],aiY[1],aiX[2],aiY[2]);
        Line2D(true,aiX[2],aiY[2],aiX[6],aiY[6]);
        Line2D(true,aiX[6],aiY[6],aiX[5],aiY[5]);
        Line2D(true,aiX[5],aiY[5],aiX[1],aiY[1]);
    }
    else if ( m_aafFrame[0][2] > 0.0f )
    {
        // face x = -1
        Line2D(true,aiX[0],aiY[0],aiX[3],aiY[3]);
        Line2D(true,aiX[3],aiY[3],aiX[7],aiY[7]);
        Line2D(true,aiX[7],aiY[7],aiX[4],aiY[4]);
        Line2D(true,aiX[4],aiY[4],aiX[0],aiY[0]);
    }

    if ( m_aafFrame[1][2] < 0.0f )
    {
        // face y = +1
        Line2D(true,aiX[2],aiY[2],aiX[3],aiY[3]);
        Line2D(true,aiX[3],aiY[3],aiX[7],aiY[7]);
        Line2D(true,aiX[7],aiY[7],aiX[6],aiY[6]);
        Line2D(true,aiX[6],aiY[6],aiX[2],aiY[2]);
    }
    else if ( m_aafFrame[1][2] > 0.0f )
    {
        // face y = -1
        Line2D(true,aiX[0],aiY[0],aiX[1],aiY[1]);
        Line2D(true,aiX[1],aiY[1],aiX[5],aiY[5]);
        Line2D(true,aiX[5],aiY[5],aiX[4],aiY[4]);
        Line2D(true,aiX[4],aiY[4],aiX[0],aiY[0]);
    }

    if ( m_aafFrame[2][2] < 0.0f )
    {
        // face z = +1
        Line2D(true,aiX[4],aiY[4],aiX[5],aiY[5]);
        Line2D(true,aiX[5],aiY[5],aiX[6],aiY[6]);
        Line2D(true,aiX[6],aiY[6],aiX[7],aiY[7]);
        Line2D(true,aiX[7],aiY[7],aiX[4],aiY[4]);
    }
    else if ( m_aafFrame[2][2] > 0.0f )
    {
        // face z = -1
        Line2D(true,aiX[0],aiY[0],aiX[1],aiY[1]);
        Line2D(true,aiX[1],aiY[1],aiX[2],aiY[2]);
        Line2D(true,aiX[2],aiY[2],aiX[3],aiY[3]);
        Line2D(true,aiX[3],aiY[3],aiX[0],aiY[0]);
    }
}
//----------------------------------------------------------------------------
void RayTrace::Line2D (bool bVisible, int iX0, int iY0, int iX1, int iY1)
{
    // starting point of line
    int iX = iX0, iY = iY0;

    // direction of line
    int iDx = iX1-iX0, iDy = iY1-iY0;

    // increment or decrement depending on direction of line
    int iSx = (iDx > 0 ? 1 : (iDx < 0 ? -1 : 0));
    int iSy = (iDy > 0 ? 1 : (iDy < 0 ? -1 : 0));

    // decision parameters for voxel selection
    iDx = abs(iDx); iDy = abs(iDy);
    int iAx = 2*iDx, iAy = 2*iDy;
    int iDecX, iDecY;

    // determine largest direction component, single-step related variable
    int iMax = iDx, iVar = 0;
    if ( iDy > iMax ) { iMax = iDy; iVar = 1; }

    // traverse Bresenham line and accumulate density values
    switch ( iVar )
    {
    case 0:  // single-step in x-direction
        for (iDecY = iAy - iDx; /**/; iX += iSx, iDecY += iAy)
        {
            // process voxel (x,y)
            (*m_pkRender)(iX,iY) = ( bVisible ? 0x7C00 : 0x2000 );


            // take Bresenham step
            if ( iX == iX1 ) break;
            if ( iDecY >= 0 ) { iDecY -= iAx; iY += iSy; }
        }
        break;
    case 1:  // single-step in iY-direction
        for (iDecX = iAx - iDy; /**/; iY += iSy, iDecX += iAx)
        {
            // process voxel (x,y)
            (*m_pkRender)(iX,iY) = ( bVisible ? 0x7C00 : 0x2000 );

            // take Bresenham step
            if ( iY == iY1 ) break;
            if ( iDecX >= 0 ) { iDecX -= iAy; iX += iSx; }
        }
        break;
    }
}
//----------------------------------------------------------------------------
bool RayTrace::MoveTrackBall (float fX0, float fY0, float fX1, float fY1)
{
    if ( fX0 == fX1 && fY0 == fY1 )     // nothing to rotate
        return false;

    float fLength = float(sqrt(fX0*fX0+fY0*fY0)), fZ0, fZ1;
    if ( fLength > 1.0f )
    {
        // outside unit disk, project onto it
        fX0 /= fLength;
        fY0 /= fLength;
        fZ0 = 0.0f;
    }
    else
    {
        // compute point (x0,y0,z0) on negative unit hemisphere
        fZ0 = 1.0f - fX0*fX0 - fY0*fY0;
        fZ0 = ( fZ0 <= 0.0f ? 0.0f : sqrtf(fZ0) );
    }
    fZ0 *= -1.0f;

    fLength = sqrtf(fX1*fX1+fY1*fY1);
    if ( fLength > 1.0f )
    {
        // outside unit disk, project onto it
        fX1 /= fLength;
        fY1 /= fLength;
        fZ1 = 0.0f;
    }
    else
    {
        // compute point (x1,y1,z1) on negative unit hemisphere
        fZ1 = 1.0f - fX1*fX1 - fY1*fY1;
        fZ1 = ( fZ1 <= 0.0f ? 0.0f : sqrtf(fZ1) );
    }
    fZ1 *= -1.0f;

    // create unit direction vector to rotate about
    float afD[3] = { fY0*fZ1-fY1*fZ0, fX1*fZ0-fX0*fZ1, fX0*fY1-fX1*fY0 };
    fLength = sqrtf(afD[0]*afD[0]+afD[1]*afD[1]+afD[2]*afD[2]);
    if ( fLength == 0.0f )
    {
        // rotated pi radians
        fLength = sqrtf(fX0*fX0+fY0*fY0);
        afD[0] = fY0/fLength;
        afD[1] = -fX0/fLength;
        afD[2] = 0.0f;
    }
    else
    {
        afD[0] /= fLength;
        afD[1] /= fLength;
        afD[2] /= fLength;
    }

    // tensor product of direction vector with itself
    float fD00 = afD[0]*afD[0];
    float fD01 = afD[0]*afD[1];
    float fD02 = afD[0]*afD[2];
    float fD11 = afD[1]*afD[1];
    float fD12 = afD[1]*afD[2];
    float fD22 = afD[2]*afD[2];

    // cosine and sine of angle of rotation about direction vector
    float fCos = fX0*fX1+fY0*fY1+fZ0*fZ1;
    if ( fCos < -1.0f )
        fCos = -1.0f;
    else if ( fCos > 1.0f )
        fCos = 1.0f;
    float fSin = sqrtf(fabsf(1.0f-fCos*fCos)), fOmCos = 1.0f - fCos;

    // rotation matrix implied by track ball motion
    float aafRot[3][3] =
    {
        {
            fOmCos*fD00+fCos,
            fOmCos*fD01-fSin*afD[2],
            fOmCos*fD02+fSin*afD[1]
        },
        {
            fOmCos*fD01+fSin*afD[2],
            fOmCos*fD11+fCos,
            fOmCos*fD12-fSin*afD[0]
        },
        {
            fOmCos*fD02-fSin*afD[1],
            fOmCos*fD12+fSin*afD[0],
            fOmCos*fD22+fCos
        }
    };

    // rotate the eyepoint frame field
    int iRow, iCol;
    float aafRotFrame[3][3] =
    {
        {0.0f,0.0f,0.0f},
        {0.0f,0.0f,0.0f},
        {0.0f,0.0f,0.0f}
    };

    for (iRow = 0; iRow < 3; iRow++)
    {
        for (iCol = 0; iCol < 3; iCol++)
        {
            for (int iMid = 0; iMid < 3; iMid++)
            {
                aafRotFrame[iRow][iCol] +=
                    m_aafFrame[iRow][iMid]*aafRot[iCol][iMid];
            }
        }
    }

    // unitize to avoid degeneration (replace by Gram-Schmidt orth.)
    for (iCol = 0; iCol < 3; iCol++)
    {
        fLength = 0.0f;
        for (iRow = 0; iRow < 3; iRow++)
            fLength += aafRotFrame[iRow][iCol]*aafRotFrame[iRow][iCol];
        fLength = float(sqrt(fLength));
        for (iRow = 0; iRow < 3; iRow++)
            m_aafFrame[iRow][iCol] = aafRotFrame[iRow][iCol]/fLength;
    }

    return true;
}
//----------------------------------------------------------------------------
void RayTrace::Correction (float fGamma)
{
    m_fGamma = fGamma;
    for (int i = 0; i < m_pkAccum->GetQuantity(); i++)
    {
        unsigned int uiGray = (unsigned int)(31.0f*powf((*m_pkAccum)[i],
            m_fGamma));

        (*m_pkRender)[i] = uiGray + (uiGray << 5) + (uiGray << 10);
    }
}
//----------------------------------------------------------------------------


