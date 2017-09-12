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

#ifndef MGCTUBESURFACEG_H
#define MGCTUBESURFACEG_H

#include "MgcVector3.h"
#include "MgcSurface.h"

namespace Mgc {

class MultipleCurve3;


class MAGICFM TubeSurfaceG : public Surface
{
public:
    typedef Real (*RadialFunction)(Real);

    // Construction and destruction.  MgcTubeSurfaceG accepts responsibility
    // for deleting the input curve.
    TubeSurfaceG (MultipleCurve3* pkMedial, RadialFunction oRadial,
        bool bClosed, int iMedialSamples, int iSliceSamples,
        bool bSampleByArcLength, bool bWantNormals,
        const Vector3* pkUpVector);

    virtual ~TubeSurfaceG ();

    // curve data
    MultipleCurve3*& Medial ();
    const MultipleCurve3* Medial () const;
    RadialFunction& Radial ();
    RadialFunction Radial () const;
    bool& Closed ();
    bool Closed () const;

    // mesh data
    int GetVertexQuantity () const;
    Vector3* GetVertices ();
    Vector3* GetNormals ();
    int GetTriangleQuantity () const;
    const int* GetConnectivity () const;

    // Tessellation.  If no up vector is specified, the tessellation uses
    // the Frenet frame to generate the surface.  Otherwise the up vector
    // and tangent vector are used to generate frames.
    void Tessellate (int iMedialSamples, int iSliceSamples,
        bool bSampleByArcLength, bool bWantNormals,
        const Vector3* pkUpVector);

    int GetMedialSamples () const;
    int GetSliceSamples () const;
    bool GetSampleByArcLength () const;
    bool GetWantNormals () const;
    const Vector3* GetUpVector () const;

protected:
    TubeSurfaceG ();

    // tessellation
    int Index (int iS, int iM);
    void ComputeVertices ();
    void ComputeNormals ();
    void ComputeConnectivity ();

    // curve data
    MultipleCurve3* m_pkMedial;
    RadialFunction m_oRadial;
    bool m_bClosed;

    // mesh data
    int m_iVertexQuantity;
    Vector3* m_akVertex;
    Vector3* m_akNormal;
    int m_iTriangleQuantity;
    int* m_aiConnect;

    // tessellation data
    int m_iMedialSamples;
    int m_iSliceSamples;
    bool m_bSampleByArcLength;
    bool m_bWantNormals;
    const Vector3* m_pkUpVector;
};

#include "MgcTubeSurfaceG.inl"

} // namespace Mgc

#endif

