/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2011-2013.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: Poll a collection of file descriptors
 *
 ******************************************************************************/

#ifndef _utils_Poller_h_
#define _utils_Poller_h_

#include <vector>
#include <sys/poll.h>
#include <memory>

namespace masuma::utils
{
  struct PollerAction
  {
    const int      fd;
    const uint16_t events;

    PollerAction( int fd, uint16_t events ) : fd(fd), events(events) {}

    virtual ~PollerAction() {}

    virtual bool processData( uint16_t ) = 0;
    virtual bool processError( uint16_t ) = 0;

    enum EventType { Data, Error };

    bool process( uint16_t revent, EventType type )
    {
      return type == Data ? processData( revent ) : processError( revent );
    }
  };

  template <typename Processor>
  class Poller : public Processor
  {
    struct Fd : pollfd
    {
      Fd( const PollerAction& action )
      {
        fd     = action.fd;
        events = action.events;
      }
    };

    using ActionPtr = std::shared_ptr<PollerAction>;
    using Fds       = std::vector<Fd>;
    using Actions   = std::vector<ActionPtr>;

    Fds     fds;
    Actions actions;

    bool poll()
    {
      for( unsigned n = 0; n < fds.size(); fds[n++].revents = 0 );

      bool found = false;
      bool keepWaiting = true;

      do
      {
        found = CheckSys( ::poll, ( &fds[0], fds.size(), Processor::timeout() ) ) > 0;

        if( !found )
        {
          keepWaiting = Processor::timeoutHook();
        }
      }
      while( !found && keepWaiting );

      return found;
    }

    bool process()
    {
      static const uint16_t errorFlags( POLLNVAL | POLLERR | POLLHUP );

      typename Fds::iterator fd     = fds.begin();
      Actions::iterator      action = actions.begin();

      while( fd != fds.end() )
      {
        if( fd->revents & fd->events )
        {
          if( !(*action)->processData( fd->revents ) )
          {
            fd     = fds.erase( fd );
            action = actions.erase( action );
            continue;
          }
        }
        ++fd;
        ++action;
      }

      fd     = fds.begin();
      action = actions.begin();

      while( fd != fds.end() )
      {
        if( fd->revents & errorFlags )
        {
          if( !(*action)->processError( fd->revents ) )
          {
            fd     = fds.erase( fd );
            action = actions.erase( action );
            continue;
          }
        }
        ++fd;
        ++action;
      }

      return Processor::keepRunning && !fds.empty();
    }

  public:

    Poller() : Processor() {}

    template <typename T0> explicit Poller( T0& t0 )
      : Processor( t0 ) {}

    template <typename T0> explicit Poller( T0* t0 )
      : Processor( t0 ) {}

    template <typename T0, typename T1> Poller( T0& t0, T1& t1 )
      : Processor( t0, t1 ) {}

    bool once()
    {
      return poll() ? process() : false;
    }

    void run()
    {
      while( once() );
    }

    Poller& operator<<( PollerAction* action )
    {
      fds.push_back( Fd(*action) );
      actions.push_back( ActionPtr(action) );

      return *this;
    }
  };
}

#endif
