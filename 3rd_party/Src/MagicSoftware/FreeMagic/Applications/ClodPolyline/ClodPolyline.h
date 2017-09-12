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

#ifndef CLODPOLYLINE_H
#define CLODPOLYLINE_H

#include "FWinApplication.h"
#include "Polyline3.h"

class ClodPolyline : public FWinApplication
{
public:
    ClodPolyline (char* acCaption, int iWidth, int iHeight,
        unsigned int uiMenuID = 0, unsigned int uiStatusPaneQuantity = 0);

    virtual ~ClodPolyline ();

    virtual bool OnInitialize ();
    virtual void OnTerminate ();
    virtual bool OnPaint (HDC hDC);
    virtual bool OnChar (char cCharCode, long lKeyData);

protected:
    Polyline3* m_pkPolyline;
    int m_iLOD;
};

#endif


