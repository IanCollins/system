/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2010-2013.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: Streambuff for file I/O
 *
 ******************************************************************************/

#pragma once

#include "Streambuf.h"
#include "AutoFd.h"

namespace masuma::system
{
  class AutoStreamState
  {
  protected:

    int timeout;

    AutoStreamState() : timeout() {}

  public:

    void setTimeout( const timespec& ts )
    {
      timeout = ts.tv_sec*1000 + ts.tv_nsec/1000000;
    }

    void setTimeout( int ts ) { timeout = ts; }
  };

  struct AutoIStreambuf : IStreambuf, AutoStreamState
  {
    AutoFd& fd;
    unsigned reads {};
    ssize_t lastRead {};

    ssize_t read( void* data, size_t size ) override
    {
      lastRead = timeout ? fd.read( data, size, timeout ) : fd.read( data, size );

      if( lastRead >= 0 )
      {
        inputBuffer[lastRead] = 0;
      }
      else
      {
        inputBuffer[0] = 0;
      }
      ++reads;

      return lastRead;
    }

    AutoIStreambuf( AutoFd& fd, char* p, size_t size )
      : IStreambuf( p, size ), fd(fd)
    {
    }

    ~AutoIStreambuf() override = default;

    friend std::ostream& operator<<( std::ostream& out,
                                     const AutoIStreambuf& buff)
    {
      out << "Reads: " << buff.reads << " read: " << buff.lastRead << '\n';

      return out << buff.inputBuffer;
    }
  };

  struct AutoOStreambuf : OStreambuf, AutoStreamState
  {
    AutoFd& fd;

    ssize_t write( const void* data, size_t size ) override
    {
      return timeout ? fd.write( data, size, timeout ) : fd.write( data, size );
    }

    AutoOStreambuf( AutoFd& fd, char* p, size_t size )
      : OStreambuf( p, size ), fd(fd)
    {
    }

    ~AutoOStreambuf() override = default;
  };
}
