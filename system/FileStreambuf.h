/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2010-2013.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: Streambuff for file I/O
 *
 ******************************************************************************/

#ifndef _utils_FileStreambuf_h_
#define _utils_FileStreambuf_h_

#include "Streambuf.h"

namespace masuma
{
  namespace system
  {
    struct IFileStreambuf : IStreambuf
    {
      int fd;

      char* inBuf;

      ssize_t read( void* data, size_t size ) override
      {
        return ::read( fd, data, size );
      }

      IFileStreambuf( int fd, size_t bufferSize = 16*1024 )
        : IStreambuf( inBuf = new char[bufferSize], bufferSize ), fd(fd)
      {
      }

      ~IFileStreambuf() override
      {
        delete inBuf;
      }
    };

    struct OFileStreambuf : OStreambuf
    {
      int fd;

      char* outbuf;

      ssize_t write( const void* data, size_t size ) override
      {
        return ::write( fd, data, size );
      }

      OFileStreambuf( int fd, size_t bufferSize = 16*1024 )
        : OStreambuf( outbuf = new char[bufferSize], bufferSize ), fd(fd)
      {
      }

      ~OFileStreambuf() override
      {
        delete outbuf;
      }
    };

    struct FileStreambuf : Streambuf
    {
      int fd;

      char* inBuf;
      char* outbuf;

      ssize_t read( void* data, size_t size ) override
      {
        return ::read( fd, data, size );
      }

      ssize_t write( const void* data, size_t size ) override
      {
        return ::write( fd, data, size );
      }

      FileStreambuf( int fd, size_t bufferSize = 16*1024 )
        : Streambuf( inBuf  = new char[bufferSize], bufferSize,
                     outbuf = new char[bufferSize], bufferSize ), fd(fd)
      {
      }

      ~FileStreambuf() override
      {
        delete inBuf;
        delete outbuf;
      }
    };
  }
}

#endif
