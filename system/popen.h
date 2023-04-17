/******************************* C++ Header File *******************************
*
*  Copyright (c) Masuma Ltd 2011-2012.  All rights reserved.
*
*  MODULE:      utilities
*
*  DESCRIPTION: Mimic popen with stdout and stderr capture
*
*******************************************************************************/

#ifndef _utils_popen_h_
#define _utils_popen_h_

#include <pthread.h>
#include <string>
#include <sstream>
#include <fstream>

#include "Runner.h"
#include "String.h"

namespace masuma
{
  namespace utils
  {
    class OutputInfo
    {
      friend struct OutputPairReaders;
      friend int pclose( const OutputInfo&, useconds_t );

      const int pid;

    public:

      const int in;
      const int out;
      const int err;

      OutputInfo();
      OutputInfo( int in, int out, int err, int pid );
      OutputInfo( const OutputInfo& other )
        : pid(other.pid), in(other.in), out(other.out), err(other.err) {}

      static void setNonBlocking( int, int );

      void setNonBlocking() const;

      bool close( useconds_t timeout = -1 ) const;

      bool kill( int sig ) const;

      int childId() const { return pid; }
    };

    struct StreamReader
    {
      const int fd;

      StreamReader( int fd ) : fd(fd) {}

      virtual void run() = 0;
    };

    struct LogToFileStreamReader : StreamReader
    {
      const std::string filename;

      LogToFileStreamReader( int fd,  const std::string& filename )
        : StreamReader( fd ), filename(filename) {}

      void run();
    };

    struct StreamStreamReader : StreamReader
    {
      std::ostream& out;

      StreamStreamReader( int fd, std::ostream& out )
        : StreamReader( fd ), out(out) {}

      void run();
    };

    struct OutputPairReaders
    {
      OutputInfo info;

      pthread_t tids[2]{};

      OutputPairReaders( const OutputInfo&,
                         StreamReader& outReader,
                         StreamReader& errReader );

      int close();
    };

    extern void setNonBlocking( int );

    extern OutputInfo popen( const Strings&, bool noBlocking = false );
    extern OutputInfo popen( const std::string&, bool noBlocking = false );

    static const useconds_t oneSecond = 1000000;

    extern int pclose( const OutputInfo&, useconds_t timeout = -1 );
  }
}

#endif
