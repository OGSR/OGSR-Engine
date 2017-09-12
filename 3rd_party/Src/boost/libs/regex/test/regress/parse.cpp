/*
 *
 * Copyright (c) 1998-2002
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
  *
  *   FILE     parse.cpp
  *   VERSION  see <boost/version.hpp>
  *
  * Input parsing functions for regress.
  *
  */

#include <boost/regex.hpp>
#include "regress.h"

#ifndef BOOST_RE_ALGO_INCLUDED
#include <algorithm>
#endif

using namespace boost;

//
// start by defining all our flag types:

flag_info flag_data[] = {
                          { BOOST_RE_STR("REG_BASIC"), 9, REG_BASIC, 0 },
                          { BOOST_RE_STR("REG_EXTENDED"), 12, REG_EXTENDED, 0 },
                          { BOOST_RE_STR("REG_ESCAPE_IN_LISTS"), 19, REG_ESCAPE_IN_LISTS, 0 },
                          { BOOST_RE_STR("REG_ICASE"), 9, REG_ICASE, 0 },
                          { BOOST_RE_STR("REG_NOSUB"), 9, REG_NOSUB, 0 },
                          { BOOST_RE_STR("REG_NEWLINE"), 11, REG_NEWLINE, 0 },
                          { BOOST_RE_STR("REG_NOCOLLATE"), 13, REG_NOCOLLATE, 0 },
                          { BOOST_RE_STR("REG_NOSPEC"), 10, REG_NOSPEC, 0 },
                          { BOOST_RE_STR("REG_NEWLINE_ALT"), 15, REG_NEWLINE_ALT , 0 },
                          { BOOST_RE_STR("REG_PERL"), 8, REG_PERL, 0 },
                          { BOOST_RE_STR("REG_AWK"), 7, REG_AWK, 0 },
                          { BOOST_RE_STR("REG_EGREP"), 9, REG_EGREP, 0 },

                          { BOOST_RE_STR("REG_NOTBOL"), 10, REG_NOTBOL, 1 },
                          { BOOST_RE_STR("REG_NOTEOL"), 10, REG_NOTEOL, 1 },
                          { BOOST_RE_STR("REG_STARTEND"), 12, REG_STARTEND, 1 },

                          { BOOST_RE_STR("basic"), 5, regbase::basic, 2 },
                          { BOOST_RE_STR("escape_in_lists"), 15, regbase::escape_in_lists, 2 },
                          { BOOST_RE_STR("char_classes"), 12, regbase::char_classes, 2 },
                          { BOOST_RE_STR("intervals"), 9, regbase::intervals, 2 },
                          { BOOST_RE_STR("limited_ops"), 11, regbase::limited_ops, 2 },
                          { BOOST_RE_STR("newline_alt"), 11, regbase::newline_alt, 2 },
                          { BOOST_RE_STR("bk_plus_qm"), 10, regbase::bk_plus_qm, 2 },
                          { BOOST_RE_STR("bk_braces"), 9, regbase::bk_braces, 2 },
                          { BOOST_RE_STR("bk_parens"), 9, regbase::bk_parens, 2 },
                          { BOOST_RE_STR("bk_refs"), 7, regbase::bk_refs, 2 },
                          { BOOST_RE_STR("bk_vbar"), 7, regbase::bk_vbar, 2 },
                          { BOOST_RE_STR("use_except"), 10, regbase::use_except, 2 },
                          { BOOST_RE_STR("literal"), 7, regbase::literal, 2 },
                          { BOOST_RE_STR("normal"), 6, regbase::normal, 2 },
                          { BOOST_RE_STR("basic"), 5, regbase::basic, 2 },
                          { BOOST_RE_STR("extended"), 8, regbase::extended, 2 },

                          { BOOST_RE_STR("match_default"), 13, match_default, 3 },
                          { BOOST_RE_STR("match_not_bol"), 13, match_not_bol, 3 },
                          { BOOST_RE_STR("match_not_eol"), 13, match_not_eol, 3 },
                          { BOOST_RE_STR("match_not_bob"), 13, match_not_bob, 3 },
                          { BOOST_RE_STR("match_not_eob"), 13, match_not_eob, 3 },
                          { BOOST_RE_STR("match_not_bow"), 13, match_not_bow, 3 },
                          { BOOST_RE_STR("match_not_eow"), 13, match_not_eow, 3 },
                          { BOOST_RE_STR("match_not_dot_newline"), 21, match_not_dot_newline, 3 },
                          { BOOST_RE_STR("match_not_dot_null"), 18, match_not_dot_null, 3 },
                          { BOOST_RE_STR("match_prev_avail"), 16, match_prev_avail, 3 },
                          { BOOST_RE_STR("match_any"), 9, match_any, 3 },
                          { BOOST_RE_STR("match_not_null"), 14, match_not_null, 3 },
                          { BOOST_RE_STR("match_continuous"), 16, match_continuous, 3 },
                          { BOOST_RE_STR("match_partial"), 13, match_partial, 3 },

                          { BOOST_RE_STR("format_sed"), 10, format_sed, 3 },
                          { BOOST_RE_STR("format_perl"), 11, format_perl, 3 },
                          { BOOST_RE_STR("format_no_copy"), 14, format_no_copy, 3 },
                          { BOOST_RE_STR("format_first_only"), 17, format_first_only, 3 },

                          { BOOST_RE_STR("REG_NO_POSIX_TEST"), 17, REG_NO_POSIX_TEST, 4 },
                          { BOOST_RE_STR("REG_UNICODE_ONLY"), 16, REG_UNICODE_ONLY, 4 },
                          { BOOST_RE_STR("REG_GREP"), 8, REG_GREP, 4 },
                          { BOOST_RE_STR("REG_MERGE"), 9, REG_MERGE, 4 },
                          { BOOST_RE_STR("REG_MERGE_COPY"), 14, REG_MERGE_COPY, 4 },

                          { BOOST_RE_STR(""), 0, 0, 0 },
                        };

// basically we create a simple token parser
// using regular expressions

const char_t* expression_text =        BOOST_RE_STR("(;.*)|")                            // comment
                                       BOOST_RE_STR("(^[[:blank:]]*-)|")                 // -
                                       BOOST_RE_STR("([^\"[:space:]][^[:space:]]*)|")    // token
                                       BOOST_RE_STR("(\"(([^\"]|\\\\\")*)\")")             // "token"   
                                      ;

typedef reg_expression<char_t> re_parse_t;
typedef re_parse_t::allocator_type parse_alloc;
typedef match_results<string_type::const_iterator, parse_alloc>  parse_grep;
typedef string_type::const_iterator parse_iterator;

re_parse_t parse_expression(expression_text, regbase::normal);

//
// now define our grep predicate function object:
class parse_function
{
   int mode;
public:
   parse_function() : mode(0) {}
   parse_function(const parse_function& o) : mode(o.mode) {}
   bool operator()(const parse_grep& i);
};

bool parse_function::operator()(const parse_grep& g)
{
   parse_iterator i, j;
   // determine what caused the match:
   if(g[1].matched)
   {
      // we have a comment:
      return true;
   }
   else if(g[2].matched)
   {
      // we have the start of a line of flags
      mode = -1;
      for(int i = 0; i < 5; ++i)
         flags[i] = 0;
      return true;
   }
   else if(g[3].matched)
   {
      // token:
      i = g[3].first;
      j = g[3].second;
   }
   else
   {
      // token delimited by ""
      i = g[5].first;
      j = g[5].second;
   }
   
   // now we need to switch depending upon what mode we are in:
   switch(mode)
   {
   case -1:
   {
      // parse the flag:
      unsigned int id = 0;
      while(flag_data[id].len != 0)
      {
         if(static_cast<unsigned int>(j - i) != flag_data[id].len)
         {
            ++id;
            continue;
         }
         if(std::equal(i, j, flag_data[id].name) == true)
         {
            flags[flag_data[id].id] |= flag_data[id].value;
            return true;
         }
         ++id;
      }
      cout << "Warning: Unknown flag: ";
      string_type t(i, j);
      cout << make_narrow(t).c_str();
      cout << endl;
      return true;
   }
   case 0:
      // set the expression text:
      expression = string_type(i, j);
      do_test = true;
      break;
   case 1:
      // set the text to match:
      search_text = string_type(i, j);
      jm_trace("Initial search text: " << make_narrow(search_text).c_str());
      expand_escapes(search_text);
      jm_trace("Search text after escapes expanded: " << make_narrow(search_text).c_str());
      break;
   case 2:
      // maybe set format string:
      if(flags[4] & REG_MERGE)
      {
         format_string = string_type(i, j);
         break;
      }
      else
      {
         matches[mode - 2] = to_int(i, j);
         break;
      }
   case 3:
      // maybe set format result:
      if(flags[4] & REG_MERGE)
      {
         merge_string = string_type(i, j);
         expand_escapes(merge_string);
         break;
      }
      else
      {
         matches[mode - 2] = to_int(i, j);
         break;
      }
   default:
      jm_assert(mode >= 2);
      // set the relevent int value:
      matches[mode - 2] = to_int(i, j);
   }
   ++mode;
   return true;
}

void parse_input_line(const string_type& s)
{
   // set matches back to starting values:
   for(int i = 0; i < MAX_MATCHES; ++i)
   {
      matches[i] = -2;
   }
   parse_function op;
   do_test = false;
   regex_grep(op, s.begin(), s.end(), parse_expression);
   jm_trace("expression: " << make_narrow(expression).c_str());
   jm_trace("search string: " << make_narrow(search_text).c_str());
}

int to_int(string_type::const_iterator i, string_type::const_iterator j)
{
   int val = 0;
   bool neg = false;
   if((i != j) && (*i == BOOST_RE_STR('-')))
   {
      neg = true;
      ++i;
   }
   while (i != j)
   {
      val *= 10;
      val += *i - BOOST_RE_STR('0');
      ++i;
   }
   if(neg)
      val *= -1;
   return val;
}

void expand_escapes(string_type& s)
{
   for(unsigned int i = 0; i < s.size(); ++i)
   {
      if(s[i] == BOOST_RE_STR('\\'))
      {
         switch(s[i+1])
         {
         case BOOST_RE_STR('a'):
            s.erase(s.begin() + i);
            s[i] = BOOST_RE_STR('\a');
            break;
         case BOOST_RE_STR('b'):
            s.erase(s.begin() + i);
            s[i] = BOOST_RE_STR('\b');
            break;
         case BOOST_RE_STR('f'):
            s.erase(s.begin() + i);
            s[i] = BOOST_RE_STR('\f');
            break;
         case BOOST_RE_STR('n'):
            s.erase(s.begin() + i);
            s[i] = BOOST_RE_STR('\n');
            break;
         case BOOST_RE_STR('r'):
            s.erase(s.begin() + i);
            s[i] = BOOST_RE_STR('\r');
            break;
         case BOOST_RE_STR('t'):
            s.erase(s.begin() + i);
            s[i] = BOOST_RE_STR('\t');
            break;
         case BOOST_RE_STR('v'):
            s.erase(s.begin() + i);
            s[i] = BOOST_RE_STR('\v');
            break;
         default:
            if( (s[i + 1] >= BOOST_RE_STR('0')) && (s[i + 1] <= BOOST_RE_STR('9')) )
            {
               int val = 0;
               unsigned int pos = i;
               ++i;
               while((i < s.size()) && (s[i] >= BOOST_RE_STR('0')) && (s[i] <= BOOST_RE_STR('9')))
               {
                  val *= 10;
                  val += s[i] - BOOST_RE_STR('0');
                  ++i;
               }
               s.erase(s.begin() + pos, s.begin() + i);
               if(0 == val)
               {
                  s.insert(s.begin()+pos, ' ');
                  s[pos] = 0;
               }
               else
                  s.insert(s.begin() + pos, (string_type::value_type)val);
               i = pos;
            }
            else
            {
               s.erase(s.begin() + i);
            }
         }
      }
   }
}




