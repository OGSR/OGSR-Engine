#include "stdafx.h"
#include "dxUIShader.h"

void dxUIShader::Copy(IUIShader& _in) { *this = *smart_cast<dxUIShader*>(&_in); }

void dxUIShader::create(LPCSTR sh, LPCSTR tex, bool no_cache)
{
    if (no_cache)
    {
        hShader.create(sh, tex);
    }
    else
    {
        hShader = GetCachedShader(sh, tex);
    }
}

//void dxUIShader::destroy() { hShader.destroy(); }