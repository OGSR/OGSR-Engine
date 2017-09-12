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
inline Real RK4Adapt::GetSafety () const
{
    return m_fSafety;
}
//----------------------------------------------------------------------------
inline void RK4Adapt::SetEpsilon (Real fEpsilon)
{
    m_fEpsilon = fEpsilon;
}
//----------------------------------------------------------------------------
inline Real RK4Adapt::GetEpsilon () const
{
    return m_fEpsilon;
}
//----------------------------------------------------------------------------
inline Real RK4Adapt::GetStepUsed () const
{
    return m_fStepUsed;
}
//----------------------------------------------------------------------------
inline int RK4Adapt::GetPasses () const
{
    return m_iPasses;
}
//----------------------------------------------------------------------------

