#ifndef PH_COMMANDER_H
#define PH_COMMANDER_H
class CPHReqBase;
class CPHReqComparerV;

class CPHReqBase
{
public:
    virtual ~CPHReqBase() {}
    virtual bool obsolete() const = 0;
    virtual bool compare(const CPHReqComparerV* v) const { return false; };
};

class CPHCondition : public CPHReqBase
{
public:
    virtual bool is_true() = 0;
};

class CPHAction : public CPHReqBase
{
public:
    virtual void run() = 0;
};

class CPHOnesCondition : public CPHCondition
{
    bool b_called;

public:
    CPHOnesCondition() { b_called = false; }
    virtual bool is_true()
    {
        b_called = true;
        return true;
    }
    virtual bool obsolete() const { return b_called; }
};

class CPHDummiAction : public CPHAction
{
public:
    virtual void run() { ; }
    virtual bool obsolete() const { return false; }
};

class CPHCall
{
    CPHAction* m_action;
    CPHCondition* m_condition;

    bool removed;
    u32 paused;

public:
    CPHCall(CPHCondition* condition, CPHAction* action);
    ~CPHCall();
    void check();
    bool obsolete();
    bool equal(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action);
    bool is_any(CPHReqComparerV* v);

    bool isPaused();
    bool isNeedRemove();
    void removeLater();
    void setPause(u32 ms);
};

DEFINE_VECTOR(std::unique_ptr<CPHCall>, PHCALL_STORAGE, PHCALL_I);
class CPHCommander
{
    PHCALL_STORAGE m_calls;

public:
    ~CPHCommander();
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    CPHCall* add_call_unique(CPHCondition* condition, CPHReqComparerV* cmp_condition, CPHAction* action, CPHReqComparerV* cmp_action);
    CPHCall* add_call(CPHCondition* condition, CPHAction* action);

    bool has_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action);
    PHCALL_I find_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action);
    void remove_call(CPHReqComparerV* cmp_condition, CPHReqComparerV* cmp_action);
    void remove_calls(CPHReqComparerV* cmp_object);

    void update();

    void clear();
};
#endif
