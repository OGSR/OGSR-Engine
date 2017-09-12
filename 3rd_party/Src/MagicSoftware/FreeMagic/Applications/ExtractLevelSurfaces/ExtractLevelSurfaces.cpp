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
#include "ExtractTrilinear.h"
using namespace Mgc;

#include <fstream>
using namespace std;

//----------------------------------------------------------------------------
static void ExtractUsingTetrahedrons ()
{
    //static char gs_acImage[] = "molecule.im";
    //static char gs_acLevel[] = "molecule64.txt";
    //static int gs_iLevelValue = 64;
    static char gs_acImage[] = "head.im";
    static char gs_acLevel[] = "head280.txt";
    static int gs_iLevelValue = 280;

    ImageInterp3D kImage(gs_acImage);

    int iVQuantity = 0;
    ImageInterp3D::Vertex* akVertex = NULL;
    int iEQuantity = 0;
    ImageInterp3D::Edge* akEdge = NULL;
    int iTQuantity = 0;
    ImageInterp3D::Triangle* akTriangle = NULL;

    kImage.ExtractLevelSetLinear(gs_iLevelValue,iVQuantity,akVertex,
        iEQuantity,akEdge,iTQuantity,akTriangle);
    ofstream kOStr(gs_acLevel);

    kOStr << iVQuantity << endl;
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        Vector3 kPoint;
        akVertex[i].GetTriple(kPoint);
        kOStr << kPoint.x << ' ' << kPoint.y << ' ' << kPoint.z << endl;
    }

    kOStr << iTQuantity << endl;
    for (i = 0; i < iTQuantity; i++)
    {
        kOStr << akTriangle[i].m_i0 << ' ';
        kOStr << akTriangle[i].m_i1 << ' ';
        kOStr << akTriangle[i].m_i2 << endl;
    }

    delete[] akVertex;
    delete[] akEdge;
    delete[] akTriangle;
}
//----------------------------------------------------------------------------
static void ExtractUsingCubes ()
{
    ImageInt3D kImage("molecule.im");  // 97x97x116

    ExtractTrilinear kETImage(kImage.GetBound(0),kImage.GetBound(1),
        kImage.GetBound(2),(int*)kImage.GetData());

    V3Array kVA, kNA;
    T3Array kTA;

    kETImage.ExtractContour(64.5f,kVA,kTA);
    kETImage.MakeUnique(kVA,kTA);
    kETImage.OrientTriangles(kVA,kTA,false);
    kETImage.ComputeNormals(kVA,kTA,kNA);

    ofstream kOStr("vtdata.txt");
    int iVQuantity = (int)kVA.size();
    kOStr << iVQuantity << endl;
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        Vertex3& rkV = kVA[i];
        kOStr << rkV.x << " " << rkV.y << " " << rkV.z << endl;
    }
    kOStr << endl;

    for (i = 0; i < iVQuantity; i++)
    {
        Vertex3& rkN = kNA[i];
        kOStr << rkN.x << " " << rkN.y << " " << rkN.z << endl;
    }
    kOStr << endl;

    int iTQuantity = (int)kTA.size();
    kOStr << iTQuantity << endl;
    for (i = 0; i < iTQuantity; i++)
    {
        Triangle3& rkT = kTA[i];
        kOStr << rkT.i0 << " " << rkT.i1 << " " << rkT.i2 << endl;
    }
}
//----------------------------------------------------------------------------
int main ()
{
    ExtractUsingTetrahedrons();
    //ExtractUsingCubes();
    return 0;
}
//----------------------------------------------------------------------------
