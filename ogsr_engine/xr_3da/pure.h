#pragma once

// messages
#define REG_PRIORITY_LOW 0x11111111ul
#define REG_PRIORITY_NORMAL 0x22222222ul
#define REG_PRIORITY_HIGH 0x33333333ul

#define REG_PRIORITY_INVALID 0xfffffffful

typedef void RP_FUNC(void* obj);

extern ENGINE_API RP_FUNC rp_Frame;
class ENGINE_API pureFrame
{
public:
    virtual void OnFrame(void) = 0;
};
extern ENGINE_API RP_FUNC rp_Render;
class ENGINE_API pureRender
{
public:
    virtual void OnRender(void) = 0;
};
extern ENGINE_API RP_FUNC rp_AppActivate;
class ENGINE_API pureAppActivate
{
public:
    virtual void OnAppActivate(void) = 0;
};
extern ENGINE_API RP_FUNC rp_AppDeactivate;
class ENGINE_API pureAppDeactivate
{
public:
    virtual void OnAppDeactivate(void) = 0;
};
extern ENGINE_API RP_FUNC rp_AppStart;
class ENGINE_API pureAppStart
{
public:
    virtual void OnAppStart(void) = 0;
};
extern ENGINE_API RP_FUNC rp_AppEnd;
class ENGINE_API pureAppEnd
{
public:
    virtual void OnAppEnd(void) = 0;
};
extern ENGINE_API RP_FUNC rp_DeviceReset;
class ENGINE_API pureDeviceReset
{
public:
    virtual void OnDeviceReset(void) = 0;
};
extern ENGINE_API RP_FUNC rp_ScreenResolutionChanged;
class ENGINE_API pureScreenResolutionChanged
{
public:
    virtual void OnScreenResolutionChanged(void) = 0;
};

template <class T>
class CRegistrator // the registrator itself
{
    //-----------------------------------------------------------------------------
    struct _REG_INFO
    {
        T* Object;
        int Prio;
    };

public:
    xr_vector<_REG_INFO> R;

    // constructor
    struct
    {
        u32 in_process : 1;
        u32 changed : 1;
    };

    CRegistrator()
    {
        in_process = false;
        changed = false;
    }

    constexpr void Add(T* object) { Add({object, REG_PRIORITY_NORMAL}); }
    constexpr void Add(T* object, const int priority) { Add({object, priority}); }

    void Add(_REG_INFO&& newMessage)
    {
        bool found = false;

        for (u32 i = 0; i < R.size(); i++)
        {
            if (R[i].Object == newMessage.Object)
            {
                if (R[i].Prio == newMessage.Prio)
                {
                    return; // found with same priority
                }

                R[i].Prio = newMessage.Prio;
                found = true;
            }
        }

        if (!found)
            R.emplace_back(newMessage);

        if (in_process)
            changed = true;
        else
            Resort();
    }

    void Remove(T* obj)
    {
        for (u32 i = 0; i < R.size(); i++)
        {
            if (R[i].Object == obj)
                R[i].Prio = REG_PRIORITY_INVALID;
        }

        if (in_process)
            changed = true;
        else
            Resort();
    }

    void Process(RP_FUNC* f)
    {
        if (R.empty())
            return;

        in_process = true;

        // if (R[0].Prio == REG_PRIORITY_CAPTURE)
        //{
        //     f(R[0].Object);
        // }
        // else
        {
            for (u32 i = 0; i < R.size(); i++)
                if (R[i].Prio != REG_PRIORITY_INVALID)
                    f(R[i].Object);
        }

        if (changed)
            Resort();

        in_process = false;
    }

    void Resort(void)
    {
        if (!R.empty())
        {
            std::sort(std::begin(R), std::end(R), [](const auto& a, const auto& b) { return a.Prio > b.Prio; });
        }

        while (!R.empty() && R[R.size() - 1].Prio == REG_PRIORITY_INVALID)
        {
            R.pop_back();
        }

        if (R.empty())
            R.clear();

        changed = false;
    }
};
