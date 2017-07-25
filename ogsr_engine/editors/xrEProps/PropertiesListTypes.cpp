#include "stdafx.h"
#pragma hdrstop

#include "PropertiesListTypes.h"
#include "MxShortcut.hpp"
//------------------------------------------------------------------------------
            
xr_string	ShortcutValue::GetDrawText		(TOnDrawTextEvent OnDrawText)
{
    xr_string 	txt = MxShortCutToText(value->hotkey).c_str();
    if (!OnDrawText.empty())	OnDrawText(this, txt);
    return 			txt;
}

