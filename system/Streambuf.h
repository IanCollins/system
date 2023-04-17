/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2010-2013.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: Strambuff template
 *
 ******************************************************************************/

#ifndef _utils_Streambuf_h_
#define _utils_Streambuf_h_

#include <unistd.h>
#include <stdint.h>
#include <streambuf>

namespace masuma
{
  namespace system
  {
    class IStreambuf : virtual public std::streambuf
    {
    protected:

      const size_t     inbufSize;
      char_type* const inputBuffer;

      uint64_t bytesIn;

    private:

      virtual ssize_t read( void* data, size_t size ) = 0;

      int_type underflow()
      {
        if( gptr() >= egptr() )
        {
          int numRead = read( inputBuffer, inbufSize );

          if( numRead <= 0 )
          {
            return traits_type::eof();
          }

          setg( inputBuffer, inputBuffer, inputBuffer+numRead );

          bytesIn += numRead;
        }

        return traits_type::to_int_type( *gptr() );
      }

    public:

      IStreambuf( char_type* buffer, size_t size )
        : inbufSize(size), inputBuffer(buffer), bytesIn()
      {
        setg( inputBuffer,  inputBuffer, inputBuffer );
      }

      uint64_t bytesRead() const { return bytesIn; }
    };

    // *******************

    class OStreambuf : virtual public std::streambuf
    {
    protected:

      const size_t     outbufSize;
      char_type* const outputBuffer;

      uint64_t bytesOut;

      void reset()
      {
        setp( outputBuffer, outputBuffer+outbufSize-1 );
      }

    private:

      virtual ssize_t write( const void* data, size_t size ) = 0;

      int sync()
      {
        bytesOut += write( pbase(), pptr() - pbase() );
        setp( outputBuffer, outputBuffer+outbufSize-1 );

        return 0;
      }

      int_type overflow( int_type c )
      {
        if( pbase() )
        {
          size_t toWrite = pptr() - pbase();

          // If there is a character to write, put it on the end of the buffer
          // (note the stream end pointer is initialised to bufferSize-1).
          //
          if( traits_type::eq_int_type( c, traits_type::eof() ) )
          {
          }
          else
          {
            char_type ch = traits_type::to_char_type( c );

            *pptr() = ch;

            ++toWrite;
          }

          bytesOut += write( pbase(), toWrite );
        }

        reset();

        return c;
      }

    public:

      OStreambuf( char_type* buffer, size_t size )
        : outbufSize(size), outputBuffer(buffer), bytesOut()
      {
        reset();
      }

      uint64_t bytesWritten() const { return bytesOut; }
    };

    struct Streambuf : IStreambuf, OStreambuf
    {
      Streambuf( char_type* inputBuffer,  size_t inbufSize,
                 char_type* outputBuffer, size_t oubufSize)
        : IStreambuf( inputBuffer,  inbufSize ),
          OStreambuf( outputBuffer, oubufSize )
      {
      }
    };
  }
}

#endif
