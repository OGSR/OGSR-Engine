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

#ifndef MGCBEZIERTRIANGLE2G_H
#define MGCBEZIERTRIANGLE2G_H

#include "MgcBezierTriangleG.h"
#include "MgcVector3.h"

namespace Mgc {


class MAGICFM BezierTriangle2G : public BezierTriangleG
{
public:
    // Construction.  BezierTriangle2G accepts responsibility for deleting
    // the input array.
    BezierTriangle2G (Vector3* akCtrlPoint);

    // position and derivatives up to second order
    virtual Vector3 GetPosition (Real fU, Real fV) const;
    virtual Vector3 GetDerivativeU (Real fU, Real fV) const;
    virtual Vector3 GetDerivativeV (Real fU, Real fV) const;
    virtual Vector3 GetDerivativeUU (Real fU, Real fV) const;
    virtual Vector3 GetDerivativeUV (Real fU, Real fV) const;
    virtual Vector3 GetDerivativeVV (Real fU, Real fV) const;

    // tessellation by recursive subdivision
    virtual void Tessellate (int iLevel, bool bWantNormals);

protected:
    // precomputation (d/dh = d/du - d/dv)
    class MAGICFM BlockParameters
    {
    public:
        Vector3 m_kXuu, m_kXvv, m_kXhh;
    };

    // subdivide triangle with right angle at Lower Left corner
    void SubdivideLL (int iLevel, Real fDSqr, Vector3* akX, Vector3* akXu,
        Vector3* akXv, int i0, int i1, int i2, BlockParameters& rkBP);

    // subdivide triangle with right angle at Upper Right corner
    void SubdivideUR (int iLevel, Real fDSqr, Vector3* akX, Vector3* akXu,
        Vector3* akXv, int i0, int i1, int i2, BlockParameters& rkBP);
};

} // namespace Mgc

#endif

