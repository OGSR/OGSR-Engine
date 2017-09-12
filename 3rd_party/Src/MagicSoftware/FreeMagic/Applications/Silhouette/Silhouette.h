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

#ifndef SILHOUETTE_H
#define SILHOUETTE_H

#include "FWinApplication.h"
#include "MgcConvexPolyhedron.h"
#include "MgcMatrix3.h"
#include <vector>

class Silhouette : public FWinApplication
{
public:
    Silhouette (char* acCaption, int iWidth, int iHeight,
        unsigned int uiMenuID = 0, unsigned int uiStatusPaneQuantity = 0);

    virtual ~Silhouette ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual bool OnPaint (HDC hDC);
    virtual bool OnKeyDown (int iVirtKey, long lKeyData);

protected:
    void ConstructCube ();
    void ProjectPoint (const Vector3& rkPoint, int& riX, int& riY);
    void DrawPolyhedron (HDC hDC);
    void DrawTerminator (HDC hDC);

    ConvexPolyhedron m_kPoly;
    Vector3 m_kEye;
    std::vector<Vector3> m_kTerminator;
    Matrix3 m_kRot;
    Matrix3 m_kRotXP, m_kRotXM, m_kRotYP, m_kRotYM, m_kProd;
};

#endif
