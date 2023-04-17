/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2013.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: Mimic popen with stdout and stderr capture.
 *
 ******************************************************************************/

#include "CommandRunner.h"
#include "popen.h"

#include <unistd.h>
#include <signal.h>
#include <iostream>

namespace masuma::system
{
  CommandRunnerActions::CommandRunnerActions( ReaderPair* p )
    : readers(p), keepRunning(true)
  {
  }

  void
  CommandRunnerActions::process()
  {
    try
    {
      run();
    }
    catch( const std::exception& e )
    {
      kill( SIGTERM );
    }
  }

  bool
  CommandRunnerActions::timeoutHook()
  {
    return readers->timeoutHook();
  }

  CommandRunnerActions::OutAction::~OutAction()
  {
    runner.keepRunning = false;
  }

  bool
  CommandRunnerActions::ForwardAction::processData( uint16_t )
  {
    static const size_t bufSize = 16*1024;

    static char buf[bufSize];

    ssize_t n = CheckSys( read, ( fd, buf, bufSize ) );

    if( n )
    {
      CheckSys( write, ( outFd, buf, n ) );
    }

    return n;
  }

  bool
  CommandRunnerActions::operator()( const std::string& cmd )
  {
    return operator()( Tokens( cmd, ' ' ) );
  }

  bool
  CommandRunnerActions::operator()( const Strings& cmd )
  {
    OutputInfo info = system::popen( cmd, true );

    childId = info.childId();

    (*this) << new OutAction( info.out, *this )
            << new ErrAction( info.err, *this );

    process();

    return info.close( system::oneSecond*10 );
  }

  bool
  CommandRunnerActions::operator()( const std::string& cmd, int in )
  {
    OutputInfo info = system::popen( cmd, true );

    childId = info.childId();

    (*this) << new OutAction( info.out, *this )
            << new ErrAction( info.err, *this )
            << new ForwardAction( in, info.in );

    process();

    return info.close( system::oneSecond*10 );
  }

  bool
  StreamsReader::cin( int fd )
  {
    ssize_t n = read( fd, buf, bufSize );

    if( n )
    {
      out.write( buf, n );
    }

    return n;
  }

  bool
  StreamsReader::cerr( int fd )
  {
    ssize_t n = read( fd, buf, bufSize );

    if( n )
    {
      err.write( buf, n );
    }

    return n;
  }

  void
  StreamsReader::clear()
  {
    out.str(std::string());
    err.str(std::string());
  }

  FileOutReader::FileOutReader( const std::string& name )
    : out( name.c_str(), std::ios::trunc )
  {
  }

  bool
  FileOutReader::cin( int fd )
  {
    ssize_t n = read( fd, buf, bufSize );

    if( n )
    {
      out.write( buf, n );
    }

    return n;
  }

  void
  FileOutReader::clear()
  {
    out << std::flush;
  }

  bool
  ForwardingReader::cin( int fd )
  {
    ssize_t n = read( fd, buf, bufSize );

    if( n )
    {
      outFd.write( buf, n );
    }

    return n;
  }

  bool
  ForwardingReader::cerr( int fd )
  {
    ssize_t n = read( fd, buf, bufSize );

    if( n )
    {
      err.write( buf, n );
    }

    return n;
  }

  bool
  StdioReader::cin( int fd )
  {
    static char buf[bufSize+1];

    ssize_t n = read( fd, buf, bufSize );

    if( n )
    {
      std::cout.write( buf, n );
    }

    return n;
  }

  bool
  StdioReader::cerr( int fd )
  {
    static char buf[bufSize+1];

    ssize_t n = read( fd, buf, bufSize );

    if( n )
    {
      std::cerr.write( buf, n );
      err.write( buf, n );
    }

    return n;
  }
}