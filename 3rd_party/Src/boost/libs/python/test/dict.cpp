#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include <boost/python/dict.hpp>
#include <exception>
#include <string>

using namespace boost::python;

object new_dict()
{
    return dict();
}

object data_dict()
{
    dict tmp1;
    tmp1["key1"] = "value1";

    dict tmp2;
    tmp2["key2"] = "value2";
    tmp1[1] = tmp2;
    return tmp1;
}

object dict_from_sequence(object sequence)
{
    return dict(sequence);
}

object dict_keys(dict data)
{
    return data.keys();
}

object dict_values(dict data)
{
    return data.values();
}

object dict_items(dict data)
{
    return data.items();
}

void work_with_dict(dict data1, dict data2)
{
    if (!data1.has_key("k1")) {
        throw std::runtime_error("dict does not have key 'k1'");
    }
    data1.update(data2);
}

void test_templates(object print)
{
    std::string key = "key";
    
    dict tmp;
    tmp[1] = "a test string";
    print(tmp.get(1));
    //print(tmp[1]);
    tmp[1.5] = 13;
    print(tmp.get(1.5));
    print(tmp.get(44));
    print(tmp);
    print(tmp.get(2,"default"));
    print(tmp.has_key(key));
    print(tmp.setdefault(3,"default"));
    //print(tmp[3]);
}
    
BOOST_PYTHON_MODULE(dict_ext)
{
    def("new_dict", new_dict);
    def("data_dict", data_dict);
    def("dict_keys", dict_keys);
    def("dict_values", dict_values);
    def("dict_items", dict_items);
    def("dict_from_sequence", dict_from_sequence);
    def("work_with_dict", work_with_dict);
    def("test_templates", test_templates);
}
