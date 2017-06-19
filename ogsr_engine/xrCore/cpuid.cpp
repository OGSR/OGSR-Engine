#include "stdafx.h"
#pragma hdrstop

#include "cpuid.h"

//#ifdef _M_AMD64

#ifndef M_BORLAND

int _cpuid (_processor_info *pinfo)
{
	_processor_info&	P	= *pinfo;

	/*
	strcpy				(P.v_name,		"AuthenticAMD");
	strcpy				(P.model_name,	"AMD64 family");
	P.family			=	8;
	P.model				=	8;
	P.stepping			=	0;
	P.feature			=	_CPU_FEATURE_SSE | _CPU_FEATURE_SSE2;
	P.os_support		=	_CPU_FEATURE_SSE | _CPU_FEATURE_SSE2;
	return P.feature;
	*/

	ZeroMemory(&P, sizeof(_processor_info));

	int cpinfo[4];
	// detect cpu vendor
	__cpuid(cpinfo, 0);

	memcpy(P.v_name, &(cpinfo[1]), sizeof(int));
	memcpy(P.v_name+sizeof(int), &(cpinfo[3]), sizeof(int));
	memcpy(P.v_name+2*sizeof(int), &(cpinfo[2]), sizeof(int));

	// detect cpu model
	__cpuid(cpinfo, 0x80000002);
	memcpy(P.model_name, cpinfo, sizeof(cpinfo));
	__cpuid(cpinfo, 0x80000003);
	memcpy(P.model_name+16, cpinfo, sizeof(cpinfo));
	__cpuid(cpinfo, 0x80000004);
	memcpy(P.model_name+32, cpinfo, sizeof(cpinfo));

	// detect cpu main features
	__cpuid(cpinfo, 1);

	P.stepping = cpinfo[0] & 0xf;
    P.model = (u8)((cpinfo[0] >> 4) & 0xf) | ((u8)((cpinfo[0] >> 16) & 0xf) << 4);
    P.family = (u8)((cpinfo[0] >> 8) & 0xf) | ((u8)((cpinfo[0] >> 20) & 0xff) << 4);

	if (cpinfo[3] & (1 << 23))
		P.feature |= _CPU_FEATURE_MMX;
	if (cpinfo[3] & (1 << 25))
		P.feature |= _CPU_FEATURE_SSE;
	if (cpinfo[3] & (1 << 26))
		P.feature |= _CPU_FEATURE_SSE2;
	if (cpinfo[2] & 0x1)
		P.feature |= _CPU_FEATURE_SSE3;
	if (cpinfo[2] & 0x80000)
		P.feature |= _CPU_FEATURE_SSE41;
	if (cpinfo[2] & 0x100000)
		P.feature |= _CPU_FEATURE_SSE42;

	// and check 3DNow! support
	__cpuid(cpinfo, 0x80000001);
	if (cpinfo[3] & (1 << 31))
		P.feature |= _CPU_FEATURE_3DNOW;

	P.os_support = P.feature;

	// get version of OS
	DWORD dwMajorVersion = 0;
	DWORD dwVersion = 0;
	dwVersion = GetVersion();

	dwMajorVersion = (DWORD)(LOBYTE(LOWORD(dwVersion)));

	if (dwMajorVersion <= 5)		// XP don't support SSE3+ instruction sets
	{
		P.os_support	&= ~_CPU_FEATURE_SSE3	;
		P.os_support	&= ~_CPU_FEATURE_SSE41	;
		P.os_support	&= ~_CPU_FEATURE_SSE42	;
	}

	return P.feature;
}

#else

// These are the bit flags that get set on calling cpuid
// with register eax set to 1
#define _MMX_FEATURE_BIT			0x00800000
#define _SSE_FEATURE_BIT			0x02000000
#define _SSE2_FEATURE_BIT			0x04000000

// This bit is set when cpuid is called with
// register set to 80000001h (only applicable to AMD)
#define _3DNOW_FEATURE_BIT			0x80000000
 
int IsCPUID()
{
    __try {
        _asm
        {
            xor eax, eax
            cpuid
        }
    } __except ( EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
    return 1;
}

/***
* int _os_support(int feature,...)
*   - Checks if OS Supports the capablity or not
****************************************************************/

#ifdef M_BORLAND
// borland doesn't understand MMX/3DNow!/SSE/SSE2 asm opcodes
void _os_support(int feature, int& res)
{
	res |= feature;
}
#endif

/***
*
* void map_mname(int, int, const char *, char *) maps family and model to processor name
*
****************************************************/

void map_mname( int family, int model, const char * v_name, char *m_name)
{
    if (!strncmp("AuthenticAMD", v_name, 12))
    {
        switch (family) // extract family code
        {
        case 4: // Am486/AM5x86
            strcpy (m_name,"Am486");
            break;

        case 5: // K6
            switch (model) // extract model code
            {
            case 0:		strcpy (m_name,"K5 Model 0");	break;
            case 1:		strcpy (m_name,"K5 Model 1");	break;
            case 2:		strcpy (m_name,"K5 Model 2");	break;
            case 3:		strcpy (m_name,"K5 Model 3");	break;
            case 4:     break;	// Not really used
            case 5:     break;  // Not really used
            case 6:		strcpy (m_name,"K6 Model 1");	break;
            case 7:		strcpy (m_name,"K6 Model 2");	break;
            case 8:		strcpy (m_name,"K6-2");			break;
            case 9: 
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:	strcpy (m_name,"K6-3");			break;
            default:	strcpy (m_name,"K6 family");	break;
            }
            break;

        case 6: // Athlon
            switch(model)  // No model numbers are currently defined
            {
            case 1:		strcpy (m_name,"ATHLON Model 1");	break;
			case 2:		strcpy (m_name,"ATHLON Model 2");	break;
			case 3:		strcpy (m_name,"DURON");			break;
			case 4:	
			case 5:		strcpy (m_name,"ATHLON TB");		break;
			case 6:		strcpy (m_name,"ATHLON XP");		break;
			case 7:		strcpy (m_name,"DURON XP");			break;
            default:    strcpy (m_name,"K7 Family");		break;
			}
            break;
        }
    } else if ( !strncmp("GenuineIntel", v_name, 12))
    {
        switch (family) // extract family code
        {
        case 4:
            switch (model) // extract model code
            {
            case 0:
            case 1:		strcpy (m_name,"i486DX");			break;
            case 2:		strcpy (m_name,"i486SX");			break;
            case 3:		strcpy (m_name,"i486DX2");			break;
            case 4:		strcpy (m_name,"i486SL");			break;
            case 5:		strcpy (m_name,"i486SX2");			break;
            case 7:		strcpy (m_name,"i486DX2E");			break;
            case 8:		strcpy (m_name,"i486DX4");			break;
            default:    strcpy (m_name,"i486 family");		break;
            }
            break;
        case 5:
            switch (model) // extract model code
            {
            case 1:
            case 2:
            case 3:		strcpy (m_name,"Pentium");			break;
            case 4:		strcpy (m_name,"Pentium-MMX");		break;
            default:	strcpy (m_name,"P5 family");		break;
            }
            break;
        case 6:
            switch (model) // extract model code
            {
            case 1:		strcpy (m_name,"Pentium-Pro");		break;
            case 3:		strcpy (m_name,"Pentium-II");		break;
            case 5:		strcpy (m_name,"Pentium-II");		break;  // actual differentiation depends on cache settings
            case 6:		strcpy (m_name,"Celeron");			break;
            case 7:		strcpy (m_name,"Pentium-III");		break;  // actual differentiation depends on cache settings
			case 8:		strcpy (m_name,"P3 Coppermine");	break;
            default:	strcpy (m_name,"P3 family");		break;
            }
            break;
		case 15:
			// F15/M2/S4 ???
			switch (model)
			{
			case 2:		strcpy	(m_name,"Pentium 4");		break;
			default:	strcpy	(m_name,"P4 family");		break;
			}
        }
    } else if ( !strncmp("CyrixInstead", v_name,12))
    {
        strcpy (m_name,"Unknown");
    } else if ( !strncmp("CentaurHauls", v_name,12))
    {
        strcpy (m_name,"Unknown");
    } else 
    {
        strcpy (m_name, "Unknown");
    }

}

/***
*
* int _cpuid (_p_info *pinfo)
* 
* Entry:
*
*   pinfo: pointer to _p_info.
*
* Exit:
*
*   Returns int with capablity bit set even if pinfo = NULL
*
****************************************************/

int _cpuid (_processor_info *pinfo)
{
    u32 dwStandard = 0;
    u32 dwFeature = 0;
    u32 dwMax = 0;
    u32 dwExt = 0;
    int feature = 0, os_support = 0;
    union
    {
        char cBuf[12+1];
        struct
        {
            u32 dw0;
            u32 dw1;
            u32 dw2;
        };
    } Ident;

    if (!IsCPUID())
    {
        return 0;
    }

    __asm
    {
        push ebx
        push ecx
        push edx

        // get the vendor string
        xor eax,eax
        cpuid
        mov dwMax,eax
        mov dword ptr Ident.dw0,ebx
        mov dword ptr Ident.dw1,edx
        mov dword ptr Ident.dw2,ecx

        // get the Standard bits
        mov eax,1
        cpuid
        mov dwStandard,eax
        mov dwFeature,edx

        // get AMD-specials
        mov eax,80000000h
        cpuid
        cmp eax,80000000h
        jc notamd
        mov eax,80000001h
        cpuid
        mov dwExt,edx

notamd:
        pop ecx
        pop ebx
        pop edx
    }

    if (dwFeature & _MMX_FEATURE_BIT)
    {
        feature |= _CPU_FEATURE_MMX;
        _os_support(_CPU_FEATURE_MMX,os_support);
    }
    if (dwExt & _3DNOW_FEATURE_BIT)
    {
        feature |= _CPU_FEATURE_3DNOW;
        _os_support(_CPU_FEATURE_3DNOW,os_support);
    }
    if (dwFeature & _SSE_FEATURE_BIT)
    {
        feature |= _CPU_FEATURE_SSE;
        _os_support(_CPU_FEATURE_SSE,os_support);
    }
    if (dwFeature & _SSE2_FEATURE_BIT)
    {
        feature |= _CPU_FEATURE_SSE2;
        _os_support(_CPU_FEATURE_SSE2,os_support);
    }

	if (pinfo)
    {
        memset		(pinfo, 0, sizeof(_processor_info));
        pinfo->os_support = os_support;
        pinfo->feature = feature;
        pinfo->family = (dwStandard >> 8)&0xF;  // retriving family
        pinfo->model = (dwStandard >> 4)&0xF;   // retriving model
        pinfo->stepping = (dwStandard) & 0xF;   // retriving stepping
        Ident.cBuf[12] = 0;
        strcpy		(pinfo->v_name, Ident.cBuf);
        map_mname	(pinfo->family, pinfo->model, pinfo->v_name, pinfo->model_name);
    }
   return feature;
}

#endif