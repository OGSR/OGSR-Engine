#pragma once

// refs
class ENGINE_API CObject;

//-----------------------------------------------------------------------------------------------------------
class ENGINE_API IGame_ObjectPool
{
    typedef xr_vector<CObject*> ObjectVec;
    typedef ObjectVec::iterator ObjectVecIt;
    ObjectVec m_PrefetchObjects;

public:
    void prefetch();
    void clear();

    CObject* create(LPCSTR name);
    void destroy(CObject* O);

    IGame_ObjectPool();
    virtual ~IGame_ObjectPool();
};
//-----------------------------------------------------------------------------------------------------------
