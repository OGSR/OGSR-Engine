#pragma once

#include "client_id.h"

#pragma pack(push, 1)

constexpr u32 NET_PacketSizeLimit = 8192;

struct NET_Buffer
{
    BYTE data[NET_PacketSizeLimit];
    u32 count;
};

class NET_Packet final
{
public:
    void construct(const void* data, unsigned size)
    {
        memcpy(B.data, data, size);
        B.count = size;
    }

    NET_Buffer B{};
    u32 r_pos{};
    u32 timeReceive{};

    // writing - main
    IC void write_start() { B.count = 0; }
    IC void w_begin(u16 type) // begin of packet 'type'
    {
        B.count = 0;
        w_u16(type);
    }
    IC void w(const void* p, u32 count)
    {
        R_ASSERT(p && count && (B.count + count < NET_PacketSizeLimit));

        CopyMemory(&B.data[B.count], p, count);
        B.count += count;
    }
    IC void w_seek(u32 pos, const void* p, u32 count) // random write (only inside allocated region)
    {
        R_ASSERT(p && count && (pos + count < NET_PacketSizeLimit));
        //ASSERT_FMT_DBG(pos + count <= B.count, "!![%s] pos: [%u], count [%u], B.count: [%u]", __FUNCTION__, pos, count, B.count);

        CopyMemory(&B.data[pos], p, count);
    }
    IC u32 w_tell() { return B.count; }
    IC void w_advance(u32 count)
    {
        B.count += count;

        R_ASSERT(B.count < NET_PacketSizeLimit);
    }

    // writing - utilities
    IC void w_float(float a)
    {
        *(float*)(&B.data[B.count]) = a;
        w_advance(sizeof(float));
    }; // float
    IC void w_vec3(const Fvector& a)
    {
        *(Fvector*)(&B.data[B.count]) = a;
        w_advance(sizeof(Fvector));
    }; // vec3
    IC void w_vec4(const Fvector4& a)
    {
        *(Fvector4*)(&B.data[B.count]) = a;
        w_advance(sizeof(Fvector4));
    }; // vec4
    IC void w_u64(u64 a)
    {
        *(u64*)(&B.data[B.count]) = a;
        w_advance(sizeof(u64));
    }; // qword (8b)
    IC void w_s64(s64 a)
    {
        *(s64*)(&B.data[B.count]) = a;
        w_advance(sizeof(s64));
    }; // qword (8b)
    IC void w_u32(u32 a)
    {
        *(u32*)(&B.data[B.count]) = a;
        w_advance(sizeof(u32));
    }; // dword (4b)
    IC void w_s32(s32 a)
    {
        *(s32*)(&B.data[B.count]) = a;
        w_advance(sizeof(s32));
    }; // dword (4b)
    IC void w_u16(u16 a)
    {
        *(u16*)(&B.data[B.count]) = a;
        w_advance(sizeof(u16));
    }; // word (2b)
    IC void w_s16(s16 a)
    {
        *(s16*)(&B.data[B.count]) = a;
        w_advance(sizeof(s16));
    }; // word (2b)
    IC void w_u8(u8 a)
    {
        *(u8*)(&B.data[B.count]) = a;
        w_advance(sizeof(u8));
    }; // byte (1b)
    IC void w_s8(s8 a)
    {
        *(s8*)(&B.data[B.count]) = a;
        w_advance(sizeof(s8));
    }; // byte (1b)
    IC void w_float_q16(float a, float min, float max)
    {
        VERIFY(a >= min && a <= max);
        float q = (a - min) / (max - min);
        w_u16(u16(iFloor(q * 65535.f + 0.5f)));
    }
    IC void w_float_q8(float a, float min, float max)
    {
        VERIFY(a >= min && a <= max);
        float q = (a - min) / (max - min);
        w_u8(u8(iFloor(q * 255.f + 0.5f)));
    }
    IC void w_angle16(float a) { w_float_q16(angle_normalize(a), 0, PI_MUL_2); }
    IC void w_angle8(float a) { w_float_q8(angle_normalize(a), 0, PI_MUL_2); }
    IC void w_dir(const Fvector& D) { w_u16(pvCompress(D)); }
    IC void w_sdir(const Fvector& D)
    {
        Fvector C;
        float mag = D.magnitude();
        if (mag > EPS_S)
        {
            C.div(D, mag);
        }
        else
        {
            C.set(0, 0, 1);
            mag = 0;
        }
        w_dir(C);
        w_float(mag);
    }
    IC void w_stringZ(LPCSTR S) { w(S, (u32)xr_strlen(S) + 1); }
    IC void w_stringZ(shared_str& p)
    {
        if (*p)
            w(*p, (u32)xr_strlen(p) + 1);
        else
            w_u8(0);
    }
    IC void w_matrix(Fmatrix& M)
    {
        w_vec3(M.i);
        w_vec3(M.j);
        w_vec3(M.k);
        w_vec3(M.c);
    }

    IC void w_clientID(ClientID& C) { w_u32(C.value()); }

    IC void w_chunk_open8(u32& position)
    {
        position = w_tell();
        w_u8(0);
    }

    IC void w_chunk_close8(u32 position)
    {
        u32 size = u32(w_tell() - position) - sizeof(u8);
        VERIFY(size < 256);
        u8 _size = (u8)size;
        w_seek(position, &_size, sizeof(_size));
    }

    IC void w_chunk_open16(u32& position)
    {
        position = w_tell();
        w_u16(0);
    }

    IC void w_chunk_close16(u32 position)
    {
        u32 size = u32(w_tell() - position) - sizeof(u16);
        VERIFY(size < 65536);
        u16 _size = (u16)size;
        w_seek(position, &_size, sizeof(_size));
    }

    // reading
    IC void read_start() { r_pos = 0; }

    IC u32 r_begin(u16& type) // returns time of receiving
    {
        r_pos = 0;
        r_u16(type);
        return timeReceive;
    }

    IC void r_seek(u32 pos)
    {
        R_ASSERT(pos < NET_PacketSizeLimit);
        //ASSERT_FMT_DBG(pos < B.count, "!![%s] pos: [%u], B.count: [%u]", __FUNCTION__, pos, B.count);

        r_pos = pos;
    }
    IC u32 r_tell() { return r_pos; }

    IC void r(void* p, u32 count)
    {
        R_ASSERT(p && count && (r_pos + count < NET_PacketSizeLimit));
        //ASSERT_FMT_DBG(r_pos + count <= B.count, "!![%s] r_pos: [%u], count [%u], B.count: [%u]", __FUNCTION__, r_pos, count, B.count);

        CopyMemory(p, &B.data[r_pos], count);
        r_pos += count;
    }
    IC BOOL r_eof() { return r_pos >= B.count; }
    IC u32 r_elapsed() { return B.count - r_pos; }
    IC void r_advance(u32 size)
    {
        R_ASSERT(r_pos + size < NET_PacketSizeLimit);
        //ASSERT_FMT_DBG(r_pos + size <= B.count, "!![%s] r_pos: [%u], size [%u], B.count: [%u]", __FUNCTION__, r_pos, size, B.count);

        r_pos += size;
    }

    // reading - utilities
    IC void r_vec3(Fvector& A)
    {
        A = *(Fvector*)(&B.data[r_pos]);
        r_advance(sizeof(Fvector));
    }; // vec3
    IC void r_vec4(Fvector4& A)
    {
        A = *(Fvector4*)(&B.data[r_pos]);
        r_advance(sizeof(Fvector4));
    }; // vec4
    IC void r_float(float& A)
    {
        A = *(float*)(&B.data[r_pos]);
        r_advance(sizeof(float));
    }; // float
    IC void r_u64(u64& A)
    {
        A = *(u64*)(&B.data[r_pos]);
        r_advance(sizeof(u64));
    }; // qword (8b)
    IC void r_s64(s64& A)
    {
        A = *(s64*)(&B.data[r_pos]);
        r_advance(sizeof(s64));
    }; // qword (8b)
    IC void r_u32(u32& A)
    {
        A = *(u32*)(&B.data[r_pos]);
        r_advance(sizeof(u32));
    }; // dword (4b)
    IC void r_s32(s32& A)
    {
        A = *(s32*)(&B.data[r_pos]);
        r_advance(sizeof(s32));
    }; // dword (4b)
    IC void r_u16(u16& A)
    {
        A = *(u16*)(&B.data[r_pos]);
        r_advance(sizeof(u16));
    }; // word (2b)
    IC void r_s16(s16& A)
    {
        A = *(s16*)(&B.data[r_pos]);
        r_advance(sizeof(s16));
    }; // word (2b)
    IC void r_u8(u8& A)
    {
        A = *(u8*)(&B.data[r_pos]);
        r_advance(sizeof(u8));
    }; // byte (1b)
    IC void r_s8(s8& A)
    {
        A = *(s8*)(&B.data[r_pos]);
        r_advance(sizeof(s8));
    }; // byte (1b)
    // IReader compatibility
    IC Fvector r_vec3()
    {
        Fvector tmp;
        tmp = *(Fvector*)(&B.data[r_pos]);
        r_advance(sizeof(Fvector));
        return tmp;
    }; // vec3
    IC Fvector4 r_vec4()
    {
        Fvector4 tmp;
        tmp = *(Fvector4*)(&B.data[r_pos]);
        r_advance(sizeof(Fvector4));
        return tmp;
    }; // vec4
    IC float r_float_q8(float min, float max)
    {
        float A;
        r_float_q8(A, min, max);
        return A;
    }
    IC float r_float_q16(float min, float max)
    {
        float A;
        r_float_q16(A, min, max);
        return A;
    }
    IC float r_float()
    {
        float tmp;
        tmp = *(float*)(&B.data[r_pos]);
        r_advance(sizeof(float));
        return tmp;
    }; // float
    IC u64 r_u64()
    {
        u64 tmp;
        tmp = *(u64*)(&B.data[r_pos]);
        r_advance(sizeof(u64));
        return tmp;
    }; // qword (8b)
    IC s64 r_s64()
    {
        s64 tmp;
        tmp = *(s64*)(&B.data[r_pos]);
        r_advance(sizeof(s64));
        return tmp;
    }; // qword (8b)
    IC u32 r_u32()
    {
        u32 tmp;
        tmp = *(u32*)(&B.data[r_pos]);
        r_advance(sizeof(u32));
        return tmp;
    }; // dword (4b)
    IC s32 r_s32()
    {
        s32 tmp;
        tmp = *(s32*)(&B.data[r_pos]);
        r_advance(sizeof(s32));
        return tmp;
    }; // dword (4b)
    IC u16 r_u16()
    {
        u16 tmp;
        tmp = *(u16*)(&B.data[r_pos]);
        r_advance(sizeof(u16));
        return tmp;
    }; // word (2b)
    IC s16 r_s16()
    {
        s16 tmp;
        tmp = *(s16*)(&B.data[r_pos]);
        r_advance(sizeof(s16));
        return tmp;
    }; // word (2b)
    IC u8 r_u8()
    {
        u8 tmp;
        tmp = *(u8*)(&B.data[r_pos]);
        r_advance(sizeof(u8));
        return tmp;
    }; // byte (1b)
    IC s8 r_s8()
    {
        s8 tmp;
        tmp = *(s8*)(&B.data[r_pos]);
        r_advance(sizeof(s8));
        return tmp;
    }; // byte (1b)

    IC void r_float_q16(float& A, float min, float max)
    {
        u16& val = *(u16*)(&B.data[r_pos]);
        r_advance(sizeof(u16));
        A = (float(val) * (max - min)) / 65535.f + min; // floating-point-error possible
        VERIFY((A >= min - EPS_S) && (A <= max + EPS_S));
    }
    IC void r_float_q8(float& A, float min, float max)
    {
        u8& val = *(u8*)(&B.data[r_pos]);
        r_advance(sizeof(u8));
        A = (float(val) / 255.0001f) * (max - min) + min; // floating-point-error possible
        VERIFY((A >= min) && (A <= max));
    }
    IC void r_angle16(float& A) { r_float_q16(A, 0, PI_MUL_2); }
    IC void r_angle8(float& A) { r_float_q8(A, 0, PI_MUL_2); }
    IC void r_dir(Fvector& A)
    {
        u16& t = *(u16*)(&B.data[r_pos]);
        r_advance(sizeof(u16));
        pvDecompress(A, t);
    }

    IC void r_sdir(Fvector& A)
    {
        u16& t = *(u16*)(&B.data[r_pos]);
        r_advance(sizeof(u16));
        float& s = *(float*)(&B.data[r_pos]);
        r_advance(sizeof(float));
        pvDecompress(A, t);
        A.mul(s);
    }

    IC void r_stringZ(LPSTR S)
    {
        LPCSTR data = LPCSTR(&B.data[r_pos]);
        size_t len = xr_strlen(data);
        r(S, (u32)len + 1);
    }

    IC void r_stringZ(xr_string& dest)
    {
        dest = LPCSTR(&B.data[r_pos]);
        r_advance(u32(dest.size() + 1));
    }

    void r_stringZ(shared_str& dest)
    {
        dest = LPCSTR(&B.data[r_pos]);
        r_advance(dest.size() + 1);
    }

    IC void r_matrix(Fmatrix& M)
    {
        r_vec3(M.i);
        M._14_ = 0;
        r_vec3(M.j);
        M._24_ = 0;
        r_vec3(M.k);
        M._34_ = 0;
        r_vec3(M.c);
        M._44_ = 1;
    }
    IC void r_clientID(ClientID& C)
    {
        u32 tmp;
        r_u32(tmp);
        C.set(tmp);
    }
};

#pragma pack(pop)
