#pragma once

#include "xrMessages.h"

class NET_Event
{
public:
    u16 ID;
    u32 timestamp;
    u16 type;
    u16 destination;
    xr_vector<u8> data;

public:
    void import(NET_Packet& P)
    {
        data.clear();
        P.r_begin(ID); // VERIFY(M_EVENT==ID);
        switch (ID)
        {
        case M_SPAWN: {
            P.read_start();
            //				timestamp = P->
        }
        break;
        case M_EVENT: {
            P.r_u32(timestamp);
            P.r_u16(type);
            P.r_u16(destination);
        }
        break;
        default: {
            VERIFY(0);
        }
        break;
        }

        u32 size = P.r_elapsed();
        if (size)
        {
            data.resize(size);
            P.r(&*data.begin(), size);
        }
    }
    void n_export(NET_Packet& P)
    {
        u16 ID = M_EVENT;
        P.w_begin(ID);
        P.w_u32(timestamp);
        P.w_u16(type);
        P.w_u16(destination);
        if (data.size())
            P.w(&*data.begin(), (u32)data.size());
    }
    void implication(NET_Packet& P) const
    {
        std::copy(data.begin(), data.end(), P.B.data);
        P.B.count = data.size();
        P.r_pos = 0;
    }
};

IC bool operator<(const NET_Event& A, const NET_Event& B) { return A.timestamp < B.timestamp; }

class NET_Queue_Event
{
public:
    xr_deque<NET_Event> queue;

public:
    IC void insert(NET_Packet& P)
    {
        NET_Event E;
        E.import(P);
        queue.push_back(E);
    }
    IC BOOL available(u32 T)
    {
        if (queue.empty())
            return FALSE;

        return TRUE;
    }
    IC void get(u16& ID, u16& dest, u16& type, NET_Packet& P)
    {
        const NET_Event& E = *queue.begin();
        ID = E.ID;
        dest = E.destination;
        type = E.type;
        E.implication(P);
        queue.pop_front();
    }
};
