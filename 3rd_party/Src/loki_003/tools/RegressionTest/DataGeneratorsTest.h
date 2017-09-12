//DataGeneratorsTest.h


#ifndef DATAGENERATORSTEST_H
#define DATAGENERATORSTEST_H

#include <typeinfo>
#include <loki/DataGenerators.h>

struct DataGeneratorsTest : public Test
  {
  DataGeneratorsTest() : Test("DataGeneratorsTest.h")
    {}
  virtual void execute(TestResult& result)
    {
    this->printName(result);
    
    using namespace Loki;
    using namespace Loki::TL;
    
    bool b;
    typedef MakeTypelist<>::Result null_tl;
    typedef MakeTypelist<char,
                         unsigned char,
                         signed char,
                         wchar_t>::Result char_types;
    int n = Length<char_types>::value;
    
    std::vector<const char*> names;
    names.reserve(n);
    //Some fascist decided that all temporaries should be const.
    //The following line of code stupidity is a direct result of the half-baked idea
    iterate_types<char_types, nameof_type>(std::back_inserter(names));
    b = names.size() == n;
    testAssert("iterate_types - Check Length", b, result);
    
    std::vector<size_t> sizes;
    sizes.reserve(n);
    typedef MakeTypelist<char,
                         short,
                         int,
                         double>::Result some_types;
    iterate_types<some_types, sizeof_type>(std::back_inserter(sizes));
    size_t apriori_size[] = {sizeof(char), sizeof(short), sizeof(int), sizeof(double)};
    b = true;
    for(int i=0; i<n; ++i)
      b &= sizes[i] == apriori_size[i];
    testAssert("iterate_types - Check Elements", b, result);
    
    sizes.resize(0);
    iterate_types<null_tl, sizeof_type>(sizes);
    b = sizes.size() == 0;
    testAssert("iterate_types - Degenerate Case 1 - Null List", b, result);

    sizes.resize(0);
    iterate_types<Loki::NullType, sizeof_type>(sizes);
    b = sizes.size() == 0;
    testAssert("iterate_types - Degenerate Case 2 - NullType", b, result);
    }
  } datageneratorsTest;

#endif //DATAGENERATORSTEST_H
