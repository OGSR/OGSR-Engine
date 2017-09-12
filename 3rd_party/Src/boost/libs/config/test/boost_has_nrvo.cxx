//  (C) Copyright John Maddock 2001. Permission to copy, use, modify, sell and
//  distribute this software is granted provided this copyright notice appears
//  in all copies. This software is provided "as is" without express or implied
//  warranty, and with no claim as to its suitability for any purpose.

//  MACRO:         BOOST_HAS_NRVO
//  TITLE:         Named return value optimisation.
//  DESCRIPTION:   Named return value optimisation.


namespace boost_has_nrvo
{

class test_class
{
public:
  test_class() {}
  test_class(const test_class &other)
  {
    ++copy_count;
  }

  static int copy_count;
};

int test_class::copy_count;

test_class f()
{
  test_class nrv;

  return nrv;
}

int test()
{
  test_class::copy_count=0;

  f();

  return test_class::copy_count;
}

} // namespace boost_has_nrvo








