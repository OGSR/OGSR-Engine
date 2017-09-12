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
inline MTIVertex::MTIVertex (int iLabel)
{
    m_iLabel = iLabel;
}
//----------------------------------------------------------------------------
inline int MTIVertex::GetLabel () const
{
    return m_iLabel;
}
//----------------------------------------------------------------------------
inline bool MTIVertex::operator< (const MTIVertex& rkV) const
{
    return m_iLabel < rkV.m_iLabel;
}
//----------------------------------------------------------------------------
inline bool MTIVertex::operator== (const MTIVertex& rkV) const
{
    return m_iLabel == rkV.m_iLabel;
}
//----------------------------------------------------------------------------
inline bool MTIVertex::operator!= (const MTIVertex& rkV) const
{
    return m_iLabel != rkV.m_iLabel;
}
//----------------------------------------------------------------------------
