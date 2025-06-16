#pragma once

void _compressLZ(u8** dest, size_t* dest_sz, void* src, size_t src_sz);
bool _decompressLZ(u8** dest, size_t* dest_sz, void* src, size_t src_sz, size_t total_size = -1);
