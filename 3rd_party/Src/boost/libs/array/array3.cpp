/* example for using class array<>
 * (C) Copyright Nicolai M. Josuttis 2001.
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 */

#include <string>
#include <iostream>
#include <boost/array.hpp>

template <class T>
void print_elements (const T& x);

int main()
{
    // create array of four seasons
    boost::array<std::string,4> seasons = {
        { "spring", "summer", "autumn", "winter" }
    };

    // copy and change order
    boost::array<std::string,4> seasons_orig = seasons;
    for (unsigned i=seasons.size()-1; i>0; --i) {
        std::swap(seasons.at(i),seasons.at((i+1)%seasons.size()));
    }

    std::cout << "one way:   ";
    print_elements(seasons);

    // try swap()
    std::cout << "other way: ";
    std::swap(seasons,seasons_orig);
    print_elements(seasons);

    // try reverse iterators
    std::cout << "reverse:   ";
    for (boost::array<std::string,4>::reverse_iterator pos
           =seasons.rbegin(); pos<seasons.rend(); ++pos) {
        std::cout << " " << *pos;
    }
    std::cout << std::endl;

    return 0;  // makes Visual-C++ compiler happy
}

template <class T>
void print_elements (const T& x)
{
    for (unsigned i=0; i<x.size(); ++i) {
        std::cout << " " << x[i];
    }
    std::cout << std::endl;
}

