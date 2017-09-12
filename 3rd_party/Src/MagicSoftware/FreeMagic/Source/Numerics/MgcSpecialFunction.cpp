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

#include "MgcSpecialFunction.h"
using namespace Mgc;

//----------------------------------------------------------------------------
Real SpecialFunction::LogGamma (Real fX)
{
    static const Real s_afCoeff[6] =
    {
        76.18009173f, -86.50532033f, 24.01409822f, -1.231739516f,
        0.120858003e-2f, -0.536382e-5f
    };

    fX -= 1.0f;
    Real fTmp = fX + 5.5f;
    fTmp -= (fX+0.5f)*Math::Log(fTmp);
    Real fSeries = 1.0f;
    for (int j = 0; j <= 5; j++)
    {
        fX += 1.0f;
        fSeries += s_afCoeff[j]/fX;
    }
    return -fTmp + Math::Log(2.50662827465f*fSeries);
}
//----------------------------------------------------------------------------
Real SpecialFunction::Gamma (Real fX)
{
    return Math::Exp(LogGamma(fX));
}
//----------------------------------------------------------------------------
Real SpecialFunction::IncompleteGammaS (Real fA, Real fX)
{
    const int iMaxIterations = 100;
    const Real fTolerance = 3e-07f;

    if ( fX > 0.0f )
    {
        Real fAp = fA;
        Real fSum = 1.0f/fA, fDel = fSum;
        for (int i = 1; i <= iMaxIterations; i++)
        {
            fAp += 1.0f;
            fDel *= fX/fAp;
            fSum += fDel;
            if ( Math::FAbs(fDel) < Math::FAbs(fSum)*fTolerance )
            {
                Real fArg = -fX+fA*Math::Log(fX)-LogGamma(fA);
                return fSum*Math::Exp(fArg);
            }
        }
    }

    if ( fX == 0.0f )
        return 0.0f;

    return Math::MAX_REAL; // LogGamma not defined for x < 0
}
//----------------------------------------------------------------------------
Real SpecialFunction::IncompleteGammaCF (Real fA, Real fX)
{
    const int iMaxIterations = 100;
    const Real fTolerance = 3e-07f;

    Real fA0 = 1.0f, fA1 = fX;
    Real fB0 = 0, fB1 = 1.0f;
    Real fGold = 0.0, fFac = 1.0f;

    for (int i = 1; i <= iMaxIterations; i++)
    {
        Real fI = (Real) i;
        Real fImA = fI - fA;
        fA0 = (fA1 + fA0*fImA)*fFac;
        fB0 = (fB1 + fB0*fImA)*fFac;
        Real fItF = fI*fFac;
        fA1 = fX*fA0 + fItF*fA1;
        fB1 = fX*fB0 + fItF*fB1;
        if ( fA1 != 0.0f )
        {
            fFac = 1.0f/fA1;
            Real fG = fB1*fFac;
            if ( Math::FAbs((fG-fGold)/fG) < fTolerance)
            {
                Real fArg = -fX + fA*Math::Log(fX) - LogGamma(fA);
                return fG*Math::Exp(fArg);
            }
            fGold = fG;
        }
    }

    return Math::MAX_REAL;  // numerical error if you get here
}
//----------------------------------------------------------------------------
Real SpecialFunction::IncompleteGamma (Real fA, Real fX)
{
    if ( fX < 1.0f + fA )
        return IncompleteGammaS(fA,fX);
    else
        return 1.0f-IncompleteGammaCF(fA,fX);
}
//----------------------------------------------------------------------------
Real SpecialFunction::Erf (Real fX)
{
    return 1.0f-Erfc(fX);
}
//----------------------------------------------------------------------------
Real SpecialFunction::Erfc (Real fX)
{
    static const Real s_afCoeff[10] =
    {
        -1.26551223f,  1.00002368f, 0.37409196f,  0.09678418f, -0.18628806f,
         0.27886807f, -1.13520398f, 1.48851587f, -0.82215223f,  0.17087277f
    };

    Real fZ = Math::FAbs(fX);
    Real fT = 1.0f/(1.0f+0.5f*fZ);
    Real fSum = s_afCoeff[9];

    for (int i = 9; i >= 0; i--)
        fSum = fT*fSum + s_afCoeff[i];

    Real fResult = fT*Math::Exp(-fZ*fZ + fSum);

    return fX >= 0.0f ? fResult : 2.0f - fResult;
}
//----------------------------------------------------------------------------
Real SpecialFunction::ModBessel0 (Real fX)
{
    if ( fX < 0.0f )  // function is even
        fX = -fX;

    Real fT, fResult;
    int i;

    if ( fX <= 3.75f )
    {
        static const Real s_afCoeff[7] =
        {
            +1.0000000f, +3.5156229f, +3.0899424f, +1.2067492f, +0.2659732f,
            +0.0360768f, +0.0045813f
        };

        fT = fX/3.75f;
        Real fT2 = fT*fT;
        fResult = s_afCoeff[6];
        for (i = 5; i >= 0; i--)
        {
            fResult *= fT2;
            fResult += s_afCoeff[i];
        }
        // |error| < 1.6e-07
    }
    else
    {
        static const Real s_afCoeff[9] =
        {
            +0.39894228f, +0.01328592f, +0.00225319f, -0.00157565f,
            +0.00916281f, -0.02057706f, +0.02635537f, -0.01647633f,
            +0.00392377f
        };

        fT = fX/3.75f;
        Real fInvT = 1.0f/fT;
        fResult = s_afCoeff[8];
        for (i = 7; i >= 0; i--)
        {
            fResult *= fInvT;
            fResult += s_afCoeff[i];
        }
        fResult *= Math::Exp(fX);
        fResult /= Math::Sqrt(fX);
        // |error| < 1.9e-07
    }

    return fResult;
}
//----------------------------------------------------------------------------
Real SpecialFunction::ModBessel1 (Real fX)
{
    int iSign;
    if ( fX > 0.0f )
    {
        iSign = 1;
    }
    else if ( fX < 0.0f )
    {
        fX = -fX;
        iSign = -1;
    }
    else
    {
        return 0.0f;
    }

    Real fT, fResult;
    int i;

    if ( fX <= 3.75f )
    {
        static const Real s_afCoeff[7] =
        {
            +0.50000000f, +0.87890549f, +0.51498869f, +0.15084934f,
            +0.02658733f, +0.00301532f, +0.00032411f
        };

        fT = fX/3.75f;
        Real fT2 = fT*fT;
        fResult = s_afCoeff[6];
        for (i = 5; i >= 0; i--)
        {
            fResult *= fT2;
            fResult += s_afCoeff[i];
        }
        fResult *= fX;
        // |error| < 8e-09
    }
    else
    {
        static const Real s_afCoeff[9] =
        {
            +0.39894228f, -0.03988024f, -0.00362018f, +0.00163801f,
            -0.01031555f, +0.02282967f, -0.02895312f, +0.01787654f,
            -0.00420059f
        };

        fT = fX/3.75f;
        Real fInvT = 1.0f/fT;
        fResult = s_afCoeff[8];
        for (i = 7; i >= 0; i--)
        {
            fResult *= fInvT;
            fResult += s_afCoeff[i];
        }
        fResult *= Math::Exp(fX);
        fResult /= Math::Sqrt(fX);
        // |error| < 2.2e-07
    }

    fResult *= iSign;
    return fResult;
}
//----------------------------------------------------------------------------

