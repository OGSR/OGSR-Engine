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

#ifndef MGCBEZIERCYLINDER2G_H
#define MGCBEZIERCYLINDER2G_H

#include "MgcBezierCylinderG.h"
#include "MgcVector3.h"

namespace Mgc {


class MAGICFM BezierCylinder2G : public BezierCylinderG
{
public:
    // Construction.  BezierCylinder2G accepts responsibility for deleting
    // the input array.
    BezierCylinder2G (Vector3* akCtrlPoint);

    // position and derivatives up to second order
    virtual Vector3 GetPosition (Real fU, Real fV) const;
    virtual Vector3 GetDerivativeU (Real fU, Real fV) const;
    virtual Vector3 GetDerivativeV (Real fU, Real fV) const;
    virtual Vector3 GetDerivativeUU (Real fU, Real fV) const;
    virtual Vector3 GetDerivativeUV (Real fU, Real fV) const;
    virtual Vector3 GetDerivativeVV (Real fU, Real fV) const;

    // tessellation
    virtual void Tessellate (int iLevel, bool bWantNormals);

protected:
    // precomputation
    class MAGICFM IntervalParameters
    {
    public:
        // u = curve direction, v = cylinder direction
        int m_i00, m_i01, m_i10, m_i11;
        Vector3 m_aakXuu[2];
    };

    // tessellate cylinder curves by recursive subdivision
    void SubdivideCylinder (int iCLevel, Vector3* akX, Vector3* akXu,
        Vector3* akXv, int i0, int i1, int iTwoPowL);

    // tessellate boundary curves by recursive subdivision
    void SubdivideBoundary (int iLevel, Real fDSqr, Vector3* akX,
        Vector3* akXu, Vector3* akXv, IntervalParameters& rkIP);
};

} // namespace Mgc

#endif

