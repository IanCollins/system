/******************************* C++ Source File *******************************
*
*  Copyright (c) Masuma Ltd 2012-2016.  All rights reserved.
*
*  MODULE:      utils
*
*******************************************************************************/

#include "TransferReport.h"
#include "Setup.h"

#include <iomanip>

namespace masuma::utils
{
  TransferBytes::ValueUnit
  TransferBytes::valueUnit( double scale ) const
  {
    double value = bytes/scale;

    if( value < oneK )
    {
      return ValueUnit(value, "B");
    }
    else if( value < oneM )
    {
      return ValueUnit(value / oneK, "KB");
    }
    else if( value < oneG )
    {
      return ValueUnit(value / oneM, "MB");
    }
    else if( value < oneT )
    {
      return ValueUnit(value / oneG, "GB");
    }
    else
    {
      return ValueUnit(value / oneT, "TB");
    }
  }

  void
  TransferBytes::stream( std::ostream& out ) const
  {
    if( bytes < oneK )
    {
      out << std::setw(7) << bytes << 'B';
    }
    else
    {
      out.precision( 2 );

      ValueUnit vu = valueUnit();

      out << std::fixed << std::setw(7) << vu.first << vu.second;
    }
  }

  double
  ElapsedTime::streamBit( std::ostream& out, double time,
                          time_t unit, const std::string& unitString )
  {
    if( time >= unit )
    {
      time_t scaled = time / unit;

      out << scaled << unitString;

      if( scaled != 1.0 )
      {
        out << 's';
      }

      out << ' ';

      return time - scaled*unit;
    }
    else
    {
      return time;
    }
  }

  void
  ElapsedTime::stream( std::ostream& out ) const
  {
    out.precision( 2 );
    out << std::fixed;

    double reamining(time);

    reamining = streamBit( out, reamining, oneDay, " Day" );
    reamining = streamBit( out, reamining, oneHour, " Hour" );
    reamining = streamBit( out, reamining, oneMinute, " Minute" );

    out << reamining << " Second";

    if( reamining != 1.0 )
    {
      out << 's';
    }
  }

  uint64_t TransferReport::threshold = 1024 * 1024 * 1024;

  TransferReport::TransferReport( utils::Log::Level level )
#if defined CLOCK_HIGHRES
      : stopwatch {CLOCK_HIGHRES, true}, level {level}
#else
      : stopwatch {CLOCK_REALTIME, true}, level {level}
#endif
  {
  }

  uint64_t
  TransferReport::report( uint64_t& n )
  {
    double elapsed = stopwatch.elapsed();

    TransferBytes::ValueUnit vu = TransferBytes(n).valueUnit( elapsed );

    Log(level) << TransferBytes(n) << " in " << ElapsedTime(elapsed) << ", "
               << vu.first << vu.second << "/sec" << std::endl;

    return n;
  }

  uint64_t
  TransferReport::report()
  {
    return report(bytesSent);
  }
}
