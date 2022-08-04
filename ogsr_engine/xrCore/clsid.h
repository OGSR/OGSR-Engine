#pragma once

using CLASS_ID = u64; // CLASS ID type

constexpr auto MK_CLSID = [](const char& a, const char& b, const char& c, const char& d, const char& e, const char& f, const char& g, const char& h) {
    return CLASS_ID((CLASS_ID(a) << CLASS_ID(56)) | (CLASS_ID(b) << CLASS_ID(48)) | (CLASS_ID(c) << CLASS_ID(40)) | (CLASS_ID(d) << CLASS_ID(32)) | (CLASS_ID(e) << CLASS_ID(24)) |
                    (CLASS_ID(f) << CLASS_ID(16)) | (CLASS_ID(g) << CLASS_ID(8)) | (CLASS_ID(h)));
};

extern XRCORE_API void __stdcall CLSID2TEXT(CLASS_ID id, LPSTR text);
extern XRCORE_API CLASS_ID __stdcall TEXT2CLSID(LPCSTR text);
