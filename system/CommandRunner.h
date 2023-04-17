/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2011-2013.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: Mimic popen with stdout and stderr capture
 *
 ******************************************************************************/

#ifndef _utils_CommandRunner_h_
#define _utils_CommandRunner_h_

#include <sstream>
#include <fstream>
#include <signal.h>
#include <sys/poll.h>
#include "AutoFd.h"
#include "String.h"
#include "popen.h"
#include "Poller.h"

namespace masuma::utils
{
  struct ReaderPair
  {
    virtual bool cin( int ) = 0;
    virtual bool cerr( int ) = 0;

    virtual int  pollTimeout() const { return -1; }
    virtual bool timeoutHook() { return true; }

    virtual void clear() {}

    virtual ~ReaderPair() {}

    virtual std::string errorString() const = 0;
  };

  struct NullReader : ReaderPair
  {
    bool cin( int )  { return true; }
    bool cerr( int ) { return true; }
    std::string errorString() const { return std::string(); }
  };

  class CommandRunnerActions
  {
    ReaderPair* const readers;

    int childId;

    void process();

    struct ReaderAction : PollerAction
    {
      CommandRunnerActions& runner;

      ReaderAction( int fd, CommandRunnerActions& runner )
        : PollerAction( fd, POLLIN ), runner(runner) {}

      bool processError( uint16_t )
      {
        return false;
      }
    };

    struct OutAction : ReaderAction
    {
      OutAction( int fd, CommandRunnerActions& runner )
        : ReaderAction( fd, runner ) {}

      ~OutAction();

      bool processData( uint16_t )
      {
        return runner.readers->cin( fd );
      }
    };

    struct ErrAction : ReaderAction
    {
      ErrAction( int fd, CommandRunnerActions& runner )
        : ReaderAction( fd, runner ) {}

      bool processData( uint16_t )
      {
        return runner.readers->cerr( fd );
      }
    };

    struct ForwardAction : PollerAction
    {
      const int outFd;

      ForwardAction( int in, int out )
        : PollerAction( in, POLLIN ), outFd(out) {}

      bool processData( uint16_t );

      bool processError( uint16_t )
      {
        return false;
      }
   };

  protected:

    bool keepRunning;

    time_t timeout() const { return readers->pollTimeout(); }

    bool timeoutHook();

    virtual void run() = 0;

    virtual CommandRunnerActions& operator<<( PollerAction* action ) = 0;

  public:

    CommandRunnerActions() : readers( new NullReader ), keepRunning(true) {}
    CommandRunnerActions( ReaderPair* );

    virtual ~CommandRunnerActions()
    {
      delete readers;
    }

    bool operator()( const Strings& );
    bool operator()( const std::string& );
    bool operator()( const std::string&, int in );

    bool kill( int sig ) const { return ::kill( childId, sig ) == 0; }
    bool commandIsRunning() const { return kill( 0 ); }

    void clearReaders() { readers->clear(); }
    std::string readerError() const { return readers->errorString(); }
  };

  typedef Poller<CommandRunnerActions> CommandRunner;

  struct BufferedReader : ReaderPair
  {
    static const size_t bufSize = 16*1024;

    char buf[bufSize];
  };

  struct StreamsReader : ReaderPair
  {
    static const size_t bufSize = 64*1024;

    char buf[bufSize+1];

    std::stringstream out;
    std::stringstream err;

    bool cin( int fd );
    bool cerr( int fd );

    bool hasOutput() const { return !out.str().empty(); }
    bool hasErrors() const { return !err.str().empty(); }

    void clear();

    std::string errorString() const { return err.str(); }
  };

  struct FileOutReader : BufferedReader
  {
    std::ofstream out;

    FileOutReader( const std::string& );

    bool cin( int fd );
    bool cerr( int ) { return true; }

    void clear();

    std::string errorString() const { return ""; }
  };

  struct ForwardingReader : BufferedReader
  {
    AutoFd outFd;

    std::stringstream err;

    ForwardingReader() : outFd() {}
    ForwardingReader( AutoFd fd ) : outFd(fd) {}

    virtual void peek( ssize_t, const char* ) {}

    bool cin( int );
    bool cerr( int );

    bool hasErrors() const { return !err.str().empty(); }
    std::string errorString() const { return err.str(); }
  };

  struct StdioReader : ReaderPair
  {
    static const size_t bufSize = 16*1024;

    std::stringstream err;

    bool cin( int );
    bool cerr( int );

    std::string errorString() const { return err.str(); }
  };
}

#endif
