/******************************* C++ Header File *******************************
*
*  Copyright (c) Masuma Ltd 2012-2016.  All rights reserved.
*
*  MODULE:      utils transfer statistics
*
*******************************************************************************/

#ifndef _utils_TransferReport_h_
#define	_utils_TransferReport_h_

#include "Time.h"
#include "Streamable.h"
#include "Log.h"

namespace masuma::utils
{
  class TransferBytes : public Streamable
  {
  public:

    static const uint64_t oneK = 1024;
    static const uint64_t oneM = oneK * oneK;
    static const uint64_t oneG = oneM * oneK;
    static const uint64_t oneT = oneG * oneK;

  private:

    uint64_t bytes;

    void stream( std::ostream& ) const;

  public:

    typedef std::pair<double,std::string> ValueUnit;

    explicit TransferBytes( uint64_t bytes ) : bytes {bytes} {}

    ValueUnit valueUnit( double scale = 1.0 ) const;
  };

  class ElapsedTime : public utils::Streamable
  {
    static const time_t oneMinute = 60;
    static const time_t oneHour   = oneMinute*60;
    static const time_t oneDay    = oneHour*24;

    double time;

    static double streamBit( std::ostream&, double, time_t, const std::string& );
    void stream( std::ostream& ) const;

  public:

    explicit ElapsedTime( double time ) : time(time) {}
  };

  class TransferReport
  {
    Stopwatch stopwatch;

    uint64_t bytesSent  {0};
    uint64_t lastReport {0};

    Log::Level level;

  public:

    static uint64_t threshold;

    TransferReport( Log::Level );

    void start() { stopwatch.start(); }

    double elapsed() const { return stopwatch.elapsed(); }

    uint64_t operator()( uint64_t n )
    {
      bytesSent += n;

      if( bytesSent - lastReport > threshold )
      {
        report();

        lastReport = bytesSent;
      }

      return n;
    }

    uint64_t report();
    uint64_t report( uint64_t& );
  };
}

#endif
