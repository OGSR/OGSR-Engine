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
#include <windows.h>
#include "MgcQuadricSurface.h"
using namespace Mgc;

#define WSIZE 512
//#define PERSPECTIVE
#define CreatePoly CreateOctahedron
//#define CreatePoly CreateTetrahedron

//---------------------------------------------------------------------------
void CreateTetrahedron (QuadricSurface::ConvexPolyhedron& rkTetra)
{
    rkTetra.m_iNumVertices = 4;
    rkTetra.m_apkVertex = new QuadricSurface::Vertex[4];
    rkTetra.m_iNumEdges = 6;
    rkTetra.m_apkEdge = new QuadricSurface::Edge[6];
    rkTetra.m_iNumTriangles = 4;
    rkTetra.m_apkTriangle = new QuadricSurface::Triangle[4];

    // vertices
    rkTetra.m_apkVertex[0].m_pkPoint = new Vector3;
    rkTetra.m_apkVertex[0].m_pkPoint->x = 1.0f;
    rkTetra.m_apkVertex[0].m_pkPoint->y = 0.0f;
    rkTetra.m_apkVertex[0].m_pkPoint->z = 0.0f;
    rkTetra.m_apkVertex[0].m_iNumEdges = 3;
    rkTetra.m_apkVertex[0].m_apkEdge = new QuadricSurface::Edge*[3];
    rkTetra.m_apkVertex[0].m_apkEdge[0] = &rkTetra.m_apkEdge[0];
    rkTetra.m_apkVertex[0].m_apkEdge[1] = &rkTetra.m_apkEdge[2];
    rkTetra.m_apkVertex[0].m_apkEdge[2] = &rkTetra.m_apkEdge[3];

    rkTetra.m_apkVertex[1].m_pkPoint = new Vector3;
    rkTetra.m_apkVertex[1].m_pkPoint->x = 0.0f;
    rkTetra.m_apkVertex[1].m_pkPoint->y = 1.0f;
    rkTetra.m_apkVertex[1].m_pkPoint->z = 0.0f;
    rkTetra.m_apkVertex[1].m_iNumEdges = 3;
    rkTetra.m_apkVertex[1].m_apkEdge = new QuadricSurface::Edge*[3];
    rkTetra.m_apkVertex[1].m_apkEdge[0] = &rkTetra.m_apkEdge[0];
    rkTetra.m_apkVertex[1].m_apkEdge[1] = &rkTetra.m_apkEdge[1];
    rkTetra.m_apkVertex[1].m_apkEdge[2] = &rkTetra.m_apkEdge[4];

    rkTetra.m_apkVertex[2].m_pkPoint = new Vector3;
    rkTetra.m_apkVertex[2].m_pkPoint->x = 0.0f;
    rkTetra.m_apkVertex[2].m_pkPoint->y = 0.0f;
    rkTetra.m_apkVertex[2].m_pkPoint->z = 1.0f;
    rkTetra.m_apkVertex[2].m_iNumEdges = 3;
    rkTetra.m_apkVertex[2].m_apkEdge = new QuadricSurface::Edge*[3];
    rkTetra.m_apkVertex[2].m_apkEdge[0] = &rkTetra.m_apkEdge[1];
    rkTetra.m_apkVertex[2].m_apkEdge[1] = &rkTetra.m_apkEdge[2];
    rkTetra.m_apkVertex[2].m_apkEdge[2] = &rkTetra.m_apkEdge[5];

    rkTetra.m_apkVertex[3].m_pkPoint = new Vector3;
    rkTetra.m_apkVertex[3].m_pkPoint->x = sqrtf(1.0f/3.0f);
    rkTetra.m_apkVertex[3].m_pkPoint->y = sqrtf(1.0f/3.0f);
    rkTetra.m_apkVertex[3].m_pkPoint->z = sqrtf(1.0f/3.0f);
    rkTetra.m_apkVertex[3].m_iNumEdges = 3;
    rkTetra.m_apkVertex[3].m_apkEdge = new QuadricSurface::Edge*[3];
    rkTetra.m_apkVertex[3].m_apkEdge[0] = &rkTetra.m_apkEdge[3];
    rkTetra.m_apkVertex[3].m_apkEdge[1] = &rkTetra.m_apkEdge[4];
    rkTetra.m_apkVertex[3].m_apkEdge[2] = &rkTetra.m_apkEdge[5];

    // edges
    rkTetra.m_apkEdge[0].m_apkVertex[0] = &rkTetra.m_apkVertex[0];
    rkTetra.m_apkEdge[0].m_apkVertex[1] = &rkTetra.m_apkVertex[1];
    rkTetra.m_apkEdge[0].m_apkTriangle[0] = &rkTetra.m_apkTriangle[0];
    rkTetra.m_apkEdge[0].m_apkTriangle[1] = &rkTetra.m_apkTriangle[1];

    rkTetra.m_apkEdge[1].m_apkVertex[0] = &rkTetra.m_apkVertex[1];
    rkTetra.m_apkEdge[1].m_apkVertex[1] = &rkTetra.m_apkVertex[2];
    rkTetra.m_apkEdge[1].m_apkTriangle[0] = &rkTetra.m_apkTriangle[0];
    rkTetra.m_apkEdge[1].m_apkTriangle[1] = &rkTetra.m_apkTriangle[2];

    rkTetra.m_apkEdge[2].m_apkVertex[0] = &rkTetra.m_apkVertex[2];
    rkTetra.m_apkEdge[2].m_apkVertex[1] = &rkTetra.m_apkVertex[0];
    rkTetra.m_apkEdge[2].m_apkTriangle[0] = &rkTetra.m_apkTriangle[0];
    rkTetra.m_apkEdge[2].m_apkTriangle[1] = &rkTetra.m_apkTriangle[3];

    rkTetra.m_apkEdge[3].m_apkVertex[0] = &rkTetra.m_apkVertex[0];
    rkTetra.m_apkEdge[3].m_apkVertex[1] = &rkTetra.m_apkVertex[3];
    rkTetra.m_apkEdge[3].m_apkTriangle[0] = &rkTetra.m_apkTriangle[1];
    rkTetra.m_apkEdge[3].m_apkTriangle[1] = &rkTetra.m_apkTriangle[3];

    rkTetra.m_apkEdge[4].m_apkVertex[0] = &rkTetra.m_apkVertex[1];
    rkTetra.m_apkEdge[4].m_apkVertex[1] = &rkTetra.m_apkVertex[3];
    rkTetra.m_apkEdge[4].m_apkTriangle[0] = &rkTetra.m_apkTriangle[1];
    rkTetra.m_apkEdge[4].m_apkTriangle[1] = &rkTetra.m_apkTriangle[2];

    rkTetra.m_apkEdge[5].m_apkVertex[0] = &rkTetra.m_apkVertex[2];
    rkTetra.m_apkEdge[5].m_apkVertex[1] = &rkTetra.m_apkVertex[3];
    rkTetra.m_apkEdge[5].m_apkTriangle[0] = &rkTetra.m_apkTriangle[2];
    rkTetra.m_apkEdge[5].m_apkTriangle[1] = &rkTetra.m_apkTriangle[3];

    // triangles
    rkTetra.m_apkTriangle[0].m_apkVertex[0] = &rkTetra.m_apkVertex[0];
    rkTetra.m_apkTriangle[0].m_apkVertex[1] = &rkTetra.m_apkVertex[2];
    rkTetra.m_apkTriangle[0].m_apkVertex[2] = &rkTetra.m_apkVertex[1];
    rkTetra.m_apkTriangle[0].m_apkEdge[0] = &rkTetra.m_apkEdge[2];
    rkTetra.m_apkTriangle[0].m_apkEdge[1] = &rkTetra.m_apkEdge[1];
    rkTetra.m_apkTriangle[0].m_apkEdge[2] = &rkTetra.m_apkEdge[0];
    rkTetra.m_apkTriangle[0].m_apkAdjacent[0] = &rkTetra.m_apkTriangle[3];
    rkTetra.m_apkTriangle[0].m_apkAdjacent[1] = &rkTetra.m_apkTriangle[2];
    rkTetra.m_apkTriangle[0].m_apkAdjacent[2] = &rkTetra.m_apkTriangle[1];

    rkTetra.m_apkTriangle[1].m_apkVertex[0] = &rkTetra.m_apkVertex[0];
    rkTetra.m_apkTriangle[1].m_apkVertex[1] = &rkTetra.m_apkVertex[1];
    rkTetra.m_apkTriangle[1].m_apkVertex[2] = &rkTetra.m_apkVertex[3];
    rkTetra.m_apkTriangle[1].m_apkEdge[0] = &rkTetra.m_apkEdge[0];
    rkTetra.m_apkTriangle[1].m_apkEdge[1] = &rkTetra.m_apkEdge[4];
    rkTetra.m_apkTriangle[1].m_apkEdge[2] = &rkTetra.m_apkEdge[3];
    rkTetra.m_apkTriangle[1].m_apkAdjacent[0] = &rkTetra.m_apkTriangle[0];
    rkTetra.m_apkTriangle[1].m_apkAdjacent[1] = &rkTetra.m_apkTriangle[2];
    rkTetra.m_apkTriangle[1].m_apkAdjacent[2] = &rkTetra.m_apkTriangle[3];

    rkTetra.m_apkTriangle[2].m_apkVertex[0] = &rkTetra.m_apkVertex[1];
    rkTetra.m_apkTriangle[2].m_apkVertex[1] = &rkTetra.m_apkVertex[2];
    rkTetra.m_apkTriangle[2].m_apkVertex[2] = &rkTetra.m_apkVertex[3];
    rkTetra.m_apkTriangle[2].m_apkEdge[0] = &rkTetra.m_apkEdge[1];
    rkTetra.m_apkTriangle[2].m_apkEdge[1] = &rkTetra.m_apkEdge[5];
    rkTetra.m_apkTriangle[2].m_apkEdge[2] = &rkTetra.m_apkEdge[4];
    rkTetra.m_apkTriangle[2].m_apkAdjacent[0] = &rkTetra.m_apkTriangle[0];
    rkTetra.m_apkTriangle[2].m_apkAdjacent[1] = &rkTetra.m_apkTriangle[3];
    rkTetra.m_apkTriangle[2].m_apkAdjacent[2] = &rkTetra.m_apkTriangle[1];

    rkTetra.m_apkTriangle[3].m_apkVertex[0] = &rkTetra.m_apkVertex[0];
    rkTetra.m_apkTriangle[3].m_apkVertex[1] = &rkTetra.m_apkVertex[3];
    rkTetra.m_apkTriangle[3].m_apkVertex[2] = &rkTetra.m_apkVertex[2];
    rkTetra.m_apkTriangle[3].m_apkEdge[0] = &rkTetra.m_apkEdge[3];
    rkTetra.m_apkTriangle[3].m_apkEdge[1] = &rkTetra.m_apkEdge[5];
    rkTetra.m_apkTriangle[3].m_apkEdge[2] = &rkTetra.m_apkEdge[2];
    rkTetra.m_apkTriangle[3].m_apkAdjacent[0] = &rkTetra.m_apkTriangle[1];
    rkTetra.m_apkTriangle[3].m_apkAdjacent[1] = &rkTetra.m_apkTriangle[2];
    rkTetra.m_apkTriangle[3].m_apkAdjacent[2] = &rkTetra.m_apkTriangle[0];

    // For testing purposes, but not necessary for the algorithm.  This
    // allows the display program to show the subdivision structure.
    for (int i = 0; i < 6; i++)
        rkTetra.m_apkEdge[i].m_iStep = 0;
}
//---------------------------------------------------------------------------
void CreateOctahedron (QuadricSurface::ConvexPolyhedron& rkOct)
{
    rkOct.m_iNumVertices = 6;
    rkOct.m_apkVertex = new QuadricSurface::Vertex[6];
    rkOct.m_iNumEdges = 12;
    rkOct.m_apkEdge = new QuadricSurface::Edge[12];
    rkOct.m_iNumTriangles = 8;
    rkOct.m_apkTriangle = new QuadricSurface::Triangle[8];

    // vertices
    rkOct.m_apkVertex[0].m_pkPoint = new Vector3;
    rkOct.m_apkVertex[0].m_pkPoint->x = 0.0f;
    rkOct.m_apkVertex[0].m_pkPoint->y = 0.0f;
    rkOct.m_apkVertex[0].m_pkPoint->z = 1.0f;
    rkOct.m_apkVertex[0].m_iNumEdges = 4;
    rkOct.m_apkVertex[0].m_apkEdge = new QuadricSurface::Edge*[4];
    rkOct.m_apkVertex[0].m_apkEdge[0] = &rkOct.m_apkEdge[0];
    rkOct.m_apkVertex[0].m_apkEdge[1] = &rkOct.m_apkEdge[1];
    rkOct.m_apkVertex[0].m_apkEdge[2] = &rkOct.m_apkEdge[2];
    rkOct.m_apkVertex[0].m_apkEdge[3] = &rkOct.m_apkEdge[3];

    rkOct.m_apkVertex[1].m_pkPoint = new Vector3;
    rkOct.m_apkVertex[1].m_pkPoint->x = 1.0f;
    rkOct.m_apkVertex[1].m_pkPoint->y = 0.0f;
    rkOct.m_apkVertex[1].m_pkPoint->z = 0.0f;
    rkOct.m_apkVertex[1].m_iNumEdges = 4;
    rkOct.m_apkVertex[1].m_apkEdge = new QuadricSurface::Edge*[4];
    rkOct.m_apkVertex[1].m_apkEdge[0] = &rkOct.m_apkEdge[0];
    rkOct.m_apkVertex[1].m_apkEdge[1] = &rkOct.m_apkEdge[4];
    rkOct.m_apkVertex[1].m_apkEdge[2] = &rkOct.m_apkEdge[7];
    rkOct.m_apkVertex[1].m_apkEdge[3] = &rkOct.m_apkEdge[8];

    rkOct.m_apkVertex[2].m_pkPoint = new Vector3;
    rkOct.m_apkVertex[2].m_pkPoint->x = 0.0f;
    rkOct.m_apkVertex[2].m_pkPoint->y = 1.0f;
    rkOct.m_apkVertex[2].m_pkPoint->z = 0.0f;
    rkOct.m_apkVertex[2].m_iNumEdges = 4;
    rkOct.m_apkVertex[2].m_apkEdge = new QuadricSurface::Edge*[4];
    rkOct.m_apkVertex[2].m_apkEdge[0] = &rkOct.m_apkEdge[1];
    rkOct.m_apkVertex[2].m_apkEdge[1] = &rkOct.m_apkEdge[4];
    rkOct.m_apkVertex[2].m_apkEdge[2] = &rkOct.m_apkEdge[5];
    rkOct.m_apkVertex[2].m_apkEdge[3] = &rkOct.m_apkEdge[9];

    rkOct.m_apkVertex[3].m_pkPoint = new Vector3;
    rkOct.m_apkVertex[3].m_pkPoint->x = -1.0f;
    rkOct.m_apkVertex[3].m_pkPoint->y = 0.0f;
    rkOct.m_apkVertex[3].m_pkPoint->z = 0.0f;
    rkOct.m_apkVertex[3].m_iNumEdges = 4;
    rkOct.m_apkVertex[3].m_apkEdge = new QuadricSurface::Edge*[4];
    rkOct.m_apkVertex[3].m_apkEdge[0] = &rkOct.m_apkEdge[2];
    rkOct.m_apkVertex[3].m_apkEdge[1] = &rkOct.m_apkEdge[5];
    rkOct.m_apkVertex[3].m_apkEdge[2] = &rkOct.m_apkEdge[6];
    rkOct.m_apkVertex[3].m_apkEdge[3] = &rkOct.m_apkEdge[10];

    rkOct.m_apkVertex[4].m_pkPoint = new Vector3;
    rkOct.m_apkVertex[4].m_pkPoint->x = 0.0f;
    rkOct.m_apkVertex[4].m_pkPoint->y = -1.0f;
    rkOct.m_apkVertex[4].m_pkPoint->z = 0.0f;
    rkOct.m_apkVertex[4].m_iNumEdges = 4;
    rkOct.m_apkVertex[4].m_apkEdge = new QuadricSurface::Edge*[4];
    rkOct.m_apkVertex[4].m_apkEdge[0] = &rkOct.m_apkEdge[3];
    rkOct.m_apkVertex[4].m_apkEdge[1] = &rkOct.m_apkEdge[6];
    rkOct.m_apkVertex[4].m_apkEdge[2] = &rkOct.m_apkEdge[7];
    rkOct.m_apkVertex[4].m_apkEdge[3] = &rkOct.m_apkEdge[11];

    rkOct.m_apkVertex[5].m_pkPoint = new Vector3;
    rkOct.m_apkVertex[5].m_pkPoint->x = 0.0f;
    rkOct.m_apkVertex[5].m_pkPoint->y = 0.0f;
    rkOct.m_apkVertex[5].m_pkPoint->z = -1.0f;
    rkOct.m_apkVertex[5].m_iNumEdges = 4;
    rkOct.m_apkVertex[5].m_apkEdge = new QuadricSurface::Edge*[4];
    rkOct.m_apkVertex[5].m_apkEdge[0] = &rkOct.m_apkEdge[8];
    rkOct.m_apkVertex[5].m_apkEdge[1] = &rkOct.m_apkEdge[9];
    rkOct.m_apkVertex[5].m_apkEdge[2] = &rkOct.m_apkEdge[10];
    rkOct.m_apkVertex[5].m_apkEdge[3] = &rkOct.m_apkEdge[11];

    // edges
    rkOct.m_apkEdge[0].m_apkVertex[0] = &rkOct.m_apkVertex[0];
    rkOct.m_apkEdge[0].m_apkVertex[1] = &rkOct.m_apkVertex[1];
    rkOct.m_apkEdge[0].m_apkTriangle[0] = &rkOct.m_apkTriangle[3];
    rkOct.m_apkEdge[0].m_apkTriangle[1] = &rkOct.m_apkTriangle[0];

    rkOct.m_apkEdge[1].m_apkVertex[0] = &rkOct.m_apkVertex[0];
    rkOct.m_apkEdge[1].m_apkVertex[1] = &rkOct.m_apkVertex[2];
    rkOct.m_apkEdge[1].m_apkTriangle[0] = &rkOct.m_apkTriangle[0];
    rkOct.m_apkEdge[1].m_apkTriangle[1] = &rkOct.m_apkTriangle[1];

    rkOct.m_apkEdge[2].m_apkVertex[0] = &rkOct.m_apkVertex[0];
    rkOct.m_apkEdge[2].m_apkVertex[1] = &rkOct.m_apkVertex[3];
    rkOct.m_apkEdge[2].m_apkTriangle[0] = &rkOct.m_apkTriangle[1];
    rkOct.m_apkEdge[2].m_apkTriangle[1] = &rkOct.m_apkTriangle[2];

    rkOct.m_apkEdge[3].m_apkVertex[0] = &rkOct.m_apkVertex[0];
    rkOct.m_apkEdge[3].m_apkVertex[1] = &rkOct.m_apkVertex[4];
    rkOct.m_apkEdge[3].m_apkTriangle[0] = &rkOct.m_apkTriangle[2];
    rkOct.m_apkEdge[3].m_apkTriangle[1] = &rkOct.m_apkTriangle[3];

    rkOct.m_apkEdge[4].m_apkVertex[0] = &rkOct.m_apkVertex[1];
    rkOct.m_apkEdge[4].m_apkVertex[1] = &rkOct.m_apkVertex[2];
    rkOct.m_apkEdge[4].m_apkTriangle[0] = &rkOct.m_apkTriangle[0];
    rkOct.m_apkEdge[4].m_apkTriangle[1] = &rkOct.m_apkTriangle[4];

    rkOct.m_apkEdge[5].m_apkVertex[0] = &rkOct.m_apkVertex[2];
    rkOct.m_apkEdge[5].m_apkVertex[1] = &rkOct.m_apkVertex[3];
    rkOct.m_apkEdge[5].m_apkTriangle[0] = &rkOct.m_apkTriangle[1];
    rkOct.m_apkEdge[5].m_apkTriangle[1] = &rkOct.m_apkTriangle[5];

    rkOct.m_apkEdge[6].m_apkVertex[0] = &rkOct.m_apkVertex[3];
    rkOct.m_apkEdge[6].m_apkVertex[1] = &rkOct.m_apkVertex[4];
    rkOct.m_apkEdge[6].m_apkTriangle[0] = &rkOct.m_apkTriangle[2];
    rkOct.m_apkEdge[6].m_apkTriangle[1] = &rkOct.m_apkTriangle[6];

    rkOct.m_apkEdge[7].m_apkVertex[0] = &rkOct.m_apkVertex[4];
    rkOct.m_apkEdge[7].m_apkVertex[1] = &rkOct.m_apkVertex[1];
    rkOct.m_apkEdge[7].m_apkTriangle[0] = &rkOct.m_apkTriangle[3];
    rkOct.m_apkEdge[7].m_apkTriangle[1] = &rkOct.m_apkTriangle[7];

    rkOct.m_apkEdge[8].m_apkVertex[0] = &rkOct.m_apkVertex[1];
    rkOct.m_apkEdge[8].m_apkVertex[1] = &rkOct.m_apkVertex[5];
    rkOct.m_apkEdge[8].m_apkTriangle[0] = &rkOct.m_apkTriangle[7];
    rkOct.m_apkEdge[8].m_apkTriangle[1] = &rkOct.m_apkTriangle[4];

    rkOct.m_apkEdge[9].m_apkVertex[0] = &rkOct.m_apkVertex[2];
    rkOct.m_apkEdge[9].m_apkVertex[1] = &rkOct.m_apkVertex[5];
    rkOct.m_apkEdge[9].m_apkTriangle[0] = &rkOct.m_apkTriangle[4];
    rkOct.m_apkEdge[9].m_apkTriangle[1] = &rkOct.m_apkTriangle[5];

    rkOct.m_apkEdge[10].m_apkVertex[0] = &rkOct.m_apkVertex[3];
    rkOct.m_apkEdge[10].m_apkVertex[1] = &rkOct.m_apkVertex[5];
    rkOct.m_apkEdge[10].m_apkTriangle[0] = &rkOct.m_apkTriangle[5];
    rkOct.m_apkEdge[10].m_apkTriangle[1] = &rkOct.m_apkTriangle[6];

    rkOct.m_apkEdge[11].m_apkVertex[0] = &rkOct.m_apkVertex[4];
    rkOct.m_apkEdge[11].m_apkVertex[1] = &rkOct.m_apkVertex[5];
    rkOct.m_apkEdge[11].m_apkTriangle[0] = &rkOct.m_apkTriangle[6];
    rkOct.m_apkEdge[11].m_apkTriangle[1] = &rkOct.m_apkTriangle[7];

    // triangles
    rkOct.m_apkTriangle[0].m_apkVertex[0] = &rkOct.m_apkVertex[0];
    rkOct.m_apkTriangle[0].m_apkVertex[1] = &rkOct.m_apkVertex[1];
    rkOct.m_apkTriangle[0].m_apkVertex[2] = &rkOct.m_apkVertex[2];
    rkOct.m_apkTriangle[0].m_apkEdge[0] = &rkOct.m_apkEdge[0];
    rkOct.m_apkTriangle[0].m_apkEdge[1] = &rkOct.m_apkEdge[4];
    rkOct.m_apkTriangle[0].m_apkEdge[2] = &rkOct.m_apkEdge[1];
    rkOct.m_apkTriangle[0].m_apkAdjacent[0] = &rkOct.m_apkTriangle[3];
    rkOct.m_apkTriangle[0].m_apkAdjacent[1] = &rkOct.m_apkTriangle[4];
    rkOct.m_apkTriangle[0].m_apkAdjacent[2] = &rkOct.m_apkTriangle[1];

    rkOct.m_apkTriangle[1].m_apkVertex[0] = &rkOct.m_apkVertex[0];
    rkOct.m_apkTriangle[1].m_apkVertex[1] = &rkOct.m_apkVertex[2];
    rkOct.m_apkTriangle[1].m_apkVertex[2] = &rkOct.m_apkVertex[3];
    rkOct.m_apkTriangle[1].m_apkEdge[0] = &rkOct.m_apkEdge[1];
    rkOct.m_apkTriangle[1].m_apkEdge[1] = &rkOct.m_apkEdge[5];
    rkOct.m_apkTriangle[1].m_apkEdge[2] = &rkOct.m_apkEdge[2];
    rkOct.m_apkTriangle[1].m_apkAdjacent[0] = &rkOct.m_apkTriangle[0];
    rkOct.m_apkTriangle[1].m_apkAdjacent[1] = &rkOct.m_apkTriangle[5];
    rkOct.m_apkTriangle[1].m_apkAdjacent[2] = &rkOct.m_apkTriangle[2];

    rkOct.m_apkTriangle[2].m_apkVertex[0] = &rkOct.m_apkVertex[0];
    rkOct.m_apkTriangle[2].m_apkVertex[1] = &rkOct.m_apkVertex[3];
    rkOct.m_apkTriangle[2].m_apkVertex[2] = &rkOct.m_apkVertex[4];
    rkOct.m_apkTriangle[2].m_apkEdge[0] = &rkOct.m_apkEdge[2];
    rkOct.m_apkTriangle[2].m_apkEdge[1] = &rkOct.m_apkEdge[6];
    rkOct.m_apkTriangle[2].m_apkEdge[2] = &rkOct.m_apkEdge[3];
    rkOct.m_apkTriangle[2].m_apkAdjacent[0] = &rkOct.m_apkTriangle[1];
    rkOct.m_apkTriangle[2].m_apkAdjacent[1] = &rkOct.m_apkTriangle[6];
    rkOct.m_apkTriangle[2].m_apkAdjacent[2] = &rkOct.m_apkTriangle[3];

    rkOct.m_apkTriangle[3].m_apkVertex[0] = &rkOct.m_apkVertex[0];
    rkOct.m_apkTriangle[3].m_apkVertex[1] = &rkOct.m_apkVertex[4];
    rkOct.m_apkTriangle[3].m_apkVertex[2] = &rkOct.m_apkVertex[1];
    rkOct.m_apkTriangle[3].m_apkEdge[0] = &rkOct.m_apkEdge[3];
    rkOct.m_apkTriangle[3].m_apkEdge[1] = &rkOct.m_apkEdge[7];
    rkOct.m_apkTriangle[3].m_apkEdge[2] = &rkOct.m_apkEdge[0];
    rkOct.m_apkTriangle[3].m_apkAdjacent[0] = &rkOct.m_apkTriangle[2];
    rkOct.m_apkTriangle[3].m_apkAdjacent[1] = &rkOct.m_apkTriangle[7];
    rkOct.m_apkTriangle[3].m_apkAdjacent[2] = &rkOct.m_apkTriangle[0];

    rkOct.m_apkTriangle[4].m_apkVertex[0] = &rkOct.m_apkVertex[5];
    rkOct.m_apkTriangle[4].m_apkVertex[1] = &rkOct.m_apkVertex[2];
    rkOct.m_apkTriangle[4].m_apkVertex[2] = &rkOct.m_apkVertex[1];
    rkOct.m_apkTriangle[4].m_apkEdge[0] = &rkOct.m_apkEdge[9];
    rkOct.m_apkTriangle[4].m_apkEdge[1] = &rkOct.m_apkEdge[4];
    rkOct.m_apkTriangle[4].m_apkEdge[2] = &rkOct.m_apkEdge[8];
    rkOct.m_apkTriangle[4].m_apkAdjacent[0] = &rkOct.m_apkTriangle[5];
    rkOct.m_apkTriangle[4].m_apkAdjacent[1] = &rkOct.m_apkTriangle[0];
    rkOct.m_apkTriangle[4].m_apkAdjacent[2] = &rkOct.m_apkTriangle[7];

    rkOct.m_apkTriangle[5].m_apkVertex[0] = &rkOct.m_apkVertex[5];
    rkOct.m_apkTriangle[5].m_apkVertex[1] = &rkOct.m_apkVertex[3];
    rkOct.m_apkTriangle[5].m_apkVertex[2] = &rkOct.m_apkVertex[2];
    rkOct.m_apkTriangle[5].m_apkEdge[0] = &rkOct.m_apkEdge[10];
    rkOct.m_apkTriangle[5].m_apkEdge[1] = &rkOct.m_apkEdge[5];
    rkOct.m_apkTriangle[5].m_apkEdge[2] = &rkOct.m_apkEdge[9];
    rkOct.m_apkTriangle[5].m_apkAdjacent[0] = &rkOct.m_apkTriangle[6];
    rkOct.m_apkTriangle[5].m_apkAdjacent[1] = &rkOct.m_apkTriangle[1];
    rkOct.m_apkTriangle[5].m_apkAdjacent[2] = &rkOct.m_apkTriangle[4];

    rkOct.m_apkTriangle[6].m_apkVertex[0] = &rkOct.m_apkVertex[5];
    rkOct.m_apkTriangle[6].m_apkVertex[1] = &rkOct.m_apkVertex[4];
    rkOct.m_apkTriangle[6].m_apkVertex[2] = &rkOct.m_apkVertex[3];
    rkOct.m_apkTriangle[6].m_apkEdge[0] = &rkOct.m_apkEdge[11];
    rkOct.m_apkTriangle[6].m_apkEdge[1] = &rkOct.m_apkEdge[6];
    rkOct.m_apkTriangle[6].m_apkEdge[2] = &rkOct.m_apkEdge[10];
    rkOct.m_apkTriangle[6].m_apkAdjacent[0] = &rkOct.m_apkTriangle[7];
    rkOct.m_apkTriangle[6].m_apkAdjacent[1] = &rkOct.m_apkTriangle[2];
    rkOct.m_apkTriangle[6].m_apkAdjacent[2] = &rkOct.m_apkTriangle[5];

    rkOct.m_apkTriangle[7].m_apkVertex[0] = &rkOct.m_apkVertex[5];
    rkOct.m_apkTriangle[7].m_apkVertex[1] = &rkOct.m_apkVertex[1];
    rkOct.m_apkTriangle[7].m_apkVertex[2] = &rkOct.m_apkVertex[4];
    rkOct.m_apkTriangle[7].m_apkEdge[0] = &rkOct.m_apkEdge[8];
    rkOct.m_apkTriangle[7].m_apkEdge[1] = &rkOct.m_apkEdge[7];
    rkOct.m_apkTriangle[7].m_apkEdge[2] = &rkOct.m_apkEdge[11];
    rkOct.m_apkTriangle[7].m_apkAdjacent[0] = &rkOct.m_apkTriangle[4];
    rkOct.m_apkTriangle[7].m_apkAdjacent[1] = &rkOct.m_apkTriangle[3];
    rkOct.m_apkTriangle[7].m_apkAdjacent[2] = &rkOct.m_apkTriangle[6];

    // For testing purposes, but not necessary for the algorithm.  This
    // allows the display program to show the subdivision structure.
    for (int i = 0; i < 12; i++)
        rkOct.m_apkEdge[i].m_iStep = 0;
}
//---------------------------------------------------------------------------
#ifdef PERSPECTIVE
void ProjectPoint (Vector3& rkPoint, Vector3& rkEye, Matrix3& rkRot,
    int& riX, int& riY)
{
    // perspective projection
    // projection plane is Dot(eye,(x,y,z)) = -4

    float fT = 8.0f/(4.0f-rkEye.Dot(rkPoint));
    Vector3 kProj = (1.0f-fT)*rkEye + fT*rkPoint;
    Vector3 kDir = kProj + rkEye;
    Vector3 kProd = kDir*rkRot;

    riX = WSIZE/4+int((WSIZE/8)*(kProd.x+2.0f));
    riY = WSIZE/4+int((WSIZE/8)*(kProd.y+2.0f));
}
#else
void ProjectPoint (Vector3& rkPoint, Vector3& rkEye, Matrix3& rkRot,
    int& riX, int& riY)
{
    // parallel projection
    // projection plane is Dot(eye,(x,y,z)) = -4

    Vector3 kDir = rkPoint + rkEye;
    Vector3 kProd = kDir*rkRot;

    riX = int((WSIZE/4)*(kProd.x+2.0f));
    riY = int((WSIZE/4)*(kProd.y+2.0f));
}
#endif
//---------------------------------------------------------------------------
void DrawPolyhedron (HDC hDC, Vector3& rkEye, Matrix3& rkRot,
    QuadricSurface::ConvexPolyhedron& rkPoly)
{
    static COLORREF color[4] =
    {
        RGB(0,0,0),
        RGB(255,0,0),
        RGB(0,255,0),
        RGB(0,0,255)
    };

    for (int i = 0; i < rkPoly.m_iNumEdges; i++)
    {
        int iIndex = rkPoly.m_apkEdge[i].m_iStep % 4;
        HPEN hPen = CreatePen(PS_SOLID,2,color[iIndex]);
        HGDIOBJ hOldPen = SelectObject(hDC,hPen);

        Vector3* pP0 = rkPoly.m_apkEdge[i].m_apkVertex[0]->m_pkPoint;
        int iX0, iY0;
        ProjectPoint(*pP0,rkEye,rkRot,iX0,iY0);

        Vector3* pP1 = rkPoly.m_apkEdge[i].m_apkVertex[1]->m_pkPoint;
        int iX1, iY1;
        ProjectPoint(*pP1,rkEye,rkRot,iX1,iY1);

        MoveToEx(hDC,iX0,iY0,NULL);
        LineTo(hDC,iX1,iY1);

        SelectObject(hDC,hOldPen);
        DeleteObject(hPen);
    }
}
//---------------------------------------------------------------------------
long FAR PASCAL 
WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static Vector3 s_kEye;
    static QuadricSurface::ConvexPolyhedron s_kPoly;
    static int s_iSteps = 0;
    static Matrix3 s_kRot;
    Matrix3 kIncr;

    switch ( message ) 
    {
    case WM_CREATE:
    {
        CreatePoly(s_kPoly);
        QuadricSurface::TessellateSphere(s_iSteps,s_kPoly);

        // initial camera orientation and eye point
        s_kRot.FromAxisAngle(Vector3::UNIT_Z,0.0f);
        s_kEye = 2.0f*s_kRot.GetColumn(2);

        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWnd,&ps);
        DrawPolyhedron(hDC,s_kEye,s_kRot,s_kPoly);
        EndPaint(hWnd,&ps);
        return 0;
    }
    case WM_KEYDOWN:
    {
        switch ( wParam )
        {
        case VK_LEFT:
        {
            // rotate camera about its 'up' vector
            kIncr.FromAxisAngle(s_kRot.GetColumn(1),0.1f);
            s_kRot = kIncr*s_kRot;
            s_kEye = 2.0f*s_kRot.GetColumn(2);
            InvalidateRect(hWnd,NULL,TRUE);
            break;
        }
        case VK_RIGHT:
        {
            // rotate camera about its 'up' vector
            kIncr.FromAxisAngle(s_kRot.GetColumn(1),-0.1f);
            s_kRot = kIncr*s_kRot;
            s_kEye = 2.0f*s_kRot.GetColumn(2);
            InvalidateRect(hWnd,NULL,TRUE);
            break;
        }
        case VK_UP:
        {
            // rotate camera about its 'right' vector
            kIncr.FromAxisAngle(s_kRot.GetColumn(0),-0.1f);
            s_kRot = kIncr*s_kRot;
            s_kEye = 2.0f*s_kRot.GetColumn(2);
            InvalidateRect(hWnd,NULL,TRUE);
            break;
        }
        case VK_DOWN:
        {
            // rotate camera about its 'right' vector
            kIncr.FromAxisAngle(s_kRot.GetColumn(0),0.1f);
            s_kRot = kIncr*s_kRot;
            s_kEye = 2.0f*s_kRot.GetColumn(2);
            InvalidateRect(hWnd,NULL,TRUE);
            break;
        }
        }
        return 0;
    }
    case WM_CHAR:
    {
        switch ( wParam )
        {
            case '+':
            case '=':
            {
                s_iSteps++;
                QuadricSurface::DeletePolyhedron(s_kPoly);
                CreatePoly(s_kPoly);
                QuadricSurface::TessellateSphere(s_iSteps,s_kPoly);
                InvalidateRect(hWnd,NULL,TRUE);
                break;
            }
            case '-':
            case '_':
            {
                if ( s_iSteps >= 1 )
                {
                    s_iSteps--;
                    QuadricSurface::DeletePolyhedron(s_kPoly);
                    CreatePoly(s_kPoly);
                    QuadricSurface::TessellateSphere(s_iSteps,s_kPoly);
                    InvalidateRect(hWnd,NULL,TRUE);
                }
                break;
            }
            case 'q':
            case 'Q':
            case VK_ESCAPE:
            {
                PostMessage(hWnd,WM_DESTROY,0,0);
                break;
            }
        }
        return 0;
    }
    case WM_DESTROY:
    {
        QuadricSurface::DeletePolyhedron(s_kPoly);
        PostQuitMessage(0);
        return 0;
    }
    }
	return DefWindowProc(hWnd,message,wParam,lParam);
}
//---------------------------------------------------------------------------
int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpszCmdLine, int nCmdShow)
{
    static char szAppName[] = "Quadric Surface";

    WNDCLASS wc;
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL,IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = szAppName;
    RegisterClass(&wc);

    RECT rect = { 0, 0, WSIZE-1, WSIZE-1 };
    AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE);

    HWND hWnd = CreateWindow (
        szAppName,
        szAppName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right-rect.left+1,
        rect.bottom-rect.top+1,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hWnd,nCmdShow);
    UpdateWindow(hWnd);

    MSG kMsg;
    while ( TRUE )
    {
        if ( PeekMessage(&kMsg,NULL,0,0,PM_REMOVE) )
        {
            if ( kMsg.message == WM_QUIT )
                break;

            HACCEL hAccel = 0;
            if ( !TranslateAccelerator(hWnd,hAccel,&kMsg) )
            {
                TranslateMessage(&kMsg);
                DispatchMessage(&kMsg);
            }
        }
    }
    return kMsg.wParam;
}
//---------------------------------------------------------------------------


