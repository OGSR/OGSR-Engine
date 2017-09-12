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

#include <stdlib.h>
#include "ImageTessellator.h"

//---------------------------------------------------------------------------
ImageTessellator::ImageTessellator (int iSize, const int* aiImage)
{
    // NOTE:  iSize must be 2^N+1 for N >= 1 is absolutely required!
    m_iSize = iSize;
    m_iSizeM1 = m_iSize-1;
    m_iQuantity = m_iSize*m_iSize;
    m_aiImage = aiImage;
    m_iTolerance = 0;

    // connectivity array for triangles
    m_iMaxNumTriangles = 2*m_iSizeM1*m_iSizeM1;
    m_aiIndex = new int[3*m_iMaxNumTriangles];

    // vertex information array
    m_akVertex = new Vertex[m_iQuantity];

    // allocate quadtree
    m_iNumBlocks = m_iSize*(m_iSize-2)/3;
    m_akBlock = new Block[m_iNumBlocks];

    // initialize quadtree
    m_akBlock[0].Initialize(this,0,0,0,(m_iSize-1)/2,true);
}
//---------------------------------------------------------------------------
ImageTessellator::~ImageTessellator ()
{
    delete[] m_aiIndex;
    delete[] m_akVertex;
    delete[] m_akBlock;
}
//---------------------------------------------------------------------------
void ImageTessellator::Tessellate (int iTolerance)
{
    m_iTolerance = iTolerance;

    // reset vertices
    int i;
    for (i = 0; i < m_iQuantity; i++)
        m_akVertex[i].m_bEnabled = false;

    // simplify blocks
    m_akBlock[0].Simplify(this,0);

    m_iNumTriangles = 0;

    // upper left triangle
    int iT0 = 0;
    int iL0 = m_iSize*m_iSizeM1;
    int iR0 = m_iSizeM1;
    SelectTriangles(iT0,iL0,iR0);

    // lower right triangle
    int iT1 = m_iSizeM1*(m_iSize+1);
    int iL1 = m_iSizeM1;
    int iR1 = m_iSize*m_iSizeM1;
    SelectTriangles(iT1,iL1,iR1);
}
//---------------------------------------------------------------------------
void ImageTessellator::SelectTriangles (int iT, int iL, int iR)
{
    // T is the "top" vertex (at the right-angle of the triangle).  L is
    // the "left" vertex and R is the "right" vertex where the vertices
    // <T,L,R> are in counterclockwise order.

    // Determine if triangle is interior or leaf.  Triangles with non-axis
    // aligned edges (i.e. diagonal edges) sharing vertex T can always be
    // subdivided, so we need only analyze those triangles whose edges
    // sharing T are axis-aligned.

    bool bInterior;
    if ( iR > iT )
    {
        if ( iL > iT )
            bInterior = (iR - iT > 1);
        else
            bInterior = (iT - iL > 1);
    }
    else
    {
        if ( iL > iT )
            bInterior = (iL - iT > 1);
        else
            bInterior = (iT - iR > 1);
    }

    if ( bInterior )
    {
        // M is the midpoint of edge <L,R>.
        int iM = (iL + iR)/2;
        if ( m_akVertex[iM].m_bEnabled )
        {
            SelectTriangles(iM,iT,iL);
            SelectTriangles(iM,iR,iT);
            return;
        }
    }

    // add triangle to connectivity array
    int i = 3*m_iNumTriangles;
    m_aiIndex[i++] = iT;
    m_aiIndex[i++] = iL;
    m_aiIndex[i  ] = iR;
    m_iNumTriangles++;
}
//---------------------------------------------------------------------------
void ImageTessellator::Block::Initialize (ImageTessellator* pTess,
    int iBlock, int iX, int iY, int iStride, bool bEven)
{
    m_iX = iX;
    m_iY = iY;
    m_iStride = iStride;

    // get image intensities for 3x3 block
    int iOrigin = m_iX + pTess->m_iSize*m_iY;
    int iZ[9];
    iZ[0] = pTess->m_aiImage[iOrigin];
    iZ[1] = pTess->m_aiImage[iOrigin + m_iStride];
    iZ[2] = pTess->m_aiImage[iOrigin + 2*m_iStride];
    iOrigin += pTess->m_iSize*m_iStride;
    iZ[3] = pTess->m_aiImage[iOrigin];
    iZ[4] = pTess->m_aiImage[iOrigin + m_iStride];
    iZ[5] = pTess->m_aiImage[iOrigin + 2*m_iStride];
    iOrigin += pTess->m_iSize*m_iStride;
    iZ[6] = pTess->m_aiImage[iOrigin];
    iZ[7] = pTess->m_aiImage[iOrigin + m_iStride];
    iZ[8] = pTess->m_aiImage[iOrigin + 2*m_iStride];

    // compute delta values
    m_iDelta[0] = abs((iZ[0] - 2*iZ[1] + iZ[2])/2);
    m_iDelta[1] = abs((iZ[8] - 2*iZ[5] + iZ[2])/2);
    m_iDelta[2] = abs((iZ[8] - 2*iZ[7] + iZ[6])/2);
    m_iDelta[3] = abs((iZ[0] - 2*iZ[3] + iZ[6])/2);
    if ( bEven )
        m_iDelta[4] = abs((iZ[2] - 2*iZ[4] + iZ[6])/2);
    else
        m_iDelta[4] = abs((iZ[0] - 2*iZ[4] + iZ[8])/2);

    // set vertex dependencies
    int iTwoStride = 2*m_iStride;
    int iSizePlus1 = pTess->m_iSize+1;
    int iSizePlus2 = pTess->m_iSize+2;
    int iTwoSizePlus1 = 2*pTess->m_iSize+1;
    int iSizeTimesStride = pTess->m_iSize*m_iStride;
    int iSizePlus1TimesStride = iSizePlus1*m_iStride;
    int iSizePlus2TimesStride = iSizePlus2*m_iStride;
    int iSizeTimesTwoStride = pTess->m_iSize*iTwoStride;
    int iTwoSizePlus1TimesStride = iTwoSizePlus1*m_iStride;
    int iSizePlus1TimesTwoStride = iSizePlus1*iTwoStride;
    Vertex* pkVOrigin = pTess->m_akVertex + m_iX + pTess->m_iSize*m_iY;
    Vertex* apkVertex[9];
    apkVertex[0] = &pkVOrigin[0];
    apkVertex[1] = &pkVOrigin[m_iStride];
    apkVertex[2] = &pkVOrigin[iTwoStride];
    apkVertex[3] = &pkVOrigin[iSizeTimesStride];
    apkVertex[4] = &pkVOrigin[iSizePlus1TimesStride];
    apkVertex[5] = &pkVOrigin[iSizePlus2TimesStride];
    apkVertex[6] = &pkVOrigin[iSizeTimesTwoStride];
    apkVertex[7] = &pkVOrigin[iTwoSizePlus1TimesStride];
    apkVertex[8] = &pkVOrigin[iSizePlus1TimesTwoStride];

    apkVertex[1]->m_apkDependent[0] = apkVertex[4];
    apkVertex[3]->m_apkDependent[1] = apkVertex[4];
    apkVertex[5]->m_apkDependent[0] = apkVertex[4];
    apkVertex[7]->m_apkDependent[1] = apkVertex[4];
    if ( bEven )
    {
        apkVertex[4]->m_apkDependent[0] = apkVertex[6];
        apkVertex[4]->m_apkDependent[1] = apkVertex[2];
    }
    else
    {
        apkVertex[4]->m_apkDependent[0] = apkVertex[0];
        apkVertex[4]->m_apkDependent[1] = apkVertex[8];
    }

    // recurse on children (if any)
    if ( iStride > 1 )
    {
        // child stride is half the parent stride
        int iChildStride = iStride/2;

        // process child 00
        int iChild = 4*iBlock+1;
        Block* pkChild = &pTess->m_akBlock[iChild];
        pkChild->Initialize(pTess,iChild,iX,iY,iChildStride,true);
        m_iMin = pkChild->m_iMin;
        m_iMax = pkChild->m_iMax;

        // process child 01
        iChild++;
        pkChild++;
        pkChild->Initialize(pTess,iChild,iX+iStride,iY,iChildStride,
            false);
        if ( pkChild->m_iMin < m_iMin )
            m_iMin = pkChild->m_iMin;
        if ( pkChild->m_iMax > m_iMax )
            m_iMax = pkChild->m_iMax;

        // process child 10
        iChild++;
        pkChild++;
        pkChild->Initialize(pTess,iChild,iX,iY+iStride,iChildStride,
            false);
        if ( pkChild->m_iMin < m_iMin )
            m_iMin = pkChild->m_iMin;
        if ( pkChild->m_iMax > m_iMax )
            m_iMax = pkChild->m_iMax;

        // process child 11
        iChild++;
        pkChild++;
        pkChild->Initialize(pTess,iChild,iX+iStride,iY+iStride,
            iChildStride,true);
        if ( pkChild->m_iMin < m_iMin )
            m_iMin = pkChild->m_iMin;
        if ( pkChild->m_iMax > m_iMax )
            m_iMax = pkChild->m_iMax;
    }
    else
    {
        // compute z components of bounding box at leaf node of quadtree
        m_iMin = iZ[0];
        m_iMax = m_iMin;
        for (int i = 1; i < 9; i++)
        {
            int iTmp = iZ[i];
            if ( iTmp < m_iMin )
                m_iMin = iTmp;
            if ( iTmp > m_iMax )
                m_iMax = iTmp;
        }
    }
}
//---------------------------------------------------------------------------
void ImageTessellator::Block::Simplify (ImageTessellator* pTess,
    int iBlock)
{
    int iOrigin, iIndex;

    if ( m_iMax - m_iMin > pTess->m_iTolerance )
    {
        if ( m_iStride > 1 )
        {
            int iChild = 4*iBlock+1;
            pTess->m_akBlock[iChild].Simplify(pTess,iChild);
            iChild++;
            pTess->m_akBlock[iChild].Simplify(pTess,iChild);
            iChild++;
            pTess->m_akBlock[iChild].Simplify(pTess,iChild);
            iChild++;
            pTess->m_akBlock[iChild].Simplify(pTess,iChild);
            return;
        }

        // leaf block, test individual triangles
        iOrigin = m_iX + pTess->m_iSize*m_iY;

        // simplify at (stride,0)
        if ( m_iDelta[0] > pTess->m_iTolerance )
        {
            iIndex = iOrigin + m_iStride;
            if ( !pTess->m_akVertex[iIndex].m_bEnabled )
                pTess->m_akVertex[iIndex].Enable();
        }

        // simplify at (2*stride,stride)
        if ( m_iDelta[1] > pTess->m_iTolerance )
        {
            iIndex = iOrigin + (pTess->m_iSize+2)*m_iStride;
            if ( !pTess->m_akVertex[iIndex].m_bEnabled )
                pTess->m_akVertex[iIndex].Enable();
        }

        // simplify at (stride,2*stride)
        if ( m_iDelta[2] > pTess->m_iTolerance )
        {
            iIndex = iOrigin + (2*pTess->m_iSize+1)*m_iStride;
            if ( !pTess->m_akVertex[iIndex].m_bEnabled )
                pTess->m_akVertex[iIndex].Enable();
        }

        // simplify at (0,stride)
        if ( m_iDelta[3] > pTess->m_iTolerance )
        {
            iIndex = iOrigin + pTess->m_iSize*m_iStride;
            if ( !pTess->m_akVertex[iIndex].m_bEnabled )
                pTess->m_akVertex[iIndex].Enable();
        }

        // simplify at (stride,stride)
        if ( m_iDelta[4] > pTess->m_iTolerance )
        {
            iIndex = iOrigin + (pTess->m_iSize+1)*m_iStride;
            if ( !pTess->m_akVertex[iIndex].m_bEnabled )
                pTess->m_akVertex[iIndex].Enable();
        }
    }

    // enable four corner vertices
    iOrigin = m_iX + pTess->m_iSize*m_iY;
    if ( !pTess->m_akVertex[iOrigin].m_bEnabled )
        pTess->m_akVertex[iOrigin].Enable();
    
    iIndex = iOrigin + 2*m_iStride;
    if ( !pTess->m_akVertex[iIndex].m_bEnabled )
        pTess->m_akVertex[iIndex].Enable();
    
    iIndex = iOrigin + 2*pTess->m_iSize*m_iStride;
    if ( !pTess->m_akVertex[iIndex].m_bEnabled )
        pTess->m_akVertex[iIndex].Enable();
    
    iIndex = iOrigin + 2*(pTess->m_iSize + 1)*m_iStride;
    if ( !pTess->m_akVertex[iIndex].m_bEnabled )
        pTess->m_akVertex[iIndex].Enable();
}
//---------------------------------------------------------------------------
ImageTessellator::Vertex::Vertex ()
{
    m_bEnabled = false;
    m_apkDependent[0] = 0;
    m_apkDependent[1] = 0;
}
//---------------------------------------------------------------------------
void ImageTessellator::Vertex::Enable ()
{
    m_bEnabled = true;

    if ( m_apkDependent[0] )
    {
        if ( !m_apkDependent[0]->m_bEnabled )
            m_apkDependent[0]->Enable();
    }
    
    if ( m_apkDependent[1] )
    {
        if ( !m_apkDependent[1]->m_bEnabled )
            m_apkDependent[1]->Enable();
    }
}
//---------------------------------------------------------------------------
void ImageTessellator::Vertex::Disable ()
{
    m_bEnabled = false;
    
    if ( m_apkDependent[0] )
    {
        if ( m_apkDependent[0]->m_bEnabled )
            m_apkDependent[0]->Disable();
    }

    if ( m_apkDependent[1] )
    {
        if ( m_apkDependent[1]->m_bEnabled )
            m_apkDependent[1]->Disable();
    }
}
//---------------------------------------------------------------------------


