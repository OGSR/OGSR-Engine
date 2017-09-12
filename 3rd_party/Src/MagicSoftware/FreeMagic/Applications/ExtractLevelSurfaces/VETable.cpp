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

#include "VETable.h"
#include <cassert>

//----------------------------------------------------------------------------
VETable::VETable ()
{
}
//----------------------------------------------------------------------------
bool VETable::IsValidVertex (int i) const
{
    assert( 0 <= i && i < 18 );
    return m_akVertex[i].m_bValid;
}
//----------------------------------------------------------------------------
float VETable::GetX (int i) const
{
    assert( 0 <= i && i < 18 );
    return m_akVertex[i].m_fX;
}
//----------------------------------------------------------------------------
float VETable::GetY (int i) const
{
    assert( 0 <= i && i < 18 );
    return m_akVertex[i].m_fY;
}
//----------------------------------------------------------------------------
float VETable::GetZ (int i) const
{
    assert( 0 <= i && i < 18 );
    return m_akVertex[i].m_fZ;
}
//----------------------------------------------------------------------------
void VETable::Insert (int i, float fX, float fY, float fZ)
{
    assert( 0 <= i && i < 18 );
    Vertex& rkV = m_akVertex[i];
    rkV.m_fX = fX;
    rkV.m_fY = fY;
    rkV.m_fZ = fZ;
    rkV.m_bValid = true;
}
//----------------------------------------------------------------------------
void VETable::Insert (int i0, int i1)
{
    assert( 0 <= i0 && i0 < 18 && 0 <= i1 && i1 < 18 );
    Vertex& rkV0 = m_akVertex[i0];
    Vertex& rkV1 = m_akVertex[i1];

    assert( rkV0.m_iAdjQuantity < 4 && rkV1.m_iAdjQuantity < 4 );
    rkV0.m_aiAdj[rkV0.m_iAdjQuantity++] = i1;
    rkV1.m_aiAdj[rkV1.m_iAdjQuantity++] = i0;
}
//----------------------------------------------------------------------------
void VETable::RemoveVertex (int i)
{
    assert( 0 <= i && i < 18 );
    Vertex& rkV0 = m_akVertex[i];

    assert( rkV0.m_iAdjQuantity == 2 );

    int iA0 = rkV0.m_aiAdj[0], iA1 = rkV0.m_aiAdj[1];
    Vertex& rkVA0 = m_akVertex[iA0];
    Vertex& rkVA1 = m_akVertex[iA1];

    int j;
    for (j = 0; j < rkVA0.m_iAdjQuantity; j++)
    {
        if ( rkVA0.m_aiAdj[j] == i )
        {
            rkVA0.m_aiAdj[j] = iA1;
            break;
        }
    }
    assert( j != rkVA0.m_iAdjQuantity );

    for (j = 0; j < rkVA1.m_iAdjQuantity; j++)
    {
        if ( rkVA1.m_aiAdj[j] == i )
        {
            rkVA1.m_aiAdj[j] = iA0;
            break;
        }
    }
    assert( j != rkVA1.m_iAdjQuantity );

    rkV0.m_bValid = false;

    if ( rkVA0.m_iAdjQuantity == 2 )
    {
        if ( rkVA0.m_aiAdj[0] == rkVA0.m_aiAdj[1] )
            rkVA0.m_bValid = false;
    }

    if ( rkVA1.m_iAdjQuantity == 2 )
    {
        if ( rkVA1.m_aiAdj[0] == rkVA1.m_aiAdj[1] )
            rkVA1.m_bValid = false;
    }
}
//----------------------------------------------------------------------------
bool VETable::Remove (Triangle3& rkTri)
{
    for (int i = 0; i < 18; i++)
    {
        Vertex& rkV = m_akVertex[i];
        if ( rkV.m_bValid && rkV.m_iAdjQuantity == 2 )
        {
            rkTri.i0 = i;
            rkTri.i1 = rkV.m_aiAdj[0];
            rkTri.i2 = rkV.m_aiAdj[1];
            RemoveVertex(i);
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
void VETable::RemoveTriangles (V3Array& rkVA, T3Array& rkTA)
{
    // ear-clip the wireframe to get the triangles
    Triangle3 kTri;
    while ( Remove(kTri) )
    {
        int iV0 = (int)rkVA.size(), iV1 = iV0+1, iV2 = iV1+1;
        rkTA.push_back(Triangle3(iV0,iV1,iV2));

        const Vertex& rkV0 = m_akVertex[kTri.i0];
        const Vertex& rkV1 = m_akVertex[kTri.i1];
        const Vertex& rkV2 = m_akVertex[kTri.i2];

        rkVA.push_back(Vertex3(rkV0.m_fX,rkV0.m_fY,rkV0.m_fZ));
        rkVA.push_back(Vertex3(rkV1.m_fX,rkV1.m_fY,rkV1.m_fZ));
        rkVA.push_back(Vertex3(rkV2.m_fX,rkV2.m_fY,rkV2.m_fZ));
    }
}
//----------------------------------------------------------------------------
