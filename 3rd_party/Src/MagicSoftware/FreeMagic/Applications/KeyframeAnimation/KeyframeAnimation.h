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

#ifndef KEYFRAMEANIMATION_H
#define KEYFRAMEANIMATION_H

#include "FWinApplication.h"
#include "PosSpline.h"
#include "RotSpline.h"


class KeyframeAnimation : public FWinApplication
{
public:
    KeyframeAnimation (char* acCaption, int iWidth, int iHeight,
        unsigned int uiMenuID = 0, unsigned int uiStatusPaneQuantity = 0);

    virtual ~KeyframeAnimation ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();

    virtual bool OnPaint (HDC hDC);
    virtual bool OnChar (char cCharCode, long);

protected:
    class Leg
    {
    public:
        // specified
        Mgc::Vector3 m_kH;
        float m_fHKLength, m_fKALength, m_fAFLength;
        float m_fHAngle, m_fKAngle, m_fAAngle;

        // derived
        Mgc::Vector3 m_kK, m_kA, m_kF;
    };

    void InverseKinematics (Leg& rkLeg);

    const int m_iNumKeys;
    const float m_fHKLength, m_fKALength, m_fAFLength;
    const float m_fTMin, m_fTMax, m_fDt;

    Leg* m_akLeg;
    PosSpline* m_pkPosSpline;
    RotSpline* m_pkHRotSpline;
    RotSpline* m_pkKRotSpline;
    RotSpline* m_pkARotSpline;
    float m_fTime;
    bool m_bDrawKeys;
};

#endif


