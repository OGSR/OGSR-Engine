////////////////////////////////////////////////////////////////////////////
// Module : XR_IOConsole_get.cpp
// Created : 17.05.2008
// Author : Evgeniy Sokolov
// Description : Console`s get-functions class implementation
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "XR_IOConsole.h"
#include "xr_ioc_cmd.h"

LPCSTR CConsole::GetString(LPCSTR cmd) const
{
    IConsole_Command* cc = GetCommand(cmd);
    if (!cc)
        return nullptr;

    static IConsole_Command::TStatus stat;
    cc->Status(stat);
    return stat;
}

LPCSTR CConsole::GetToken(LPCSTR cmd) const { return GetString(cmd); }

const xr_token* CConsole::GetXRToken(LPCSTR cmd) const
{
    IConsole_Command* cc = GetCommand(cmd);

    CCC_Token* cf = dynamic_cast<CCC_Token*>(cc);
    if (cf)
    {
        const xr_token* v = cf->GetToken();
        return v;
    }
    return nullptr;
}

bool CConsole::GetBool(LPCSTR cmd) const
{
	IConsole_Command* cc = GetCommand(cmd);

	CCC_Mask* cf = dynamic_cast<CCC_Mask*>(cc);
	if (cf)
	{
		return (cf->GetValue() != 0);
	}

    CCC_Mask64* cf64 = dynamic_cast<CCC_Mask64*>(cc);
    if (cf64)
    {
        return (cf64->GetValue() != 0);
    }

	CCC_Integer* ci = dynamic_cast<CCC_Integer*>(cc);
	if (ci)
	{
		return (ci->GetValue() != 0);
	}

    CCC_Bool* cb = dynamic_cast<CCC_Bool*>(cc);
    if (cb)
    {
        return cb->GetValue();
    }

	return false;
}

float CConsole::GetFloat(LPCSTR cmd, float& min, float& max) const
{
	min = 0.0f;
	max = 0.0f;
	IConsole_Command* cc = GetCommand(cmd);

	CCC_Float* cf = dynamic_cast<CCC_Float*>(cc);
	if (cf)
	{
		cf->GetBounds(min, max);
		return cf->GetValue();
	}
	return 0.0f;
}

int CConsole::GetInteger(LPCSTR cmd, int& min, int& max) const
{
	min = 0;
	max = 1;
	IConsole_Command* cc = GetCommand(cmd);

	CCC_Integer* cf = dynamic_cast<CCC_Integer*>(cc);
	if (cf)
	{
		cf->GetBounds(min, max);
		return cf->GetValue();
	}

	CCC_Mask* cm = dynamic_cast<CCC_Mask*>(cc);
	if (cm)
	{
		min = 0;
		max = 1;
		return (cm->GetValue()) ? 1 : 0;
	}

    CCC_Mask64* cf64 = dynamic_cast<CCC_Mask64*>(cc);
    if (cf64)
    {
        min = 0;
        max = 1;
        return (cf64->GetValue() != 0);
    }

	return 0;
}

Fvector* CConsole::GetFVector3Ptr(LPCSTR cmd) const
{
    CCC_Vector3* cf = dynamic_cast<CCC_Vector3*>(GetCommand(cmd));
	if (cf)
	{
		return cf->GetValuePtr();
	}
    CCC_Vector3NonStrict* cfns = dynamic_cast<CCC_Vector3NonStrict*>(GetCommand(cmd));
    if (cfns)
    {
        return cfns->GetValuePtr();
    }
	return nullptr;
}

Fvector CConsole::GetFVector3(LPCSTR cmd) const
{
	Fvector* pV = GetFVector3Ptr(cmd);
	if (pV)
	{
		return *pV;
	}
	return Fvector().set(0.0f, 0.0f, 0.0f);
}

void CConsole::GetFVector3Bounds(LPCSTR cmd, Fvector& imin, Fvector& imax) const
{
    CCC_Vector3* cf = dynamic_cast<CCC_Vector3*>(GetCommand(cmd));
    if (cf)
    {
        cf->GetBounds(imin, imax);
    }
    CCC_Vector3NonStrict* cfns = dynamic_cast<CCC_Vector3NonStrict*>(GetCommand(cmd));
    if (cfns)
    {
        cfns->GetBounds(imin, imax);
    }
}

Fvector4* CConsole::GetFVector4Ptr(LPCSTR cmd) const
{
    CCC_Vector4* cf = dynamic_cast<CCC_Vector4*>(GetCommand(cmd));
    if (cf)
    {
        return cf->GetValuePtr();
    }
    CCC_Vector4NonStrict* cfns = dynamic_cast<CCC_Vector4NonStrict*>(GetCommand(cmd));
    if (cfns)
    {
        return cfns->GetValuePtr();
    }
    return nullptr;
}

Fvector4 CConsole::GetFVector4(LPCSTR cmd) const
{
    Fvector4* pV = GetFVector4Ptr(cmd);
    if (pV)
    {
        return *pV;
    }
    return Fvector4().set(0.0f, 0.0f, 0.0f, 0.0f);
}

void CConsole::GetFVector4Bounds(LPCSTR cmd, Fvector4& imin, Fvector4& imax) const
{
    CCC_Vector4* cf = dynamic_cast<CCC_Vector4*>(GetCommand(cmd));
    if (cf)
    {
        cf->GetBounds(imin, imax);
    }
    CCC_Vector4NonStrict* cfns = dynamic_cast<CCC_Vector4NonStrict*>(GetCommand(cmd));
    if (cfns)
    {
        cfns->GetBounds(imin, imax);
    }
}

IConsole_Command* CConsole::GetCommand(LPCSTR cmd) const
{
    vecCMD_CIT it = Commands.find(cmd);
    if (it == Commands.end())
        return nullptr;
    return it->second;
}
