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
inline Triangle2::Triangle2 ()
{
    // no initialization for efficiency
}
//----------------------------------------------------------------------------
inline Vector2& Triangle2::Origin ()
{
    return m_kOrigin;
}
//----------------------------------------------------------------------------
inline const Vector2& Triangle2::Origin () const
{
    return m_kOrigin;
}
//----------------------------------------------------------------------------
inline Vector2& Triangle2::Edge0 ()
{
    return m_kEdge0;
}
//----------------------------------------------------------------------------
inline const Vector2& Triangle2::Edge0 () const
{
    return m_kEdge0;
}
//----------------------------------------------------------------------------
inline Vector2& Triangle2::Edge1 ()
{
    return m_kEdge1;
}
//----------------------------------------------------------------------------
inline const Vector2& Triangle2::Edge1 () const
{
    return m_kEdge1;
}
//----------------------------------------------------------------------------
inline Vector2& Triangle2::Vertex (unsigned int uiI)
{
    assert( uiI < 3 );
    return m_akV[uiI];
}
//----------------------------------------------------------------------------
inline const Vector2& Triangle2::Vertex (unsigned int uiI) const
{
    assert( uiI < 3 );
    return m_akV[uiI];
}
//----------------------------------------------------------------------------


