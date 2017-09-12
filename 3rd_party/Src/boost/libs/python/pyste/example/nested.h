namespace nested {
    
struct X
{
    struct Y 
    {
        int valueY;
        static int staticYValue; 
        struct Z
        {
            int valueZ;
        };
    };
    
    static int staticXValue;
    int valueX;
};

int X::staticXValue = 10;
int X::Y::staticYValue = 20;

typedef X Root;

}
