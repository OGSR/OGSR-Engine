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
using namespace Mgc;

#include <algorithm>
#include <cmath>
using namespace std;

Real ConvexHull3D::ms_fVertexEqualityEpsilon = 0.0f;
Real ConvexHull3D::ms_fCollinearEpsilon = 1e-06f;
Real ConvexHull3D::ms_fCoplanarEpsilon = 1e-06f;

//----------------------------------------------------------------------------
ConvexHull3D::ConvexHull3D (int iVQuantity, const Vector3* akVertex)
{
    m_iVQuantity = iVQuantity;
    m_akVertex = akVertex;
    m_iHQuantity = 0;
    m_aiHIndex = NULL;
}
//----------------------------------------------------------------------------
ConvexHull3D::~ConvexHull3D ()
{
    delete[] m_aiHIndex;
}
//----------------------------------------------------------------------------
bool ConvexHull3D::SortedVertex::operator== (const SortedVertex& rkSV) const
{
    return m_kV == rkSV.m_kV;
}
//----------------------------------------------------------------------------
bool ConvexHull3D::SortedVertex::operator< (const SortedVertex& rkSV) const
{
    Real fZTmp = rkSV.m_kV.z;
    if ( Math::FAbs(m_kV.z - fZTmp) <= Vector3::FUZZ )
        fZTmp = m_kV.z;

    if ( m_kV.z < fZTmp )
        return true;
    else if ( m_kV.z > fZTmp )
        return false;

    Real fYTmp = rkSV.m_kV.y;
    if ( Math::FAbs(m_kV.y - fYTmp) <= Vector3::FUZZ )
        fYTmp = m_kV.y;

    if ( m_kV.y < fYTmp )
        return true;
    else if ( m_kV.y > fYTmp )
        return false;

    Real fXTmp = rkSV.m_kV.x;
    if ( Math::FAbs(m_kV.x - fXTmp) <= Vector3::FUZZ )
        fXTmp = m_kV.x;

    return m_kV.x < fXTmp;
}
//----------------------------------------------------------------------------
int ConvexHull3D::CollinearTest (const Vector3& rkP, const Vector3& rkQ0,
    const Vector3& rkQ1) const
{
    Vector3 kD = rkQ1 - rkQ0; 
    Vector3 kA = rkP - rkQ0;
    Vector3 kCross = kD.Cross(kA);
    Real fDdD = kD.SquaredLength();
    Real fCross2 = kCross.SquaredLength();
    Real fRelative;

    if ( m_iHullType == HULL_PLANAR )
    {
        Real fDet = m_kPlaneNormal.Dot(kCross);
        fRelative = fDet*fDet - ms_fCollinearEpsilon*fCross2;
        if ( fRelative > 0.0f )
        {
            if ( fDet > 0.0f )
                return ORDER_POSITIVE;
            if ( fDet < 0.0f )
                return ORDER_NEGATIVE;
        }
    }
    else
    {
        if ( fCross2 != 0.0f )
        {
            Real fAdA = kA.SquaredLength();
            fRelative = fCross2*fCross2 - ms_fCollinearEpsilon*fDdD*fAdA;
            if ( fRelative > 0.0f )
            {
                // points form a triangle <P,Q0,Q1>
                return ORDER_TRIANGLE;
            }
        }
    }

    // P is on line of <Q0,Q1>
    Real fDdA = kD.Dot(kA);
    if ( fDdA < 0.0f )
    {
        // order is <P,Q0,Q1>
        return ORDER_COLLINEAR_LEFT;
    }

    if ( fDdA > fDdD )
    {
        // order is <Q0,Q1,P>
        return ORDER_COLLINEAR_RIGHT;
    }

    // order is <Q0,P,Q1>
    return ORDER_COLLINEAR_CONTAIN;
}
//----------------------------------------------------------------------------
int ConvexHull3D::CoplanarTest (const Vector3& rkP, const Vector3& rkQ0,
    const Vector3& rkQ1, const Vector3& rkQ2) const
{
    // assert:  Q0, Q1, and Q2 are not collinear
    Vector3 kE0 = rkQ0 - rkP, kE1 = rkQ1 - rkP, kE2 = rkQ2 - rkP;
    kE0.Unitize();
    kE1.Unitize();
    kE2.Unitize();
    Vector3 kE1xE2 = kE1.UnitCross(kE2);
    Real fOrder = kE0.Dot(kE1xE2);
    if ( fOrder > 0.0f )
        return ORDER_POSITIVE;
    else
        return ORDER_NEGATIVE;

    // P is in the plane of triangle <Q0,Q1,Q2>.  Now determine if P is inside
    // or outside the triangle.  Use the signs of the barycentric coordinates
    // for P = a0*Q0+a1*Q1+a2*Q2, a0+a1+a2=1.  This equation is solved by:
    // P-Q2 = a0*Q0+a1*Q1+(a2-1)*Q2 = a0*(Q0-Q2)+a1*(Q1-Q2) = a0*E0+a1*E1.
    // The plane normal is N = Cross(E0,E1).  Let [U,V,W] = Dot(U,Cross(V,W)),
    // the triple scalar product.  Let |U| denote length of vectorU.  Then
    //   [N,P-Q2,E1] = a0*|N|^2
    //   [N,P-Q2,E0] = -a1*|N|^2
    //   a2 = 1 - a0 - a1
    // Avoid the divisions in solving for a0 and a1 and compute instead
    //   b0 = a0*|N|^2 = [N,P-Q2,E1]
    //   b1 = a1*|N|^2 = [N,E0,P-Q2]
    //   b2 = a2*|N|^2 = |N|^2 - b0 - b1

    kE0 = rkQ0 - rkQ2;
    kE1 = rkQ1 - rkQ2;
    Vector3 kPmQ2 = rkP - rkQ2;
    Vector3 kN = kE0.Cross(kE1);
    Real fB0 = kN.Dot(kPmQ2.Cross(kE1));
    Real fB1 = kN.Dot(kE0.Cross(kPmQ2));
    Real fB2 = kN.SquaredLength() - fB0 - fB1;

    if ( fB0 >= 0.0f && fB1 >= 0.0f && fB2 >= 0.0f )
        return ORDER_COPLANAR_INSIDE;
    else
        return ORDER_COPLANAR_OUTSIDE;
}
//----------------------------------------------------------------------------
int ConvexHull3D::CoplanarTest (const Vector3& rkP) const
{
    Vector3 kDiff = rkP - m_kPlaneOrigin;
    kDiff.Unitize();
    Real fOrder = m_kPlaneNormal.Dot(kDiff);

    if ( fOrder > ms_fCoplanarEpsilon )
        return ORDER_POSITIVE;
    else if ( fOrder < -ms_fCoplanarEpsilon )
        return ORDER_NEGATIVE;

    return ORDER_COPLANAR;
}
//----------------------------------------------------------------------------
void ConvexHull3D::RemoveCollinear ()
{
    // Handle only planar hulls for now.  TO DO: How to handle collinear
    // points on edges when hull is spatial?  Do a RemoveCoplanar call first
    // to generate convex polygon faces.  Then apply RemoveCollinear to each
    // face.
    if ( m_iHullType != HULL_PLANAR )
        return;

    Real fSaveFuzz = Vector3::FUZZ;
    Vector3::FUZZ = ms_fVertexEqualityEpsilon;

    vector<int> kHull;
    kHull.reserve(m_iHQuantity);
    for (int i0 = m_iHQuantity-1, i1 = 0, i2 = 1; i1 < m_iHQuantity; /**/)
    {
        int iCT = CollinearTest(m_akVertex[m_aiHIndex[i0]],
            m_akVertex[m_aiHIndex[i1]],m_akVertex[m_aiHIndex[i2]]);

        if ( iCT == ORDER_TRIANGLE
        ||   iCT == ORDER_POSITIVE
        ||   iCT == ORDER_NEGATIVE )
        {
            // points are not collinear
            kHull.push_back(m_aiHIndex[i1]);
        }

        i0 = i1++;
        if ( ++i2 == m_iHQuantity )
            i2 = 0;
    }

    // construct index array for ordered vertices of convex hull
    m_iHQuantity = kHull.size();
    delete[] m_aiHIndex;
    m_aiHIndex = new int[m_iHQuantity];
    memcpy(m_aiHIndex,&kHull.front(),m_iHQuantity*sizeof(int));

    Vector3::FUZZ = fSaveFuzz;
}
//----------------------------------------------------------------------------
void ConvexHull3D::ByIncremental ()
{
    Real fSaveFuzz = Vector3::FUZZ;
    Vector3::FUZZ = ms_fVertexEqualityEpsilon;

    // Sort by z-component and store in contiguous array.  The sort is
    // O(N log N).
    SVArray kSVArray(m_iVQuantity);
    int i;
    for (i = 0; i < m_iVQuantity; i++)
    {
        kSVArray[i].m_kV = m_akVertex[i];
        kSVArray[i].m_iIndex = i;
    }
    sort(kSVArray.begin(),kSVArray.end());

    // remove duplicate points
    SVArray::iterator pkEnd = unique(kSVArray.begin(),kSVArray.end());
    kSVArray.erase(pkEnd,kSVArray.end());

    // Compute convex hull incrementally.  The first and second vertices in
    // the hull are managed separately until at least one triangle is formed.
    // At that point, a triangle mesh is used to store the hull.
    m_iHullType = HULL_POINT;
    m_kHullP.push_back(kSVArray[0]);
    for (i = 1; i < (int)kSVArray.size(); i++)
    {
        switch ( m_iHullType )
        {
        case HULL_POINT:
            m_iHullType = HULL_LINEAR;
            m_kHullP.push_back(kSVArray[i]);
            break;
        case HULL_LINEAR:
            MergeLinear(kSVArray[i]);
            break;
        case HULL_PLANAR:
            MergePlanar(kSVArray[i]);
            break;
        case HULL_SPATIAL:
            MergeSpatial(kSVArray[i]);
            break;
        }
    }

    // construct final hull representation
    if ( m_iHullType == HULL_SPATIAL )
    {
        m_iHQuantity = m_kHullS.GetTriangleQuantity();
        m_aiHIndex = new int[3*m_iHQuantity];
        const TriangleMesh::TMap& rkTMap = m_kHullS.GetTriangleMap();
        TriangleMesh::MTCIter pkTIter;
        i = 0;
        for (pkTIter = rkTMap.begin(); pkTIter != rkTMap.end(); pkTIter++)
        {
            const TriangleMesh::Triangle& rkT = pkTIter->first;
            m_aiHIndex[i++] = rkT.m_aiV[0];
            m_aiHIndex[i++] = rkT.m_aiV[1];
            m_aiHIndex[i++] = rkT.m_aiV[2];
        }
    }
    else
    {
        m_iHQuantity = m_kHullP.size();
        m_aiHIndex = new int[m_iHQuantity];
        for (i = 0; i < m_iHQuantity; i++)
            m_aiHIndex[i] = m_kHullP[i].m_iIndex;
        RemoveCollinear();
    }

    Vector3::FUZZ = fSaveFuzz;
}
//----------------------------------------------------------------------------
void ConvexHull3D::MergeLinear (const SortedVertex& rkP)
{
    switch ( CollinearTest(rkP.m_kV,m_kHullP[0].m_kV,m_kHullP[1].m_kV) )
    {
    case ORDER_TRIANGLE:
    {
        // merged hull is triangle <P,Q0,Q1>
        m_iHullType = HULL_PLANAR;
        m_kHullP.push_back(m_kHullP[1]);
        m_kHullP[1] = m_kHullP[0];
        m_kHullP[0] = rkP;

        Vector3 kEdge0 = m_kHullP[1].m_kV - rkP.m_kV;
        Vector3 kEdge1 = m_kHullP[2].m_kV - rkP.m_kV;
        m_kPlaneOrigin = rkP.m_kV;
        m_kPlaneNormal = kEdge0.UnitCross(kEdge1);
        break;
    }
    case ORDER_COLLINEAR_LEFT:
        // collinear order is <P,Q0,Q1>, merged hull is <P,Q1>
        m_kHullP[0] = rkP;
        break;
    case ORDER_COLLINEAR_RIGHT:
        // collinear order is <Q0,Q1,P>, merged hull is <Q0,P>
        m_kHullP[1] = rkP;
        break;
    // case ORDER_COLLINEAR_CONTAIN:
    //   collinear order is <Q0,P,Q1>, merged hull is <Q0,Q1> (no change)
    }
}
//----------------------------------------------------------------------------
void ConvexHull3D::MergePlanar (const SortedVertex& rkP)
{
    int iCT = CoplanarTest(rkP.m_kV);

    if ( iCT == ORDER_COPLANAR )
    {
        int iSize = m_kHullP.size();
        int i, iU, iL;

        // search counterclockwise for last visible vertex
        for (iU = 0, i = 1; iU < iSize; iU = i++)
        {
            if ( i == iSize )
                i = 0;

            iCT = CollinearTest(rkP.m_kV,m_kHullP[iU].m_kV,m_kHullP[i].m_kV);

            if ( iCT == ORDER_NEGATIVE )
                continue;
            if ( iCT == ORDER_POSITIVE || iCT == ORDER_COLLINEAR_LEFT )
                break;

            // iCT == ORDER_COLLINEAR_CONTAIN || iCT == ORDER_COLLINEAR_RIGHT
            return;
        }
        assert( iU < iSize );

        // search clockwise for last visible vertex
        for (iL = 0, i = iSize-1; i >= 0; iL = i--)
        {
            iCT = CollinearTest(rkP.m_kV,m_kHullP[i].m_kV,m_kHullP[iL].m_kV);

            if ( iCT == ORDER_NEGATIVE )
                continue;
            if ( iCT == ORDER_POSITIVE || iCT == ORDER_COLLINEAR_RIGHT )
                break;

            // iCT == ORDER_COLLINEAR_CONTAIN || iCT == ORDER_COLLINEAR_LEFT
            return;
        }
        assert( i >= 0 );

        // construct the counterclockwise-ordered merged-hull vertices
        SVArray kTmpHull;
        kTmpHull.push_back(rkP);
        while ( true )
        {
            kTmpHull.push_back(m_kHullP[iU]);
            if ( iU == iL )
                break;

            if ( ++iU == iSize )
                iU = 0;
        }
        assert( kTmpHull.size() > 2 );

        m_kHullP = kTmpHull;
        return;
    }

    // Hull is about to become spatial.  The current planar hull is a convex
    // polygon that must be tri-fanned to initialize the triangle mesh that
    // represents the final hull.  The triangle order is required to be
    // counterclockwise when viewed from outside the hull.
    m_iHullType = HULL_SPATIAL;

    int iSize = m_kHullP.size();
    int i0 = m_kHullP[0].m_iIndex;
    int i1 = m_kHullP[1].m_iIndex;
    int j, i2;
    m_iLastIndex = rkP.m_iIndex;

    if ( iCT == ORDER_POSITIVE )
    {
        // insert <P,Q[0],Q[1]>
        m_kHullS.InsertTriangle(m_iLastIndex,i0,i1);

        for (j = 2; j < iSize; i1 = i2, j++)
        {
            i2 = m_kHullP[j].m_iIndex;

            // insert <Q[0],Q[i+1],Q[i]>
            m_kHullS.InsertTriangle(i0,i2,i1);

            // insert <P,Q[i],Q[i+1]>
            m_kHullS.InsertTriangle(m_iLastIndex,i1,i2);
        }

        // insert <P,Q[n-1],Q[0]>
        m_kHullS.InsertTriangle(m_iLastIndex,i1,i0);
    }
    else  // ORDER_NEGATIVE
    {
        // insert <P,Q[1],Q[0]>
        m_kHullS.InsertTriangle(m_iLastIndex,i1,i0);

        for (j = 2; j < iSize; i1 = i2, j++)
        {
            i2 = m_kHullP[j].m_iIndex;

            // insert <Q[0],Q[i],Q[i+1]>
            m_kHullS.InsertTriangle(i0,i1,i2);

            // insert <P,Q[i+1],Q[i]>
            m_kHullS.InsertTriangle(m_iLastIndex,i2,i1);
        }

        // insert <P,Q[0],Q[n-1]>
        m_kHullS.InsertTriangle(m_iLastIndex,i0,i1);
    }
}
//----------------------------------------------------------------------------
void ConvexHull3D::MergeSpatial (const SortedVertex& rkP)
{
    // TO DO.  This can be made faster by finding the terminator (polyline
    // separating visible/invisible faces) in O(N^{1/2}) time by following a
    // linear path of visible (invisible) triangles until you find one an
    // adjacent one that is invisible (visible).  Then walk along the edges
    // of terminator.  For each edge, delete the old visible triangle and add
    // the new one sharing P.  Do *not* delete all visible triangles.  This
    // makes the algorithm O(N).  After a call to this function, you have the
    // current hull and those non-deleted triangles inside it.  After the
    // final call, you have the convex hull, but it contains a lot of the
    // non-deleted triangles inside.  At this point, traverse the triangle
    // mesh data structure and make a copy of the hull, but ignore of those
    // non-deleted triangles inside.  The traversal is O(N), but it is only
    // applied once (outside the incremental loop).  Finally, when you delete
    // the temporary mesh that was used during MergeSpatial calls, all of the
    // memory is delete in O(1) time.  If you rewrite to use your own triangle
    // mesh structure, make sure that the mesh delete does not iterate over
    // the triangles to do some specific work (for example, maybe you had
    // dynamically allocated extra data attached to each triangle that you
    // need to delete).  If you do the traversal, you are back to O(N^2)
    // because there are O(N) non-deleted triangles.  This terminator-based
    // approach makes the total algorithm O(N^{3/2}):  O(N log N) for the
    // initial sort is "smaller" than O(N^{3/2}).  O(N) for the loop, the
    // work inside the loop begin O(N^{1/2}).


    // remove triangles visible to P
    set<TriangleMesh::Triangle> kTSet;
    m_kHullS.GetTriangles(kTSet);
    set<TriangleMesh::Triangle>::iterator pkTIter;
    for (pkTIter = kTSet.begin(); pkTIter != kTSet.end(); pkTIter++)
    {
        const TriangleMesh::Triangle& rkT = *pkTIter;
        int iCT = CoplanarTest(rkP.m_kV,m_akVertex[rkT.m_aiV[0]],
            m_akVertex[rkT.m_aiV[1]],m_akVertex[rkT.m_aiV[2]]);
        if ( iCT == ORDER_NEGATIVE )
            m_kHullS.RemoveTriangle(rkT);
    }

    // insert triangles sharing P and boundary edges of mesh
    kTSet.clear();
    const TriangleMesh::EMap& rkEMap = m_kHullS.GetEdgeMap();
    TriangleMesh::MECIter pkEIter;
    for (pkEIter = rkEMap.begin(); pkEIter != rkEMap.end(); pkEIter++)
    {
        if ( pkEIter->second.m_kTSet.GetSize() == 1 )
        {
            // Determine the orientation of the edge relative to its
            // containing triangle.  This is necessary to make sure the
            // triangles sharing P are inserted into to the mesh in
            // counterclockwise order.
            const TriangleMesh::Triangle& rkT = pkEIter->second.m_kTSet[0];
            int iV0 = pkEIter->first.m_aiV[0];
            int iV1 = pkEIter->first.m_aiV[1];
            if ((rkT.m_aiV[0] == iV0 && rkT.m_aiV[1] == iV1)
            ||  (rkT.m_aiV[1] == iV0 && rkT.m_aiV[2] == iV1)
            ||  (rkT.m_aiV[2] == iV0 && rkT.m_aiV[0] == iV1))
            {
                // insert <P,V1,V0>
                kTSet.insert(TriangleMesh::Triangle(rkP.m_iIndex,iV1,iV0));
            }
            else
            {
                // insert <P,V0,V1>
                kTSet.insert(TriangleMesh::Triangle(rkP.m_iIndex,iV0,iV1));
            }
        }
    }

    for (pkTIter = kTSet.begin(); pkTIter != kTSet.end(); pkTIter++)
    {
        const TriangleMesh::Triangle& rkT = *pkTIter;
        m_kHullS.InsertTriangle(rkT);
    }
}
//----------------------------------------------------------------------------


