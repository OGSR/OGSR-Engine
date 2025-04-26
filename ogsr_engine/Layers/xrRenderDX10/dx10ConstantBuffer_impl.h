#pragma once

IC Fvector4* dx10ConstantBuffer::Access(u16 offset)
{
    m_bChanged = true;

    //	Check buffer size in client code: don't know if actual data will cross
    //	buffer boundaries.
    VERIFY(offset < (int)m_uiBufferSize);
    BYTE* res = ((BYTE*)m_pBufferData) + offset;
    return (Fvector4*)res;
}

inline void set_matrix(Fvector4* it, const Fmatrix& A, u32 cnt, bool& changed)
{
    for (u32 i=0;i< cnt;i++)
    {
        if (changed || !it[i].similar(A.m[0][i], A.m[1][i], A.m[2][i], A.m[3][i]))
        {
            it[i].set(A.m[0][i], A.m[1][i], A.m[2][i], A.m[3][i]);

            changed = true;
        }
    }
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, const Fmatrix& A)
{
    VERIFY(RC_float == C->type);

    bool changed = m_bChanged;

    Fvector4* it = Access(L.index);
    switch (L.cls)
    {
    case RC_2x4:
        VERIFY(u32((u32)L.index + 2 * lineSize) <= m_uiBufferSize);
        set_matrix(it, A, 2, changed);
        break;
    case RC_3x4:
        VERIFY(u32((u32)L.index + 3 * lineSize) <= m_uiBufferSize);
        set_matrix(it, A, 3, changed);
        break;
    case RC_4x4:
        VERIFY(u32((u32)L.index + 4 * lineSize) <= m_uiBufferSize);
        set_matrix(it, A, 4, changed);
        break;
    default:
#ifdef DEBUG
        FATAL("Invalid constant run-time-type for '%s'", *C->name);
#else
        NODEFAULT;
#endif
    }

    if (changed)
        m_bChanged = true;
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, const Fvector4& A)
{
    VERIFY(RC_float == C->type);
    VERIFY(RC_1x4 == L.cls || RC_1x3 == L.cls || RC_1x2 == L.cls);

    VERIFY(u32((u32)L.index + lineSize) <= m_uiBufferSize);

    bool changed = m_bChanged;

    Fvector4* it = Access(L.index);
    if (changed || !it->similar(A))
    {
        it->set(A);
        changed = true;
    }

    if (changed)
        m_bChanged = true;
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, float A)
{
    VERIFY(RC_float == C->type);
    VERIFY(RC_1x1 == L.cls);
    VERIFY(u32((u32)L.index + sizeof(float)) <= m_uiBufferSize);

    bool changed = m_bChanged;

    float* it = (float*)Access(L.index);
    if (changed || !fsimilar(*it, A))
    {
        *it = A;
        changed = true;
    }

    if (changed)
        m_bChanged = true;
}

IC void dx10ConstantBuffer::set(R_constant* C, R_constant_load& L, int A)
{
    VERIFY(RC_int == C->type);
    VERIFY(RC_1x1 == L.cls);
    VERIFY(u32((u32)L.index + sizeof(int)) <= m_uiBufferSize);

    bool changed = m_bChanged;

    int* it = (int*)Access(L.index);
    if (changed || (*it) != A)
    {
        *it = A;
        changed = true;
    }

    if (changed)
        m_bChanged = true;
}

IC void dx10ConstantBuffer::seta(R_constant* C, R_constant_load& L, u32 e, const Fmatrix& A)
{
    VERIFY(RC_float == C->type);

    bool changed = m_bChanged;

    u32 base;
    Fvector4* it;
    switch (L.cls)
    {
    case RC_2x4:
        base = (u32)L.index + 2 * lineSize * e;
        VERIFY((base + 2 * lineSize) <= m_uiBufferSize);
        it = Access((u16)base);
        set_matrix(it, A, 2, changed);
        break;
    case RC_3x4:
        base = (u32)L.index + 3 * lineSize * e;
        VERIFY((base + 3 * lineSize) <= m_uiBufferSize);
        it = Access((u16)base);
        set_matrix(it, A, 3, changed);
        break;
    case RC_4x4:
        base = (u32)L.index + 4 * lineSize * e;
        VERIFY((base + 4 * lineSize) <= m_uiBufferSize);
        it = Access((u16)base);
        set_matrix(it, A, 4, changed);
        break;
    default:
#ifdef DEBUG
        FATAL("Invalid constant run-time-type for '%s'", *C->name);
#else
        NODEFAULT;
#endif
    }

    if (changed)
        m_bChanged = true;
}

IC void dx10ConstantBuffer::seta(R_constant* C, R_constant_load& L, u32 e, const Fvector4& A)
{
    VERIFY(RC_float == C->type);
    VERIFY(RC_1x4 == L.cls || RC_1x3 == L.cls || RC_1x2 == L.cls);

    u32 base = (u32)L.index + lineSize * e;
    VERIFY((base + lineSize) <= m_uiBufferSize);

    bool changed = m_bChanged;

    Fvector4* it = Access((u16)base);
    if (changed || !it->similar(A))
    {
        it->set(A);
        changed = true;
    }

    if (changed)
        m_bChanged = true;
}

IC void* dx10ConstantBuffer::AccessDirect(R_constant_load& L, u32 DataSize) // this always mark buffer as dirty
{
    //	Check buffer size in client code: don't know if actual data will cross
    //	buffer boundaries.
    VERIFY(L.index < (int)m_uiBufferSize);

    if ((u32)L.index + DataSize <= m_uiBufferSize)
    {
        m_bChanged = true;

        BYTE* res = ((BYTE*)m_pBufferData) + L.index;
        return res;
    }
    else
        return nullptr;
}
