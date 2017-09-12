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
inline ColorRGB::ColorRGB ()
{
    // For efficiency in construction of large arrays of colors,
    // the default constructor does not initialize a color.
}
//----------------------------------------------------------------------------
inline Real& ColorRGB::operator[] (int i) const
{
    return ((Real*)this)[i];
}
//----------------------------------------------------------------------------
inline ColorRGB::operator Real* ()
{
    return (Real*)this;
}
//----------------------------------------------------------------------------


