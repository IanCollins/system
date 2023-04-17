/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2013.  All rights reserved.
 *
 *  DESCRIPTION: Generic auto file descriptor
 *
 ******************************************************************************/

#ifndef _utils_AutoFd_h_
#define _utils_AutoFd_h_

#include <poll.h>

#include <string>
#include <memory>
#include "Exception.h"

namespace masuma::utils
{
  class AutoFd
  {
    struct Data
    {
      const int fd;

      static int active;

      bool   blocking  {true};
      bool   connected {false};
      pollfd fds;

      Data( int n ) : fd {n} {}
      ~Data();
      Data( const Data& ) = delete;
      Data& operator=( const Data& ) = delete;
    };

    void handlePollError();
    
  protected:

    std::shared_ptr<Data> data;

    void setNonBlocking();
    void setBlocking();
    int poll( short, int );

  public:

    AutoFd();

    explicit AutoFd( int );

    template <typename Op, typename T, typename... Args>
    AutoFd( Op op, const T& t, Args... args )
      : AutoFd {CheckSys(op, (t, args...))} {}

    AutoFd( const AutoFd& ) = default;
    AutoFd& operator=( const AutoFd& ) = default;
    AutoFd( AutoFd&& ) = default;
    AutoFd& operator=( AutoFd&& ) = default;

    virtual AutoFd& operator=( int );

    virtual ~AutoFd() = default;;

    int get() const { return data->fd; }

    bool isBlocking() const { return data->blocking; }

//    void reset();

    ssize_t read( void*, size_t );
    ssize_t read( void*, size_t, int ms );
    ssize_t read( void*, size_t, const timespec& );

    ssize_t write( const void*, size_t );
    ssize_t write( const void*, size_t, int ms );
    ssize_t write( const void*, size_t, const timespec& );

    off_t seekSet( off_t posn );
    off_t seekOffset( off_t offset );

    off_t size();
  };
}

#endif
