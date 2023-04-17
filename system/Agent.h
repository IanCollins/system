/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2008-2013.  All rights reserved.
 *
 *  DESCRIPTION: Generic TCP server agent
 *
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <cstdlib>
#include "Exception.h"
#include "SocketAutoFd.h"

namespace masuma::system
{
  class Agent
  {
    bool runForever;

    void outer();

    virtual void inner()
    {
      process();
    }

  protected:

    const uint16_t port;
    TcpSocketAutoFd fd;
    SocketAutoFd readSocket;

    virtual void process() = 0;
    [[nodiscard]] virtual std::string name() const = 0;
    virtual void onConnection( const SockaddrIn& ) {}

  public:

    explicit Agent( uint16_t );
    explicit Agent( const SockaddrIn& );

    Agent( const Agent& ) = delete;
    Agent& operator=( const Agent& ) = delete;

    virtual ~Agent() = default;

    void acceptConnection();

    void run( bool forever );
  };
}
