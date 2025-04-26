// PHDynamicData.h: interface for the PHDynamicData class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "ode_include.h"
#include "PHInterpolation.h"
#include "MathUtils.h"


class PHDynamicData;

class PHDynamicData
{
public:
    static inline void DMXPStoFMX(const dReal* R, const dReal* pos, Fmatrix& aTransform)
    {
        CopyMemory(&aTransform, R, sizeof(dMatrix3));
        aTransform.transpose();
        CopyMemory(&aTransform.c, pos, sizeof(Fvector));
        aTransform._14 = 0.f;
        aTransform._24 = 0.f;
        aTransform._34 = 0.f;
        aTransform._44 = 1.f;
    };
    static inline void DMXtoFMX(const dReal* R, Fmatrix& aTransform)
    {
        aTransform._11 = R[0];
        aTransform._12 = R[4];
        aTransform._13 = R[8];
        aTransform._14 = 0.f;

        aTransform._21 = R[1];
        aTransform._22 = R[5];
        aTransform._23 = R[9];
        aTransform._24 = 0.f;

        aTransform._31 = R[2];
        aTransform._32 = R[6];
        aTransform._33 = R[10];
        aTransform._34 = 0.f;
        aTransform._44 = 1.f;
    };
    static inline void FMX33toDMX(const Fmatrix33& aTransform, dReal* R)
    {
        R[0] = aTransform._11;
        R[4] = aTransform._12;
        R[8] = aTransform._13;

        R[1] = aTransform._21;
        R[5] = aTransform._22;
        R[9] = aTransform._23;

        R[2] = aTransform._31;
        R[6] = aTransform._32;
        R[10] = aTransform._33;
    };
    static inline void FMXtoDMX(const Fmatrix& aTransform, dReal* R)
    {
        R[0] = aTransform._11;
        R[4] = aTransform._12;
        R[8] = aTransform._13;

        R[1] = aTransform._21;
        R[5] = aTransform._22;
        R[9] = aTransform._23;

        R[2] = aTransform._31;
        R[6] = aTransform._32;
        R[10] = aTransform._33;
    }
};
