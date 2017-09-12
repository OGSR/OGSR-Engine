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
inline int Interp2DBicubic::GetXBound () const
{
    return m_iXBound;
}
//----------------------------------------------------------------------------
inline int Interp2DBicubic::GetYBound () const
{
    return m_iYBound;
}
//----------------------------------------------------------------------------
inline int Interp2DBicubic::GetQuantity () const
{
    return m_iQuantity;
}
//----------------------------------------------------------------------------
inline Real** Interp2DBicubic::GetF () const
{
    return m_aafF;
}
//----------------------------------------------------------------------------
inline Real Interp2DBicubic::GetXMin () const
{
    return m_fXMin;
}
//----------------------------------------------------------------------------
inline Real Interp2DBicubic::GetXMax () const
{
    return m_fXMax;
}
//----------------------------------------------------------------------------
inline Real Interp2DBicubic::GetXSpacing () const
{
    return m_fXSpacing;
}
//----------------------------------------------------------------------------
inline Real Interp2DBicubic::GetYMin () const
{
    return m_fYMin;
}
//----------------------------------------------------------------------------
inline Real Interp2DBicubic::GetYMax () const
{
    return m_fYMax;
}
//----------------------------------------------------------------------------
inline Real Interp2DBicubic::GetYSpacing () const
{
    return m_fYSpacing;
}
//----------------------------------------------------------------------------


