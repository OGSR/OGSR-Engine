#pragma once

class VertexCache
{
public:
    VertexCache(int size);
    VertexCache();
    ~VertexCache();

    bool InCache(int entry) const;
    int AddEntry(int entry);
    void Clear();

    void Copy(VertexCache* inVcache);
    int At(int index) const;
    void Set(int index, int value);

private:
    xr_vector<int> entries;
};

IC bool VertexCache::InCache(int entry) const
{
    bool returnVal = false;

    for (const int entrie : entries)
    {
        if (entrie == entry)
        {
            returnVal = true;
            break;
        }
    }

    return returnVal;
}

IC int VertexCache::AddEntry(int entry)
{
    int removed;

    removed = entries[entries.size() - 1];

    // push everything right one
    for (int i = (u32)entries.size() - 2; i >= 0; i--)
    {
        entries[i + 1] = entries[i];
    }

    entries[0] = entry;

    return removed;
}
