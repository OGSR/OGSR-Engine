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
inline Arc2::Arc2 ()
{
    // no initialization for efficiency
}
//----------------------------------------------------------------------------
inline Vector2& Arc2::End0 ()
{
    return m_kEnd0;
}
//----------------------------------------------------------------------------
inline const Vector2& Arc2::End0 () const
{
    return m_kEnd0;
}
//----------------------------------------------------------------------------
inline Vector2& Arc2::End1 ()
{
    return m_kEnd1;
}
//----------------------------------------------------------------------------
inline const Vector2& Arc2::End1 () const
{
    return m_kEnd1;
}
//----------------------------------------------------------------------------
inline bool Arc2::Contains (const Vector2& rkP) const
{
    // Assert: |P-C| = R where P is the input point, C is the circle center,
    // and R is the circle radius.  For P to be on the arc from A to B, it
    // must be on the side of the plane containing A with normal N = Perp(B-A)
    // where Perp(u,v) = (v,-u).

    Real fLhs = (rkP.x - m_kEnd0.x)*(m_kEnd1.y - m_kEnd0.y);
    Real fRhs = (rkP.y - m_kEnd0.y)*(m_kEnd1.x - m_kEnd0.x);
    return fLhs >= fRhs;
}
//----------------------------------------------------------------------------


