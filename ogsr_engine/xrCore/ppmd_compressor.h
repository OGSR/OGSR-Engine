#ifndef PPMD_COMPRESSOR_H
#define PPMD_COMPRESSOR_H

XRCORE_API u32 ppmd_compress	(void *dest_buffer, const u32 &dest_buffer_size, const void *source_buffer, const u32 &source_buffer_size);
XRCORE_API u32 ppmd_decompress	(void *dest_buffer, const u32 &dest_buffer_size, const void *source_buffer, const u32 &source_buffer_size);

#endif // PPMD_COMPRESSOR_H