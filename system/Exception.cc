/*******************************************************************************

  C++ source for thread lib base classes

  created 01.07.00                                                        ian

  (c) Masuma Limited 2000.  All rights reserved.

*******************************************************************************/

#include "Exception.h"
#include <execinfo.h>

namespace masuma::utils
{
  int Exception::backtraceDepth {};

  Exception::Exception( int          err,
                        std::string  reason,
                        std::string  file,
                        int          line ) noexcept
    : reason {std::move( reason )}, file {std::move( file )}, line {line}, err {err}
  {
    if( backtraceDepth > 0 )
    {
      backtraceSize = backtrace(backtraceData, backtraceDepth);
    }
  }

  const char*
  Exception::what() const noexcept
  {
    static std::string stuff;

    stuff = reason;

    if( !file.empty() )
    {
      stuff += " in ";
      stuff += file;
    }

    if( line )
    {
      stuff += " at line ";

      char buff[16];

      sprintf( buff, "%d", line );

      stuff += buff;
    }

    if( err )
    {
      stuff = stuff + ": " + err2String();
    }

    if( backtraceDepth > 0 )
    {
      char** strings{backtrace_symbols(backtraceData, backtraceDepth)};

      if (strings)
      {
        for (unsigned n = 3; n < backtraceSize; ++n)
        {
          stuff += '\n' + std::string(strings[n]);
        }
        free(strings);
      }
    }
    return stuff.c_str();
  }

  int
  Exception::check( const char* fn,
                    int         result,
                    const char* file,
                    int         line ,
                    const std::string& message )
  {
    if( result != 0 )
    {
      if( message.empty() )
      {
        throw Exception( result, std::string( "check: " )+fn, file, line );
      }
      else
      {
        throw Exception( errno, message );
      }
    }

    return result;
  }

  int
  Exception::check( const char* fn,
                    int         result,
                    const char* file,
                    int         line ,
                    std::string (*errorFn)(int) )
  {
    if( result != 0 )
    {
      throw Exception( doNotExpandErrno,
                       std::string("check: ")+fn+' '+errorFn(result),
                       file, line );
    }

    return result;
  }

  int
  Exception::checkErrno( const char* fn,
                         int         result,
                         const char* file,
                         int         line,
                         const std::string& message )
  {
    if( errno != 0 )
    {
      if( message.empty() )
      {
        throw Exception(errno, std::string("checkSys: ")+fn, file, line);
      }
      else
      {
        throw Exception( errno, message );
      }
    }

    return result;
  }

  void
  Exception::checkCondition( bool        condidtion,
                             const char* cond,
                             const char* file,
                             int         line,
                             const std::string& message )
  {
    if( !condidtion )
    {
      if( message.empty() )
      {
        throw Exception( std::string( "checkCondition: " )+cond, file, line );
      }
      else
      {
        throw Exception( errno, message );
      }
    }
  }
}
