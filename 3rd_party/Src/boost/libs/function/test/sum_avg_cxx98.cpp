// Function library

// Copyright (C) 2001-2003 Douglas Gregor

// Permission to copy, use, sell and distribute this software is granted 
// provided this copyright notice appears in all copies. Permission to modify 
// the code and to distribute modified code is granted provided this copyright 
// notice appears in all copies, and a notice that the code was modified is 
// included with the copyright notice. This software is provided "as is" 
// without express or implied warranty, and with no claim as to its 
// suitability for any purpose. 

// For more information, see http://www.boost.org/

    
#include <boost/function.hpp>
#include <iostream>

void do_sum_avg(int values[], int n, int& sum, float& avg)
{
  sum = 0;
  for (int i = 0; i < n; i++)
    sum += values[i];
  avg = (float)sum / n;
}
int main()
{
    boost::function<void (int values[], int n, int& sum, float& avg)> sum_avg;
    sum_avg = &do_sum_avg;

    return 0;
}
