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

#ifndef RAYTRACE_H
#define RAYTRACE_H

#include "MgcImages.h"
using namespace Mgc;


class RayTrace
{
public:
    RayTrace (ImageUChar3D* pkImage, float fGamma);
    ~RayTrace ();

    bool MoveTrackBall (float fX0, float fY0, float fX1, float fY1);
    void Trace (int iSpacing);
    void DrawWireFrame ();
    void Correction (float fGamma);

    unsigned short GetRendered (int iX, int iY)
    {
        return (*m_pkRender)(iX,iY);
    }

    unsigned short GetRendered (int i)
    {
        return (*m_pkRender)[i];
    }

    float& Frame (int iY, int iX)
    {
        return m_aafFrame[iY][iX];
    }

private:
    // storage for scaled volume data
    ImageFloat3D* m_pkDensity;

    // accumulator and render images
    int m_iBound, m_iBoundM1;
    int m_iHBound;
    ImageFloat2D* m_pkAccum;
    ImageUShort2D* m_pkRender;

    // center point of image
    float m_fXCenter, m_fYCenter, m_fZCenter;

    // for gamma correction of rendered image values
    float m_fGamma;

    // frame field for eyepoint:  u = column 0, v = column 1, w = column 2
    float m_aafFrame[3][3];

    bool Clipped (float fP, float fQ, float& rfU1, float& rfU2);
    bool Clip3D (float& rfX1, float& rfY1, float& rfZ1, float& rfX2,
        float& rfY2, float& rfZ2);
    void Line3D (int iJ0, int iJ1, int iX0, int iY0, int iZ0, int iX1,
        int iY1, int iZ1);
    void Line2D (bool bVisible, int iX0, int iY0, int iX1, int iY1);
};

#endif


