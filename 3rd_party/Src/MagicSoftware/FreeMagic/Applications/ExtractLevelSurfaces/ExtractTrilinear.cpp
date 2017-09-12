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

#include "ExtractTrilinear.h"
#include <cassert>
#include <cmath>
using namespace std;

//----------------------------------------------------------------------------
ExtractTrilinear::ExtractTrilinear(int iXBound, int iYBound, int iZBound,
    int* aiData)
{
    assert( iXBound > 0 && iYBound > 0 && iZBound > 0 && aiData );
    m_iXBound = iXBound;
    m_iYBound = iYBound;
    m_iZBound = iZBound;
    m_iXYBound = iXBound*iYBound;
    m_aiData = aiData;
}
//----------------------------------------------------------------------------
void ExtractTrilinear::ExtractContour (float fLevel, V3Array& rkVA,
    T3Array& rkTA)
{
    rkVA.clear();
    rkTA.clear();

    for (int iZ = 0; iZ < m_iZBound-1; iZ++)
    {
        for (int iY = 0; iY < m_iYBound-1; iY++)
        {
            for (int iX = 0; iX < m_iXBound-1; iX++)
            {
                // get vertices on edges of box (if any)
                VETable kTable;
                int iType = GetVertices(fLevel,iX,iY,iZ,kTable);
                if ( iType != 0 )
                {
                    // get edges on faces of box
                    GetXMinEdges(iX,iY,iZ,iType,kTable);
                    GetXMaxEdges(iX,iY,iZ,iType,kTable);
                    GetYMinEdges(iX,iY,iZ,iType,kTable);
                    GetYMaxEdges(iX,iY,iZ,iType,kTable);
                    GetZMinEdges(iX,iY,iZ,iType,kTable);
                    GetZMaxEdges(iX,iY,iZ,iType,kTable);

                    // ear-clip the wireframe mesh
                    kTable.RemoveTriangles(rkVA,rkTA);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void ExtractTrilinear::MakeUnique (V3Array& rkVA, T3Array& rkTA)
{
    int iVQuantity = (int)rkVA.size(), iTQuantity = (int)rkTA.size();
    if ( iVQuantity == 0 || iTQuantity == 0 )
        return;

    // use a hash table to generate unique storage
    V3Map kVMap;
    V3MapIterator pkVIter;
    for (int iV = 0, iNextVertex = 0; iV < iVQuantity; iV++)
    {
        // keep only unique vertices
        pair<V3MapIterator,bool> kResult = kVMap.insert(
            make_pair(rkVA[iV],iNextVertex));
        if ( kResult.second == true )
            iNextVertex++;
    }

    // use a hash table to generate unique storage
    T3Map kTMap;
    T3MapIterator pkTIter;
    for (int iT = 0, iNextTriangle = 0; iT < iTQuantity; iT++)
    {
        // replace old vertex indices by new ones
        Triangle3& rkTri = rkTA[iT];
        pkVIter = kVMap.find(rkVA[rkTri.i0]);
        assert( pkVIter != kVMap.end() );
        rkTri.i0 = pkVIter->second;
        pkVIter = kVMap.find(rkVA[rkTri.i1]);
        assert( pkVIter != kVMap.end() );
        rkTri.i1 = pkVIter->second;
        pkVIter = kVMap.find(rkVA[rkTri.i2]);
        assert( pkVIter != kVMap.end() );
        rkTri.i2 = pkVIter->second;

        // keep only unique triangles
        pair<T3MapIterator,bool> kResult = kTMap.insert(
            make_pair(rkTri,iNextTriangle));
        if ( kResult.second == true )
            iNextTriangle++;
    }

    // pack the vertices
    rkVA.resize(kVMap.size());
    for (pkVIter = kVMap.begin(); pkVIter != kVMap.end(); pkVIter++)
        rkVA[pkVIter->second] = pkVIter->first;

    // pack the triangles
    rkTA.resize(kTMap.size());
    for (pkTIter = kTMap.begin(); pkTIter != kTMap.end(); pkTIter++)
        rkTA[pkTIter->second] = pkTIter->first;
}
//----------------------------------------------------------------------------
void ExtractTrilinear::OrientTriangles (V3Array& rkVA, T3Array& rkTA,
    bool bSameDir)
{
    const float fOneThird = 1.0f/3.0f;
    for (int i = 0; i < (int)rkTA.size(); i++)
    {
        Triangle3& rkTri = rkTA[i];

        // get triangle vertices
        Vertex3 kV0 = rkVA[rkTri.i0];
        Vertex3 kV1 = rkVA[rkTri.i1];
        Vertex3 kV2 = rkVA[rkTri.i2];
        
        // construct triangle normal based on current orientation
        float fEX1 = kV1.x - kV0.x;
        float fEY1 = kV1.y - kV0.y;
        float fEZ1 = kV1.z - kV0.z;
        float fEX2 = kV2.x - kV0.x;
        float fEY2 = kV2.y - kV0.y;
        float fEZ2 = kV2.z - kV0.z;
        float fNX = fEY1*fEZ2 - fEY2*fEZ1;
        float fNY = fEZ1*fEX2 - fEZ2*fEX1;
        float fNZ = fEX1*fEY2 - fEX2*fEY1;

        // get the image gradient at the vertices
        float fGX0, fGY0, fGZ0, fGX1, fGY1, fGZ1, fGX2, fGY2, fGZ2;
        GetGradient(kV0.x,kV0.y,kV0.z,fGX0,fGY0,fGZ0);
        GetGradient(kV1.x,kV1.y,kV1.z,fGX1,fGY1,fGZ1);
        GetGradient(kV2.x,kV2.y,kV2.z,fGX2,fGY2,fGZ2);

        // compute the average gradient
        float fGX = (fGX0+fGX1+fGX2)*fOneThird;
        float fGY = (fGY0+fGY1+fGY2)*fOneThird;
        float fGZ = (fGZ0+fGZ1+fGZ2)*fOneThird;
        
        // compute the dot product of normal and average gradient
        float fDot = fGX*fNX + fGY*fNY + fGZ*fNZ;

        // choose triangle orientation based on gradient direction
        int iSave;
        if ( bSameDir )
        {
            if ( fDot < 0.0f )
            {
                // wrong orientation, reorder it
                iSave = rkTri.i1;
                rkTri.i1 = rkTri.i2;
                rkTri.i2 = iSave;
            }
        }
        else
        {
            if ( fDot > 0.0f )
            {
                // wrong orientation, reorder it
                iSave = rkTri.i1;
                rkTri.i1 = rkTri.i2;
                rkTri.i2 = iSave;
            }
        }
    }
}
//----------------------------------------------------------------------------
void ExtractTrilinear::ComputeNormals (const V3Array& rkVA,
    const T3Array& rkTA, V3Array& rkNA)
{
    // maintain a running sum of triangle normals at each vertex
    int iVQuantity = (int)rkVA.size(), iTQuantity = (int)rkTA.size();
    rkNA.resize(iVQuantity);
    int i, j;
    for (i = 0; i < iVQuantity; i++)
    {
        rkNA[i].x = 0.0f;
        rkNA[i].y = 0.0f;
        rkNA[i].z = 0.0f;
    }

    for (i = 0, j = 0; i < iTQuantity; i++)
    {
        const Triangle3& rkT = rkTA[i];
        Vertex3 kV0 = rkVA[rkT.i0];
        Vertex3 kV1 = rkVA[rkT.i1];
        Vertex3 kV2 = rkVA[rkT.i2];

        // construct triangle normal
        float fEX1 = kV1.x - kV0.x;
        float fEY1 = kV1.y - kV0.y;
        float fEZ1 = kV1.z - kV0.z;
        float fEX2 = kV2.x - kV0.x;
        float fEY2 = kV2.y - kV0.y;
        float fEZ2 = kV2.z - kV0.z;
        float fNX = fEY1*fEZ2 - fEY2*fEZ1;
        float fNY = fEZ1*fEX2 - fEZ2*fEX1;
        float fNZ = fEX1*fEY2 - fEX2*fEY1;

        // maintain the sum of normals at each vertex
        rkNA[rkT.i0].x += fNX;
        rkNA[rkT.i0].y += fNY;
        rkNA[rkT.i0].z += fNZ;
        rkNA[rkT.i1].x += fNX;
        rkNA[rkT.i1].y += fNY;
        rkNA[rkT.i1].z += fNZ;
        rkNA[rkT.i2].x += fNX;
        rkNA[rkT.i2].y += fNY;
        rkNA[rkT.i2].z += fNZ;
    }

    // The normal vector storage was used to accumulate the sum of
    // triangle normals.  Now these vectors must be rescaled to be
    // unit length.
    for (i = 0; i < iVQuantity; i++)
    {
        Vertex3& rkV = rkNA[i];
        float fLength = sqrtf(rkV.x*rkV.x + rkV.y*rkV.y + rkV.z*rkV.z);
        if ( fLength > 1e-08f )
        {
            float fInvLength = 1.0f/fLength;
            rkV.x *= fInvLength;
            rkV.y *= fInvLength;
            rkV.z *= fInvLength;
        }
        else
        {
            rkV.x = 0.0f;
            rkV.y = 0.0f;
            rkV.z = 0.0f;
        }
    }
}
//----------------------------------------------------------------------------
int ExtractTrilinear::GetVertices (float fLevel, int iX, int iY, int iZ,
    VETable& rkTable)
{
    int iType = 0;

    // get image values at corners of voxel
    int i000 = iX + m_iXBound*(iY + m_iYBound*iZ);
    int i100 = i000 + 1;
    int i010 = i000 + m_iXBound;
    int i110 = i010 + 1;
    int i001 = i000 + m_iXYBound;
    int i101 = i001 + 1;
    int i011 = i001 + m_iXBound;
    int i111 = i011 + 1;
    float fF000 = (float)m_aiData[i000];
    float fF100 = (float)m_aiData[i100];
    float fF010 = (float)m_aiData[i010];
    float fF110 = (float)m_aiData[i110];
    float fF001 = (float)m_aiData[i001];
    float fF101 = (float)m_aiData[i101];
    float fF011 = (float)m_aiData[i011];
    float fF111 = (float)m_aiData[i111];

    float fX0 = (float)iX, fY0 = (float)iY, fZ0 = (float)iZ;
    float fX1 = fX0+1.0f, fY1 = fY0+1.0f, fZ1 = fZ0+1.0f;

    // xmin-ymin edge
    float fDiff0 = fLevel - fF000;
    float fDiff1 = fLevel - fF001;
    if ( fDiff0*fDiff1 < 0.0f )
    {
        iType |= EB_XMIN_YMIN;
        rkTable.Insert(EI_XMIN_YMIN,fX0,fY0,fZ0+fDiff0/(fF001-fF000));
    }

    // xmin-ymax edge
    fDiff0 = fLevel - fF010;
    fDiff1 = fLevel - fF011;
    if ( fDiff0*fDiff1 < 0.0f )
    {
        iType |= EB_XMIN_YMAX;
        rkTable.Insert(EI_XMIN_YMAX,fX0,fY1,fZ0+fDiff0/(fF011-fF010));
    }

    // xmax-ymin edge
    fDiff0 = fLevel - fF100;
    fDiff1 = fLevel - fF101;
    if ( fDiff0*fDiff1 < 0.0f )
    {
        iType |= EB_XMAX_YMIN;
        rkTable.Insert(EI_XMAX_YMIN,fX1,fY0,fZ0+fDiff0/(fF101-fF100));
    }

    // xmax-ymax edge
    fDiff0 = fLevel - fF110;
    fDiff1 = fLevel - fF111;
    if ( fDiff0*fDiff1 < 0.0f )
    {
        iType |= EB_XMAX_YMAX;
        rkTable.Insert(EI_XMAX_YMAX,fX1,fY1,fZ0+fDiff0/(fF111-fF110));
    }

    // xmin-zmin edge
    fDiff0 = fLevel - fF000;
    fDiff1 = fLevel - fF010;
    if ( fDiff0*fDiff1 < 0.0f )
    {
        iType |= EB_XMIN_ZMIN;
        rkTable.Insert(EI_XMIN_ZMIN,fX0,fY0+fDiff0/(fF010-fF000),fZ0);
    }

    // xmin-zmax edge
    fDiff0 = fLevel - fF001;
    fDiff1 = fLevel - fF011;
    if ( fDiff0*fDiff1 < 0.0f )
    {
        iType |= EB_XMIN_ZMAX;
        rkTable.Insert(EI_XMIN_ZMAX,fX0,fY0+fDiff0/(fF011-fF001),fZ1);
    }

    // xmax-zmin edge
    fDiff0 = fLevel - fF100;
    fDiff1 = fLevel - fF110;
    if ( fDiff0*fDiff1 < 0.0f )
    {
        iType |= EB_XMAX_ZMIN;
        rkTable.Insert(EI_XMAX_ZMIN,fX1,fY0+fDiff0/(fF110-fF100),fZ0);
    }

    // xmax-zmax edge
    fDiff0 = fLevel - fF101;
    fDiff1 = fLevel - fF111;
    if ( fDiff0*fDiff1 < 0.0f )
    {
        iType |= EB_XMAX_ZMAX;
        rkTable.Insert(EI_XMAX_ZMAX,fX1,fY0+fDiff0/(fF111-fF101),fZ1);
    }

    // ymin-zmin edge
    fDiff0 = fLevel - fF000;
    fDiff1 = fLevel - fF100;
    if ( fDiff0*fDiff1 < 0.0f )
    {
        iType |= EB_YMIN_ZMIN;
        rkTable.Insert(EI_YMIN_ZMIN,fX0+fDiff0/(fF100-fF000),fY0,fZ0);
    }

    // ymin-zmax edge
    fDiff0 = fLevel - fF001;
    fDiff1 = fLevel - fF101;
    if ( fDiff0*fDiff1 < 0.0f )
    {
        iType |= EB_YMIN_ZMAX;
        rkTable.Insert(EI_YMIN_ZMAX,fX0+fDiff0/(fF101-fF001),fY0,fZ1);
    }

    // ymax-zmin edge
    fDiff0 = fLevel - fF010;
    fDiff1 = fLevel - fF110;
    if ( fDiff0*fDiff1 < 0.0f )
    {
        iType |= EB_YMAX_ZMIN;
        rkTable.Insert(EI_YMAX_ZMIN,fX0+fDiff0/(fF110-fF010),fY1,fZ0);
    }

    // ymax-zmax edge
    fDiff0 = fLevel - fF011;
    fDiff1 = fLevel - fF111;
    if ( fDiff0*fDiff1 < 0.0f )
    {
        iType |= EB_YMAX_ZMAX;
        rkTable.Insert(EI_YMAX_ZMAX,fX0+fDiff0/(fF111-fF011),fY1,fZ1);
    }

    return iType;
}
//----------------------------------------------------------------------------
void ExtractTrilinear::GetXMinEdges (int iX, int iY, int iZ, int iType,
    VETable& rkTable)
{
    int iFaceType = 0;
    if ( iType & EB_XMIN_YMIN ) iFaceType |= 0x01;
    if ( iType & EB_XMIN_YMAX ) iFaceType |= 0x02;
    if ( iType & EB_XMIN_ZMIN ) iFaceType |= 0x04;
    if ( iType & EB_XMIN_ZMAX ) iFaceType |= 0x08;

    switch ( iFaceType )
    {
    case  0: return;
    case  3: rkTable.Insert(EI_XMIN_YMIN,EI_XMIN_YMAX); break;
    case  5: rkTable.Insert(EI_XMIN_YMIN,EI_XMIN_ZMIN); break;
    case  6: rkTable.Insert(EI_XMIN_YMAX,EI_XMIN_ZMIN); break;
    case  9: rkTable.Insert(EI_XMIN_YMIN,EI_XMIN_ZMAX); break;
    case 10: rkTable.Insert(EI_XMIN_YMAX,EI_XMIN_ZMAX); break;
    case 12: rkTable.Insert(EI_XMIN_ZMIN,EI_XMIN_ZMAX); break;
    case 15:
    {
        // four vertices, one per edge, need to disambiguate
        int i = iX + m_iXBound*(iY + m_iYBound*iZ);
        int iF00 = m_aiData[i];  // F(x,y,z)
        i += m_iXBound;
        int iF10 = m_aiData[i];  // F(x,y+1,z)
        i += m_iXYBound;
        int iF11 = m_aiData[i];  // F(x,y+1,z+1)
        i -= m_iXBound;
        int iF01 = m_aiData[i];  // F(x,y,z+1)
        int iDet = iF00*iF11 - iF01*iF10;

        if ( iDet > 0 )
        {
            // disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>
            rkTable.Insert(EI_XMIN_YMIN,EI_XMIN_ZMIN);
            rkTable.Insert(EI_XMIN_YMAX,EI_XMIN_ZMAX);
        }
        else if ( iDet < 0 )
        {
            // disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>
            rkTable.Insert(EI_XMIN_YMIN,EI_XMIN_ZMAX);
            rkTable.Insert(EI_XMIN_YMAX,EI_XMIN_ZMIN);
        }
        else
        {
            // plus-sign configuration, add branch point to tessellation
            rkTable.Insert(FI_XMIN,rkTable.GetX(EI_XMIN_ZMIN),
                rkTable.GetY(EI_XMIN_ZMIN),rkTable.GetZ(EI_XMIN_YMIN));

            // add edges sharing the branch point
            rkTable.Insert(EI_XMIN_YMIN,FI_XMIN);
            rkTable.Insert(EI_XMIN_YMAX,FI_XMIN);
            rkTable.Insert(EI_XMIN_ZMIN,FI_XMIN);
            rkTable.Insert(EI_XMIN_ZMAX,FI_XMIN);
        }
        break;
    }
    default:  assert( false );
    }
}
//----------------------------------------------------------------------------
void ExtractTrilinear::GetXMaxEdges (int iX, int iY, int iZ, int iType,
    VETable& rkTable)
{
    int iFaceType = 0;
    if ( iType & EB_XMAX_YMIN ) iFaceType |= 0x01;
    if ( iType & EB_XMAX_YMAX ) iFaceType |= 0x02;
    if ( iType & EB_XMAX_ZMIN ) iFaceType |= 0x04;
    if ( iType & EB_XMAX_ZMAX ) iFaceType |= 0x08;

    switch ( iFaceType )
    {
    case  0: return;
    case  3: rkTable.Insert(EI_XMAX_YMIN,EI_XMAX_YMAX); break;
    case  5: rkTable.Insert(EI_XMAX_YMIN,EI_XMAX_ZMIN); break;
    case  6: rkTable.Insert(EI_XMAX_YMAX,EI_XMAX_ZMIN); break;
    case  9: rkTable.Insert(EI_XMAX_YMIN,EI_XMAX_ZMAX); break;
    case 10: rkTable.Insert(EI_XMAX_YMAX,EI_XMAX_ZMAX); break;
    case 12: rkTable.Insert(EI_XMAX_ZMIN,EI_XMAX_ZMAX); break;
    case 15:
    {
        // four vertices, one per edge, need to disambiguate
        int i = (iX+1) + m_iXBound*(iY + m_iYBound*iZ);
        int iF00 = m_aiData[i];  // F(x,y,z)
        i += m_iXBound;
        int iF10 = m_aiData[i];  // F(x,y+1,z)
        i += m_iXYBound;
        int iF11 = m_aiData[i];  // F(x,y+1,z+1)
        i -= m_iXBound;
        int iF01 = m_aiData[i];  // F(x,y,z+1)
        int iDet = iF00*iF11 - iF01*iF10;

        if ( iDet > 0 )
        {
            // disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>
            rkTable.Insert(EI_XMAX_YMIN,EI_XMAX_ZMIN);
            rkTable.Insert(EI_XMAX_YMAX,EI_XMAX_ZMAX);
        }
        else if ( iDet < 0 )
        {
            // disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>
            rkTable.Insert(EI_XMAX_YMIN,EI_XMAX_ZMAX);
            rkTable.Insert(EI_XMAX_YMAX,EI_XMAX_ZMIN);
        }
        else
        {
            // plus-sign configuration, add branch point to tessellation
            rkTable.Insert(FI_XMAX,rkTable.GetX(EI_XMAX_ZMIN),
                rkTable.GetY(EI_XMAX_ZMIN),rkTable.GetZ(EI_XMAX_YMIN));

            // add edges sharing the branch point
            rkTable.Insert(EI_XMAX_YMIN,FI_XMAX);
            rkTable.Insert(EI_XMAX_YMAX,FI_XMAX);
            rkTable.Insert(EI_XMAX_ZMIN,FI_XMAX);
            rkTable.Insert(EI_XMAX_ZMAX,FI_XMAX);
        }
        break;
    }
    default:  assert( false );
    }
}
//----------------------------------------------------------------------------
void ExtractTrilinear::GetYMinEdges (int iX, int iY, int iZ, int iType,
    VETable& rkTable)
{
    int iFaceType = 0;
    if ( iType & EB_XMIN_YMIN ) iFaceType |= 0x01;
    if ( iType & EB_XMAX_YMIN ) iFaceType |= 0x02;
    if ( iType & EB_YMIN_ZMIN ) iFaceType |= 0x04;
    if ( iType & EB_YMIN_ZMAX ) iFaceType |= 0x08;

    switch ( iFaceType )
    {
    case  0: return;
    case  3: rkTable.Insert(EI_XMIN_YMIN,EI_XMAX_YMIN); break;
    case  5: rkTable.Insert(EI_XMIN_YMIN,EI_YMIN_ZMIN); break;
    case  6: rkTable.Insert(EI_XMAX_YMIN,EI_YMIN_ZMIN); break;
    case  9: rkTable.Insert(EI_XMIN_YMIN,EI_YMIN_ZMAX); break;
    case 10: rkTable.Insert(EI_XMAX_YMIN,EI_YMIN_ZMAX); break;
    case 12: rkTable.Insert(EI_YMIN_ZMIN,EI_YMIN_ZMAX); break;
    case 15:
    {
        // four vertices, one per edge, need to disambiguate
        int i = iX + m_iXBound*(iY + m_iYBound*iZ);
        int iF00 = m_aiData[i];  // F(x,y,z)
        i++;
        int iF10 = m_aiData[i];  // F(x+1,y,z)
        i += m_iXYBound;
        int iF11 = m_aiData[i];  // F(x+1,y,z+1)
        i--;
        int iF01 = m_aiData[i];  // F(x,y,z+1)
        int iDet = iF00*iF11 - iF01*iF10;

        if ( iDet > 0 )
        {
            // disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>
            rkTable.Insert(EI_XMIN_YMIN,EI_YMIN_ZMIN);
            rkTable.Insert(EI_XMAX_YMIN,EI_YMIN_ZMAX);
        }
        else if ( iDet < 0 )
        {
            // disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>
            rkTable.Insert(EI_XMIN_YMIN,EI_YMIN_ZMAX);
            rkTable.Insert(EI_XMAX_YMIN,EI_YMIN_ZMIN);
        }
        else
        {
            // plus-sign configuration, add branch point to tessellation
            rkTable.Insert(FI_YMIN,rkTable.GetX(EI_YMIN_ZMIN),
                rkTable.GetY(EI_XMIN_YMIN),rkTable.GetZ(EI_XMIN_YMIN));

            // add edges sharing the branch point
            rkTable.Insert(EI_XMIN_YMIN,FI_YMIN);
            rkTable.Insert(EI_XMAX_YMIN,FI_YMIN);
            rkTable.Insert(EI_YMIN_ZMIN,FI_YMIN);
            rkTable.Insert(EI_YMIN_ZMAX,FI_YMIN);
        }
        break;
    }
    default:  assert( false );
    }
}
//----------------------------------------------------------------------------
void ExtractTrilinear::GetYMaxEdges (int iX, int iY, int iZ, int iType,
    VETable& rkTable)
{
    int iFaceType = 0;
    if ( iType & EB_XMIN_YMAX ) iFaceType |= 0x01;
    if ( iType & EB_XMAX_YMAX ) iFaceType |= 0x02;
    if ( iType & EB_YMAX_ZMIN ) iFaceType |= 0x04;
    if ( iType & EB_YMAX_ZMAX ) iFaceType |= 0x08;

    switch ( iFaceType )
    {
    case  0: return;
    case  3: rkTable.Insert(EI_XMIN_YMAX,EI_XMAX_YMAX); break;
    case  5: rkTable.Insert(EI_XMIN_YMAX,EI_YMAX_ZMIN); break;
    case  6: rkTable.Insert(EI_XMAX_YMAX,EI_YMAX_ZMIN); break;
    case  9: rkTable.Insert(EI_XMIN_YMAX,EI_YMAX_ZMAX); break;
    case 10: rkTable.Insert(EI_XMAX_YMAX,EI_YMAX_ZMAX); break;
    case 12: rkTable.Insert(EI_YMAX_ZMIN,EI_YMAX_ZMAX); break;
    case 15:
    {
        // four vertices, one per edge, need to disambiguate
        int i = iX + m_iXBound*((iY+1) + m_iYBound*iZ);
        int iF00 = m_aiData[i];  // F(x,y,z)
        i++;
        int iF10 = m_aiData[i];  // F(x+1,y,z)
        i += m_iXYBound;
        int iF11 = m_aiData[i];  // F(x+1,y,z+1)
        i--;
        int iF01 = m_aiData[i];  // F(x,y,z+1)
        int iDet = iF00*iF11 - iF01*iF10;

        if ( iDet > 0 )
        {
            // disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>
            rkTable.Insert(EI_XMIN_YMAX,EI_YMAX_ZMIN);
            rkTable.Insert(EI_XMAX_YMAX,EI_YMAX_ZMAX);
        }
        else if ( iDet < 0 )
        {
            // disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>
            rkTable.Insert(EI_XMIN_YMAX,EI_YMAX_ZMAX);
            rkTable.Insert(EI_XMAX_YMAX,EI_YMAX_ZMIN);
        }
        else
        {
            // plus-sign configuration, add branch point to tessellation
            rkTable.Insert(FI_YMAX,rkTable.GetX(EI_YMAX_ZMIN),
                rkTable.GetY(EI_XMIN_YMAX),rkTable.GetZ(EI_XMIN_YMAX));

            // add edges sharing the branch point
            rkTable.Insert(EI_XMIN_YMAX,FI_YMAX);
            rkTable.Insert(EI_XMAX_YMAX,FI_YMAX);
            rkTable.Insert(EI_YMAX_ZMIN,FI_YMAX);
            rkTable.Insert(EI_YMAX_ZMAX,FI_YMAX);
        }
        break;
    }
    default:  assert( false );
    }
}
//----------------------------------------------------------------------------
void ExtractTrilinear::GetZMinEdges (int iX, int iY, int iZ, int iType,
    VETable& rkTable)
{
    int iFaceType = 0;
    if ( iType & EB_XMIN_ZMIN ) iFaceType |= 0x01;
    if ( iType & EB_XMAX_ZMIN ) iFaceType |= 0x02;
    if ( iType & EB_YMIN_ZMIN ) iFaceType |= 0x04;
    if ( iType & EB_YMAX_ZMIN ) iFaceType |= 0x08;

    switch ( iFaceType )
    {
    case  0: return;
    case  3: rkTable.Insert(EI_XMIN_ZMIN,EI_XMAX_ZMIN); break;
    case  5: rkTable.Insert(EI_XMIN_ZMIN,EI_YMIN_ZMIN); break;
    case  6: rkTable.Insert(EI_XMAX_ZMIN,EI_YMIN_ZMIN); break;
    case  9: rkTable.Insert(EI_XMIN_ZMIN,EI_YMAX_ZMIN); break;
    case 10: rkTable.Insert(EI_XMAX_ZMIN,EI_YMAX_ZMIN); break;
    case 12: rkTable.Insert(EI_YMIN_ZMIN,EI_YMAX_ZMIN); break;
    case 15:
    {
        // four vertices, one per edge, need to disambiguate
        int i = iX + m_iXBound*(iY + m_iYBound*iZ);
        int iF00 = m_aiData[i];  // F(x,y,z)
        i++;
        int iF10 = m_aiData[i];  // F(x+1,y,z)
        i += m_iXBound;
        int iF11 = m_aiData[i];  // F(x+1,y+1,z)
        i--;
        int iF01 = m_aiData[i];  // F(x,y+1,z)
        int iDet = iF00*iF11 - iF01*iF10;

        if ( iDet > 0 )
        {
            // disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>
            rkTable.Insert(EI_XMIN_ZMIN,EI_YMIN_ZMIN);
            rkTable.Insert(EI_XMAX_ZMIN,EI_YMAX_ZMIN);
        }
        else if ( iDet < 0 )
        {
            // disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>
            rkTable.Insert(EI_XMIN_ZMIN,EI_YMAX_ZMIN);
            rkTable.Insert(EI_XMAX_ZMIN,EI_YMIN_ZMIN);
        }
        else
        {
            // plus-sign configuration, add branch point to tessellation
            rkTable.Insert(FI_ZMIN,rkTable.GetX(EI_YMIN_ZMIN),
                rkTable.GetY(EI_XMIN_ZMIN),rkTable.GetZ(EI_XMIN_ZMIN));

            // add edges sharing the branch point
            rkTable.Insert(EI_XMIN_ZMIN,FI_ZMIN);
            rkTable.Insert(EI_XMAX_ZMIN,FI_ZMIN);
            rkTable.Insert(EI_YMIN_ZMIN,FI_ZMIN);
            rkTable.Insert(EI_YMAX_ZMIN,FI_ZMIN);
        }
        break;
    }
    default:  assert( false );
    }
}
//----------------------------------------------------------------------------
void ExtractTrilinear::GetZMaxEdges (int iX, int iY, int iZ, int iType,
    VETable& rkTable)
{
    int iFaceType = 0;
    if ( iType & EB_XMIN_ZMAX ) iFaceType |= 0x01;
    if ( iType & EB_XMAX_ZMAX ) iFaceType |= 0x02;
    if ( iType & EB_YMIN_ZMAX ) iFaceType |= 0x04;
    if ( iType & EB_YMAX_ZMAX ) iFaceType |= 0x08;

    switch ( iFaceType )
    {
    case  0: return;
    case  3: rkTable.Insert(EI_XMIN_ZMAX,EI_XMAX_ZMAX); break;
    case  5: rkTable.Insert(EI_XMIN_ZMAX,EI_YMIN_ZMAX); break;
    case  6: rkTable.Insert(EI_XMAX_ZMAX,EI_YMIN_ZMAX); break;
    case  9: rkTable.Insert(EI_XMIN_ZMAX,EI_YMAX_ZMAX); break;
    case 10: rkTable.Insert(EI_XMAX_ZMAX,EI_YMAX_ZMAX); break;
    case 12: rkTable.Insert(EI_YMIN_ZMAX,EI_YMAX_ZMAX); break;
    case 15:
    {
        // four vertices, one per edge, need to disambiguate
        int i = iX + m_iXBound*(iY + m_iYBound*(iZ+1));
        int iF00 = m_aiData[i];  // F(x,y,z)
        i++;
        int iF10 = m_aiData[i];  // F(x+1,y,z)
        i += m_iXBound;
        int iF11 = m_aiData[i];  // F(x+1,y+1,z)
        i--;
        int iF01 = m_aiData[i];  // F(x,y+1,z)
        int iDet = iF00*iF11 - iF01*iF10;

        if ( iDet > 0 )
        {
            // disjoint hyperbolic segments, pair <P0,P2>, <P1,P3>
            rkTable.Insert(EI_XMIN_ZMAX,EI_YMIN_ZMAX);
            rkTable.Insert(EI_XMAX_ZMAX,EI_YMAX_ZMAX);
        }
        else if ( iDet < 0 )
        {
            // disjoint hyperbolic segments, pair <P0,P3>, <P1,P2>
            rkTable.Insert(EI_XMIN_ZMAX,EI_YMAX_ZMAX);
            rkTable.Insert(EI_XMAX_ZMAX,EI_YMIN_ZMAX);
        }
        else
        {
            // plus-sign configuration, add branch point to tessellation
            rkTable.Insert(FI_ZMAX,rkTable.GetX(EI_YMIN_ZMAX),
                rkTable.GetY(EI_XMIN_ZMAX),rkTable.GetZ(EI_XMIN_ZMAX));

            // add edges sharing the branch point
            rkTable.Insert(EI_XMIN_ZMAX,FI_ZMAX);
            rkTable.Insert(EI_XMAX_ZMAX,FI_ZMAX);
            rkTable.Insert(EI_YMIN_ZMAX,FI_ZMAX);
            rkTable.Insert(EI_YMAX_ZMAX,FI_ZMAX);
        }
        break;
    }
    default:  assert( false );
    }
}
//----------------------------------------------------------------------------
void ExtractTrilinear::GetGradient (float fX, float fY, float fZ, float& rfGX,
    float& rfGY, float& rfGZ)
{
    int iX = (int)fX;
    if ( iX < 0 || iX >= m_iXBound-1 )
    {
        rfGX = 0.0f;
        rfGY = 0.0f;
        rfGZ = 0.0f;
        return;
    }

    int iY = (int)fY;
    if ( iY < 0 || iY >= m_iYBound-1 )
    {
        rfGX = 0.0f;
        rfGY = 0.0f;
        rfGZ = 0.0f;
        return;
    }

    int iZ = (int)fZ;
    if ( iZ < 0 || iZ >= m_iZBound-1 )
    {
        rfGX = 0.0f;
        rfGY = 0.0f;
        rfGZ = 0.0f;
        return;
    }

    // get image values at corners of voxel
    int i000 = iX + m_iXBound*(iY + m_iYBound*iZ);
    int i100 = i000 + 1;
    int i010 = i000 + m_iXBound;
    int i110 = i010 + 1;
    int i001 = i000 + m_iXYBound;
    int i101 = i001 + 1;
    int i011 = i001 + m_iXBound;
    int i111 = i011 + 1;
    float fF000 = (float)m_aiData[i000];
    float fF100 = (float)m_aiData[i100];
    float fF010 = (float)m_aiData[i010];
    float fF110 = (float)m_aiData[i110];
    float fF001 = (float)m_aiData[i001];
    float fF101 = (float)m_aiData[i101];
    float fF011 = (float)m_aiData[i011];
    float fF111 = (float)m_aiData[i111];

    fX -= iX;
    fY -= iY;
    fZ -= iZ;
    float fOmX = 1.0f - fX, fOmY = 1.0f - fY, fOmZ = 1.0f - fZ;

    float fTmp0 = fOmY*(fF100-fF000) + fY*(fF110-fF010);
    float fTmp1 = fOmY*(fF101-fF001) + fY*(fF111-fF011);
    rfGX = fOmZ*fTmp0 + fZ*fTmp1;
    
    fTmp0 = fOmX*(fF010-fF000) + fX*(fF110-fF100);
    fTmp1 = fOmX*(fF011-fF001) + fX*(fF111-fF101);
    rfGY = fOmZ*fTmp0 + fZ*fTmp1;
    
    fTmp0 = fOmX*(fF001-fF000) + fX*(fF101-fF100);
    fTmp1 = fOmX*(fF011-fF010) + fX*(fF111-fF110);
    rfGZ = fOmY*fTmp0 + fY*fTmp1;
}
//----------------------------------------------------------------------------
