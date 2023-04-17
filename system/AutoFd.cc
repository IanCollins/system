/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2013.  All rights reserved.
 *
 *  DESCRIPTION: Generic auto file descriptor
 *
 ******************************************************************************/

#include "AutoFd.h"

#include <sstream>
#include <stdexcept>

#include <fcntl.h>
#include <unistd.h>

namespace
{
  const short errorFlags( POLLNVAL | POLLERR | POLLHUP );
}

namespace masuma::utils
{
  int AutoFd::Data::active {};
  
  AutoFd::Data::~Data()
  {
    ::close(fd);
//    std::cout << "Closed " << fd << std::endl;
  }

  AutoFd::AutoFd()
    : data()
  {
  }

  AutoFd::AutoFd( int n )
    : data {new Data( n )} 
  {
  }

  AutoFd&
  AutoFd::operator=( int n )
  {
    data.reset( new Data( n ) );

    return *this;
  }

  void
  AutoFd::setNonBlocking()
  {
    int flags = CheckSys( fcntl,( data->fd, F_GETFL, 0 ) );
    CheckSys( fcntl, ( data->fd, F_SETFL, flags | O_NONBLOCK ) );
    CheckSys( fcntl, ( data->fd, F_GETFL, 0 ) );

    data->fds.fd = data->fd;
    data->blocking = false;
  }

  void
  AutoFd::setBlocking()
  {
    int flags = CheckSys( fcntl,( data->fd, F_GETFL, 0 ) );
    CheckSys( fcntl, ( data->fd, F_SETFL, flags & ~O_NONBLOCK ) );
    CheckSys( fcntl, ( data->fd, F_GETFL, 0 ) );

    data->blocking = true;
  }

  void
  AutoFd::handlePollError()
  {
    std::ostringstream out;
    out << "AutoFd poll error: " << data->fds.revents;

    throw std::runtime_error( out.str() );
  }

  int
  AutoFd::poll( short events, int timeout )
  {
    if( data->blocking )
    {
      setNonBlocking();
    }

    data->fds.revents = 0;
    data->fds.events  = events;

    if( ::poll( &data->fds, 1, timeout ) != 1 )
    {
      return -1;
    }

    if( data->fds.revents & errorFlags )
    {
      handlePollError();
    }
    else if( !(data->fds.revents & events) )
    {
      handlePollError();
    }

    return 1;
  }

  ssize_t
  AutoFd::read( void* buff, size_t size )
  {
    if( !data->blocking )
    {
      setBlocking();
    }

    return CheckSys( ::read, ( data->fd, buff, size ) );
  }

  ssize_t
  AutoFd::read( void* buff, size_t size, int pollTimeout )
  {
    if( poll( POLLIN, pollTimeout ) == 1 )
    {
      return CheckSys( ::read, ( data->fd, buff, size ) );
    }

    return -1;
  }

  ssize_t
  AutoFd::read( void* buff, size_t size, const timespec& ts )
  {
    const int pollTimeout = ts.tv_sec*1000 + ts.tv_nsec/1000000;

    return read( buff, size, pollTimeout );
  }

  ssize_t
  AutoFd::write( const void* buff, size_t size )
  {
    if( !data->blocking )
    {
      setBlocking();
    }
    
    return CheckSys( ::write, ( data->fd, buff, size ) );
  }

  ssize_t
  AutoFd::write( const void* buff, size_t size, int pollTimeout )
  {
    ssize_t nWritten = 0;

    const auto* p = static_cast<const uint8_t*>(buff);

    while( nWritten < size )
    {
      if( poll( POLLOUT, pollTimeout ) == 1 )
      {
        nWritten += CheckSys( ::write, ( data->fd, p+nWritten, size-nWritten ) );
      }
      else
      {
        errno = ETIMEDOUT;
        return -1;
      }
    }

    return nWritten;
  }

  ssize_t
  AutoFd::write( const void* buff, size_t size, const timespec& ts )
  {
    const int pollTimeout = ts.tv_sec*1000 + ts.tv_nsec/1000000;

    return write( buff, size, pollTimeout );
  }

  off_t
  AutoFd::seekSet( off_t posn )
  {
    return CheckSys( lseek, ( data->fd, posn, SEEK_SET ) );
  }

  off_t
  AutoFd::seekOffset( off_t offset )
  {
    return CheckSys( lseek, ( data->fd, offset, SEEK_CUR ) );
  }

  off_t
  AutoFd::size()
  {
    off_t result = CheckSys( lseek, ( data->fd, 0, SEEK_END ) );
    seekSet(0);
    return result;
  }
}
