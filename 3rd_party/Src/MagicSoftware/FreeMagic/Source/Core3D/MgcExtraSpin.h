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

#ifndef MGCEXTRASPIN_H
#define MGCEXTRASPIN_H

#include "MgcQuaternion.h"

namespace Mgc {


class MAGICFM ExtraSpin
{
public:
    ExtraSpin ();

    // attributes
    void SetAngle (Real fAngle);
    Real GetAngle () const;
    void SetAxis (const Vector3& rkAxis);
    const Vector3& GetAxis () const;
    void SetQuaternion (const Quaternion& rkQuat);
    const Quaternion& GetQuaternion () const;
    void SetExtraSpins (int iExtraSpins);
    int GetExtraSpins () const;

    // Preprocess sequence of rotation keys.  This call determines if extra
    // spins are required and what type of interpolation should occur between
    // consecutive pairs of keys.  This must be called before interpolation
    // of pairs of keys in the sequence.
    static void PreprocessKeys (int iNumKeys, ExtraSpin* akKey);

    // linear-style interpolation of two rotation keys
    void Interpolate (Real fTime, const ExtraSpin& rkNextKey,
        ExtraSpin& rkInterpKey);

protected:
    Real m_fAngle;
    Vector3 m_kAxis;
    Quaternion m_kQuat;

    enum ExtraSpinInfo
    {
        NONE,               // no extra spins
        SAME_AXIS,          // extra spins, same axis of rotation
        DIFF_AXIS_NO_ZERO,  // extra spins, different axes, no zero angles
        DIFF_AXIS_ZERO      // extra spins, different axes, zero angles
    };

    int m_iExtraSpins;
    ExtraSpinInfo m_eInfo;

    static void InterpolateAxis (Real fTime, const Vector3& rkAxis0,
        const Vector3& rkAxis1, Vector3& rkInterpAxis);
};

#include "MgcExtraSpin.inl"

} // namespace Mgc

#endif


