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

#include "MgcConvexClipper.h"
#include "MgcConvexPolyhedron.h"
#include <algorithm>
#include <fstream>
#include <map>
using namespace Mgc;
using namespace std;

//----------------------------------------------------------------------------
ConvexClipper::ConvexClipper (const ConvexPolyhedron& rkPoly, Real fEpsilon)
{
    m_fEpsilon = fEpsilon;

    const vector<Vector3>& rakPoint = rkPoly.GetPoints();
    int iVQuantity = rkPoly.GetVQuantity();
    m_akVertex.resize(iVQuantity);
    for (int iV = 0; iV < iVQuantity; iV++)
        m_akVertex[iV].m_kPoint = rakPoint[iV];

    int iEQuantity = rkPoly.GetEQuantity();
    m_akEdge.resize(iEQuantity);
    for (int iE = 0; iE < iEQuantity; iE++)
    {
        const MTEdge& rkE = rkPoly.GetEdge(iE);
        for (int i = 0; i < 2; i++)
        {
            m_akEdge[iE].m_aiVertex[i] = rkPoly.GetVLabel(rkE.GetVertex(i));
            m_akEdge[iE].m_aiFace[i] = rkE.GetTriangle(i);
        }
    }

    int iTQuantity = rkPoly.GetTQuantity();
    m_akFace.resize(iTQuantity);
    for (int iT = 0; iT < iTQuantity; iT++)
    {
        m_akFace[iT].m_kPlane = rkPoly.GetPlane(iT);

        const MTTriangle& rkT = rkPoly.GetTriangle(iT);
        for (int i = 0; i < 3; i++)
            m_akFace[iT].m_akEdge.insert(rkT.GetEdge(i));
    }
}
//----------------------------------------------------------------------------
int ConvexClipper::Clip (const Plane& rkPlane)
{
    // compute signed distances from vertices to plane
    int iPositive = 0, iNegative = 0;
    for (int iV = 0; iV < (int)m_akVertex.size(); iV++)
    {
        Vertex& rkV = m_akVertex[iV];
        if ( rkV.m_bVisible )
        {
            rkV.m_fDistance = rkPlane.DistanceTo(rkV.m_kPoint);
            if ( rkV.m_fDistance >= m_fEpsilon )
            {
                iPositive++;
            }
            else if ( rkV.m_fDistance <= -m_fEpsilon )
            {
                iNegative++;
                rkV.m_bVisible = false;
            }
            else
            {
                // The point is on the plane (within floating point
                // tolerance).
                rkV.m_fDistance = 0.0f;
            }
        }
    }

    if ( iPositive == 0 )
    {
        // mesh is in negative half-space, fully clipped
        return Plane::NEGATIVE_SIDE;
    }

    if ( iNegative == 0 )
    {
        // mesh is in positive half-space, fully visible
        return Plane::POSITIVE_SIDE;
    }

    // clip the visible edges
    for (int iE = 0; iE < (int)m_akEdge.size(); iE++)
    {
        Edge& rkE = m_akEdge[iE];
        if ( rkE.m_bVisible )
        {
            int iV0 = rkE.m_aiVertex[0], iV1 = rkE.m_aiVertex[1];
            int iF0 = rkE.m_aiFace[0], iF1 = rkE.m_aiFace[1];
            Face& rkF0 = m_akFace[iF0];
            Face& rkF1 = m_akFace[iF1];
            Real fD0 = m_akVertex[iV0].m_fDistance;
            Real fD1 = m_akVertex[iV1].m_fDistance;

            if ( fD0 <= 0.0f && fD1 <= 0.0f )
            {
                // The edge is culled.  If the edge is exactly on the clip
                // plane, it is possible that a visible triangle shares it.
                // The edge will be re-added during the face loop.
                rkF0.m_akEdge.erase(iE);
                if ( rkF0.m_akEdge.empty() )
                    rkF0.m_bVisible = false;

                rkF1.m_akEdge.erase(iE);
                if ( rkF1.m_akEdge.empty() )
                    rkF1.m_bVisible = false;

                rkE.m_bVisible = false;
                continue;
            }

            if ( fD0 >= 0.0f && fD1 >= 0.0f )
            {
                // face retains the edge
                continue;
            }

            // The edge is split by the plane.  Compute the point of
            // intersection.  If the old edge is <V0,V1> and I is the
            // intersection point, the new edge is <V0,I> when d0 > 0 or
            // <I,V1> when d1 > 0.
            int iNV = m_akVertex.size();
            m_akVertex.push_back(Vertex());
            Vertex& rkNV = m_akVertex[iNV];

            Vector3& rkP0 = m_akVertex[iV0].m_kPoint;
            Vector3& rkP1 = m_akVertex[iV1].m_kPoint;
            rkNV.m_kPoint = rkP0+(fD0/(fD0-fD1))*(rkP1-rkP0);

            if ( fD0 > 0.0f )
                rkE.m_aiVertex[1] = iNV;
            else
                rkE.m_aiVertex[0] = iNV;
        }
    }

    // The mesh straddles the plane.  A new convex polygonal face will be
    // generated.  Add it now and insert edges when they are visited.
    int iNF = m_akFace.size();
    m_akFace.push_back(Face());
    Face& rkNF = m_akFace[iNF];
    rkNF.m_kPlane = rkPlane;

    // process the faces
    for (int iF = 0; iF < iNF; iF++)
    {
        Face& rkF = m_akFace[iF];
        if ( rkF.m_bVisible )
        {
            // Determine if the face is on the negative side, the positive
            // side, or split by the clipping plane.  The m_iOccurs members
            // are set to zero to help find the end points of the polyline
            // that results from clipping a face.
            assert( rkF.m_akEdge.size() >= 2 );
            set<int>::iterator pkIter = rkF.m_akEdge.begin();
            while ( pkIter != rkF.m_akEdge.end() )
            {
                int iE = *pkIter++;
                Edge& rkE = m_akEdge[iE];
                assert( rkE.m_bVisible );
                m_akVertex[rkE.m_aiVertex[0]].m_iOccurs = 0;
                m_akVertex[rkE.m_aiVertex[1]].m_iOccurs = 0;
            }

            int iVStart, iVFinal;
            if ( GetOpenPolyline(rkF,iVStart,iVFinal) )
            {
                // polyline is open, close it up
                int iNE = m_akEdge.size();
                m_akEdge.push_back(Edge());
                Edge& rkNE = m_akEdge[iNE];

                rkNE.m_aiVertex[0] = iVStart;
                rkNE.m_aiVertex[1] = iVFinal;
                rkNE.m_aiFace[0] = iF;
                rkNE.m_aiFace[1] = iNF;

                // add new edge to polygons
                rkF.m_akEdge.insert(iNE);
                rkNF.m_akEdge.insert(iNE);
            }
        }
    }

    // Process face rkNF to make sure it is a simple polygon (theoretically
    // convex, but numerically may be slightly not convex).  Floating point
    // round-off errors can cause the new face from the last loop to be
    // needle-like with a collapse of two edges into a single edge.  This
    // block guarantees the invariant "face always a simple polygon".
    PostProcess(iNF,rkNF);
    int iESize = rkNF.m_akEdge.size();
    if ( iESize < 3 )
    {
        // face is completely degenerate, remove it from mesh
        m_akFace.pop_back();
    }

    return Plane::NO_SIDE;
}
//----------------------------------------------------------------------------
void ConvexClipper::PostProcess (int iNF, Face& rkNF)
{
    int i = 0, iEQuantity = rkNF.m_akEdge.size();
    vector<EdgePlus> kEdges(iEQuantity);
    set<int>::iterator pkIter = rkNF.m_akEdge.begin();
    while ( pkIter != rkNF.m_akEdge.end() )
    {
        int iE = *pkIter++;
        Edge& rkE = m_akEdge[iE];
        kEdges[i++] = EdgePlus(iE,m_akEdge[iE]);
    }
    assert( i == iEQuantity );

    sort(kEdges.begin(),kEdges.end());

    // process duplicate edges
    for (int i0 = 0, i1 = 1; i1 < iEQuantity; i0 = i1++)
    {
        if ( kEdges[i0] == kEdges[i1] )
        {
            // found two equivalent edges (same vertex end points)
#ifdef _DEBUG
            int i2 = i1+1;
            if ( i2 < iEQuantity )
            {
                // Make sure an edge occurs at most twice.  If not, then
                // algorithm needs to be modified to handle it.
                assert( kEdges[i1] != kEdges[i2] );
            }
#endif
            // Edge E0 has vertices V0, V1 and faces F0, NF.  Edge E1 has
            // vertices V0, V1 and faces F1, NF.
            int iE0 = kEdges[i0].m_iE;
            int iE1 = kEdges[i1].m_iE;
            Edge& rkE0 = m_akEdge[iE0];
            Edge& rkE1 = m_akEdge[iE1];

            // remove E0 and E1 from NF
            rkNF.m_akEdge.erase(iE0);
            rkNF.m_akEdge.erase(iE1);

            // remove NF from E0
            if ( rkE0.m_aiFace[0] == iNF )
            {
                rkE0.m_aiFace[0] = rkE0.m_aiFace[1];
            }
            else
            {
                assert( rkE0.m_aiFace[1] == iNF );
            }
            rkE0.m_aiFace[1] = -1;

            // remove NF from E1
            if ( rkE1.m_aiFace[0] == iNF )
            {
                rkE1.m_aiFace[0] = rkE1.m_aiFace[1];
            }
            else
            {
                assert( rkE1.m_aiFace[1] == iNF );
            }
            rkE1.m_aiFace[1] = -1;

            // E2 is being booted from the system.  Update the face F1 that
            // shares it.  Update E1 to share F1.
            int iF1 = rkE1.m_aiFace[0];
            Face& rkF1 = m_akFace[iF1];
            rkF1.m_akEdge.erase(iE1);
            rkF1.m_akEdge.insert(iE0);
            rkE0.m_aiFace[1] = iF1;
            rkE1.m_bVisible = false;
        }
    }
}
//----------------------------------------------------------------------------
bool ConvexClipper::GetOpenPolyline (Face& rkF, int& riVStart, int& riVFinal)
{
    // count the number of occurrences of each vertex in the polyline
    bool bOkay = true;
    set<int>::iterator pkIter = rkF.m_akEdge.begin();
    while ( pkIter != rkF.m_akEdge.end() )
    {
        int iE = *pkIter++;
        Edge& rkE = m_akEdge[iE];

        int iV0 = rkE.m_aiVertex[0];
        m_akVertex[iV0].m_iOccurs++;
        if ( m_akVertex[iV0].m_iOccurs > 2 )
            bOkay = false;

        int iV1 = rkE.m_aiVertex[1];
        m_akVertex[iV1].m_iOccurs++;
        if ( m_akVertex[iV1].m_iOccurs > 2 )
            bOkay = false;
    }

    if ( !bOkay )
    {
#ifdef _DEBUG
        // If you reach this block, there is a good chance that floating
        // point round-off error had caused this face to be needle-like and
        // what was theoretically a narrow V-shaped portion (a vertex shared
        // by two edges forming a small angle) has collapsed into a single
        // line segment.
        //
        // NOTE.  Once I added PostProcess, I have not gotten to this block.
        ofstream kOStr("error.txt");
        pkIter = rkF.m_akEdge.begin();
        while ( pkIter != rkF.m_akEdge.end() )
        {
            int iE = *pkIter++;
            Edge& rkE = m_akEdge[iE];
            kOStr << "e<" << iE << "> = <" << rkE.m_aiVertex[0] << ","
                  << rkE.m_aiVertex[1] << "|" << rkE.m_aiFace[0] << ","
                  << rkE.m_aiFace[1] << "> ";
            if ( rkE.m_bVisible )
                kOStr << "T" << endl;
            else
                kOStr << "F" << endl;
        }
        kOStr.close();

        assert( false );
#else
        return false;
#endif
    }

    // determine if the polyline is open
    pkIter = rkF.m_akEdge.begin();
    riVStart = -1;
    riVFinal = -1;
    while ( pkIter != rkF.m_akEdge.end() )
    {
        int iE = *pkIter++;
        Edge& rkE = m_akEdge[iE];

        int iV0 = rkE.m_aiVertex[0];
        if ( m_akVertex[iV0].m_iOccurs == 1 )
        {
            if ( riVStart == -1 )
            {
                riVStart = iV0;
            }
            else if ( riVFinal == -1 )
            {
                riVFinal = iV0;
            }
            else
            {
                // If you reach this assert, there is a good chance that the
                // polyhedron is not convex.  To check this, use the function
                // ValidateHalfSpaceProperty() on your polyhedron right after
                // you construct it.
                assert( false );
            }
        }

        int iV1 = rkE.m_aiVertex[1];
        if ( m_akVertex[iV1].m_iOccurs == 1 )
        {
            if ( riVStart == -1 )
            {
                riVStart = iV1;
            }
            else if ( riVFinal == -1 )
            {
                riVFinal = iV1;
            }
            else
            {
                // If you reach this assert, there is a good chance that the
                // polyhedron is not convex.  To check this, use the function
                // ValidateHalfSpaceProperty() on your polyhedron right after
                // you construct it.
                assert( false );
            }
        }
    }

    assert( (riVStart == -1 && riVFinal == -1)
        ||  (riVStart != -1 && riVFinal != -1) );

    return riVStart != -1;
}
//----------------------------------------------------------------------------
void ConvexClipper::OrderVertices (Face& rkF, vector<int>& aiVOrdered)
{
    // copy edge indices into contiguous memory
    int i = 0, iQuantity = rkF.m_akEdge.size();
    vector<int> aiEOrdered(iQuantity);
    set<int>::iterator pkIter = rkF.m_akEdge.begin();
    while ( pkIter != rkF.m_akEdge.end() )
        aiEOrdered[i++] = *pkIter++;

    // bubble sort (yes, it is...)
    for (int i0 = 0, i1 = 1, iChoice = 1; i1 < iQuantity-1; i0 = i1, i1++)
    {
        Edge& rkECurr = m_akEdge[aiEOrdered[i0]];
        int j, iCurr = rkECurr.m_aiVertex[iChoice];
        for (j = i1; j < iQuantity; j++)
        {
            Edge& rkETemp = m_akEdge[aiEOrdered[j]];
            int iSave;
            if ( rkETemp.m_aiVertex[0] == iCurr )
            {
                iSave = aiEOrdered[i1];
                aiEOrdered[i1] = aiEOrdered[j];
                aiEOrdered[j] = iSave;
                iChoice = 1;
                break;
            }
            if ( rkETemp.m_aiVertex[1] == iCurr )
            {
                iSave = aiEOrdered[i1];
                aiEOrdered[i1] = aiEOrdered[j];
                aiEOrdered[j] = iSave;
                iChoice = 0;
                break;
            }
        }
        assert( j < iQuantity );
    }

    aiVOrdered[0] = m_akEdge[aiEOrdered[0]].m_aiVertex[0];
    aiVOrdered[1] = m_akEdge[aiEOrdered[0]].m_aiVertex[1];
    for (i = 1; i < iQuantity; i++)
    {
        Edge& rkE = m_akEdge[aiEOrdered[i]];
        if ( rkE.m_aiVertex[0] == aiVOrdered[i] )
            aiVOrdered[i+1] = rkE.m_aiVertex[1];
        else
            aiVOrdered[i+1] = rkE.m_aiVertex[0];
    }
}
//----------------------------------------------------------------------------
void ConvexClipper::GetTriangles (vector<int>& raiConnect,
    vector<Plane>& rakPlane)
{
    for (int iF = 0; iF < (int)m_akFace.size(); iF++)
    {
        Face& rkF = m_akFace[iF];
        if ( rkF.m_bVisible )
        {
            int iQuantity = rkF.m_akEdge.size();
            assert( iQuantity >= 3 );
            vector<int> aiOrdered(iQuantity+1);
            OrderVertices(rkF,aiOrdered);

            int iV0 = aiOrdered[0], iV2 = aiOrdered[iQuantity-1];
            int iV1 = aiOrdered[(iQuantity-1) >> 1];
            Vector3 kD1 = m_akVertex[iV1].m_kPoint-m_akVertex[iV0].m_kPoint;
            Vector3 kD2 = m_akVertex[iV2].m_kPoint-m_akVertex[iV0].m_kPoint;
            Real fTSP = rkF.m_kPlane.Normal().Dot(kD1.Cross(kD2));
            if ( fTSP > 0.0f )
            {
                // clockwise, need to swap
                for (int i = 1; i+1 < iQuantity; i++)
                {
                    raiConnect.push_back(iV0);
                    raiConnect.push_back(aiOrdered[i+1]);
                    raiConnect.push_back(aiOrdered[i]);
                    rakPlane.push_back(rkF.m_kPlane);
                }
            }
            else
            {
                // counterclockwise
                for (int i = 1; i+1 < iQuantity; i++)
                {
                    raiConnect.push_back(iV0);
                    raiConnect.push_back(aiOrdered[i]);
                    raiConnect.push_back(aiOrdered[i+1]);
                    rakPlane.push_back(rkF.m_kPlane);
                }
            }
        }
    }
}
//----------------------------------------------------------------------------
void ConvexClipper::Convert (ConvexPolyhedron& rkPoly)
{
    // get visible vertices
    int iVQuantity = m_akVertex.size();
    vector<Vector3> akPoint;
    int* aiVMap = new int[iVQuantity];
    memset(aiVMap,0xFF,iVQuantity*sizeof(int));
    for (int iV = 0; iV < iVQuantity; iV++)
    {
        const Vertex& rkV = m_akVertex[iV];
        if ( rkV.m_bVisible )
        {
            aiVMap[iV] = akPoint.size();
            akPoint.push_back(rkV.m_kPoint);
        }
    }

    vector<int> aiConnect;
    vector<Plane> akPlane;
    GetTriangles(aiConnect,akPlane);

    // re-order
    int iNewVQuantity = akPoint.size();
    for (int iC = 0; iC < (int)aiConnect.size(); iC++)
    {
        int iOldC = aiConnect[iC];
        assert( 0 <= iOldC && iOldC < iVQuantity );
        int iNewC = aiVMap[iOldC];
        assert( 0 <= iNewC && iNewC < iNewVQuantity );
        aiConnect[iC] = iNewC;
    }

    delete[] aiVMap;

    rkPoly.Create(akPoint,aiConnect,akPlane);
}
//----------------------------------------------------------------------------
bool ConvexClipper::Print (const char* acFilename) const
{
    ofstream kOStr(acFilename);
    if ( !kOStr )
        return false;

    int iVQuantity = m_akVertex.size();
    int iEQuantity = m_akEdge.size();
    int iFQuantity = m_akFace.size();
    int iVDigits = (int)(ceil(log10((double)iVQuantity)));
    int iEDigits = (int)(ceil(log10((double)iEQuantity)));
    int iFDigits = (int)(ceil(log10((double)iFQuantity)));
    char acMsg[1024];

    kOStr << iVQuantity << " vertices" << endl;
    for (int iV = 0; iV < iVQuantity; iV++)
    {
        const Vertex& rkV = m_akVertex[iV];
        sprintf(acMsg,"v<%*d> %c: (%f,%f,%f)",
            iVDigits,iV,(rkV.m_bVisible ? 'T' : 'F'),
            rkV.m_kPoint.x,
            rkV.m_kPoint.y,
            rkV.m_kPoint.z);
        kOStr << acMsg << endl;
    }
    kOStr << endl;

    kOStr << iEQuantity << " edges" << endl;
    for (int iE = 0; iE < iEQuantity; iE++)
    {
        const Edge& rkE = m_akEdge[iE];
        sprintf(acMsg,"e<%*d> %c: v[%*d,%*d], t[%*d,%*d]",
            iEDigits,iE,(rkE.m_bVisible ? 'T' : 'F'),
            iVDigits,rkE.m_aiVertex[0],
            iVDigits,rkE.m_aiVertex[1],
            iFDigits,rkE.m_aiFace[0],
            iFDigits,rkE.m_aiFace[1]);
        kOStr << acMsg << endl;
    }
    kOStr << endl;

    kOStr << iFQuantity << " faces" << endl;
    for (int iF = 0; iF < iFQuantity; iF++)
    {
        const Face& rkF = m_akFace[iF];
        sprintf(acMsg,"t<%*d> %d: e = ",
            iFDigits,iF,(rkF.m_bVisible ? 'T' : 'F'));
        kOStr << acMsg;

        set<int>::const_iterator pkIter = rkF.m_akEdge.begin();
        while ( pkIter != rkF.m_akEdge.end() )
        {
            kOStr << *pkIter << ' ';
            pkIter++;
        }
        kOStr << endl;
    }

    return true;
}
//----------------------------------------------------------------------------
