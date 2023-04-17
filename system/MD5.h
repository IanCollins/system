/******************************* C++ Header File *******************************
*
*  Copyright (c) Masuma Ltd 2012.  All rights reserved.
*
*  MODULE:      utils
*
*******************************************************************************/

#ifndef _utils_MD5_h_
#define _utils_MD5_h_

#include <openssl/md5.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

namespace masuma
{
  namespace utils
  {
    inline char toHex( uint8_t n ) { return n < 10 ? '0'+n : ('a'-10)+n; }

    union MD5Sum
    {
      uint64_t bits[2];
      uint8_t  sum[16];

      bool operator==( const MD5Sum& other ) const
      {
        return (bits[0] == other.bits[0] && bits[1] == other.bits[1]);
      }

      bool operator<( const MD5Sum& other ) const
      {
        return (bits[0] == other.bits[0] ?
                bits[1] < other.bits[1] :
                bits[0] < other.bits[0]);
      }

      operator std::string() const
      {
        std::ostringstream os;

        os << *this;

        return os.str();
      }

      friend std::ostream& operator<<( std::ostream& out, const MD5Sum& sum )
      {
        for( int n = 0; n < 16; ++n )
        {
          out << toHex((sum.sum[n]&0xf0)/16) << toHex(sum.sum[n]&0xf);
        }

        return out;
      }

      friend std::istream& operator>>( std::istream& in, MD5Sum& sum )
      {
        return in >> std::hex >> sum.bits[0] >> sum.bits[1] >> std::dec;
      }

      static MD5Sum hash( const uint8_t* start, size_t size )
      {
        MD5Sum sum = {};

        MD5( start, size, sum.sum );

        return sum;
      }
    };

    class MD5Hash
    {
      MD5_CTX context;

    public:

      MD5Hash()
      {
        MD5_Init( &context );
      }

      void update( const void* buf, size_t size )
      {
        MD5_Update( &context, buf, size );
      }

      void sum( MD5Sum& result )
      {
        MD5_Final( result.sum, &context );
      }
    };
  }
}

#endif
