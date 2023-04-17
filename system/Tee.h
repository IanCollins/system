/******************************* C++ Header File *******************************
*
*  Copyright (c) Masuma Ltd 2012.  All rights reserved.
*
*  MODULE:      utilities
*
*  DESCRIPTION: Tee Strambuff for file I/O
*
*******************************************************************************/

#pragma once

#include "Streambuf.h"
#include <utility>
#include <vector>

namespace masuma::utils
{
  struct OutTee : OStreambuf
  {
    class Fds : std::vector<int>
    {
      typedef std::vector<int> Base;

    public:

      using Base::const_iterator;
      using Base::begin;
      using Base::end;

      Fds& operator<<( int n )
      {
        push_back(n);
        return *this;
      }
    };

    Fds fds;

    char* outbuf {};

    ssize_t write( const void* data, size_t size ) override
    {
      for(int fd : fds)
      {
        ssize_t n = ::write( fd, data, size );

        if( n != (ssize_t)size )
        {
          return n;
        }
      }
      return size;
    }

    explicit OutTee( int* p, size_t bufferSize = 16*1024 )
      : OStreambuf( outbuf = new char[bufferSize], bufferSize )
    {
      while( *p )
      {
        fds << *p++;
      }
    }

    explicit OutTee( Fds fds, size_t bufferSize = 16*1024 )
      : OStreambuf( outbuf = new char[bufferSize], bufferSize ), fds(std::move(fds)) {}

    ~OutTee() override
    {
      delete outbuf;
    }
  };
}
