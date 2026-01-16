/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2011, all rights reserved
 *
 *  PROJECT:     Utilities
 *
 *  VERSION:     $Id: IgnoreSigPipe.h 15641 2011-11-24 05:01:23Z ian $
 *
 *  DESCRIPTION: Simple object to disable SIGPIPE
 *
 ******************************************************************************/

#ifndef IgnoreSigPipe_h
#define	IgnoreSigPipe_h

#include <signal.h>
#include "Exception.h"

namespace masuma
{
  namespace system
  {
    struct IgnoreSigPipe
    {
      struct sigaction oldact;
      struct sigaction newact;

      IgnoreSigPipe( )
      {
        CheckSys( ::sigaction, (SIGPIPE, NULL, &newact) );
        newact.sa_handler = SIG_IGN;
        newact.sa_flags = 0;
        CheckSys( ::sigaction, (SIGPIPE, &newact, &oldact) );
      }

      ~IgnoreSigPipe( )
      {
        sigaction( SIGPIPE, &oldact, NULL );
      }
    };
  }
}

#endif
