
struct C
{
public:
    virtual int f()
    {
        return f_abs();
    }

    const char* get_name()
    {
        return name();
    }

protected:    
    virtual int f_abs() = 0;

private:
    virtual const char* name() { return "C"; }
};

int call_f(C& c) { return c.f(); }

