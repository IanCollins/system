/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2013.  All rights reserved.
 *
 *  DESCRIPTION: Generic auto file descriptor
 *
 ******************************************************************************/

#include "SocketAutoFd.h"
#include "Log.h"
#include "Time.h"

#include "Exception.h"
#include <sstream>

#include <netinet/in.h>
#include <arpa/inet.h>

namespace masuma::utils
{
  SockaddrIn::SockaddrIn() : sockaddr_in {}
  {
    sin_family = sin_port = sin_addr.s_addr = 0;
  }

  SockaddrIn::SockaddrIn( const uint32_t& peer, const int16_t& port ) : sockaddr_in {}
  {
    sin_family      = PF_INET;
    sin_port        = htons(port);
    sin_addr.s_addr = peer;
  }

  SockaddrIn::SockaddrIn( const sockaddr_in& in ) : sockaddr_in {}
  {
    sin_family = in.sin_family;
    sin_port   = in.sin_port;
    sin_addr   = in.sin_addr;
  }

  std::ostream&
  operator<<( std::ostream& out, const SockaddrIn& in )
  {
    return out << inet_ntoa(in.sin_addr) << ':' << ntohs(in.sin_port);
  }

  SocketAutoFd::SocketAutoFd( int domain, int type, int protocol )
    : AutoFd {::socket, domain, type, protocol}
  {
  }

  SockaddrIn
  SocketAutoFd::connectPrepare( const std::string& peer, int16_t port )
  {
    CheckCondition( !data->connected );

    SockaddrIn in {nsl::peerAddress(peer), port};

    std::ostringstream out;

    out << in;

    peerName = out.str();

    return in;
  }

  void
  SocketAutoFd::connect( const std::string& peer, int16_t port )
  {
    SockaddrIn in {connectPrepare( peer, port )};

    CheckSysM( ::connect, ( data->fd, in, in.size() ), peerName );

    data->connected = true;
  }

  void
  SocketAutoFd::connect( const std::string& peer, int16_t port, 
                         std::chrono::milliseconds timeout )
  {
    if( timeout == noTimeout )
    {
      connect( peer, port );
      return;
    }
    
    SockaddrIn in {connectPrepare( peer, port )};

    setNonBlocking();
      
    const auto before {now<long long>()};

    int result {::connect( data->fd, in, SockaddrIn::size() )};

    if( result < 0 )
    {
      CheckCondition( errno == EINPROGRESS );

      if( poll( POLLOUT, (int)timeout.count() ) != 1 )
      {
        const auto took {since(before)};
        
        std::ostringstream out("Connection Timeout ");
        out << double(took)/1'000'000.0 << "mS errno " << strerror(errno);
        
        throw std::runtime_error {out.str()};
      }
              
      try
      {
        //std::cout << "try again " << data->fd << std::endl;

        result = ::connect( data->fd, in, SockaddrIn::size() );
     
        //std::cout << "result " << result << " errno " << errno << std::endl;
       
        CheckConditionM( result == 0 || errno == EISCONN, strerror(errno) );

        setBlocking();
      }
      catch( const utils::Exception& e )
      {
        setBlocking();

        throw e;
      }
    }

    data->connected = true;
  }

  void
  SocketAutoFd::bind( const SockaddrIn& in, bool reusable )
  {
    if( reusable )
    {
      int one(1);
      CheckSys( setsockopt, ( data->fd, SOL_SOCKET, SO_REUSEADDR,
                              &one, sizeof(int) ) );
    }

    CheckSys( ::bind, ( data->fd, in, in.size() ) );
  }

  void
  SocketAutoFd::bind( int16_t port, bool reusable )
  {
    bind( SockaddrIn( INADDR_ANY, port ), reusable );
  }

  void
  SocketAutoFd::listen( int backlog )
  {
    CheckSys( ::listen, ( data->fd, backlog ) );
  }

  SocketAutoFd
  SocketAutoFd::accept()
  {
    SockaddrIn other;
    socklen_t  otherSize(SockaddrIn::size());

    return SocketAutoFd( CheckSys( ::accept, (data->fd, other, &otherSize ) ) );
  }

  SocketAutoFd
  SocketAutoFd::accept( SockaddrIn& other )
  {
    socklen_t otherSize(SockaddrIn::size());

    return SocketAutoFd( CheckSys( ::accept, (data->fd, other, &otherSize ) ) );
  }

  ssize_t
  SocketAutoFd::setWindSize( int windowSize, int opt, const char* text )
  {
    socklen_t length(sizeof(windowSize));
    int wind;

    CheckSys( getsockopt, ( data->fd, SOL_SOCKET, opt, &wind, &length ) );

    if( text ) std::cout << text << " was " << wind << std::endl;

    CheckSys( setsockopt, ( data->fd, SOL_SOCKET, opt, &windowSize, length ) );
    CheckSys( getsockopt, ( data->fd, SOL_SOCKET, opt, &wind, &length ) );

    if( text ) std::cout << text << " now " << wind << std::endl;

    return wind;
  }

  void 
  SocketAutoFd::shutdown( int how )
  {
    ::shutdown( data->fd, how );
  }
  
//  void
//  SocketAutoFd::close()
//  {
//    ::close( data->fd );
//    data->connected = false;
//  }

  TcpSocketAutoFd::TcpSocketAutoFd()
    : SocketAutoFd( CheckSys(socket, (PF_INET, SOCK_STREAM, 0) ) )
  {
  }
}
