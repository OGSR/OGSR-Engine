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

#include "MgcDelaunay2D.h"
using namespace Mgc;

Real Delaunay2D::ms_fEpsilon = 0.00001f;
Real Delaunay2D::ms_fRange = 10.0f;
int Delaunay2D::ms_iTSize = 75;

//----------------------------------------------------------------------------
Delaunay2D::Delaunay2D (int iVertexQuantity, Vector2* akVertex)
{
    assert( iVertexQuantity >= 3 && akVertex );
    m_akTriangle = NULL;

    // for edge processing
    typedef int _Index[2];
    _Index* akIndex = NULL;
    int iE;

    m_bOwner = true;

    m_iVertexQuantity = iVertexQuantity;
    m_akVertex = akVertex;

    // Make a copy of the input vertices.  These will be modified.  The
    // extra three slots are required for temporary storage.
    Vector2* akPoint = new Vector2[m_iVertexQuantity+3];
    memcpy(akPoint,akVertex,m_iVertexQuantity*sizeof(Vector2));

    // compute the axis-aligned bounding rectangle of the vertices
    m_fXMin = akPoint[0].x;
    m_fXMax = m_fXMin;
    m_fYMin = akPoint[0].y;
    m_fYMax = m_fYMin;

    int i;
    for (i = 1; i < m_iVertexQuantity; i++)
    {
        Real fValue = akPoint[i].x;
        if ( m_fXMax < fValue )
            m_fXMax = fValue;
        if ( m_fXMin > fValue )
            m_fXMin = fValue;

        fValue = akPoint[i].y;
        if ( m_fYMax < fValue )
            m_fYMax = fValue;
        if ( m_fYMin > fValue )
            m_fYMin = fValue;
    }

    m_fXRange = m_fXMax-m_fXMin;
    m_fYRange = m_fYMax-m_fYMin;

    // need to scale the data later to do a correct triangle count
    Real fMaxRange = ( m_fXRange > m_fYRange ? m_fXRange : m_fYRange );
    Real fMaxRangeSqr = fMaxRange*fMaxRange;

    // Tweak the points by very small random numbers to help avoid
    // cocircularities in the vertices.
    Real fAmplitude = 0.5f*ms_fEpsilon*fMaxRange;
    for (i = 0; i < m_iVertexQuantity; i++) 
    {
        akPoint[i].x += fAmplitude*Math::SymmetricRandom();
        akPoint[i].y += fAmplitude*Math::SymmetricRandom();
    }

    Real aafWork[3][2] =
    {
        { 5.0f*ms_fRange, -ms_fRange },
        { -ms_fRange, 5.0f*ms_fRange },
        { -ms_fRange, -ms_fRange }
    };

    for (i = 0; i < 3; i++)
    {
        akPoint[m_iVertexQuantity+i].x = m_fXMin+m_fXRange*aafWork[i][0];
        akPoint[m_iVertexQuantity+i].y = m_fYMin+m_fYRange*aafWork[i][1];
    }

    int i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i11, aiII[3];
    Real fTmp;

    int iTwoTSize = 2*ms_iTSize;
    int** aaiTmp = new int*[iTwoTSize+1];
    aaiTmp[0] = new int[2*(iTwoTSize+1)];
    for (i0 = 1; i0 < iTwoTSize+1; i0++)
        aaiTmp[i0] = aaiTmp[0] + 2*i0;
    i1 = 2*(m_iVertexQuantity + 2);

    int* aiID = new int[i1];
    for (i0 = 0; i0 < i1; i0++) 
        aiID[i0] = i0; 

    int** aaiA3S = new int*[i1];
    aaiA3S[0] = new int[3*i1];
    for (i0 = 1; i0 < i1; i0++)
        aaiA3S[i0] = aaiA3S[0] + 3*i0;
    aaiA3S[0][0] = m_iVertexQuantity;
    aaiA3S[0][1] = m_iVertexQuantity+1;
    aaiA3S[0][2] = m_iVertexQuantity+2;

    // circumscribed centers and radii
    Real** aafCCR = new Real*[i1];
    aafCCR[0] = new Real[3*i1];
    for (i0 = 1; i0 < i1; i0++)
        aafCCR[i0] = aafCCR[0] + 3*i0;
    aafCCR[0][0] = 0.0f;
    aafCCR[0][1] = 0.0f;
    aafCCR[0][2] = Math::MAX_REAL;

    int iTriangleQuantity = 1;  // number of triangles
    i4 = 1;

    // compute triangulation
    for (i0 = 0; i0 < m_iVertexQuantity; i0++)
    {  
        i1 = i7 = -1;
        i9 = 0;
        for (i11 = 0; i11 < iTriangleQuantity; i11++)
        {
            i1++;
            while ( aaiA3S[i1][0] < 0 ) 
                i1++;
            fTmp = aafCCR[i1][2];
            for (i2 = 0; i2 < 2; i2++)
            {  
                Real fZ = akPoint[i0][i2] - aafCCR[i1][i2];
                fTmp -= fZ*fZ;
                if ( fTmp < 0.0f ) 
                    goto Corner3;
            }
            i9--;
            i4--;
            aiID[i4] = i1;
            for (i2 = 0; i2 < 3; i2++)
            {  
                aiII[0] = 0;
                if ( aiII[0] == i2 )
                    aiII[0]++;
                for (i3 = 1; i3 < 2; i3++)
                {  
                    aiII[i3] = aiII[i3-1] + 1;
                    if ( aiII[i3] == i2 )
                        aiII[i3]++;
                }
                if ( i7 > 1 )
                {  
                    i8 = i7;
                    for (i3 = 0; i3 <= i8; i3++)
                    {  
                        for (i5 = 0; i5 < 2; i5++) 
                            if ( aaiA3S[i1][aiII[i5]] != aaiTmp[i3][i5] ) 
                                goto Corner1;
                        for (i6 = 0; i6 < 2; i6++) 
                            aaiTmp[i3][i6] = aaiTmp[i8][i6];
                        i7--;
                        goto Corner2;
Corner1:;
                    }
                }
                if ( ++i7 > iTwoTSize )
                {
                    // Temporary storage exceeded.  Increase ms_iTSize and
                    // call the constructor again.
                    assert( false );
                    goto ExitDelaunay;
                }
                for (i3 = 0; i3 < 2; i3++) 
                    aaiTmp[i7][i3] = aaiA3S[i1][aiII[i3]];
Corner2:;
            }
            aaiA3S[i1][0] = -1;
Corner3:;
        }

        for (i1 = 0; i1 <= i7; i1++)
        {  
            for (i2 = 0; i2 < 2; i2++)
                for (aafWork[2][i2] = 0, i3 = 0; i3 < 2; i3++)
                {  
                    aafWork[i3][i2] = akPoint[aaiTmp[i1][i2]][i3] -
                        akPoint[i0][i3];
                    aafWork[2][i2] += 0.5f*aafWork[i3][i2]*(
                        akPoint[aaiTmp[i1][i2]][i3] + akPoint[i0][i3]);
                }

            fTmp = aafWork[0][0]*aafWork[1][1] - aafWork[1][0]*aafWork[0][1];
            assert( Math::FAbs(fTmp) > 0.0f );
            fTmp = 1.0f/fTmp;
            aafCCR[aiID[i4]][0] = (aafWork[2][0]*aafWork[1][1] -
                aafWork[2][1]*aafWork[1][0])*fTmp;
            aafCCR[aiID[i4]][1] = (aafWork[0][0]*aafWork[2][1] -
                aafWork[0][1]*aafWork[2][0])*fTmp;

            for (aafCCR[aiID[i4]][2] = 0, i2 = 0; i2 < 2; i2++) 
            {  
                Real fZ = akPoint[i0][i2] - aafCCR[aiID[i4]][i2];
                aafCCR[aiID[i4]][2] += fZ*fZ;
                aaiA3S[aiID[i4]][i2] = aaiTmp[i1][i2];
            }

            aaiA3S[aiID[i4]][2] = i0;
            i4++;
            i9++;
        }
        iTriangleQuantity += i9;
    }

    // count the number of triangles
    m_iTriangleQuantity = 0;
    i0 = -1;
    for (i11 = 0; i11 < iTriangleQuantity; i11++)
    {  
        i0++;
        while ( aaiA3S[i0][0] < 0 ) 
            i0++;
        if ( aaiA3S[i0][0] < m_iVertexQuantity )
        {  
            for (i1 = 0; i1 < 2; i1++)
            {
                for (i2 = 0; i2 < 2; i2++)
                {
                    aafWork[i2][i1] = akPoint[aaiA3S[i0][i1]][i2] -
                        akPoint[aaiA3S[i0][2]][i2];
                }
            }

            fTmp = aafWork[0][0]*aafWork[1][1] - aafWork[0][1]*aafWork[1][0];
            if ( Math::FAbs(fTmp) > ms_fEpsilon*fMaxRangeSqr )
                m_iTriangleQuantity++;
        }
    }

    // create the triangles
    m_akTriangle = new Triangle[m_iTriangleQuantity];
    m_iTriangleQuantity = 0;
    i0 = -1;
    for (i11 = 0; i11 < iTriangleQuantity; i11++)
    {  
        i0++;
        while ( aaiA3S[i0][0] < 0 ) 
            i0++;
        if ( aaiA3S[i0][0] < m_iVertexQuantity )
        {  
            for (i1 = 0; i1 < 2; i1++)
            {
                for (i2 = 0; i2 < 2; i2++)
                {
                    aafWork[i2][i1] = akPoint[aaiA3S[i0][i1]][i2] -
                        akPoint[aaiA3S[i0][2]][i2];
                }
            }

            fTmp = aafWork[0][0]*aafWork[1][1] - aafWork[0][1]*aafWork[1][0];
            if ( Math::FAbs(fTmp) > ms_fEpsilon*fMaxRangeSqr )
            {  
                int iDelta = (fTmp < 0.0f ? 1 : 0);
                Triangle& rkTri = m_akTriangle[m_iTriangleQuantity];
                rkTri.m_aiVertex[0] = aaiA3S[i0][0];
                rkTri.m_aiVertex[1] = aaiA3S[i0][1+iDelta];
                rkTri.m_aiVertex[2] = aaiA3S[i0][2-iDelta];
                rkTri.m_aiAdjacent[0] = -1;
                rkTri.m_aiAdjacent[1] = -1;
                rkTri.m_aiAdjacent[2] = -1;
                m_iTriangleQuantity++;
            }
        }
    }

    // build edge table
    m_iEdgeQuantity = 0;
    m_akEdge = new Edge[3*m_iTriangleQuantity];
    akIndex = new _Index[3*m_iTriangleQuantity];

    int j, j0, j1;
    for (i = 0; i < m_iTriangleQuantity; i++)
    {
        Triangle& rkTri = m_akTriangle[i];

        for (j0 = 0, j1 = 1; j0 < 3; j0++, j1 = (j1+1)%3)
        {
            for (j = 0; j < m_iEdgeQuantity; j++)
            {
                Edge& rkEdge = m_akEdge[j];
                if ( (rkTri.m_aiVertex[j0] == rkEdge.m_aiVertex[0] 
                   && rkTri.m_aiVertex[j1] == rkEdge.m_aiVertex[1])
                ||   (rkTri.m_aiVertex[j0] == rkEdge.m_aiVertex[1] 
                   && rkTri.m_aiVertex[j1] == rkEdge.m_aiVertex[0]) )
                {
                    break;
                }
            }
            if ( j == m_iEdgeQuantity )  // add edge to table
            {
                m_akEdge[j].m_aiVertex[0] = rkTri.m_aiVertex[j0];
                m_akEdge[j].m_aiVertex[1] = rkTri.m_aiVertex[j1];
                m_akEdge[j].m_aiTriangle[0] = i;
                akIndex[j][0] = j0;
                m_akEdge[j].m_aiTriangle[1] = -1;
                m_iEdgeQuantity++;
            }
            else  // edge already exists, add triangle to table
            {
                m_akEdge[j].m_aiTriangle[1] = i;
                akIndex[j][1] = j0;
            }
        }
    }

    // count boundary edges (the convex hull of the points)
    // and establish links between adjacent triangles
    m_iExtraTriangleQuantity = 0;
    for (i = 0; i < m_iEdgeQuantity; i++)
    {
        if ( m_akEdge[i].m_aiTriangle[1] != -1 )
        {
            j0 = m_akEdge[i].m_aiTriangle[0];
            j1 = m_akEdge[i].m_aiTriangle[1];
            m_akTriangle[j0].m_aiAdjacent[akIndex[i][0]] = j1;
            m_akTriangle[j1].m_aiAdjacent[akIndex[i][1]] = j0;
        }
        else
        {
            m_iExtraTriangleQuantity++;
        }
    }
    delete[] akIndex;

    // set up extra triangle data
    m_akExtraTriangle = new Triangle[m_iExtraTriangleQuantity];
    for (i = 0, iE = 0; i < m_iTriangleQuantity; i++)
    {
        Triangle& rkTri = m_akTriangle[i];
        for (int j = 0; j < 3; j++)
        {
            if ( rkTri.m_aiAdjacent[j] == -1 )
            {
                Triangle& rkETri = m_akExtraTriangle[iE];
                int j1 = (j+1) % 3, j2 = (j+2) % 3;
                int iS0 = rkTri.m_aiVertex[j];
                int iS1 = rkTri.m_aiVertex[j1];
                rkETri.m_aiVertex[j] = iS0;
                rkETri.m_aiVertex[j1] = iS1;
                rkETri.m_aiVertex[j2] = -1;
                rkTri.m_aiAdjacent[j] = iE + m_iTriangleQuantity;
                iE++;
            }
        }
    }

ExitDelaunay:;
    delete[] aaiTmp[0];
    delete[] aaiTmp;
    delete[] aiID;
    delete[] aaiA3S[0];
    delete[] aaiA3S;
    delete[] aafCCR[0];
    delete[] aafCCR;
    delete[] akPoint;
}
//----------------------------------------------------------------------------
Delaunay2D::Delaunay2D (Delaunay2D& rkNet)
{
    m_bOwner = false;

    m_iVertexQuantity = rkNet.m_iVertexQuantity;
    m_akVertex = rkNet.m_akVertex;
    m_fXMin = rkNet.m_fXMin;
    m_fXMax = rkNet.m_fXMax;
    m_fXRange = rkNet.m_fXRange;
    m_fYMin = rkNet.m_fYMin;
    m_fYMax = rkNet.m_fYMax;
    m_fYRange = rkNet.m_fYRange;
    m_iEdgeQuantity = rkNet.m_iEdgeQuantity;
    m_akEdge = rkNet.m_akEdge;
    m_iTriangleQuantity = rkNet.m_iTriangleQuantity;
    m_akTriangle = rkNet.m_akTriangle;
    m_iExtraTriangleQuantity = rkNet.m_iExtraTriangleQuantity;
    m_akExtraTriangle = rkNet.m_akExtraTriangle;
}
//----------------------------------------------------------------------------
Delaunay2D::~Delaunay2D ()
{
    if ( m_bOwner )
    {
        delete[] m_akVertex;
        delete[] m_akEdge;
        delete[] m_akTriangle;
        delete[] m_akExtraTriangle;
    }
}
//----------------------------------------------------------------------------
void Delaunay2D::ComputeBarycenter (const Vector2& rkV0, const Vector2& rkV1,
    const Vector2& rkV2, const Vector2& rkP, Real afBary[3])
{
    Vector2 kV02 = rkV0 - rkV2;
    Vector2 kV12 = rkV1 - rkV2;
    Vector2 kPV2 = rkP - rkV2;

    Real fM00 = kV02.Dot(kV02);
    Real fM01 = kV02.Dot(kV12);
    Real fM11 = kV12.Dot(kV12);
    Real fR0 = kV02.Dot(kPV2);
    Real fR1 = kV12.Dot(kPV2);
    Real fDet = fM00*fM11 - fM01*fM01;
    assert( Math::FAbs(fDet) > 0.0f );
    Real fInvDet = 1.0f/fDet;

    afBary[0] = (fM11*fR0 - fM01*fR1)*fInvDet;
    afBary[1] = (fM00*fR1 - fM01*fR0)*fInvDet;
    afBary[2] = 1.0f - afBary[0] - afBary[1];
}
//----------------------------------------------------------------------------
bool Delaunay2D::InTriangle (const Vector2& rkV0, const Vector2& rkV1,
    const Vector2& rkV2, const Vector2& rkTest)
{
    const Real fEpsilon = 1e-06f;

    // test against normal to first edge
    Vector2 kDir = rkTest - rkV0;
    Vector2 kNor(rkV0.y - rkV1.y, rkV1.x - rkV0.x);
    if ( kDir.Dot(kNor) < -fEpsilon )
        return false;

    // test against normal to second edge
    kDir = rkTest - rkV1;
    kNor = Vector2(rkV1.y - rkV2.y, rkV2.x - rkV1.x);
    if ( kDir.Dot(kNor) < -fEpsilon )
        return false;

    // test against normal to third edge
    kDir = rkTest - rkV2;
    kNor = Vector2(rkV2.y - rkV0.y, rkV0.x - rkV2.x);
    if ( kDir.Dot(kNor) < -fEpsilon )
        return false;

    return true;
}
//----------------------------------------------------------------------------
void Delaunay2D::ComputeInscribedCenter (const Vector2& rkV0,
    const Vector2& rkV1, const Vector2& rkV2, Vector2& rkCenter)
{
    Vector2 kD10 = rkV1 - rkV0, kD20 = rkV2 - rkV0, kD21 = rkV2 - rkV1;
    Real fL10 = kD10.Length(), fL20 = kD20.Length(), fL21 = kD21.Length();
    Real fPerimeter = fL10 + fL20 + fL21;
    if ( fPerimeter > 0.0f )
    {
        Real fInv = 1.0f/fPerimeter;
        fL10 *= fInv;
        fL20 *= fInv;
        fL21 *= fInv;
    }

    rkCenter = fL21*rkV0 + fL20*rkV1 + fL10*rkV2;
}
//----------------------------------------------------------------------------

