/* boost histogram.cpp graphical verification of distribution functions
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
 * $Id: histogram.cpp,v 1.6 2002/10/06 23:22:34 jmaurer Exp $
 *
 * This test program allows to visibly examine the results of the
 * distribution functions.
 */

#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <boost/random.hpp>


void plot_histogram(const std::vector<int>& slots, int samples,
                    double from, double to)
{
  int m = *std::max_element(slots.begin(), slots.end());
  const int nRows = 20;
  std::cout.setf(std::ios::fixed|std::ios::left);
  std::cout.precision(5);
  for(int r = 0; r < nRows; r++) {
    double y = ((nRows - r) * double(m))/(nRows * samples);
    std::cout << std::setw(10) << y << "  ";
    for(unsigned int col = 0; col < slots.size(); col++) {
      char out = ' ';
      if(slots[col]/double(samples) >= y)
        out = 'x';
      std::cout << out;
    }
    std::cout << std::endl;
  }
  std::cout << std::setw(12) << " "
            << std::setw(10) << from;
  std::cout.setf(std::ios::right, std::ios::adjustfield);
  std::cout << std::setw(slots.size()-10) << to << std::endl;
}

// I am not sure whether these two should be in the library as well

// maintain sum of NumberGenerator results
template<class NumberGenerator, 
  class Sum = typename NumberGenerator::result_type>
class sum_result
{
public:
  typedef NumberGenerator base_type;
  typedef typename base_type::result_type result_type;
  explicit sum_result(const base_type & g) : gen(g), _sum(0) { }
  result_type operator()() { result_type r = gen(); _sum += r; return r; }
  base_type & base() { return gen; }
  Sum sum() const { return _sum; }
  void reset() { _sum = 0; }
private:
  base_type gen;
  Sum _sum;
};


// maintain square sum of NumberGenerator results
template<class NumberGenerator, 
  class Sum = typename NumberGenerator::result_type>
class squaresum_result
{
public:
  typedef NumberGenerator base_type;
  typedef typename base_type::result_type result_type;
  explicit squaresum_result(const base_type & g) : gen(g), _sum(0) { }
  result_type operator()() { result_type r = gen(); _sum += r*r; return r; }
  base_type & base() { return gen; }
  Sum squaresum() const { return _sum; }
  void reset() { _sum = 0; }
private:
  base_type gen;
  Sum _sum;
};


template<class RNG>
void histogram(RNG base, int samples, double from, double to, 
               const std::string & name)
{
  typedef squaresum_result<sum_result<RNG, double>, double > SRNG;
  SRNG gen((sum_result<RNG, double>(base)));
  const int nSlots = 60;
  std::vector<int> slots(nSlots,0);
  for(int i = 0; i < samples; i++) {
    double val = gen();
    if(val < from || val >= to)    // early check avoids overflow
      continue;
    int slot = int((val-from)/(to-from) * nSlots);
    if(slot < 0 || slot > (int)slots.size())
      continue;
    slots[slot]++;
  }
  std::cout << name << std::endl;
  plot_histogram(slots, samples, from, to);
  double mean = gen.base().sum() / samples;
  std::cout << "mean: " << mean
            << " sigma: " << std::sqrt(gen.squaresum()/samples-mean*mean)
            << "\n" << std::endl;
}


template<class PRNG>
void histograms()
{
  PRNG rng;
  using namespace boost;
  histogram(uniform_smallint<PRNG>(rng, 0, 5), 100000, -1, 6, "uniform_smallint(0,5)");
  histogram(uniform_int<PRNG>(rng, 0, 5), 100000, -1, 6, "uniform_int(0,5)");
  histogram(uniform_01<PRNG>(rng), 100000, -0.5, 1.5, "uniform_01(0,1)");
  histogram(bernoulli_distribution<PRNG>(rng, 0.2), 100000, -0.5, 1.5,
            "bernoulli(0.2)");
  histogram(binomial_distribution<PRNG>(rng, 4, 0.2), 100000, -1, 5,
            "binomial(4, 0.2)");
  histogram(triangle_distribution<PRNG>(rng, 1, 2, 8), 100000, 0, 10,
            "triangle(1,2,8)");
  histogram(geometric_distribution<PRNG>(rng, 5.0/6.0), 100000, 0, 10,
            "geometric(5/6)");
  histogram(exponential_distribution<PRNG>(rng, 0.3), 100000, 0, 10,
            "exponential(0.3)");
  histogram(cauchy_distribution<PRNG>(rng), 100000, -5, 5,
            "cauchy");
  histogram(lognormal_distribution<PRNG>(rng, 3, 2), 100000, 0, 10,
            "lognormal");
  histogram(normal_distribution<PRNG>(rng), 100000, -3, 3,
            "normal");
  histogram(normal_distribution<PRNG>(rng, 0.5, 0.5), 100000, -3, 3,
            "normal(0.5, 0.5)");
  histogram(poisson_distribution<PRNG>(rng, 1.5), 100000, 0, 5,
            "poisson(1.5)");
  histogram(poisson_distribution<PRNG>(rng, 10), 100000, 0, 20,
            "poisson(10)");
  histogram(gamma_distribution<PRNG>(rng, 0.5), 100000, 0, 0.5,
            "gamma(0.5)");
  histogram(gamma_distribution<PRNG>(rng, 1), 100000, 0, 3,
            "gamma(1)");
  histogram(gamma_distribution<PRNG>(rng, 2), 100000, 0, 6,
            "gamma(2)");
}


int main()
{
  histograms<boost::mt19937>();
  // histograms<boost::lagged_fibonacci607>();
}

