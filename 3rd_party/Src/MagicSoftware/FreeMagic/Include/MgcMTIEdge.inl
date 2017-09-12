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
inline MTIEdge::MTIEdge (int iLabel0, int iLabel1)
{
    if ( iLabel0 < iLabel1 )
    {
        // L0 is minimum
        m_aiLabel[0] = iLabel0;
        m_aiLabel[1] = iLabel1;
    }
    else
    {
        // L1 is minimum
        m_aiLabel[0] = iLabel1;
        m_aiLabel[1] = iLabel0;
    }
}
//----------------------------------------------------------------------------
inline int MTIEdge::GetLabel (int i) const
{
    assert( 0 <= i && i < 2 );
    return m_aiLabel[i];
}
//----------------------------------------------------------------------------
inline bool MTIEdge::operator< (const MTIEdge& rkE) const
{
    if ( m_aiLabel[1] < rkE.m_aiLabel[1] )
        return true;

    if ( m_aiLabel[1] == rkE.m_aiLabel[1] )
        return m_aiLabel[0] < rkE.m_aiLabel[0];

    return false;
}
//----------------------------------------------------------------------------
inline bool MTIEdge::operator== (const MTIEdge& rkE) const
{
    return m_aiLabel[0] == rkE.m_aiLabel[0]
        && m_aiLabel[1] == rkE.m_aiLabel[1];
}
//----------------------------------------------------------------------------
inline bool MTIEdge::operator!= (const MTIEdge& rkE) const
{
    return !operator==(rkE);
}
//----------------------------------------------------------------------------

