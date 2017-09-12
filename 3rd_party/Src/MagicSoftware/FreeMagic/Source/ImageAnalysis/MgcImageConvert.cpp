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

#include "MgcElement.h"
#include "MgcImageConvert.h"
using namespace Mgc;

#include <cassert>
#include <cstddef>

//----------------------------------------------------------------------------
#define CONVERTER(SrcType,TrgType) \
static void SrcType##To##TrgType (int iQuantity, void* pvSrc, void* pvTrg) \
{ \
    SrcType* ptSrc = (SrcType*)pvSrc; \
    TrgType* ptTrg = (TrgType*)pvTrg; \
    for (int i = 0; i < iQuantity; i++, ptSrc++, ptTrg++) \
        *ptTrg = (TrgType)(*ptSrc); \
}
//----------------------------------------------------------------------------
CONVERTER(char,uchar)
CONVERTER(char,short)
CONVERTER(char,ushort)
CONVERTER(char,int)
CONVERTER(char,uint)
CONVERTER(char,long)
CONVERTER(char,ulong)
CONVERTER(char,float)
CONVERTER(char,double)
CONVERTER(char,rgb5)
CONVERTER(char,rgb8)

CONVERTER(uchar,char)
CONVERTER(uchar,short)
CONVERTER(uchar,ushort)
CONVERTER(uchar,int)
CONVERTER(uchar,uint)
CONVERTER(uchar,long)
CONVERTER(uchar,ulong)
CONVERTER(uchar,float)
CONVERTER(uchar,double)
CONVERTER(uchar,rgb5)
CONVERTER(uchar,rgb8)

CONVERTER(short,char)
CONVERTER(short,uchar)
CONVERTER(short,ushort)
CONVERTER(short,int)
CONVERTER(short,uint)
CONVERTER(short,long)
CONVERTER(short,ulong)
CONVERTER(short,float)
CONVERTER(short,double)
CONVERTER(short,rgb5)
CONVERTER(short,rgb8)

CONVERTER(ushort,char)
CONVERTER(ushort,uchar)
CONVERTER(ushort,short)
CONVERTER(ushort,int)
CONVERTER(ushort,uint)
CONVERTER(ushort,long)
CONVERTER(ushort,ulong)
CONVERTER(ushort,float)
CONVERTER(ushort,double)
CONVERTER(ushort,rgb5)
CONVERTER(ushort,rgb8)

CONVERTER(int,char)
CONVERTER(int,uchar)
CONVERTER(int,short)
CONVERTER(int,ushort)
CONVERTER(int,uint)
CONVERTER(int,long)
CONVERTER(int,ulong)
CONVERTER(int,float)
CONVERTER(int,double)
CONVERTER(int,rgb5)
CONVERTER(int,rgb8)

CONVERTER(uint,char)
CONVERTER(uint,uchar)
CONVERTER(uint,short)
CONVERTER(uint,ushort)
CONVERTER(uint,int)
CONVERTER(uint,long)
CONVERTER(uint,ulong)
CONVERTER(uint,float)
CONVERTER(uint,double)
CONVERTER(uint,rgb5)
CONVERTER(uint,rgb8)

CONVERTER(long,char)
CONVERTER(long,uchar)
CONVERTER(long,short)
CONVERTER(long,ushort)
CONVERTER(long,int)
CONVERTER(long,uint)
CONVERTER(long,ulong)
CONVERTER(long,float)
CONVERTER(long,double)
CONVERTER(long,rgb5)
CONVERTER(long,rgb8)

CONVERTER(ulong,char)
CONVERTER(ulong,uchar)
CONVERTER(ulong,short)
CONVERTER(ulong,ushort)
CONVERTER(ulong,int)
CONVERTER(ulong,uint)
CONVERTER(ulong,long)
CONVERTER(ulong,float)
CONVERTER(ulong,double)
CONVERTER(ulong,rgb5)
CONVERTER(ulong,rgb8)

CONVERTER(float,char)
CONVERTER(float,uchar)
CONVERTER(float,short)
CONVERTER(float,ushort)
CONVERTER(float,int)
CONVERTER(float,uint)
CONVERTER(float,long)
CONVERTER(float,ulong)
CONVERTER(float,double)
CONVERTER(float,rgb5)
CONVERTER(float,rgb8)

CONVERTER(double,char)
CONVERTER(double,uchar)
CONVERTER(double,short)
CONVERTER(double,ushort)
CONVERTER(double,int)
CONVERTER(double,uint)
CONVERTER(double,long)
CONVERTER(double,ulong)
CONVERTER(double,float)
CONVERTER(double,rgb5)
CONVERTER(double,rgb8)

CONVERTER(rgb5,char)
CONVERTER(rgb5,uchar)
CONVERTER(rgb5,short)
CONVERTER(rgb5,ushort)
CONVERTER(rgb5,int)
CONVERTER(rgb5,uint)
CONVERTER(rgb5,long)
CONVERTER(rgb5,ulong)
CONVERTER(rgb5,float)
CONVERTER(rgb5,double)
CONVERTER(rgb5,rgb8)

CONVERTER(rgb8,char)
CONVERTER(rgb8,uchar)
CONVERTER(rgb8,short)
CONVERTER(rgb8,ushort)
CONVERTER(rgb8,int)
CONVERTER(rgb8,uint)
CONVERTER(rgb8,long)
CONVERTER(rgb8,ulong)
CONVERTER(rgb8,float)
CONVERTER(rgb8,double)
CONVERTER(rgb8,rgb5)
//----------------------------------------------------------------------------
#define ENTRY(SrcType,TrgType) SrcType##To##TrgType
//----------------------------------------------------------------------------
typedef void (*Converter)(int,void*,void*);
static Converter gs_aaoConvert[MGC_ELEMENT_QUANTITY][MGC_ELEMENT_QUANTITY] =
{
    { NULL,
      ENTRY(char,uchar),
      ENTRY(char,short),
      ENTRY(char,ushort),
      ENTRY(char,int),
      ENTRY(char,uint),
      ENTRY(char,long),
      ENTRY(char,ulong),
      ENTRY(char,float),
      ENTRY(char,double),
      ENTRY(char,rgb5),
      ENTRY(char,rgb8) },

    { ENTRY(uchar,char),
      NULL,
      ENTRY(uchar,short),
      ENTRY(uchar,ushort),
      ENTRY(uchar,int),
      ENTRY(uchar,uint),
      ENTRY(uchar,long),
      ENTRY(uchar,ulong),
      ENTRY(uchar,float),
      ENTRY(uchar,double),
      ENTRY(uchar,rgb5),
      ENTRY(uchar,rgb8) },

    { ENTRY(short,char),
      ENTRY(short,uchar),
      NULL,
      ENTRY(short,ushort),
      ENTRY(short,int),
      ENTRY(short,uint),
      ENTRY(short,long),
      ENTRY(short,ulong),
      ENTRY(short,float),
      ENTRY(short,double),
      ENTRY(short,rgb5),
      ENTRY(short,rgb8) },

    { ENTRY(ushort,char),
      ENTRY(ushort,uchar),
      ENTRY(ushort,short),
      NULL,
      ENTRY(ushort,int),
      ENTRY(ushort,uint),
      ENTRY(ushort,long),
      ENTRY(ushort,ulong),
      ENTRY(ushort,float),
      ENTRY(ushort,double),
      ENTRY(ushort,rgb5),
      ENTRY(ushort,rgb8) },

    { ENTRY(int,char),
      ENTRY(int,uchar),
      ENTRY(int,short),
      ENTRY(int,ushort),
      NULL,
      ENTRY(int,uint),
      ENTRY(int,long),
      ENTRY(int,ulong),
      ENTRY(int,float),
      ENTRY(int,double),
      ENTRY(int,rgb5),
      ENTRY(int,rgb8) },

    { ENTRY(uint,char),
      ENTRY(uint,uchar),
      ENTRY(uint,short),
      ENTRY(uint,ushort),
      ENTRY(uint,int),
      NULL,
      ENTRY(uint,long),
      ENTRY(uint,ulong),
      ENTRY(uint,float),
      ENTRY(uint,double),
      ENTRY(uint,rgb5),
      ENTRY(uint,rgb8) },

    { ENTRY(long,char),
      ENTRY(long,uchar),
      ENTRY(long,short),
      ENTRY(long,ushort),
      ENTRY(long,int),
      ENTRY(long,uint),
      NULL,
      ENTRY(long,ulong),
      ENTRY(long,float),
      ENTRY(long,double),
      ENTRY(long,rgb5),
      ENTRY(long,rgb8) },

    { ENTRY(ulong,char),
      ENTRY(ulong,uchar),
      ENTRY(ulong,short),
      ENTRY(ulong,ushort),
      ENTRY(ulong,int),
      ENTRY(ulong,uint),
      ENTRY(ulong,long),
      NULL,
      ENTRY(ulong,float),
      ENTRY(ulong,double),
      ENTRY(ulong,rgb5),
      ENTRY(ulong,rgb8) },

    { ENTRY(float,char),
      ENTRY(float,uchar),
      ENTRY(float,short),
      ENTRY(float,ushort),
      ENTRY(float,int),
      ENTRY(float,uint),
      ENTRY(float,long),
      ENTRY(float,ulong),
      NULL,
      ENTRY(float,double),
      ENTRY(float,rgb5),
      ENTRY(float,rgb8) },

    { ENTRY(double,char),
      ENTRY(double,uchar),
      ENTRY(double,short),
      ENTRY(double,ushort),
      ENTRY(double,int),
      ENTRY(double,uint),
      ENTRY(double,long),
      ENTRY(double,ulong),
      ENTRY(double,float),
      NULL,
      ENTRY(double,rgb5),
      ENTRY(double,rgb8) },

    { ENTRY(rgb5,char),
      ENTRY(rgb5,uchar),
      ENTRY(rgb5,short),
      ENTRY(rgb5,ushort),
      ENTRY(rgb5,int),
      ENTRY(rgb5,uint),
      ENTRY(rgb5,long),
      ENTRY(rgb5,ulong),
      ENTRY(rgb5,float),
      ENTRY(rgb5,double),
      NULL,
      ENTRY(rgb5,rgb8) },

    { ENTRY(rgb8,char),
      ENTRY(rgb8,uchar),
      ENTRY(rgb8,short),
      ENTRY(rgb8,ushort),
      ENTRY(rgb8,int),
      ENTRY(rgb8,uint),
      ENTRY(rgb8,long),
      ENTRY(rgb8,ulong),
      ENTRY(rgb8,float),
      ENTRY(rgb8,double),
      ENTRY(rgb8,rgb5),
      NULL }
};
//----------------------------------------------------------------------------
void Mgc::ImageConvert (int iQuantity, int iSrcRTTI, void* pvSrcData,
    int iTrgRTTI, void* pvTrgData)
{
    assert( iSrcRTTI < MGC_ELEMENT_QUANTITY
        &&  iTrgRTTI < MGC_ELEMENT_QUANTITY );

    Converter oConverter = gs_aaoConvert[iSrcRTTI][iTrgRTTI];
    oConverter(iQuantity,pvSrcData,pvTrgData);
}
//----------------------------------------------------------------------------


