/* boost random_speed.cpp performance measurements
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
 * $Id: random_speed.cpp,v 1.10 2002/10/16 12:50:21 jmaurer Exp $
 */

#include <iostream>
#include <cstdlib>
#include <string>
#include <boost/random.hpp>
#include <boost/progress.hpp>
#include <boost/shared_ptr.hpp>

/*
 * Configuration Section
 */

// define if your C library supports the non-standard drand48 family
#undef HAVE_DRAND48

// define if you have the original mt19937int.c (with commented out main())
#undef HAVE_MT19937INT_C

// set to your CPU frequency in MHz
static const double cpu_frequency = 200 * 1e6;

/*
 * End of Configuration Section
 */

/*
 * General portability note:
 * MSVC mis-compiles explicit function template instantiations.
 * For example, f<A>() and f<B>() are both compiled to call f<A>().
 * BCC is unable to implicitly convert a "const char *" to a std::string
 * when using explicit function template instantiations.
 *
 * Therefore, avoid explicit function template instantiations.
 */

// provides a run-time configurable linear congruential generator, just
// for comparison
template<class IntType>
class linear_congruential
{
public:
  typedef IntType result_type;

  BOOST_STATIC_CONSTANT(bool, has_fixed_range = false);

  linear_congruential(IntType x0, IntType a, IntType c, IntType m)
    : _x(x0), _a(a), _c(c), _m(m) { }
  // compiler-generated copy ctor and assignment operator are fine
  void seed(IntType x0, IntType a, IntType c, IntType m)
    { _x = x0; _a = a; _c = c; _m = m; }
  void seed(IntType x0) { _x = x0; }
  result_type operator()() { _x = (_a*_x+_c) % _m; return _x; }
  result_type min() const { return _c == 0 ? 1 : 0; }
  result_type max() const { return _m -1; }

private:
  IntType _x, _a, _c, _m;
};


// simplest "random" number generator possible, to check on overhead
class counting
{
public:
  typedef int result_type;

  BOOST_STATIC_CONSTANT(bool, has_fixed_range = false);

  counting() : _x(0) { }
  result_type operator()() { return ++_x; }
  result_type min() const { return 1; }
  result_type max() const { return std::numeric_limits<result_type>::max(); }

private:
  int _x;
};


// make distributions (and underlying engines) runtime-exchangeable,
// for speed comparison
template<class Ret>
class DistributionBase
{
public:
  virtual void reset() = 0;
  virtual Ret operator()() = 0;
  virtual ~DistributionBase() { }
};

template<class Dist, class Ret = typename Dist::result_type>
class Distribution
  : public DistributionBase<Ret>
{
public:
  Distribution(const Dist& d) : _d(d) { }
  void reset() { _d.reset(); }
  Ret operator()() { return _d(); }
private:
  Dist _d;
};

template<class Ret>
class GenericDistribution
{
public:
  typedef Ret result_type;

  GenericDistribution() { };
  void set(boost::shared_ptr<DistributionBase<Ret> > p) { _p = p; }
  // takes over ownership
  void set(DistributionBase<Ret> * p) { _p.reset(p); }
  Ret operator()() { return (*_p)(); }
private:
  boost::shared_ptr<DistributionBase<Ret> > _p;
};


// start implementation of measuring timing

void show_elapsed(double end, int iter, const std::string & name)
{
  double usec = end/iter*1e6;
  double cycles = usec * cpu_frequency/1e6;
  std::cout << name << ": " 
            << usec*1e3 << " nsec/loop = " 
            << cycles << " CPU cycles"
            << std::endl;
}

template<class RNG>
void timing(RNG & rng, int iter, const std::string& name)
{
  // make sure we're not optimizing too much
  volatile typename RNG::result_type tmp;
  boost::timer t;
  for(int i = 0; i < iter; i++)
    tmp = rng();
  show_elapsed(t.elapsed(), iter, name);
}

template<class RNG>
void timing_sphere(RNG & rng, int iter, const std::string & name)
{
  boost::timer t;
  for(int i = 0; i < iter; i++) {
    // the special return value convention of uniform_on_sphere saves 20% CPU
    const std::vector<double> & tmp = rng();
    (void) tmp[0];
  }
  show_elapsed(t.elapsed(), iter, name);
}

template<class RNG>
void run(int iter, const std::string & name, const RNG &)
{
  RNG rng;
  std::cout << (RNG::has_fixed_range ? "fixed-range " : "");
  // BCC has trouble with string autoconversion for explicit specializations
  timing(rng, iter, std::string(name));
}

#ifdef HAVE_DRAND48
// requires non-standard C library support for srand48/lrand48
void run(int iter, const std::string & name, int)
{
  std::srand48(1);
  timing(std::lrand48, iter, name);
}
#endif

#ifdef HAVE_MT19937INT_C  // requires the original mt19937int.c
extern "C" void sgenrand(unsigned long);
extern "C" unsigned long genrand();

void run(int iter, const std::string & name, float)
{
  sgenrand(4357);
  timing(genrand, iter, name, 0u);
}
#endif

template<class Gen>
void distrib(int iter, const std::string & name, const Gen &)
{
  Gen gen;

  boost::uniform_smallint<Gen> usmallint(gen, -2, 4);
  timing(usmallint, iter, name + " uniform_smallint");

  boost::uniform_int<Gen> uint(gen, -2, 4);
  timing(uint, iter, name + " uniform_int");

  boost::geometric_distribution<Gen> geo(gen, 0.5);
  timing(geo, iter, name + " geometric");


  boost::uniform_01<Gen> uni(gen);
  timing(uni, iter, name + " uniform_01");

  boost::uniform_real<Gen> ur(gen, -5.3, 4.8);
  timing(ur, iter, name + " uniform_real");

  boost::triangle_distribution<Gen> tria(gen, 1, 2, 7);
  timing(tria, iter, name + " triangle");

  boost::exponential_distribution<Gen> ex(gen, 3);
  timing(ex, iter, name + " exponential");

  boost::normal_distribution<Gen,double> no2(gen);
  timing(no2, iter, name + " normal polar");

  boost::lognormal_distribution<Gen,double> lnorm(gen, 1, 1);
  timing(lnorm, iter, name + " lognormal");

  boost::cauchy_distribution<Gen,double> cauchy(gen);
  timing(cauchy, iter, name + " cauchy");

  boost::gamma_distribution<Gen,double> gamma(gen, 0.4);
  timing(gamma, iter, name + " gamma");

  boost::uniform_on_sphere<Gen> usph(gen, 3);
  timing_sphere(usph, iter/10, name + " uniform_on_sphere");
}

template<class Gen>
void distrib_runtime(int iter, const std::string & n, const Gen &)
{
  std::string name = n + " virtual function ";
  Gen gen;

  GenericDistribution<int> g_int;

  boost::uniform_smallint<Gen> usmallint(gen, -2, 4);
  g_int.set(new Distribution<boost::uniform_smallint<Gen> >(usmallint));
  timing(g_int, iter, name + "uniform_smallint");

  boost::uniform_int<Gen> uint(gen, -2, 4);
  g_int.set(new Distribution<boost::uniform_int<Gen> >(uint));
  timing(g_int, iter, name + "uniform_int");

  boost::geometric_distribution<Gen> geo(gen, 0.5);
  g_int.set(new Distribution<boost::geometric_distribution<Gen> >(geo));
  timing(g_int, iter, name + "geometric");

  GenericDistribution<double> g;

  boost::uniform_01<Gen> uni(gen);
  g.set(new Distribution<boost::uniform_01<Gen> >(uni));
  timing(g, iter, name + "uniform_01");

  boost::uniform_real<Gen> ur(gen, -5.3, 4.8);
  g.set(new Distribution<boost::uniform_real<Gen> >(ur));
  timing(g, iter, name + "uniform_real");

  boost::triangle_distribution<Gen> tria(gen, 1, 2, 7);
  g.set(new Distribution<boost::triangle_distribution<Gen> >(tria));
  timing(g, iter, name + "triangle");

  boost::exponential_distribution<Gen> ex(gen, 3);
  g.set(new Distribution<boost::exponential_distribution<Gen> >(ex));
  timing(g, iter, name + "exponential");

  boost::normal_distribution<Gen,double> no2(gen);
  g.set(new Distribution<boost::normal_distribution<Gen,double> >(no2));
  timing(g, iter, name + "normal polar");

  boost::lognormal_distribution<Gen,double> lnorm(gen, 1, 1);
  g.set(new Distribution<boost::lognormal_distribution<Gen,double> >(lnorm));
  timing(g, iter, name + "lognormal");

  boost::cauchy_distribution<Gen,double> cauchy(gen);
  g.set(new Distribution<boost::cauchy_distribution<Gen,double> >(cauchy));
  timing(g, iter, name + "cauchy");

  boost::gamma_distribution<Gen,double> gamma(gen, 0.4);
  g.set(new Distribution<boost::gamma_distribution<Gen,double> >(gamma));
  timing(g, iter, name + "gamma");
}


int main(int argc, char*argv[])
{
  if(argc != 2) {
    std::cerr << "usage: " << argv[0] << " iterations" << std::endl;
    return 1;
  }

  // okay, it's ugly, but it's only used here
  int iter =
#ifndef BOOST_NO_STDC_NAMESPACE
    std::
#endif
    atoi(argv[1]);

#if !defined(BOOST_NO_INT64_T) && \
    !defined(BOOST_NO_INCLASS_MEMBER_INITIALIZATION)
  run(iter, "rand48", boost::rand48());
  linear_congruential<boost::uint64_t>
    lcg48(boost::uint64_t(1)<<16 | 0x330e,
          boost::uint64_t(0xDEECE66DUL) | (boost::uint64_t(0x5) << 32), 0xB,
          boost::uint64_t(1)<<48);
  timing(lcg48, iter, "lrand48 run-time");
#endif

#ifdef HAVE_DRAND48
  // requires non-standard C library support for srand48/lrand48
  run(iter, "lrand48", 0);   // coded for lrand48()
#endif

  run(iter, "minstd_rand", boost::minstd_rand());
  run(iter, "ecuyer combined", boost::ecuyer1988());
  run(iter, "kreutzer1986", boost::kreutzer1986());

  run(iter, "hellekalek1995 (inversive)", boost::hellekalek1995());

  run(iter, "mt11213b", boost::mt11213b());
  run(iter, "mt19937", boost::mt19937());

  run(iter, "subtract_with_carry", boost::random::ranlux_base());
  run(iter, "subtract_with_carry_01", boost::random::ranlux_base_01());
  run(iter, "ranlux3", boost::ranlux3());
  run(iter, "ranlux4", boost::ranlux4());
  run(iter, "ranlux3_01", boost::ranlux3_01());
  run(iter, "ranlux4_01", boost::ranlux4_01());
  run(iter, "counting", counting());

#ifdef HAVE_MT19937INT_C
  // requires the original mt19937int.c
  run<float>(iter, "mt19937 original");   // coded for sgenrand()/genrand()
#endif

  distrib(iter, "counting", counting());
  distrib_runtime(iter, "counting", counting());

  distrib(iter, "minstd_rand", boost::minstd_rand());

  distrib(iter, "kreutzer1986", boost::kreutzer1986());

  distrib(iter, "mt19937", boost::mt19937());
  distrib_runtime(iter, "mt19937", boost::mt19937());
}
