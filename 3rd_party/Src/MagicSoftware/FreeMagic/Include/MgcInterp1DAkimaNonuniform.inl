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
inline const Real* Interp1DAkimaNonuniform::GetX () const
{
    return m_afX;
}
//----------------------------------------------------------------------------
inline Real Interp1DAkimaNonuniform::GetXMin () const
{
    return m_afX[0];
}
//----------------------------------------------------------------------------
inline Real Interp1DAkimaNonuniform::GetXMax () const
{
    return m_afX[m_iQuantity-1];
}
//----------------------------------------------------------------------------


