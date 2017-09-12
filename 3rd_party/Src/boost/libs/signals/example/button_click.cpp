// Boost.Signals library
//
// Copyright (C) 2001 Doug Gregor (gregod@cs.rpi.edu)
//
// Permission to copy, use, sell and distribute this software is granted
// provided this copyright notice appears in all copies.
// Permission to modify the code and to distribute modified code is granted
// provided this copyright notice appears in all copies, and a notice
// that the code was modified is included with the copyright notice.
//
// This software is provided "as is" without express or implied warranty,
// and with no claim as to its suitability for any purpose.
 
// For more information, see http://www.boost.org

#include <boost/bind.hpp>
#include <boost/signals/signal1.hpp>
#include <iostream>

struct print_string : public boost::signals::trackable {
  typedef void result_type;
  
  void print(const std::string& s) const { std::cout << s << std::endl; }
};

struct my_button {
  typedef boost::signal1<void, const std::string&> click_signal_type;
  typedef click_signal_type::slot_type click_slot_type;

  boost::signals::connection on_click_connect(const click_slot_type& s)
    { return on_click.connect(s); }

  my_button(const std::string& l) : label(l) {}

  virtual ~my_button() {}

  void click();

protected:
  virtual void clicked() { on_click(label); }

private:
  std::string label;
  click_signal_type on_click;
};

void my_button::click()
{
  clicked();
}

int main() 
{
  my_button* b = new my_button("OK!");
  print_string* ps = new print_string();
  b->on_click_connect(boost::bind(&print_string::print, ps, _1));

  b->click(); // prints OK!

  delete ps;

  b->click(); // prints nothing

  return 0;
}
