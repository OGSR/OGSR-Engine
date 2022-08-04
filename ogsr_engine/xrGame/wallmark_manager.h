#pragma once

class CWalmarkManager
{
private:
    FactoryPtr<IWallMarkArray> m_wallmarks;
    Fvector m_pos;

public:
    CObject* m_owner;
    CWalmarkManager();
    ~CWalmarkManager();
    void Load(LPCSTR section);
    void Clear();
    void PlaceWallmarks(const Fvector& start_pos);

    void __stdcall StartWorkflow();
};
