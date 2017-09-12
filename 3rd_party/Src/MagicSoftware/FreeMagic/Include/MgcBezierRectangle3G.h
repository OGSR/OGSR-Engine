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

#ifndef MGCBEZIERRECTANGLE3G_H
#define MGCBEZIERRECTANGLE3G_H

#include "MgcBezierRectangleG.h"
#include "MgcVector3.h"

namespace Mgc {


class MAGICFM BezierRectangle3G : public BezierRectangleG
{
public:
    // Construction.  BezierRectangle3G accepts responsibility for deleting
    // the input array.
    BezierRectangle3G (Vector3* akCtrlPoint);

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

        // Xuuu(0,0), Xuuu(0,1), Xuuu(1,0) = Xuuu(0,0), Xuuu(1,1) = Xuuu(0,1)
        Vector3 m_akXuuu[2];

        // Xvvv(0,0), Xvvv(1,0), Xvvv(0,1) = Xvvv(0,0), Xvvv(1,1) = Xvvv(1,0)
        Vector3 m_akXvvv[2];

        // Xuuuvv(0,0), Xuuuvv(0,1), Xuuuvv(1,0) = Xuuuvv(0,0),
        //   Xuuuvv(1,1) = Xuuuvv(0,1)
        Vector3 m_akXuuuvv[2];

        // Xuuvvv(0,0), Xuuvvv(1,0), Xuuvvv(0,1) = Xuuvvv(0,0),
        //   Xuuvvv(1,1) = Xuuvvv(1,0)
        Vector3 m_akXuuvvv[2];

        Vector3 m_aakXuu[2][2], m_aakXvv[2][2], m_aakXuuvv[2][2],
            m_aakXuuv[2][2], m_aakXuvv[2][2];
    };

    // subdivide rectangle into four quadrants
    void Subdivide (int iLevel, Real fDSqr, Vector3* akX,
        Vector3* akXs, Vector3* akXt, BlockParameters& rkBP);
};

} // namespace Mgc

#endif

