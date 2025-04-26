#pragma once

#ifdef TRACY_ENABLE

class dxPixEventWrapper
{
    CBackend* cmd_list;

public:
    dxPixEventWrapper(CBackend& cmd_list_in, const wchar_t* wszName) : cmd_list(&cmd_list_in) { cmd_list->gpu_mark_begin(wszName); }
    ~dxPixEventWrapper() { cmd_list->gpu_mark_end(); }
};

#define PIX_EVENT(Name) dxPixEventWrapper pixEvent##Name(RCache, L## #Name)
#define PIX_EVENT_CTX(C, Name) dxPixEventWrapper pixEvent##Name(C, L## #Name)

#define DXUT_SetDebugName(pObj, pstrName) pObj->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(pstrName), pstrName)

#else

#define PIX_EVENT(Name) __noop(RCache, L## #Name)
#define PIX_EVENT_CTX(C, Name) __noop(C, L## #Name)

#define DXUT_SetDebugName(pObj, pstrName) __noop(pObj, pstrName)

#endif
