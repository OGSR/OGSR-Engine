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
#include "VolumeRender.h"

// Initial size of application window is irrelevant.  OnPrecreate will
// change the size based on the selected image.
VolumeRender g_kTheApp("VolumeRender",16,16);

//----------------------------------------------------------------------------
VolumeRender::VolumeRender (char* acCaption, int iWidth, int iHeight,
    unsigned int uiMenuID, unsigned int uiStatusPaneQuantity)
    :
    FWinApplication(acCaption,iWidth,iHeight,uiMenuID,uiStatusPaneQuantity)
{
    m_bButtonDown = false;
    m_hWindowDC = 0;
    m_hMemoryDC = 0;
    m_hImage = 0;
    m_ausBits = NULL;
    m_fGamma = 0.25f;
}
//----------------------------------------------------------------------------
VolumeRender::~VolumeRender ()
{
}
//----------------------------------------------------------------------------
bool VolumeRender::OnPrecreate ()
{
    if ( !GetCommand() )
    {
        // command line must be specified
        return false;
    }

    char* acFilename = NULL;
    GetCommand()->Filename(acFilename);
    if ( !acFilename )
    {
        // input filename must be specified on the command line
        return false;
    }

    // load image, must be 3D and pixels must be unsigned char
    int iDimensions, iQuantity, iRTTI, iSizeOf;
    int* aiBound = NULL;
    char* acData = NULL;
    bool bLoaded = Lattice::LoadRaw(acFilename,iDimensions,aiBound,
        iQuantity,iRTTI,iSizeOf,acData);
    if ( !bLoaded || iDimensions != 3 || iRTTI != Euchar::GetRTTI() )
    {
        delete[] acData;
        delete[] acFilename;
        return false;
    }

    ImageUChar3D* pkImage = new ImageUChar3D(aiBound[0],aiBound[1],
        aiBound[2],(Euchar*)acData);

    // get maximum dimension size
    int iBMax = pkImage->GetBound(0);
    if ( pkImage->GetBound(1) > iBMax )
        iBMax = pkImage->GetBound(1);
    if ( pkImage->GetBound(2) > iBMax )
        iBMax = pkImage->GetBound(2);
    m_iBound = 2*iBMax;
    m_fHBound = (float)iBMax;

    m_pkRT = new RayTrace(pkImage,m_fGamma);
    delete pkImage;

    // resize application window
    SetWidth(m_iBound);
    SetHeight(m_iBound);

    delete[] acFilename;
    return true;
}
//----------------------------------------------------------------------------
bool VolumeRender::OnInitialize ()
{
    // create the bitmap
    m_hWindowDC = GetDC(GetWindowHandle());
    m_hMemoryDC = CreateCompatibleDC(m_hWindowDC);

    char* acBMI = new char[sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD*)];
    BITMAPINFOHEADER& rkBMIH = *(BITMAPINFOHEADER*)acBMI;
    rkBMIH.biSize = sizeof(BITMAPINFOHEADER);
    rkBMIH.biWidth = m_iBound;
    rkBMIH.biHeight = -m_iBound;
    rkBMIH.biPlanes = 1;
    rkBMIH.biBitCount = 16;
    rkBMIH.biCompression = BI_RGB;
    rkBMIH.biSizeImage = m_iBound*m_iBound;
    rkBMIH.biXPelsPerMeter = 0;
    rkBMIH.biYPelsPerMeter = 0;
    rkBMIH.biClrUsed = 0;
    rkBMIH.biClrImportant = 0;

    m_hImage = CreateDIBSection(m_hMemoryDC,(CONST BITMAPINFO*)acBMI,
        DIB_RGB_COLORS,(void**)&m_ausBits,NULL,0);

    delete[] acBMI;

    SelectObject(m_hMemoryDC,m_hImage);
    m_pkRT->Trace(1);
    Draw();

    return true;
}
//----------------------------------------------------------------------------
void VolumeRender::OnTerminate ()
{
    DeleteObject(m_hImage);
    DeleteDC(m_hMemoryDC);
    ReleaseDC(GetWindowHandle(),m_hWindowDC);
}
//----------------------------------------------------------------------------
void VolumeRender::Draw ()
{
    m_pkRT->DrawWireFrame();

    // write the rendered result to a 2D bitmap
    for (int iY = 0; iY < m_iBound; iY++)
    {
        for (int iX = 0; iX < m_iBound; iX++)
        {
            int i = iX + m_iBound*iY;
            m_ausBits[i] = m_pkRT->GetRendered(i);
        }
    }

    // display the back buffer
    BitBlt(m_hWindowDC,0,0,m_iBound,m_iBound,m_hMemoryDC,0,0,SRCCOPY);
}
//----------------------------------------------------------------------------
bool VolumeRender::OnPaint (HDC hDC)
{
    if ( m_ausBits )
        Draw();
    return true;
}
//----------------------------------------------------------------------------
bool VolumeRender::OnLButtonDown (int iXPos, int iYPos, unsigned int)
{
    m_bButtonDown = true;
    m_fX0 = (iXPos-m_fHBound)/m_fHBound;
    m_fY0 = (iYPos-m_fHBound)/m_fHBound;

    return true;
}
//----------------------------------------------------------------------------
bool VolumeRender::OnLButtonUp (int iXPos, int iYPos, unsigned int)
{
    m_bButtonDown = false;
    m_fX1 = (iXPos-m_fHBound)/m_fHBound;
    m_fY1 = (iYPos-m_fHBound)/m_fHBound;

    if ( m_pkRT->MoveTrackBall(m_fX0,m_fY0,m_fX1,m_fY1) )
    {
        m_pkRT->Trace(1);
        Draw();
    }

    return true;
}
//----------------------------------------------------------------------------
bool VolumeRender::OnMouseMove (int iXPos, int iYPos, unsigned int)
{
    if ( !m_bButtonDown )
        return false;

    // save old frame
    float m_aafSFrame[3][3];
    int iX, iY;
    for (iY = 0; iY < 3; iY++)
    {
        for (iX = 0; iX < 3; iX++)
            m_aafSFrame[iY][iX] = m_pkRT->Frame(iY,iX);
    }
        
    m_fX1 = (iXPos-m_fHBound)/m_fHBound;
    m_fY1 = (iYPos-m_fHBound)/m_fHBound;
    if ( m_pkRT->MoveTrackBall(m_fX0,m_fY0,m_fX1,m_fY1) )
    {
        // Trace every second ray, then fill in with nearest neighbor values.
        m_pkRT->Trace(2);
        Draw();

        // restore old frame
        for (iY = 0; iY < 3; iY++)
        {
            for (iX = 0; iX < 3; iX++)
                m_pkRT->Frame(iY,iX) = m_aafSFrame[iY][iX];
        }
    }

    return true;
}
//----------------------------------------------------------------------------
bool VolumeRender::OnChar (char cCharCode, long)
{
    switch ( cCharCode )
    {
    case '+':
        m_fGamma /= 1.1f;
        m_pkRT->Correction(m_fGamma);
        Draw();
        return true;
    case '-':
        m_fGamma *= 1.1f;
        m_pkRT->Correction(m_fGamma);
        Draw();
        return true;
    case 'q':
    case 'Q':
    case VK_ESCAPE:
        PostMessage(GetWindowHandle(),WM_DESTROY,0,0);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------


