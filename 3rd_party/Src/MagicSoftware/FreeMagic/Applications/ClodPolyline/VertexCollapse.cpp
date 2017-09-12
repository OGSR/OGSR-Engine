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

#include <cassert>
#include <cfloat>
#include "MgcDist3DVecLin.h"
#include "VertexCollapse.h"

//----------------------------------------------------------------------------
VertexCollapse::VertexCollapse (int iVQuantity, Vector3*& rakVertex,
    bool bClosed, int*& raiMap, int& riEQuantity, int*& raiEdge)
{
    raiMap = new int[iVQuantity];

    if ( bClosed )
    {
        riEQuantity = iVQuantity;
        raiEdge = new int[2*riEQuantity];

        if ( iVQuantity == 3 )
        {
            raiMap[0] = 0;
            raiMap[1] = 1;
            raiMap[2] = 3;
            raiEdge[0] = 0;  raiEdge[1] = 1;
            raiEdge[2] = 1;  raiEdge[3] = 2;
            raiEdge[4] = 2;  raiEdge[5] = 0;
            return;
        }
    }
    else
    {
        riEQuantity = iVQuantity-1;
        raiEdge = new int[2*riEQuantity];

        if ( iVQuantity == 2 )
        {
            raiMap[0] = 0;
            raiMap[1] = 1;
            raiEdge[0] = 0;  raiEdge[1] = 1;
            return;
        }
    }

    // create the heap of records
    InitializeHeap(iVQuantity,rakVertex,bClosed);
    BuildHeap();
    assert( IsValid() );

    // create the level of detail information for the polyline
    int* aiCollapse = new int[iVQuantity];
    CollapseVertices(iVQuantity,rakVertex,aiCollapse);
    ComputeEdges(iVQuantity,bClosed,aiCollapse,raiMap,riEQuantity,raiEdge);
    ReorderVertices(iVQuantity,rakVertex,aiCollapse,riEQuantity,raiEdge);
    delete[] aiCollapse;
}
//----------------------------------------------------------------------------
VertexCollapse::~VertexCollapse ()
{
    delete[] m_akRecord;
    delete[] m_apkHeap;
}
//----------------------------------------------------------------------------
float VertexCollapse::GetWeight (int iM, int iZ, int iP, Vector3* akVertex)
{
    Segment3 kSegment;
    kSegment.Origin() = akVertex[iM];
    kSegment.Direction() = akVertex[iP] - akVertex[iM];
    float fSqrDist = SqrDistance(akVertex[iZ],kSegment);
    float fSqrLen = kSegment.Direction().SquaredLength();

    return ( fSqrLen > 0.0f ? fSqrDist/fSqrLen : FLT_MAX );
}
//----------------------------------------------------------------------------
void VertexCollapse::InitializeHeap (int iVQuantity, Vector3* akVertex,
    bool bClosed)
{
    // Build the initial heap of weights, a max heap.  The weights are set
    // to negative values so that we get a min heap.  TO DO:  Modify the
    // code to directly implement a min heap.
    m_iHQuantity = iVQuantity;
    m_akRecord = new Record[m_iHQuantity];
    m_apkHeap = new Record*[m_iHQuantity];

    int i;
    for (i = 0; i < m_iHQuantity; i++)
    {
        m_akRecord[i].m_iVIndex = i;
        m_akRecord[i].m_iHIndex = i;
        m_akRecord[i].m_pkLAdj = &m_akRecord[(m_iHQuantity+i-1)%m_iHQuantity];
        m_akRecord[i].m_pkRAdj = &m_akRecord[(i+1)%m_iHQuantity];
        m_apkHeap[i] = &m_akRecord[i];
    }

    int iQm1 = m_iHQuantity - 1;
    if ( bClosed )
    {
        int iQm2 = m_iHQuantity - 2;
        m_akRecord[0].m_fWeight = GetWeight(iQm1,0,1,akVertex);
        m_akRecord[iQm1].m_fWeight = GetWeight(iQm2,iQm1,0,akVertex);
    }
    else
    {
        m_akRecord[0].m_fWeight = FLT_MAX;
        m_akRecord[iQm1].m_fWeight = FLT_MAX;
    }

    for (int iM = 0, iZ = 1, iP = 2; iZ < iQm1; iM++, iZ++, iP++)
        m_akRecord[iZ].m_fWeight = GetWeight(iM,iZ,iP,akVertex);
}
//----------------------------------------------------------------------------
void VertexCollapse::BuildHeap ()
{
    int iLast = m_iHQuantity - 1;
    for (int iLeft = iLast/2; iLeft >= 0; iLeft--)
    {
        Record* pkRecord = m_apkHeap[iLeft];
        int iPa = iLeft, iCh = 2*iLeft + 1;
        while ( iCh <= iLast )
        {
            if ( iCh < iLast )
            {
                if ( m_apkHeap[iCh]->m_fWeight > m_apkHeap[iCh+1]->m_fWeight )
                    iCh++;
            }

            if ( m_apkHeap[iCh]->m_fWeight >= pkRecord->m_fWeight )
                break;

            m_apkHeap[iCh]->m_iHIndex = iPa;
            m_apkHeap[iPa] = m_apkHeap[iCh];
            iPa = iCh;
            iCh = 2*iCh + 1;
        }

        pkRecord->m_iHIndex = iPa;
        m_apkHeap[iPa] = pkRecord;
    }
}
//----------------------------------------------------------------------------
int VertexCollapse::RemoveRoot (Vector3* akVertex)
{
    Record* pkRoot = m_apkHeap[0];

    int iLast = m_iHQuantity - 1;
    Record* pkRecord = m_apkHeap[iLast];
    int iPa = 0, iCh = 1;
    while ( iCh <= iLast )
    {
        if ( iCh < iLast )
        {
            if ( m_apkHeap[iCh]->m_fWeight > m_apkHeap[iCh+1]->m_fWeight )
                iCh++;
        }

        if ( m_apkHeap[iCh]->m_fWeight >= pkRecord->m_fWeight )
            break;

        m_apkHeap[iCh]->m_iHIndex = iPa;
        m_apkHeap[iPa] = m_apkHeap[iCh];
        iPa = iCh;
        iCh = 2*iCh + 1;
    }

    pkRecord->m_iHIndex = iPa;
    m_apkHeap[iPa] = pkRecord;
    m_iHQuantity--;

    // remove root from the doubly-linked list
    Record* pkLAdj = pkRoot->m_pkLAdj;
    Record* pkRAdj = pkRoot->m_pkRAdj;
    pkLAdj->m_pkRAdj = pkRAdj;
    pkRAdj->m_pkLAdj = pkLAdj;

    // update the weights of the vertices affected by the removal
    int iM, iZ, iP;
    float fWeight;

    if ( pkLAdj->m_fWeight != FLT_MAX )
    {
        iZ = pkLAdj->m_iVIndex;
        iM = pkLAdj->m_pkLAdj->m_iVIndex;
        iP = pkLAdj->m_pkRAdj->m_iVIndex;
        fWeight = GetWeight(iM,iZ,iP,akVertex);

        Update(pkLAdj->m_iHIndex,fWeight);
        assert( IsValid() );
    }

    if ( pkRAdj->m_fWeight != FLT_MAX )
    {
        iZ = pkRAdj->m_iVIndex;
        iM = pkRAdj->m_pkLAdj->m_iVIndex;
        iP = pkRAdj->m_pkRAdj->m_iVIndex;
        fWeight = GetWeight(iM,iZ,iP,akVertex);

        Update(pkRAdj->m_iHIndex,fWeight);
        assert( IsValid() );
    }

    return pkRoot->m_iVIndex;
}
//----------------------------------------------------------------------------
void VertexCollapse::Update (int iHIndex, float fWeight)
{
    Record* pkRecord = m_apkHeap[iHIndex];
    int iPa, iCh, iChP, iMaxCh;

    if ( fWeight > pkRecord->m_fWeight )
    {
        pkRecord->m_fWeight = fWeight;

        // new weight larger than old, propagate it towards the leaves
        iPa = iHIndex;
        iCh = 2*iPa+1;
        while ( iCh < m_iHQuantity )
        {
            // at least one child exists
            if ( iCh < m_iHQuantity-1 )
            {
                // two children exist
                iChP = iCh+1;
                if ( m_apkHeap[iCh]->m_fWeight <= m_apkHeap[iChP]->m_fWeight )
                    iMaxCh = iCh;
                else
                    iMaxCh = iChP;
            }
            else
            {
                // one child exists
                iMaxCh = iCh;
            }

            if ( m_apkHeap[iMaxCh]->m_fWeight >= fWeight )
                break;

            m_apkHeap[iMaxCh]->m_iHIndex = iPa;
            m_apkHeap[iPa] = m_apkHeap[iMaxCh];
            pkRecord->m_iHIndex = iMaxCh;
            m_apkHeap[iMaxCh] = pkRecord;
            iPa = iMaxCh;
            iCh = 2*iPa+1;
        }
    }
    else if ( fWeight < pkRecord->m_fWeight )
    {
        pkRecord->m_fWeight = fWeight;

        // new weight smaller than old, propagate it towards the root
        iCh = iHIndex;
        while ( iCh > 0 )
        {
            // a parent exists
            iPa = (iCh-1)/2;

            if ( m_apkHeap[iPa]->m_fWeight <= fWeight )
                break;

            m_apkHeap[iPa]->m_iHIndex = iCh;
            m_apkHeap[iCh] = m_apkHeap[iPa];
            pkRecord->m_iHIndex = iPa;
            pkRecord->m_fWeight = fWeight;
            m_apkHeap[iPa] = pkRecord;
            iCh = iPa;
        }
    }
}
//----------------------------------------------------------------------------
void VertexCollapse::CollapseVertices (int iVQuantity, Vector3* akVertex,
    int* aiCollapse)
{
    for (int i = iVQuantity-1; i >= 0; i--)
        aiCollapse[i] = RemoveRoot(akVertex);
}
//----------------------------------------------------------------------------
void VertexCollapse::ComputeEdges (int iVQuantity, bool bClosed,
    int* aiCollapse, int* aiMap, int iEQuantity, int* aiEdge)
{
    // Compute the edges (first to collapse is last in array).  Do not
    // collapse last line segment of open polyline.  Do not collapse last
    // triangle of closed polyline.
    int i, iVIndex, iEIndex = 2*iEQuantity-1;

    if ( bClosed )
    {
        for (i = iVQuantity-1; i >= 0; i--)
        {
            iVIndex = aiCollapse[i];
            aiEdge[iEIndex--] = (iVIndex+1) % iVQuantity;
            aiEdge[iEIndex--] = iVIndex;
        }
    }
    else
    {
        for (i = iVQuantity-1; i >= 2; i--)
        {
            iVIndex = aiCollapse[i];
            aiEdge[iEIndex--] = iVIndex+1;
            aiEdge[iEIndex--] = iVIndex;
        }

        iVIndex = aiCollapse[0];
        aiEdge[0] = iVIndex;
        aiEdge[1] = iVIndex+1;
    }

    // In the given edge order, find the index in the edge array that
    // corresponds to a collapse vertex and save the index for the dynamic
    // change in level of detail.  This relies on the assumption that a
    // vertex is shared by at most two edges.
    iEIndex = 2*iEQuantity-1;
    for (i = iVQuantity-1; i >= 0; i--)
    {
        iVIndex = aiCollapse[i];
        for (int iE = 0; iE < 2*iEQuantity; iE++)
        {
            if ( iVIndex == aiEdge[iE] )
            {
                aiMap[i] = iE;
                aiEdge[iE] = aiEdge[iEIndex];
                break;
            }
        }
        iEIndex -= 2;

        if ( bClosed )
        {
            if ( iEIndex == 5 )
                break;
        }
        else
        {
            if ( iEIndex == 1 )
                break;
        }
    }

    // restore the edge array to full level of detail
    if ( bClosed )
    {
        for (i = 3; i < iVQuantity; i++)
        {
            iVIndex = aiCollapse[i];
            aiEdge[aiMap[i]] = iVIndex;
        }
    }
    else
    {
        for (i = 2; i < iVQuantity; i++)
        {
            iVIndex = aiCollapse[i];
            aiEdge[aiMap[i]] = iVIndex;
        }
    }
}
//----------------------------------------------------------------------------
void VertexCollapse::ReorderVertices (int iVQuantity, Vector3*& rakVertex,
    int* aiCollapse, int iEQuantity, int* aiEdge)
{
    int* aiPermute = new int[iVQuantity];
    Vector3* akPVertex = new Vector3[iVQuantity];

    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        int iVIndex = aiCollapse[i];
        aiPermute[iVIndex] = i;
        akPVertex[i] = rakVertex[iVIndex];
    }

    for (i = 0; i < 2*iEQuantity; i++)
        aiEdge[i] = aiPermute[aiEdge[i]];

    delete[] rakVertex;
    rakVertex = akPVertex;

    delete[] aiPermute;
}
//----------------------------------------------------------------------------
bool VertexCollapse::IsValid (int iStart, int iFinal)
{
    for (int iC = iStart; iC <= iFinal; iC++)
    {
        int iP = (iC-1)/2;
        if ( iP > iStart )
        {
            if ( m_apkHeap[iP]->m_fWeight > m_apkHeap[iC]->m_fWeight )
                return false;

            if ( m_apkHeap[iP]->m_iHIndex != iP )
                return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool VertexCollapse::IsValid ()
{
    return IsValid(0,m_iHQuantity-1);
}
//----------------------------------------------------------------------------


