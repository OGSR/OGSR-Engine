// (C) Copyright Jeremy Siek 2000. Permission to copy, use, modify,
// sell and distribute this software is granted provided this
// copyright notice appears in all copies. This software is provided
// "as is" without express or implied warranty, and with no claim as
// to its suitability for any purpose.

#include <list>
#include <algorithm>

int main() {
  std::list<int> v;
  std::stable_sort(v.begin(), v.end());
  return 0;
}
