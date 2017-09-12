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
inline Line2::Line2 ()
{
    // no initialization for efficiency
}
//----------------------------------------------------------------------------
inline Line2::Line2 (const Vector2& rkV0, const Vector2& rkV1,
    bool bParametricForm)
{
    Vector2 kDiff = rkV1 - rkV0;

    if ( bParametricForm )
    {
        m_kOrigin = rkV0;
        m_kDirection = kDiff;
    }
    else
    {
        m_kNormal = (rkV1-rkV0).Cross();
        m_fConstant = m_kNormal.Dot(rkV0);
    }
}
//----------------------------------------------------------------------------
inline Vector2& Line2::Origin ()
{
    return m_kOrigin;
}
//----------------------------------------------------------------------------
inline const Vector2& Line2::Origin () const
{
    return m_kOrigin;
}
//----------------------------------------------------------------------------
inline Vector2& Line2::Direction ()
{
    return m_kDirection;
}
//----------------------------------------------------------------------------
inline const Vector2& Line2::Direction () const
{
    return m_kDirection;
}
//----------------------------------------------------------------------------
inline Vector2& Line2::Normal ()
{
    return m_kNormal;
}
//----------------------------------------------------------------------------
inline const Vector2& Line2::Normal () const
{
    return m_kNormal;
}
//----------------------------------------------------------------------------
inline Real& Line2::Constant ()
{
    return m_fConstant;
}
//----------------------------------------------------------------------------
inline const Real& Line2::Constant () const
{
    return m_fConstant;
}
//----------------------------------------------------------------------------
inline Real Line2::GetPseudodistance (const Vector2& rkV) const
{
    return m_kNormal.Dot(rkV) - m_fConstant;
}
//----------------------------------------------------------------------------


