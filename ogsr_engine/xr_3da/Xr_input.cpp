#include "stdafx.h"

#include "xr_input.h"
#include "IInputReceiver.h"

CInput* pInput = NULL;
IInputReceiver dummyController;

ENGINE_API float psMouseSens = 1.f;
ENGINE_API float psMouseSensScale = 1.f;
ENGINE_API Flags32 psMouseInvert = {FALSE};

#define MOUSEBUFFERSIZE 64
#define KEYBOARDBUFFERSIZE 64
#define _KEYDOWN(name, key) (name[key] & 0x80)

CInput::CInput(bool bExclusive, int deviceForInit)
{
    is_exclusive_mode = bExclusive;

    Log("Starting INPUT device...");

    mouse_property.mouse_dt = 25;

    //===================== Dummy pack
    iCapture(&dummyController);

    if (!pDI)
        CHK_DX(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&pDI, NULL));

    // KEYBOARD
    if (deviceForInit & keyboard_device_key)
        CHK_DX(CreateInputDevice(&pKeyboard, GUID_SysKeyboard, &c_dfDIKeyboard, (is_exclusive_mode ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND, KEYBOARDBUFFERSIZE));

    // MOUSE
    if (deviceForInit & mouse_device_key)
        CHK_DX(CreateInputDevice(&pMouse, GUID_SysMouse, &c_dfDIMouse2, (is_exclusive_mode ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND | DISCL_NOWINKEY,
                                 MOUSEBUFFERSIZE));

    Device.seqAppActivate.Add(this);
    Device.seqAppDeactivate.Add(this);
    Device.seqFrame.Add(this, REG_PRIORITY_HIGH);
}

CInput::~CInput(void)
{
    Device.seqFrame.Remove(this);
    Device.seqAppDeactivate.Remove(this);
    Device.seqAppActivate.Remove(this);

    // Unacquire and release the device's interfaces
    if (pMouse)
    {
        pMouse->Unacquire();
        _RELEASE(pMouse);
    }

    if (pKeyboard)
    {
        pKeyboard->Unacquire();
        _RELEASE(pKeyboard);
    }

    _SHOW_REF("Input: ", pDI);
    _RELEASE(pDI);
}

//-----------------------------------------------------------------------------
// Name: CreateInputDevice()
// Desc: Create a DirectInput device.
//-----------------------------------------------------------------------------
HRESULT CInput::CreateInputDevice(LPDIRECTINPUTDEVICE8* device, GUID guidDevice, const DIDATAFORMAT* pdidDataFormat, u32 dwFlags, u32 buf_size)
{
    // Obtain an interface to the input device
    //.	CHK_DX( pDI->CreateDeviceEx( guidDevice, IID_IDirectInputDevice8, (void**)device, NULL ) );
    CHK_DX(pDI->CreateDevice(guidDevice, /*IID_IDirectInputDevice8,*/ device, NULL));

    // Set the device data format. Note: a data format specifies which
    // controls on a device we are interested in, and how they should be
    // reported.
    CHK_DX((*device)->SetDataFormat(pdidDataFormat));

    // Set the cooperativity level to let DirectInput know how this device
    // should interact with the system and with other DirectInput applications.
    HRESULT _hr = (*device)->SetCooperativeLevel(Device.m_hWnd, dwFlags);
    if (FAILED(_hr) && (_hr == E_NOTIMPL))
        Msg("! INPUT: Can't set coop level. Emulation???");
    else
        R_CHK(_hr);

    // setup the buffer size for the keyboard data
    DIPROPDWORD dipdw;
    dipdw.diph.dwSize = sizeof(DIPROPDWORD);
    dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    dipdw.diph.dwObj = 0;
    dipdw.diph.dwHow = DIPH_DEVICE;
    dipdw.dwData = buf_size;

    CHK_DX((*device)->SetProperty(DIPROP_BUFFERSIZE, &dipdw.diph));

    return S_OK;
}

//-----------------------------------------------------------------------

void CInput::SetAllAcquire(BOOL bAcquire)
{
    if (pMouse)
        bAcquire ? pMouse->Acquire() : pMouse->Unacquire();
    if (pKeyboard)
        bAcquire ? pKeyboard->Acquire() : pKeyboard->Unacquire();
}

void CInput::SetMouseAcquire(BOOL bAcquire)
{
    if (pMouse)
        bAcquire ? pMouse->Acquire() : pMouse->Unacquire();
}
void CInput::SetKBDAcquire(BOOL bAcquire)
{
    if (pKeyboard)
        bAcquire ? pKeyboard->Acquire() : pKeyboard->Unacquire();
}

void CInput::exclusive_mode(const bool exclusive)
{
    is_exclusive_mode = exclusive;

    pKeyboard->Unacquire();
    pMouse->Unacquire();

    R_CHK(pKeyboard->SetCooperativeLevel(RDEVICE.m_hWnd, (exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND));
    pKeyboard->Acquire();
    R_CHK(pMouse->SetCooperativeLevel(RDEVICE.m_hWnd, (exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE) | DISCL_FOREGROUND | DISCL_NOWINKEY));
    pMouse->Acquire();
}

//-----------------------------------------------------------------------
BOOL b_altF4 = FALSE;
void CInput::KeyUpdate()
{
    HRESULT hr;
    DWORD dwElements = KEYBOARDBUFFERSIZE;
    DIDEVICEOBJECTDATA od[KEYBOARDBUFFERSIZE];
    DWORD key = 0;

    VERIFY(pKeyboard);

    hr = pKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0);
    if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
    {
        hr = pKeyboard->Acquire();
        if (hr != S_OK)
            return;
        hr = pKeyboard->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0);
        if (hr != S_OK)
            return;
    }
    u32 i = 0;
    for (i = 0; i < dwElements; i++)
    {
        key = od[i].dwOfs;
        KBState[key] = od[i].dwData & 0x80;
        if (KBState[key])
        {
            if (this->is_exclusive_mode && (key == DIK_LSHIFT || key == DIK_RSHIFT) && (this->iGetAsyncKeyState(DIK_LMENU) || this->iGetAsyncKeyState(DIK_RMENU)))
                PostMessage(gGameWindow, WM_INPUTLANGCHANGEREQUEST, 2, 0); //Переключили язык. В эксклюзивном режиме это обязательно для правильной работы функции DikToChar

            cbStack.back()->IR_OnKeyboardPress(key);
        }
        else
        {
            cbStack.back()->IR_OnKeyboardRelease(key);
        }
    }
    for (i = 0; i < COUNT_KB_BUTTONS; i++)
        if (KBState[i])
            cbStack.back()->IR_OnKeyboardHold(i);

    if (!b_altF4 && iGetAsyncKeyState(DIK_F4) && (iGetAsyncKeyState(DIK_RMENU) || iGetAsyncKeyState(DIK_LMENU)))
    {
        b_altF4 = TRUE;
        Engine.Event.Defer("KERNEL:disconnect");
        Engine.Event.Defer("KERNEL:quit");
    }
}

bool CInput::get_dik_name(int dik, LPSTR dest_str, int dest_sz)
{
    DIPROPSTRING keyname;
    keyname.diph.dwSize = sizeof(DIPROPSTRING);
    keyname.diph.dwHeaderSize = sizeof(DIPROPHEADER);
    keyname.diph.dwObj = static_cast<DWORD>(dik);
    keyname.diph.dwHow = DIPH_BYOFFSET;
    HRESULT hr = pKeyboard->GetProperty(DIPROP_KEYNAME, &keyname.diph);
    if (FAILED(hr))
        return false;

    const wchar_t* wct = keyname.wsz;
    if (!wcslen(wct))
        return false;

    const size_t cnt = wcstombs(dest_str, wct, dest_sz);

    return cnt != -1;
}

BOOL CInput::iGetAsyncKeyState(int dik)
{
    // KRodin: да-да, я знаю, что этот код ужасен.
    switch (dik)
    {
    case DIK_LMENU: return GetAsyncKeyState(VK_LMENU) & 0x8000;
    case DIK_RMENU: return GetAsyncKeyState(VK_RMENU) & 0x8000;
    case DIK_TAB: return GetAsyncKeyState(VK_TAB) & 0x8000;
    case DIK_LCONTROL: return GetAsyncKeyState(VK_LCONTROL) & 0x8000;
    case DIK_RCONTROL: return GetAsyncKeyState(VK_RCONTROL) & 0x8000;
    case DIK_DELETE: return GetAsyncKeyState(VK_DELETE) & 0x8000;
    default: return KBState[dik];
    }
}

BOOL CInput::iGetAsyncBtnState(int btn) { return mouseState[btn]; }

void CInput::MouseUpdate()
{
#pragma push_macro("FIELD_OFFSET")
#undef FIELD_OFFSET
#define FIELD_OFFSET offsetof // Фиксим warning C4644 - просто переводим макрос из винсдк на использование стандартного оффсетофа.

    HRESULT hr;
    DWORD dwElements = MOUSEBUFFERSIZE;
    DIDEVICEOBJECTDATA od[MOUSEBUFFERSIZE];

    VERIFY(pMouse);

    hr = pMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0);
    if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
    {
        hr = pMouse->Acquire();
        if (hr != S_OK)
            return;
        hr = pMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), &od[0], &dwElements, 0);
        if (hr != S_OK)
            return;
    };
    BOOL mouse_prev[COUNT_MOUSE_BUTTONS];

    mouse_prev[0] = mouseState[0];
    mouse_prev[1] = mouseState[1];
    mouse_prev[2] = mouseState[2];

    offs[0] = offs[1] = offs[2] = 0;
    for (u32 i = 0; i < dwElements; i++)
    {
        switch (od[i].dwOfs)
        {
        case DIMOFS_X:
            offs[0] += od[i].dwData;
            timeStamp[0] = od[i].dwTimeStamp;
            break;
        case DIMOFS_Y:
            offs[1] += od[i].dwData;
            timeStamp[1] = od[i].dwTimeStamp;
            break;
        case DIMOFS_Z:
            offs[2] += od[i].dwData;
            timeStamp[2] = od[i].dwTimeStamp;
            break;
        case DIMOFS_BUTTON0:
            if (od[i].dwData & 0x80)
            {
                mouseState[0] = TRUE;
                cbStack.back()->IR_OnMousePress(0);
            }
            if (!(od[i].dwData & 0x80))
            {
                mouseState[0] = FALSE;
                cbStack.back()->IR_OnMouseRelease(0);
            }
            break;
        case DIMOFS_BUTTON1:
            if (od[i].dwData & 0x80)
            {
                mouseState[1] = TRUE;
                cbStack.back()->IR_OnMousePress(1);
            }
            if (!(od[i].dwData & 0x80))
            {
                mouseState[1] = FALSE;
                cbStack.back()->IR_OnMouseRelease(1);
            }
            break;
        case DIMOFS_BUTTON2:
            if (od[i].dwData & 0x80)
            {
                mouseState[2] = TRUE;
                cbStack.back()->IR_OnMousePress(2);
            }
            if (!(od[i].dwData & 0x80))
            {
                mouseState[2] = FALSE;
                cbStack.back()->IR_OnMouseRelease(2);
            }
            break;
        case DIMOFS_BUTTON3:
            if (od[i].dwData & 0x80)
            {
                mouseState[2] = TRUE;
                cbStack.back()->IR_OnKeyboardPress(0xED + 103);
            }
            if (!(od[i].dwData & 0x80))
            {
                mouseState[2] = FALSE;
                cbStack.back()->IR_OnKeyboardRelease(0xED + 103);
            }
            break;
        case DIMOFS_BUTTON4:
            if (od[i].dwData & 0x80)
            {
                mouseState[2] = TRUE;
                cbStack.back()->IR_OnKeyboardPress(0xED + 104);
            }
            if (!(od[i].dwData & 0x80))
            {
                mouseState[2] = FALSE;
                cbStack.back()->IR_OnKeyboardRelease(0xED + 104);
            }
            break;
        case DIMOFS_BUTTON5:
            if (od[i].dwData & 0x80)
            {
                mouseState[2] = TRUE;
                cbStack.back()->IR_OnKeyboardPress(0xED + 105);
            }
            if (!(od[i].dwData & 0x80))
            {
                mouseState[2] = FALSE;
                cbStack.back()->IR_OnKeyboardRelease(0xED + 105);
            }
            break;
        case DIMOFS_BUTTON6:
            if (od[i].dwData & 0x80)
            {
                mouseState[2] = TRUE;
                cbStack.back()->IR_OnKeyboardPress(0xED + 106);
            }
            if (!(od[i].dwData & 0x80))
            {
                mouseState[2] = FALSE;
                cbStack.back()->IR_OnKeyboardRelease(0xED + 106);
            }
            break;
        case DIMOFS_BUTTON7:
            if (od[i].dwData & 0x80)
            {
                mouseState[2] = TRUE;
                cbStack.back()->IR_OnKeyboardPress(0xED + 107);
            }
            if (!(od[i].dwData & 0x80))
            {
                mouseState[2] = FALSE;
                cbStack.back()->IR_OnKeyboardRelease(0xED + 107);
            }
            break;
        }
    }

    // Giperion: double check mouse buttons state
    DIMOUSESTATE2 MouseState;
    hr = pMouse->GetDeviceState(sizeof(MouseState), &MouseState);

    auto RecheckMouseButtonFunc = [&](int i) {
        if (MouseState.rgbButtons[i] & 0x80 && mouseState[i] == FALSE)
        {
            mouseState[i] = TRUE;
            cbStack.back()->IR_OnMousePress(i);
        }
        else if (!(MouseState.rgbButtons[i] & 0x80) && mouseState[i] == TRUE)
        {
            mouseState[i] = FALSE;
            cbStack.back()->IR_OnMouseRelease(i);
        }
    };

    if (hr == S_OK)
    {
        RecheckMouseButtonFunc(0);
        RecheckMouseButtonFunc(1);
        RecheckMouseButtonFunc(2);
    }
    //-Giperion

    auto isButtonOnHold = [&](int i) {
        if (mouseState[i] && mouse_prev[i])
            cbStack.back()->IR_OnMouseHold(i);
    };

    isButtonOnHold(0);
    isButtonOnHold(1);
    isButtonOnHold(2);

    if (dwElements)
    {
        if (offs[0] || offs[1])
            cbStack.back()->IR_OnMouseMove(offs[0], offs[1]);
        if (offs[2])
            cbStack.back()->IR_OnMouseWheel(offs[2]);
    }
    else
    {
        if (timeStamp[1] && ((dwCurTime - timeStamp[1]) >= mouse_property.mouse_dt))
            cbStack.back()->IR_OnMouseStop(DIMOFS_Y, timeStamp[1] = 0);
        if (timeStamp[0] && ((dwCurTime - timeStamp[0]) >= mouse_property.mouse_dt))
            cbStack.back()->IR_OnMouseStop(DIMOFS_X, timeStamp[0] = 0);
    }

#pragma pop_macro("FIELD_OFFSET")
}

//-------------------------------------------------------
void CInput::iCapture(IInputReceiver* p)
{
    VERIFY(p);
    if (pMouse)
        MouseUpdate();
    if (pKeyboard)
        KeyUpdate();

    // change focus
    if (!cbStack.empty())
        cbStack.back()->IR_OnDeactivate();
    cbStack.push_back(p);
    cbStack.back()->IR_OnActivate();

    // prepare for _new_ controller
    ZeroMemory(timeStamp, sizeof(timeStamp));
    ZeroMemory(timeSave, sizeof(timeSave));
    ZeroMemory(offs, sizeof(offs));
}

void CInput::iRelease(IInputReceiver* p)
{
    if (p == cbStack.back())
    {
        cbStack.back()->IR_OnDeactivate();
        cbStack.pop_back();
        IInputReceiver* ir = cbStack.back();
        ir->IR_OnActivate();
    }
    else
    { // we are not topmost receiver, so remove the nearest one
        u32 cnt = cbStack.size();
        for (; cnt > 0; --cnt)
            if (cbStack[cnt - 1] == p)
            {
                xr_vector<IInputReceiver*>::iterator it = cbStack.begin();
                std::advance(it, cnt - 1);
                cbStack.erase(it);
                break;
            }
    }
}

void CInput::OnAppActivate(void)
{
    if (CurrentIR())
        CurrentIR()->IR_OnActivate();

    SetAllAcquire(true);
    ZeroMemory(mouseState, sizeof(mouseState));
    ZeroMemory(KBState, sizeof(KBState));
    ZeroMemory(timeStamp, sizeof(timeStamp));
    ZeroMemory(timeSave, sizeof(timeSave));
    ZeroMemory(offs, sizeof(offs));
}

void CInput::OnAppDeactivate(void)
{
    if (CurrentIR())
        CurrentIR()->IR_OnDeactivate();

    SetAllAcquire(false);
    ZeroMemory(mouseState, sizeof(mouseState));
    ZeroMemory(KBState, sizeof(KBState));
    ZeroMemory(timeStamp, sizeof(timeStamp));
    ZeroMemory(timeSave, sizeof(timeSave));
    ZeroMemory(offs, sizeof(offs));
}

void CInput::OnFrame(void)
{
    Device.Statistic->Input.Begin();
    dwCurTime = Device.TimerAsync_MMT();
    if (pKeyboard)
        KeyUpdate();
    if (pMouse)
        MouseUpdate();
    Device.Statistic->Input.End();
}

IInputReceiver* CInput::CurrentIR()
{
    if (cbStack.size())
        return cbStack.back();
    else
        return NULL;
}

u16 CInput::DikToChar(const int dik, const bool utf)
{
    switch (dik)
    {
    // Эти клавиши через ToAscii не обработать, поэтому пропишем явно
    case DIK_NUMPAD0: return '0';
    case DIK_NUMPAD1: return '1';
    case DIK_NUMPAD2: return '2';
    case DIK_NUMPAD3: return '3';
    case DIK_NUMPAD4: return '4';
    case DIK_NUMPAD5: return '5';
    case DIK_NUMPAD6: return '6';
    case DIK_NUMPAD7: return '7';
    case DIK_NUMPAD8: return '8';
    case DIK_NUMPAD9: return '9';
    case DIK_NUMPADSLASH: return '/';
    case DIK_NUMPADPERIOD: return '.';
    //
    default:
        u8 State[256]{};
        if (this->is_exclusive_mode)
        { // GetKeyboardState в данном случае не используем, потому что оно очень глючно работает в эксклюзивном режиме
            if (this->iGetAsyncKeyState(DIK_LSHIFT) || this->iGetAsyncKeyState(DIK_RSHIFT))
                State[VK_SHIFT] = 0x80; //Для получения правильных символов при зажатом shift
        }
        else
        {
            if (!GetKeyboardState(State))
                return 0;
        }

        u16 output{};
        if (utf)
        {
            WCHAR symbol{};
            if (this->is_exclusive_mode)
            {
                auto layout = GetKeyboardLayout(GetWindowThreadProcessId(gGameWindow, nullptr));
                if (ToUnicodeEx(MapVirtualKeyEx(dik, MAPVK_VSC_TO_VK, layout), dik, State, &symbol, 1, 0, layout) != 1)
                    return 0;
            }
            else
            {
                if (ToUnicode(MapVirtualKey(dik, MAPVK_VSC_TO_VK), dik, State, &symbol, 1, 0) != 1)
                    return 0;
            }
            WideCharToMultiByte(CP_UTF8, 0, &symbol, 1, reinterpret_cast<char*>(&output), sizeof output, nullptr, nullptr);
            return output;
        }
        else
        {
            if (this->is_exclusive_mode)
            {
                auto layout = GetKeyboardLayout(GetWindowThreadProcessId(gGameWindow, nullptr));
                if (ToAsciiEx(MapVirtualKeyEx(dik, MAPVK_VSC_TO_VK, layout), dik, State, &output, 0, layout) == 1)
                    return output;
            }
            else
            {
                if (ToAscii(MapVirtualKey(dik, MAPVK_VSC_TO_VK), dik, State, &output, 0) == 1)
                    return output;
            }
        }
    }

    return 0;
}

// https://stackoverflow.com/a/36827574
void CInput::clip_cursor(bool clip)
{
    if (clip)
    {
        ShowCursor(FALSE);
        if (Device.m_hWnd && !psDeviceFlags.is(rsFullscreen))
        {
            RECT rect;
            GetClientRect(Device.m_hWnd, &rect);

            POINT ul;
            ul.x = rect.left;
            ul.y = rect.top;

            POINT lr;
            lr.x = rect.right;
            lr.y = rect.bottom;

            MapWindowPoints(Device.m_hWnd, nullptr, &ul, 1);
            MapWindowPoints(Device.m_hWnd, nullptr, &lr, 1);

            rect.left = ul.x;
            rect.top = ul.y;

            rect.right = lr.x;
            rect.bottom = lr.y;

            ClipCursor(&rect);
        }
    }
    else
    {
        while (ShowCursor(TRUE) < 0)
            ;
        ClipCursor(nullptr);
    }
}
