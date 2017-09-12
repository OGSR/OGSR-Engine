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

#include "ImageInterp2D.h"
using namespace Mgc;
#include <map>
using namespace std;

//----------------------------------------------------------------------------
ImageInterp2D::ImageInterp2D (int iXBound, int iYBound, int* aiData)
    :
    ImageInt2D(iXBound,iYBound,(Eint*)aiData)
{
}
//----------------------------------------------------------------------------
ImageInterp2D::ImageInterp2D (const char* acFilename)
    :
    ImageInt2D(acFilename)
{
}
//----------------------------------------------------------------------------
void ImageInterp2D::MakeUnique (int& riVertexQuantity, Vertex*& rakVertex,
    int& riEdgeQuantity, Edge*& rakEdge)
{
    if ( riVertexQuantity == 0 )
        return;

    // use a hash table to generate unique storage
    typedef map<Vertex,int> VMap;
    typedef map<Vertex,int>::iterator VIterator;
    VMap kVMap;
    for (int iV = 0, iNextVertex = 0; iV < riVertexQuantity; iV++)
    {
        pair<VIterator,bool> kResult = kVMap.insert(make_pair(rakVertex[iV],
            iNextVertex));
        if ( kResult.second == true )
            iNextVertex++;
    }

    // use a hash table to generate unique storage
    typedef map<Edge,int> EMap;
    typedef map<Edge,int>::iterator EIterator;
    EMap* pkEMap = NULL;
    int iE;
    VIterator pkVIter;

    if ( riEdgeQuantity )
    {
        pkEMap = new EMap;
        int iNextEdge = 0;
        for (iE = 0; iE < riEdgeQuantity; iE++)
        {
            // replace old vertex indices by new ones
            pkVIter = kVMap.find(rakVertex[rakEdge[iE].m_i0]);
            assert( pkVIter != kVMap.end() );
            rakEdge[iE].m_i0 = pkVIter->second;
            pkVIter = kVMap.find(rakVertex[rakEdge[iE].m_i1]);
            assert( pkVIter != kVMap.end() );
            rakEdge[iE].m_i1 = pkVIter->second;

            // keep only unique edges
            pair<EIterator,bool> kResult = pkEMap->insert(make_pair(
                rakEdge[iE],iNextEdge));
            if ( kResult.second == true )
                iNextEdge++;
        }
    }

    // pack vertices into an array
    delete[] rakVertex;
    if ( riVertexQuantity > 0 )
    {
        riVertexQuantity = kVMap.size();
        rakVertex = new Vertex[riVertexQuantity];
        for (pkVIter = kVMap.begin(); pkVIter != kVMap.end(); pkVIter++)
            rakVertex[pkVIter->second] = pkVIter->first;
    }
    else
    {
        rakVertex = NULL;
    }

    // pack edges into an array
    delete[] rakEdge;
    if ( riEdgeQuantity > 0 )
    {
        riEdgeQuantity = pkEMap->size();
        rakEdge = new Edge[riEdgeQuantity];
        EIterator pkEIter;
        for (pkEIter = pkEMap->begin(); pkEIter != pkEMap->end(); pkEIter++)
            rakEdge[pkEIter->second] = pkEIter->first;
        delete pkEMap;
    }
    else
    {
        rakEdge = NULL;
    }
}
//----------------------------------------------------------------------------
void ImageInterp2D::AddVertex (VertexArray& rkVArray, int iXNumer,
    int iXDenom, int iYNumer, int iYDenom)
{
#ifdef _DEBUG
    int iX = (int)(iXNumer/(float)iXDenom);
    int iY = (int)(iYNumer/(float)iYDenom);
    assert( 0 <= iX && iX < m_aiBound[0]
        &&  0 <= iY && iY < m_aiBound[1] );
#endif

    rkVArray.push_back(Vertex(iXNumer,iXDenom,iYNumer,iYDenom));
}
//----------------------------------------------------------------------------
void ImageInterp2D::AddEdge (VertexArray& rkVArray, EdgeArray& rkEArray,
    int iXNumer0, int iXDenom0, int iYNumer0, int iYDenom0, int iXNumer1,
    int iXDenom1, int iYNumer1, int iYDenom1)
{
#ifdef _DEBUG
    int iX = (int)(iXNumer0/(float)iXDenom0);
    int iY = (int)(iYNumer0/(float)iYDenom0);
    assert( 0 <= iX && iX < m_aiBound[0]
        &&  0 <= iY && iY < m_aiBound[1] );

    iX = (int)(iXNumer1/(float)iXDenom1);
    iY = (int)(iYNumer1/(float)iYDenom1);
    assert( 0 <= iX && iX < m_aiBound[0]
        &&  0 <= iY && iY < m_aiBound[1] );
#endif

    int iV0 = rkVArray.size(), iV1 = iV0+1;
    rkEArray.push_back(Edge(iV0,iV1));
    rkVArray.push_back(Vertex(iXNumer0,iXDenom0,iYNumer0,iYDenom0));
    rkVArray.push_back(Vertex(iXNumer1,iXDenom1,iYNumer1,iYDenom1));
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// linear interpolation and extraction
//----------------------------------------------------------------------------
float ImageInterp2D::LinearInterpolate (float fX, float fY) const
{
    int iX = (int) fX;
    if ( iX < 0 || iX >= m_aiBound[0] )
        return 0.0f;

    int iY = (int) fY;
    if ( iY < 0 || iY >= m_aiBound[1] )
        return 0.0f;

    float fDX = fX - iX, fDY = fY - iY;

    int i00 = iX + m_aiBound[0]*iY;
    int i10 = i00 + 1;
    int i01 = i00 + m_aiBound[0];
    int i11 = i10 + m_aiBound[0];
    float fF00 = (float) m_atData[i00];
    float fF10 = (float) m_atData[i10];
    float fF01 = (float) m_atData[i01];
    float fF11 = (float) m_atData[i11];
    float fInterp;

    int iXParity = (iX & 1), iYParity = (iY & 1);
    if ( iXParity == iYParity )
    {
        if ( fDX + fDY <= 1.0f )
            fInterp = fF00 + fDX*(fF10-fF00) + fDY*(fF01-fF00);
        else
            fInterp = fF10+fF01-fF11 + fDX*(fF11-fF01) + fDY*(fF11-fF10);
    }
    else
    {
        if ( fDY <= fDX )
            fInterp = fF00 + fDX*(fF10-fF00) + fDY*(fF11-fF10);
        else
            fInterp = fF00 + fDX*(fF11-fF01) + fDY*(fF01-fF00);
    }

    return fInterp;
}
//----------------------------------------------------------------------------
void ImageInterp2D::ExtractLevelSetLinear (int iLevel, int& riVertexQuantity,
    Vertex*& rakVertex, int& riEdgeQuantity, Edge*& rakEdge)
{
    // adjust image so level set is F(x,y) = 0
    int i;
    for (i = 0; i < m_iQuantity; i++)
        m_atData[i] = m_atData[i] - iLevel;

    VertexArray kVArray;
    EdgeArray kEArray;

    int iXBoundM1 = m_aiBound[0] - 1, iYBoundM1 = m_aiBound[1] - 1;
    for (int iY = 0, iYP = 1; iY < iYBoundM1; iY++, iYP++)
    {
        int iYParity = (iY & 1);

        for (int iX = 0, iXP = 1; iX < iXBoundM1; iX++, iXP++)
        {
            int iXParity = (iX & 1);

            int i00 = iX + m_aiBound[0]*iY;
            int i10 = i00 + 1;
            int i01 = i00 + m_aiBound[0];
            int i11 = i10 + m_aiBound[0];
            int iF00 = m_atData[i00];
            int iF10 = m_atData[i10];
            int iF01 = m_atData[i01];
            int iF11 = m_atData[i11];

            if ( iXParity == iYParity )
            {
                ProcessTriangle(iLevel,kVArray,kEArray,iX,iY,iF00,iX,iYP,
                    iF01,iXP,iY,iF10);

                ProcessTriangle(iLevel,kVArray,kEArray,iXP,iYP,iF11,iXP,iY,
                    iF10,iX,iYP,iF01);
            }
            else
            {
                ProcessTriangle(iLevel,kVArray,kEArray,iX,iYP,iF01,iXP,iYP,
                    iF11,iX,iY,iF00);

                ProcessTriangle(iLevel,kVArray,kEArray,iXP,iY,iF10,iX,iY,
                    iF00,iXP,iYP,iF11);
            }
        }
    }

    riVertexQuantity = kVArray.size();
    rakVertex = new Vertex[riVertexQuantity];
    memcpy(rakVertex,&kVArray.front(),riVertexQuantity*sizeof(Vertex));

    riEdgeQuantity = kEArray.size();
    rakEdge = new Edge[riEdgeQuantity];
    memcpy(rakEdge,&kEArray.front(),riEdgeQuantity*sizeof(Edge));

    // readjust image so level set is F(x,y) = L
    for (i = 0; i < m_iQuantity; i++)
        m_atData[i] = m_atData[i] + iLevel;
}
//----------------------------------------------------------------------------
void ImageInterp2D::ProcessTriangle (int iLevel, VertexArray& rkVA,
    EdgeArray& rkEA, int iX0, int iY0, int iF0, int iX1, int iY1, int iF1,
    int iX2, int iY2, int iF2)
{
    int iXN0, iYN0, iXN1, iYN1, iD0, iD1;

    if ( iF0 != 0 )
    {
        // convert to case "+**"
        if ( iF0 < 0 )
        {
            iF0 = -iF0;
            iF1 = -iF1;
            iF2 = -iF2;
        }

        if ( iF1 > 0 )
        {
            if ( iF2 > 0 )
            {
                // +++
                return;
            }
            else if ( iF2 < 0 )
            {
                // ++-
                iD0 = iF0 - iF2;
                iXN0 = iF0*iX2 - iF2*iX0;
                iYN0 = iF0*iY2 - iF2*iY0;
                iD1 = iF1 - iF2;
                iXN1 = iF1*iX2 - iF2*iX1;
                iYN1 = iF1*iY2 - iF2*iY1;
                AddEdge(rkVA,rkEA,iXN0,iD0,iYN0,iD0,iXN1,iD1,iYN1,iD1);
            }
            else
            {
                // ++0
                AddVertex(rkVA,iX2,1,iY2,1);
            }
        }
        else if ( iF1 < 0 )
        {
            iD0 = iF0 - iF1;
            iXN0 = iF0*iX1 - iF1*iX0;
            iYN0 = iF0*iY1 - iF1*iY0;

            if ( iF2 > 0 )
            {
                // +-+
                iD1 = iF2 - iF1;
                iXN1 = iF2*iX1 - iF1*iX2;
                iYN1 = iF2*iY1 - iF1*iY2;
                AddEdge(rkVA,rkEA,iXN0,iD0,iYN0,iD0,iXN1,iD1,iYN1,iD1);
            }
            else if ( iF2 < 0 )
            {
                // +--
                iD1 = iF2 - iF0;
                iXN1 = iF2*iX0 - iF0*iX2;
                iYN1 = iF2*iY0 - iF0*iY2;
                AddEdge(rkVA,rkEA,iXN0,iD0,iYN0,iD0,iXN1,iD1,iYN1,iD1);
            }
            else
            {
                // +-0
                AddEdge(rkVA,rkEA,iX2,1,iY2,1,iXN0,iD0,iYN0,iD0);
            }
        }
        else
        {
            if ( iF2 > 0 )
            {
                // +0+
                AddVertex(rkVA,iX1,1,iY1,1);
            }
            else if ( iF2 < 0 )
            {
                // +0-
                iD0 = iF2 - iF0;
                iXN0 = iF2*iX0 - iF0*iX2;
                iYN0 = iF2*iY0 - iF0*iY2;
                AddEdge(rkVA,rkEA,iX1,1,iY1,1,iXN0,iD0,iYN0,iD0);
            }
            else
            {
                // +00
                AddEdge(rkVA,rkEA,iX1,1,iY1,1,iX2,1,iY2,1);
            }
        }
    }
    else if ( iF1 != 0 )
    {
        // convert to case 0+*
        if ( iF1 < 0 )
        {
            iF1 = -iF1;
            iF2 = -iF2;
        }

        if ( iF2 > 0 )
        {
            // 0++
            AddVertex(rkVA,iX0,1,iY0,1);
        }
        else if ( iF2 < 0 )
        {
            // 0+-
            iD0 = iF1 - iF2;
            iXN0 = iF1*iX2 - iF2*iX1;
            iYN0 = iF1*iY2 - iF2*iY1;
            AddEdge(rkVA,rkEA,iX0,1,iY0,1,iXN0,iD0,iYN0,iD0);
        }
        else
        {
            // 0+0
            AddEdge(rkVA,rkEA,iX0,1,iY0,1,iX2,1,iY2,1);
        }
    }
    else if ( iF2 != 0 )
    {
        // cases 00+ or 00-
        AddEdge(rkVA,rkEA,iX0,1,iY0,1,iX1,1,iY1,1);
    }
    else
    {
        // case 000
        AddEdge(rkVA,rkEA,iX0,1,iY0,1,iX1,1,iY1,1);
        AddEdge(rkVA,rkEA,iX1,1,iY1,1,iX2,1,iY2,1);
        AddEdge(rkVA,rkEA,iX2,1,iY2,1,iX0,1,iY0,1);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// bilinear interpolation and extraction
//----------------------------------------------------------------------------
float ImageInterp2D::BilinearInterpolate (float fX, float fY) const
{
    int iX = (int) fX;
    if ( iX < 0 || iX >= m_aiBound[0] )
        return 0.0;

    int iY = (int) fY;
    if ( iY < 0 || iY >= m_aiBound[1] )
        return 0.0;

    float fDX = fX - iX, fDY = fY - iY;
    float fOmDX = 1.0f - fDX, fOmDY = 1.0f - fDY;

    int i00 = iX + m_aiBound[0]*iY;
    int i10 = i00 + 1;
    int i01 = i00 + m_aiBound[0];
    int i11 = i10 + m_aiBound[0];
    float fF00 = (float) m_atData[i00];
    float fF10 = (float) m_atData[i10];
    float fF01 = (float) m_atData[i01];
    float fF11 = (float) m_atData[i11];

    float fInterp = fOmDX*(fOmDY*fF00+fDY*fF01)+fDX*(fOmDY*fF10+fDY*fF11);
    return fInterp;
}
//----------------------------------------------------------------------------
void ImageInterp2D::ExtractLevelSetBilinear (int iLevel,
    int& riVertexQuantity, Vertex*& rakVertex, int& riEdgeQuantity,
    Edge*& rakEdge)
{
    // adjust image so level set is F(x,y) = 0
    int i;
    for (i = 0; i < m_iQuantity; i++)
        m_atData[i] = m_atData[i] - iLevel;

    VertexArray kVArray;
    EdgeArray kEArray;

    int iXBoundM1 = m_aiBound[0] - 1, iYBoundM1 = m_aiBound[1] - 1;
    for (int iY = 0, iYP = 1; iY < iYBoundM1; iY++, iYP++)
    {
        for (int iX = 0, iXP = 1; iX < iXBoundM1; iX++, iXP++)
        {
            int i00 = iX + m_aiBound[0]*iY;
            int i10 = i00 + 1;
            int i01 = i00 + m_aiBound[0];
            int i11 = i10 + m_aiBound[0];
            int iF00 = m_atData[i00];
            int iF10 = m_atData[i10];
            int iF01 = m_atData[i01];
            int iF11 = m_atData[i11];

            ProcessSquare(iLevel,kVArray,kEArray,iX,iXP,iY,iYP,iF00,iF10,
                iF11,iF01);
        }
    }

    riVertexQuantity = kVArray.size();
    rakVertex = new Vertex[riVertexQuantity];
    memcpy(rakVertex,&kVArray.front(),riVertexQuantity*sizeof(Vertex));

    riEdgeQuantity = kEArray.size();
    rakEdge = new Edge[riEdgeQuantity];
    memcpy(rakEdge,&kEArray.front(),riEdgeQuantity*sizeof(Edge));

    // readjust image so level set is F(x,y) = L
    for (i = 0; i < m_iQuantity; i++)
        m_atData[i] = m_atData[i] + iLevel;
}
//----------------------------------------------------------------------------
void ImageInterp2D::ProcessSquare (int iLevel, VertexArray& rkVA,
    EdgeArray& rkEA, int iX, int iXP, int iY, int iYP, int iF00, int iF10,
    int iF11, int iF01)
{
    int iXN0, iYN0, iXN1, iYN1, iD0, iD1, iD2, iD3, iDet;

    if ( iF00 != 0 )
    {
        // convert to case "+***"
        if ( iF00 < 0 )
        {
            iF00 = -iF00;
            iF10 = -iF10;
            iF11 = -iF11;
            iF01 = -iF01;
        }

        if ( iF10 > 0 )
        {
            if ( iF11 > 0 )
            {
                if ( iF01 > 0 )
                {
                    // ++++
                    return;
                }
                else if ( iF01 < 0 )
                {
                    // +++-
                    iD0 = iF11 - iF01;
                    iXN0 = iF11*iX - iF01*iXP;
                    iD1 = iF00 - iF01;
                    iYN1 = iF00*iYP - iF01*iY;
                    AddEdge(rkVA,rkEA,iXN0,iD0,iYP,1,iX,1,iYN1,iD1);
                }
                else
                {
                    // +++0
                    AddVertex(rkVA,iX,1,iYP,1);
                }
            }
            else if ( iF11 < 0 )
            {
                iD0 = iF10 - iF11;
                iYN0 = iF10*iYP - iF11*iY;

                if ( iF01 > 0 )
                {
                    // ++-+
                    iD1 = iF01 - iF11;
                    iXN1 = iF01*iXP - iF11*iX;
                    AddEdge(rkVA,rkEA,iXP,1,iYN0,iD0,iXN1,iD1,iYP,1);
                }
                else if ( iF01 < 0 )
                {
                    // ++--
                    iD1 = iF01 - iF00;
                    iYN1 = iF01*iY - iF00*iYP;
                    AddEdge(rkVA,rkEA,iX,1,iYN1,iD1,iXP,1,iYN0,iD0);
                }
                else
                {
                    // ++-0
                    AddEdge(rkVA,rkEA,iX,1,iYP,1,iXP,1,iYN0,iD0);
                }
            }
            else
            {
                if ( iF01 > 0 )
                {
                    // ++0+
                    AddVertex(rkVA,iXP,1,iYP,1);
                }
                else if ( iF01 < 0 )
                {
                    // ++0-
                    iD0 = iF01 - iF00;
                    iYN0 = iF01*iY - iF00*iYP;
                    AddEdge(rkVA,rkEA,iXP,1,iYP,1,iX,1,iYN0,iD0);
                }
                else
                {
                    // ++00
                    AddEdge(rkVA,rkEA,iXP,1,iYP,1,iX,1,iYP,1);
                }
            }
        }
        else if ( iF10 < 0 )
        {
            iD0 = iF00 - iF10;
            iXN0 = iF00*iXP - iF10*iX;

            if ( iF11 > 0 )
            {
                iD1 = iF11 - iF10;
                iYN1 = iF11*iY - iF10*iYP;

                if ( iF01 > 0 )
                {
                    // +-++
                    AddEdge(rkVA,rkEA,iXN0,iD0,iY,1,iXP,1,iYN1,iD1);
                }
                else if ( iF01 < 0 )
                {
                    // +-+-
                    iD3 = iF11 - iF01;
                    iXN1 = iF11*iX - iF01*iXP;
                    iD2 = iF01 - iF00;
                    iYN0 = iF01*iY - iF00*iYP;

                    if ( iD0*iD3 > 0 )
                        iDet = iXN1*iD0 - iXN0*iD3;
                    else
                        iDet = iXN0*iD3 - iXN1*iD0;

                    if ( iDet > 0 )
                    {
                        AddEdge(rkVA,rkEA,iXN1,iD3,iYP,1,iXP,1,iYN1,iD1);
                        AddEdge(rkVA,rkEA,iXN0,iD0,iY,1,iX,1,iYN0,iD2);
                    }
                    else if ( iDet < 0 )
                    {
                        AddEdge(rkVA,rkEA,iXN1,iD3,iYP,1,iX,1,iYN0,iD2);
                        AddEdge(rkVA,rkEA,iXN0,iD0,iY,1,iXP,1,iYN1,iD1);
                    }
                    else
                    {
                        AddEdge(rkVA,rkEA,iXN0,iD0,iYN0,iD2,iXN0,iD0,iY,1);
                        AddEdge(rkVA,rkEA,iXN0,iD0,iYN0,iD2,iXN0,iD0,iYP,1);
                        AddEdge(rkVA,rkEA,iXN0,iD0,iYN0,iD2,iX,1,iYN0,iD2);
                        AddEdge(rkVA,rkEA,iXN0,iD0,iYN0,iD2,iXP,1,iYN0,iD2);
                    }
                }
                else
                {
                    // +-+0
                    AddEdge(rkVA,rkEA,iXN0,iD0,iY,1,iXP,1,iYN1,iD1);
                    AddVertex(rkVA,iX,1,iYP,1);
                }
            }
            else if ( iF11 < 0 )
            {
                if ( iF01 > 0 )
                {
                    // +--+
                    iD1 = iF11 - iF01;
                    iXN1 = iF11*iX - iF01*iXP;
                    AddEdge(rkVA,rkEA,iXN0,iD0,iY,1,iXN1,iD1,iYP,1);
                }
                else if ( iF01 < 0 )
                {
                    // +---
                    iD1 = iF01 - iF00;
                    iYN1 = iF01*iY - iF00*iYP;
                    AddEdge(rkVA,rkEA,iX,1,iYN1,iD1,iXN0,iD0,iY,1);
                }
                else
                {
                    // +--0
                    AddEdge(rkVA,rkEA,iX,1,iYP,1,iXN0,iD0,iY,1);
                }
            }
            else
            {
                if ( iF01 > 0 )
                {
                    // +-0+
                    AddEdge(rkVA,rkEA,iXP,1,iYP,1,iXN0,iD0,iY,1);
                }
                else if ( iF01 < 0 )
                {
                    // +-0-
                    iD1 = iF01 - iF00;
                    iYN1 = iF01*iY - iF00*iYP;
                    AddEdge(rkVA,rkEA,iX,1,iYN1,iD1,iXN0,iD0,iY,1);
                    AddVertex(rkVA,iXP,1,iYP,1);
                }
                else
                {
                    // +-00
                    AddEdge(rkVA,rkEA,iXP,1,iYP,1,iXN0,iD0,iYP,1);
                    AddEdge(rkVA,rkEA,iXN0,iD0,iYP,1,iX,1,iYP,1);
                    AddEdge(rkVA,rkEA,iXN0,iD0,iYP,1,iXN0,iD0,iY,1);
                }
            }
        }
        else
        {
            if ( iF11 > 0 )
            {
                if ( iF01 > 0 )
                {
                    // +0++
                    AddVertex(rkVA,iXP,1,iY,1);
                }
                else if ( iF01 < 0 )
                {
                    // +0+-
                    iD0 = iF11 - iF01;
                    iXN0 = iF11*iX - iF01*iXP;
                    iD1 = iF00 - iF01;
                    iYN1 = iF00*iYP - iF01*iY;
                    AddEdge(rkVA,rkEA,iXN0,iD0,iYP,1,iX,1,iYN1,iD1);
                    AddVertex(rkVA,iXP,1,iY,1);
                }
                else
                {
                    // +0+0
                    AddVertex(rkVA,iXP,1,iY,1);
                    AddVertex(rkVA,iX,1,iYP,1);
                }
            }
            else if ( iF11 < 0 )
            {
                if ( iF01 > 0 )
                {
                    // +0-+
                    iD0 = iF11 - iF01;
                    iXN0 = iF11*iX - iF01*iXP;
                    AddEdge(rkVA,rkEA,iXP,1,iY,1,iXN0,iD0,iYP,1);
                }
                else if ( iF01 < 0 )
                {
                    // +0--
                    iF01 -= iLevel;
                    iF00 -= iLevel;
                    iD0 = iF01 - iF00;
                    iYN0 = iF01*iY - iF00*iYP;
                    AddEdge(rkVA,rkEA,iXP,1,iY,1,iX,1,iYN0,iD0);
                }
                else
                {
                    // +0-0
                    AddEdge(rkVA,rkEA,iXP,1,iY,1,iX,1,iYP,1);
                }
            }
            else
            {
                if ( iF01 > 0 )
                {
                    // +00+
                    AddEdge(rkVA,rkEA,iXP,1,iY,1,iXP,1,iYP,1);
                }
                else if ( iF01 < 0 )
                {
                    // +00-
                    iD0 = iF00 - iF01;
                    iYN0 = iF00*iYP - iF01*iY;
                    AddEdge(rkVA,rkEA,iXP,1,iY,1,iXP,1,iYN0,iD0);
                    AddEdge(rkVA,rkEA,iXP,1,iYN0,iD0,iXP,1,iYP,1);
                    AddEdge(rkVA,rkEA,iXP,1,iYN0,iD0,iX,1,iYN0,iD0);
                }
                else
                {
                    // +000
                    AddEdge(rkVA,rkEA,iX,1,iYP,1,iX,1,iY,1);
                    AddEdge(rkVA,rkEA,iX,1,iY,1,iXP,1,iY,1);
                }
            }
        }
    }
    else if ( iF10 != 0 )
    {
        // convert to case 0+**
        if ( iF10 < 0 )
        {
            iF10 = -iF10;
            iF11 = -iF11;
            iF01 = -iF01;
        }

        if ( iF11 > 0 )
        {
            if ( iF01 > 0 )
            {
                // 0+++
                AddVertex(rkVA,iX,1,iY,1);
            }
            else if ( iF01 < 0 )
            {
                // 0++-
                iD0 = iF11 - iF01;
                iXN0 = iF11*iX - iF01*iXP;
                AddEdge(rkVA,rkEA,iX,1,iY,1,iXN0,iD0,iYP,1);
            }
            else
            {
                // 0++0
                AddEdge(rkVA,rkEA,iX,1,iYP,1,iX,1,iY,1);
            }
        }
        else if ( iF11 < 0 )
        {
            if ( iF01 > 0 )
            {
                // 0+-+
                iD0 = iF10 - iF11;
                iYN0 = iF10*iYP - iF11*iY;
                iD1 = iF01 - iF11;
                iXN1 = iF01*iXP - iF11*iX;
                AddEdge(rkVA,rkEA,iXP,1,iYN0,iD0,iXN1,iD1,iYP,1);
                AddVertex(rkVA,iX,1,iY,1);
            }
            else if ( iF01 < 0 )
            {
                // 0+--
                iD0 = iF10 - iF11;
                iYN0 = iF10*iYP - iF11*iY;
                AddEdge(rkVA,rkEA,iX,1,iY,1,iXP,1,iYN0,iD0);
            }
            else
            {
                // 0+-0
                iD0 = iF10 - iF11;
                iYN0 = iF10*iYP - iF11*iY;
                AddEdge(rkVA,rkEA,iX,1,iY,1,iX,1,iYN0,iD0);
                AddEdge(rkVA,rkEA,iX,1,iYN0,iD0,iX,1,iYP,1);
                AddEdge(rkVA,rkEA,iX,1,iYN0,iD0,iXP,1,iYN0,iD0);
            }
        }
        else
        {
            if ( iF01 > 0 )
            {
                // 0+0+
                AddVertex(rkVA,iX,1,iY,1);
                AddVertex(rkVA,iXP,1,iYP,1);
            }
            else if ( iF01 < 0 )
            {
                // 0+0-
                AddEdge(rkVA,rkEA,iX,1,iY,1,iXP,1,iYP,1);
            }
            else
            {
                // 0+00
                AddEdge(rkVA,rkEA,iXP,1,iYP,1,iX,1,iYP,1);
                AddEdge(rkVA,rkEA,iX,1,iYP,1,iX,1,iY,1);
            }
        }
    }
    else if ( iF11 != 0 )
    {
        // convert to case 00+*
        if ( iF11 < 0 )
        {
            iF11 = -iF11;
            iF01 = -iF01;
        }

        if ( iF01 > 0 )
        {
            // 00++
            AddEdge(rkVA,rkEA,iX,1,iY,1,iXP,1,iY,1);
        }
        else if ( iF01 < 0 )
        {
            // 00+-
            iD0 = iF01 - iF11;
            iXN0 = iF01*iXP - iF11*iX;
            AddEdge(rkVA,rkEA,iX,1,iY,1,iXN0,iD0,iY,1);
            AddEdge(rkVA,rkEA,iXN0,iD0,iY,1,iXP,1,iY,1);
            AddEdge(rkVA,rkEA,iXN0,iD0,iY,1,iXN0,iD0,iYP,1);
        }
        else
        {
            // 00+0
            AddEdge(rkVA,rkEA,iXP,1,iY,1,iXP,1,iYP,1);
            AddEdge(rkVA,rkEA,iXP,1,iYP,1,iX,1,iYP,1);
        }
    }
    else if ( iF01 != 0 )
    {
        // cases 000+ or 000-
        AddEdge(rkVA,rkEA,iX,1,iY,1,iXP,1,iY,1);
        AddEdge(rkVA,rkEA,iXP,1,iY,1,iXP,1,iYP,1);
    }
    else
    {
        // case 0000
        AddEdge(rkVA,rkEA,iX,1,iY,1,iXP,1,iY,1);
        AddEdge(rkVA,rkEA,iXP,1,iY,1,iXP,1,iYP,1);
        AddEdge(rkVA,rkEA,iXP,1,iYP,1,iX,1,iYP,1);
        AddEdge(rkVA,rkEA,iX,1,iYP,1,iX,1,iY,1);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Support for STL maps involving Vertex and Edge keys.  I tried using the
// equal_to comparison and had two problems.
//   1.  typedef map<Vertex,int,equal_to<Vertex> > VMap;
//       The space between the last two '>' symbol *must* occur, otherwise
//       the compiler complains about a mising ',' (go figure).
//   2.  The first VMap 'find' call fails (the assert is fired).  I have no
//       idea why.
//----------------------------------------------------------------------------
bool ImageInterp2D::Vertex::operator< (const Vertex& rkVertex) const
{
    unsigned int auiValue0[4] =
    {
        *(unsigned int*)&m_iXNumer,
        *(unsigned int*)&m_iXDenom,
        *(unsigned int*)&m_iYNumer,
        *(unsigned int*)&m_iYDenom
    };

    unsigned int auiValue1[4] =
    {
        *(unsigned int*)&rkVertex.m_iXNumer,
        *(unsigned int*)&rkVertex.m_iXDenom,
        *(unsigned int*)&rkVertex.m_iYNumer,
        *(unsigned int*)&rkVertex.m_iYDenom
    };

    for (int i = 0; i < 4; i++)
    {
        if ( auiValue0[i] < auiValue1[i] )
            return true;
        if ( auiValue0[i] > auiValue1[i] )
            return false;
    }

    return false;
}
//----------------------------------------------------------------------------
bool ImageInterp2D::Edge::operator< (const Edge& rkEdge) const
{
    unsigned int auiValue0[2] =
    {
        *(unsigned int*)&m_i0,
        *(unsigned int*)&m_i1
    };

    unsigned int auiValue1[2] =
    {
        *(unsigned int*)&rkEdge.m_i0,
        *(unsigned int*)&rkEdge.m_i1
    };

    for (int i = 0; i < 2; i++)
    {
        if ( auiValue0[i] < auiValue1[i] )
            return true;
        if ( auiValue0[i] > auiValue1[i] )
            return false;
    }

    return false;
}
//----------------------------------------------------------------------------


