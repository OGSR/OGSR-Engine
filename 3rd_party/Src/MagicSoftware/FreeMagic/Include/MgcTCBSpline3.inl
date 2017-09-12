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
inline const Vector3* TCBSpline3::GetPoints () const
{
    return m_akPoint;
}
//----------------------------------------------------------------------------
inline const Real* TCBSpline3::GetTensions () const
{
    return m_afTension;
}
//----------------------------------------------------------------------------
inline const Real* TCBSpline3::GetContinuities () const
{
    return m_afContinuity;
}
//----------------------------------------------------------------------------
inline const Real* TCBSpline3::GetBiases () const
{
    return m_afBias;
}
//----------------------------------------------------------------------------


