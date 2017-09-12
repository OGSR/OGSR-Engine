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

#ifndef MGCBEZIERRECTANGLE2G_H
#define MGCBEZIERRECTANGLE2G_H

#include "MgcBezierRectangleG.h"

namespace Mgc {


class MAGICFM BezierRectangle2G : public BezierRectangleG
{
public:
    // Construction.  BezierRectangle2G accepts responsibility for deleting
    // the input array.
    BezierRectangle2G (Vector3* akCtrlPoint);

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
    // precomputation
    class MAGICFM BlockParameters
    {
    public:
        int m_i00, m_i01, m_i10, m_i11;

        // Xuu(0,0), Xuu(0,1), Xuu(1,0) = Xuu(0,0), Xuu(1,1) = Xuu(0,1)
        Vector3 m_akXuu[2];

        // Xvv(0,0), Xvv(1,0), Xvv(0,1) = Xvv(0,0), Xvv(1,1) = Xvv(1,0)
        Vector3 m_akXvv[2];

        // Xuuv(0,0), Xuuv(0,1), Xuuv(1,0) = Xuuv(0,0), Xuuv(1,1) = Xuuv(0,1)
        Vector3 m_akXuuv[2];

        // Xuvv(0,0), Xuvv(1,0), Xuvv(0,1) = Xuvv(0,0), Xuvv(1,1) = Xuvv(1,0)
        Vector3 m_akXuvv[2];
    };

    // subdivide rectangle into four quadrants
    void Subdivide (int iLevel, Real fDSqr, Vector3* akX, Vector3* akXu,
        Vector3* akXv, Vector3& rkXuuvv, BlockParameters& rkBP);
};

} // namespace Mgc

#endif

