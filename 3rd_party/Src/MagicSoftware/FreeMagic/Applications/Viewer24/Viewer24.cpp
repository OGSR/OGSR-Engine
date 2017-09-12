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

#include "MgcImages.h"
#include "Viewer24.h"
using namespace Mgc;

Viewer24 g_kTheApp("Viewer24",256,276,0,1);

//----------------------------------------------------------------------------
Viewer24::Viewer24 (char* acCaption, int iWidth, int iHeight,
    unsigned int uiMenuID, unsigned int uiStatusPaneQuantity)
    :
    FWinApplication(acCaption,iWidth,iHeight,uiMenuID,uiStatusPaneQuantity)
{
    m_hImage = 0;
    m_hDragDC = 0;
    m_bMouseDown = false;
    m_iZ = 0;
    m_afData = NULL;
    m_auiData = NULL;
    m_iSliceQuantity = 0;
}
//----------------------------------------------------------------------------
Viewer24::~Viewer24 ()
{
}
//----------------------------------------------------------------------------
bool Viewer24::OnPrecreate ()
{
    if ( !GetCommand() )
    {
        // command line must be specified
        return false;
    }

    char* acFilename;
    GetCommand()->Filename(acFilename);
    if ( !acFilename )
    {
        // input filename must be specified on the command line
        return false;
    }

    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_DIR];
    char acFName[_MAX_FNAME];
    char acExt[_MAX_EXT];
    _splitpath(acFilename,acDrive,acDir,acFName,acExt);
    strcpy(m_acFilename,acFName);

    int iRTTI, iSizeOf;
    char* acData = NULL;
    bool bLoaded = Lattice::LoadRaw(acFilename,m_iDimensions,
        m_aiBound,m_iQuantity,iRTTI,iSizeOf,acData);
    if ( !bLoaded )
    {
        delete[] acData;
        delete[] acFilename;
        return false;
    }

    m_auiBits = new unsigned int[m_iQuantity];
    memset(m_auiBits,0,m_iQuantity*sizeof(unsigned int));

    int i;

    if ( iRTTI == Ergb5::GetRTTI() )
    {
        m_auiData = new unsigned int[m_iQuantity];
        unsigned short* ausData = (unsigned short*) acData;
        for (i = 0; i < m_iQuantity; i++)
        {
            unsigned char ucR = GetRed16(ausData[i]);
            unsigned char ucG = GetGreen16(ausData[i]);
            unsigned char ucB = GetBlue16(ausData[i]);
            m_auiData[i] = GetColor24(ucR,ucG,ucB);
        }
    }
    else if ( iRTTI == Ergb8::GetRTTI() )
    {
        m_auiData = new unsigned int[m_iQuantity];
        memcpy(m_auiData,acData,m_iQuantity*sizeof(rgb8));
    }
    else if ( iRTTI == Efloat::GetRTTI() )
    {
        m_afData = new float[m_iQuantity];
        memcpy(m_afData,acData,m_iQuantity*sizeof(float));
    }
    else
    {
        m_afData = new float[m_iQuantity];
        ImageConvert(m_iQuantity,iRTTI,acData,Efloat::GetRTTI(),m_afData);
    }

    if ( m_afData )
    {
        m_fMin = m_afData[0];
        m_fMax = m_fMin;
        for (i = 1; i < m_iQuantity; i++)
        {
            if ( m_afData[i] < m_fMin )
                m_fMin = m_afData[i];
            else if ( m_afData[i] > m_fMax )
                m_fMax = m_afData[i];
        }
        m_fRange = m_fMax - m_fMin;

        if ( m_fRange > 0.0f )
        {
            float fInvRange = 1.0f/m_fRange;
            for (i = 0; i < m_iQuantity; i++)
            {
                unsigned int uiGray = (unsigned int)(255.0f*(m_afData[i] -
                    m_fMin)*fInvRange);
                m_auiBits[i] = uiGray+(uiGray<<8)+(uiGray<<16);
            }
        }
    }
    else
    {
        memcpy(m_auiBits,m_auiData,m_iQuantity*sizeof(unsigned int));
    }

    // Set window size based on image size.  Adjust height to allow for
    // status bar.
    const int iStatusHeight = 20;
    SetWidth(m_aiBound[0]);
    SetHeight(m_aiBound[1] + iStatusHeight);
    m_iSliceQuantity = m_aiBound[0]*m_aiBound[1];

    delete[] acData;
    delete[] acFilename;
    return true;
}
//----------------------------------------------------------------------------
bool Viewer24::OnInitialize ()
{
    SetWindowText(ms_hWnd,m_acFilename);

    // create the bitmap
    m_acBMI = new char[sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD*)];
    BITMAPINFOHEADER& rkBMIH = *(BITMAPINFOHEADER*)m_acBMI;
    rkBMIH.biSize = sizeof(BITMAPINFOHEADER);
    rkBMIH.biWidth = m_aiBound[0];
    rkBMIH.biHeight = -m_aiBound[1];
    rkBMIH.biPlanes = 1;
    rkBMIH.biBitCount = 32;
    rkBMIH.biCompression = BI_RGB;
    rkBMIH.biSizeImage = m_iQuantity;
    rkBMIH.biXPelsPerMeter = 0;
    rkBMIH.biYPelsPerMeter = 0;
    rkBMIH.biClrUsed = 0;
    rkBMIH.biClrImportant = 0;
    HDC hDC = GetDC(ms_hWnd);
    m_hImage = CreateDIBitmap(hDC,&rkBMIH,CBM_INIT,(const void*)m_auiBits,
        (const BITMAPINFO*)m_acBMI,DIB_RGB_COLORS);
    ReleaseDC(ms_hWnd,hDC);

    InvalidateRect(GetWindowHandle(),NULL,TRUE);
    return true;
}
//----------------------------------------------------------------------------
void Viewer24::OnTerminate ()
{
    delete[] m_afData;
    delete[] m_auiData;
    delete[] m_acBMI;
    delete[] m_auiBits;
    delete[] m_aiBound;
    DeleteObject(m_hImage);
}
//----------------------------------------------------------------------------
bool Viewer24::OnPaint (HDC hDC)
{
    if ( !m_hImage )
        return false;

    HDC hImageDC = CreateCompatibleDC(hDC);

    BITMAP kBitmap;
    GetObject(m_hImage,sizeof(BITMAP),&kBitmap);
    SelectObject(hImageDC,m_hImage);
    BitBlt(hDC,0,0,kBitmap.bmWidth,kBitmap.bmHeight,hImageDC,0,0,SRCCOPY);

    DeleteDC(hImageDC);

    return true;
}
//----------------------------------------------------------------------------
bool Viewer24::OnChar (char cCharCode, long)
{
    switch ( cCharCode )
    {
    case 'q':
    case 'Q':
    case VK_ESCAPE:
        PostMessage(GetWindowHandle(),WM_DESTROY,0,0);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
bool Viewer24::OnKeyDown (int iVirtKey, long)
{
    if ( m_iDimensions != 3 )
        return false;

    if ( iVirtKey == VK_UP )
    {
        // up-arrow pressed, go to next image slize
        if ( m_iZ < m_aiBound[2] - 1 )
        {
            m_iZ++;

            // get slice and convert image values to palette indices
            if ( m_afData )
            {
                float* afSlice = &m_afData[m_iZ*m_iSliceQuantity];
                if ( m_fRange > 0.0f )
                {
                    float fInvRange = 1.0f/m_fRange;
                    for (int i = 0; i < m_iSliceQuantity; i++)
                    {
                        unsigned int uiGray = (unsigned int)(255.0f*(
                            afSlice[i]-m_fMin)*fInvRange);
                        m_auiBits[i] = uiGray+(uiGray<<8)+(uiGray<<16);
                    }
                }
            }
            else
            {
                unsigned int* auiSlice = &m_auiData[m_iZ*m_iSliceQuantity];
                memcpy(m_auiBits,auiSlice,m_iSliceQuantity *
                    sizeof(unsigned int));
            }
            
            // update bitmap and redraw client area
            HDC hDC = GetDC(ms_hWnd);
            SetDIBits(hDC,m_hImage,0,m_aiBound[1],(const void*)m_auiBits,
                (const BITMAPINFO*)m_acBMI,DIB_RGB_COLORS);
            ReleaseDC(ms_hWnd,hDC);
            InvalidateRect(ms_hWnd,NULL,FALSE);
        }
    }
    else if ( iVirtKey == VK_DOWN )
    {
        // down-arrow pressed, go to previous image slice
        if ( m_iZ > 0 )
        {
            m_iZ--;

            // get slice and convert image values to palette indices
            if ( m_afData )
            {
                float* afSlice = &m_afData[m_iZ*m_iSliceQuantity];
                if ( m_fRange > 0.0f )
                {
                    float fInvRange = 1.0f/m_fRange;
                    for (int i = 0; i < m_iSliceQuantity; i++)
                    {
                        unsigned int uiGray = (unsigned int)(255.0f*(
                            afSlice[i]-m_fMin)*fInvRange);
                        m_auiBits[i] = uiGray+(uiGray<<8)+(uiGray<<16);
                    }
                }
            }
            else
            {
                unsigned int* auiSlice = &m_auiData[m_iZ*m_iSliceQuantity];
                memcpy(m_auiBits,auiSlice,m_iSliceQuantity *
                    sizeof(unsigned int));
            }
            
            // update bitmap and redraw client area
            HDC hDC = GetDC(ms_hWnd);
            SetDIBits(hDC,m_hImage,0,m_aiBound[1],(const void*)m_auiBits,
                (const BITMAPINFO*)m_acBMI,DIB_RGB_COLORS);
            ReleaseDC(ms_hWnd,hDC);
            InvalidateRect(ms_hWnd,NULL,FALSE);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool Viewer24::OnLButtonDown (int iXPos, int iYPos, unsigned int)
{
    // read and display pixel value at mouse location
    if ( !m_bMouseDown )
    {
        m_bMouseDown = true;
        SetCapture(ms_hWnd);  // let window have control of mouse events
        m_hDragDC = GetDC(ms_hWnd);
        if ( 0 <= iXPos && iXPos < m_aiBound[0]
        &&   0 <= iYPos && iYPos < m_aiBound[1] )
        {
            unsigned int uiIndex = iXPos + m_aiBound[0]*iYPos;
            if ( m_afData )
            {
                float* afSlice = &m_afData[m_iZ*m_iSliceQuantity];
                float fValue = afSlice[uiIndex];
                if ( m_iDimensions == 2 )
                {
                    sprintf(m_acPixelStr,"(%d,%d) %f",iXPos,iYPos,fValue);
                }
                else
                {
                    sprintf(m_acPixelStr,"(%d,%d,%d) %f",iXPos,iYPos,m_iZ,
                        fValue);
                }
            }
            else
            {
                unsigned int* auiSlice = &m_auiData[m_iZ*m_iSliceQuantity];
                unsigned int uiValue = auiSlice[uiIndex];
                if ( m_iDimensions == 2 )
                {
                    sprintf(m_acPixelStr,"(%d,%d) r=%d g=%d b=%d",iXPos,iYPos,
                        GetRed24(uiValue),GetGreen24(uiValue),
                        GetBlue24(uiValue));
                }
                else
                {
                    sprintf(m_acPixelStr,"(%d,%d,%d) r=%d g=%d b=%d",iXPos,
                        iYPos,m_iZ,GetRed24(uiValue),GetGreen24(uiValue),
                        GetBlue24(uiValue));
                }
            }
        }
        else
        {
            int iLength = strlen(m_acPixelStr);
            memset(m_acPixelStr,' ',iLength);
        }
        
        WritePixelString();
    }

    return true;
}
//----------------------------------------------------------------------------
bool Viewer24::OnLButtonUp (int iXPos, int iYPos, unsigned int)
{
    // finished reading pixel values, redisplay window caption
    if ( m_bMouseDown )
    {
        m_bMouseDown = false;
        ReleaseCapture();  // window gives up control of mouse events
        ReleaseDC(ms_hWnd,m_hDragDC);

        int iLength = strlen(m_acPixelStr);
        memset(m_acPixelStr,' ',iLength);
        WritePixelString();
    }

    return false;
}
//----------------------------------------------------------------------------
bool Viewer24::OnMouseMove (int iXPos, int iYPos, unsigned int)
{
    // allow mouse dragging for continuous update/display of pixel values
    if ( m_bMouseDown )
    {
        if ( 0 <= iXPos && iXPos < (int) m_aiBound[0]
        &&   0 <= iYPos && iYPos < (int) m_aiBound[1] )
        {
            unsigned int uiIndex = iXPos + m_aiBound[0]*iYPos;
            if ( m_afData )
            {
                float* afSlice = &m_afData[m_iZ*m_iSliceQuantity];
                float fValue = afSlice[uiIndex];
                if ( m_iDimensions == 2 )
                {
                    sprintf(m_acPixelStr,"(%d,%d) %f",iXPos,iYPos,fValue);
                }
                else
                {
                    sprintf(m_acPixelStr,"(%d,%d,%d) %f",iXPos,iYPos,m_iZ,
                        fValue);
                }
            }
            else
            {
                unsigned int* auiSlice = &m_auiData[m_iZ*m_iSliceQuantity];
                unsigned int uiValue = auiSlice[uiIndex];
                if ( m_iDimensions == 2 )
                {
                    sprintf(m_acPixelStr,"(%d,%d) r=%d g=%d b=%d",iXPos,iYPos,
                        GetRed24(uiValue),GetGreen24(uiValue),
                        GetBlue24(uiValue));
                }
                else
                {
                    sprintf(m_acPixelStr,"(%d,%d,%d) r=%d g=%d b=%d",iXPos,
                        iYPos,m_iZ,GetRed24(uiValue),GetGreen24(uiValue),
                        GetBlue24(uiValue));
                }
            }
        }
        else
        {
            int iLength = strlen(m_acPixelStr);
            memset(m_acPixelStr,' ',iLength);
        }
        
        WritePixelString();
    }

    return false;
}
//----------------------------------------------------------------------------
void Viewer24::WritePixelString ()
{
    SendMessage(GetStatusWindowHandle(),SB_SETTEXT,(WPARAM)0,
        (LPARAM)m_acPixelStr);
    SendMessage(GetStatusWindowHandle(),WM_PAINT,0,0); 
}
//----------------------------------------------------------------------------


