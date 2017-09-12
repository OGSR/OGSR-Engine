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

#include "MgcConvexHull3D.h"
#include "MgcSeparatePointSets3D.h"
using namespace Mgc;

class EdgeNode
{
public:
    int m_iV0, m_iV1;
    EdgeNode* m_pkNext;
};

static const Real gs_fEpsilon = 1e-06f;

//----------------------------------------------------------------------------
static void AddEdge (EdgeNode*& rpkList, int iV0, int iV1)
{
    EdgeNode* pkNode;

    for (pkNode = rpkList; pkNode; pkNode = pkNode->m_pkNext)
    {
        if ( (pkNode->m_iV0 == iV0 && pkNode->m_iV1 == iV1)
        ||   (pkNode->m_iV0 == iV1 && pkNode->m_iV1 == iV0) )
        {
            return;
        }
    }

    pkNode = new EdgeNode;
    pkNode->m_iV0 = iV0;
    pkNode->m_iV1 = iV1;
    pkNode->m_pkNext = rpkList;
    rpkList = pkNode;
}
//----------------------------------------------------------------------------
static void DeleteList (EdgeNode* pkList)
{
    while ( pkList )
    {
        EdgeNode* pkSave = pkList->m_pkNext;
        delete pkList;
        pkList = pkSave;
    }
}
//----------------------------------------------------------------------------
static int OnSameSide (const Plane& rkPlane, int iTriangleQuantity,
    const int* aiConnect, const Vector3* akPoint)
{
    // test if all points on same side of plane (nx,ny,nz)*(x,y,z) = c
    int iPosSide = 0, iNegSide = 0;

    for (int iT = 0; iT < iTriangleQuantity; iT++)
    {
        for (int i = 0; i < 3; i++)
        {
            int iV = aiConnect[3*iT+i];;
            Real fC0 = rkPlane.Normal().Dot(akPoint[iV]);
            if ( fC0 > rkPlane.Constant() + gs_fEpsilon )
                iPosSide++;
            else if ( fC0 < rkPlane.Constant() - gs_fEpsilon )
                iNegSide++;
            
            if ( iPosSide && iNegSide )
            {
                // plane splits point set
                return 0;
            }
        }
    }

    return iPosSide ? +1 : -1;
}
//----------------------------------------------------------------------------
static int WhichSide (const Plane& rkPlane, int iTriangleQuantity,
    const int* aiConnect, const Vector3* akPoint)
{
    // establish which side of plane hull is on
    for (int iT = 0; iT < iTriangleQuantity; iT++)
    {
        for (int i = 0; i < 3; i++)
        {
            int iV = aiConnect[3*iT+i];
            Real fC0 = rkPlane.Normal().Dot(akPoint[iV]);
            if ( fC0 > rkPlane.Constant()+gs_fEpsilon )  // positive side
                return +1;
            if ( fC0 < rkPlane.Constant()-gs_fEpsilon )  // negative side
                return -1;
        }
    }

    // hull is effectively collinear
    return 0;
}
//----------------------------------------------------------------------------
bool Mgc::SeparatePointSets3D (int iQuantity0, const Vector3* akVertex0,
    int iQuantity1, const Vector3* akVertex1, Plane& rkSeprPlane)
{
    // construct convex hull of point set 0
    ConvexHull3D kHull0(iQuantity0,akVertex0);
    assert( kHull0.GetType() == ConvexHull3D::HULL_SPATIAL );
    int iTriangleQuantity0 = kHull0.GetQuantity();
    const int* aiConnect0 = kHull0.GetIndices();

    // construct convex hull of point set 1
    ConvexHull3D kHull1(iQuantity1,akVertex1);
    assert( kHull1.GetType() == ConvexHull3D::HULL_SPATIAL );
    int iTriangleQuantity1 = kHull1.GetQuantity();
    const int* aiConnect1 = kHull1.GetIndices();

    // test faces of hull 0 for possible separation of points
    int i, iI0, iI1, iI2, iSide0, iSide1;
    Vector3 kDiff0, kDiff1;
    for (i = 0; i < iTriangleQuantity0; i++)
    {
        // lookup face (assert: iI0 != iI1 && iI0 != iI2 && iI1 != iI2)
        iI0 = aiConnect0[3*i  ];
        iI1 = aiConnect0[3*i+1];
        iI2 = aiConnect0[3*i+2];

        // compute potential separating plane (assert: normal != (0,0,0))
        kDiff0 = akVertex0[iI1] - akVertex0[iI0];
        kDiff1 = akVertex0[iI2] - akVertex0[iI0];
        rkSeprPlane.Normal() = kDiff0.Cross(kDiff1);
        rkSeprPlane.Constant() = rkSeprPlane.Normal().Dot(akVertex0[iI0]);

        // determine if hull 1 is on same side of plane
        iSide1 = OnSameSide(rkSeprPlane,iTriangleQuantity1,aiConnect1,
            akVertex1);
        if ( iSide1 )
        {
            // determine which side of plane hull 0 lies
            iSide0 = WhichSide(rkSeprPlane,iTriangleQuantity0,aiConnect0,
                akVertex0);
            if ( iSide0*iSide1 <= 0 )  // plane separates hulls
                return true;
        }
    }

    // test faces of hull 1 for possible separation of points
    for (i = 0; i < iTriangleQuantity1; i++)
    {
        // lookup edge (assert: iI0 != iI1 && iI0 != iI2 && iI1 != iI2)
        iI0 = aiConnect1[3*i  ];
        iI1 = aiConnect1[3*i+1];
        iI2 = aiConnect1[3*i+2];

        // compute perpendicular to face (assert: normal != (0,0,0))
        kDiff0 = akVertex1[iI1] - akVertex1[iI0];
        kDiff1 = akVertex1[iI2] - akVertex1[iI0];
        rkSeprPlane.Normal() = kDiff0.Cross(kDiff1);
        rkSeprPlane.Constant() = rkSeprPlane.Normal().Dot(akVertex1[iI0]);

        // determine if hull 0 is on same side of plane
        iSide0 = OnSameSide(rkSeprPlane,iTriangleQuantity0,aiConnect0,
            akVertex0);
        if ( iSide0 )
        {
            // determine which side of plane hull 1 lies
            iSide1 = WhichSide(rkSeprPlane,iTriangleQuantity1,aiConnect1,
                akVertex1);
            if ( iSide0*iSide1 <= 0 )  // plane separates hulls
                return true;
        }
    }

    // build edge list for hull 0
    EdgeNode* pkList0 = 0;
    for (i = 0; i < iTriangleQuantity0; i++)
    {
        // lookup face (assert: iI0 != iI1 && iI0 != iI2 && iI1 != iI2)
        iI0 = aiConnect0[3*i  ];
        iI1 = aiConnect0[3*i+1];
        iI2 = aiConnect0[3*i+2];
        AddEdge(pkList0,iI0,iI1);
        AddEdge(pkList0,iI0,iI2);
        AddEdge(pkList0,iI1,iI2);
    }

    // build edge list for hull 1
    EdgeNode* pkList1 = 0;
    for (i = 0; i < iTriangleQuantity1; i++)
    {
        // lookup face (assert: iI0 != iI1 && iI0 != iI2 && iI1 != iI2)
        iI0 = aiConnect1[3*i  ];
        iI1 = aiConnect1[3*i+1];
        iI2 = aiConnect1[3*i+2];
        AddEdge(pkList1,iI0,iI1);
        AddEdge(pkList1,iI0,iI2);
        AddEdge(pkList1,iI1,iI2);
    }

    // Test planes whose normals are cross products of two edges,
    // one from each hull.
    for (EdgeNode* pkNode0 = pkList0; pkNode0; pkNode0 = pkNode0->m_pkNext)
    {
        // get edge
        kDiff0 = akVertex0[pkNode0->m_iV1] - akVertex0[pkNode0->m_iV0];

        for (EdgeNode* pkNode1=pkList1; pkNode1; pkNode1 = pkNode1->m_pkNext)
        {
            kDiff1 = akVertex1[pkNode1->m_iV1] - akVertex1[pkNode1->m_iV0];

            // compute potential separating plane
            rkSeprPlane.Normal() = kDiff0.Cross(kDiff1);
            rkSeprPlane.Constant() = rkSeprPlane.Normal().Dot(
                akVertex0[pkNode0->m_iV0]);

            // determine if hull 0 is on same side of plane
            iSide0 = OnSameSide(rkSeprPlane,iTriangleQuantity0,aiConnect0,
                akVertex0);
            iSide1 = OnSameSide(rkSeprPlane,iTriangleQuantity1,aiConnect1,
                akVertex1);
            if ( iSide0*iSide1 < 0 )  // plane separates hulls
                return true;
        }
    }

    DeleteList(pkList0);
    DeleteList(pkList1);
    return false;
}
//----------------------------------------------------------------------------


