#include <map>
#include <string>

namespace header_test {
    
enum choice { red, blue };

std::string choice_str(choice c)
{
    std::map<choice, std::string> choice_map;
    choice_map[red] = "red";
    choice_map[blue] = "blue";
    return choice_map[c];
}

struct C
{
    choice c;

    std::string get()
    {
        return choice_str(c);
    }
};

}
