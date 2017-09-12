#include <windows.h>
#include "alThunk.h"

class CALThunkListItem
{
private:
    CALThunkListItem *  m_Next;
    CALThunkListItem *  m_Prev;

public:
    ALvoid *            m_ALThunkPointer;
    ALuint              m_ALThunkIndex;

    friend class CALThunkList;
};

class CALThunkList
{
private:
    CALThunkListItem *  m_Head;
    CALThunkListItem *  m_Tail;
    ALuint              m_NumItems;
    CRITICAL_SECTION    m_ListLock;

public:
    CALThunkList()      { m_Head = m_Tail = NULL; m_NumItems = 0; }

    ~CALThunkList()     { DeleteCriticalSection(&m_ListLock); };

    void Init(CALThunkListItem * Items, ALuint ItemCount)   { InitializeCriticalSection(&m_ListLock);
                                                              for (ALuint i=0; i<ItemCount; i++) {
                                                              CALThunkListItem * Item = &Items[i];
                                                              Item->m_Next = Item->m_Prev = NULL;
                                                              Item->m_ALThunkPointer = NULL;
                                                              Item->m_ALThunkIndex = i+1;
                                                              Put(Item); }}

    void Lock()                                 { EnterCriticalSection(&m_ListLock); }
    void Unlock()                               { LeaveCriticalSection(&m_ListLock); }

    BOOLEAN IsItemInList(CALThunkListItem * Item) { CALThunkListItem * current = m_Head;
                                                    while (current) {if (current==Item) return TRUE; current = current->m_Next;}
                                                    return FALSE; }

    BOOLEAN IsEmpty()                           { return (m_Head == NULL); }

    ULONG Put(CALThunkListItem * Item)          { //ASSERT(!IsItemInList(Item));
                                                  Item->m_Next = NULL;
                                                  Item->m_Prev = NULL;
                                                  if (m_Head) {m_Head->m_Prev = Item; Item->m_Next = m_Head; m_Head = Item;}
                                                  else {m_Head = m_Tail = Item;}
                                                  m_NumItems++;
                                                  return m_NumItems; }

    ULONG Remove(CALThunkListItem * Item)       { if (!IsItemInList(Item)) {/*ASSERT(0);*/ return m_NumItems;}
                                                  if (Item->m_Prev) {Item->m_Prev->m_Next = Item->m_Next;}
                                                  if (Item->m_Next) {Item->m_Next->m_Prev = Item->m_Prev;}
                                                  if (Item==m_Head) {m_Head = Item->m_Next;}
                                                  if (Item==m_Tail) {m_Tail=Item->m_Prev;}
                                                  Item->m_Next = NULL;
                                                  Item->m_Prev = NULL;
                                                  m_NumItems--;
                                                  return m_NumItems; }

    CALThunkListItem * Pop()                      { if (m_Tail) {CALThunkListItem * Item = m_Tail; Remove(Item); return Item;}
                                                    return NULL; }

    CALThunkListItem * First()                      { return m_Tail; }
    CALThunkListItem * Last()                       { return m_Head; }

    CALThunkListItem * Next(CALThunkListItem * Item) { //ASSERT(IsItemInList(Item));
                                                       if (!IsItemInList(Item)) return NULL;
                                                       return Item->m_Prev; }

    ULONG Count()                               { return m_NumItems; }
};

CALThunkList FreeThunkList;
CALThunkList InUseThunkList;

CALThunkListItem ThunkItems[8192];

void alThunkInit(void)
{
    FreeThunkList.Init(ThunkItems, sizeof(ThunkItems)/sizeof(ThunkItems[0]));
    InUseThunkList.Init(NULL, 0);
}

void alThunkExit(void)
{
}

ALuint alThunkAddEntry(ALvoid * ptr)
{
    ALuint index = 0;

    FreeThunkList.Lock();
    InUseThunkList.Lock();

    CALThunkListItem * Item = FreeThunkList.Pop();

    if (Item)
    {
        Item->m_ALThunkPointer = ptr;

        InUseThunkList.Put(Item);

        index = Item->m_ALThunkIndex;
    }

    InUseThunkList.Unlock();
    FreeThunkList.Unlock();

    return index;
}

void alThunkRemoveEntry(ALuint index)
{
    FreeThunkList.Lock();
    InUseThunkList.Lock();

    if ((index > 0) && (index <= sizeof(ThunkItems)/sizeof(ThunkItems[0])))
    {
        CALThunkListItem * Item = &ThunkItems[index-1];

        Item->m_ALThunkPointer = NULL;

        InUseThunkList.Remove(Item);

        FreeThunkList.Put(Item);
    }

    InUseThunkList.Unlock();
    FreeThunkList.Unlock();
}

ALvoid * alThunkLookupEntry(ALuint index)
{
    ALvoid * ptr = NULL;

    FreeThunkList.Lock();
    InUseThunkList.Lock();

    if ((index > 0) && (index <= sizeof(ThunkItems)/sizeof(ThunkItems[0])))
    {
        CALThunkListItem * Item = &ThunkItems[index-1];

        ptr = Item->m_ALThunkPointer;
    }

    InUseThunkList.Unlock();
    FreeThunkList.Unlock();

    return ptr;
}

