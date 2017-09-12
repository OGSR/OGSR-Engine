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
  *   LOCATION:    see http://www.boost.org for most recent version.
  *   FILE:        c_regex_traits_common.cpp
  *   VERSION:     see <boost/version.hpp>
  *   DESCRIPTION: Implements common code and data for the
  *                c_regex_traits<charT> traits classes.
  */


#define BOOST_REGEX_SOURCE

#include <clocale>
#include <cstdio>
#include <list>
#include <cctype>
#include <boost/regex/regex_traits.hpp>
#include <boost/regex/v3/regex_synch.hpp>


namespace boost{
   namespace re_detail{

//
// these are the POSIX collating names:
//
BOOST_REGEX_DECL const char* def_coll_names[] = {
"NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "alert", "backspace", "tab", "newline", 
"vertical-tab", "form-feed", "carriage-return", "SO", "SI", "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", 
"SYN", "ETB", "CAN", "EM", "SUB", "ESC", "IS4", "IS3", "IS2", "IS1", "space", "exclamation-mark", 
"quotation-mark", "number-sign", "dollar-sign", "percent-sign", "ampersand", "apostrophe", 
"left-parenthesis", "right-parenthesis", "asterisk", "plus-sign", "comma", "hyphen", 
"period", "slash", "zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", 
"colon", "semicolon", "less-than-sign", "equals-sign", "greater-than-sign", 
"question-mark", "commercial-at", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", 
"Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "left-square-bracket", "backslash", 
"right-square-bracket", "circumflex", "underscore", "grave-accent", "a", "b", "c", "d", "e", "f", 
"g", "h", "i", "j", "k", "l", "m", "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", "left-curly-bracket", 
"vertical-line", "right-curly-bracket", "tilde", "DEL", "", 
};

// these multi-character collating elements
// should keep most Western-European locales
// happy - we should really localise these a
// little more - but this will have to do for
// now:

BOOST_REGEX_DECL const char* def_multi_coll[] = {
   "ae",
   "Ae",
   "AE",
   "ch",
   "Ch",
   "CH",
   "ll",
   "Ll",
   "LL",
   "ss",
   "Ss",
   "SS",
   "nj",
   "Nj",
   "NJ",
   "dz",
   "Dz",
   "DZ",
   "lj",
   "Lj",
   "LJ",
   "",
};



BOOST_REGEX_DECL bool BOOST_REGEX_CALL re_lookup_def_collate_name(std::string& buf, const char* name)
{
   BOOST_RE_GUARD_STACK
   unsigned int i = 0;
   while(*def_coll_names[i])
   {
      if(std::strcmp(def_coll_names[i], name) == 0)
      {
         buf = (char)i;
         return true;
      }
      ++i;
   }
   i = 0;
   while(*def_multi_coll[i])
   {
      if(std::strcmp(def_multi_coll[i], name) == 0)
      {
         buf = def_multi_coll[i];
         return true;
      }
      ++i;
   }
   return false;
}

//
// messages:
BOOST_REGEX_DECL const char * re_default_error_messages[] =
{  "Success",             /* REG_NOERROR */
   "No match",             /* REG_NOMATCH */
   "Invalid regular expression",    /* REG_BADPAT */
   "Invalid collation character",      /* REG_ECOLLATE */
   "Invalid character class name",     /* REG_ECTYPE */
   "Trailing backslash",         /* REG_EESCAPE */
   "Invalid back reference",        /* REG_ESUBREG */
   "Unmatched [ or [^",       /* REG_EBRACK */
   "Unmatched ( or \\(",         /* REG_EPAREN */
   "Unmatched \\{",           /* REG_EBRACE */
   "Invalid content of \\{\\}",     /* REG_BADBR */
   "Invalid range end",       /* REG_ERANGE */
   "Memory exhausted",           /* REG_ESPACE */
   "Invalid preceding regular expression",   /* REG_BADRPT */
   "Premature end of regular expression", /* REG_EEND */
   "Regular expression too big",    /* REG_ESIZE */
   "Unmatched ) or \\)",         /* REG_ERPAREN */
   "Empty expression",           /* REG_EMPTY */
   "Unknown error",    /* REG_E_UNKNOWN */
   "",
   "",
   "",
};

const mss default_messages[] = {
                              { 100+ c_regex_traits<char>::syntax_open_bracket, "(", },
                              { 100+ c_regex_traits<char>::syntax_close_bracket, ")", },
                              { 100+ c_regex_traits<char>::syntax_dollar, "$", },
                              { 100+ c_regex_traits<char>::syntax_caret, "^", },
                              { 100+ c_regex_traits<char>::syntax_dot, ".", },
                              { 100+ c_regex_traits<char>::syntax_star, "*", },
                              { 100+ c_regex_traits<char>::syntax_plus, "+", },
                              { 100+ c_regex_traits<char>::syntax_question, "?", },
                              { 100+ c_regex_traits<char>::syntax_open_set, "[", },
                              { 100+ c_regex_traits<char>::syntax_close_set, "]", },
                              { 100+ c_regex_traits<char>::syntax_or, "|", },
                              { 100+ c_regex_traits<char>::syntax_slash, "\\", },
                              { 100+ c_regex_traits<char>::syntax_hash, "#", },
                              { 100+ c_regex_traits<char>::syntax_dash, "-", },
                              { 100+ c_regex_traits<char>::syntax_open_brace, "{", },
                              { 100+ c_regex_traits<char>::syntax_close_brace, "}", },
                              { 100+ c_regex_traits<char>::syntax_digit, "0123456789", },
                              { 100+ c_regex_traits<char>::syntax_b, "b", },
                              { 100+ c_regex_traits<char>::syntax_B, "B", },
                              { 100+ c_regex_traits<char>::syntax_left_word, "<", },
                              { 100+ c_regex_traits<char>::syntax_right_word, ">", },
                              { 100+ c_regex_traits<char>::syntax_w, "w", },
                              { 100+ c_regex_traits<char>::syntax_W, "W", },
                              { 100+ c_regex_traits<char>::syntax_start_buffer, "`A", },
                              { 100+ c_regex_traits<char>::syntax_end_buffer, "'z", },
                              { 100+ c_regex_traits<char>::syntax_newline, "\n", },
                              { 100+ c_regex_traits<char>::syntax_comma, ",", },
                              { 100+ c_regex_traits<char>::syntax_a, "a", },
                              { 100+ c_regex_traits<char>::syntax_f, "f", },
                              { 100+ c_regex_traits<char>::syntax_n, "n", },
                              { 100+ c_regex_traits<char>::syntax_r, "r", },
                              { 100+ c_regex_traits<char>::syntax_t, "t", },
                              { 100+ c_regex_traits<char>::syntax_v, "v", },
                              { 100+ c_regex_traits<char>::syntax_x, "x", },
                              { 100+ c_regex_traits<char>::syntax_c, "c", },
                              { 100+ c_regex_traits<char>::syntax_colon, ":", },
                              { 100+ c_regex_traits<char>::syntax_equal, "=", },

                              { 100 + c_regex_traits<char>::syntax_e, "e", },
                              { 100 + c_regex_traits<char>::syntax_l, "l", },
                              { 100 + c_regex_traits<char>::syntax_L, "L", },
                              { 100 + c_regex_traits<char>::syntax_u, "u", },
                              { 100 + c_regex_traits<char>::syntax_U, "U", },
                              { 100 + c_regex_traits<char>::syntax_s, "s", },
                              { 100 + c_regex_traits<char>::syntax_S, "S", },
                              { 100 + c_regex_traits<char>::syntax_d, "d", },
                              { 100 + c_regex_traits<char>::syntax_D, "D", },
                              { 100 + c_regex_traits<char>::syntax_E, "E", },
                              { 100 + c_regex_traits<char>::syntax_Q, "Q", },
                              { 100 + c_regex_traits<char>::syntax_X, "X", },
                              { 100 + c_regex_traits<char>::syntax_C, "C", },
                              { 100 + c_regex_traits<char>::syntax_Z, "Z", },
                              { 100 + c_regex_traits<char>::syntax_G, "G", },

                              { 100 + c_regex_traits<char>::syntax_not, "!", },

                            { 0, "", },
                         };

BOOST_REGEX_DECL std::size_t BOOST_REGEX_CALL re_get_default_message(char* buf, std::size_t len, std::size_t id)
{
   BOOST_RE_GUARD_STACK
   const mss* pm = default_messages;
   while(pm->id)
   {
      if(pm->id == id)
      {
         std::size_t size = re_strlen(pm->what) + 1;
         if(size > len)
            return size;
         re_strcpy(buf, pm->what);
         return size;
      }
      ++pm;
   }
   if(buf && len)
      *buf = 0;
   return 1;
}

#ifndef BOOST_NO_WREGEX
const wchar_t combining_ranges[] = { 0x0300, 0x0361, 
                           0x0483, 0x0486, 
                           0x0903, 0x0903, 
                           0x093E, 0x0940, 
                           0x0949, 0x094C,
                           0x0982, 0x0983,
                           0x09BE, 0x09C0,
                           0x09C7, 0x09CC,
                           0x09D7, 0x09D7,
                           0x0A3E, 0x0A40,
                           0x0A83, 0x0A83,
                           0x0ABE, 0x0AC0,
                           0x0AC9, 0x0ACC,
                           0x0B02, 0x0B03,
                           0x0B3E, 0x0B3E,
                           0x0B40, 0x0B40,
                           0x0B47, 0x0B4C,
                           0x0B57, 0x0B57,
                           0x0B83, 0x0B83,
                           0x0BBE, 0x0BBF,
                           0x0BC1, 0x0BCC,
                           0x0BD7, 0x0BD7,
                           0x0C01, 0x0C03,
                           0x0C41, 0x0C44,
                           0x0C82, 0x0C83,
                           0x0CBE, 0x0CBE,
                           0x0CC0, 0x0CC4,
                           0x0CC7, 0x0CCB,
                           0x0CD5, 0x0CD6,
                           0x0D02, 0x0D03,
                           0x0D3E, 0x0D40,
                           0x0D46, 0x0D4C,
                           0x0D57, 0x0D57,
                           0x0F7F, 0x0F7F,
                           0x20D0, 0x20E1, 
                           0x3099, 0x309A,
                           0xFE20, 0xFE23, 
                           0xffff, 0xffff, };

BOOST_REGEX_DECL bool BOOST_REGEX_CALL is_combining(wchar_t c)
{
   BOOST_RE_GUARD_STACK
   const wchar_t* p = combining_ranges + 1;
   while(*p < c) p += 2;
   --p;
   if((c >= *p) && (c <= *(p+1)))
         return true;
   return false;
}

BOOST_REGEX_DECL unsigned short wide_unicode_classes[] = {
   c_traits_base::char_class_cntrl,        // ''  0
   c_traits_base::char_class_cntrl,        // ''  1
   c_traits_base::char_class_cntrl,        // ''  2
   c_traits_base::char_class_cntrl,        // ''  3
   c_traits_base::char_class_cntrl,        // ''  4
   c_traits_base::char_class_cntrl,        // ''  5
   c_traits_base::char_class_cntrl,        // ''  6
   c_traits_base::char_class_cntrl,        // ''  7
   c_traits_base::char_class_cntrl,        // ''  8
   c_traits_base::char_class_cntrl | c_traits_base::char_class_space | c_traits_base::char_class_blank,        // ''  9
   c_traits_base::char_class_cntrl | c_traits_base::char_class_space,        // ''  10
   c_traits_base::char_class_cntrl | c_traits_base::char_class_space,        // ''  11
   c_traits_base::char_class_cntrl | c_traits_base::char_class_space,        // ''  12
   c_traits_base::char_class_cntrl | c_traits_base::char_class_space,        // ''  13
   c_traits_base::char_class_cntrl,        // '.'  14
   c_traits_base::char_class_cntrl,        // '.'  15
   c_traits_base::char_class_cntrl,        // '.'  16
   c_traits_base::char_class_cntrl,        // '.'  17
   c_traits_base::char_class_cntrl,        // '.'  18
   c_traits_base::char_class_cntrl,        // '.'  19
   c_traits_base::char_class_cntrl,        // '.'  20
   c_traits_base::char_class_cntrl,        // '.'  21
   c_traits_base::char_class_cntrl,        // '.'  22
   c_traits_base::char_class_cntrl,        // '.'  23
   c_traits_base::char_class_cntrl,        // '.'  24
   c_traits_base::char_class_cntrl,        // ''  25
   c_traits_base::char_class_cntrl,        // ''  26
   c_traits_base::char_class_cntrl,        // ''  27
   c_traits_base::char_class_cntrl,        // '.'  28
   c_traits_base::char_class_cntrl,        // '.'  29
   c_traits_base::char_class_cntrl,        // '.'  30
   c_traits_base::char_class_cntrl,        // '.'  31
   c_traits_base::char_class_space | c_traits_base::char_class_blank,        // ' '  32
   c_traits_base::char_class_punct,        // '!'  33
   c_traits_base::char_class_punct,        // '"'  34
   c_traits_base::char_class_punct,        // '#'  35
   c_traits_base::char_class_punct,        // '$'  36
   c_traits_base::char_class_punct,        // '%'  37
   c_traits_base::char_class_punct,        // '&'  38
   c_traits_base::char_class_punct,        // '''  39
   c_traits_base::char_class_punct,        // '('  40
   c_traits_base::char_class_punct,        // ')'  41
   c_traits_base::char_class_punct,        // '*'  42
   c_traits_base::char_class_punct,        // '+'  43
   c_traits_base::char_class_punct,        // ','  44
   c_traits_base::char_class_punct,        // '-'  45
   c_traits_base::char_class_punct,        // '.'  46
   c_traits_base::char_class_punct,        // '/'  47
   c_traits_base::char_class_digit |   c_traits_base::char_class_xdigit,        // '0'  48
   c_traits_base::char_class_digit |   c_traits_base::char_class_xdigit,        // '1'  49
   c_traits_base::char_class_digit |   c_traits_base::char_class_xdigit,        // '2'  50
   c_traits_base::char_class_digit |   c_traits_base::char_class_xdigit,        // '3'  51
   c_traits_base::char_class_digit |   c_traits_base::char_class_xdigit,        // '4'  52
   c_traits_base::char_class_digit |   c_traits_base::char_class_xdigit,        // '5'  53
   c_traits_base::char_class_digit |   c_traits_base::char_class_xdigit,        // '6'  54
   c_traits_base::char_class_digit |   c_traits_base::char_class_xdigit,        // '7'  55
   c_traits_base::char_class_digit |   c_traits_base::char_class_xdigit,        // '8'  56
   c_traits_base::char_class_digit |   c_traits_base::char_class_xdigit,        // '9'  57
   c_traits_base::char_class_punct,        // ':'  58
   c_traits_base::char_class_punct,        // ';'  59
   c_traits_base::char_class_punct,        // '<'  60
   c_traits_base::char_class_punct,        // '='  61
   c_traits_base::char_class_punct,        // '>'  62
   c_traits_base::char_class_punct,        // '?'  63
   c_traits_base::char_class_punct,        // '@'  64
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper | c_traits_base::char_class_xdigit,        // 'A'  65
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper | c_traits_base::char_class_xdigit,        // 'B'  66
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper | c_traits_base::char_class_xdigit,        // 'C'  67
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper | c_traits_base::char_class_xdigit,        // 'D'  68
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper | c_traits_base::char_class_xdigit,        // 'E'  69
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper | c_traits_base::char_class_xdigit,        // 'F'  70
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'G'  71
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'H'  72
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'I'  73
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'J'  74
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'K'  75
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'L'  76
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'M'  77
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'N'  78
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'O'  79
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'P'  80
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'Q'  81
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'R'  82
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'S'  83
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'T'  84
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'U'  85
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'V'  86
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'W'  87
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'X'  88
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'Y'  89
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'Z'  90
   c_traits_base::char_class_punct,        // '['  91
   c_traits_base::char_class_punct,        // '\'  92
   c_traits_base::char_class_punct,        // ']'  93
   c_traits_base::char_class_punct,        // '^'  94
   c_traits_base::char_class_punct | c_traits_base::char_class_underscore,        // '_'  95
   c_traits_base::char_class_punct,        // '`'  96
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower |  c_traits_base::char_class_xdigit,        // 'a'  97
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower |  c_traits_base::char_class_xdigit,        // 'b'  98
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower |  c_traits_base::char_class_xdigit,        // 'c'  99
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower |  c_traits_base::char_class_xdigit,        // 'd'  100
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower |  c_traits_base::char_class_xdigit,        // 'e'  101
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower |  c_traits_base::char_class_xdigit,        // 'f'  102
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'g'  103
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'h'  104
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'i'  105
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'j'  106
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'k'  107
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'l'  108
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'm'  109
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'n'  110
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'o'  111
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'p'  112
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'q'  113
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'r'  114
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 's'  115
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 't'  116
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'u'  117
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'v'  118
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'w'  119
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'x'  120
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'y'  121
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'z'  122
   c_traits_base::char_class_punct,        // '{'  123
   c_traits_base::char_class_punct,        // '|'  124
   c_traits_base::char_class_punct,        // '}'  125
   c_traits_base::char_class_punct,        // '~'  126

   c_traits_base::char_class_cntrl,        // ''  127
   c_traits_base::char_class_cntrl,        // 'Ä'  128
   c_traits_base::char_class_cntrl,        // 'Å'  129
   c_traits_base::char_class_cntrl,        // 'Ç'  130
   c_traits_base::char_class_cntrl,        // 'É'  131
   c_traits_base::char_class_cntrl,        // 'Ñ'  132
   c_traits_base::char_class_cntrl,        // 'Ö'  133
   c_traits_base::char_class_cntrl,        // 'Ü'  134
   c_traits_base::char_class_cntrl,        // 'á'  135
   c_traits_base::char_class_cntrl,        // 'à'  136
   c_traits_base::char_class_cntrl,        // 'â'  137
   c_traits_base::char_class_cntrl,        // 'ä'  138
   c_traits_base::char_class_cntrl,        // 'ã'  139
   c_traits_base::char_class_cntrl,        // 'å'  140
   c_traits_base::char_class_cntrl,        // 'ç'  141
   c_traits_base::char_class_cntrl,        // 'é'  142
   c_traits_base::char_class_cntrl,        // 'è'  143
   c_traits_base::char_class_cntrl,        // 'ê'  144
   c_traits_base::char_class_cntrl,        // 'ë'  145
   c_traits_base::char_class_cntrl,        // 'í'  146
   c_traits_base::char_class_cntrl,        // 'ì'  147
   c_traits_base::char_class_cntrl,        // 'î'  148
   c_traits_base::char_class_cntrl,        // 'ï'  149
   c_traits_base::char_class_cntrl,        // 'ñ'  150
   c_traits_base::char_class_cntrl,        // 'ó'  151
   c_traits_base::char_class_cntrl,        // 'ò'  152
   c_traits_base::char_class_cntrl,        // 'ô'  153
   c_traits_base::char_class_cntrl,        // 'ö'  154
   c_traits_base::char_class_cntrl,        // 'õ'  155
   c_traits_base::char_class_cntrl,        // 'ú'  156
   c_traits_base::char_class_cntrl,        // 'ù'  157
   c_traits_base::char_class_cntrl,        // 'û'  158
   c_traits_base::char_class_cntrl,        // 'ü'  159
   c_traits_base::char_class_space | c_traits_base::char_class_blank,        // '†'  160
   c_traits_base::char_class_punct,        // '°'  161
   c_traits_base::char_class_punct,        // '¢'  162
   c_traits_base::char_class_punct,        // '£'  163
   c_traits_base::char_class_punct,        // '§'  164
   c_traits_base::char_class_punct,        // '•'  165
   c_traits_base::char_class_punct,        // '¶'  166
   c_traits_base::char_class_punct,        // 'ß'  167
   c_traits_base::char_class_punct,        // '®'  168
   c_traits_base::char_class_punct,        // '©'  169
   c_traits_base::char_class_punct,        // '™'  170
   c_traits_base::char_class_punct,        // '´'  171
   c_traits_base::char_class_punct,        // '¨'  172
   c_traits_base::char_class_punct,        // '≠'  173
   c_traits_base::char_class_punct,        // 'Æ'  174
   c_traits_base::char_class_punct,        // 'Ø'  175
   c_traits_base::char_class_punct,        // '∞'  176
   c_traits_base::char_class_punct,        // '±'  177
   c_traits_base::char_class_punct,        // '≤'  178
   c_traits_base::char_class_punct,        // '≥'  179
   c_traits_base::char_class_punct,        // '¥'  180
   c_traits_base::char_class_punct,        // 'µ'  181
   c_traits_base::char_class_punct,        // '∂'  182
   c_traits_base::char_class_punct,        // '∑'  183
   c_traits_base::char_class_punct,        // '∏'  184
   c_traits_base::char_class_punct,        // 'π'  185
   c_traits_base::char_class_punct,        // '∫'  186
   c_traits_base::char_class_punct,        // 'ª'  187
   c_traits_base::char_class_punct,        // 'º'  188
   c_traits_base::char_class_punct,        // 'Ω'  189
   c_traits_base::char_class_punct,        // 'æ'  190
   c_traits_base::char_class_punct,        // 'ø'  191
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '¿'  192
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '¡'  193
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '¬'  194
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '√'  195
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'ƒ'  196
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '≈'  197
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '∆'  198
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '«'  199
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '»'  200
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '…'  201
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // ' '  202
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'À'  203
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'Ã'  204
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'Õ'  205
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'Œ'  206
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'œ'  207
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '–'  208
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '—'  209
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '“'  210
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '”'  211
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '‘'  212
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '’'  213
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '÷'  214
   c_traits_base::char_class_punct,        // '◊'  215
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'ÿ'  216
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // 'Ÿ'  217
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '⁄'  218
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '€'  219
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '‹'  220
   c_traits_base::char_class_alpha |   c_traits_base::char_class_upper,        // '›'  221
   c_traits_base::char_class_alpha |  c_traits_base::char_class_upper,        // 'ﬁ'  222
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'ﬂ'  223
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // '‡'  224
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // '·'  225
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // '‚'  226
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // '„'  227
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // '‰'  228
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'Â'  229
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'Ê'  230
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'Á'  231
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'Ë'  232
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'È'  233
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'Í'  234
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'Î'  235
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'Ï'  236
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'Ì'  237
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'Ó'  238
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'Ô'  239
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // ''  240
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'Ò'  241
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'Ú'  242
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'Û'  243
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'Ù'  244
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'ı'  245
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'ˆ'  246
   c_traits_base::char_class_punct,        // '˜'  247
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // '¯'  248
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // '˘'  249
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // '˙'  250
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // '˚'  251
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // '¸'  252
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // '˝'  253
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // '˛'  254
   c_traits_base::char_class_alpha |  c_traits_base::char_class_lower,        // 'ˇ'  255
};

BOOST_REGEX_DECL wchar_t wide_lower_case_map[] = {
   0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa, 0xb, 0xc, 0xd, 0xe, 0xf,
   0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 
   0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 
   0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, 
   0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 
   0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, 
   0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 
   0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, 
   0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, 
   0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 
   0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, 
   0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 
   0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 
   0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xd7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xdf, 
   0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 
   0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff, 
};
#endif // BOOST_NO_WREGEX

   } // namespace re_detail
} // namespace boost


