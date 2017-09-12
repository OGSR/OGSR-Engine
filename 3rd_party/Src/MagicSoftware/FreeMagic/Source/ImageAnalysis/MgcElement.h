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

#ifndef MGCELEMENT_H
#define MGCELEMENT_H

#include "MagicFMLibType.h"

// wrappers for native types
const unsigned int MGC_ELEMENT_QUANTITY = 12;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short rgb5;
typedef unsigned int rgb8;

#define MgcDeclareElement(T) \
class MAGICFM E##T \
{ \
public: \
    E##T (T tValue = 0) { m_tValue = tValue; } \
    \
    E##T& operator= (E##T kElement) \
    { \
        m_tValue = kElement.m_tValue; \
        return *this; \
    } \
    \
    operator T () { return m_tValue; } \
    \
    static int GetRTTI () { return ms_iRTTI; } \
    \
protected: \
    T m_tValue; \
    static const int ms_iRTTI; \
}

#define MgcImplementElement(T,iRTTI) \
const int E##T::ms_iRTTI = iRTTI

namespace Mgc {

MgcDeclareElement(char);
MgcDeclareElement(uchar);
MgcDeclareElement(short);
MgcDeclareElement(ushort);
MgcDeclareElement(int);
MgcDeclareElement(uint);
MgcDeclareElement(long);
MgcDeclareElement(ulong);
MgcDeclareElement(float);
MgcDeclareElement(double);
MgcDeclareElement(rgb5);
MgcDeclareElement(rgb8);

#include "MgcElement.inl"

} // namespace Mgc

#endif


