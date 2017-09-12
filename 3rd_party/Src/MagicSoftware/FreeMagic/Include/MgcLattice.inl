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
inline int Lattice::GetDimensions () const
{
    return m_iDimensions;
}
//----------------------------------------------------------------------------
inline const int* Lattice::GetBounds () const
{
    return m_aiBound;
}
//----------------------------------------------------------------------------
inline int Lattice::GetBound (int i) const
{
    assert( i < m_iDimensions );
    return m_aiBound[i];
}
//----------------------------------------------------------------------------
inline int Lattice::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
inline const int* Lattice::GetOffsets () const
{
    return m_aiOffset;
}
//----------------------------------------------------------------------------
inline int Lattice::GetOffset (int i) const
{
    assert( i < m_iDimensions );
    return m_aiOffset[i];
}
//----------------------------------------------------------------------------


