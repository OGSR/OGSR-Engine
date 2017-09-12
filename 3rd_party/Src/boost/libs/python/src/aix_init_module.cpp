// Copyright David Abrahams 2002. Permission to copy, use,
// modify, sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.
#ifdef _AIX
#include <cstdio>
#include <cstdlib>

extern "C"
{
#include <sys/stat.h>
#include <unistd.h>
}

# include <string>
# include <boost/python/detail/wrap_python.hpp>
# include <boost/python/errors.hpp>
# include <boost/python/detail/aix_init_module.hpp>
# include <boost/python/module.hpp>

namespace boost { namespace python { namespace detail {

namespace
{
  static PyMethodDef initial_methods[] = { { 0, 0, 0, 0 } };
  extern "C" void initlibboost_python()
  {
      Py_InitModule("libboost_python", initial_methods);
  }

  struct find_and_open_file
  {
      FILE*       fp;
      std::string libpath;  // -- search path
      std::string filename; // -- filename to look for
      std::string fullpath; // -- full path to file
   
      find_and_open_file(
          const std::string& libpath_env
          , const std::string& file);
  };

  find_and_open_file::find_and_open_file(
      const std::string& libpath_env
      , const std::string& file)
      : fp(0)
  {
      char* value = std::getenv(libpath_env.c_str());

      if(value == 0)
          return;
      
      libpath = value;
      
      if (libpath == "")
          return;

      std::string::size_type pos = 0, prev_pos = 0;

      // -- loop through all search paths looking for file 
      while((pos = libpath.find_first_of(":",pos)) != std::string::npos)
      {
          fullpath = libpath.substr(prev_pos,pos - prev_pos) + "/" + file;
          if (::access(fullpath.c_str(), R_OK) == 0)
          {
              struct stat filestat;
              ::stat(fullpath.c_str(), &filestat);
              if (!S_ISDIR(filestat.st_mode))
              { 
                  fp = std::fopen(fullpath.c_str(), "r"); 
                  if (fp)
                  {
                      filename = file;
                  }
                  return;
              }
          }
          prev_pos = ++pos;
      }

      // -- mop up odd path
      if (libpath.find_first_of(":", prev_pos) == std::string::npos)
      {
          fullpath = libpath.substr(prev_pos, libpath.size() - prev_pos) + "/" + file;
          if (::access(fullpath.c_str(), R_OK) == 0)
          {
              struct stat filestat;
              ::stat(fullpath.c_str(),&filestat);
              if (!S_ISDIR(filestat.st_mode))
              { 
                  fp = std::fopen(fullpath.c_str(), "r"); 
                  filename = file;
              }
          }
      }
  }
}

void aix_init_module(
    so_load_function load_dynamic_module
    , char const* module_name
    , void (*init_module)())
{
    static bool initialized;
    if (!initialized)
    {
        char const* const name = "libboost_python.so";
        find_and_open_file dynlib("LIBPATH", name);
        if (dynlib.fp == 0)
        {
            fprintf(stderr, " Error: could not find %s\n", name);
            return;
        }
        
        std::string::size_type pos = pos = dynlib.filename.rfind(".so");
        if (pos != dynlib.filename.size() - 3)
        {
            fprintf(stderr, "dynamic library %s must end with .so\n", dynlib.filename.c_str());
            return;
        }

        PyObject* m = 
            load_dynamic_module(
                const_cast<char*>(dynlib.filename.substr(0,pos).c_str()),
                const_cast<char*>(dynlib.fullpath.c_str()),
                dynlib.fp);

        if (m == 0)
        {
            fprintf(stderr, "failed to load library %s\n", name);
            return;
        }
        Py_DECREF(m);

        initialized = true;
    }
    python::detail::init_module(module_name, init_module);
}

}}} // namespace boost::python
#endif
