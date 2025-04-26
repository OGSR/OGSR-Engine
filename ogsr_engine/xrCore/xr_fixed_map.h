#pragma once

// Both xr_fixed_map and xr_fixed_map_node doesn't support move operation
// Trying to do so will result in a crash at some point
// (at first it would be just fine, but this is a lie)

// The container is just designed for copying, not moving

template <typename T>
class xr_allocator_legacy
{
public:
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
    using value_type = T;

    static pointer allocate(const size_type n, const void* p = nullptr) { return xr_alloc<T>(n); }
    static void deallocate(pointer p, const size_type /*n*/) { xr_free(p); }

    template <class U, class... Args>
    static void construct(U* ptr, Args&&... args)
    {
        new (ptr) U(std::forward<Args>(args)...);
    }
};

template <class K, class T>
struct xr_fixed_map_node
{
    K first;
    T second;

    xr_fixed_map_node<K, T>*left, *right;

    xr_fixed_map_node() : first(), second(), left(nullptr), right(nullptr) {}

    ~xr_fixed_map_node()
    {
        left = nullptr;
        right = nullptr;
    }

    xr_fixed_map_node(const xr_fixed_map_node& other) noexcept
    {
        first = other.first;
        second = other.second;
        left = other.left;
        right = other.right;
    }

    xr_fixed_map_node& operator=(const xr_fixed_map_node& other) noexcept
    {
        first = other.first;
        second = other.second;
        left = other.left;
        right = other.right;
        return *this;
    }

    friend void swap(xr_fixed_map_node& left, xr_fixed_map_node& right) noexcept
    {
        std::swap(left.first, right.first);
        std::swap(left.second, right.second);
        std::swap(left.left, right.left);
        std::swap(left.right, right.right);
    }
};

template <class K, class T, size_t TGrowMultiplier = 2, class allocator = xr_allocator_legacy<xr_fixed_map_node<K, T>>>
class xr_fixed_map
{
    static constexpr size_t SG_REALLOC_ADVANCE = 128;

public:
    using key_type = K;
    using mapped_type = T;
    using value_type = xr_fixed_map_node<K, T>;

    using callback = void __fastcall(u32, const value_type&);
    using callback_cmp = bool __fastcall(const value_type& N1, const value_type& N2);

    static_assert(TGrowMultiplier >= 1, "Grow multiplier can't be less than 1");
    static_assert(std::is_same_v<value_type, typename allocator::value_type>, "xr_fixed_map<K, T, allocator> allocator mismatch");

private:
    value_type* nodes;
    size_t pool;
    size_t limit;

    void resize()
    {
        ZoneScoped;

        size_t newLimit;

        if constexpr (TGrowMultiplier > 1)
        {
            if (limit == 0) // first allocation
                newLimit = SG_REALLOC_ADVANCE;
            else
                newLimit = limit * TGrowMultiplier;
        }
        else
        {
            newLimit = limit + SG_REALLOC_ADVANCE;
            VERIFY(newLimit % SG_REALLOC_ADVANCE == 0);
        }

        value_type* newNodes = allocator::allocate(newLimit);
        R_ASSERT(newNodes);

        // (pool == limit) !!!

        if constexpr (std::is_pod<T>::value)
        {
            ZeroMemory(newNodes, sizeof(value_type) * newLimit);

            if (pool)
                CopyMemory(newNodes, nodes, allocated_memory());
        }
        else
        {
            if (limit)
                std::copy(first(), last(), newNodes);

            for (value_type* cur = newNodes + limit; cur != newNodes + newLimit; ++cur) // construct only new nodes
                allocator::construct(cur);            
        }

        for (size_t i = 0; i < pool; ++i)
        {
            VERIFY(nodes);
            value_type* nodeOld = nodes + i;
            value_type* nodeNew = newNodes + i;

            if (nodeOld->left)
            {
                size_t leftId = nodeOld->left - nodes;
                nodeNew->left = newNodes + leftId;
            }
            if (nodeOld->right)
            {
                size_t rightId = nodeOld->right - nodes;
                nodeNew->right = newNodes + rightId;
            }
        }

        if (nodes)
            allocator::deallocate(nodes, limit);

        nodes = newNodes;
        limit = newLimit;
    }

    value_type* add(const K& key)
    {
        if (pool == limit)
            resize();

        value_type* node = nodes + pool;
        node->first = key;
        node->left = nullptr;
        node->right = nullptr;
        ++pool;

        return node;
    }

    value_type* create_child(value_type*& parent, const K& key)
    {
        ptrdiff_t PID = ptrdiff_t(parent - nodes);
        value_type* N = add(key);
        parent = nodes + PID;
        return N;
    }

    void recurse_left_right(u32 id, value_type* N, callback CB)
    {
        if (N->left)
            recurse_left_right(id, N->left, CB);
        CB(id, *N);
        if (N->right)
            recurse_left_right(id, N->right, CB);
    }

    void recurse_right_left(u32 id, value_type* N, callback CB)
    {
        if (N->right)
            recurse_right_left(id, N->right, CB);
        CB(id, *N);
        if (N->left)
            recurse_right_left(id, N->left, CB);
    }

    void get_left_right(value_type* N, xr_vector<T, xr_allocator<T>>& D)
    {
        if (N->left)
            get_left_right(N->left, D);
        D.push_back(N->second);
        if (N->right)
            get_left_right(N->right, D);
    }

    void get_right_left(value_type* N, xr_vector<T, xr_allocator<T>>& D)
    {
        if (N->right)
            get_right_left(N->right, D);
        D.push_back(N->second);
        if (N->left)
            get_right_left(N->left, D);
    }

public:
    xr_fixed_map()
    {
        pool = 0;
        limit = 0;
        nodes = nullptr;
    }

    ~xr_fixed_map() { destroy(); }

    void destroy()
    {
        if (nodes)
        {
            for (value_type* cur = begin(); cur != last(); ++cur)
                cur->~value_type();
            allocator::deallocate(nodes, limit);
        }
        nodes = 0;
        pool = 0;
        limit = 0;
    }

    value_type* insert(const K& key)
    {
        if (!pool)
            return add(key);

        value_type* node = nodes;

    once_more:
        if (key < node->first)
        {
            if (node->left)
            {
                node = node->left;
                goto once_more;
            }
            else
            {
                value_type* N = create_child(node, key);
                node->left = N;
                return N;
            }
        }
        else if (key > node->first)
        {
            if (node->right)
            {
                node = node->right;
                goto once_more;
            }
            else
            {
                value_type* N = create_child(node, key);
                node->right = N;
                return N;
            }
        }
        else
            return node;
    }

    value_type* insert_anyway(const K& key)
    {
        if (!pool)
            return add(key);

        value_type* node = nodes;

    once_more:
        if (key <= node->first)
        {
            if (node->left)
            {
                node = node->left;
                goto once_more;
            }
            else
            {
                value_type* N = create_child(node, key);
                node->left = N;
                return N;
            }
        }
        else
        {
            if (node->right)
            {
                node = node->right;
                goto once_more;
            }
            else
            {
                value_type* N = create_child(node, key);
                node->right = N;
                return N;
            }
        }
    }

    value_type* insert(const K& key, const T& value)
    {
        value_type* N = insert(key);
        N->second = value;
        return N;
    }

    value_type* insert_anyway(const K& key, const T& value)
    {
        value_type* N = insert_anyway(key);
        N->second = value;
        return N;
    }

    value_type* insert(K&& key, T&& value)
    {
        value_type* N = insert(key);
        N->second = value;
        return N;
    }

    value_type* insert_anyway(K&& key, T&& value)
    {
        value_type* N = insert_anyway(key);
        N->second = value;
        return N;
    }

    size_t allocated() const { return limit; }
    size_t allocated_memory() const { return limit * sizeof(value_type); }

    [[nodiscard]] bool empty() const { return pool == 0; }
    void clear() { pool = 0; }

    value_type* begin() { return nodes; }
    value_type* end() { return nodes + pool; }

    value_type* first() { return nodes; }
    value_type* last() { return nodes + limit; } // for setup only

    [[nodiscard]] size_t size() const { return pool; }

    value_type& at_index(size_t v) { return nodes[v]; }

    mapped_type& at(const key_type& key) { return insert(key)->second; }
    mapped_type& operator[](const key_type& key) { return insert(key)->second; }

    void traverse_left_right(u32 id, callback CB)
    {
        if (pool)
            recurse_left_right(id, nodes, CB);
    }

    void traverse_right_left(u32 id, callback CB)
    {
        if (pool)
            recurse_right_left(id, nodes, CB);
    }

    void get_left_right(xr_vector<T>& D)
    {
        if (pool)
            get_left_right(nodes, D);
    }

    void get_right_left(xr_vector<T>& D)
    {
        if (pool)
            get_right_left(nodes, D);
    }

    void get_any_p(xr_vector<value_type*>& D)
    {
        if (empty())
            return;
        D.resize(size());
        value_type** _it = &D.front();
        value_type* _end = end();
        for (value_type* cur = begin(); cur != _end; ++cur, ++_it)
            *_it = cur;
    }

    //void setup(callback CB)
    //{
    //    for (size_t i = 0; i < limit; i++)
    //        CB(*(nodes + i));
    //}
};
