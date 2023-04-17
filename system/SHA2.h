/******************************* C++ Header File *******************************
*
*  Copyright (c) Masuma Ltd 2014-2018.  All rights reserved.
*
*  MODULE:      utils
*
*******************************************************************************/

#pragma once

#include <openssl/sha.h>
#include <limits.h>
#include <array>
#include <iostream>
#include <iomanip>

namespace masuma::utils
{
  constexpr size_t sha2Size = 32;

  using SHA1_Number = std::array<uint8_t,sha2Size>;

  inline auto
  sha2( const void* buf, size_t size )
  {
    SHA_CTX   context;

    SHA1_Init( &context );

    SHA1_Update( &context, buf, size );

    SHA1_Number result;

    SHA1_Final( result.data(), &context );

    return result;
  }

  class SHA1_56Sum
  {
    SHA_CTX context;

  public:

    SHA1_56Sum()
    {
      SHA1_Init( &context );
    }

    void update( const void* buf, size_t size )
    {
      SHA1_Update( &context, buf, size );
    }

    void sum( SHA1_Number& result )
    {
      SHA1_Final( result.data(), &context );
    }

    friend std::ostream& operator<<( std::ostream& out, const SHA1_Number& sha )
    {
      for( auto n : sha )
      {
        out << std::hex << std::setfill('0') << std::setw(2) << (unsigned)n;
      }

      return out;
    }
  };
}
