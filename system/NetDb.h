/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2013.  All rights reserved.
 *
 *  DESCRIPTION: NSL wrappers
 *
 ******************************************************************************/

#ifndef _utils_NetDb_h_
#define _utils_NetDb_h_

#include <netdb.h>
#include <unistd.h>
#include <string>

namespace masuma::nsl
{
  uint32_t    peerAddress( const std::string& );
  std::string peerAddressString( const std::string& );
}


#endif
