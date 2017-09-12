/*
The following shows the creation of a facet for the output of 
dates in German (please forgive me for any errors in my German --
I'm not a native speaker).



*/

#include "boost/date_time/gregorian/gregorian.hpp"
#include <iostream>

//Define a series of char arrays for short and long name strings to be 
//associated with date output.
const char* const de_short_month_names[]={"Jan","Feb","Mar","Apr","Mai","Jun","Jul","Aug","Sep","Okt","Nov","Dez", "NAM"};

const char* const de_long_month_names[]={"Januar","Februar","Marz","April","Mai","Juni","Juli","August","September","Oktober","November","Dezember","NichtDerMonat"};
const char* const de_special_value_names[]={"NichtDatumzeit","-unbegrenztheit", "+unbegrenztheit"};
 
const char* const de_long_weekday_names[]={"Sonntag", "Montag", "Dienstag","Mittwoch", "Donnerstag", "Freitag", "Samstag"};

const char* const de_short_weekday_names[]={"Son", "Mon", "Die","Mit", "Don", "Fre", "Sam"};

const char* const us_short_month_names[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec", "NAD"};

const char* const us_long_month_names[]={"January","February","March","April","May","June","July","August","September","October","November","December","Not-A-Date"};
const char* const us_special_value_names[]={"Not-A-Date","-infinity", "+infinity"};
 
const char* const us_long_weekday_names[]={"Sunday", "Monday", "Tuesday","Wenesday", "Thursday", "Friday", "Saturday"};

const char* const us_short_weekday_names[]={"Sun", "Mon", "Tue","Wed", "Thu", "Fri", "Sat"};


int
main() 
{

#ifndef BOOST_DATE_TIME_NO_LOCALE

  using namespace boost::gregorian;
  typedef boost::date_time::all_date_names_put<greg_facet_config> date_facet;
  
  //create a new local
  std::locale default_locale;
  std::locale german_dates1(default_locale, 
                            new date_facet(de_short_month_names, 
                                           de_long_month_names,
                                           de_special_value_names,
                                           de_short_weekday_names,
                                           de_long_weekday_names,
                                           '.',
                                           boost::date_time::ymd_order_dmy,
                                           boost::date_time::month_as_integer));
  
  date d1(2002, Oct, 1);
  std::cout.imbue(german_dates1); 
  // output the date in German using short month names
  std::cout << d1 << std::endl; //01.10.2002
  
  std::locale german_dates2(default_locale, 
                            new date_facet(de_short_month_names, 
                                           de_long_month_names,
                                           de_special_value_names,
                                           de_short_weekday_names,
                                           de_long_weekday_names,
                                           '.',
                                           boost::date_time::ymd_order_dmy,
                                           boost::date_time::month_as_long_string));
  
  std::cout.imbue(german_dates2); 
  greg_month m = d1.month();
  std::cout << m << std::endl; //Oktober
  
  greg_weekday wd = d1.day_of_week();
  std::cout << wd << std::endl; //Dienstag


  //Numeric date format with US month/day/year ordering
  std::locale usa_dates1(default_locale, 
                         new date_facet(us_short_month_names, 
                                        us_long_month_names,
                                        us_special_value_names,
                                        us_short_weekday_names,
                                        us_long_weekday_names,
                                        '/',
                                        boost::date_time::ymd_order_us,
                                        boost::date_time::month_as_integer));
  
  std::cout.imbue(usa_dates1); 
  std::cout << d1 << std::endl; //  10/01/2002
  //English names, iso order (year-month-day), '-' separator
  std::locale usa_dates2(default_locale, 
                         new date_facet(us_short_month_names, 
                                        us_long_month_names,
                                        us_special_value_names,
                                        us_short_weekday_names,
                                        us_long_weekday_names,
                                        '-',
                                        boost::date_time::ymd_order_iso,
                                        boost::date_time::month_as_short_string));

  std::cout.imbue(usa_dates2); 
  std::cout << d1 << std::endl; //  2002-Oct-01
  
  
#else 
  std::cout << "Sorry, localization is not supported by this compiler/library" 
	    << std::endl;
#endif
  return 0;

}
