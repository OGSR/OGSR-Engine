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

#include "MgcEllipsoidGeodesicDist.h"
#include "MgcIntegrate.h"
using namespace Mgc;

// This is experimental code.  It appears to work so far.  Numeric
// parameters that may be varied to see how the performance varies
// are
//
//   1.  ds, the default spacing between sample points on the initial curve
//   2.  tmax, the number of iterations in the relaxation.  It should be
//       possible instead to terminate the relaxation when the change in
//       arc length is below a given threshold.
//   3.  factor, the ratio dt/ds^2.  Try to choose as large as possible
//       while retaining numerical stability.

//----------------------------------------------------------------------------
// These are for the initial curve calculations.  They need to be set before
// evaluating speed.
static Real gs_fPP, gs_fPQ, gs_fQQ, gs_fPDQ;
//----------------------------------------------------------------------------
static Real Speed (Real fT, void*)
{
    Real fUVal = -Math::Sin(fT)/Math::Sqrt(2.0f*(1.0f+gs_fPDQ));
    Real fVVal = Math::Cos(fT)/Math::Sqrt(2.0f*(1.0f-gs_fPDQ));
    Real fDAlph = fUVal - fVVal;
    Real fDBeta = fUVal + fVVal;
    Real fTmp = fDAlph*fDAlph*gs_fPP + 2.0f*fDAlph*fDBeta*gs_fPQ +
        fDBeta*fDBeta*gs_fQQ;
    return Math::Sqrt(Math::FAbs(fTmp));
}
//----------------------------------------------------------------------------
Real Mgc::EllipsoidGeodesicDist (Real afAxis[3], const Vector3& rkP,
    const Vector3& rkQ)
{
    // Calculate products P*P, P*Q, Q*Q, and P*DQ where
    // D = diag((1/a0)^2,(1/a1)^2,(1/a2)^2).
    gs_fPP = rkP.Dot(rkP);
    gs_fPQ = rkP.Dot(rkQ);
    gs_fQQ = rkQ.Dot(rkQ);
    gs_fPDQ =
        rkP.x*rkQ.x/(afAxis[0]*afAxis[0]) +
        rkP.y*rkQ.y/(afAxis[1]*afAxis[1]) +
        rkP.z*rkQ.z/(afAxis[2]*afAxis[2]);

    // length of initial curve
    Real fLimit = Math::ATan(Math::Sqrt((1.0f-gs_fPDQ)/(1.0f+gs_fPDQ)));
    Real fLength = Integrate::RombergIntegral(-fLimit,fLimit,Speed);

    // calculate number of samples for the default spacing
    Real fDS = 0.01f;
    int iQuantity = 1 + int(Math::Ceil(fLength/fDS));
    fDS = fLength/(iQuantity-1);

    // allocate N sample points for the evolution scheme
    Real** aafX = new Real*[iQuantity];
    Real** aafY = new Real*[iQuantity];
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        aafX[i] = new Real[3];
        aafY[i] = new Real[3];
    }

    // Calculate initial curve as a planar elliptic arc from P to Q.
    // The curve is
    //   C(t) = a(t) P + b(t) Q
    // where
    //   a(t) = cos(t)/sqrt(2*(1+r)) - sin(t)/sqrt(2*(1-r))
    //   b(t) = cos(t)/sqrt(2*(1+r)) + sin(t)/sqrt(2*(1-r))
    //   r = P^T D Q,  D = diag((1/a0)^2,(1/a1)^2,(1/a2)^2)
    //   -t0 <= t <= t0, t0 = Tan^{-1}(sqrt((1-r)/(1+r)))
    //
    // Sample points are chosen to be uniformly spaced along curve.

    // end points remain fixed
    int iDim;
    for (iDim = 0; iDim < 3; iDim++)
    {
        aafX[0][iDim] = rkP[iDim];
        aafY[0][iDim] = rkP[iDim];
        aafX[iQuantity-1][iDim] = rkQ[iDim];
        aafY[iQuantity-1][iDim] = rkQ[iDim];
    }

    // compute evenly spaced points
    for (i = 1; i < iQuantity-1; i++)
    {
        // find point along curve at this distance from P
        Real fPartialLength = i*fDS;

        // use Newton's method
        Real fAngle = -fLimit + 2.0f*i*fLimit/(iQuantity-1);
        const int iMaxNewton = 32;
        int iIter;
        for (iIter = 0; iIter < iMaxNewton; iIter++)
        {
            Real fG = Integrate::RombergIntegral(-fLimit,fAngle,Speed) -
                fPartialLength;
            if ( Math::FAbs(fG) <= 1e-04f )
                break;
            Real fDG = Speed(fAngle,NULL);
            if ( Math::FAbs(fDG) <= 1e-04f )
            {
                iIter = iMaxNewton;
                break;
            }
            fAngle -= fG/fDG;
        }
        if ( iIter == iMaxNewton )
        {
            // Newton's failed.  Deallocate the sample points and
            // return an invalid distance.
            for (i = 0; i < iQuantity; i++)
            {
                delete[] aafX[i];
                delete[] aafY[i];
            }
            delete[] aafX;
            delete[] aafY;
            return -1.0f;
        }

        Real fUVal = Math::Cos(fAngle)/Math::Sqrt(2.0f*(1.0f+gs_fPDQ));
        Real fVVal = Math::Sin(fAngle)/Math::Sqrt(2.0f*(1.0f-gs_fPDQ));
        Real fAlph = fUVal - fVVal;
        Real fBeta = fUVal + fVVal;
        for (iDim = 0; iDim < 3; iDim++)
            aafX[i][iDim] = fAlph*rkP[iDim] + fBeta*rkQ[iDim];
    }

    // The general evolution scheme for finding geodesics is
    //
    //   X_t = X_{ss} - (X_{ss}*N) N, t > 0
    //   X(s,0) = C(s)  [initial conditions]
    //   X(0,t) = P, X(L,t) = Q  [boundary conditions, L=curve length]
    //
    // N is a unit normal to the ellipsoid.  Try to use central difference
    // in s and forward difference in t.  This approach generally can be
    // numerically unstable.  I want to see if the special case of an
    // ellipsoid is stable.

    const int iTMax = 16;
    int iT;
    for (iT = 1; iT <= iTMax; iT++)
    {
        Real fDot, afNorm[3], fNorLen;

        // take a time step
        for (i = 1; i < iQuantity-1; i++)
        {
            // compute X_{ss}*N
            Real afDiff[3];
            fNorLen = 0.0f;
            for (iDim = 0; iDim < 3; iDim++)
            {
                afDiff[iDim] = aafX[i+1][iDim] - 2.0f*aafX[i][iDim] +
                    aafX[i-1][iDim];
                afNorm[iDim] = aafX[i][iDim]/(afAxis[iDim]*afAxis[iDim]);
                fNorLen += afNorm[iDim]*afNorm[iDim];
            }
            fNorLen = Math::Sqrt(fNorLen);
            for (iDim = 0; iDim < 3; iDim++)
                afNorm[iDim] /= fNorLen;
            fDot = afDiff[0]*afNorm[0] + afDiff[1]*afNorm[1] +
                afDiff[2]*afNorm[2];

            // evaluate the next point in time, dt/ds^2 = 0.125
            const Real fFactor = 0.125f;
            for (iDim = 0; iDim < 3; iDim++)
            {
                aafY[i][iDim] = aafX[i][iDim] + fFactor*(afDiff[iDim] -
                    fDot*afNorm[iDim]);
            }
        }

        // project back onto ellipsoid
        for (i = 1; i < iQuantity-1; i++)
        {
            Real fLevel = 0.0f;
            for (iDim = 0; iDim < 3; iDim++)
            {
                Real fTmp = aafY[i][iDim]/afAxis[iDim];
                fLevel += fTmp*fTmp;
            }
            fLevel = Math::Sqrt(fLevel);
            for (iDim = 0; iDim < 3; iDim++)
                aafX[i][iDim] = aafY[i][iDim]/fLevel;
        }
    }

    // compute length of polyline
    fLength = 0.0f;
    for (i = 1; i < iQuantity; i++)
    {
        Real fSegLen = 0.0f;
        for (iDim = 0; iDim < 3; iDim++)
        {
            Real fTmp = aafX[i][iDim] - aafX[i-1][iDim];
            fSegLen += fTmp*fTmp;
        }
        fLength += Math::Sqrt(fSegLen);
    }

    // deallocate the sample points
    for (i = 0; i < iQuantity; i++)
    {
        delete[] aafX[i];
        delete[] aafY[i];
    }
    delete[] aafX;
    delete[] aafY;

    return fLength;
}
//----------------------------------------------------------------------------

#ifdef ELPSGEO_TEST
#include "MgcMath.h"

void TestSphere ()
{
    Real afAxis[3] = { 1.0f, 1.0f, 1.0f };
    Vector3 kP, kQ;
    Real fMaxError = 0.0f;
    int iMaxSim = -1;

    for (int iSim = 0; iSim < 32; iSim++)
    {
        kP.x = Math::SymmetricRandom();
        kP.y = Math::SymmetricRandom();
        kP.z = Math::SymmetricRandom();
        kQ.x = Math::SymmetricRandom();
        kQ.y = Math::SymmetricRandom();
        kQ.z = Math::SymmetricRandom();
        kP.Unitize();
        kQ.Unitize();

        Real fAngle = Math::ACos(kP.Dot(kQ));
        Real fDistance = EllipsoidGeodesicDist(afAxis,kP,kQ);
        Real fError = Math::FAbs(fAngle-fDistance);
        if ( fError > fMaxError )
        {
            fMaxError = fError;
            iMaxSim = iSim;
        }
    }
}

void TestEllipsoid ()
{
    Real afAxis[3] = { 1.0f, 2.0f, 3.0f };
    Vector3 kP, kQ;
    Real fMaxError = 0.0f;
    int iMaxSim = -1;

    // geodesic is planar elliptic arc in xy-plane
    for (int iSim = 0; iSim < 32; iSim++)
    {
        kP.x = Math::SymmetricRandom();
        kP.y = Math::SymmetricRandom();
        kP.z = 0.0f;
        kQ.x = Math::SymmetricRandom();
        kQ.y = Math::SymmetricRandom();
        kQ.z = 0.0f;
        kP.Unitize();
        kQ.Unitize();
        for (int i = 0; i < 3; i++)
        {
            kP[i] *= afAxis[i];
            kQ[i] *= afAxis[i];
        }

        gs_fPP = kP.Dot(kP);
        gs_fPQ = kP.Dot(kQ);
        gs_fQQ = kQ.Dot(kQ);
        gs_fPDQ =
            kP.x*kQ.x/(afAxis[0]*afAxis[0]) +
            kP.y*kQ.y/(afAxis[1]*afAxis[1]) +
            kP.z*kQ.z/(afAxis[2]*afAxis[2]);
        Real fLimit = Math::ATan(Math::Sqrt((1.0f-gs_fPDQ)/(1.0f+gs_fPDQ)));
        Real fActual = Integrate::RombergIntegral(-fLimit,fLimit,Speed);
        Real fDistance = EllipsoidGeodesicDist(afAxis,kP,kQ);
        Real fError = Math::FAbs(fActual-fDistance);
        if ( fError > fMaxError )
        {
            fMaxError = fError;
            iMaxSim = iSim;
        }
    }
}

void StandardUse ()
{
    Real afAxis[3] = { 1.0f, 2.0f, 3.0f };
    Vector3 kP, kQ;

    // Construct two random points on the ellipsoid given by
    // (x/a0)^2 + (y/a1)^2 + (z/a2)^2 = 1.
    kP.x = Math::SymmetricRandom();
    kP.y = Math::SymmetricRandom();
    kP.z = Math::SymmetricRandom();
    kQ.x = Math::SymmetricRandom();
    kQ.y = Math::SymmetricRandom();
    kQ.z = Math::SymmetricRandom();
    kP.Unitize();
    kQ.Unitize();
    for (int i = 0; i < 3; i++)
    {
        kP[i] *= afAxis[i];
        kQ[i] *= afAxis[i];
    }

    Real fDistance = EllipsoidGeodesicDist(afAxis,kP,kQ);
}

int main ()
{
    TestSphere();
    TestEllipsoid();
    StandardUse();
    return 0;
}

#endif


