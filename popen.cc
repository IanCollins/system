/******************************* C++ Source File *******************************
*
*  Copyright (c) Masuma Ltd 2011.  All rights reserved.
*
*  MODULE:      utilities
*
*  DESCRIPTION: Mimic popen with stdout and stderr capture.
*
*******************************************************************************/

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include <sys/wait.h>

#include "Exception.h"
#include "FileStreambuf.h"
#include "popen.h"
#include "String.h"

namespace
{
  extern "C" void* reader( void* p )
  {
    auto* reader = static_cast<masuma::system::StreamReader*>(p);

    try
    {
      reader->run();
    }
    catch( const std::exception& e )
    {
      std::cerr << e.what() << std::endl;
    }
    return nullptr;
  }
}

namespace masuma::system
{
  void
  setNonBlocking( int fd )
  {
    int flags = fcntl( fd, F_GETFL, 0 );
    fcntl( fd, F_SETFL, flags | O_NONBLOCK );
    fcntl( fd, F_GETFL, 0 );
  }

  OutputInfo::OutputInfo()
    : pid(), in(), out(), err() {}

  OutputInfo::OutputInfo( int i, int o, int e, int p )
    : pid(p), in(i), out(o), err(e) {}

  void
  OutputInfo::setNonBlocking( int fd0, int fd1 )
  {
    system::setNonBlocking( fd0 );
    system::setNonBlocking( fd1 );
  }

  void
  OutputInfo::setNonBlocking() const
  {
    setNonBlocking( out, err );
  }

  bool
  OutputInfo::kill( int sig ) const
  {
    return ::kill( pid, sig) == 0;
  }

  bool
  OutputInfo::close( useconds_t timeout ) const
  {
    const int status = pclose( *this, timeout );

    ::close(in);
    ::close(out);
    ::close(err);

    if( WIFEXITED(status) )
    {
      int returnValue = WEXITSTATUS(status);

      return returnValue == 0;
    }
    else if( WIFSIGNALED(status) )
    {
      std::ostringstream os;
      os << "Child terminated with signal " << WTERMSIG(status);
      Throw( 0, os.str() );
    }
    else
    {
      Throw( 0, "Child terminated, no idea why.. " );
    }
  }

  OutputInfo
  popen( const std::string& cmd, bool nonBlocking )
  {
    Tokens tokens( cmd, ' ' );

    return popen( tokens, nonBlocking );
  }

  OutputInfo
  popen( const Strings& tokens, bool nonBlocking )
  {
    int outfd[2];
    int infd[2];
    int errfd[2];

    CheckSys( pipe, (outfd) );
    CheckSys( pipe, (infd) );
    CheckSys( pipe, (errfd) );

    if( nonBlocking )
    {
      OutputInfo::setNonBlocking( infd[0], errfd[0] );
    }

    int pid;

    if( (pid = fork()) == 0 )
    {
      // Child
      //
      dup2( outfd[0], STDIN_FILENO );
      dup2( infd[1],  STDOUT_FILENO );
      dup2( errfd[1], STDERR_FILENO );

      close( outfd[1] );
      close( infd[0] );
      close( errfd[0] );

      std::vector<char*> args;

      args.reserve(tokens.size());
      for( unsigned n = 0; n < tokens.size(); ++n )
      {
        args.push_back( const_cast<char*>(tokens[n].c_str()));
      }

      args.push_back(nullptr);

      const char* path = tokens[0].c_str();

      execv( path, &args[0] );

      std::cerr << strerror(errno) << std::endl;

      exit(errno);
    }
    else
    {
      // Parent
      //
      close( outfd[0] );
      close( infd[1] );
      close( errfd[1] );

      return OutputInfo( outfd[1], infd[0], errfd[0], pid );
    }
  }

  int
  pclose( const OutputInfo& info, useconds_t timeout )
  {
    int status;

    if( timeout == (useconds_t)-1 )
    {
      CheckCondition( waitpid( info.pid, &status, 0 ) == info.pid );
      return status;
    }
    else
    {
      static const useconds_t onems = 1000;

      int result = CheckSys(  waitpid, ( info.pid, &status, WNOHANG ) );

      if( result == info.pid )
      {
        return status;
      }

      useconds_t waitTime = onems;
      useconds_t waited   = 0;

      while( result != info.pid )
      {
        usleep( waitTime );

        waited += waitTime;

        if( waited > timeout )
        {
          info.kill( SIGKILL );

          return pclose( info, -1 );
        }

        waitTime *= 2;

        result = CheckSys( waitpid, ( info.pid, &status, WNOHANG ) );
      }

      return status;
    }
  }

  OutputPairReaders::OutputPairReaders( const OutputInfo& info,
                                        StreamReader& out,
                                        StreamReader& err )
    : info(info)
  {
    CheckSys( pthread_create, ( &tids[0], nullptr, reader, &out ) );
    CheckSys( pthread_create, ( &tids[1], nullptr, reader, &err ) );
  }

  int
  OutputPairReaders::close()
  {
    void* val;

    pthread_join( tids[0], &val );
    pthread_join( tids[1], &val );

    info.close();

    return 0;
  }

  void
  LogToFileStreamReader::run()
  {
    system::FileStreambuf fb( fd );

    std::istream in(&fb);
    std::string line;

    if( filename.empty() )
    {
      while( std::getline( in, line ) );
    }
    else
    {
      std::ofstream out(filename.c_str());

      while( std::getline( in, line ) )
      {
        out << line << std::endl;
      }
    }
  }

  void
  StreamStreamReader::run()
  {
    std::string line;

    system::FileStreambuf buf( fd );

    std::istream in(&buf);

    while( std::getline( in, line ) )
    {
      out << line << std::endl;
    }
  }
}
