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

#include "MgcEigen.h"
#include "MgcQuadricSurface.h"
using namespace Mgc;

//----------------------------------------------------------------------------
QuadricSurface::QuadricSurface (const Matrix3& rkA, const Vector3& rkB,
    Real fC)
    :
    m_kA(rkA), m_kB(rkB)
{
    m_fC = fC;
}
//----------------------------------------------------------------------------
void QuadricSurface::GetCharacterization (Type& eType, Real afD[3]) const
{
    eType = QST_MAX_TYPE;

    Eigen kES(3);
    int iRow, iCol;
    for (iRow = 0; iRow < 3; iRow++)
    {
        for (iCol = 0; iCol < 3; iCol++)
            kES.Matrix(iRow,iCol) = m_kA[iRow][iCol];
    }

    kES.IncrSortEigenStuff3();
    for (iRow = 0; iRow < 3; iRow++)
        afD[iRow] = kES.GetEigenvalue(iRow);

    const Real fEpsilon = 1e-06f;
    if ( Math::FAbs(afD[2]) > fEpsilon )
    {
        // matrix A has at least one nonzero eigenvalue, change coordinates
        Vector3 kE = Vector3::ZERO;
        for (iRow = 0; iRow < 3; iRow++)
        {
            for (iCol = 0; iCol < 3; iCol++)
                kE[iRow] += kES.Matrix(iRow,iCol)*m_kB[iCol];
        }

        Real fF;
        if ( Math::FAbs(afD[1]) > fEpsilon )
        {
            if ( Math::FAbs(afD[0]) > fEpsilon )
            {
                // matrix A has three nonzero eigenvalues
                fF = -m_fC;
                for (iRow = 0; iRow <= 2; iRow++)
                    fF += 0.25f*kE[iRow]*kE[iRow]/afD[iRow];

                if ( fF > fEpsilon )
                {
                    if ( afD[2] < 0.0f )
                        eType = QST_NONE;
                    else if ( afD[0] > 0.0f )
                        eType = QST_ELLIPSOID;
                    else if ( afD[1] > 0.0f )
                        eType = QST_HYPERBOLOID_ONE_SHEET;
                    else
                        eType = QST_HYPERBOLOID_TWO_SHEETS;
                }
                else if ( fF < -fEpsilon )
                {
                    if ( afD[0] > 0.0f )
                        eType = QST_NONE;
                    else if ( afD[2] < 0.0f )
                        eType = QST_ELLIPSOID;
                    else if ( afD[1] > 0.0f )
                        eType = QST_HYPERBOLOID_ONE_SHEET;
                    else
                        eType = QST_HYPERBOLOID_TWO_SHEETS;
                }
                else
                {
                    if ( afD[0] > 0.0f || afD[2] < 0.0f )
                        eType = QST_POINT;
                    else
                        eType = QST_ELLIPTIC_CONE;
                }
            }
            else
            {
                // matrix A has two nonzero eigenvalues (d1,d2)
                if ( Math::FAbs(kE[0]) > fEpsilon )
                {
                    fF = -m_fC;
                    for (iRow = 1; iRow <= 2; iRow++)
                        fF += 0.25f*kE[iRow]*kE[iRow]/afD[iRow];

                    if ( fF > fEpsilon )
                    {
                        if ( afD[1] > 0.0f )
                        {
                            if ( afD[2] > 0.0f )
                                eType = QST_ELLIPTIC_CYLINDER;
                            else
                                eType = QST_HYPERBOLIC_CYLINDER;
                        }
                        else
                        {
                            if ( afD[2] > 0.0f )
                                eType = QST_HYPERBOLIC_CYLINDER;
                            else
                                eType = QST_NONE;
                        }
                    }
                    else if ( fF < -fEpsilon )
                    {
                        if ( afD[1] > 0.0f )
                        {
                            if ( afD[2] > 0.0f )
                                eType = QST_NONE;
                            else
                                eType = QST_HYPERBOLIC_CYLINDER;
                        }
                        else
                        {
                            if ( afD[2] > 0.0f )
                                eType = QST_HYPERBOLIC_CYLINDER;
                            else
                                eType = QST_ELLIPTIC_CYLINDER;
                        }
                    }
                    else
                    {
                        if ( afD[1]*afD[2] > 0.0f )
                            eType = QST_LINE;
                        else
                            eType = QST_TWO_PLANES;
                    }
                }
                else
                {
                    if ( afD[1]*afD[2] > 0.0f )
                        eType = QST_ELLIPTIC_PARABOLOID;
                    else
                        eType = QST_HYPERBOLIC_PARABOLOID;
                }
            }
        }
        else
        {
            // matrix A has one nonzero eigenvalues (d2)
            if ( Math::FAbs(kE[0]) > fEpsilon || Math::FAbs(kE[1]) > fEpsilon )
            {
                eType = QST_PARABOLIC_CYLINDER;
            }
            else
            {
                fF = 0.25f*kE[2]*kE[2]/afD[2] - m_fC;
                if ( fF*afD[2] > 0.0f )
                    eType = QST_PLANE;
                else
                    eType = QST_NONE;
            }
        }
    }
    else
    {
        // matrix A is (numerically) the zero matrix
        if ( m_kB.Length() > fEpsilon )
            eType = QST_PLANE;
        else
            eType = QST_NONE;
    }

    assert( eType != QST_MAX_TYPE );
}
//----------------------------------------------------------------------------
int QuadricSurface::VertexIndex (const ConvexPolyhedron& rkPoly,
    const Vertex* pkV)
{
    return int(pkV - rkPoly.m_apkVertex);
}
//----------------------------------------------------------------------------
int QuadricSurface::EdgeIndex (const ConvexPolyhedron& rkPoly,
    const Edge* pkE)
{
    return int(pkE - rkPoly.m_apkEdge);
}
//----------------------------------------------------------------------------
int QuadricSurface::TriangleIndex (const ConvexPolyhedron& rkPoly,
    const Triangle* pkT)
{
    return int(pkT - rkPoly.m_apkTriangle);
}
//----------------------------------------------------------------------------
int QuadricSurface::AdjacentOrient (const Triangle* pkT,
    const Triangle* pkA)
{
    if ( pkA->m_apkAdjacent[0] == pkT )
        return 0;
    if ( pkA->m_apkAdjacent[1] == pkT )
        return 1;
    return 2;
}
//----------------------------------------------------------------------------
void QuadricSurface::ComputeCentroid (ConvexPolyhedron& rkPoly,
    int iNumVertices)
{
    rkPoly.m_kCentroid = Vector3::ZERO;
    for (int i = 0; i < iNumVertices; i++)
        rkPoly.m_kCentroid += *rkPoly.m_apkVertex[i].m_pkPoint;

    Real fInvNumVertices = 1.0f/Real(iNumVertices);
    rkPoly.m_kCentroid *= fInvNumVertices;
}
//----------------------------------------------------------------------------
void QuadricSurface::RayIntersectSphere (const Vector3& rkCen,
    const Vector3& rkMid, Vector3* pkIntersect)
{
    // ray is X(t) = cen+t*(mid-cen) = cen+t*dir
    Vector3 kDir = rkMid - rkCen;

    // Find t > 0 for which Dot(X(t),X(t)) = 1.  This yields a quadratic
    // equation a2*t^2+a1*t+a0 = 0 for which we want the positive root.
    Real fA2 = kDir.SquaredLength();
    Real fA1 = 2.0f*kDir.Dot(rkCen);
    Real fA0 = rkCen.SquaredLength() - 1.0f;
    Real fDiscr = Math::Sqrt(Math::FAbs(fA1*fA1-4.0f*fA0*fA2));
    Real fT = (-fA1+fDiscr)/(2.0f*fA2);

    *pkIntersect = rkCen + fT*kDir;
}
//----------------------------------------------------------------------------
void QuadricSurface::Expand (int iSteps, ConvexPolyhedron& rkPoly)
{
    // compute centroid of seed polyhedron
    ComputeCentroid(rkPoly,rkPoly.m_iNumVertices);

    // save seed polyhedron (shallow copy)
    ConvexPolyhedron kSeed = rkPoly;

    // allocate space for fully subdivided polyhedron
    for (int i = 1; i <= iSteps; i++)
    {
        rkPoly.m_iNumVertices = rkPoly.m_iNumVertices + rkPoly.m_iNumEdges;
        rkPoly.m_iNumEdges = 2*rkPoly.m_iNumEdges + 3*rkPoly.m_iNumTriangles;
        rkPoly.m_iNumTriangles = 4*rkPoly.m_iNumTriangles;
    }

    rkPoly.m_apkVertex = new Vertex[rkPoly.m_iNumVertices];
    rkPoly.m_apkEdge = new Edge[rkPoly.m_iNumEdges];
    rkPoly.m_apkTriangle = new Triangle[rkPoly.m_iNumTriangles];

    // duplicate data structure

    // duplicate vertices, compute centroid
    int iV, iE;
    for (iV = 0; iV < kSeed.m_iNumVertices; iV++)
    {
        // transfer points from seed to rkPoly
        Vertex& rkVS = kSeed.m_apkVertex[iV];
        Vertex& rkVP = rkPoly.m_apkVertex[iV];
        rkVP.m_pkPoint = rkVS.m_pkPoint;

        // duplicate edge information
        rkVP.m_iNumEdges = rkVS.m_iNumEdges;
        int iQuantity = rkVS.m_iNumEdges*sizeof(Edge*);
        rkVP.m_apkEdge = new Edge*[iQuantity];

        for (iE = 0; iE < rkVS.m_iNumEdges; iE++)
        {
            rkVP.m_apkEdge[iE] = &rkPoly.m_apkEdge[EdgeIndex(kSeed,
                rkVS.m_apkEdge[iE])];
        }

        delete[] rkVS.m_apkEdge;
    }

    delete[] kSeed.m_apkVertex;

    // duplicate edges
    for (iE = 0; iE < kSeed.m_iNumEdges; iE++)
    {
        Edge& rkES = kSeed.m_apkEdge[iE];
        Edge& rkEP = rkPoly.m_apkEdge[iE];

        for (int i = 0; i < 2; i++)
        {
            rkEP.m_apkVertex[i] = &rkPoly.m_apkVertex[
                VertexIndex(kSeed,rkES.m_apkVertex[i])];
            rkEP.m_apkTriangle[i] = &rkPoly.m_apkTriangle[
                TriangleIndex(kSeed,rkES.m_apkTriangle[iE])];
        }

        // For testing purposes, but not necessary for the algorithm.
        // This allows the display program to show the subdivision
        // structure.
        rkEP.m_iStep = 0;
    }

    delete[] kSeed.m_apkEdge;

    // duplicate triangles
    for (int iT = 0; iT < kSeed.m_iNumTriangles; iT++)
    {
        Triangle& rkTS = kSeed.m_apkTriangle[iT];
        Triangle& rkTP = rkPoly.m_apkTriangle[iT];

        for (int i = 0; i < 3; i++)
        {
            rkTP.m_apkVertex[i] = &rkPoly.m_apkVertex[
                VertexIndex(kSeed,rkTS.m_apkVertex[i])];
            rkTP.m_apkEdge[i] = &rkPoly.m_apkEdge[
                EdgeIndex(kSeed,rkTS.m_apkEdge[i])];
            rkTP.m_apkAdjacent[i] = &rkPoly.m_apkTriangle[
                TriangleIndex(kSeed,rkTS.m_apkAdjacent[i])];
        }
    }

    delete[] kSeed.m_apkTriangle;
}
//----------------------------------------------------------------------------
void QuadricSurface::TessellateSphere (int iSteps, ConvexPolyhedron& rkPoly)
{
    if ( iSteps == 0 )
        return;

    // indices to beginning of new slots
    int iVMax = rkPoly.m_iNumVertices;
    int iEMax = rkPoly.m_iNumEdges;
    int iTMax = rkPoly.m_iNumTriangles;

    // increase memory allocation for polyhedron to support subdivision
    Expand(iSteps,rkPoly);

    // subdivide polyhedron
    for (int i = 1; i <= iSteps; i++)
    {
        // Add new point locations to vertices.  Each vertex added in the
        // subdivision always has 6 edges sharing it.  The edge pointers
        // will be set later to point to the subdivided edges.
        //
        // Split old edges in half.  The vertex pointers are set to the new
        // vertices, but the triangle pointers are the old ones and must be
        // updated later to point to the subdivide triangles.
        int iE;
        for (iE = 0; iE < iEMax; iE++)
        {
            Edge* pkE0 = &rkPoly.m_apkEdge[iE];
            Edge* pkE1 = &rkPoly.m_apkEdge[iEMax+iE];

            // get end points of edge
            Vector3& rkP0 = *pkE0->m_apkVertex[0]->m_pkPoint;
            Vector3& rkP1 = *pkE0->m_apkVertex[1]->m_pkPoint;

            // New vertex is intersection of sphere and ray from polyhedron
            // centroid through midpoint of edge.
            Vector3 kMid = 0.5f*(rkP0 + rkP1);
            Vertex* pkM = &rkPoly.m_apkVertex[iVMax+iE];
            pkM->m_pkPoint = new Vector3;
            RayIntersectSphere(rkPoly.m_kCentroid,kMid,pkM->m_pkPoint);

            // numEdges will be incremented dynamically to 6 later on
            pkM->m_iNumEdges = 0;
            pkM->m_apkEdge = new Edge*[6];

            // edge[iE] = <V0,V1>, M = (V0+V1)/2, E0 = <V0,M>, E1 = <V1,M>
            pkE1->m_apkVertex[0] = pkE0->m_apkVertex[1];
            pkE0->m_apkVertex[1] = pkM;
            pkE1->m_apkVertex[1] = pkM;

            // Set as a flag for later use (avoids redundant copies of
            // edge triangle pointers).
            pkE0->m_apkTriangle[0] = NULL;
            pkE1->m_apkTriangle[0] = NULL;

            // For testing purposes, but not necessary for the algorithm.
            // This allows the display program to show the subdivision
            // structure.
            pkE1->m_iStep = pkE0->m_iStep;
        }

        // compute new centroid (for next step of iteration)
        ComputeCentroid(rkPoly,iVMax+iEMax);

        // Add new edges.  Vertex pointers are set to the new vertices, but
        // the triangle pointers are not calculated until later.
        //
        // Add new triangles.  The middle triangle will replace the old
        // triangle to save storage.  As a result, the adjacency values for
        // the middle triangle are calculated later since the current value
        // are needed globally for computing the other adjacencies.
        Triangle *pkT0, *pkT1, *pkT2, *pkT3;
        Vertex *pkM0, *pkM1, *pkM2;
        int iT, i0, i1, i2, i3, iOrient;
        int iTM1 = iTMax, iTM2 = 2*iTMax, iTM3 = 3*iTMax;
        iE = 2*iEMax;
        for (iT = 0; iT < iTMax; iT++, iTM1++, iTM2++, iTM3++)
        {
            // get triangle
            pkT0 = &rkPoly.m_apkTriangle[iT];

            // get vertices
            Vertex* pkV0 = pkT0->m_apkVertex[0];
            Vertex* pkV1 = pkT0->m_apkVertex[1];
            Vertex* pkV2 = pkT0->m_apkVertex[2];

            // get edges
            Edge* pkE0 = pkT0->m_apkEdge[0];
            Edge* pkE1 = pkT0->m_apkEdge[1];
            Edge* pkE2 = pkT0->m_apkEdge[2];

            // get adjacent triangles
            Triangle* pkA0 = pkT0->m_apkAdjacent[0];
            Triangle* pkA1 = pkT0->m_apkAdjacent[1];
            Triangle* pkA2 = pkT0->m_apkAdjacent[2];

            // get midpoints of triangle edges
            pkM0 = &rkPoly.m_apkVertex[iVMax+EdgeIndex(rkPoly,pkE0)];
            pkM1 = &rkPoly.m_apkVertex[iVMax+EdgeIndex(rkPoly,pkE1)];
            pkM2 = &rkPoly.m_apkVertex[iVMax+EdgeIndex(rkPoly,pkE2)];

            // get new edges and set vertex values
            Edge* pkE0New = &rkPoly.m_apkEdge[iE++];
            pkE0New->m_apkVertex[0] = pkM2;
            pkE0New->m_apkVertex[1] = pkM0;
            Edge* pkE1New = &rkPoly.m_apkEdge[iE++];
            pkE1New->m_apkVertex[0] = pkM0;
            pkE1New->m_apkVertex[1] = pkM1;
            Edge* pkE2New = &rkPoly.m_apkEdge[iE++];
            pkE2New->m_apkVertex[0] = pkM1;
            pkE2New->m_apkVertex[1] = pkM2;

            // For testing purposes, but not necessary for the algorithm.
            // This allows the display program to show the subdivision
            // structure.
            pkE0New->m_iStep = i;
            pkE1New->m_iStep = i;
            pkE2New->m_iStep = i;

            // construct triangle T1
            pkT1 = &rkPoly.m_apkTriangle[iTM1];
            pkT1->m_apkVertex[0] = pkV0;
            pkT1->m_apkVertex[1] = pkM0;
            pkT1->m_apkVertex[2] = pkM2;
            pkT1->m_apkEdge[0] =
                ( pkE0->m_apkVertex[0] == pkV0 ? pkE0 : pkE0+iEMax );
            pkT1->m_apkEdge[1] = pkE0New;
            pkT1->m_apkEdge[2] =
                ( pkE2->m_apkVertex[0] == pkV0 ? pkE2 : pkE2+iEMax );
            pkT1->m_apkAdjacent[1] = pkT0;

            // construct triangle T2
            pkT2 = &rkPoly.m_apkTriangle[iTM2];
            pkT2->m_apkVertex[0] = pkM0;
            pkT2->m_apkVertex[1] = pkV1;
            pkT2->m_apkVertex[2] = pkM1;
            pkT2->m_apkEdge[0] =
                ( pkE0->m_apkVertex[0] == pkV1 ? pkE0 : pkE0+iEMax );
            pkT2->m_apkEdge[1] =
                ( pkE1->m_apkVertex[0] == pkV1 ? pkE1 : pkE1+iEMax );
            pkT2->m_apkEdge[2] = pkE1New;
            pkT2->m_apkAdjacent[2] = pkT0;

            // construct triangle T3
            pkT3 = &rkPoly.m_apkTriangle[iTM3];
            pkT3->m_apkVertex[0] = pkM2;
            pkT3->m_apkVertex[1] = pkM1;
            pkT3->m_apkVertex[2] = pkV2;
            pkT3->m_apkEdge[0] = pkE2New;
            pkT3->m_apkEdge[1] =
                ( pkE1->m_apkVertex[0] == pkV2 ? pkE1 : pkE1+iEMax );
            pkT3->m_apkEdge[2] =
                ( pkE2->m_apkVertex[0] == pkV2 ? pkE2 : pkE2+iEMax );
            pkT3->m_apkAdjacent[0] = pkT0;

            // set edge triangle pointers
            pkE0New->m_apkTriangle[0] = pkT0;
            pkE0New->m_apkTriangle[1] = pkT1;
            pkE1New->m_apkTriangle[0] = pkT0;
            pkE1New->m_apkTriangle[1] = pkT2;
            pkE2New->m_apkTriangle[0] = pkT0;
            pkE2New->m_apkTriangle[1] = pkT3;

            // get the indices for the subdivided triangles of A0
            i0 = TriangleIndex(rkPoly,pkA0);
            i1 = i0+iTMax;
            i2 = i1+iTMax;
            i3 = i2+iTMax;

            // Determine the orientation of A0 relative to T0 and set the
            // adjacency links.
            iOrient = AdjacentOrient(pkT0,pkA0);
            if ( iOrient == 0 )
            {
                pkT1->m_apkAdjacent[0] = &rkPoly.m_apkTriangle[i2];
                pkT2->m_apkAdjacent[0] = &rkPoly.m_apkTriangle[i1];
                rkPoly.m_apkTriangle[i2].m_apkAdjacent[0] = pkT1;
                rkPoly.m_apkTriangle[i1].m_apkAdjacent[0] = pkT2;
            }
            else if ( iOrient == 1 )
            {
                pkT1->m_apkAdjacent[0] = &rkPoly.m_apkTriangle[i3];
                pkT2->m_apkAdjacent[0] = &rkPoly.m_apkTriangle[i2];
                rkPoly.m_apkTriangle[i3].m_apkAdjacent[1] = pkT1;
                rkPoly.m_apkTriangle[i2].m_apkAdjacent[1] = pkT2;
            }
            else
            {
                pkT1->m_apkAdjacent[0] = &rkPoly.m_apkTriangle[i1];
                pkT2->m_apkAdjacent[0] = &rkPoly.m_apkTriangle[i3];
                rkPoly.m_apkTriangle[i1].m_apkAdjacent[2] = pkT1;
                rkPoly.m_apkTriangle[i3].m_apkAdjacent[2] = pkT2;
            }

            // get the indices for the subdivided triangles of A1
            i0 = TriangleIndex(rkPoly,pkA1);
            i1 = i0+iTMax;
            i2 = i1+iTMax;
            i3 = i2+iTMax;

            // Determine the orientation of A1 relative to T0 and set the
            // adjacency links.
            iOrient = AdjacentOrient(pkT0,pkA1);
            if ( iOrient == 0 )
            {
                pkT2->m_apkAdjacent[1] = &rkPoly.m_apkTriangle[i2];
                pkT3->m_apkAdjacent[1] = &rkPoly.m_apkTriangle[i1];
                rkPoly.m_apkTriangle[i2].m_apkAdjacent[0] = pkT2;
                rkPoly.m_apkTriangle[i1].m_apkAdjacent[0] = pkT3;
            }
            else if ( iOrient == 1 )
            {
                pkT2->m_apkAdjacent[1] = &rkPoly.m_apkTriangle[i3];
                pkT3->m_apkAdjacent[1] = &rkPoly.m_apkTriangle[i2];
                rkPoly.m_apkTriangle[i3].m_apkAdjacent[1] = pkT2;
                rkPoly.m_apkTriangle[i2].m_apkAdjacent[1] = pkT3;
            }
            else
            {
                pkT2->m_apkAdjacent[1] = &rkPoly.m_apkTriangle[i1];
                pkT3->m_apkAdjacent[1] = &rkPoly.m_apkTriangle[i3];
                rkPoly.m_apkTriangle[i1].m_apkAdjacent[2] = pkT2;
                rkPoly.m_apkTriangle[i3].m_apkAdjacent[2] = pkT3;
            }

            // get the indices for the subdivided triangles of A2
            i0 = TriangleIndex(rkPoly,pkA2);
            i1 = i0+iTMax;
            i2 = i1+iTMax;
            i3 = i2+iTMax;

            // Determine the orientation of A2 relative to T0 and set the
            // adjacency links.
            iOrient = AdjacentOrient(pkT0,pkA2);
            if ( iOrient == 0 )
            {
                pkT3->m_apkAdjacent[2] = &rkPoly.m_apkTriangle[i2];
                pkT1->m_apkAdjacent[2] = &rkPoly.m_apkTriangle[i1];
                rkPoly.m_apkTriangle[i2].m_apkAdjacent[0] = pkT3;
                rkPoly.m_apkTriangle[i1].m_apkAdjacent[0] = pkT1;
            }
            else if ( iOrient == 1 )
            {
                pkT3->m_apkAdjacent[2] = &rkPoly.m_apkTriangle[i3];
                pkT1->m_apkAdjacent[2] = &rkPoly.m_apkTriangle[i2];
                rkPoly.m_apkTriangle[i3].m_apkAdjacent[1] = pkT3;
                rkPoly.m_apkTriangle[i2].m_apkAdjacent[1] = pkT1;
            }
            else
            {
                pkT3->m_apkAdjacent[2] = &rkPoly.m_apkTriangle[i1];
                pkT1->m_apkAdjacent[2] = &rkPoly.m_apkTriangle[i3];
                rkPoly.m_apkTriangle[i1].m_apkAdjacent[2] = pkT3;
                rkPoly.m_apkTriangle[i3].m_apkAdjacent[2] = pkT1;
            }

            // add edge links to midpoint vertices
            if ( pkM0->m_iNumEdges == 0 )
            {
                // add four edges (first time edges are added for this vertex)
                pkM0->m_iNumEdges = 4;
                pkM0->m_apkEdge[0] = pkT1->m_apkEdge[0];
                pkM0->m_apkEdge[1] = pkT1->m_apkEdge[1];
                pkM0->m_apkEdge[2] = pkT2->m_apkEdge[2];
                pkM0->m_apkEdge[3] = pkT2->m_apkEdge[0];
            }
            else if ( pkM0->m_iNumEdges == 4 )
            {
                // add two edges (last time edges are added for this vertex)
                pkM0->m_iNumEdges = 6;
                pkM0->m_apkEdge[4] = pkT1->m_apkEdge[1];
                pkM0->m_apkEdge[5] = pkT2->m_apkEdge[2];
            }

            if ( pkM1->m_iNumEdges == 0 )
            {
                // add four edges (first time edges are added for this vertex)
                pkM1->m_iNumEdges = 4;
                pkM1->m_apkEdge[0] = pkT2->m_apkEdge[1];
                pkM1->m_apkEdge[1] = pkT2->m_apkEdge[2];
                pkM1->m_apkEdge[2] = pkT3->m_apkEdge[0];
                pkM1->m_apkEdge[3] = pkT3->m_apkEdge[1];
            }
            else if ( pkM1->m_iNumEdges == 4 )
            {
                // add two edges (last time edges are added for this vertex)
                pkM1->m_iNumEdges = 6;
                pkM1->m_apkEdge[4] = pkT2->m_apkEdge[2];
                pkM1->m_apkEdge[5] = pkT3->m_apkEdge[0];
            }

            if ( pkM2->m_iNumEdges == 0 )
            {
                // add four edges (first time edges are added for this vertex)
                pkM2->m_iNumEdges = 4;
                pkM2->m_apkEdge[0] = pkT1->m_apkEdge[2];
                pkM2->m_apkEdge[1] = pkT1->m_apkEdge[1];
                pkM2->m_apkEdge[2] = pkT3->m_apkEdge[0];
                pkM2->m_apkEdge[3] = pkT3->m_apkEdge[2];
            }
            else if ( pkM2->m_iNumEdges == 4 )
            {
                // add two edges (last time edges are added for this vertex)
                pkM2->m_iNumEdges = 6;
                pkM2->m_apkEdge[4] = pkT1->m_apkEdge[1];
                pkM2->m_apkEdge[5] = pkT3->m_apkEdge[0];
            }
        }

        // Set middle triangle vertex, edge, and triangle pointers.  Set
        // edge triangle pointers.
        iTM1 = iTMax;
        iTM2 = 2*iTMax;
        iTM3 = 3*iTMax;
        for (iT = 0; iT < iTMax; iT++, iTM1++, iTM2++, iTM3++)
        {
            // get subdivided triangles
            pkT0 = &rkPoly.m_apkTriangle[iT];
            pkT1 = &rkPoly.m_apkTriangle[iTM1];
            pkT2 = &rkPoly.m_apkTriangle[iTM2];
            pkT3 = &rkPoly.m_apkTriangle[iTM3];

            // get midpoints of original triangle edges
            pkM0 = &rkPoly.m_apkVertex[iVMax+EdgeIndex(rkPoly,
                pkT0->m_apkEdge[0])];
            pkM1 = &rkPoly.m_apkVertex[iVMax+EdgeIndex(rkPoly,
                pkT0->m_apkEdge[1])];
            pkM2 = &rkPoly.m_apkVertex[iVMax+EdgeIndex(rkPoly,
                pkT0->m_apkEdge[2])];

            // set vertices for middle triangle
            pkT0->m_apkVertex[0] = pkM2;
            pkT0->m_apkVertex[1] = pkM0;
            pkT0->m_apkVertex[2] = pkM1;

            // set edges for middle triangle
            pkT0->m_apkEdge[0] = pkT1->m_apkEdge[1];
            pkT0->m_apkEdge[1] = pkT2->m_apkEdge[2];
            pkT0->m_apkEdge[2] = pkT3->m_apkEdge[0];

            // set adjacencies for middle triangle
            pkT0->m_apkAdjacent[0] = pkT1;
            pkT0->m_apkAdjacent[1] = pkT2;
            pkT0->m_apkAdjacent[2] = pkT3;

            // set edge triangle pointers
            if ( pkT1->m_apkEdge[0]->m_apkTriangle[0] == 0 )
            {
                pkT1->m_apkEdge[0]->m_apkTriangle[0] = pkT1;
                pkT1->m_apkEdge[0]->m_apkTriangle[1] = pkT1->m_apkAdjacent[0];
            }
            if ( pkT1->m_apkEdge[2]->m_apkTriangle[0] == 0 )
            {
                pkT1->m_apkEdge[2]->m_apkTriangle[0] = pkT1;
                pkT1->m_apkEdge[2]->m_apkTriangle[1] = pkT1->m_apkAdjacent[2];
            }

            if ( pkT2->m_apkEdge[0]->m_apkTriangle[0] == 0 )
            {
                pkT2->m_apkEdge[0]->m_apkTriangle[0] = pkT2;
                pkT2->m_apkEdge[0]->m_apkTriangle[1] = pkT2->m_apkAdjacent[0];
            }
            if ( pkT2->m_apkEdge[1]->m_apkTriangle[0] == 0 )
            {
                pkT2->m_apkEdge[1]->m_apkTriangle[0] = pkT2;
                pkT2->m_apkEdge[1]->m_apkTriangle[1] = pkT2->m_apkAdjacent[1];
            }

            if ( pkT3->m_apkEdge[1]->m_apkTriangle[0] == 0 )
            {
                pkT3->m_apkEdge[1]->m_apkTriangle[0] = pkT3;
                pkT3->m_apkEdge[1]->m_apkTriangle[1] = pkT3->m_apkAdjacent[1];
            }
            if ( pkT3->m_apkEdge[2]->m_apkTriangle[0] == 0 )
            {
                pkT3->m_apkEdge[2]->m_apkTriangle[0] = pkT3;
                pkT3->m_apkEdge[2]->m_apkTriangle[1] = pkT3->m_apkAdjacent[2];
            }
        }

        // Adjust edge pointers of original vertices to account for the
        // edge splitting.
        for (int iV = 0; iV < iVMax; iV++)
        {
            Vertex* pkV = &rkPoly.m_apkVertex[iV];
            for (iE = 0; iE < pkV->m_iNumEdges; iE++)
            {
                Edge* pkE = pkV->m_apkEdge[iE];
                if ( pkE->m_apkVertex[0] != pkV
                &&   pkE->m_apkVertex[1] != pkV )
                {
                    pkV->m_apkEdge[iE] = &rkPoly.m_apkEdge[
                        iEMax+EdgeIndex(rkPoly,pkE)];
                }
            }
        }

        // update indices
        iVMax = iVMax + iEMax;
        iEMax = 2*iEMax + 3*iTMax;
        iTMax = 4*iTMax;
    }
}
//----------------------------------------------------------------------------
void QuadricSurface::DeletePolyhedron (ConvexPolyhedron& rkPoly)
{
    // assumes that rkPoly.vertex[*].point was dynamically allocated
    for (int i = 0; i < rkPoly.m_iNumVertices; i++)
    {
        delete rkPoly.m_apkVertex[i].m_pkPoint;
        delete[] rkPoly.m_apkVertex[i].m_apkEdge;
    }

    delete[] rkPoly.m_apkVertex;
    delete[] rkPoly.m_apkEdge;
    delete[] rkPoly.m_apkTriangle;

    rkPoly.m_iNumVertices = 0;
    rkPoly.m_apkVertex = NULL;
    rkPoly.m_iNumEdges = 0;
    rkPoly.m_apkEdge = NULL;
    rkPoly.m_iNumTriangles = 0;
    rkPoly.m_apkTriangle = NULL;
}
//----------------------------------------------------------------------------

