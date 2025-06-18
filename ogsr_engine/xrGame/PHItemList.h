#pragma once

#define DECLARE_PHLIST_ITEM(class_name) \
    friend class CPHItemList<class_name>; \
    friend class CPHItemListIterator<class_name>; \
    class_name* next; \
    class_name** tome;

template <class T>
class CPHItemListIterator
{
    T* my_ptr{};

public:
    CPHItemListIterator() = default;
    CPHItemListIterator(T* i) : my_ptr(i) {}
    CPHItemListIterator operator++() { return my_ptr = ((my_ptr)->next); }
    T* operator*() { return my_ptr; }
    bool operator!=(const CPHItemListIterator& right) const { return my_ptr != right.my_ptr; }
};

template <class T>
class CPHItemList
{
    T* first_next;
    T** last_tome;

protected:
    u16 size;

public:
    CPHItemList() { empty(); }
    u16 count() { return size; }
    void push_back(T* item)
    {
        *(last_tome) = item;
        item->tome = last_tome;
        last_tome = &((item)->next);
        item->next = 0;
        size++;
    }
    void move_items(CPHItemList<T>& sourse_list)
    {
        if (!sourse_list.first_next)
            return;
        *(last_tome) = sourse_list.first_next;
        sourse_list.first_next->tome = last_tome;
        last_tome = sourse_list.last_tome;
        size = size + sourse_list.size;
        sourse_list.empty();
    }
    void erase(CPHItemListIterator<T>& i)
    {
        T* item = *i;
        T* next = item->next;
        *(item->tome) = next;
        if (next)
            next->tome = item->tome;
        else
            last_tome = item->tome;
        size--;
    }
    void empty()
    {
        last_tome = &first_next;
        first_next = 0;
        size = 0;
    }
    CPHItemListIterator<T> begin() { return CPHItemListIterator<T>(first_next); }
    CPHItemListIterator<T> end() { return CPHItemListIterator<T>(nullptr); }
};

#define DEFINE_PHITEM_LIST(T, N, I) \
    typedef CPHItemList<T> N; \
    typedef CPHItemListIterator<T> I;
