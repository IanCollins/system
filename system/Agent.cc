/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2012.  All rights reserved.
 *
 *  MODULE:      Utilities
 *
 *  Created:     October 2, 2012, 11:14 AM
 *
 ******************************************************************************/

#include "Agent.h"
#include "Log.h"

#include <stdexcept>

namespace masuma::utils
{
  void
  Agent::acceptConnection()
  {
    fd.listen(0);

    SockaddrIn peer;

    readSocket = fd.accept( peer );

    Log(Log::Debug) << name() << ": connection to " << port <<  " from " << peer << '\n';

    onConnection( peer );
  }

  void
  Agent::outer()
  {
    acceptConnection();

    inner();

    readSocket.shutdown();
  }

  Agent::Agent( uint16_t p )
    : runForever(), port(p), readSocket(-1)
  {
    fd.bind(port);
  }

  Agent::Agent( const SockaddrIn& in )
    :  runForever(), port(ntohs(in.sin_port)), readSocket(-1)
  {
    fd.bind( in );
  }

  void
  Agent::run( bool forever )
  {
    DebugLog() << name() << " running " << (forever ? "forever" : "once") << '\n';

    runForever = forever;

    do
    {
      outer();
    }
    while( runForever );

    DebugLog() << name() << " done\n";
 }
}
