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
inline Tetrahedron::Tetrahedron ()
{
    // no initialization of vertices
}
//----------------------------------------------------------------------------
inline Vector3& Tetrahedron::operator[] (int i)
{
    assert( 0 <= i && i <= 3 );
    return m_akVertex[i];
}
//----------------------------------------------------------------------------
inline const Vector3& Tetrahedron::operator[] (int i) const
{
    assert( 0 <= i && i <= 3 );
    return m_akVertex[i];
}
//----------------------------------------------------------------------------
