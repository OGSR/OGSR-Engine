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
  *   FILE:        cregex.cpp
  *   VERSION:     see <boost/version.hpp>
  *   DESCRIPTION: Implements high level class boost::RexEx
  */


#define BOOST_REGEX_SOURCE

#include <boost/cregex.hpp>
#include <boost/regex.hpp>
#if !defined(BOOST_NO_STD_STRING)
#include <map>
#include <list>
#include <boost/regex/v3/fileiter.hpp>
#include <cstdio>

namespace boost{

#ifdef __BORLANDC__
#if __BORLANDC__ < 0x530
//
// we need to instantiate the vector classes we use
// since declaring a reference to type doesn't seem to
// do the job...
std::vector<std::size_t> inst1;
std::vector<std::string> inst2;
#endif
#endif

namespace{

template <class iterator>
std::string to_string(iterator i, iterator j)
{
   BOOST_RE_GUARD_STACK
   std::string s;
   while(i != j)
   {
      s.append(1, *i);
      ++i;
   }
   return s;
}

inline std::string to_string(const char* i, const char* j)
{
   return std::string(i, j);
}

}
namespace re_detail{

class RegExData
{
public:
   enum type
   {
      type_pc,
      type_pf,
      type_copy
   };
   regex e;
   cmatch m;
#ifndef BOOST_REGEX_NO_FILEITER
   match_results<mapfile::iterator, regex::allocator_type> fm;
#endif
   type t;
   const char* pbase;
   unsigned line;
#ifndef BOOST_REGEX_NO_FILEITER
   mapfile::iterator fbase;
#endif
   std::map<int, std::string, std::less<int> > strings;
   std::map<int, std::ptrdiff_t, std::less<int> > positions;
   void update();
   void clean();
   RegExData() : e(), m(),
#ifndef BOOST_REGEX_NO_FILEITER
   fm(),
#endif
   t(type_copy), pbase(0), line(0),
#ifndef BOOST_REGEX_NO_FILEITER
   fbase(),
#endif
   strings(), positions() {}
};

void RegExData::update()
{
   BOOST_RE_GUARD_STACK
   strings.erase(strings.begin(), strings.end());
   positions.erase(positions.begin(), positions.end());
   if(t == type_pc)
   {
      for(unsigned int i = 0; i < m.size(); ++i)
      {
         if(m[i].matched) strings[i] = std::string(m[i].first, m[i].second);
         positions[i] = m[i].matched ? m[i].first - pbase : -1;
      }
      line = m.line();
   }
#ifndef BOOST_REGEX_NO_FILEITER
   else
   {
      for(unsigned int i = 0; i < fm.size(); ++i)
      {
         if(fm[i].matched) strings[i] = to_string(fm[i].first, fm[i].second);
         positions[i] = fm[i].matched ? fm[i].first - fbase : -1;
      }
      line = fm.line();
   }
#endif
   t = type_copy;
}

void RegExData::clean()
{
   BOOST_RE_GUARD_STACK
#ifndef BOOST_REGEX_NO_FILEITER
   fbase = mapfile::iterator();
   fm = match_results<mapfile::iterator, regex::allocator_type>();
#endif
}

} // namespace

RegEx::RegEx()
{
   BOOST_RE_GUARD_STACK
   pdata = new re_detail::RegExData();
}

RegEx::RegEx(const RegEx& o)
{
   BOOST_RE_GUARD_STACK
   pdata = new re_detail::RegExData(*(o.pdata));
}

RegEx::~RegEx()
{
   BOOST_RE_GUARD_STACK
   delete pdata;
}

RegEx::RegEx(const char* c, bool icase)
{
   BOOST_RE_GUARD_STACK
   pdata = new re_detail::RegExData();
   SetExpression(c, icase);
}

RegEx::RegEx(const std::string& s, bool icase)
{
   BOOST_RE_GUARD_STACK
   pdata = new re_detail::RegExData();
   SetExpression(s.c_str(), icase);
}

RegEx& RegEx::operator=(const RegEx& o)
{
   BOOST_RE_GUARD_STACK
   *pdata = *(o.pdata);
   return *this;
}

RegEx& RegEx::operator=(const char* p)
{
   BOOST_RE_GUARD_STACK
   SetExpression(p, false);
   return *this;
}

unsigned int RegEx::SetExpression(const char* p, bool icase)
{
   BOOST_RE_GUARD_STACK
   boost::uint_fast32_t f = icase ? regbase::normal | regbase::use_except | regbase::icase : regbase::normal | regbase::use_except;
   return pdata->e.set_expression(p, f);
}

unsigned int RegEx::error_code()const
{
   return pdata->e.error_code();
}


std::string RegEx::Expression()const
{
   BOOST_RE_GUARD_STACK
   return pdata->e.expression();
}

//
// now matching operators:
//
bool RegEx::Match(const char* p, unsigned int flags)
{
   BOOST_RE_GUARD_STACK
   pdata->t = re_detail::RegExData::type_pc;
   pdata->pbase = p;
   const char* end = p;
   while(*end)++end;

   if(regex_match(p, end, pdata->m, pdata->e, flags))
   {
      pdata->update();
      return true;
   }
   return false;
}

bool RegEx::Search(const char* p, unsigned int flags)
{
   BOOST_RE_GUARD_STACK
   pdata->t = re_detail::RegExData::type_pc;
   pdata->pbase = p;
   const char* end = p;
   while(*end)++end;

   if(regex_search(p, end, pdata->m, pdata->e, flags))
   {
      pdata->update();
      return true;
   }
   return false;
}
namespace re_detail{
struct pred1
{
   GrepCallback cb;
   RegEx* pe;
   pred1(GrepCallback c, RegEx* i) : cb(c), pe(i) {}
   bool operator()(const cmatch& m)
   {
      pe->pdata->m = m;
      return cb(*pe);
   }
};
}
unsigned int RegEx::Grep(GrepCallback cb, const char* p, unsigned int flags)
{
   BOOST_RE_GUARD_STACK
   pdata->t = re_detail::RegExData::type_pc;
   pdata->pbase = p;
   const char* end = p;
   while(*end)++end;

   unsigned int result = regex_grep(re_detail::pred1(cb, this), p, end, pdata->e, flags);
   if(result)
      pdata->update();
   return result;
}
namespace re_detail{
struct pred2
{
   std::vector<std::string>& v;
   RegEx* pe;
   pred2(std::vector<std::string>& o, RegEx* e) : v(o), pe(e) {}
   bool operator()(const cmatch& m)
   {
      pe->pdata->m = m;
      v.push_back(std::string(m[0].first, m[0].second));
      return true;
   }
private:
   pred2& operator=(const pred2&);
};
}

unsigned int RegEx::Grep(std::vector<std::string>& v, const char* p, unsigned int flags)
{
   BOOST_RE_GUARD_STACK
   pdata->t = re_detail::RegExData::type_pc;
   pdata->pbase = p;
   const char* end = p;
   while(*end)++end;

   unsigned int result = regex_grep(re_detail::pred2(v, this), p, end, pdata->e, flags);
   if(result)
      pdata->update();
   return result;
}
namespace re_detail{
struct pred3
{
   std::vector<std::size_t>& v;
   const char* base;
   RegEx* pe;
   pred3(std::vector<std::size_t>& o, const char* pb, RegEx* p) : v(o), base(pb), pe(p) {}
   bool operator()(const cmatch& m)
   {
      pe->pdata->m = m;
      v.push_back(static_cast<std::size_t>(m[0].first - base));
      return true;
   }
private:
   pred3& operator=(const pred3&);
};
}
unsigned int RegEx::Grep(std::vector<std::size_t>& v, const char* p, unsigned int flags)
{
   BOOST_RE_GUARD_STACK
   pdata->t = re_detail::RegExData::type_pc;
   pdata->pbase = p;
   const char* end = p;
   while(*end)++end;

   unsigned int result = regex_grep(re_detail::pred3(v, p, this), p, end, pdata->e, flags);
   if(result)
      pdata->update();
   return result;
}
#ifndef BOOST_REGEX_NO_FILEITER
namespace re_detail{
struct pred4
{
   GrepFileCallback cb;
   RegEx* pe;
   const char* file;
   bool ok;
   pred4(GrepFileCallback c, RegEx* i, const char* f) : cb(c), pe(i), file(f), ok(true) {}
   bool operator()(const match_results<mapfile::iterator, regex::allocator_type>& m)
   {
      pe->pdata->t = RegExData::type_pf;
      pe->pdata->fm = m;
      pe->pdata->update();
      ok = cb(file, *pe);
      return ok;
   }
};
}
namespace{
void BuildFileList(std::list<std::string>* pl, const char* files, bool recurse)
{
   BOOST_RE_GUARD_STACK
   file_iterator start(files);
   file_iterator end;
   if(recurse)
   {
      // go through sub directories:
      char buf[MAX_PATH];
      std::strcpy(buf, start.root());
      if(*buf == 0)
      {
         std::strcpy(buf, ".");
         std::strcat(buf, directory_iterator::separator());
         std::strcat(buf, "*");
      }
      else
      {
         std::strcat(buf, directory_iterator::separator());
         std::strcat(buf, "*");
      }
      directory_iterator dstart(buf);
      directory_iterator dend;

      // now get the file mask bit of "files":
      const char* ptr = files;
      while(*ptr) ++ptr;
      while((ptr != files) && (*ptr != *directory_iterator::separator()) && (*ptr != '/'))--ptr;
      if(ptr != files) ++ptr;

      while(dstart != dend)
      {
         std::sprintf(buf, "%s%s%s", dstart.path(), directory_iterator::separator(), ptr);
         BuildFileList(pl, buf, recurse);
         ++dstart;
      }
   }
   while(start != end)
   {
      pl->push_back(*start);
      ++start;
   }
}
}

unsigned int RegEx::GrepFiles(GrepFileCallback cb, const char* files, bool recurse, unsigned int flags)
{
   BOOST_RE_GUARD_STACK
   unsigned int result = 0;
   std::list<std::string> file_list;
   BuildFileList(&file_list, files, recurse);
   std::list<std::string>::iterator start, end;
   start = file_list.begin();
   end = file_list.end();

   while(start != end)
   {
      mapfile map((*start).c_str());
      pdata->t = re_detail::RegExData::type_pf;
      pdata->fbase = map.begin();
      re_detail::pred4 pred(cb, this, (*start).c_str());
      int r = regex_grep(pred, map.begin(), map.end(), pdata->e, flags);
      result += r;
      ++start;
      pdata->clean();
      if(pred.ok == false)
         return result;
   }

   return result;
}


unsigned int RegEx::FindFiles(FindFilesCallback cb, const char* files, bool recurse, unsigned int flags)
{
   BOOST_RE_GUARD_STACK
   unsigned int result = 0;
   std::list<std::string> file_list;
   BuildFileList(&file_list, files, recurse);
   std::list<std::string>::iterator start, end;
   start = file_list.begin();
   end = file_list.end();

   while(start != end)
   {
      mapfile map((*start).c_str());
      pdata->t = re_detail::RegExData::type_pf;
      pdata->fbase = map.begin();

      if(regex_search(map.begin(), map.end(), pdata->fm, pdata->e, flags))
      {
         ++result;
         if(false == cb((*start).c_str()))
            return result;
      }
      //pdata->update();
      ++start;
      //pdata->clean();
   }

   return result;
}
#endif

std::string RegEx::Merge(const std::string& in, const std::string& fmt,
                    bool copy, unsigned int flags)
{
   std::string result;
   re_detail::string_out_iterator<std::string> i(result);
   if(!copy) flags |= format_no_copy;
   regex_merge(i, in.begin(), in.end(), pdata->e, fmt.c_str(), flags);
   return result;
}

std::string RegEx::Merge(const char* in, const char* fmt,
                    bool copy, unsigned int flags)
{
   std::string result;
   if(!copy) flags |= format_no_copy;
   re_detail::string_out_iterator<std::string> i(result);
   regex_merge(i, in, in + std::strlen(in), pdata->e, fmt, flags);
   return result;
}

std::size_t RegEx::Split(std::vector<std::string>& v, 
                      std::string& s,
                      unsigned flags,
                      unsigned max_count)
{
   return regex_split(std::back_inserter(v), s, pdata->e, flags, max_count);
}



//
// now operators for returning what matched in more detail:
//
std::size_t RegEx::Position(int i)const
{
   BOOST_RE_GUARD_STACK
   switch(pdata->t)
   {
   case re_detail::RegExData::type_pc:
      return pdata->m[i].matched ? pdata->m[i].first - pdata->pbase : RegEx::npos;
#ifndef BOOST_REGEX_NO_FILEITER
   case re_detail::RegExData::type_pf:
      return pdata->fm[i].matched ? pdata->fm[i].first - pdata->fbase : RegEx::npos;
#endif
   case re_detail::RegExData::type_copy:
      {
      std::map<int, std::ptrdiff_t, std::less<int> >::iterator pos = pdata->positions.find(i);
      if(pos == pdata->positions.end())
         return RegEx::npos;
      return (*pos).second;
      }
   }
   return RegEx::npos;
}

unsigned int RegEx::Line()const
{
   BOOST_RE_GUARD_STACK
   switch(pdata->t)
   {
   case re_detail::RegExData::type_pc:
      return pdata->m[0].matched ? pdata->m.line() : RegEx::npos;
#ifndef BOOST_REGEX_NO_FILEITER
   case re_detail::RegExData::type_pf:
      return pdata->fm[0].matched ? pdata->fm.line() : RegEx::npos;
#endif
   case re_detail::RegExData::type_copy:
      {
         return pdata->line;
      }
   }
   return RegEx::npos;
}

unsigned int RegEx::Marks()const
{
   BOOST_RE_GUARD_STACK
   return pdata->e.mark_count();
}


std::size_t RegEx::Length(int i)const
{
   BOOST_RE_GUARD_STACK
   switch(pdata->t)
   {
   case re_detail::RegExData::type_pc:
      return pdata->m[i].matched ? pdata->m[i].second - pdata->m[i].first : RegEx::npos;
#ifndef BOOST_REGEX_NO_FILEITER
   case re_detail::RegExData::type_pf:
      return pdata->fm[i].matched ? pdata->fm[i].second - pdata->fm[i].first : RegEx::npos;
#endif
   case re_detail::RegExData::type_copy:
      {
      std::map<int, std::string, std::less<int> >::iterator pos = pdata->strings.find(i);
      if(pos == pdata->strings.end())
         return RegEx::npos;
      return (*pos).second.size();
      }
   }
   return RegEx::npos;
}

bool RegEx::Matched(int i)const
{
   BOOST_RE_GUARD_STACK
   switch(pdata->t)
   {
   case re_detail::RegExData::type_pc:
      return pdata->m[i].matched;
#ifndef BOOST_REGEX_NO_FILEITER
   case re_detail::RegExData::type_pf:
      return pdata->fm[i].matched;
#endif      
   case re_detail::RegExData::type_copy:
      {
      std::map<int, std::string, std::less<int> >::iterator pos = pdata->strings.find(i);
      if(pos == pdata->strings.end())
         return false;
      return true;
      }
   }
   return false;
}


std::string RegEx::What(int i)const
{
   BOOST_RE_GUARD_STACK
   std::string result;
   switch(pdata->t)
   {
   case re_detail::RegExData::type_pc:
      if(pdata->m[i].matched) 
         result.assign(pdata->m[i].first, pdata->m[i].second);
      break;
   case re_detail::RegExData::type_pf:
      if(pdata->m[i].matched) 
         result.assign(to_string(pdata->m[i].first, pdata->m[i].second));
      break;
   case re_detail::RegExData::type_copy:
      {
      std::map<int, std::string, std::less<int> >::iterator pos = pdata->strings.find(i);
      if(pos != pdata->strings.end())
         result = (*pos).second;
      break;
      }
   }
   return result;
}

const unsigned int RegEx::npos = ~0u;


} // namespace boost

#if defined(__BORLANDC__) && (__BORLANDC__ >= 0x550) && (__BORLANDC__ <= 0x551) && !defined(_RWSTD_COMPILE_INSTANTIATE)
//
// this is an ugly hack to work around an ugly problem:
// by default this file will produce unresolved externals during
// linking unless _RWSTD_COMPILE_INSTANTIATE is defined (Borland bug).
// However if _RWSTD_COMPILE_INSTANTIATE is defined then we get separate
// copies of basic_string's static data in the RTL and this DLL, this messes
// with basic_string's memory management and results in run-time crashes,
// Oh sweet joy of Catch 22....
//
namespace std{
template<> template<>
basic_string<char>&
basic_string<char>::replace<const char*>(char* f1, char* f2, const char* i1, const char* i2)
{
   unsigned insert_pos = f1 - begin();
   unsigned remove_len = f2 - f1;
   unsigned insert_len = i2 - i1;
   unsigned org_size = size();
   if(insert_len > remove_len)
   {
      append(insert_len-remove_len, ' ');
      std::copy_backward(begin() + insert_pos + remove_len, begin() + org_size, end());
      std::copy(i1, i2, begin() + insert_pos);
   }
   else
   {
      std::copy(begin() + insert_pos + remove_len, begin() + org_size, begin() + insert_pos + insert_len);
      std::copy(i1, i2, begin() + insert_pos);
      erase(size() + insert_len - remove_len);
   }
   return *this;
}
}
#endif

#endif













