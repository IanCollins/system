/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2010.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: Time related utility functions.
 *
 ******************************************************************************/

#pragma once

#include <string>
#include <sstream>
#include <ctime>
#include <cstdint>
#include "Exception.h"

namespace masuma
{
  namespace times
  {
    constexpr time_t oneMinute = 60;
    constexpr time_t oneHour   = oneMinute*60;
    constexpr time_t oneDay    = oneHour*24;
  }

  namespace system
  {
    constexpr int64_t oneThousand {1'000};
    constexpr int64_t oneMillion {oneThousand*1'000};
    constexpr int64_t oneBillion {oneMillion*1'000};

    tm localTime( clockid_t clock = CLOCK_REALTIME );

    int64_t timeNow( clockid_t clock = CLOCK_REALTIME );
    time_t timeNowInSeconds( clockid_t clock = CLOCK_REALTIME );

    inline double
    timeInSeconds( int64_t time )
    {
      return double(time)/oneBillion;
    }

    inline timespec
    asTimespec( int64_t time )
    {
      timespec ts {};
      ts.tv_sec  = time/oneBillion;
      ts.tv_nsec = time%oneBillion;
      return ts;
    }

    std::string asSysTime( double );

    template <typename T> T now( clockid_t clock = CLOCK_REALTIME );

  //  template <typename T> void now( T& t, clockid_t clock = CLOCK_REALTIME )
  //  {
  //    t = new<T>( clock );
  //  }

    template<> inline timespec
    now( clockid_t clock )
    {
      timespec ts {};
      clock_gettime( clock, &ts );
      return ts;
    }

    template<> inline
    long long now( clockid_t clock )
    {
      timespec ts = now<timespec>( clock );
      return ts.tv_sec*oneBillion + ts.tv_nsec;
    }

    template<> inline time_t
    now( clockid_t clock )
    {
      timespec ts = now<timespec>( clock );
      return ts.tv_sec;
    }

    template<> inline tm
    now( clockid_t clock )
    {
      return localTime( clock );
    }

    template <clockid_t CLOCK>
    class Clock
    {
    public:

      typedef int64_t tick_t;

      explicit operator tick_t() const { return timeNow(CLOCK); }
    };

    template <typename T>
    T since( T before )
    {
      return now<T>() - before;
    }

    template <typename T>
    void setToNow( T& t )
    {
      t = now<T>();
    }

    struct [[maybe_unused]] GMT : tm
    {
      explicit GMT( time_t nowSeconds ) : tm()
      {
        CheckNull( gmtime_r, ( &nowSeconds, this ) );
      }

      GMT() : GMT{now<time_t>()} {}
    };

#if defined CLOCK_HIGHRES
    typedef Clock<CLOCK_HIGHRES>  HighresClock;
#endif
    typedef Clock<CLOCK_REALTIME> RealtimeClock;

    class Stopwatch
    {
      const clockid_t clock;
      int64_t startTime;

    public:

      Stopwatch() : clock(CLOCK_REALTIME), startTime() {}

      explicit Stopwatch( int64_t t ) : clock(CLOCK_REALTIME), startTime(t) {}

      Stopwatch( clockid_t clock, bool ) : clock(clock), startTime() {}

      void start() { startTime = timeNow(clock); }

      [[nodiscard]] double elapsed() const
      {
        return timeInSeconds( timeNow(clock) - startTime );
      }
    };
  }
}

