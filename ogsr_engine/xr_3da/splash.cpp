#include "stdafx.h"

#include "splash.h"

#include "resource.h"

// какой то говнокод что б подключить <atlimage.h>

#define min(x, y) ((x) < (y) ? (x) : (y))
#define max(x, y) ((x) > (y) ? (x) : (y))
#include <atlstr.h>
#include <atlimage.h>
#undef min
#undef max

#pragma comment(lib, "Windowscodecs")

static IStream* CreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType)
{
    IStream* ipStream = nullptr;

    const HRSRC hrsrc = FindResource(nullptr, lpName, lpType);
    if (hrsrc == nullptr)
        return nullptr;

    const HGLOBAL hglbImage = LoadResource(nullptr, hrsrc);
    if (hglbImage == nullptr)
        return nullptr;

    const LPVOID pvSourceResourceData = LockResource(hglbImage);
    if (pvSourceResourceData == nullptr)
        return nullptr;

    const DWORD dwResourceSize = SizeofResource(nullptr, hrsrc);

    const HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
    if (hgblResourceData == nullptr)
        return nullptr;

    const LPVOID pvResourceData = GlobalLock(hgblResourceData);
    if (pvResourceData == nullptr)
    {
        GlobalFree(hgblResourceData);
        return nullptr;
    }

    CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);

    GlobalUnlock(hgblResourceData);

    if (SUCCEEDED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))
        return ipStream;

    GlobalFree(hgblResourceData);
    return nullptr;
}

const char* c_szSplashClass = "SplashWindow";

static HWND logoWindow = nullptr;

void ShowSplash(HINSTANCE hInstance)
{
    WNDCLASS wc = {0};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = c_szSplashClass;
    RegisterClass(&wc);

    // image
    CImage img; // объект изображения

    CHAR path[MAX_PATH];

    GetModuleFileName(nullptr, path, MAX_PATH);

    std::string splash_path{path};
    splash_path = splash_path.erase(splash_path.find_last_of('\\'), splash_path.size() - 1);
    splash_path += "\\splash.png";

    img.Load(splash_path.c_str()); // загрузка сплеша

    if (img.IsNull()) // если картинки нет на диске, то грузим из ресурсов
    {
        img.Destroy();
        img.Load(CreateStreamOnResource(MAKEINTRESOURCE(IDB_PNG1), "PNG")); // загружаем сплеш
    }

    // фиксируем ширину картинки
    // фиксируем высоту картинки
    const int splashWidth = img.GetWidth();
    const int splashHeight = img.GetHeight();

    const HWND hwndOwner = CreateWindow(c_szSplashClass, NULL, WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
    const HWND hWnd = CreateWindowEx(WS_EX_LAYERED, c_szSplashClass, nullptr, WS_POPUP, 0, 0, 0, 0, hwndOwner, nullptr, hInstance, nullptr);

    if (!hWnd)
        return;

    const HDC hdcScreen = GetDC(nullptr);
    const HDC hDC = CreateCompatibleDC(hdcScreen);
    const HBITMAP hBmp = CreateCompatibleBitmap(hdcScreen, splashWidth, splashHeight);

    const HBITMAP hbmpOld = (HBITMAP)SelectObject(hDC, hBmp);

    // рисуем картиночку
    for (int i = 0; i < img.GetWidth(); i++)
    {
        for (int j = 0; j < img.GetHeight(); j++)
        {
            BYTE* ptr = (BYTE*)img.GetPixelAddress(i, j);
            ptr[0] = ((ptr[0] * ptr[3]) + 127) / 255;
            ptr[1] = ((ptr[1] * ptr[3]) + 127) / 255;
            ptr[2] = ((ptr[2] * ptr[3]) + 127) / 255;
        }
    }

    img.AlphaBlend(hDC, 0, 0, splashWidth, splashHeight, 0, 0, splashWidth, splashHeight);

    // alpha
    BLENDFUNCTION blend = {0};
    blend.BlendOp = AC_SRC_OVER;
    blend.AlphaFormat = AC_SRC_ALPHA;
    blend.SourceConstantAlpha = 255;

    SIZE sizeWnd = {splashWidth, splashHeight};

    POINT ptPos = {0, 0};
    POINT ptSrc = {0, 0};

    if (const HWND hDT = GetDesktopWindow())
    {
        RECT rcDT;
        GetWindowRect(hDT, &rcDT);
        ptPos.x = (rcDT.right - splashWidth) / 2;
        ptPos.y = (rcDT.bottom - splashHeight) / 2;
    }

    UpdateLayeredWindow(hWnd, hdcScreen, &ptPos, &sizeWnd, hDC, &ptSrc, 0, &blend, LWA_ALPHA);

    const HWND logoInsertPos = IsDebuggerPresent() ? HWND_NOTOPMOST : HWND_TOPMOST;

    // mmccxvii: захардкорил размер битмапа, чтобы не было бага, связанного с увеличенным масштабом интерфейса винды
    SetWindowPos(hWnd, logoInsertPos, 0, 0, splashWidth, splashHeight, SWP_NOMOVE | SWP_SHOWWINDOW);

    // delete temporary objects
    SelectObject(hDC, hbmpOld);
    DeleteDC(hDC);
    ReleaseDC(nullptr, hdcScreen);

    logoWindow = hWnd;
}

void HideSplash()
{
    // Destroy LOGO
    DestroyWindow(logoWindow);
    logoWindow = nullptr;
}