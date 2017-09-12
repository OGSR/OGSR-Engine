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

//----------------------------------------------------------------------------
inline void ExtraSpin::SetAngle (Real fAngle)
{
    m_fAngle = fAngle;
}
//----------------------------------------------------------------------------
inline Real ExtraSpin::GetAngle () const
{
    return m_fAngle;
}
//----------------------------------------------------------------------------
inline void ExtraSpin::SetAxis (const Vector3& rkAxis)
{
    m_kAxis = rkAxis;
}
//----------------------------------------------------------------------------
inline const Vector3& ExtraSpin::GetAxis () const
{
    return m_kAxis;
}
//----------------------------------------------------------------------------
inline void ExtraSpin::SetQuaternion (const Quaternion& rkQuat)
{
    m_kQuat = rkQuat;
}
//----------------------------------------------------------------------------
inline const Quaternion& ExtraSpin::GetQuaternion () const
{
    return m_kQuat;
}
//----------------------------------------------------------------------------
inline void ExtraSpin::SetExtraSpins (int iExtraSpins)
{
    m_iExtraSpins = iExtraSpins;
}
//----------------------------------------------------------------------------
inline int ExtraSpin::GetExtraSpins () const
{
    return m_iExtraSpins;
}
//----------------------------------------------------------------------------


