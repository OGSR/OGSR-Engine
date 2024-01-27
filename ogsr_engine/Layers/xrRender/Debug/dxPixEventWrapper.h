#pragma once

#include "..\xrRender\HW.h"

#define PIX_EVENT(Name) dxPixEventWrapper pixEvent##Name(L## #Name)

class dxPixEventWrapper
{
public:
    dxPixEventWrapper(const wchar_t* wszName)
    {
        if (HW.pAnnotation)
            HW.pAnnotation->BeginEvent(wszName);
    }
    ~dxPixEventWrapper()
    {
        if (HW.pAnnotation)
            HW.pAnnotation->EndEvent();
    }
};
