/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2013.  All rights reserved.
 *
 *  DESCRIPTION: NSL wrappers
 *
 ******************************************************************************/

#include "NetDb.h"
#include "Exception.h"

#include <sstream>

#include <netdb.h>

namespace masuma::nsl
{
  class Hostent : public hostent
  {
    static const int buffSize = 1024;

    char buffer[buffSize];

  public:

    Hostent( const std::string& );
  };

  Hostent::Hostent( const std::string& peer )
  {
    int error {0};

#if defined __sun
    CheckNullError( gethostbyname_r, ( peer.c_str(),
                                       this, buffer, buffSize,
                                       &error), error );
#else
    hostent* result;
    Check( gethostbyname_r, ( peer.c_str(),
                              this, buffer, buffSize,
                              &result, &error) );
#endif
  }

  uint32_t
  peerAddress( const std::string& peer )
  {
    Hostent ent( peer );

    char* addr = ent.h_addr_list[0];

    return *(uint32_t*)addr;
  }

  std::string
  peerAddressString( const std::string& peer )
  {
    Hostent ent( peer );

    const uint8_t* addr = reinterpret_cast<const uint8_t*>(ent.h_addr_list[0]);

    char tmp[16];

    sprintf( tmp, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3] );

    return std::string(tmp);
  }
}