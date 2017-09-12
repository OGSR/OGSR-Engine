/* boost nondet_random_speed.cpp performance test
 *
 * Copyright Jens Maurer 2000
 * Permission to use, copy, modify, sell, and distribute this software
 * is hereby granted without fee provided that the above copyright notice
 * appears in all copies and that both that copyright notice and this
 * permission notice appear in supporting documentation,
 *
 * Jens Maurer makes no representations about the suitability of this
 * software for any purpose. It is provided "as is" without express or
 * implied warranty.
 *
 * $Id: nondet_random_speed.cpp,v 1.4 2001/11/19 22:13:04 jmaurer Exp $
 *
 */

#include <iostream>
#include <string>
#include <boost/timer.hpp>
#include <boost/nondet_random.hpp>

// set to your CPU frequency in MHz
static const double cpu_frequency = 200 * 1e6;

static void show_elapsed(double end, int iter, const std::string & name)
{
  double usec = end/iter*1e6;
  double cycles = usec * cpu_frequency/1e6;
  std::cout << name << ": " 
            << usec*1e3 << " nsec/loop = " 
            << cycles << " CPU cycles"
            << std::endl;
}

template<class Result, class RNG>
static void timing(RNG & rng, int iter, const std::string& name)
{
  volatile Result tmp; // make sure we're not optimizing too much
  boost::timer t;
  for(int i = 0; i < iter; i++)
    tmp = rng();
  show_elapsed(t.elapsed(), iter, name);
}

template<class RNG>
void run(int iter, const std::string & name)
{
  RNG rng;
  timing<long>(rng, iter, name);
}

int main(int argc, char*argv[])
{
  if(argc != 2) {
    std::cerr << "usage: " << argv[0] << " iterations" << std::endl;
    return 1;
  }

  int iter = std::atoi(argv[1]);

#ifdef __linux__
  boost::random_device dev;
  timing<unsigned int>(dev, iter, "random_device");
#else
#error The non-deterministic random device is currently available on Linux only.
#endif

  return 0;
}
