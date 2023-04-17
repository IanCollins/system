/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2010-2014.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: Time related utility functions.
 *
 ******************************************************************************/

#include "Time.h"
#include "Exception.h"

namespace masuma::utils
{
  int64_t
  timeNow( clockid_t clock )
  {
    timespec tp{};
    clock_gettime( clock, &tp );

    int64_t time = tp.tv_sec;
    time = time*oneBillion+tp.tv_nsec;
    return time;
  }

  time_t
  timeNowInSeconds( clockid_t clock )
  {
    timespec tp{};
    clock_gettime( clock, &tp );

    return tp.tv_sec;
  }

  tm
  localTime( clockid_t clock )
  {
    time_t tick = timeNowInSeconds( clock );

    tm timeNow{};
    CheckNull( localtime_r, ( &tick, &timeNow ) );

    return timeNow;
  }

  std::string
  asSysTime( double t )
  {
    uint64_t time = t*1000;
    unsigned ms   = time%1000ULL;
    time /= 1000;
    unsigned s = time%60ULL;
    time /= 60;
    unsigned m = time%60ULL;
    time /= 60;
    unsigned h = time%60ULL;

    std::ostringstream out;

    if( h )
    {
      out << h << 'h';
    }

    if( m )
    {
      out << m << 'm';
    }

    out<< s << '.' << ms << 's';

    return  out.str();
  }
}
