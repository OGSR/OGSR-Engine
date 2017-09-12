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
inline const std::vector<Vector3>& ConvexPolyhedron::GetPoints ()
    const
{
    return m_akPoint;
}
//----------------------------------------------------------------------------
inline const Vector3& ConvexPolyhedron::GetPoint (int iV) const
{
    assert( 0 <= iV && iV < (int)m_akPoint.size() );
    return m_akPoint[iV];
}
//----------------------------------------------------------------------------
inline int ConvexPolyhedron::AddPoint (const Vector3& rkPoint)
{
    int iLocation = m_akPoint.size();
    m_akPoint.push_back(rkPoint);
    return iLocation;
}
//----------------------------------------------------------------------------
inline std::vector<Vector3>& ConvexPolyhedron::Points ()
{
    return m_akPoint;
}
//----------------------------------------------------------------------------
inline Vector3& ConvexPolyhedron::Point (int iV)
{
    assert( 0 <= iV && iV < (int)m_akPoint.size() );
    return m_akPoint[iV];
}
//----------------------------------------------------------------------------
inline const std::vector<Plane>& ConvexPolyhedron::GetPlanes ()
    const
{
    return m_akPlane;
}
//----------------------------------------------------------------------------
inline const Plane& ConvexPolyhedron::GetPlane (int iT) const
{
    assert( 0 <= iT && iT < (int)m_akPlane.size() );
    return m_akPlane[iT];
}
//----------------------------------------------------------------------------
inline const Vector3& ConvexPolyhedron::GetCentroid () const
{
    return m_kCentroid;
}
//----------------------------------------------------------------------------
