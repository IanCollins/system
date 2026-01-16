/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2013.  All rights reserved.
 *
 *  DESCRIPTION: Generic auto file descriptor
 *
 ******************************************************************************/

#ifndef _utils_SocketAutoFd_h_
#define _utils_SocketAutoFd_h_

#include "AutoFd.h"
#include "NetDb.h"
#include <chrono>

#ifdef HAVE_WINSOCK_H
# include <winsock2.h>
typedef int socklen_t;
#else
# include <sys/socket.h>
# include <netinet/in.h>
#endif

namespace masuma::system
{
  using namespace std::chrono_literals;
  
  struct SockaddrIn : sockaddr_in
  {
    SockaddrIn();

    SockaddrIn( const uint32_t& peer, const int16_t& port );

    explicit SockaddrIn( const sockaddr_in& );

    SockaddrIn( const SockaddrIn& ) = default;
    SockaddrIn& operator=( const SockaddrIn& ) = delete;

    operator const sockaddr*() const
    {
      return reinterpret_cast<const sockaddr*>(this);
    }

    operator sockaddr*()
    {
      return reinterpret_cast<sockaddr*>(this);
    }

    [[nodiscard]] static socklen_t size() { return sizeof(sockaddr_in); }

    friend std::ostream& operator<<( std::ostream&, const SockaddrIn& );
  };

  class SocketAutoFd : public AutoFd
  {
    SockaddrIn connectPrepare( const std::string&, int16_t );

    std::string peerName;

  public:
    
    static constexpr std::chrono::milliseconds noTimeout {-1ms};
    
    SocketAutoFd() : AutoFd() {}
    explicit SocketAutoFd( int n ) : AutoFd(n) {}
    SocketAutoFd( int domain, int type, int protocol = 0 );
    explicit SocketAutoFd( const AutoFd& f ) : AutoFd(f) {}

    SocketAutoFd& operator=( int n ) override { AutoFd::operator=( n ); return *this; }

    SocketAutoFd accept();
    SocketAutoFd accept( SockaddrIn& );

    void connect( const std::string&, int16_t );
    void connect( const std::string&, int16_t, std::chrono::milliseconds );
    void bind( int16_t, bool reusable = true );
    void bind( const SockaddrIn&, bool reusable = true );
    void listen( int backlog = 1);
    void shutdown( int how = SHUT_RDWR );
//    void close();
    [[nodiscard]] bool isConnected() const { return data && data->connected; }

    ssize_t setWindSize( int windowSize, int opt, const char* text = nullptr );
  };

  struct TcpSocketAutoFd : SocketAutoFd
  {
    using SocketAutoFd::operator=;
    TcpSocketAutoFd();
  };
}

#endif
