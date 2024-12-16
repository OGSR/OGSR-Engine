// FS.h: interface for the CFS class.
//
//////////////////////////////////////////////////////////////////////

#pragma once

constexpr auto CFS_CompressMark = 1ul << 31ul;

XRCORE_API void VerifyPath(const std::string_view path);

#ifdef DEBUG
XRCORE_API extern u32 g_file_mapped_memory;
XRCORE_API extern u32 g_file_mapped_count;
XRCORE_API void dump_file_mappings();
extern void register_file_mapping(void* address, const u32& size, LPCSTR file_name);
extern void unregister_file_mapping(void* address, const u32& size);
#endif // DEBUG

//------------------------------------------------------------------------------------
// Write
//------------------------------------------------------------------------------------
class XRCORE_API IWriter
{
private:
    xr_stack<u32> chunk_pos;

public:
    shared_str fName;

public:
    IWriter() {}
    virtual ~IWriter() { R_ASSERT3(chunk_pos.empty(), "Opened chunk not closed.", *fName); }

    // kernel
    virtual void seek(u32 pos) = 0;
    virtual u32 tell() = 0;

    virtual void w(const void* ptr, u32 count) = 0;

    // generalized writing functions
    IC void w_u64(u64 d) { w(&d, sizeof(u64)); }
    IC void w_u32(u32 d) { w(&d, sizeof(u32)); }
    IC void w_u16(u16 d) { w(&d, sizeof(u16)); }
    IC void w_u8(u8 d) { w(&d, sizeof(u8)); }
    IC void w_s64(s64 d) { w(&d, sizeof(s64)); }
    IC void w_s32(s32 d) { w(&d, sizeof(s32)); }
    IC void w_s16(s16 d) { w(&d, sizeof(s16)); }
    IC void w_s8(s8 d) { w(&d, sizeof(s8)); }
    IC void w_float(float d) { w(&d, sizeof(float)); }
    IC void w_string(const char* p)
    {
        w(p, (u32)xr_strlen(p));
        w_u8('\r');
        w_u8('\n');
    }
    IC void w_stringZ(const char* p) { w(p, (u32)xr_strlen(p) + 1); }
    IC void w_stringZ(const shared_str& p)
    {
        w(*p ? *p : "", p.size());
        w_u8(0);
    }
    IC void w_stringZ(shared_str& p)
    {
        w(*p ? *p : "", p.size());
        w_u8(0);
    }
    IC void w_stringZ(const xr_string& p)
    {
        w(p.c_str() ? p.c_str() : "", (u32)p.size());
        w_u8(0);
    }
    IC void w_fcolor(const Fcolor& v) { w(&v, sizeof(Fcolor)); }
    IC void w_fvector4(const Fvector4& v) { w(&v, sizeof(Fvector4)); }
    IC void w_fvector3(const Fvector3& v) { w(&v, sizeof(Fvector3)); }
    IC void w_fvector2(const Fvector2& v) { w(&v, sizeof(Fvector2)); }
    IC void w_ivector4(const Ivector4& v) { w(&v, sizeof(Ivector4)); }
    IC void w_ivector3(const Ivector3& v) { w(&v, sizeof(Ivector3)); }
    IC void w_ivector2(const Ivector2& v) { w(&v, sizeof(Ivector2)); }

    // quant writing functions
    IC void w_float_q16(float a, float min, float max)
    {
        VERIFY(a >= min && a <= max);
        float q = (a - min) / (max - min);
        w_u16(u16(iFloor(q * 65535.f + .5f)));
    }
    IC void w_float_q8(float a, float min, float max)
    {
        VERIFY(a >= min && a <= max);
        float q = (a - min) / (max - min);
        w_u8(u8(iFloor(q * 255.f + .5f)));
    }
    IC void w_angle16(float a) { w_float_q16(angle_normalize(a), 0, PI_MUL_2); }
    IC void w_angle8(float a) { w_float_q8(angle_normalize(a), 0, PI_MUL_2); }
    IC void w_dir(const Fvector& D) { w_u16(pvCompress(D)); }
    void w_sdir(const Fvector& D);
    void __cdecl w_printf(const char* format, ...);

    // generalized chunking
    u32 align();
    void open_chunk(u32 type);
    void close_chunk();
    u32 chunk_size(); // returns size of currently opened chunk, 0 otherwise
    void w_compressed(void* ptr, u32 count, const bool encrypt = false, const bool is_ww = false);
    void w_chunk(u32 type, void* data, u32 size, const bool encrypt = false, const bool is_ww = false);
    virtual bool valid() { return true; }
    virtual int flush() { return 0; } // RvP
};

class XRCORE_API CMemoryWriter : public IWriter
{
    u8* data;
    u32 position;
    u32 mem_size;
    u32 file_size;

public:
    CMemoryWriter()
    {
        data = 0;
        position = 0;
        mem_size = 0;
        file_size = 0;
    }
    virtual ~CMemoryWriter();

    // kernel
    virtual void w(const void* ptr, u32 count);

    virtual void seek(u32 pos) { position = pos; }
    virtual u32 tell() { return position; }

    // specific
    IC u8* pointer() { return data; }
    IC u32 size() const { return file_size; }
    IC void clear()
    {
        file_size = 0;
        position = 0;
    }
#pragma warning(push)
#pragma warning(disable : 4995)
    IC void free()
    {
        file_size = 0;
        position = 0;
        mem_size = 0;
        xr_free(data);
    }
#pragma warning(pop)
    bool save_to(LPCSTR fn);
    void reserve(const size_t count);
};

//------------------------------------------------------------------------------------
// Read
//------------------------------------------------------------------------------------
template <typename implementation_type>
class IReaderBase
{
public:
    virtual ~IReaderBase() {}

    IC implementation_type& impl() { return *(implementation_type*)this; }
    IC const implementation_type& impl() const { return *(implementation_type*)this; }

    IC BOOL eof() const { return impl().elapsed() <= 0; };

    IC void r(void* p, int cnt) { impl().r(p, cnt); }

    virtual Fvector r_vec3()
    {
        Fvector tmp;
        r(&tmp, 3 * sizeof(float));
        return tmp;
    };
    virtual Fvector4 r_vec4()
    {
        Fvector4 tmp;
        r(&tmp, 4 * sizeof(float));
        return tmp;
    };
    virtual u64 r_u64()
    {
        u64 tmp;
        r(&tmp, sizeof(tmp));
        return tmp;
    };
    virtual u32 r_u32()
    {
        u32 tmp;
        r(&tmp, sizeof(tmp));
        return tmp;
    };
    virtual u16 r_u16()
    {
        u16 tmp;
        r(&tmp, sizeof(tmp));
        return tmp;
    };
    virtual u8 r_u8()
    {
        u8 tmp;
        r(&tmp, sizeof(tmp));
        return tmp;
    };
    virtual s64 r_s64()
    {
        s64 tmp;
        r(&tmp, sizeof(tmp));
        return tmp;
    };
    virtual s32 r_s32()
    {
        s32 tmp;
        r(&tmp, sizeof(tmp));
        return tmp;
    };
    virtual s16 r_s16()
    {
        s16 tmp;
        r(&tmp, sizeof(tmp));
        return tmp;
    };
    virtual s8 r_s8()
    {
        s8 tmp;
        r(&tmp, sizeof(tmp));
        return tmp;
    };
    virtual float r_float()
    {
        float tmp;
        r(&tmp, sizeof(tmp));
        return tmp;
    };

    virtual void r_fvector4(Fvector4& v) { r(&v, sizeof(Fvector4)); }
    virtual void r_fvector3(Fvector3& v) { r(&v, sizeof(Fvector3)); }
    virtual void r_fvector2(Fvector2& v) { r(&v, sizeof(Fvector2)); }
    virtual void r_ivector4(Ivector4& v) { r(&v, sizeof(Ivector4)); }
    virtual void r_ivector4(Ivector3& v) { r(&v, sizeof(Ivector3)); }
    virtual void r_ivector4(Ivector2& v) { r(&v, sizeof(Ivector2)); }
    virtual void r_fcolor(Fcolor& v) { r(&v, sizeof(Fcolor)); }

    virtual float r_float_q16(float min, float max)
    {
        u16 val = r_u16();
        float A = (float(val) * (max - min)) / 65535.f + min; // floating-point-error possible
        VERIFY((A >= min - EPS_S) && (A <= max + EPS_S));
        return A;
    }
    virtual float r_float_q8(float min, float max)
    {
        u8 val = r_u8();
        float A = (float(val) / 255.0001f) * (max - min) + min; // floating-point-error possible
        VERIFY((A >= min) && (A <= max));
        return A;
    }
    IC float r_angle16() { return r_float_q16(0, PI_MUL_2); }
    IC float r_angle8() { return r_float_q8(0, PI_MUL_2); }

    virtual void r_dir(Fvector& A)
    {
        u16 t = r_u16();
        pvDecompress(A, t);
    }

    virtual void r_sdir(Fvector& A)
    {
        u16 t = r_u16();
        float s = r_float();
        pvDecompress(A, t);
        A.mul(s);
    }
    // Set file pointer to start of chunk data (0 for root chunk)
    IC void rewind() { impl().seek(0); }

    IC u32 find_chunk(const u32 ID, BOOL* bCompressed = nullptr)
    {
        u32 dwSize{}, dwType{};
        bool success{};

        if (m_last_pos != 0)
        {
            impl().seek(m_last_pos);
            if (impl().elapsed() >= static_cast<int>(sizeof(u32) * 2))
            {
                dwType = r_u32();
                dwSize = r_u32();

                if ((dwType & (~CFS_CompressMark)) == ID)
                {
                    success = true;
                }
            }
        }

        if (!success)
        {
            rewind();
            while (impl().elapsed() >= static_cast<int>(sizeof(u32) * 2)) // while (!eof())
            {
                dwType = r_u32();
                dwSize = r_u32();
                if ((dwType & (~CFS_CompressMark)) == ID)
                {
                    success = true;
                    break;
                }
                else
                {
                    if (impl().elapsed() > static_cast<int>(dwSize))
                        impl().advance(dwSize);
                    else
                        break;
                }
            }

            if (!success)
            {
                m_last_pos = 0;
                return 0;
            }
        }

        if (bCompressed)
            *bCompressed = dwType & CFS_CompressMark;

        // Встречаются объекты, в которых dwSize последнего чанка больше чем реальный размер чанка который там есть.
        // К примеру, в одной из моделей гранат получается превышение на 9 байт.
        // Не знаю, от чего такое бывает, но попробуем обработать эту ситуацию.
        //R_ASSERT((u32)impl().tell() + dwSize <= (u32)impl().length());

        if (impl().elapsed() >= static_cast<int>(dwSize))
        {
            m_last_pos = impl().tell() + dwSize;
            return dwSize;
        }
        else
        {
            Msg("!![%s] chunk [%u] has invalid size [%u], return elapsed size [%d]", __FUNCTION__, ID, dwSize, impl().elapsed());
            m_last_pos = 0;
            return impl().elapsed();
        }
    }

    u32 find_chunk_thm(const u32 ID, const char* dbg_name)
    {
        u32 dwSize{}, dwType{};
        bool success{};

        if (m_last_pos != 0)
        {
            impl().seek(m_last_pos);
            if (impl().elapsed() >= static_cast<int>(sizeof(u32) * 2))
            {
                dwType = r_u32();
                dwSize = r_u32();
                if ((dwType & (~CFS_CompressMark)) == ID)
                {
                    success = true;
                }
            }
        }

        if (!success)
        {
            rewind();
            while (impl().elapsed() >= static_cast<int>(sizeof(u32) * 2)) // while (!eof())
            {
                dwType = r_u32();
                dwSize = r_u32();
                if ((dwType & (~CFS_CompressMark)) == ID)
                {
                    success = true;
                    break;
                }
                else
                {
                    if ((ID & 0x7ffffff0) == 0x810) // is it a thm chunk ID?
                    {
                        const u32 pos = (u32)impl().tell();
                        const u32 size = (u32)impl().length();
                        u32 length = dwSize;

                        if (pos + length != size) // not the last chunk in the file?
                        {
                            bool ok = true;
                            if (pos + length > size - 8)
                                ok = false; // size too large?
                            if (ok)
                            {
                                impl().seek(pos + length);
                                if ((r_u32() & 0x7ffffff0) != 0x810)
                                    ok = false; // size too small?
                            }
                            if (!ok) // size incorrect?
                            {
                                length = 0;
                                while (pos + length < size) // find correct size, up to eof
                                {
                                    impl().seek(pos + length);
                                    if (pos + length <= size - 8 && (r_u32() & 0x7ffffff0) == 0x810)
                                        break; // found start of next section
                                    length++;
                                }
                                Msg("!![%s] THM [%s] chunk [%u] fixed, wrong size = [%u], correct size = [%u]", __FUNCTION__, dbg_name, ID, dwSize, length);
                            }
                        }

                        impl().seek(pos); // go back to beginning of chunk
                        dwSize = length; // use correct(ed) size
                    }
                    impl().advance(dwSize);
                }
            }

            if (!success)
            {
                m_last_pos = 0;
                return 0;
            }
        }

        // см. комментарии выше в функции find_chunk
        // R_ASSERT((u32)impl().tell() + dwSize <= (u32)impl().length());

       if (impl().elapsed() >= static_cast<int>(dwSize))
        {
            m_last_pos = impl().tell() + dwSize;
            return dwSize;
        }
        else
        {
            Msg("!![%s][%p] chunk [%u] has invalid size [%u], return elapsed size [%d]", __FUNCTION__, impl().pointer(), ID, dwSize, impl().elapsed());
            m_last_pos = 0;
            return impl().elapsed();
        }
    }

    IC BOOL r_chunk(u32 ID, void* dest) // чтение XR Chunk'ов (4b-ID,4b-size,??b-data)
    {
        u32 dwSize = find_chunk(ID);
        if (dwSize != 0)
        {
            r(dest, dwSize);
            return TRUE;
        }
        else
            return FALSE;
    }

    IC BOOL r_chunk_safe(u32 ID, void* dest, u32 dest_size) // чтение XR Chunk'ов (4b-ID,4b-size,??b-data)
    {
        u32 dwSize = find_chunk(ID);
        if (dwSize != 0)
        {
            R_ASSERT(dwSize == dest_size);
            r(dest, dwSize);
            return TRUE;
        }
        else
            return FALSE;
    }

private:
    u32 m_last_pos{};
};

class XRCORE_API IReader : public IReaderBase<IReader>
{
protected:
    char* data{};
    int Pos{};
    int Size{};
    int iterpos{};

public:
    IC IReader() { Pos = 0; }

    IC IReader(void* _data, int _size, int _iterpos = 0)
    {
        data = (char*)_data;
        Size = _size;
        Pos = 0;
        iterpos = _iterpos;
    }

    virtual ~IReader() {}

protected:
    u32 advance_term_string();

public:
    IC int elapsed() const { return Size - Pos; }
    IC int tell() const { return Pos; }
    IC void seek(int ptr)
    {
        Pos = ptr;
        R_ASSERT((Pos <= Size) && (Pos >= 0));
    }

    IC int length() const { return Size; };
    IC void* pointer() const { return &(data[Pos]); };
    IC void advance(int cnt)
    {
        Pos += cnt;
        R_ASSERT((Pos <= Size) && (Pos >= 0));
    }

    void close();

    // поиск XR Chunk'ов - возврат - размер или 0
    IReader* open_chunk(u32 ID);

    // iterators
    IReader* open_chunk_iterator(u32& ID, IReader* previous = NULL); // NULL=first

    void skip_bom(const char* dbg_name);

public:
    void r(void* p, int cnt);

    void r_string(char* dest, u32 tgt_sz);
    void r_string(xr_string& dest);

    void skip_stringZ();

    void r_stringZ(char* dest, u32 tgt_sz);
    void r_stringZ(shared_str& dest);
    void r_stringZ(xr_string& dest);

    IC Fvector r_vec3() override
    {
        auto tmp = *reinterpret_cast<Fvector*>(&data[Pos]);
        advance(sizeof tmp);
        return tmp;
    }
    IC Fvector4 r_vec4() override
    {
        auto tmp = *reinterpret_cast<Fvector4*>(&data[Pos]);
        advance(sizeof tmp);
        return tmp;
    }
    IC u64 r_u64() override
    {
        auto tmp = *reinterpret_cast<u64*>(&data[Pos]);
        advance(sizeof tmp);
        return tmp;
    }
    IC u32 r_u32() override
    {
        auto tmp = *reinterpret_cast<u32*>(&data[Pos]);
        advance(sizeof tmp);
        return tmp;
    }
    IC u16 r_u16() override
    {
        auto tmp = *reinterpret_cast<u16*>(&data[Pos]);
        advance(sizeof tmp);
        return tmp;
    }
    IC u8 r_u8() override
    {
        auto tmp = *reinterpret_cast<u8*>(&data[Pos]);
        advance(sizeof tmp);
        return tmp;
    }
    IC s64 r_s64() override
    {
        auto tmp = *reinterpret_cast<s64*>(&data[Pos]);
        advance(sizeof tmp);
        return tmp;
    }
    IC s32 r_s32() override
    {
        auto tmp = *reinterpret_cast<s32*>(&data[Pos]);
        advance(sizeof tmp);
        return tmp;
    }
    IC s16 r_s16() override
    {
        auto tmp = *reinterpret_cast<s16*>(&data[Pos]);
        advance(sizeof tmp);
        return tmp;
    }
    IC s8 r_s8() override
    {
        auto tmp = *reinterpret_cast<s8*>(&data[Pos]);
        advance(sizeof tmp);
        return tmp;
    }
    IC float r_float() override
    {
        auto tmp = *reinterpret_cast<float*>(&data[Pos]);
        advance(sizeof tmp);
        return tmp;
    }
    IC void r_fvector4(Fvector4& v) override
    {
        v = *reinterpret_cast<decltype(&v)>(&data[Pos]);
        advance(sizeof v);
    }
    IC void r_fvector3(Fvector3& v) override
    {
        v = *reinterpret_cast<decltype(&v)>(&data[Pos]);
        advance(sizeof v);
    }
    IC void r_fvector2(Fvector2& v) override
    {
        v = *reinterpret_cast<decltype(&v)>(&data[Pos]);
        advance(sizeof v);
    }
    IC void r_ivector4(Ivector4& v) override
    {
        v = *reinterpret_cast<decltype(&v)>(&data[Pos]);
        advance(sizeof v);
    }
    IC void r_ivector4(Ivector3& v) override
    {
        v = *reinterpret_cast<decltype(&v)>(&data[Pos]);
        advance(sizeof v);
    }
    IC void r_ivector4(Ivector2& v) override
    {
        v = *reinterpret_cast<decltype(&v)>(&data[Pos]);
        advance(sizeof v);
    }
    IC void r_fcolor(Fcolor& v) override
    {
        v = *reinterpret_cast<decltype(&v)>(&data[Pos]);
        advance(sizeof v);
    }

    IC float r_float_q16(float min, float max) override
    {
        auto& val = *reinterpret_cast<u16*>(&data[Pos]);
        advance(sizeof val);
        float A = (float(val) * (max - min)) / 65535.f + min; // floating-point-error possible
        VERIFY((A >= min - EPS_S) && (A <= max + EPS_S));
        return A;
    }
    IC float r_float_q8(float min, float max) override
    {
        auto& val = *reinterpret_cast<u8*>(&data[Pos]);
        advance(sizeof val);
        float A = (float(val) / 255.0001f) * (max - min) + min; // floating-point-error possible
        VERIFY(A >= min && A <= max);
        return A;
    }
    IC void r_dir(Fvector& A) override
    {
        auto& t = *reinterpret_cast<u16*>(&data[Pos]);
        advance(sizeof t);
        pvDecompress(A, t);
    }
    IC void r_sdir(Fvector& A) override
    {
        auto& t = *reinterpret_cast<u16*>(&data[Pos]);
        advance(sizeof t);
        auto& s = *reinterpret_cast<float*>(&data[Pos]);
        advance(sizeof s);
        pvDecompress(A, t);
        A.mul(s);
    }
};
