/******************************* C++ Header File *******************************
*
*  Copyright (c) Masuma Ltd 2011.  All rights reserved.
*
*  MODULE:      utilities
*
*  DESCRIPTION: An object that can be passed to a thread and run.
*
*******************************************************************************/

#ifndef _utils_Runner_h_
#define _utils_Runner_h_

#include "Log.h"
#include "IgnoreSigPipe.h"

namespace masuma
{
  namespace utils
  {
    struct RunSomething
    {
      bool keepRunning {true};
      bool joinable    {true};

      RunSomething() = default;
      virtual  ~RunSomething() = default;

      explicit RunSomething( bool b ) : keepRunning {b} {}

      RunSomething( const RunSomething& ) = delete;
      RunSomething& operator=( const RunSomething& ) = delete;

      virtual void onStart() {}
      virtual void onCompletion() {}

      virtual void action() = 0;

      void run()
      {
        do
        {
          try
          {
            action();
          }
          catch( const std::exception& e )
          {
            WarnLog() << e.what() << std::endl;
          }
        }
        while( keepRunning );
      }
    };

    extern "C" void* runner( void* );

    inline void* runner( void* p )
    {
      RunSomething* runner = static_cast<RunSomething*>(p);

      IgnoreSigPipe ignore;

      runner->joinable = true;
      runner->run();
      runner->onCompletion();

      return NULL;
    }

    template <typename Op>
    class Runner : public Op
    {
      friend void* runner( void* );

      pthread_t tid {0};

      void joinSafe() noexcept
      {
        void* p;
        pthread_join( tid, &p );
        Op::joinable = false;
      }

    public:

      Runner() : Op() {}
      Runner( const Runner& ) = delete;

      template <typename ...Args>
      explicit Runner( Args&&... args ) : Op {args...} {}

      void start()
      {
        Check( pthread_create, ( &tid, NULL, runner, this ) );
        Op::onStart();
      }

      void detach()
      {
        Check( pthread_detach, (tid) );
        Op::joinable = false;
      }

      bool isRunning() const
      {
        return (tid > 0) && (pthread_kill( tid, 0 ) == 0);
      }

      void cancel()
      {
        pthread_cancel( tid );
      }

      void join()
      {
        CheckCondition(Op::joinable);

        joinSafe();
      }

      void waitForExit( uint32_t ms )
      {
        Op::waitForExit( ms );
      }

      ~Runner() noexcept
      {
        if(Op::joinable)
          join();
      }
    };
  }
}

#endif

