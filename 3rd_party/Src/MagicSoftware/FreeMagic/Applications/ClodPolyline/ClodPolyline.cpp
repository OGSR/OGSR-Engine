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

#include "ClodPolyline.h"
#include "MgcMath.h"
#include "MgcVector3.h"

const int iSize = 256;
ClodPolyline g_kTheApp("ClodPolyline",iSize,iSize);

//----------------------------------------------------------------------------
ClodPolyline::ClodPolyline (char* acCaption, int iWidth, int iHeight,
    unsigned int uiMenuID, unsigned int uiStatusPaneQuantity)
    :
    FWinApplication(acCaption,iWidth,iHeight,uiMenuID,uiStatusPaneQuantity)
{
    m_pkPolyline = NULL;
}
//----------------------------------------------------------------------------
ClodPolyline::~ClodPolyline ()
{
}
//----------------------------------------------------------------------------
bool ClodPolyline::OnInitialize ()
{
    // generate points on unit circle, then adjust the distances to center
    int iVQuantity = 16;
    Vector3* akVertex = new Vector3[iVQuantity];
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        float fAngle = Math::TWO_PI*i/iVQuantity;
        akVertex[i].x = cosf(fAngle);
        akVertex[i].y = sinf(fAngle);
        akVertex[i].z = 0.0f;

        float fAdjust = 1.0f + 0.25f*Math::SymmetricRandom();
        akVertex[i] *= fAdjust;
    }

    m_pkPolyline = new Polyline3(iVQuantity,akVertex,true);
    InvalidateRect(ms_hWnd,NULL,TRUE);

    return true;
}
//----------------------------------------------------------------------------
void ClodPolyline::OnTerminate ()
{
    delete m_pkPolyline;
}
//----------------------------------------------------------------------------
bool ClodPolyline::OnPaint (HDC hDC)
{
    if ( !m_pkPolyline )
        return false;

    int iEQuantity = m_pkPolyline->GetEdgeQuantity();
    const int* aiEdge = m_pkPolyline->GetEdges();
    const Vector3* akVertex = m_pkPolyline->GetVertices();

    Vector3 kV;
    int iX, iY, i;

    for (i = 0; i < m_pkPolyline->GetVertexQuantity(); i++)
    {
        kV = akVertex[i];
        iX = int(0.25f*iSize*(kV.x+2.0f));
        iY = iSize - 1 - (int)(0.25f*iSize*(kV.y+2.0f));
        for (int iDy = -1; iDy <= 1; iDy++)
        {
            for (int iDx = -1; iDx <= 1; iDx++)
                SetPixel(hDC,iX+iDx,iY+iDy,RGB(0,0,0));
        }
    }

    for (i = 0; i < iEQuantity; i++)
    {
        kV = akVertex[aiEdge[2*i]];
        iX = int(0.25f*iSize*(kV.x+2.0f));
        iY = iSize - 1 - (int)(0.25f*iSize*(kV.y+2.0f));
        MoveToEx(hDC,iX,iY,NULL);

        kV = akVertex[aiEdge[2*i+1]];
        iX = int(0.25*iSize*(kV.x+2.0));
        iY = iSize - 1 - (int)(0.25f*iSize*(kV.y+2.0f));
        LineTo(hDC,iX,iY);
    }

    return true;
}
//----------------------------------------------------------------------------
bool ClodPolyline::OnChar (char cCharCode, long)
{
    int iLOD, iMaxLOD;

    switch ( cCharCode )
    {
    case '+':  // increase level of detail
    case '=':
        iLOD = m_pkPolyline->GetLevelOfDetail();
        iMaxLOD = m_pkPolyline->GetMaxLevelOfDetail();
        if ( iLOD < iMaxLOD )
        {
            m_pkPolyline->SetLevelOfDetail(iLOD+1);
            InvalidateRect(ms_hWnd,NULL,TRUE);
        }
        return true;

    case '-':  // decrease level of detail
    case '_':
        iLOD = m_pkPolyline->GetLevelOfDetail();
        if ( iLOD > 0 )
        {
            m_pkPolyline->SetLevelOfDetail(iLOD-1);
            InvalidateRect(ms_hWnd,NULL,TRUE);
        }
        return true;

    case 'q':
    case 'Q':
    case VK_ESCAPE:
        PostMessage(GetWindowHandle(),WM_DESTROY,0,0);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------


