#pragma once

class ECORE_API R_constants
{
    enum BufferType
    {
        BT_PixelBuffer,
        BT_VertexBuffer,
        BT_GeometryBuffer,
        BT_HullBuffer,
        BT_DomainBuffer,
        BT_Compute
    };

public:
    //	ALIGN(16)	R_constant_array	a_pixel;
    //	ALIGN(16)	R_constant_array	a_vertex;

    explicit R_constants(CBackend& cmd_list_in) : cmd_list(cmd_list_in) {}
    void flush_cache() const;

public:
    // fp, non-array versions
    template <typename... Args>
    ICF void set(R_constant* C, Args&&... args)
    {
        if (C->destination & RC_dest_pixel)
        {
            set<BT_PixelBuffer>(C, C->ps, std::forward<Args>(args)...);
        } // a_pixel.b_dirty=TRUE;		}
        if (C->destination & RC_dest_vertex)
        {
            set<BT_VertexBuffer>(C, C->vs, std::forward<Args>(args)...);
        } //  a_vertex.b_dirty=TRUE;		}
        if (C->destination & RC_dest_geometry)
        {
            set<BT_GeometryBuffer>(C, C->gs, std::forward<Args>(args)...);
        } //  a_vertex.b_dirty=TRUE;		}
        if (C->destination & RC_dest_hull)
        {
            set<BT_HullBuffer>(C, C->hs, std::forward<Args>(args)...);
        } //  a_vertex.b_dirty=TRUE;		}
        if (C->destination & RC_dest_domain)
        {
            set<BT_DomainBuffer>(C, C->ds, std::forward<Args>(args)...);
        } //  a_vertex.b_dirty=TRUE;		}
    }

    // scalars, non-array versions
    ICF void set(R_constant* C, float x, float y)
    {
        Fvector2 data;
        data.set(x, y);
        set(C, data);
    }
    ICF void set(R_constant* C, float x, float y, float z)
    {
        Fvector3 data;
        data.set(x, y, z);
        set(C, data);
    }
    ICF void set(R_constant* C, float x, float y, float z, float w)
    {
        Fvector4 data;
        data.set(x, y, z, w);
        set(C, data);
    }

    // fp, array versions
    template <typename... Args>
    ICF void seta(R_constant* C, u32 e, Args&&... args)
    {
        if (C->destination & RC_dest_pixel)
        {
            seta<BT_PixelBuffer>(C, C->ps, e, std::forward<Args>(args)...);
        } //  a_pixel.b_dirty=TRUE;	}
        if (C->destination & RC_dest_vertex)
        {
            seta<BT_VertexBuffer>(C, C->vs, e, std::forward<Args>(args)...);
        } //  a_vertex.b_dirty=TRUE;	}
        if (C->destination & RC_dest_geometry)
        {
            seta<BT_GeometryBuffer>(C, C->gs, e, std::forward<Args>(args)...);
        } //  a_vertex.b_dirty=TRUE;	}
        if (C->destination & RC_dest_hull)
        {
            seta<BT_HullBuffer>(C, C->hs, e, std::forward<Args>(args)...);
        } //  a_vertex.b_dirty=TRUE;		}
        if (C->destination & RC_dest_domain)
        {
            seta<BT_DomainBuffer>(C, C->ds, e, std::forward<Args>(args)...);
        } //  a_vertex.b_dirty=TRUE;		}
    }

    // scalars, array versions
    ICF void seta(R_constant* C, u32 e, float x, float y)
    {
        Fvector2 data;
        data.set(x, y);
        seta(C, e, data);
    }
    ICF void seta(R_constant* C, u32 e, float x, float y, float z)
    {
        Fvector3 data;
        data.set(x, y, z);
        seta(C, e, data);
    }
    ICF void seta(R_constant* C, u32 e, float x, float y, float z, float w)
    {
        Fvector4 data;
        data.set(x, y, z, w);
        seta(C, e, data);
    }

    ICF void flush()
    {
        // if (a_pixel.b_dirty || a_vertex.b_dirty)	flush_cache();
        flush_cache();
    }

    ICF void access_direct(R_constant* C, size_t DataSize, void** ppVData, void** ppGData, void** ppPData) const
    {
        if (ppPData)
        {
            if (C->destination & RC_dest_pixel)
            {
                access_direct<BT_PixelBuffer>(C, C->ps, ppPData, DataSize);
            }
            else
                *ppPData = nullptr;
        }

        if (ppVData)
        {
            if (C->destination & RC_dest_vertex)
            {
                access_direct<BT_VertexBuffer>(C, C->vs, ppVData, DataSize);
            }
            else
                *ppVData = nullptr;
        }

        if (ppGData)
        {
            if (C->destination & RC_dest_geometry)
            {
                access_direct<BT_GeometryBuffer>(C, C->gs, ppGData, DataSize);
            }
            else
                *ppGData = nullptr;
        }
    }

private:
    template <BufferType BType, typename... Args>
    void set(R_constant* C, R_constant_load& L, Args&&... args)
    {
        dx10ConstantBuffer& Buffer = GetCBuffer<BType>(C);
        Buffer.set(C, L, std::forward<Args>(args)...);
    }

    template <BufferType BType, typename... Args>
    void seta(R_constant* C, R_constant_load& L, u32 e, Args&&... args)
    {
        dx10ConstantBuffer& Buffer = GetCBuffer<BType>(C);
        Buffer.seta(C, L, e, std::forward<Args>(args)...);
    }

    template <BufferType BType>
    void access_direct(R_constant* C, R_constant_load& L, void** ppData, size_t DataSize) const
    {
        dx10ConstantBuffer& Buffer = GetCBuffer<BType>(C);
        *ppData = Buffer.AccessDirect(L, DataSize);
    }

    template <BufferType BType>
    dx10ConstantBuffer& GetCBuffer(R_constant* C) const = delete; // no implicit specialization

    CBackend& cmd_list;
};

template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_PixelBuffer>(R_constant* C) const;
template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_VertexBuffer>(R_constant* C) const;
template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_GeometryBuffer>(R_constant* C) const;
template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_HullBuffer>(R_constant* C) const;
template <>
dx10ConstantBuffer& R_constants::GetCBuffer<R_constants::BT_DomainBuffer>(R_constant* C) const;
