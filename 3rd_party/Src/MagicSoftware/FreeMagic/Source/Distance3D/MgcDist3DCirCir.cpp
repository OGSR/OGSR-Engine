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

#include "MgcDist3DCirCir.h"
#include "MgcPolynomial.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real Mgc::SqrDistance (const Circle3& rkCircle0, const Circle3& rkCircle1,
    Vector3* pkClosest0, Vector3* pkClosest1)
{
    Vector3 kDiff = rkCircle1.Center() - rkCircle0.Center();
    Real fU0U1 = rkCircle0.U().Dot(rkCircle1.U());
    Real fU0V1 = rkCircle0.U().Dot(rkCircle1.V());
    Real fV0U1 = rkCircle0.V().Dot(rkCircle1.U());
    Real fV0V1 = rkCircle0.V().Dot(rkCircle1.V());

    Real fA0 = -kDiff.Dot(rkCircle0.U());
    Real fA1 = -rkCircle1.Radius()*fU0U1;
    Real fA2 = -rkCircle1.Radius()*fU0V1;
    Real fA3 = kDiff.Dot(rkCircle0.V());
    Real fA4 = rkCircle1.Radius()*fV0U1;
    Real fA5 = rkCircle1.Radius()*fV0V1;

    Real fB0 = -kDiff.Dot(rkCircle1.U());
    Real fB1 = rkCircle0.Radius()*fU0U1;
    Real fB2 = rkCircle0.Radius()*fV0U1;
    Real fB3 = kDiff.Dot(rkCircle1.V());
    Real fB4 = -rkCircle0.Radius()*fU0V1;
    Real fB5 = -rkCircle0.Radius()*fV0V1;

    // compute polynomial p0 = p00+p01*z+p02*z^2
    Polynomial kP0(2);
    kP0[0] = fA2*fB1-fA5*fB2;
    kP0[1] = fA0*fB4-fA3*fB5;
    kP0[2] = fA5*fB2-fA2*fB1+fA1*fB4-fA4*fB5;

    // compute polynomial p1 = p10+p11*z
    Polynomial kP1(1);
    kP1[0] = fA0*fB1-fA3*fB2;
    kP1[1] = fA1*fB1-fA5*fB5+fA2*fB4-fA4*fB2;

    // compute polynomial q0 = q00+q01*z+q02*z^2
    Polynomial kQ0(2);
    kQ0[0] = fA0*fA0+fA2*fA2+fA3*fA3+fA5*fA5;
    kQ0[1] = 2.0f*(fA0*fA1+fA3*fA4);
    kQ0[2] = fA1*fA1-fA2*fA2+fA4*fA4-fA5*fA5;

    // compute polynomial q1 = q10+q11*z
    Polynomial kQ1(1);
    kQ1[0] = 2.0f*(fA0*fA2+fA3*fA5);
    kQ1[1] = 2.0f*(fA1*fA2+fA4*fA5);

    // compute coefficients of r0 = r00+r02*z^2
    Polynomial kR0(2);
    kR0[0] = fB0*fB0;
    kR0[1] = 0.0f;
    kR0[2] = fB3*fB3-fB0*fB0;

    // compute polynomial r1 = r11*z;
    Polynomial kR1(1);
    kR1[0] = 0.0f;
    kR1[1] = 2.0f*fB0*fB3;

    // compute polynomial g0 = g00+g01*z+g02*z^2+g03*z^3+g04*z^4
    Polynomial kG0(4);
    kG0[0] = kP0[0]*kP0[0] + kP1[0]*kP1[0] - kQ0[0]*kR0[0];
    kG0[1] = 2.0f*(kP0[0]*kP0[1] + kP1[0]*kP1[1]) - kQ0[1]*kR0[0] -
        kQ1[0]*kR1[1];
    kG0[2] = kP0[1]*kP0[1] + 2.0f*kP0[0]*kP0[2] - kP1[0]*kP1[0] +
        kP1[1]*kP1[1] - kQ0[2]*kR0[0] - kQ0[0]*kR0[2] - kQ1[1]*kR1[1];
    kG0[3] = 2.0f*(kP0[1]*kP0[2] - kP1[0]*kP1[1]) - kQ0[1]*kR0[2] +
        kQ1[0]*kR1[1];
    kG0[4] = kP0[2]*kP0[2] - kP1[1]*kP1[1] - kQ0[2]*kR0[2] + kQ1[1]*kR1[1];

    // compute polynomial g1 = g10+g11*z+g12*z^2+g13*z^3
    Polynomial kG1(3);
    kG1[0] = 2.0f*kP0[0]*kP1[0] - kQ1[0]*kR0[0];
    kG1[1] = 2.0f*(kP0[1]*kP1[0] + kP0[0]*kP1[1]) - kQ1[1]*kR0[0] -
        kQ0[0]*kR1[1];
    kG1[2] = 2.0f*(kP0[2]*kP1[0] + kP0[1]*kP1[1]) - kQ1[0]*kR0[2] -
        kQ0[1]*kR1[1];
    kG1[3] = 2.0f*kP0[2]*kP1[1] - kQ1[1]*kR0[2] - kQ0[2]*kR1[1];

    // compute polynomial h = sum_{i=0}^8 h_i z^i
    Polynomial kH(8);
    kH[0] = kG0[0]*kG0[0] - kG1[0]*kG1[0];
    kH[1] = 2.0f*(kG0[0]*kG0[1] - kG1[0]*kG1[1]);
    kH[2] = kG0[1]*kG0[1] + kG1[0]*kG1[0] - kG1[1]*kG1[1] +
        2.0f*(kG0[0]*kG0[2] - kG1[0]*kG1[2]);
    kH[3] = 2.0f*(kG0[1]*kG0[2] + kG0[0]*kG0[3] + kG1[0]*kG1[1] -
        kG1[1]*kG1[2] - kG1[0]*kG1[3]);
    kH[4] = kG0[2]*kG0[2] + kG1[1]*kG1[1] - kG1[2]*kG1[2] +
        2.0f*(kG0[1]*kG0[3] + kG0[0]*kG0[4] + kG1[0]*kG1[2] - kG1[1]*kG1[3]);
    kH[5] = 2.0f*(kG0[2]*kG0[3] + kG0[1]*kG0[4] + kG1[1]*kG1[2] +
        kG1[0]*kG1[3] - kG1[2]*kG1[3]);
    kH[6] = kG0[3]*kG0[3] + kG1[2]*kG1[2] - kG1[3]*kG1[3] +
        2.0f*(kG0[2]*kG0[4] + kG1[1]*kG1[3]);
    kH[7] = 2.0f*(kG0[3]*kG0[4] + kG1[2]*kG1[3]);
    kH[8] = kG0[4]*kG0[4] + kG1[3]*kG1[3];

    int iNumRoots;
    Real afRoot[8];
    kH.GetRootsOnInterval(-1.01f,1.01f,iNumRoots,afRoot);

    Real fMinSqrDist = Math::MAX_REAL;
    Real fCs0, fSn0, fCs1, fSn1;

    for (int i = 0; i < iNumRoots; i++)
    {
        fCs1 = afRoot[i];
        if ( fCs1 < -1.0f )
            fCs1 = -1.0f;
        else if ( fCs1 > 1.0f )
            fCs1 = 1.0f;

        // You can also try sn1 = -g0(cs1)/g1(cs1) to avoid the sqrt call,
        // but beware when g1 is nearly zero.  For now I use g0 and g1 to
        // determine the sign of sn1.
        fSn1 = Math::Sqrt(Math::FAbs(1.0f-fCs1*fCs1));

        Real fG0 = kG0(fCs1), fG1 = kG1(fCs1), fProd = fG0*fG1;
        if ( fProd > 0.0f )
        {
            fSn1 = -fSn1;
        }
        else if ( fProd < 0.0f )
        {
            // fSn1 already has correct sign
        }
        else if ( fG1 != 0.0f )
        {
            // g0 == 0.0
            // assert( fSn1 == 0.0 );
        }
        else // g1 == 0.0
        {
            // TO DO:  When g1 = 0, there is no constraint on fSn1.
            // What should be done here?  In this case, fCs1 is a afRoot
            // to the quartic equation g0(fCs1) = 0.  Is there some
            // geometric significance?
            assert( false );
        }

        Real fM00 = fA0 + fA1*fCs1 + fA2*fSn1;
        Real fM01 = fA3 + fA4*fCs1 + fA5*fSn1;
        Real fM10 = fB2*fSn1 + fB5*fCs1;
        Real fM11 = fB1*fSn1 + fB4*fCs1;
        Real fDet = fM00*fM11 - fM01*fM10;
        if ( Math::FAbs(fDet) >= 1e-05f )
        {
            Real fInvDet = 1.0f/fDet;
            Real fLambda = -(fB0*fSn1 + fB3*fCs1);
            fCs0 = fLambda*fM00*fInvDet;
            fSn0 = -fLambda*fM01*fInvDet;

            // Unitize in case of numerical error.  Remove if you feel
            // confidant of the accuracy for fCs0 and fSn0.
            Real fTmp = Math::InvSqrt(fCs0*fCs0+fSn0*fSn0);
            fCs0 *= fTmp;
            fSn0 *= fTmp;

            Vector3 kClosest0 = rkCircle0.Center() + rkCircle0.Radius()*(
                fCs0*rkCircle0.U() + fSn0*rkCircle0.V());
            Vector3 kClosest1 = rkCircle1.Center() + rkCircle1.Radius()*(
                fCs1*rkCircle1.U() + fSn1*rkCircle1.V());
            kDiff = kClosest1 - kClosest0;

            Real fSqrDist = kDiff.SquaredLength();
            if ( fSqrDist < fMinSqrDist )
            {
                fMinSqrDist = fSqrDist;
                if ( pkClosest0 )
                    *pkClosest0 = kClosest0;
                if ( pkClosest1 )
                    *pkClosest1 = kClosest1;
            }
        }
        else
        {
            // TO DO:  Handle this case.  Is there some geometric
            // significance?
            assert( false );
        }
    }

    return fMinSqrDist;
}
//----------------------------------------------------------------------------
Real Mgc::Distance (const Circle3& rkCircle0, const Circle3& rkCircle1,
    Vector3* pkClosest0, Vector3* pkClosest1)
{
    return Math::Sqrt(SqrDistance(rkCircle0,rkCircle1,pkClosest0,pkClosest1));
}
//----------------------------------------------------------------------------


