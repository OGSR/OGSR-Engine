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
  *   FILE     cregex.h
  *   VERSION  see <boost/version.hpp>
  *   deprecated regular expression matching algorithms
  */

#ifndef BOOST_OLD_CREGEX_H
#define BOOST_OLD_CREGEX_H

#include <boost/cregex.hpp>

//
// macro defs:
#define JM_STD std
#define JM_CSTD std
#define JM jm

#ifdef BOOST_RE_OLD_IOSTREAM
#define JM_OLD_IOSTREAM
#endif

#ifdef __cplusplus

namespace jm = boost;

#if !defined(BOOST_RE_NO_USING)
using boost::regex_tA;
using boost::regex_t;
using boost::regoff_t;
using boost::regmatch_t;
using boost::regcompA;
using boost::regerrorA;
using boost::regexecA;
using boost::regfreeA;

#ifndef BOOST_NO_WREGEX
using boost::regex_tW;
using boost::regcompW;
using boost::regerrorW;
using boost::regexecW;
using boost::regfreeW;
#endif

using boost::RegEx;
using boost::GrepCallback;
using boost::GrepFileCallback;
using boost::FindFilesCallback;

using boost::match_flags;
using boost::reg_errcode_t;

using boost::REG_NOERROR;
using boost::REG_NOMATCH;
using boost::REG_BADPAT;
using boost::REG_ECOLLATE;
using boost::REG_ECTYPE;
using boost::REG_EESCAPE;
using boost::REG_ESUBREG;
using boost::REG_EBRACK;
using boost::REG_EPAREN;
using boost::REG_EBRACE;
using boost::REG_BADBR;
using boost::REG_ERANGE;
using boost::REG_ESPACE;
using boost::REG_BADRPT;
using boost::REG_EEND;
using boost::REG_ESIZE;
using boost::REG_ERPAREN;
using boost::REG_EMPTY;
using boost::REG_E_MEMORY;
using boost::REG_E_UNKNOWN;
using boost::match_default;
using boost::match_not_bol;
using boost::match_not_eol;
using boost::match_not_bob;
using boost::match_not_eob;
using boost::match_not_bow;
using boost::match_not_eow;
using boost::match_not_dot_newline;
using boost::match_not_dot_null;
using boost::match_prev_avail;
using boost::match_init;
using boost::match_any;
using boost::match_not_null;
using boost::match_continuous;
using boost::match_stop;

using boost::REG_BASIC;
using boost::REG_EXTENDED;
using boost::REG_ICASE;
using boost::REG_NOSUB;
using boost::REG_NEWLINE;
using boost::REG_NOSPEC;
using boost::REG_PEND;
using boost::REG_DUMP;
using boost::REG_NOCOLLATE;

using boost::REG_ASSERT;
using boost::REG_INVARG;
using boost::REG_ATOI;
using boost::REG_ITOA;

using boost::REG_NOTBOL;
using boost::REG_NOTEOL;
using boost::REG_STARTEND;

#endif // BOOST_RE_NO_USING
#endif // __cplusplus

#ifdef BOOST_RE_USING_HACK
using namespace boost;
#endif

#endif // include guard
