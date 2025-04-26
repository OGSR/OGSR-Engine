////////////////////////////////////////////////////////////////////////////
//	Module 		: intrusive_ptr.h
//	Created 	: 30.07.2004
//  Modified 	: 8.11.2016 by Im-Dex
//	Author		: Dmitriy Iassenev
//	Description : Intrusive pointer template
////////////////////////////////////////////////////////////////////////////

#pragma once

struct intrusive_base
{
    intrusive_base()  : m_ref_count(0) {}

    template <typename T>
    void release(T* object) 
    {
        //try
        //{
            xr_delete(object);
        //}
        //catch (...) { }
    }

    void acquire()  { ++m_ref_count; }

    bool release()  { return --m_ref_count == 0; }

    bool released() const  { return m_ref_count == 0; }

private:
    std::atomic_size_t m_ref_count;
};

template <typename ObjectType, typename BaseType = intrusive_base>
class intrusive_ptr
{
    using object_type = ObjectType;
    using base_type = BaseType;
    using self_type = intrusive_ptr<object_type, base_type>;

    static_assert(std::is_base_of_v<BaseType, ObjectType>,
        "ObjectType must be derived from the BaseType");

    object_type* m_object;

protected:
    void dec() 
    {
        if (!m_object)
            return;

        if (m_object->release())
            m_object->release(m_object);
    }

public:
    intrusive_ptr()  : m_object(nullptr) {}

    intrusive_ptr(object_type* rhs)  : m_object(rhs)
    {
        if (m_object != nullptr)
            m_object->acquire();
    }

    intrusive_ptr(const self_type& rhs)  : m_object(rhs.m_object)
    {
        if (m_object != nullptr)
            m_object->acquire();
    }

    intrusive_ptr(self_type&& rhs)  : m_object(rhs.m_object) { rhs.m_object = nullptr; }

    ~intrusive_ptr()  { dec(); }

    self_type& operator=(object_type* rhs) 
    {
        dec();
        m_object = rhs;
        if (m_object != nullptr)
            m_object->acquire();

        return *this;
    }

    self_type& operator=(const self_type& rhs) 
    {
        dec();
        m_object = rhs.m_object;
        if (m_object != nullptr)
            m_object->acquire();

        return *this;
    }

    self_type& operator=(self_type&& rhs) 
    {
        dec();
        m_object = rhs.m_object;
        if (m_object != nullptr)
            rhs.m_object = nullptr;
        return *this;
    }

    object_type& operator*() const 
    {
        VERIFY(m_object);
        return *m_object;
    }

    object_type* operator->() const 
    {
        VERIFY(m_object);
        return m_object;
    }

    explicit operator bool() const  { return m_object != nullptr; }

    bool operator==(const self_type& rhs) const  { return m_object == rhs.m_object; }

    bool operator!=(const self_type& rhs) const  { return m_object != rhs.m_object; }

    bool operator<(const self_type& rhs) const  { return m_object < rhs.m_object; }

    bool operator>(const self_type& rhs) const  { return m_object > rhs.m_object; }

    void swap(self_type& rhs) 
    {
        object_type* tmp = m_object;
        m_object = rhs.m_object;
        rhs.m_object = tmp;
    }

    const object_type* get() const  { return m_object; }
};

template <typename object_type, typename base_type>
void swap(intrusive_ptr<object_type, base_type>& lhs,
          intrusive_ptr<object_type, base_type>& rhs) 
{
    lhs.swap(rhs);
}
