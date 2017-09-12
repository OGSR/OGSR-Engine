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


// Usage:  BicubicResampler -m magnify input.bmp output.bmp
//
// The value of 'magnify' should be a positive floating point number.  The
// input image must be a 24-bit Windows bitmap image.  If the input image
// has R rows and C columns, the output image is a 24-bit Windows bitmap
// image with floor(magnify*R) rows and floor(magnify*C) columns.
//
// The sample input.bmp is 64-by-64 and the magnification is 2.5, so the
// output.bmp is 160-by-160.  The command line that generates the output is
//     BicubicResampler -m 2.5 input.bmp output.bmp

#include <cassert>
#include <fstream>
using namespace std;

#include <windows.h>
#include "MgcCommand.h"
#include "MgcMath.h"
#include "MgcInterp2DBicubic.h"
using namespace Mgc;

//----------------------------------------------------------------------------
static char* ProcessCommand (char* acArgument)
{
    int iLength = strlen(acArgument);

    // strip off quotes if command line was built from double-clicking a file
    char* acProcessed = new char[iLength+1];
	if ( acArgument[0] == '\"' )
    {
		strcpy(acProcessed,acArgument+1);  // remove leading quote
		if ( acArgument[iLength-1] == '\"' )
			acProcessed[iLength-2] = '\0';  // remove trailing quote
	}
	else
    {
		strcpy(acProcessed,acArgument);
    }

    return acProcessed;
}
//----------------------------------------------------------------------------
static void SaveImage (const char* acFilename, int iWidth, int iHeight,
    unsigned char* aucData)
{
    ofstream kOStr(acFilename,ios::out|ios::binary);
    if ( !kOStr )
        return;

    int iDataBytes = 3*iWidth*iHeight;

    BITMAPFILEHEADER kFileHeader;
    kFileHeader.bfType = 0x4d42;  // "BM"
    kFileHeader.bfSize =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER) +
        iDataBytes;
    kFileHeader.bfReserved1 = 0;
    kFileHeader.bfReserved2 = 0;
    kFileHeader.bfOffBits =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER);

    BITMAPINFOHEADER kInfoHeader;
    kInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    kInfoHeader.biWidth = iWidth;
    kInfoHeader.biHeight = iHeight;
    kInfoHeader.biPlanes = 1;
    kInfoHeader.biBitCount = 24;
    kInfoHeader.biCompression = BI_RGB;
    kInfoHeader.biSizeImage = 0;
    kInfoHeader.biXPelsPerMeter = 0;
    kInfoHeader.biYPelsPerMeter = 0;
    kInfoHeader.biClrUsed = 0;
    kInfoHeader.biClrImportant = 0;

    kOStr.write((const char*)&kFileHeader,sizeof(BITMAPFILEHEADER));
    kOStr.write((const char*)&kInfoHeader,sizeof(BITMAPINFOHEADER));
    kOStr.write((const char*)aucData,iDataBytes*sizeof(unsigned char));

    kOStr.close();
}
//----------------------------------------------------------------------------
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE, LPSTR acArgument, int)
{
    Command* pkCommand = NULL;
    if ( acArgument && strlen(acArgument) > 0 )
    {
        char* acProcessed = ProcessCommand(acArgument);
        pkCommand = new Command(acProcessed);
        assert( pkCommand );
        delete[] acProcessed;
    }
    else
    {
        // command line was not specified
        return -1;
    }

    float fMagnify;
    if ( !pkCommand->Inf(0.0f).Float("m",fMagnify) )
    {
        // magnification factor was not specified
        return -2;
    }

    char* acInputFile;
    pkCommand->Filename(acInputFile);
    if ( !acInputFile )
    {
        // input.bmp was not specified
        return -3;
    }

    char* acOutputFile;
    pkCommand->Filename(acOutputFile);
    if ( !acOutputFile )
    {
        // output.bmp was not specified
        delete[] acInputFile;
        return -4;
    }

    HBITMAP hImage = (HBITMAP) LoadImage(NULL,acInputFile,IMAGE_BITMAP,0,0,
        LR_LOADFROMFILE | LR_CREATEDIBSECTION);

    if ( !hImage )
    {
        // input.bmp could not be loaded
        return -5;
    }

    DIBSECTION dibSection;
    GetObject(hImage,sizeof(DIBSECTION),&dibSection);

    int iWidth = dibSection.dsBm.bmWidth;
    int iHeight = dibSection.dsBm.bmHeight;
    int iQuantity = dibSection.dsBm.bmWidth*dibSection.dsBm.bmHeight;
    if ( dibSection.dsBm.bmBitsPixel != 24 )
    {
        // input.bmp was not 24-bit color
        return -6;
    }

    // split into separate channels and interpolate each channel separately
    float** aafB;
    Interp2DBicubic::Allocate(iWidth,iHeight,aafB);
    float** aafG;
    Interp2DBicubic::Allocate(iWidth,iHeight,aafG);
    float** aafR;
    Interp2DBicubic::Allocate(iWidth,iHeight,aafR);

    unsigned char* pucSrc = (unsigned char*) dibSection.dsBm.bmBits;
    int iX, iY;
    for (iY = 0; iY < iHeight; iY++)
    {
        for (iX = 0; iX < iWidth; iX++)
        {
            aafB[iY][iX] = *pucSrc++;
            aafG[iY][iX] = *pucSrc++;
            aafR[iY][iX] = *pucSrc++;
        }
    }

    Interp2DBicubic kB(iWidth,iHeight,0.0f,1.0f,0.0f,1.0f,aafB);
    Interp2DBicubic kG(iWidth,iHeight,0.0f,1.0f,0.0f,1.0f,aafG);
    Interp2DBicubic kR(iWidth,iHeight,0.0f,1.0f,0.0f,1.0f,aafR);

    // Compute width of output image.  Make sure it is a multiple of 4 to
    // handle the BMP requirement of row-byte-count being a multiple of 4.
    int iOutWidth = (int)(fMagnify*iWidth);
    int iRemainder = iOutWidth % 4;
    if ( iRemainder > 0 )
        iOutWidth += 4 - iRemainder;

    int iOutHeight = (int)(fMagnify*iHeight);
    int iOutQuantity = iOutWidth*iOutHeight;
    float fXRatio = (iWidth-1.0f)/(iOutWidth-1.0f);
    float fYRatio = (iHeight-1.0f)/(iOutHeight-1.0f);

    unsigned char* aucDst = new unsigned char[3*iOutQuantity];
    pucSrc = (unsigned char*) dibSection.dsBm.bmBits;
    int i;
    for (iY = 0, i = 0; iY < iOutHeight; iY++)
    {
        float fY = fYRatio*iY;
        for (iX = 0; iX < iOutWidth; iX++)
        {
            float fX = fXRatio*iX;

            float fValue = kB(fX,fY);
            if ( fValue >= 0.0f )
            {
                if ( fValue <= 255.0f )
                    aucDst[i++] = (unsigned char)fValue;
                else
                    aucDst[i++] = 255;
            }
            else
            {
                aucDst[i++] = 0;
            }

            fValue = kG(fX,fY);
            if ( fValue >= 0.0f )
            {
                if ( fValue <= 255.0f )
                    aucDst[i++] = (unsigned char)fValue;
                else
                    aucDst[i++] = 255;
            }
            else
            {
                aucDst[i++] = 0;
            }

            fValue = kR(fX,fY);
            if ( fValue >= 0.0f )
            {
                if ( fValue <= 255.0f )
                    aucDst[i++] = (unsigned char)fValue;
                else
                    aucDst[i++] = 255;
            }
            else
            {
                aucDst[i++] = 0;
            }
        }
    }

    SaveImage(acOutputFile,iOutWidth,iOutHeight,aucDst);

    Interp2DBicubic::Deallocate(aafB);
    Interp2DBicubic::Deallocate(aafG);
    Interp2DBicubic::Deallocate(aafR);
    DeleteObject(hImage);
    delete[] acInputFile;
    delete[] acOutputFile;

    return 0;
}
//----------------------------------------------------------------------------


