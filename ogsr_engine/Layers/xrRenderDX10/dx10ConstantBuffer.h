#pragma once

struct R_constant;
struct R_constant_load;

class dx10ConstantBuffer : public xr_resource_named
{
public:
    dx10ConstantBuffer(ID3DShaderReflectionConstantBuffer* pTable);
    ~dx10ConstantBuffer();

    bool Similar(const dx10ConstantBuffer& _in) const;
    ID3DBuffer* GetBuffer() const { return m_pBuffer; }
    void Flush(u32 context_id);

    // Set copy data into constant buffer
    template <typename T>
    void set(R_constant* C, R_constant_load& L, const T& A);

    // Array buffer member
    template <typename T>
    void seta(R_constant* C, R_constant_load& L, const u32 e, const T& A);

    void* AccessDirect(R_constant_load& L, const u32 DataSize);

    void dbg_dump() const;

private:
    template <class T>
    T* Access(const u32 offset);

    shared_str m_strBufferName;
    D3D_CBUFFER_TYPE m_eBufferType;

    //	Buffer data description
    u32 m_uiMembersCRC;
    xr_vector<D3D_SHADER_TYPE_DESC> m_MembersList;
    xr_vector<shared_str> m_MembersNames;

    ID3DBuffer* m_pBuffer;
    u32 m_uiBufferSize; //	Cache buffer size for debug validation
    void* m_pBufferData;
    bool m_bChanged;

    //	Never try to copy objects of this class due to the pointer and autoptr members
    dx10ConstantBuffer(const dx10ConstantBuffer&) = delete;
    dx10ConstantBuffer& operator=(dx10ConstantBuffer&) = delete;
};

typedef resptr_core<dx10ConstantBuffer, resptr_base<dx10ConstantBuffer>> ref_cbuffer;
