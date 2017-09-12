/*
 *
 * Copyright (c) 1998-2000
 * Dr John Maddock
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Dr John Maddock makes no representations
 * about the suitability of this software for any purpose.  
 * It is provided "as is" without express or implied warranty.
 *
 */
 
 /*
  *   FILE     jgrep.h
  *   VERSION  see <boost/version.hpp>
  */

#ifndef _JGREP_H
#define _JGREP_H

#include <boost/regex.hpp>

// case sensitive reg_expression determines our allocator type:
typedef boost::reg_expression<char> re_type;
typedef re_type::allocator_type allocator_type;

// now declare static (global) data, including an allocator
// instance which we'll pass to all instances that require an allocator.

extern allocator_type a;

extern re_type e;

// flags for output:

extern bool use_case;
extern bool show_lines;
extern bool count_only;
extern bool files_only;
extern bool recurse;
extern bool regularexs;
extern bool words_only;
extern bool verbose;

void process_grep(const char* file);

#endif



