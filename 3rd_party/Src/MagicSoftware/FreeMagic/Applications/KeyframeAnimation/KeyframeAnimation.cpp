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

#include "KeyframeAnimation.h"

// Initial size of application window is irrelevant.  OnPrecreate will
// change the size based on the selected image.
KeyframeAnimation g_kTheApp("KeyframeAnimation",256,256);

//----------------------------------------------------------------------------
KeyframeAnimation::KeyframeAnimation (char* acCaption, int iWidth,
    int iHeight, unsigned int uiMenuID, unsigned int uiStatusPaneQuantity)
    :
    FWinApplication(acCaption,iWidth,iHeight,uiMenuID,uiStatusPaneQuantity),
    m_iNumKeys(7),
    m_fHKLength(70.0f),
    m_fKALength(70.0f),
    m_fAFLength(15.0f),
    m_fTMin(0.0f),
    m_fTMax((float)(m_iNumKeys-3)),
    m_fDt(0.01f*(m_fTMax-m_fTMin))
{
    m_pkPosSpline = NULL;
    m_pkHRotSpline = NULL;
    m_pkKRotSpline = NULL;
    m_pkARotSpline = NULL;
    m_fTime = m_fTMin;
    m_bDrawKeys = true;
}
//----------------------------------------------------------------------------
KeyframeAnimation::~KeyframeAnimation ()
{
}
//----------------------------------------------------------------------------
void KeyframeAnimation::InverseKinematics (Leg& rkLeg)
{
    rkLeg.m_kK.x = rkLeg.m_kH.x+rkLeg.m_fHKLength*sinf(rkLeg.m_fHAngle);
    rkLeg.m_kK.y = rkLeg.m_kH.y+rkLeg.m_fHKLength*cosf(rkLeg.m_fHAngle);
    rkLeg.m_kK.z = 0.0f;

    float fCos = cosf(rkLeg.m_fKAngle);
    float fSin = sinf(rkLeg.m_fKAngle);
    float fInvLength = 1.0f/rkLeg.m_fHKLength;
    float fDx = (rkLeg.m_kH.x-rkLeg.m_kK.x)*fInvLength;
    float fDy = (rkLeg.m_kH.y-rkLeg.m_kK.y)*fInvLength;
    rkLeg.m_kA.x = rkLeg.m_kK.x+rkLeg.m_fKALength*( fCos*fDx+fSin*fDy);
    rkLeg.m_kA.y = rkLeg.m_kK.y+rkLeg.m_fKALength*(-fSin*fDx+fCos*fDy);
    rkLeg.m_kA.z = 0;

    fCos = cosf(rkLeg.m_fAAngle);
    fSin = sinf(rkLeg.m_fAAngle);
    fInvLength = 1.0f/rkLeg.m_fKALength;
    fDx = (rkLeg.m_kK.x-rkLeg.m_kA.x)*fInvLength;
    fDy = (rkLeg.m_kK.y-rkLeg.m_kA.y)*fInvLength;
    rkLeg.m_kF.x = rkLeg.m_kA.x+rkLeg.m_fAFLength*( fCos*fDx+fSin*fDy);
    rkLeg.m_kF.y = rkLeg.m_kA.y+rkLeg.m_fAFLength*(-fSin*fDx+fCos*fDy);
    rkLeg.m_kF.z = 0.0f;
}
//---------------------------------------------------------------------------
bool KeyframeAnimation::OnInitialize ()
{
    // keyframe data
    Vector3 akPos[7] =
    {
        Vector3(50.0f,34.0f,0.0f),
        Vector3(50.0f,34.0f,0.0f),
        Vector3(80.0f,33.0f,0.0f),
        Vector3(110.0f,33.0f,0.0f),
        Vector3(140.0f,34.0f,0.0f),
        Vector3(207.0f,34.0f,0.0f),
        Vector3(207.0f,34.0f,0.0f)
    };

    float afHAngle[7] =
    {
        0.0f, 0.0f, 0.392699f, 0.589049f, 0.490874f, 0.0f, 0.0f
    };

    float afKAngle[7] =
    {
        2.748894f, 2.748894f, 1.963495f, 2.356194f, 2.748894f, 2.748894f,
        2.748894f
    };

    float afAAngle[7] =
    {
        -1.178097f, -1.178097f, -1.178097f, -1.570796f, -1.963495f,
        -1.178097f, -1.178097f
    };

    PosKey akHPos[7];
    RotKey akHRot[7], akKRot[7], akARot[7];
    m_akLeg = new Leg[7];

    int i;
    for (i = 0; i < m_iNumKeys; i++)
    {
        float fTime = (float)(i-1);
        akHPos[i].Time() = fTime;
        akHPos[i].P() = akPos[i];
        akHRot[i].Time() = fTime;
        akHRot[i].Q().FromAngleAxis(afHAngle[i],Vector3::UNIT_Z);
        akKRot[i].Time() = fTime;
        akKRot[i].Q().FromAngleAxis(afKAngle[i],Vector3::UNIT_Z);
        akARot[i].Time() = fTime;
        akARot[i].Q().FromAngleAxis(afAAngle[i],Vector3::UNIT_Z);

        m_akLeg[i].m_fHKLength = m_fHKLength;
        m_akLeg[i].m_fKALength = m_fKALength;
        m_akLeg[i].m_fAFLength = m_fAFLength;
        m_akLeg[i].m_kH = akHPos[i].P();
        m_akLeg[i].m_fHAngle = afHAngle[i];
        m_akLeg[i].m_fKAngle = afKAngle[i];
        m_akLeg[i].m_fAAngle = afAAngle[i];
        InverseKinematics(m_akLeg[i]);
    }

    m_pkPosSpline = new PosSpline(m_iNumKeys,akHPos);
    m_pkHRotSpline = new RotSpline(m_iNumKeys,akHRot);
    m_pkKRotSpline = new RotSpline(m_iNumKeys,akKRot);
    m_pkARotSpline = new RotSpline(m_iNumKeys,akARot);

    InvalidateRect(GetWindowHandle(),NULL,TRUE);
    return true;
}
//----------------------------------------------------------------------------
void KeyframeAnimation::OnTerminate ()
{
    delete[] m_akLeg;
    delete m_pkPosSpline;
    delete m_pkHRotSpline;
    delete m_pkKRotSpline;
    delete m_pkARotSpline;
}
//----------------------------------------------------------------------------
bool KeyframeAnimation::OnPaint (HDC hDC)
{
    if ( !m_pkPosSpline )
        return false;

    HPEN hPen;
    HGDIOBJ hOldPen;

    if ( m_bDrawKeys )
    {
        // draw key frames
        hPen = CreatePen(PS_SOLID,2,RGB(0,0,0));
        hOldPen = SelectObject(hDC,hPen);

        for (int i = 0; i < m_iNumKeys; i++)
        {
            MoveToEx(hDC,int(m_akLeg[i].m_kH.x),int(m_akLeg[i].m_kH.y),NULL);
            LineTo(hDC,int(m_akLeg[i].m_kK.x),int(m_akLeg[i].m_kK.y));
            LineTo(hDC,int(m_akLeg[i].m_kA.x),int(m_akLeg[i].m_kA.y));
            LineTo(hDC,int(m_akLeg[i].m_kF.x),int(m_akLeg[i].m_kF.y));
        }

        SelectObject(hDC,hOldPen);
        DeleteObject(hPen);
    }

    // interpolate key frames
    Leg kInterp;
    float fAngle;
    Vector3 kAxis;

    kInterp.m_fHKLength = m_fHKLength;
    kInterp.m_fKALength = m_fKALength;
    kInterp.m_fAFLength = m_fAFLength;
    kInterp.m_kH =  m_pkPosSpline->Position(m_fTime);

    m_pkHRotSpline->Q(m_fTime).ToAngleAxis(fAngle,kAxis);
    kInterp.m_fHAngle = ( kAxis.z > 0.0f ? fAngle : -fAngle );

    m_pkKRotSpline->Q(m_fTime).ToAngleAxis(fAngle,kAxis);
    kInterp.m_fKAngle = ( kAxis.z > 0.0f ? fAngle : -fAngle );

    m_pkARotSpline->Q(m_fTime).ToAngleAxis(fAngle,kAxis);
    kInterp.m_fAAngle = ( kAxis.z > 0.0f ? fAngle : -fAngle );

    InverseKinematics(kInterp);
    
    // draw interpolated key frame
    hPen = CreatePen(PS_SOLID,2,RGB(255,0,0));
    hOldPen = SelectObject(hDC,hPen);
    MoveToEx(hDC,int(kInterp.m_kH.x),int(kInterp.m_kH.y),NULL);
    LineTo(hDC,int(kInterp.m_kK.x),int(kInterp.m_kK.y));
    LineTo(hDC,int(kInterp.m_kA.x),int(kInterp.m_kA.y));
    LineTo(hDC,int(kInterp.m_kF.x),int(kInterp.m_kF.y));
    SelectObject(hDC,hOldPen);
    DeleteObject(hPen);

    return true;
}
//----------------------------------------------------------------------------
bool KeyframeAnimation::OnChar (char cCharCode, long)
{
    switch ( cCharCode )
    {
    case 'k':
    case 'K':
        // toggle drawing of key frames
        m_bDrawKeys = !m_bDrawKeys;
        InvalidateRect(GetWindowHandle(),NULL,TRUE);
        break;
    case 'b':
    case 'B':
        // single step backwards through animation
        m_fTime -= m_fDt;
        if ( m_fTime < m_fTMin )
            m_fTime = m_fTMax;
        InvalidateRect(GetWindowHandle(),NULL,TRUE);
        break;
    case 'f':
    case 'F':
        // single step forwards through animation
        m_fTime += m_fDt;
        if ( m_fTime > m_fTMax )
            m_fTime = m_fTMin;
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


