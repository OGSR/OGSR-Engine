namespace enums {
    
enum color { red, blue };

struct X
{
    enum choices
    {
        good = 1,
        bad = 2
    };

    int set(choices c)
    {
        return (int)c;
    }
};

}
