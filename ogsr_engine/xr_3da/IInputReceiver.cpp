#include "stdafx.h"

#include "xr_input.h"
#include "iinputreceiver.h"

void IInputReceiver::IR_Capture(void)
{
    VERIFY(pInput);
    pInput->iCapture(this);
}

void IInputReceiver::IR_Release(void)
{
    VERIFY(pInput);
    pInput->iRelease(this);
}

void IInputReceiver::IR_OnDeactivate(void)
{
    int i;
    for (i = 0; i < CInput::COUNT_KB_BUTTONS; i++)
        if (pInput->iGetAsyncKeyState(i))
            IR_OnKeyboardRelease(i);
    for (i = 0; i < CInput::COUNT_MOUSE_BUTTONS; i++)
        if (pInput->iGetAsyncKeyState(i + MOUSE_1))
            IR_OnMouseRelease(i);

    IR_OnMouseStop(DIMOFS_X, 0);
    IR_OnMouseStop(DIMOFS_Y, 0);
}

void IInputReceiver::IR_OnActivate(void) {}
