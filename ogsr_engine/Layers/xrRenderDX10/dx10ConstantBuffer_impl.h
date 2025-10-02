#pragma once

#if defined(_DEBUG) || defined(OGSR_TOTAL_DBG)
#define CBUF_ASSERT R_ASSERT
#else
#define CBUF_ASSERT __noop
#endif

template <class T>
IC T* dx10ConstantBuffer::Access(const u32 offset)
{
    //	Check buffer size in client code: don't know if actual data will cross buffer boundaries.
    CBUF_ASSERT(offset < m_uiBufferSize);

    BYTE* res = reinterpret_cast<BYTE*>(m_pBufferData) + offset;
    return reinterpret_cast<T*>(res);
}

template <typename T>
IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, const T& A)
{
    if constexpr (std::is_same_v<T, float> || std::is_same_v<T, Fvector2> || std::is_same_v<T, Fvector3> || std::is_same_v<T, Fvector4> || std::is_same_v<T, Fmatrix>)
        CBUF_ASSERT(RC_float == C->type);
    else if constexpr (std::is_same_v<T, int>)
        CBUF_ASSERT(RC_int == C->type);
    else if constexpr (std::is_same_v<T, bool>)
        CBUF_ASSERT(RC_bool == C->type);
    else
        static_assert(false, "something strange with arg typename!");

    if constexpr (std::is_same_v<T, Fmatrix>)
    {
        auto set_matrix = [&](const u32 cnt) {
            CBUF_ASSERT(static_cast<u32>(L.index + cnt * sizeof(Fvector4)) <= m_uiBufferSize);

            Fvector4* it = Access<Fvector4>(L.index);
            for (u32 i{}; i < cnt; ++i)
            {
                const Fvector4 cmp{A.m[0][i], A.m[1][i], A.m[2][i], A.m[3][i]};
                if (m_bChanged || memcmp(&it[i], &cmp, sizeof(*it)))
                {
                    it[i].set(cmp);
                    m_bChanged = true;
                }
            }
        };

        switch (L.cls)
        {
        case RC_2x4: set_matrix(2); break;
        case RC_3x4: set_matrix(3); break;
        case RC_4x4: set_matrix(4); break;
        default: FATAL("Invalid constant run-time-type for '%s'", C->name.c_str());
        }
    }
    else
    {
        if constexpr (std::is_same_v<T, float> || std::is_same_v<T, int> || std::is_same_v<T, bool>)
            CBUF_ASSERT(RC_1x1 == L.cls);
        else if constexpr (std::is_same_v<T, Fvector2>)
            CBUF_ASSERT(RC_1x2 == L.cls);
        else if constexpr (std::is_same_v<T, Fvector3>)
            CBUF_ASSERT(RC_1x3 == L.cls || RC_1x2 == L.cls);
        else if constexpr (std::is_same_v<T, Fvector4>)
            CBUF_ASSERT(RC_1x4 == L.cls || RC_1x3 == L.cls || RC_1x2 == L.cls);

        CBUF_ASSERT(static_cast<u32>(L.index + sizeof(T)) <= m_uiBufferSize);

        T* it = Access<T>(L.index);
        if (m_bChanged || memcmp(it, &A, sizeof(T)))
        {
            *it = A;
            m_bChanged = true;
        }
    }
}

template <typename T>
IC void dx10ConstantBuffer::seta(R_constant* C, R_constant_load& L, const u32 e, const T& A)
{
    if constexpr (std::is_same_v<T, Fvector2> || std::is_same_v<T, Fvector3> || std::is_same_v<T, Fvector4> || std::is_same_v<T, Fmatrix>)
        CBUF_ASSERT(RC_float == C->type);
    else
        static_assert(false, "something strange with arg typename!");

    if constexpr (std::is_same_v<T, Fmatrix>)
    {
        auto set_matrix = [&](const u32 cnt) {
            auto base = L.index + cnt * sizeof(Fvector4) * e;
            CBUF_ASSERT((base + cnt * sizeof(Fvector4)) <= m_uiBufferSize);

            Fvector4* it = Access<Fvector4>(base);
            for (u32 i{}; i < cnt; ++i)
            {
                const Fvector4 cmp{A.m[0][i], A.m[1][i], A.m[2][i], A.m[3][i]};
                if (m_bChanged || memcmp(&it[i], &cmp, sizeof(*it)))
                {
                    it[i].set(cmp);
                    m_bChanged = true;
                }
            }
        };

        switch (L.cls)
        {
        case RC_2x4: set_matrix(2); break;
        case RC_3x4: set_matrix(3); break;
        case RC_4x4: set_matrix(4); break;
        default: FATAL("Invalid constant run-time-type for '%s'", C->name.c_str());
        }
    }
    else
    {
        if constexpr (std::is_same_v<T, Fvector2>)
            CBUF_ASSERT(RC_1x2 == L.cls);
        else if constexpr (std::is_same_v<T, Fvector3>)
            CBUF_ASSERT(RC_1x3 == L.cls || RC_1x2 == L.cls);
        else if constexpr (std::is_same_v<T, Fvector4>)
            CBUF_ASSERT(RC_1x4 == L.cls || RC_1x3 == L.cls || RC_1x2 == L.cls);

        const u32 base = L.index + sizeof(Fvector4) * e; //Буфер выровнен по 16 байт, потому здесь смещение на sizeof(Fvector4)
        CBUF_ASSERT((base + sizeof(T)) <= m_uiBufferSize);

        T* it = Access<T>(base);
        if (m_bChanged || memcmp(it, &A, sizeof(T)))
        {
            *it = A;
            m_bChanged = true;
        }
    }
}

IC void* dx10ConstantBuffer::AccessDirect(R_constant_load& L, const u32 DataSize) // this always mark buffer as dirty
{
    //	Check buffer size in client code: don't know if actual data will cross buffer boundaries.
    CBUF_ASSERT(L.index < m_uiBufferSize);

    if (L.index + DataSize <= m_uiBufferSize)
    {
        m_bChanged = true;

        BYTE* res = reinterpret_cast<BYTE*>(m_pBufferData) + L.index;
        return res;
    }
    else
        return nullptr;
}
