#ifndef INCLUDED_TIMESTAMP_HPP
#define INCLUDED_TIMESTAMP_HPP

#include <ctime>
#include <iostream>
#include <string>

struct timestamp : public tm
    {
    timestamp() { memset(this, 0, sizeof(*this)); }
    int tzoffset;
    };

std::ostream& operator<< (std::ostream& os, const timestamp& ts)
    {
    os << "timestamp =\n"
       << "    {\n"
       << "    tm_sec   = " << ts.tm_sec   << "\n"
       << "    tm_min   = " << ts.tm_min   << "\n"
       << "    tm_hour  = " << ts.tm_hour  << "\n"
       << "    tm_mday  = " << ts.tm_mday  << "\n"
       << "    tm_mon   = " << ts.tm_mon   << "\n"
       << "    tm_year  = " << ts.tm_year  << "\n"
       << "    tm_wday  = " << ts.tm_wday  << "\n"
       << "    tm_yday  = " << ts.tm_yday  << "\n"
       << "    tm_isdst = " << ts.tm_isdst << "\n"
       << "    tzoffset = " << ts.tzoffset << "\n"
       << "    }\n";
    return os;
    }

#endif
