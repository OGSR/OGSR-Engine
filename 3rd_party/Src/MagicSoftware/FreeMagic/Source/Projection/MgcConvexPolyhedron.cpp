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

#include "MgcConvexPolyhedron.h"
#include "MgcConvexClipper.h"
#include <fstream>
#include <map>
#include <set>
using namespace Mgc;
using namespace std;

//----------------------------------------------------------------------------
ConvexPolyhedron::ConvexPolyhedron ()
{
}
//----------------------------------------------------------------------------
ConvexPolyhedron::ConvexPolyhedron (const vector<Vector3>& rakPoint,
    const vector<int>& raiConnect)
{
    Create(rakPoint,raiConnect);
}
//----------------------------------------------------------------------------
ConvexPolyhedron::ConvexPolyhedron (const vector<Vector3>& rakPoint,
    const vector<int>& raiConnect, const vector<Plane>& rakPlane)
{
    Create(rakPoint,raiConnect,rakPlane);
}
//----------------------------------------------------------------------------
ConvexPolyhedron::ConvexPolyhedron (const ConvexPolyhedron& rkPoly)
    :
    MTMesh(rkPoly),
    m_akPoint(rkPoly.m_akPoint),
    m_akPlane(rkPoly.m_akPlane)
{
}
//----------------------------------------------------------------------------
void ConvexPolyhedron::Create (const vector<Vector3>& rakPoint,
    const vector<int>& raiConnect)
{
    assert( rakPoint.size() >= 4 && raiConnect.size() >= 4 );

    int iVQuantity = rakPoint.size();
    int iTQuantity = raiConnect.size()/3;
    int iEQuantity = iVQuantity + iTQuantity - 2;
    Reset(iVQuantity,iEQuantity,iTQuantity);
    m_akPoint = rakPoint;

    // Copy polyhedron points into vertex array.  Compute centroid for use in
    // making sure the triangles are counterclockwise oriented when viewed
    // from the outside.
    ComputeCentroid();

    // get polyhedron edge and triangle information
    for (int iT = 0, iIndex = 0; iT < iTQuantity; iT++)
    {
        // get vertex indices for triangle
        int iV0 = raiConnect[iIndex++];
        int iV1 = raiConnect[iIndex++];
        int iV2 = raiConnect[iIndex++];

        // make sure triangle is counterclockwise
        Vector3& rkV0 = m_akPoint[iV0];
        Vector3& rkV1 = m_akPoint[iV1];
        Vector3& rkV2 = m_akPoint[iV2];

        Vector3 kDiff = m_kCentroid - rkV0;
        Vector3 kE1 = rkV1 - rkV0;
        Vector3 kE2 = rkV2 - rkV0;
        Vector3 kNormal = kE1.Cross(kE2);
        Real fLength = kNormal.Length();
        if ( fLength > 1e-06f )
        {
            kNormal /= fLength;
        }
        else
        {
            kNormal = kDiff;
            kNormal.Unitize();
        }

        Real fDistance = kNormal.Dot(kDiff);

        if ( fDistance < 0.0f )
        {
            // triangle is counterclockwise
            Insert(iV0,iV1,iV2);
        }
        else
        {
            // triangle is clockwise
            Insert(iV0,iV2,iV1);
        }
    }

    UpdatePlanes();
}
//----------------------------------------------------------------------------
void ConvexPolyhedron::Create (const vector<Vector3>& rakPoint,
    const vector<int>& raiConnect, const vector<Plane>& rakPlane)
{
    assert( rakPoint.size() >= 4 && raiConnect.size() >= 4 );

    int iVQuantity = rakPoint.size();
    int iTQuantity = raiConnect.size()/3;
    int iEQuantity = iVQuantity + iTQuantity - 2;
    Reset(iVQuantity,iEQuantity,iTQuantity);
    m_akPoint = rakPoint;
    m_akPlane = rakPlane;

    // Copy polyhedron points into vertex array.  Compute centroid for use in
    // making sure the triangles are counterclockwise oriented when viewed
    // from the outside.
    ComputeCentroid();

    // get polyhedron edge and triangle information
    for (int iT = 0, iIndex = 0; iT < iTQuantity; iT++)
    {
        // get vertex indices for triangle
        int iV0 = raiConnect[iIndex++];
        int iV1 = raiConnect[iIndex++];
        int iV2 = raiConnect[iIndex++];

        Real fDistance = m_akPlane[iT].DistanceTo(m_kCentroid);
        if ( fDistance > 0.0f )
        {
            // triangle is counterclockwise
            Insert(iV0,iV1,iV2);
        }
        else
        {
            // triangle is clockwise
            Insert(iV0,iV2,iV1);
        }
    }
}
//----------------------------------------------------------------------------
ConvexPolyhedron& ConvexPolyhedron::operator= (
    const ConvexPolyhedron& rkPoly)
{
    MTMesh::operator=(rkPoly);
    m_akPoint = rkPoly.m_akPoint;
    m_akPlane = rkPoly.m_akPlane;
    return *this;
}
//----------------------------------------------------------------------------
void ConvexPolyhedron::UpdatePlanes ()
{
    // The planes are constructed to have *inner pointing* normals.  This
    // supports my Wild Magic software clipping code that was based on a view
    // frustum having inner pointing normals.
    ComputeCentroid();
    int iTQuantity = m_akTriangle.GetQuantity();
    m_akPlane.resize(iTQuantity);
    for (int iT = 0; iT < iTQuantity; iT++)
    {
        MTTriangle& rkT = m_akTriangle[iT];
        int iV0 = GetVLabel(rkT.Vertex(0));
        int iV1 = GetVLabel(rkT.Vertex(1));
        int iV2 = GetVLabel(rkT.Vertex(2));
        Vector3& rkV0 = m_akPoint[iV0];
        Vector3& rkV1 = m_akPoint[iV1];
        Vector3& rkV2 = m_akPoint[iV2];

        Vector3 kDiff = m_kCentroid - rkV0;
        Vector3 kE1 = rkV1 - rkV0;
        Vector3 kE2 = rkV2 - rkV0;
        Vector3 kNormal = kE2.Cross(kE1);
        Real fLength = kNormal.Length();
        if ( fLength > 1e-06f )
        {
            kNormal /= fLength;
            Real fDot = kNormal.Dot(kDiff);
            if ( fDot < 0.0f )
                kNormal = -kNormal;
        }
        else
        {
            // triangle is degenerate, use "normal" that points towards
            // centroid
            kNormal = kDiff;
            kNormal.Unitize();
        }

        // inner pointing normal
        m_akPlane[iT] = Plane(kNormal,kNormal.Dot(rkV0));
    }
}
//----------------------------------------------------------------------------
bool ConvexPolyhedron::ValidateHalfSpaceProperty (Real fThreshold) const
{
    Real fMax = -Math::MAX_REAL, fMin = Math::MAX_REAL;
    for (int iT = 0; iT < m_akTriangle.GetQuantity(); iT++)
    {
        const Plane& rkPlane = m_akPlane[iT];
        for (int i = 0; i < (int)m_akPoint.size(); i++)
        {
            Real fDistance = rkPlane.DistanceTo(m_akPoint[i]);
            if ( fDistance < fMin )
                fMin = fDistance;
            if ( fDistance > fMax )
                fMax = fDistance;
            if ( fDistance < fThreshold )
                return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void ConvexPolyhedron::ComputeCentroid ()
{
    m_kCentroid = Vector3::ZERO;
    for (int iV = 0; iV < (int)m_akPoint.size(); iV++)
        m_kCentroid += m_akPoint[iV];
    m_kCentroid /= (Real)m_akPoint.size();
}
//----------------------------------------------------------------------------
bool ConvexPolyhedron::Clip (const Plane& rkPlane, ConvexPolyhedron& rkIntr)
    const
{
    ConvexClipper kClipper(*this);
    int iSide = kClipper.Clip(rkPlane);

    if ( iSide == Plane::POSITIVE_SIDE )
    {
        rkIntr = *this;
        return true;
    }

    if ( iSide == Plane::NEGATIVE_SIDE )
        return false;

    kClipper.Convert(rkIntr);
    return true;
}
//----------------------------------------------------------------------------
bool ConvexPolyhedron::FindIntersection (const ConvexPolyhedron& rkPoly,
    ConvexPolyhedron& rkIntr) const
{
    ConvexClipper kClipper(*this);

    const vector<Plane>& rakPlane = rkPoly.GetPlanes();
    for (int i = 0; i < (int)rakPlane.size(); i++)
    {
        if ( kClipper.Clip(rakPlane[i]) == Plane::NEGATIVE_SIDE )
            return false;
    }

    kClipper.Convert(rkIntr);
    return true;
}
//----------------------------------------------------------------------------
void ConvexPolyhedron::FindAllIntersections (int iQuantity,
    ConvexPolyhedron* akPoly, int& riCombos, ConvexPolyhedron**& rapkIntr)
{
    // Only 2^16 possible combinations for intersections are currently
    // supported.  If you need more, then GetHighBit(int) must be modified
    // to handle more than 16-bit inputs.
    if ( iQuantity <= 0 || iQuantity > 16 )
    {
        riCombos = 0;
        rapkIntr = NULL;
        return;
    }

    riCombos = (1 << iQuantity);
    bool* abNeedsTesting = new bool[riCombos];
    rapkIntr = new ConvexPolyhedron*[riCombos];
    int i;
    for (i = 0; i < riCombos; i++)
    {
        abNeedsTesting[i] = true;
        rapkIntr[i] = NULL;
    }

    // trivial cases, zero or one polyhedron--already the intersection
    abNeedsTesting[0] = false;
    for (i = 0; i < iQuantity; i++)
    {
        int j = (1 << i);
        abNeedsTesting[j] = false;
        rapkIntr[j] = new ConvexPolyhedron(akPoly[i]);
    }
    
    for (i = 3; i < riCombos; i++)
    {
        if ( abNeedsTesting[i] )
        {
            // In binary, i = b[m]...b[0] where b[m] is not zero (the
            // high-order bit.  Also, i1 = b[m-1]...b[0] is not zero
            // since, if it were, we would have ruled out the combination
            // by the j-loop below.  Therefore, i0 = b[m]0...0 and
            // i1 correspond to already existing polyhedra.  The
            // intersection finding just needs to look at the intersection
            // of the two polyhedra.
            int i0 = GetHighBit(i);
            int i1 = i & ~i0;
            rapkIntr[i] = FindSolidIntersection(*rapkIntr[i0],
                *rapkIntr[i1]);
            if ( !rapkIntr[i] )
            {
                // No intersection for this combination.  No need to test
                // other combinations that include this one.
                for (int j = 0; j < riCombos; j++)
                {
                    if ( (i & j) == i )
                        abNeedsTesting[j] = false;
                }
            }
#ifdef _DEBUG
            else  // test if well-formed convex polyhedron
            {
                Vector3 kCentroid = rapkIntr[i]->GetCentroid();
                bool bContains = rapkIntr[i]->ContainsPoint(kCentroid);
                assert( bContains );
            }
#endif
        }
    }

    delete[] abNeedsTesting;
}
//----------------------------------------------------------------------------
ConvexPolyhedron* ConvexPolyhedron::FindSolidIntersection (
    const ConvexPolyhedron& rkPoly0, const ConvexPolyhedron& rkPoly1)
{
    ConvexPolyhedron* pkIntr = new ConvexPolyhedron;
    if ( rkPoly0.FindIntersection(rkPoly1,*pkIntr) )
    {
        int iPointSize = pkIntr->m_akPoint.size();
        int iPlaneSize = pkIntr->m_akPlane.size();
        return pkIntr;
    }
    
    // As surfaces, the polyhedra do not intersect.  However, as solids,
    // one polyhedron might be fully contained in the other.
    if ( rkPoly0.ContainsPoint(rkPoly1.GetCentroid()) )
    {
        *pkIntr = rkPoly1;
        return pkIntr;
    }
        
    if ( rkPoly1.ContainsPoint(rkPoly0.GetCentroid()) )
    {
        *pkIntr = rkPoly0;
        return pkIntr;
    }

    delete pkIntr;
    return NULL;
}
//----------------------------------------------------------------------------
int ConvexPolyhedron::GetHighBit (int i)
{
    // assert: i in [1,2^16].  That is, (i>0) && (0xFFFF0000&i)==0.
    // This is a binary search for the high-order bit of i.
    if ( (i & 0xFF00) != 0 )
    {
        if ( (i & 0xF000) != 0 )
        {
            if ( (i & 0xC000) != 0 )
            {
                if ( (i & 0x8000) != 0 )
                    return 0x8000;
                else // (i & 0x4000) != 0
                    return 0x4000;
            }
            else  // (i & 0x3000) != 0
            {
                if ( (i & 0x2000) != 0 )
                    return 0x2000;
                else  // (i & 0x1000) != 0
                    return 0x1000;
            }
        }
        else  // (i & 0x0F00) != 0
        {
            if ( (i & 0x0C00) != 0 )
            {
                if ( (i & 0x0800) != 0 )
                    return 0x0800;
                else  // (i & 0x0400) != 0
                    return 0x0400;
            }
            else  // (i & 0x0300) != 0
            {
                if ( (i & 0x0200) != 0 )
                    return 0x0200;
                else  // (i & 0x0100) != 0
                    return 0x0100;
            }
        }
    }
    else  // (i & 0x00FF)
    {
        if ( (i & 0x00F0) != 0 )
        {
            if ( (i & 0x00C0) != 0 )
            {
                if ( (i & 0x0080) != 0 )
                    return 0x0080;
                else  // (i & 0x0040) != 0
                    return 0x0040;
            }
            else  // (i & 0x0030) != 0
            {
                if ( (i & 0x0020) != 0 )
                    return 0x0020;
                else  // (i & 0x0010) != 0
                    return 0x0010;
            }
        }
        else  // (i & 0x000F) != 0
        {
            if ( (i & 0x000C) != 0 )
            {
                if ( (i & 0x0008) != 0 )
                    return 0x0008;
                else  // (i & 0x0004) != 0
                    return 0x0004;
            }
            else  // (i & 0x0003) != 0
            {
                if ( (i & 0x0002) != 0 )
                    return 0x0002;
                else  // (i & 0x0001) != 0
                    return 0x0001;
            }
        }
    }
}
//----------------------------------------------------------------------------
Real ConvexPolyhedron::GetTriangleArea (const Vector3& rkN,
    const Vector3& rkV0, const Vector3& rkV1, const Vector3& rkV2) const
{
    // compute maximum absolute component of normal vector
    int iMax = 0;
    Real fMax = Math::FAbs(rkN.x);

    Real fAbs = Math::FAbs(rkN.y);
    if ( fAbs > fMax )
    {
        iMax = 1;
        fMax = fAbs;
    }

    fAbs = Math::FAbs(rkN.z);
    if ( fAbs > fMax )
    {
        iMax = 2;
        fMax = fAbs;
    }

    // catch degenerate triangles
    if ( fMax == 0.0f )
        return 0.0f;

    // compute area of projected triangle
    Real fD0, fD1, fD2, fArea;
    if ( iMax == 0 )
    {
        fD0 = rkV1.z - rkV2.z;
        fD1 = rkV2.z - rkV0.z;
        fD2 = rkV0.z - rkV1.z;
        fArea = Math::FAbs(rkV0.y*fD0 + rkV1.y*fD1 + rkV2.y*fD2);
    }
    else if ( iMax == 1 )
    {
        fD0 = rkV1.x - rkV2.x;
        fD1 = rkV2.x - rkV0.x;
        fD2 = rkV0.x - rkV1.x;
        fArea = Math::FAbs(rkV0.z*fD0 + rkV1.z*fD1 + rkV2.z*fD2);
    }
    else
    {
        fD0 = rkV1.y - rkV2.y;
        fD1 = rkV2.y - rkV0.y;
        fD2 = rkV0.y - rkV1.y;
        fArea = Math::FAbs(rkV0.x*fD0 + rkV1.x*fD1 + rkV2.x*fD2);
    }

    fArea *= 0.5f/fMax;
    return fArea;
}
//----------------------------------------------------------------------------
Real ConvexPolyhedron::GetSurfaceArea () const
{
    Real fSurfaceArea = 0.0f;

    for (int iT = 0; iT < m_akTriangle.GetQuantity(); iT++)
    {
        const MTTriangle& rkT = m_akTriangle.Get(iT);
        int iV0 = GetVLabel(rkT.GetVertex(0));
        int iV1 = GetVLabel(rkT.GetVertex(1));
        int iV2 = GetVLabel(rkT.GetVertex(2));
        const Vector3& rkV0 = m_akPoint[iV0];
        const Vector3& rkV1 = m_akPoint[iV1];
        const Vector3& rkV2 = m_akPoint[iV2];
        const Vector3& rkN = m_akPlane[iT].Normal();

        fSurfaceArea += GetTriangleArea(rkN,rkV0,rkV1,rkV2);
    }

    return fSurfaceArea;
}
//----------------------------------------------------------------------------
Real ConvexPolyhedron::GetVolume () const
{
    Real fVolume = 0.0f;

    for (int iT = 0; iT < m_akTriangle.GetQuantity(); iT++)
    {
        const MTTriangle& rkT = m_akTriangle.Get(iT);
        int iV0 = GetVLabel(rkT.GetVertex(0));
        int iV1 = GetVLabel(rkT.GetVertex(1));
        int iV2 = GetVLabel(rkT.GetVertex(2));
        const Vector3& rkV0 = m_akPoint[iV0];
        const Vector3& rkV1 = m_akPoint[iV1];
        const Vector3& rkV2 = m_akPoint[iV2];
        fVolume += rkV0.Dot(rkV1.Cross(rkV2));
    }

    fVolume /= 6.0f;
    return fVolume;
}
//----------------------------------------------------------------------------
bool ConvexPolyhedron::ContainsPoint (const Vector3& rkP) const
{
    for (int iT = 0; iT < m_akTriangle.GetQuantity(); iT++)
    {
        Real fDistance = m_akPlane[iT].DistanceTo(rkP);
        if ( fDistance < 0.0f )
            return false;
    }

    return true;
}
//----------------------------------------------------------------------------
Real ConvexPolyhedron::GetDistance (const Vector3& rkEye, int iT,
    vector<Real>& rafDistance) const
{
    // Signed distance from eye to plane of triangle.  When distance is
    // positive, triangle is visible from eye (front-facing).  When distance
    // is negative, triangle is not visible from eye (back-facing).  When
    // distance is zero, triangle is visible "on-edge" from eye.

    if ( rafDistance[iT] == Math::MAX_REAL )
    {
        rafDistance[iT] = -m_akPlane[iT].DistanceTo(rkEye);

        const Real fEpsilon = 1e-06f;
        if ( Math::FAbs(rafDistance[iT]) < fEpsilon )
            rafDistance[iT] = 0.0f;
    }

    return rafDistance[iT];
}
//----------------------------------------------------------------------------
bool ConvexPolyhedron::IsNegativeProduct (Real fDist0, Real fDist1)
{
    return (fDist0 != 0.0f ? (fDist0*fDist1 <= 0.0f) : (fDist1 != 0.0f));
}
//----------------------------------------------------------------------------
void ConvexPolyhedron::ComputeTerminator (const Vector3& rkEye,
    vector<Vector3>& rkTerminator)
{
    // temporary storage for signed distances from eye to triangles
    int iTQuantity = m_akTriangle.GetQuantity();
    vector<Real> afDistance(iTQuantity);
    int i, j;
    for (i = 0; i < iTQuantity; i++)
        afDistance[i] = Math::MAX_REAL;

    // Start a search for a front-facing triangle that has an adjacent
    // back-facing triangle or for a back-facing triangle that has an
    // adjacent front-facing triangle.
    int iTCurrent = 0;
    MTTriangle* pkTCurrent = &m_akTriangle[iTCurrent];
    Real fTriDist = GetDistance(rkEye,iTCurrent,afDistance);
    int iEFirst = -1;
    for (i = 0; i < iTQuantity; i++)
    {
        // Check adjacent neighbors for edge of terminator.  Such an
        // edge occurs if the signed distance changes sign.
        int iMinIndex = -1;
        Real fMinAbsDist = Math::MAX_REAL;
        Real afAdjDist[3];
        for (j = 0; j < 3; j++)
        {
            afAdjDist[j] = GetDistance(rkEye,pkTCurrent->Adjacent(j),
                afDistance);
            if ( IsNegativeProduct(fTriDist,afAdjDist[j]) )
            {
                iEFirst = pkTCurrent->Edge(j);
                break;
            }

            Real fAbsDist = Math::FAbs(afAdjDist[j]);
            if ( fAbsDist < fMinAbsDist )
            {
                fMinAbsDist = fAbsDist;
                iMinIndex = j;
            }
        }
        if ( j < 3 )
            break;

        // First edge not found during this iteration.  Move to adjacent
        // triangle whose distance is smallest of all adjacent triangles.
        iTCurrent = pkTCurrent->Adjacent(iMinIndex);
        pkTCurrent = &m_akTriangle[iTCurrent];
        fTriDist = afAdjDist[iMinIndex];
    }
    assert( i < iTQuantity );

    MTEdge& rkEFirst = m_akEdge[iEFirst];
    rkTerminator.push_back(m_akPoint[GetVLabel(rkEFirst.Vertex(0))]);
    rkTerminator.push_back(m_akPoint[GetVLabel(rkEFirst.Vertex(1))]);

    // walk along the terminator
    int iVFirst = rkEFirst.Vertex(0);
    int iV = rkEFirst.Vertex(1);
    int iE = iEFirst;
    int iEQuantity = m_akEdge.GetQuantity();
    for (i = 0; i < iEQuantity; i++)
    {
        // search all edges sharing the vertex for another terminator edge
        int j, jMax = m_akVertex[iV].GetEdgeQuantity();
        for (j = 0; j < m_akVertex[iV].GetEdgeQuantity(); j++)
        {
            int iENext = m_akVertex[iV].GetEdge(j);
            if ( iENext == iE )
                continue;

            Real fDist0 = GetDistance(rkEye,m_akEdge[iENext].GetTriangle(0),
                afDistance);
            Real fDist1 = GetDistance(rkEye,m_akEdge[iENext].GetTriangle(1),
                afDistance);
            if ( IsNegativeProduct(fDist0,fDist1) )
            {
                if ( m_akEdge[iENext].GetVertex(0) == iV )
                {
                    iV = m_akEdge[iENext].GetVertex(1);
                    rkTerminator.push_back(m_akPoint[GetVLabel(iV)]);
                    if ( iV == iVFirst )
                        return;
                }
                else
                {
                    iV = m_akEdge[iENext].GetVertex(0);
                    rkTerminator.push_back(m_akPoint[GetVLabel(iV)]);
                    if ( iV == iVFirst )
                        return;
                }

                iE = iENext;
                break;
            }
        }
        assert( j < jMax );
    }
    assert( i < iEQuantity );
}
//----------------------------------------------------------------------------
bool ConvexPolyhedron::ComputeSilhouette (const Vector3& rkEye,
    const Plane& rkPlane, const Vector3& rkU, const Vector3& rkV,
    vector<Vector2>& rkSilhouette)
{
    vector<Vector3> kTerminator;
    ComputeTerminator(rkEye,kTerminator);
    return ComputeSilhouette(kTerminator,rkEye,rkPlane,rkU,rkV,rkSilhouette);
}
//----------------------------------------------------------------------------
bool ConvexPolyhedron::ComputeSilhouette (vector<Vector3>& rkTerminator,
    const Vector3& rkEye, const Plane& rkPlane, const Vector3& rkU,
    const Vector3& rkV, vector<Vector2>& rkSilhouette)
{
    Real fEDist = rkPlane.DistanceTo(rkEye);  // assert:  fEDist > 0

    // closest planar point to E is K = E-dist*N
    Vector3 kClosest = rkEye - fEDist*rkPlane.Normal();

    // project polyhedron points onto plane
    for (int i = 0; i < (int)rkTerminator.size(); i++)
    {
        Vector3& rkPoint = rkTerminator[i];

        Real fVDist = rkPlane.DistanceTo(rkPoint);
        if ( fVDist >= fEDist )
        {
            // cannot project vertex onto plane
            return false;
        }

        // compute projected point Q
        Real fRatio = fEDist/(fEDist-fVDist);
        Vector3 kProjected = rkEye + fRatio*(rkPoint - rkEye);

        // compute (x,y) so that Q = K+x*U+y*V+z*N
        Vector3 kDiff = kProjected - kClosest;
        rkSilhouette.push_back(Vector2(rkU.Dot(kDiff),rkV.Dot(kDiff)));
    }

    return true;
}
//----------------------------------------------------------------------------
void ConvexPolyhedron::CreateEggShape (const Vector3& rkCenter, Real fX0,
    Real fX1, Real fY0, Real fY1, Real fZ0, Real fZ1, int iMaxSteps,
    ConvexPolyhedron& rkEgg)
{
    assert( fX0 > 0.0f && fX1 > 0.0f );
    assert( fY0 > 0.0f && fY1 > 0.0f );
    assert( fZ0 > 0.0f && fZ1 > 0.0f );
    assert( iMaxSteps >= 0 );

    // Start with an octahedron whose 6 vertices are (-x0,0,0), (x1,0,0),
    // (0,-y0,0), (0,y1,0), (0,0,-z0), (0,0,z1).  The center point will be
    // added later.
    vector<Vector3> akPoint(6);
    akPoint[0] = Vector3(-fX0,0.0f,0.0f);
    akPoint[1] = Vector3(fX1,0.0f,0.0f);
    akPoint[2] = Vector3(0.0f,-fY0,0.0f);
    akPoint[3] = Vector3(0.0f,fY1,0.0f);
    akPoint[4] = Vector3(0.0f,0.0f,-fZ0);
    akPoint[5] = Vector3(0.0f,0.0f,fZ1);

    vector<int> aiConnect(24);
    aiConnect[ 0] = 1;  aiConnect[ 1] = 3;  aiConnect[ 2] = 5;
    aiConnect[ 3] = 3;  aiConnect[ 4] = 0;  aiConnect[ 5] = 5;
    aiConnect[ 6] = 0;  aiConnect[ 7] = 2;  aiConnect[ 8] = 5;
    aiConnect[ 9] = 2;  aiConnect[10] = 1;  aiConnect[11] = 5;
    aiConnect[12] = 3;  aiConnect[13] = 1;  aiConnect[14] = 4;
    aiConnect[15] = 0;  aiConnect[16] = 3;  aiConnect[17] = 4;
    aiConnect[18] = 2;  aiConnect[19] = 0;  aiConnect[20] = 4;
    aiConnect[21] = 1;  aiConnect[22] = 2;  aiConnect[23] = 4;

    rkEgg.InitialELabel() = 0;
    rkEgg.Create(akPoint,aiConnect);

    // Subdivide the triangles.  The midpoints of the edges are computed.
    // The triangle is replaced by four sub triangles using the original 3
    // vertices and the 3 new edge midpoints.

    int i;
    for (int iStep = 1; iStep <= iMaxSteps; iStep++)
    {
        int iVQuantity = rkEgg.GetVQuantity();
        int iEQuantity = rkEgg.GetEQuantity();
        int iTQuantity = rkEgg.GetTQuantity();

        // compute lifted edge midpoints
        for (i = 0; i < iEQuantity; i++)
        {
            // get edge
            const MTEdge& rkE = rkEgg.GetEdge(i);
            int iV0 = rkEgg.GetVLabel(rkE.GetVertex(0));
            int iV1 = rkEgg.GetVLabel(rkE.GetVertex(1));

            // compute "lifted" centroid to points
            Vector3 kCen = rkEgg.Point(iV0)+rkEgg.Point(iV1);
            Real fXR = (kCen.x > 0.0f ? kCen.x/fX1 : kCen.x/fX0);
            Real fYR = (kCen.y > 0.0f ? kCen.y/fY1 : kCen.y/fY0);
            Real fZR = (kCen.z > 0.0f ? kCen.z/fZ1 : kCen.z/fZ0);
            kCen *= Math::InvSqrt(fXR*fXR+fYR*fYR+fZR*fZR);

            // Add the point to the array.  Store the point index in the edge
            // label for support in adding new triangles.
            rkEgg.ELabel(i) = iVQuantity++;
            rkEgg.AddPoint(kCen);
        }

        // Add the new triangles and remove the old triangle.  The removal
        // in slot i will cause the last added triangle to be moved to that
        // slot.  This side effect will not interfere with the iteration
        // and removal of the triangles.
        for (i = 0; i < iTQuantity; i++)
        {
            const MTTriangle& rkT = rkEgg.GetTriangle(i);
            int iV0 = rkEgg.GetVLabel(rkT.GetVertex(0));
            int iV1 = rkEgg.GetVLabel(rkT.GetVertex(1));
            int iV2 = rkEgg.GetVLabel(rkT.GetVertex(2));
            int iV01 = rkEgg.GetELabel(rkT.GetEdge(0));
            int iV12 = rkEgg.GetELabel(rkT.GetEdge(1));
            int iV20 = rkEgg.GetELabel(rkT.GetEdge(2));
            rkEgg.Insert(iV0,iV01,iV20);
            rkEgg.Insert(iV01,iV1,iV12);
            rkEgg.Insert(iV20,iV12,iV2);
            rkEgg.Insert(iV01,iV12,iV20);
            rkEgg.Remove(iV0,iV1,iV2);
        }
    }

    // add center
    for (i = 0; i < (int)rkEgg.m_akPoint.size(); i++)
        rkEgg.m_akPoint[i] += rkCenter;

    rkEgg.UpdatePlanes();
}
//----------------------------------------------------------------------------
void ConvexPolyhedron::Print (ofstream& rkOStr) const
{
    MTMesh::Print(rkOStr);

    int i;
    char acMsg[512];

    rkOStr << "points:" << endl;
    for (i = 0; i < (int)m_akPoint.size(); i++)
    {
        const Vector3& rkV = m_akPoint[i];
        sprintf(acMsg,"point<%d> = (%+8.4f,%+8.4f,%+8.4f)",i,rkV.x,rkV.y,
            rkV.z);
        rkOStr << acMsg << endl;
    }
    rkOStr << endl;

    rkOStr << "planes:" << endl;
    for (i = 0; i < (int)m_akPlane.size(); i++)
    {
        const Plane& rkP = m_akPlane[i];
        sprintf(acMsg,"plane<%d> = (%+8.6f,%+8.6f,%+8.6f;%+8.4f)",i,
            rkP.Normal().x,rkP.Normal().y,rkP.Normal().z,rkP.Constant());
        rkOStr << acMsg << endl;
    }
    rkOStr << endl;
}
//----------------------------------------------------------------------------
bool ConvexPolyhedron::Print (const char* acFilename) const
{
    ofstream kOStr(acFilename);
    if ( !kOStr )
        return false;

    Print(kOStr);
    return true;
}
//----------------------------------------------------------------------------
