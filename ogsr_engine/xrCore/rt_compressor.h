#pragma once

extern XRCORE_API void rtc_initialize();
extern XRCORE_API size_t rtc_compress(void* dst, size_t dst_len, const void* src, size_t src_len);
extern XRCORE_API size_t rtc_decompress(void* dst, size_t dst_len, const void* src, size_t src_len);
extern XRCORE_API u32 rtc_csize(u32 in);
