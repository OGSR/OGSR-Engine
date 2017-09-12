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

#include "Silhouette.h"
using namespace Mgc;
using namespace std;

Silhouette g_kTheApp("Silhouette",256,256);

//----------------------------------------------------------------------------
Silhouette::Silhouette (char* acCaption, int iWidth, int iHeight,
    unsigned int uiMenuID, unsigned int uiStatusPaneQuantity)
    :
    FWinApplication(acCaption,iWidth,iHeight,uiMenuID,uiStatusPaneQuantity)
{
}
//----------------------------------------------------------------------------
Silhouette::~Silhouette ()
{
}
//----------------------------------------------------------------------------
void Silhouette::ConstructCube ()
{
    vector<Vector3> akVertex(8);
    akVertex[0] = Vector3(-0.5f,-0.5f,-0.5f);
    akVertex[1] = Vector3(+0.5f,-0.5f,-0.5f);
    akVertex[2] = Vector3(+0.5f,+0.5f,-0.5f);
    akVertex[3] = Vector3(-0.5f,+0.5f,-0.5f);
    akVertex[4] = Vector3(-0.5f,-0.5f,+0.5f);
    akVertex[5] = Vector3(+0.5f,-0.5f,+0.5f);
    akVertex[6] = Vector3(+0.5f,+0.5f,+0.5f);
    akVertex[7] = Vector3(-0.5f,+0.5f,+0.5f);

    vector<int> aiConnect(36);
    aiConnect[ 0] = 0;  aiConnect[ 1] = 3;  aiConnect[ 2] = 2;
    aiConnect[ 3] = 0;  aiConnect[ 4] = 2;  aiConnect[ 5] = 1;
    aiConnect[ 6] = 0;  aiConnect[ 7] = 1;  aiConnect[ 8] = 5;
    aiConnect[ 9] = 0;  aiConnect[10] = 5;  aiConnect[11] = 4;
    aiConnect[12] = 0;  aiConnect[13] = 4;  aiConnect[14] = 7;
    aiConnect[15] = 0;  aiConnect[16] = 7;  aiConnect[17] = 3;
    aiConnect[18] = 6;  aiConnect[19] = 5;  aiConnect[20] = 1;
    aiConnect[21] = 6;  aiConnect[22] = 1;  aiConnect[23] = 2;
    aiConnect[24] = 6;  aiConnect[25] = 2;  aiConnect[26] = 3;
    aiConnect[27] = 6;  aiConnect[28] = 3;  aiConnect[29] = 7;
    aiConnect[30] = 6;  aiConnect[31] = 7;  aiConnect[32] = 4;
    aiConnect[33] = 6;  aiConnect[34] = 4;  aiConnect[35] = 5;

    m_kPoly.Create(akVertex,aiConnect);
}
//----------------------------------------------------------------------------
bool Silhouette::OnInitialize ()
{
    m_kEye = Vector3::UNIT_Z;
    m_kRot = Matrix3::IDENTITY;
    m_kRotXP.FromAxisAngle(Vector3::UNIT_X,+0.1f);
    m_kRotXM.FromAxisAngle(Vector3::UNIT_X,-0.1f);
    m_kRotYP.FromAxisAngle(Vector3::UNIT_Y,+0.1f);
    m_kRotYM.FromAxisAngle(Vector3::UNIT_Y,-0.1f);

    ConstructCube();
    m_kPoly.ComputeTerminator(m_kEye,m_kTerminator);

    InvalidateRect(ms_hWnd,NULL,TRUE);
    return true;
}
//----------------------------------------------------------------------------
void Silhouette::OnTerminate ()
{
}
//----------------------------------------------------------------------------
void Silhouette::ProjectPoint (const Vector3& rkPoint, int& riX, int& riY)
{
    float fDot = m_kEye.Dot(rkPoint);
    float fT = 2.0f/(1.0f - fDot);
    Vector3 kProj = m_kEye + fT*(rkPoint - m_kEye);
    Vector3 kQ = kProj + m_kEye;
    Vector3 kProd = kQ*m_kRot;

    int iSize = GetWidth();
    riX = iSize/4 + int((iSize/8)*(kProd.x + 2.0f));
    riY = iSize/4 + int((iSize/8)*(kProd.y + 2.0f));
}
//----------------------------------------------------------------------------
void Silhouette::DrawPolyhedron (HDC hDC)
{
    HPEN hPen = CreatePen(PS_SOLID,2,RGB(255,0,0));
    HGDIOBJ hOldPen = SelectObject(hDC,hPen);

    const vector<Vector3>& rakPoint = m_kPoly.GetPoints();
    int iEQuantity = m_kPoly.GetEQuantity();
    for (int i = 0; i < iEQuantity; i++)
    {
        const MTEdge& rkE = m_kPoly.GetEdge(i);
        int iX0, iY0;
        ProjectPoint(rakPoint[m_kPoly.GetVLabel(rkE.GetVertex(0))],iX0,iY0);

        int iX1, iY1;
        ProjectPoint(rakPoint[m_kPoly.GetVLabel(rkE.GetVertex(1))],iX1,iY1);

        MoveToEx(hDC,iX0,iY0,NULL);
        LineTo(hDC,iX1,iY1);
    }

    SelectObject(hDC,hOldPen);
    DeleteObject(hPen);
}
//----------------------------------------------------------------------------
void Silhouette::DrawTerminator (HDC hDC)
{
    HPEN hPen = CreatePen(PS_SOLID,2,RGB(0,0,255));
    HGDIOBJ hOldPen = SelectObject(hDC,hPen);

    int iX, iY;
    ProjectPoint(m_kTerminator[0],iX,iY);
    MoveToEx(hDC,iX,iY,NULL);
    for (int i = 1; i < (int)m_kTerminator.size(); i++)
    {
        ProjectPoint(m_kTerminator[i],iX,iY);
        LineTo(hDC,iX,iY);
    }

    SelectObject(hDC,hOldPen);
    DeleteObject(hPen);
}
//----------------------------------------------------------------------------
bool Silhouette::OnPaint (HDC hDC)
{
    if ( m_kPoly.GetVQuantity() == 0 )
        return false;

    DrawPolyhedron(hDC);
    DrawTerminator(hDC);
    return true;
}
//----------------------------------------------------------------------------
bool Silhouette::OnKeyDown (int iVirtKey, long)
{
    switch ( iVirtKey )
    {
    case VK_LEFT:
        m_kRot = m_kRotYP*m_kRot;
        m_kEye = m_kRot.GetColumn(2);
        m_kTerminator.clear();
        m_kPoly.ComputeTerminator(m_kEye,m_kTerminator);
        InvalidateRect(GetWindowHandle(),NULL,TRUE);
        return true;
    case VK_RIGHT:
        m_kRot = m_kRotYM*m_kRot;
        m_kEye = m_kRot.GetColumn(2);
        m_kTerminator.clear();
        m_kPoly.ComputeTerminator(m_kEye,m_kTerminator);
        InvalidateRect(GetWindowHandle(),NULL,TRUE);
        return true;
    case VK_UP:
        m_kRot = m_kRotXP*m_kRot;
        m_kEye = m_kRot.GetColumn(2);
        m_kTerminator.clear();
        m_kPoly.ComputeTerminator(m_kEye,m_kTerminator);
        InvalidateRect(GetWindowHandle(),NULL,TRUE);
        break;
    case VK_DOWN:
        m_kRot = m_kRotXM*m_kRot;
        m_kEye = m_kRot.GetColumn(2);
        m_kTerminator.clear();
        m_kPoly.ComputeTerminator(m_kEye,m_kTerminator);
        InvalidateRect(GetWindowHandle(),NULL,TRUE);
        break;
    case 'q':
    case 'Q':
    case VK_ESCAPE:
        PostMessage(GetWindowHandle(),WM_DESTROY,0,0);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
