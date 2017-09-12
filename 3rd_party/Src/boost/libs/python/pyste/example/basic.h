namespace basic {
    
struct C
{    
    virtual int f(int x = 10)
    {
        return x*2;
    }    
    
    int foo(int x=1){
        return x+1;
    }
};

int call_f(C& c)
{
    return c.f();
}

int call_f(C& c, int x)
{
    return c.f(x);
} 

}
