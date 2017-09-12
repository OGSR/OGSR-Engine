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
inline int MTVertex::GetLabel () const
{
    return m_iLabel;
}
//----------------------------------------------------------------------------
inline int MTVertex::GetEdgeQuantity () const
{
    return m_kESet.GetQuantity();
}
//----------------------------------------------------------------------------
inline int MTVertex::GetEdge (int i) const
{
    return m_kESet.Get(i);
}
//----------------------------------------------------------------------------
inline int MTVertex::GetTriangleQuantity () const
{
    return m_kTSet.GetQuantity();
}
//----------------------------------------------------------------------------
inline int MTVertex::GetTriangle (int i) const
{
    return m_kTSet.Get(i);
}
//----------------------------------------------------------------------------
