#pragma once


IC HRESULT CreateQuery(ID3DQuery** ppQuery, D3DQUERYTYPE Type)
{
    D3D_QUERY_DESC desc;
    desc.MiscFlags = 0;

    switch (Type)
    {
    case D3DQUERYTYPE_OCCLUSION: desc.Query = D3D_QUERY_OCCLUSION; break;
    default: VERIFY(!"No default.");
    }

    return HW.pDevice->CreateQuery(&desc, ppQuery);
}

IC HRESULT GetData(ID3DQuery* pQuery, void* pData, UINT DataSize, UINT GetDataFlags = 0)
{
    return HW.get_context(CHW::IMM_CTX_ID)->GetData(pQuery, pData, DataSize, GetDataFlags);
}

IC HRESULT BeginQuery(ID3DQuery* pQuery, u32 context_id)
{
    HW.get_context(context_id)->Begin(pQuery);
    return S_OK;
}

IC HRESULT EndQuery(ID3DQuery* pQuery, u32 context_id)
{
    HW.get_context(context_id)->End(pQuery);
    return S_OK;
}
