/******************************* C++ Header File *******************************
 *
 *  PROJECT:     Utilities
 *
 *  MODULE:      utils
 *
 ******************************************************************************/
#ifndef _utils_StreamCommandRunner_h_
#define _utils_StreamCommandRunner_h_

#include "CommandRunner.h"
#include "Log.h"

#include <string>
#include <stdexcept>

namespace masuma
{
  namespace utils
  {
    struct CommandFailedException : std::runtime_error
    {
      CommandFailedException( const std::string& command,
                              const std::string& error )
        : std::runtime_error( command + " failed " + error ) {}
    };

    class StreamCommandRunner
    {
      CommandRunner runner;

    public:

      StreamCommandRunner()
        : runner( new StreamsReader ) {}

      StreamCommandRunner( ReaderPair* rp )
        : runner( rp ) {}

      StreamCommandRunner( StreamCommandRunner& other )
        : runner( other.runner ) {}

      virtual ~StreamCommandRunner() = default;

      bool CheckResult( bool status, const std::string& command, bool mustWork )
      {
        Log(Log::Debug) << command << std::endl;

        if( !status )
        {
          if( mustWork )
          {
            throw CommandFailedException( command, runner.readerError() );
          }
          else
          {
            Log(Log::Debug) << runner.readerError();
            return false;
          }
        }

        return true;
      }

      bool run( const std::string& command, bool mustWork )
      {
        runner.clearReaders();

        return CheckResult( runner( command ), command, mustWork );
      }

      bool run( const std::string& command, int inFd, bool mustWork )
      {
        runner.clearReaders();

        return CheckResult( runner( command, inFd ), command, mustWork );
      }

      bool kill( int sig ) const { return runner.kill( sig ); }

      bool commandIsRunning() const {return runner.commandIsRunning(); }

      std::string readerError() const { return runner.readerError(); }
    };
  }
}

#endif
