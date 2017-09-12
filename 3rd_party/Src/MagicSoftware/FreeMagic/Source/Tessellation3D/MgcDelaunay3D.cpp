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

#include "MgcDelaunay3D.h"
using namespace Mgc;

Real Delaunay3D::ms_fEpsilon = 0.00001f;
Real Delaunay3D::ms_fRange = 10.0f;
int Delaunay3D::ms_iTSize = 75;
int Delaunay3D::ms_iQuantityFactor = 16;

//----------------------------------------------------------------------------
Delaunay3D::Delaunay3D (int iVertexQuantity, Vector3* akVertex)
{
    assert( iVertexQuantity >= 4 && akVertex );
    m_akTetrahedron = NULL;

    m_bOwner = true;

    m_iVertexQuantity = iVertexQuantity;
    m_akVertex = akVertex;

    // Make a copy of the input vertices.  These will be modified.  The
    // extra four slots are required for temporary storage.
    Vector3* akPoint = new Vector3[m_iVertexQuantity+4];
    memcpy(akPoint,akVertex,m_iVertexQuantity*sizeof(Vector3));

    // compute the axis-aligned bounding box of the vertices
    m_fXMin = akPoint[0].x;
    m_fXMax = m_fXMin;
    m_fYMin = akPoint[0].y;
    m_fYMax = m_fYMin;
    m_fZMin = akPoint[0].z;
    m_fZMax = m_fZMin;

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

        fValue = akPoint[i].z;
        if ( m_fZMax < fValue )
            m_fZMax = fValue;
        if ( m_fZMin > fValue )
            m_fZMin = fValue;
    }

    m_fXRange = m_fXMax-m_fXMin;
    m_fYRange = m_fYMax-m_fYMin;
    m_fZRange = m_fZMax-m_fZMin;

    // need to scale the data later to do a correct tetrahedron count
    Real fMaxRange = m_fXRange;
    if ( fMaxRange < m_fYRange )
        fMaxRange = m_fYRange;
    if ( fMaxRange < m_fZRange )
        fMaxRange = m_fZRange;
    Real fMaxRangeCube = fMaxRange*fMaxRange*fMaxRange;

    // Tweak the points by very small random numbers to help avoid
    // cosphericities in the vertices.
    Real fAmplitude = 0.5f*ms_fEpsilon*fMaxRange;
    for (i = 0; i < m_iVertexQuantity; i++) 
    {
        akPoint[i].x += fAmplitude*Math::SymmetricRandom();
        akPoint[i].y += fAmplitude*Math::SymmetricRandom();
        akPoint[i].z += fAmplitude*Math::SymmetricRandom();
    }

    Real aafWork[4][3] =
    {
        { 8.0f*ms_fRange, -ms_fRange, -ms_fRange },
        { -ms_fRange, 8.0f*ms_fRange, -ms_fRange },
        { -ms_fRange, -ms_fRange, 8.0f*ms_fRange },
        { -ms_fRange, -ms_fRange, -ms_fRange }
    };

    for (i = 0; i < 4; i++)
    {
        akPoint[m_iVertexQuantity+i].x = m_fXMin+m_fXRange*aafWork[i][0];
        akPoint[m_iVertexQuantity+i].y = m_fYMin+m_fYRange*aafWork[i][1];
        akPoint[m_iVertexQuantity+i].z = m_fZMin+m_fZRange*aafWork[i][2];
    }

    int i0, i1, i2, i3, i4, i5, i6, i7, i8, i9, i11, aiII[3];
    Real fTmp;

    int iSixTSize = 6*ms_iTSize;
    int** aaiTmp = new int*[iSixTSize+1];
    aaiTmp[0] = new int[3*(iSixTSize+1)];
    for (i0 = 1; i0 < iSixTSize+1; i0++)
        aaiTmp[i0] = aaiTmp[0] + 3*i0;

    // Estimate of how many tetrahedrons there can be.  Since theoretically
    // the number is O(N^2), this could be quite large.  You may need to
    // increase the quantity factor if a call to this function fails.
    i1 = ms_iQuantityFactor*m_iVertexQuantity;

    int* aaID = new int[i1];
    for (i0 = 0; i0 < i1; i0++)
        aaID[i0] = i0;

    int** aaiA3S = new int*[i1];
    aaiA3S[0] = new int[4*i1];
    for (i0 = 1; i0 < i1; i0++)
        aaiA3S[i0] = aaiA3S[0] + 4*i0;
    aaiA3S[0][0] = m_iVertexQuantity;
    aaiA3S[0][1] = m_iVertexQuantity+1;
    aaiA3S[0][2] = m_iVertexQuantity+2;
    aaiA3S[0][3] = m_iVertexQuantity+3;

    // circumscribed centers and radii
    Real** aafCCR = new Real*[i1];
    aafCCR[0] = new Real[4*i1];
    for (i0 = 1; i0 < i1; i0++)
        aafCCR[i0] = aafCCR[0] + 4*i0;
    aafCCR[0][0] = 0.0f;
    aafCCR[0][1] = 0.0f;
    aafCCR[0][2] = 0.0f;
    aafCCR[0][3] = Math::MAX_REAL;

    int iTetraQuantity = 1;  // number of tetrahedra
    i4 = 1;

    // compute tetrahedralization
    for (i0 = 0; i0 < m_iVertexQuantity; i0++)
    {  
        i1 = i7 = -1;
        i9 = 0;
        for (i11 = 0; i11 < iTetraQuantity; i11++)
        {  
            i1++;
            while ( aaiA3S[i1][0] < 0 ) 
                i1++;
            fTmp = aafCCR[i1][3];
            for (i2 = 0; i2 < 3; i2++)
            {
                Real fZ = akPoint[i0][i2]-aafCCR[i1][i2];
                fTmp -= fZ*fZ;
                if ( fTmp < 0.0f ) 
                    goto Corner3;
            }
            i9--;
            i4--;
            aaID[i4] = i1;
            for (i2 = 0; i2 < 4; i2++)
            {
                aiII[0] = 0;
                if ( aiII[0] == i2 ) 
                    aiII[0]++;
                for (i3 = 1; i3 < 3; i3++)
                {
                    aiII[i3] = aiII[i3-1] + 1;
                    if ( aiII[i3] == i2 ) 
                        aiII[i3]++;
                }
                if ( i7 > 2 )
                {
                    i8 = i7;
                    for (i3 = 0; i3 <= i8; i3++)
                    {
                        for (i5 = 0; i5 < 3; i5++)
                        {
                            if ( aaiA3S[i1][aiII[i5]] != aaiTmp[i3][i5] ) 
                                goto Corner1;
                        }
                        for (i6 = 0; i6 < 3; i6++)
                            aaiTmp[i3][i6] = aaiTmp[i8][i6];
                        i7--;
                        goto Corner2;
Corner1:;
                    }
                }
                if ( ++i7 > iSixTSize )
                {
                    // Temporary storage exceeded.  Increase ms_iTSize and
                    // call the constructor again.
                    assert( false );
                    goto ExitDelaunay;
                }
                for (i3 = 0; i3 < 3; i3++) 
                    aaiTmp[i7][i3] = aaiA3S[i1][aiII[i3]];
Corner2:;
            }
            aaiA3S[i1][0] = -1;
Corner3:;
        }

        for (i1 = 0; i1 <= i7; i1++)
        {
            for (i2 = 0; i2 < 3; i2++)
                for (aafWork[3][i2] = 0, i3 = 0; i3 < 3; i3++)
                {
                    aafWork[i3][i2] = akPoint[aaiTmp[i1][i2]][i3] -
                        akPoint[i0][i3];
                    aafWork[3][i2] += 0.5f*aafWork[i3][i2]*(
                        akPoint[aaiTmp[i1][i2]][i3] + akPoint[i0][i3]);
                }

            fTmp = (aafWork[0][0]*(aafWork[1][1]*aafWork[2][2] -
                aafWork[1][2]*aafWork[2][1])) - (aafWork[1][0]*(
                aafWork[0][1]*aafWork[2][2] - aafWork[0][2]*aafWork[2][1])) +
                (aafWork[2][0]*(aafWork[0][1]*aafWork[1][2] - aafWork[0][2]*
                aafWork[1][1]));
            assert( fTmp != 0.0f );
            fTmp = 1.0f/fTmp;
            aafCCR[aaID[i4]][0] = ((aafWork[3][0]*(aafWork[1][1]*
                aafWork[2][2] - aafWork[1][2]*aafWork[2][1])) -
                (aafWork[1][0]*(aafWork[3][1]*aafWork[2][2] - aafWork[3][2]*
                aafWork[2][1])) + (aafWork[2][0]*(aafWork[3][1]*
                aafWork[1][2] - aafWork[3][2]*aafWork[1][1])))*fTmp;
            aafCCR[aaID[i4]][1] = ((aafWork[0][0]*(aafWork[3][1]*
                aafWork[2][2] - aafWork[3][2]*aafWork[2][1])) -
                (aafWork[3][0]*(aafWork[0][1]*aafWork[2][2] - aafWork[0][2]*
                aafWork[2][1])) + (aafWork[2][0]*(aafWork[0][1]*
                aafWork[3][2] - aafWork[0][2]*aafWork[3][1])))*fTmp;
            aafCCR[aaID[i4]][2] = ((aafWork[0][0]*(aafWork[1][1]*
                aafWork[3][2] - aafWork[1][2]*aafWork[3][1])) -
                (aafWork[1][0]*(aafWork[0][1]*aafWork[3][2] - aafWork[0][2]*
                aafWork[3][1])) + (aafWork[3][0]*(aafWork[0][1]*
                aafWork[1][2] - aafWork[0][2]*aafWork[1][1])))*fTmp;

            for (aafCCR[aaID[i4]][3] = 0, i2 = 0; i2 < 3; i2++) 
            {
                Real fZ = akPoint[i0][i2] - aafCCR[aaID[i4]][i2];
                aafCCR[aaID[i4]][3] += fZ*fZ;
                aaiA3S[aaID[i4]][i2] = aaiTmp[i1][i2];
            }

            aaiA3S[aaID[i4]][3] = i0;
            i4++;
            i9++;
        }
        iTetraQuantity += i9;
    }

    // count the number of tetrahedra
    m_iTetrahedronQuantity = 0;
    i0 = -1;
    for (i11 = 0; i11 < iTetraQuantity; i11++)
    {
        i0++;
        while ( aaiA3S[i0][0] < 0 )
            i0++;
        if ( aaiA3S[i0][0] < m_iVertexQuantity )
        {
            for (i1 = 0; i1 < 3; i1++)
            {
                for (i2 = 0; i2 < 3; i2++) 
                {
                    aafWork[i2][i1] = akPoint[aaiA3S[i0][i1]][i2] -
                        akPoint[aaiA3S[i0][3]][i2];
                }
            }

            fTmp = ((aafWork[0][0]*(aafWork[1][1]*aafWork[2][2] -
                aafWork[1][2]*aafWork[2][1])) - (aafWork[1][0]*(
                aafWork[0][1]*aafWork[2][2] - aafWork[0][2]*aafWork[2][1])) +
                (aafWork[2][0]*(aafWork[0][1]*aafWork[1][2] - aafWork[0][2]*
                aafWork[1][1])));

            if ( Math::FAbs(fTmp) > ms_fEpsilon*fMaxRangeCube )
                m_iTetrahedronQuantity++;
        }
    }

    // create the tetrahedra
    m_akTetrahedron = new Tetrahedron[m_iTetrahedronQuantity];
    m_iTetrahedronQuantity = 0;
    i0 = -1;
    for (i11 = 0; i11 < iTetraQuantity; i11++)
    {
        i0++;
        while ( aaiA3S[i0][0] < 0 )
            i0++;
        if ( aaiA3S[i0][0] < m_iVertexQuantity )
        {
            for (i1 = 0; i1 < 3; i1++)
            {
                for (i2 = 0; i2 < 3; i2++) 
                {
                    aafWork[i2][i1] = akPoint[aaiA3S[i0][i1]][i2] -
                        akPoint[aaiA3S[i0][3]][i2];
                }
            }

            fTmp = ((aafWork[0][0]*(aafWork[1][1]*aafWork[2][2] -
                aafWork[1][2]*aafWork[2][1])) - (aafWork[1][0]*(
                aafWork[0][1]*aafWork[2][2] - aafWork[0][2]*aafWork[2][1])) +
                (aafWork[2][0]*(aafWork[0][1]*aafWork[1][2]-aafWork[0][2]*
                aafWork[1][1])));

            if ( Math::FAbs(fTmp) > ms_fEpsilon*fMaxRangeCube )
            {
                int iDelta = (fTmp < 0.0f ? 1 : 0);
                Tetrahedron& rkTetra =
                    m_akTetrahedron[m_iTetrahedronQuantity];
                rkTetra.m_aiVertex[0] = aaiA3S[i0][0];
                rkTetra.m_aiVertex[1] = aaiA3S[i0][1];
                rkTetra.m_aiVertex[2] = aaiA3S[i0][2+iDelta];
                rkTetra.m_aiVertex[3] = aaiA3S[i0][3-iDelta];
                m_iTetrahedronQuantity++;
            }
        }
    }

ExitDelaunay:
    delete[] aaiTmp[0];
    delete[] aaiTmp;
    delete[] aaID;
    delete[] aaiA3S[0];
    delete[] aaiA3S;
    delete[] aafCCR[0];
    delete[] aafCCR;
    delete[] akPoint;
}
//----------------------------------------------------------------------------
Delaunay3D::Delaunay3D (Delaunay3D& rkNet)
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
    m_fZMin = rkNet.m_fZMin;
    m_fZMax = rkNet.m_fZMax;
    m_fZRange = rkNet.m_fZRange;
    m_iTetrahedronQuantity = rkNet.m_iTetrahedronQuantity;
    m_akTetrahedron = rkNet.m_akTetrahedron;
}
//----------------------------------------------------------------------------
Delaunay3D::~Delaunay3D ()
{
    if ( m_bOwner )
    {
        delete[] m_akVertex;
        delete[] m_akTetrahedron;
    }
}
//----------------------------------------------------------------------------
void Delaunay3D::ComputeBarycenter (const Vector3& rkV0, const Vector3& rkV1,
    const Vector3& rkV2, const Vector3& rkV3, const Vector3& rkP,
    Real afNumer[4], Real& rfDenom)
{
    Vector3 kV03 = rkV0 - rkV3;
    Vector3 kV13 = rkV1 - rkV3;
    Vector3 kV23 = rkV2 - rkV3;
    Vector3 kPV3 = rkP - rkV3;

    Real fM00 = kV03.Dot(kV03);
    Real fM01 = kV03.Dot(kV13);
    Real fM02 = kV03.Dot(kV23);
    Real fM11 = kV13.Dot(kV13);
    Real fM12 = kV13.Dot(kV23);
    Real fM22 = kV23.Dot(kV23);
    Real fR0 = kV03.Dot(kPV3);
    Real fR1 = kV13.Dot(kPV3);
    Real fR2 = kV23.Dot(kPV3);

    Real fC00 = fM11*fM22-fM12*fM12;
    Real fC01 = fM02*fM12-fM01*fM22;
    Real fC02 = fM01*fM12-fM02*fM11;
    Real fC11 = fM00*fM22-fM02*fM02;
    Real fC12 = fM02*fM01-fM00*fM12;
    Real fC22 = fM00*fM11-fM01*fM01;
    rfDenom = fM00*fC00 + fM01*fC01 + fM02*fC02;
    assert( rfDenom != 0.0f );

    afNumer[0] = fC00*fR0+fC01*fR1+fC02*fR2;
    afNumer[1] = fC01*fR0+fC11*fR1+fC12*fR2;
    afNumer[2] = fC02*fR0+fC12*fR1+fC22*fR2;
    afNumer[3] = rfDenom-afNumer[0]-afNumer[1]-afNumer[2];
}
//----------------------------------------------------------------------------
bool Delaunay3D::InTetrahedron (const Vector3& rkV0, const Vector3& rkV1,
    const Vector3& rkV2, const Vector3& rkV3, const Vector3& rkP)
{
    const Real fEpsilon = 1e-06f;
    Real afNumer[4], fDenom, fPerturb;
    int i;

    ComputeBarycenter(rkV0,rkV1,rkV2,rkV3,rkP,afNumer,fDenom);
    fPerturb = fEpsilon*fDenom;

    if ( fDenom > 0.0f )
    {
        for (i = 0; i < 4; i++)
        {
            if ( afNumer[i] < -fPerturb || afNumer[i] > fDenom+fPerturb )
                return false;
        }
    }
    else
    {
        for (i = 0; i < 4; i++)
        {
            if ( afNumer[i] < fDenom+fPerturb || afNumer[i] > -fPerturb )
                return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool Delaunay3D::InTetrahedron (const Real afNumer[4], Real fDenom)
{
    const Real fEpsilon = 1e-06f;
    Real fPerturb = fEpsilon*fDenom;
    int i;

    if ( fDenom > 0.0f )
    {
        for (i = 0; i < 4; i++)
        {
            if ( afNumer[i] < -fPerturb || afNumer[i] > fDenom+fPerturb )
                return false;
        }
    }
    else
    {
        for (i = 0; i < 4; i++)
        {
            if ( afNumer[i] < fDenom+fPerturb || afNumer[i] > -fPerturb )
                return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------

