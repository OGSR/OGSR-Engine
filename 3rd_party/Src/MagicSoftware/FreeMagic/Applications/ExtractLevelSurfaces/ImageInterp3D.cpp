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

#include "ImageInterp3D.h"
#include "MgcTriangleMesh.h"
using namespace Mgc;

#include <algorithm>
#include <cfloat>
using namespace std;

//----------------------------------------------------------------------------
ImageInterp3D::ImageInterp3D (int iXBound, int iYBound, int iZBound,
    int* aiData)
    :
    ImageInt3D(iXBound,iYBound,iZBound,(Eint*)aiData)
{
    m_iXYProduct = iXBound*iYBound;
}
//----------------------------------------------------------------------------
ImageInterp3D::ImageInterp3D (const char* acFilename)
    :
    ImageInt3D(acFilename)
{
    if ( m_aiBound )
        m_iXYProduct = m_aiBound[0]*m_aiBound[1];
    else
        m_iXYProduct = 0;
}
//----------------------------------------------------------------------------
float ImageInterp3D::GetFunction (const Vector3& rkP) const
{
    int iX = (int) rkP.x;
    if ( iX < 0 || iX >= m_aiBound[0]-1 )
        return 0.0f;

    int iY = (int) rkP.y;
    if ( iY < 0 || iY >= m_aiBound[1]-1 )
        return 0.0f;

    int iZ = (int) rkP.z;
    if ( iZ < 0 || iZ >= m_aiBound[2]-1 )
        return 0.0f;

    float fDX = rkP.x - iX, fDY = rkP.y - iY, fDZ = rkP.z - iZ;

    int i000 = iX + m_aiBound[0]*(iY + m_aiBound[1]*iZ);
    int i100 = i000 + 1;
    int i010 = i000 + m_aiBound[0];
    int i110 = i100 + m_aiBound[0];
    int i001 = i000 + m_iXYProduct;
    int i101 = i100 + m_iXYProduct;
    int i011 = i010 + m_iXYProduct;
    int i111 = i110 + m_iXYProduct;
    float fF000 = (float) m_atData[i000];
    float fF100 = (float) m_atData[i100];
    float fF010 = (float) m_atData[i010];
    float fF110 = (float) m_atData[i110];
    float fF001 = (float) m_atData[i001];
    float fF101 = (float) m_atData[i101];
    float fF011 = (float) m_atData[i011];
    float fF111 = (float) m_atData[i111];
    float fC0, fC1, fC2, fInterp;

    if ( (iX & 1) ^ (iY & 1) ^ (iZ & 1) )
    {
        if ( fDX - fDY - fDZ >= 0.0f )
        {
            // 1205
            fInterp =
                (1.0f-(1.0f-fDX)-fDY-fDZ)*fF100 +
                (1.0f-fDX)*fF000 +
                fDY*fF110 +
                fDZ*fF101;
        }
        else if ( fDX - fDY + fDZ <= 0.0f )
        {
            // 3027
            fInterp =
                (1.0f-fDX-(1.0f-fDY)-fDZ)*fF010 +
                fDX*fF110 +
                (1.0f-fDY)*fF000 +
                fDZ*fF011;
        }
        else if ( fDX + fDY - fDZ <= 0.0f )
        {
            // 4750
            fInterp =
                (1.0f-fDX-fDY-(1-fDZ))*fF001 +
                fDX*fF101 +
                fDY*fF011 +
                (1.0f-fDZ)*fF000;
        }
        else if ( fDX + fDY + fDZ >= 2.0f )
        {
            // 6572
            fInterp =
                (1.0f-(1.0f-fDX)-(1.0f-fDY)-(1.0f-fDZ))*fF111 +
                (1.0f-fDX)*fF011 +
                (1.0f-fDY)*fF101 +
                (1.0f-fDZ)*fF110;
        }
        else
        {
            // 0752
            fC0 = 0.5f*(-fDX+fDY+fDZ);
            fC1 = 0.5f*(fDX-fDY+fDZ);
            fC2 = 0.5f*(fDX+fDY-fDZ);
            fInterp =
                (1.0f-fC0-fC1-fC2)*fF000 +
                fC0*fF011 +
                fC1*fF101 +
                fC2*fF110;
        }
    }
    else
    {
        if ( fDX + fDY + fDZ <= 1.0f )
        {
            // 0134
            fInterp =
                (1.0f-fDX-fDY-fDZ)*fF000 +
                fDX*fF100 +
                fDY*fF010 +
                fDZ*fF001;
        }
        else if ( fDX + fDY - fDZ >= 1.0f )
        {
            // 2316
            fInterp =
                (1.0f-(1.0f-fDX)-(1.0f-fDY)-fDZ)*fF110 +
                (1.0f-fDX)*fF010 +
                (1.0f-fDY)*fF100 +
                fDZ*fF111;
        }
        else if ( fDX - fDY + fDZ >= 1.0f )
        {
            // 5461
            fInterp =
                (1.0f-(1.0f-fDX)-fDY-(1.0f-fDZ))*fF101 +
                (1.0f-fDX)*fF001 +
                fDY*fF111 +
                (1.0f-fDZ)*fF100;
        }
        else if ( -fDX + fDY + fDZ >= 1.0f )
        {
            // 7643
            fInterp =
                (1.0f-fDX-(1.0f-fDY)-(1.0f-fDZ))*fF011 +
                fDX*fF111 +
                (1.0f-fDY)*fF001 +
                (1.0f-fDZ)*fF010;
        }
        else
        {
            // 6314
            fC0 = 0.5f*((1.0f-fDX)-(1.0f-fDY)+(1.0f-fDZ));
            fC1 = 0.5f*(-(1.0f-fDX)+(1.0f-fDY)+(1.0f-fDZ));
            fC2 = 0.5f*((1.0f-fDX)+(1.0f-fDY)-(1.0f-fDZ));
            fInterp =
                (1.0f-fC0-fC1-fC2)*fF111 +
                fC0*fF010 +
                fC1*fF100 +
                fC2*fF001;
        }
    }

    return fInterp;
}
//----------------------------------------------------------------------------
Vector3 ImageInterp3D::GetGradient (const Vector3& rkP) const
{
    int iX = (int) rkP.x;
    if ( iX < 0 || iX >= m_aiBound[0]-1 )
        return Vector3::ZERO;

    int iY = (int) rkP.y;
    if ( iY < 0 || iY >= m_aiBound[1]-1 )
        return Vector3::ZERO;

    int iZ = (int) rkP.z;
    if ( iZ < 0 || iZ >= m_aiBound[2]-1 )
        return Vector3::ZERO;

    float fDX = rkP.x - iX, fDY = rkP.y - iY, fDZ = rkP.z - iZ;

    int i000 = iX + m_aiBound[0]*(iY + m_aiBound[1]*iZ);
    int i100 = i000 + 1;
    int i010 = i000 + m_aiBound[0];
    int i110 = i100 + m_aiBound[0];
    int i001 = i000 + m_iXYProduct;
    int i101 = i100 + m_iXYProduct;
    int i011 = i010 + m_iXYProduct;
    int i111 = i110 + m_iXYProduct;
    float fF000 = (float) m_atData[i000];
    float fF100 = (float) m_atData[i100];
    float fF010 = (float) m_atData[i010];
    float fF110 = (float) m_atData[i110];
    float fF001 = (float) m_atData[i001];
    float fF101 = (float) m_atData[i101];
    float fF011 = (float) m_atData[i011];
    float fF111 = (float) m_atData[i111];
    Vector3 kInterp;

    if ( (iX & 1) ^ (iY & 1) ^ (iZ & 1) )
    {
        if ( fDX - fDY - fDZ >= 0.0f )
        {
            // 1205
            kInterp.x = + fF100 - fF000;
            kInterp.y = - fF100 + fF110;
            kInterp.z = - fF100 + fF101; 
        }
        else if ( fDX - fDY + fDZ <= 0.0f )
        {
            // 3027
            kInterp.x = - fF010 + fF110;
            kInterp.y = + fF010 - fF000;
            kInterp.z = - fF010 + fF011;
        }
        else if ( fDX + fDY - fDZ <= 0.0f )
        {
            // 4750
            kInterp.x = - fF001 + fF101;
            kInterp.y = - fF001 + fF011;
            kInterp.z = + fF001 - fF000;
        }
        else if ( fDX + fDY + fDZ >= 2.0f )
        {
            // 6572
            kInterp.x = + fF111 - fF011;
            kInterp.y = + fF111 - fF101;
            kInterp.z = + fF111 - fF110;
        }
        else
        {
            // 0752
            kInterp.x = 0.5f*(-fF000-fF011+fF101+fF110);
            kInterp.y = 0.5f*(-fF000+fF011-fF101+fF110);
            kInterp.z = 0.5f*(-fF000+fF011+fF101-fF110);
        }
    }
    else
    {
        if ( fDX + fDY + fDZ <= 1.0f )
        {
            // 0134
            kInterp.x = - fF000 + fF100;
            kInterp.y = - fF000 + fF010;
            kInterp.z = - fF000 + fF001;
        }
        else if ( fDX + fDY - fDZ >= 1.0f )
        {
            // 2316
            kInterp.x = + fF110 - fF010;
            kInterp.y = + fF110 - fF100;
            kInterp.z = - fF110 + fF111;
        }
        else if ( fDX - fDY + fDZ >= 1.0f )
        {
            // 5461
            kInterp.x = + fF101 - fF001;
            kInterp.y = - fF101 + fF111;
            kInterp.z = + fF101 - fF100;
        }
        else if ( -fDX + fDY + fDZ >= 1.0f )
        {
            // 7643
            kInterp.x = - fF011 + fF111;
            kInterp.y = + fF011 - fF001;
            kInterp.z = + fF011 - fF010;
        }
        else
        {
            // 6314
            kInterp.x = 0.5f*(fF111-fF010+fF100-fF001);
            kInterp.y = 0.5f*(fF111+fF010-fF100-fF001);
            kInterp.z = 0.5f*(fF111-fF010-fF100+fF001);
        }
    }

    return kInterp;
}
//----------------------------------------------------------------------------
void ImageInterp3D::ExtractLevelSetLinear (int iLevel, int& riVertexQuantity,
    Vertex*& rakVertex, int& riEdgeQuantity, Edge*& rakEdge,
    int& riTriangleQuantity, Triangle*& rakTriangle)
{
    VMap kVMap;
    ESet kESet;
    TSet kTSet;
    m_iNextIndex = 0;

    // adjust image so level set is F(x,y,z) = 0
    int i;
    for (i = 0; i < m_iQuantity; i++)
        m_atData[i] = m_atData[i] - iLevel;

    int iXBoundM1 = m_aiBound[0] - 1;
    int iYBoundM1 = m_aiBound[1] - 1;
    int iZBoundM1 = m_aiBound[2] - 1;
    for (int iZ = 0, iZP = 1; iZ < iZBoundM1; iZ++, iZP++)
    {
        int iZParity = (iZ & 1);

        for (int iY = 0, iYP = 1; iY < iYBoundM1; iY++, iYP++)
        {
            int iYParity = (iY & 1);

            for (int iX = 0, iXP = 1; iX < iXBoundM1; iX++, iXP++)
            {
                int iXParity = (iX & 1);

                int i000 = iX + m_aiBound[0]*(iY + m_aiBound[1]*iZ);
                int i100 = i000 + 1;
                int i010 = i000 + m_aiBound[0];
                int i110 = i100 + m_aiBound[0];
                int i001 = i000 + m_iXYProduct;
                int i101 = i100 + m_iXYProduct;
                int i011 = i010 + m_iXYProduct;
                int i111 = i110 + m_iXYProduct;
                int iF000 = m_atData[i000];
                int iF100 = m_atData[i100];
                int iF010 = m_atData[i010];
                int iF110 = m_atData[i110];
                int iF001 = m_atData[i001];
                int iF101 = m_atData[i101];
                int iF011 = m_atData[i011];
                int iF111 = m_atData[i111];

                if ( iXParity ^ iYParity ^ iZParity )
                {
                    // 1205
                    ProcessTetrahedron(iLevel,kVMap,kESet,kTSet,
                        iXP,iY,iZ,iF100,iXP,iYP,iZ,iF110,iX,iY,iZ,iF000,iXP,
                        iY,iZP,iF101);

                    // 3027
                    ProcessTetrahedron(iLevel,kVMap,kESet,kTSet,
                        iX,iYP,iZ,iF010,iX,iY,iZ,iF000,iXP,iYP,iZ,iF110,iX,
                        iYP,iZP,iF011);

                    // 4750
                    ProcessTetrahedron(iLevel,kVMap,kESet,kTSet,
                        iX,iY,iZP,iF001,iX,iYP,iZP,iF011,iXP,iY,iZP,iF101,iX,
                        iY,iZ,iF000);

                    // 6572
                    ProcessTetrahedron(iLevel,kVMap,kESet,kTSet,
                        iXP,iYP,iZP,iF111,iXP,iY,iZP,iF101,iX,iYP,iZP,iF011,
                        iXP,iYP,iZ,iF110);

                    // 0752
                    ProcessTetrahedron(iLevel,kVMap,kESet,kTSet,
                        iX,iY,iZ,iF000,iX,iYP,iZP,iF011,iXP,iY,iZP,iF101,iXP,
                        iYP,iZ,iF110);
                }
                else
                {
                    // 0134
                    ProcessTetrahedron(iLevel,kVMap,kESet,kTSet,
                        iX,iY,iZ,iF000,iXP,iY,iZ,iF100,iX,iYP,iZ,iF010,iX,iY,
                        iZP,iF001);

                    // 2316
                    ProcessTetrahedron(iLevel,kVMap,kESet,kTSet,
                        iXP,iYP,iZ,iF110,iX,iYP,iZ,iF010,iXP,iY,iZ,iF100,iXP,
                        iYP,iZP,iF111);

                    // 5461
                    ProcessTetrahedron(iLevel,kVMap,kESet,kTSet,
                        iXP,iY,iZP,iF101,iX,iY,iZP,iF001,iXP,iYP,iZP,iF111,
                        iXP,iY,iZ,iF100);

                    // 7643
                    ProcessTetrahedron(iLevel,kVMap,kESet,kTSet,
                        iX,iYP,iZP,iF011,iXP,iYP,iZP,iF111,iX,iY,iZP,iF001,iX,
                        iYP,iZ,iF010);

                    // 6314
                    ProcessTetrahedron(iLevel,kVMap,kESet,kTSet,
                        iXP,iYP,iZP,iF111,iX,iYP,iZ,iF010,iXP,iY,iZ,iF100,iX,
                        iY,iZP,iF001);
                }
            }
        }
    }

    // readjust image so level set is F(x,y,z) = L
    for (i = 0; i < m_iQuantity; i++)
        m_atData[i] = m_atData[i] + iLevel;

    // pack vertices into an array
    riVertexQuantity = kVMap.size();
    if ( riVertexQuantity > 0 )
    {
        rakVertex = new Vertex[riVertexQuantity];
        VIterator pkVIter;
        for (pkVIter = kVMap.begin(); pkVIter != kVMap.end(); pkVIter++)
            rakVertex[pkVIter->second] = pkVIter->first;
    }
    else
    {
        rakVertex = NULL;
    }

    // pack edges into an array
    riEdgeQuantity = kESet.size();
    if ( riEdgeQuantity > 0 )
    {
        rakEdge = new Edge[riEdgeQuantity];
        copy(kESet.begin(),kESet.end(),rakEdge);
    }
    else
    {
        rakEdge = NULL;
    }


    // pack triangles into an array
    riTriangleQuantity = kTSet.size();
    if ( riTriangleQuantity > 0 )
    {
        rakTriangle = new Triangle[riTriangleQuantity];
        copy(kTSet.begin(),kTSet.end(),rakTriangle);
    }
    else
    {
        rakTriangle = NULL;
    }
}
//----------------------------------------------------------------------------
int ImageInterp3D::AddVertex (VMap& rkVMap, int iXNumer, int iXDenom,
    int iYNumer, int iYDenom, int iZNumer, int iZDenom)
{
#ifdef _DEBUG
    int iX = (int)(iXNumer/(float)iXDenom);
    int iY = (int)(iYNumer/(float)iYDenom);
    int iZ = (int)(iZNumer/(float)iZDenom);
    assert( 0 <= iX && iX < m_aiBound[0]
        &&  0 <= iY && iY < m_aiBound[1]
        &&  0 <= iZ && iZ < m_aiBound[2] );
#endif

    Vertex kVertex(iXNumer,iXDenom,iYNumer,iYDenom,iZNumer,iZDenom);
    VIterator pkVIter = rkVMap.find(kVertex);
    if ( pkVIter != rkVMap.end() )
    {
        // Vertex already in map, just return its unique index.
        return pkVIter->second;
    }
    else
    {
        // Vertex not in map, insert it and assign it a unique index.
        int i = m_iNextIndex++;
        pair<VIterator,bool> kResult = rkVMap.insert(make_pair(kVertex,i));
        return i;
    }
}
//----------------------------------------------------------------------------
void ImageInterp3D::AddEdge (VMap& rkVMap, ESet& rkESet, int iXNumer0,
    int iXDenom0, int iYNumer0, int iYDenom0, int iZNumer0, int iZDenom0,
    int iXNumer1, int iXDenom1, int iYNumer1, int iYDenom1, int iZNumer1,
    int iZDenom1)
{
#ifdef _DEBUG
    int iX = (int)(iXNumer0/(float)iXDenom0);
    int iY = (int)(iYNumer0/(float)iYDenom0);
    int iZ = (int)(iZNumer0/(float)iZDenom0);
    assert( 0 <= iX && iX < m_aiBound[0]
        &&  0 <= iY && iY < m_aiBound[1]
        &&  0 <= iZ && iZ < m_aiBound[2] );

    iX = (int)(iXNumer1/(float)iXDenom1);
    iY = (int)(iYNumer1/(float)iYDenom1);
    iZ = (int)(iZNumer1/(float)iZDenom1);
    assert( 0 <= iX && iX < m_aiBound[0]
        &&  0 <= iY && iY < m_aiBound[1]
        &&  0 <= iZ && iZ < m_aiBound[2] );
#endif

    int iV0 = AddVertex(rkVMap,iXNumer0,iXDenom0,iYNumer0,iYDenom0,iZNumer0,
        iZDenom0);

    int iV1 = AddVertex(rkVMap,iXNumer1,iXDenom1,iYNumer1,iYDenom1,iZNumer1,
        iZDenom1);

    rkESet.insert(Edge(iV0,iV1));
}
//----------------------------------------------------------------------------
void ImageInterp3D::AddTriangle (VMap& rkVMap, ESet& rkESet, TSet& rkTSet,
    int iXNumer0, int iXDenom0, int iYNumer0, int iYDenom0, int iZNumer0,
    int iZDenom0, int iXNumer1, int iXDenom1, int iYNumer1, int iYDenom1,
    int iZNumer1, int iZDenom1, int iXNumer2, int iXDenom2, int iYNumer2,
    int iYDenom2, int iZNumer2, int iZDenom2)
{
#ifdef _DEBUG
    int iX = (int)(iXNumer0/(float)iXDenom0);
    int iY = (int)(iYNumer0/(float)iYDenom0);
    int iZ = (int)(iZNumer0/(float)iZDenom0);
    assert( 0 <= iX && iX < m_aiBound[0]
        &&  0 <= iY && iY < m_aiBound[1]
        &&  0 <= iZ && iZ < m_aiBound[2] );

    iX = (int)(iXNumer1/(float)iXDenom1);
    iY = (int)(iYNumer1/(float)iYDenom1);
    iZ = (int)(iZNumer1/(float)iZDenom1);
    assert( 0 <= iX && iX < m_aiBound[0]
        &&  0 <= iY && iY < m_aiBound[1]
        &&  0 <= iZ && iZ < m_aiBound[2] );

    iX = (int)(iXNumer2/(float)iXDenom2);
    iY = (int)(iYNumer2/(float)iYDenom2);
    iZ = (int)(iZNumer2/(float)iZDenom2);
    assert( 0 <= iX && iX < m_aiBound[0]
        &&  0 <= iY && iY < m_aiBound[1]
        &&  0 <= iZ && iZ < m_aiBound[2] );
#endif

    int iV0 = AddVertex(rkVMap,iXNumer0,iXDenom0,iYNumer0,iYDenom0,iZNumer0,
        iZDenom0);

    int iV1 = AddVertex(rkVMap,iXNumer1,iXDenom1,iYNumer1,iYDenom1,iZNumer1,
        iZDenom1);

    int iV2 = AddVertex(rkVMap,iXNumer2,iXDenom2,iYNumer2,iYDenom2,iZNumer2,
        iZDenom2);

    // nothing to do if triangle already exists
    Triangle kT(iV0,iV1,iV2);
    if ( rkTSet.find(kT) != rkTSet.end() )
        return;

    // prevent double-sided triangles
    int iSave = kT.m_i1;
    kT.m_i1 = kT.m_i2;
    kT.m_i2 = iSave;
    if ( rkTSet.find(kT) != rkTSet.end() )
        return;

    rkESet.insert(Edge(iV0,iV1));
    rkESet.insert(Edge(iV1,iV2));
    rkESet.insert(Edge(iV2,iV0));

    // compute triangle normal assuming counterclockwise ordering
    Vector3 kV0(
        iXNumer0/(float)iXDenom0,
        iYNumer0/(float)iYDenom0,
        iZNumer0/(float)iZDenom0);

    Vector3 kV1(
        iXNumer1/(float)iXDenom1,
        iYNumer1/(float)iYDenom1,
        iZNumer1/(float)iZDenom1);

    Vector3 kV2(
        iXNumer2/(float)iXDenom2,
        iYNumer2/(float)iYDenom2,
        iZNumer2/(float)iZDenom2);

    Vector3 kE0 = kV1 - kV0;
    Vector3 kE1 = kV2 - kV0;
    Vector3 kN = kE0.Cross(kE1);

    // choose triangle orientation based on gradient direction
    const float fOneThird = 1.0f/3.0f;
    Vector3 kCentroid = fOneThird*(kV0+kV1+kV2);
    Vector3 kGrad = GetGradient(kCentroid);
    if ( kGrad.Dot(kN) <= 0.0f )
        rkTSet.insert(Triangle(iV0,iV1,iV2));
    else
        rkTSet.insert(Triangle(iV0,iV2,iV1));
}
//----------------------------------------------------------------------------
void ImageInterp3D::ProcessTetrahedron (int iLevel, VMap& rkVM,
    ESet& rkES, TSet& rkTS, int iX0, int iY0, int iZ0, int iF0,
    int iX1, int iY1, int iZ1, int iF1, int iX2, int iY2, int iZ2, int iF2,
    int iX3, int iY3, int iZ3, int iF3)
{
    int iXN0, iYN0, iZN0, iD0;
    int iXN1, iYN1, iZN1, iD1;
    int iXN2, iYN2, iZN2, iD2;
    int iXN3, iYN3, iZN3, iD3;
    //int iV0, iV1, iV2;

    if ( iF0 != 0 )
    {
        // convert to case +***
        if ( iF0 < 0 )
        {
            iF0 = -iF0;
            iF1 = -iF1;
            iF2 = -iF2;
            iF3 = -iF3;
        }

        if ( iF1 > 0 )
        {
            if ( iF2 > 0 )
            {
                if ( iF3 > 0 )
                {
                    // ++++
                    return;
                }
                else if ( iF3 < 0 )
                {
                    // +++-
                    iD0 = iF0 - iF3;
                    iXN0 = iF0*iX3 - iF3*iX0;
                    iYN0 = iF0*iY3 - iF3*iY0;
                    iZN0 = iF0*iZ3 - iF3*iZ0;
                    iD1 = iF1 - iF3;
                    iXN1 = iF1*iX3 - iF3*iX1;
                    iYN1 = iF1*iY3 - iF3*iY1;
                    iZN1 = iF1*iZ3 - iF3*iZ1;
                    iD2 = iF2 - iF3;
                    iXN2 = iF2*iX3 - iF3*iX2;
                    iYN2 = iF2*iY3 - iF3*iY2;
                    iZN2 = iF2*iZ3 - iF3*iZ2;
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iXN2,iD2,iYN2,iD2,iZN2,iD2);
                }
                else
                {
                    // +++0
                    AddVertex(rkVM,iX3,1,iY3,1,iZ3,1);
                }
            }
            else if ( iF2 < 0 )
            {
                iD0 = iF0 - iF2;
                iXN0 = iF0*iX2 - iF2*iX0;
                iYN0 = iF0*iY2 - iF2*iY0;
                iZN0 = iF0*iZ2 - iF2*iZ0;
                iD1 = iF1 - iF2;
                iXN1 = iF1*iX2 - iF2*iX1;
                iYN1 = iF1*iY2 - iF2*iY1;
                iZN1 = iF1*iZ2 - iF2*iZ1;

                if ( iF3 > 0 )
                {
                    // ++-+
                    iD2 = iF3 - iF2;
                    iXN2 = iF3*iX2 - iF2*iX3;
                    iYN2 = iF3*iY2 - iF2*iY3;
                    iZN2 = iF3*iZ2 - iF2*iZ3;
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iXN2,iD2,iYN2,iD2,iZN2,iD2);
                }
                else if ( iF3 < 0 )
                {
                    // ++--
                    iD2 = iF0 - iF3;
                    iXN2 = iF0*iX3 - iF3*iX0;
                    iYN2 = iF0*iY3 - iF3*iY0;
                    iZN2 = iF0*iZ3 - iF3*iZ0;
                    iD3 = iF1 - iF3;
                    iXN3 = iF1*iX3 - iF3*iX1;
                    iYN3 = iF1*iY3 - iF3*iY1;
                    iZN3 = iF1*iZ3 - iF3*iZ1;
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iXN2,iD2,iYN2,iD2,iZN2,iD2);
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iXN3,iD3,iYN3,iD3,iZN3,iD3,
                        iXN2,iD2,iYN2,iD2,iZN2,iD2);
                }
                else
                {
                    // ++-0
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iX3,1,iY3,1,iZ3,1);
                }
            }
            else
            {
                if ( iF3 > 0 )
                {
                    // ++0+
                    AddVertex(rkVM,iX2,1,iY2,1,iZ2,1);
                }
                else if ( iF3 < 0 )
                {
                    // ++0-
                    iD0 = iF0 - iF3;
                    iXN0 = iF0*iX3 - iF3*iX0;
                    iYN0 = iF0*iY3 - iF3*iY0;
                    iZN0 = iF0*iZ3 - iF3*iZ0;
                    iD1 = iF1 - iF3;
                    iXN1 = iF1*iX3 - iF3*iX1;
                    iYN1 = iF1*iY3 - iF3*iY1;
                    iZN1 = iF1*iZ3 - iF3*iZ1;
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iX2,1,iY2,1,iZ2,1);
                }
                else
                {
                    // ++00
                    AddEdge(rkVM,rkES,iX2,1,iY2,1,iZ2,1,iX3,1,iY3,1,iZ3,1);
                }
            }
        }
        else if ( iF1 < 0 )
        {
            if ( iF2 > 0 )
            {
                iD0 = iF0 - iF1;
                iXN0 = iF0*iX1 - iF1*iX0;
                iYN0 = iF0*iY1 - iF1*iY0;
                iZN0 = iF0*iZ1 - iF1*iZ0;
                iD1 = iF2 - iF1;
                iXN1 = iF2*iX1 - iF1*iX2;
                iYN1 = iF2*iY1 - iF1*iY2;
                iZN1 = iF2*iZ1 - iF1*iZ2;

                if ( iF3 > 0 )
                {
                    // +-++
                    iD2 = iF3 - iF1;
                    iXN2 = iF3*iX1 - iF1*iX3;
                    iYN2 = iF3*iY1 - iF1*iY3;
                    iZN2 = iF3*iZ1 - iF1*iZ3;
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iXN2,iD2,iYN2,iD2,iZN2,iD2);
                }
                else if ( iF3 < 0 )
                {
                    // +-+-
                    iD2 = iF0 - iF3;
                    iXN2 = iF0*iX3 - iF3*iX0;
                    iYN2 = iF0*iY3 - iF3*iY0;
                    iZN2 = iF0*iZ3 - iF3*iZ0;
                    iD3 = iF2 - iF3;
                    iXN3 = iF2*iX3 - iF3*iX2;
                    iYN3 = iF2*iY3 - iF3*iY2;
                    iZN3 = iF2*iZ3 - iF3*iZ2;
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iXN2,iD2,iYN2,iD2,iZN2,iD2);
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iXN3,iD3,iYN3,iD3,iZN3,iD3,
                        iXN2,iD2,iYN2,iD2,iZN2,iD2);
                }
                else
                {
                    // +-+0
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iX3,1,iY3,1,iZ3,1);
                }
            }
            else if ( iF2 < 0 )
            {
                iD0 = iF1 - iF0;
                iXN0 = iF1*iX0 - iF0*iX1;
                iYN0 = iF1*iY0 - iF0*iY1;
                iZN0 = iF1*iZ0 - iF0*iZ1;
                iD1 = iF2 - iF0;
                iXN1 = iF2*iX0 - iF0*iX2;
                iYN1 = iF2*iY0 - iF0*iY2;
                iZN1 = iF2*iZ0 - iF0*iZ2;

                if ( iF3 > 0 )
                {
                    // +--+
                    iD2 = iF1 - iF3;
                    iXN2 = iF1*iX3 - iF3*iX1;
                    iYN2 = iF1*iY3 - iF3*iY1;
                    iZN2 = iF1*iZ3 - iF3*iZ1;
                    iD3 = iF2 - iF3;
                    iXN3 = iF2*iX3 - iF3*iX2;
                    iYN3 = iF2*iY3 - iF3*iY2;
                    iZN3 = iF2*iZ3 - iF3*iZ2;
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iXN2,iD2,iYN2,iD2,iZN2,iD2);
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iXN3,iD3,iYN3,iD3,iZN3,iD3,
                        iXN2,iD2,iYN2,iD2,iZN2,iD2);
                }
                else if ( iF3 < 0 )
                {
                    // +---
                    iD2 = iF3 - iF0;
                    iXN2 = iF3*iX0 - iF0*iX3;
                    iYN2 = iF3*iY0 - iF0*iY3;
                    iZN2 = iF3*iZ0 - iF0*iZ3;
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iXN2,iD2,iYN2,iD2,iZN2,iD2);
                }
                else
                {
                    // +--0
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iX3,1,iY3,1,iZ3,1);
                }
            }
            else
            {
                iD0 = iF1 - iF0;
                iXN0 = iF1*iX0 - iF0*iX1;
                iYN0 = iF1*iY0 - iF0*iY1;
                iZN0 = iF1*iZ0 - iF0*iZ1;

                if ( iF3 > 0 )
                {
                    // +-0+
                    iD1 = iF1 - iF3;
                    iXN1 = iF1*iX3 - iF3*iX1;
                    iYN1 = iF1*iY3 - iF3*iY1;
                    iZN1 = iF1*iZ3 - iF3*iZ1;
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iX2,1,iY2,1,iZ2,1);
                }
                else if ( iF3 < 0 )
                {
                    // +-0-
                    iD1 = iF3 - iF0;
                    iXN1 = iF3*iX0 - iF0*iX3;
                    iYN1 = iF3*iY0 - iF0*iY3;
                    iZN1 = iF3*iZ0 - iF0*iZ3;
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iX2,1,iY2,1,iZ2,1);
                }
                else
                {
                    // +-00
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iX2,1,iY2,1,iZ2,1,
                        iX3,1,iY3,1,iZ3,1);
                }
            }
        }
        else
        {
            if ( iF2 > 0 )
            {
                if ( iF3 > 0 )
                {
                    // +0++
                    AddVertex(rkVM,iX1,1,iY1,1,iZ1,1);
                }
                else if ( iF3 < 0 )
                {
                    // +0+-
                    iD0 = iF0 - iF3;
                    iXN0 = iF0*iX3 - iF3*iX0;
                    iYN0 = iF0*iY3 - iF3*iY0;
                    iZN0 = iF0*iZ3 - iF3*iZ0;
                    iD1 = iF2 - iF3;
                    iXN1 = iF2*iX3 - iF3*iX2;
                    iYN1 = iF2*iY3 - iF3*iY2;
                    iZN1 = iF2*iZ3 - iF3*iZ2;
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iX1,1,iY1,1,iZ1,1);
                }
                else
                {
                    // +0+0
                    AddEdge(rkVM,rkES,iX1,1,iY1,1,iZ1,1,iX3,1,iY3,1,iZ3,1);
                }
            }
            else if ( iF2 < 0 )
            {
                iD0 = iF2 - iF0;
                iXN0 = iF2*iX0 - iF0*iX2;
                iYN0 = iF2*iY0 - iF0*iY2;
                iZN0 = iF2*iZ0 - iF0*iZ2;

                if ( iF3 > 0 )
                {
                    // +0-+
                    iD1 = iF2 - iF3;
                    iXN1 = iF2*iX3 - iF3*iX2;
                    iYN1 = iF2*iY3 - iF3*iY2;
                    iZN1 = iF2*iZ3 - iF3*iZ2;
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iX1,1,iY1,1,iZ1,1);
                }
                else if ( iF3 < 0 )
                {
                    // +0--
                    iD1 = iF0 - iF3;
                    iXN1 = iF0*iX3 - iF3*iX0;
                    iYN1 = iF0*iY3 - iF3*iY0;
                    iZN1 = iF0*iZ3 - iF3*iZ0;
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iXN1,iD1,iYN1,iD1,iZN1,iD1,
                        iX1,1,iY1,1,iZ1,1);
                }
                else
                {
                    // +0-0
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iX1,1,iY1,1,iZ1,1,
                        iX3,1,iY3,1,iZ3,1);
                }
            }
            else
            {
                if ( iF3 > 0 )
                {
                    // +00+
                    AddEdge(rkVM,rkES,iX1,1,iY1,1,iZ1,1,iX2,1,iY2,1,iZ2,1);
                }
                else if ( iF3 < 0 )
                {
                    // +00-
                    iD0 = iF0 - iF3;
                    iXN0 = iF0*iX3 - iF3*iX0;
                    iYN0 = iF0*iY3 - iF3*iY0;
                    iZN0 = iF0*iZ3 - iF3*iZ0;
                    AddTriangle(rkVM,rkES,rkTS,
                        iXN0,iD0,iYN0,iD0,iZN0,iD0,
                        iX1,1,iY1,1,iZ1,1,
                        iX2,1,iY2,1,iZ2,1);
                }
                else
                {
                    // +000
                    AddTriangle(rkVM,rkES,rkTS,
                        iX1,1,iY1,1,iZ1,1,
                        iX2,1,iY2,1,iZ2,1,
                        iX3,1,iY3,1,iZ3,1);
                }
            }
        }
    }
    else if ( iF1 != 0 )
    {
        // convert to case 0+**
        if ( iF1 < 0 )
        {
            iF1 = -iF1;
            iF2 = -iF2;
            iF3 = -iF3;
        }

        if ( iF2 > 0 )
        {
            if ( iF3 > 0 )
            {
                // 0+++
                AddVertex(rkVM,iX0,1,iY0,1,iZ0,1);
            }
            else if ( iF3 < 0 )
            {
                // 0++-
                iD0 = iF2 - iF3;
                iXN0 = iF2*iX3 - iF3*iX2;
                iYN0 = iF2*iY3 - iF3*iY2;
                iZN0 = iF2*iZ3 - iF3*iZ2;
                iD1 = iF1 - iF3;
                iXN1 = iF1*iX3 - iF3*iX1;
                iYN1 = iF1*iY3 - iF3*iY1;
                iZN1 = iF1*iZ3 - iF3*iZ1;
                AddTriangle(rkVM,rkES,rkTS,
                    iXN0,iD0,iYN0,iD0,iZN0,iD0,
                    iXN1,iD1,iYN1,iD1,iZN1,iD1,
                    iX0,1,iY0,1,iZ0,1);
            }
            else
            {
                // 0++0
                AddEdge(rkVM,rkES,iX0,1,iY0,1,iZ0,1,iX3,1,iY3,1,iZ3,1);
            }
        }
        else if ( iF2 < 0 )
        {
            iD0 = iF2 - iF1;
            iXN0 = iF2*iX1 - iF1*iX2;
            iYN0 = iF2*iY1 - iF1*iY2;
            iZN0 = iF2*iZ1 - iF1*iZ2;

            if ( iF3 > 0 )
            {
                // 0+-+
                iD1 = iF2 - iF3;
                iXN1 = iF2*iX3 - iF3*iX2;
                iYN1 = iF2*iY3 - iF3*iY2;
                iZN1 = iF2*iZ3 - iF3*iZ2;
                AddTriangle(rkVM,rkES,rkTS,
                    iXN0,iD0,iYN0,iD0,iZN0,iD0,
                    iXN1,iD1,iYN1,iD1,iZN1,iD1,
                    iX0,1,iY0,1,iZ0,1);
            }
            else if ( iF3 < 0 )
            {
                // 0+--
                iD1 = iF1 - iF3;
                iXN1 = iF1*iX3 - iF3*iX1;
                iYN1 = iF1*iY3 - iF3*iY1;
                iZN1 = iF1*iZ3 - iF3*iZ1;
                AddTriangle(rkVM,rkES,rkTS,
                    iXN0,iD0,iYN0,iD0,iZN0,iD0,
                    iXN1,iD1,iYN1,iD1,iZN1,iD1,
                    iX0,1,iY0,1,iZ0,1);
            }
            else
            {
                // 0+-0
                AddTriangle(rkVM,rkES,rkTS,
                    iXN0,iD0,iYN0,iD0,iZN0,iD0,
                    iX0,1,iY0,1,iZ0,1,
                    iX3,1,iY3,1,iZ3,1);
            }
        }
        else
        {
            if ( iF3 > 0 )
            {
                // 0+0+
                AddEdge(rkVM,rkES,iX0,1,iY0,1,iZ0,1,iX2,1,iY2,1,iZ2,1);
            }
            else if ( iF3 < 0 )
            {
                // 0+0-
                iD0 = iF1 - iF3;
                iXN0 = iF1*iX3 - iF3*iX1;
                iYN0 = iF1*iY3 - iF3*iY1;
                iZN0 = iF1*iZ3 - iF3*iZ1;
                AddTriangle(rkVM,rkES,rkTS,
                    iXN0,iD0,iYN0,iD0,iZN0,iD0,
                    iX0,1,iY0,1,iZ0,1,
                    iX2,1,iY2,1,iZ2,1);
            }
            else
            {
                // 0+00
                AddTriangle(rkVM,rkES,rkTS,
                    iX0,1,iY0,1,iZ0,1,
                    iX2,1,iY2,1,iZ2,1,
                    iX3,1,iY3,1,iZ3,1);
            }
        }
    }
    else if ( iF2 != 0 )
    {
        // convert to case 00+*
        if ( iF2 < 0 )
        {
            iF2 = -iF2;
            iF3 = -iF3;
        }

        if ( iF3 > 0 )
        {
            // 00++
            AddEdge(rkVM,rkES,iX0,1,iY0,1,iZ0,1,iX1,1,iY1,1,iZ1,1);
        }
        else if ( iF3 < 0 )
        {
            // 00+-
            iD0 = iF2 - iF3;
            iXN0 = iF2*iX3 - iF3*iX2;
            iYN0 = iF2*iY3 - iF3*iY2;
            iZN0 = iF2*iZ3 - iF3*iZ2;
            AddTriangle(rkVM,rkES,rkTS,
                iXN0,iD0,iYN0,iD0,iZN0,iD0,
                iX0,1,iY0,1,iZ0,1,
                iX1,1,iY1,1,iZ1,1);
        }
        else
        {
            // 00+0
            AddTriangle(rkVM,rkES,rkTS,
                iX0,1,iY0,1,iZ0,1,
                iX1,1,iY1,1,iZ1,1,
                iX3,1,iY3,1,iZ3,1);
        }
    }
    else if ( iF3 != 0 )
    {
        // cases 000+ or 000-
        AddTriangle(rkVM,rkES,rkTS,
            iX0,1,iY0,1,iZ0,1,
            iX1,1,iY1,1,iZ1,1,
            iX2,1,iY2,1,iZ2,1);
    }
    else
    {
        // case 0000
        AddTriangle(rkVM,rkES,rkTS,
            iX0,1,iY0,1,iZ0,1,
            iX1,1,iY1,1,iZ1,1,
            iX2,1,iY2,1,iZ2,1);
        AddTriangle(rkVM,rkES,rkTS,
            iX0,1,iY0,1,iZ0,1,
            iX1,1,iY1,1,iZ1,1,
            iX3,1,iY3,1,iZ3,1);
        AddTriangle(rkVM,rkES,rkTS,
            iX0,1,iY0,1,iZ0,1,
            iX2,1,iY2,1,iZ2,1,
            iX3,1,iY3,1,iZ3,1);
        AddTriangle(rkVM,rkES,rkTS,
            iX1,1,iY1,1,iZ1,1,
            iX2,1,iY2,1,iZ2,1,
            iX3,1,iY3,1,iZ3,1);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Vertex
//----------------------------------------------------------------------------
ImageInterp3D::Vertex::Vertex (int iXNumer, int iXDenom, int iYNumer,
    int iYDenom, int iZNumer, int iZDenom)
{
    if ( iXDenom > 0 )
    {
        m_iXNumer = iXNumer;
        m_iXDenom = iXDenom;
    }
    else
    {
        m_iXNumer = -iXNumer;
        m_iXDenom = -iXDenom;
    }

    if ( iYDenom > 0 )
    {
        m_iYNumer = iYNumer;
        m_iYDenom = iYDenom;
    }
    else
    {
        m_iYNumer = -iYNumer;
        m_iYDenom = -iYDenom;
    }

    if ( iZDenom > 0 )
    {
        m_iZNumer = iZNumer;
        m_iZDenom = iZDenom;
    }
    else
    {
        m_iZNumer = -iZNumer;
        m_iZDenom = -iZDenom;
    }
}
//----------------------------------------------------------------------------
bool ImageInterp3D::Vertex::operator< (const Vertex& rkVertex) const
{
    // Support for STL maps.  I tried using the equal_to comparison and had
    // two problems.
    //   1.  typedef map<Vertex,int,equal_to<Vertex> > VMap;
    //       The space between the last two '>' symbol *must* occur, otherwise
    //       the compiler complains about a mising ',' (go figure).
    //   2.  The VMap 'find' calls were failing when they should have
    //       succeeded.  I have no idea why.

    unsigned int auiValue0[6] =
    {
        *(unsigned int*)&m_iXNumer,
        *(unsigned int*)&m_iXDenom,
        *(unsigned int*)&m_iYNumer,
        *(unsigned int*)&m_iYDenom,
        *(unsigned int*)&m_iZNumer,
        *(unsigned int*)&m_iZDenom
    };

    unsigned int auiValue1[6] =
    {
        *(unsigned int*)&rkVertex.m_iXNumer,
        *(unsigned int*)&rkVertex.m_iXDenom,
        *(unsigned int*)&rkVertex.m_iYNumer,
        *(unsigned int*)&rkVertex.m_iYDenom,
        *(unsigned int*)&rkVertex.m_iZNumer,
        *(unsigned int*)&rkVertex.m_iZDenom
    };

    for (int i = 0; i < 6; i++)
    {
        if ( auiValue0[i] < auiValue1[i] )
            return true;
        if ( auiValue0[i] > auiValue1[i] )
            return false;
    }

    return false;
}
//----------------------------------------------------------------------------
void ImageInterp3D::Vertex::GetTriple (Vector3& rkMeshVertex)
    const
{
    rkMeshVertex.x = float(m_iXNumer)/float(m_iXDenom);
    rkMeshVertex.y = float(m_iYNumer)/float(m_iYDenom);
    rkMeshVertex.z = float(m_iZNumer)/float(m_iZDenom);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Edge
//----------------------------------------------------------------------------
ImageInterp3D::Edge::Edge (int i0, int i1)
{
    if ( i0 < i1 )
    {
        m_i0 = i0;
        m_i1 = i1;
    }
    else
    {
        m_i0 = i1;
        m_i1 = i0;
    }
}
//----------------------------------------------------------------------------
bool ImageInterp3D::Edge::operator< (const Edge& rkEdge) const
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

//----------------------------------------------------------------------------
// Triangle
//----------------------------------------------------------------------------
ImageInterp3D::Triangle::Triangle (int i0, int i1, int i2)
{
    if ( i0 < i1 )
    {
        if ( i0 < i2 )
        {
            m_i0 = i0;
            m_i1 = i1;
            m_i2 = i2;
        }
        else
        {
            m_i0 = i2;
            m_i1 = i0;
            m_i2 = i1;
        }
    }
    else
    {
        if ( i1 < i2 )
        {
            m_i0 = i1;
            m_i1 = i2;
            m_i2 = i0;
        }
        else
        {
            m_i0 = i2;
            m_i1 = i0;
            m_i2 = i1;
        }
    }
}
//----------------------------------------------------------------------------
bool ImageInterp3D::Triangle::operator< (const Triangle& rkTriangle) const
{
    unsigned int auiValue0[3] =
    {
        *(unsigned int*)&m_i0,
        *(unsigned int*)&m_i1,
        *(unsigned int*)&m_i2
    };

    unsigned int auiValue1[3] =
    {
        *(unsigned int*)&rkTriangle.m_i0,
        *(unsigned int*)&rkTriangle.m_i1,
        *(unsigned int*)&rkTriangle.m_i2
    };

    for (int i = 0; i < 3; i++)
    {
        if ( auiValue0[i] < auiValue1[i] )
            return true;
        if ( auiValue0[i] > auiValue1[i] )
            return false;
    }

    return false;
}
//----------------------------------------------------------------------------


