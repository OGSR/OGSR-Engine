// Example by Ralf W. Grosse-Kunstleve

/*
    This example shows how to make an Extension Class "pickleable".

    The world class below can be fully restored by passing the
    appropriate argument to the constructor. Therefore it is sufficient
    to define the pickle interface method __getinitargs__.

    For more information refer to boost/libs/python/doc/pickle.html.
 */

#include <boost/python/module.hpp>
#include <boost/python/def.hpp>
#include <boost/python/class.hpp>
#include <boost/python/tuple.hpp>

#include <string>

namespace {

  // A friendly class.
  class world
  {
    private:
      std::string country;
    public:
      world(const std::string& country) {
        this->country = country;
      }
      std::string greet() const { return "Hello from " + country + "!"; }
      std::string get_country() const { return country; }
  };

  struct world_pickle_suite : boost::python::pickle_suite
  {
    static
    boost::python::tuple
    getinitargs(const world& w)
    {
        using namespace boost::python;
        return make_tuple(w.get_country());
    }
  };

}

BOOST_PYTHON_MODULE(pickle1_ext)
{
  using namespace boost::python;
  class_<world>("world", init<const std::string&>())
      .def("greet", &world::greet)
      .def_pickle(world_pickle_suite())
      ;
}
