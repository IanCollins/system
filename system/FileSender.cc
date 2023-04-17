/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2016, all rights reserved
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: File sender
 *
 ******************************************************************************/

#include "FileSender.h"

#include <thread>
#include <memory>
#include <iomanip>
#include <utility>

namespace masuma::utils
{
  void
  FileSender::send( AutoFd to, AutoFd from, size_t fileSize )
  {
    if( fileSize < bufferSize )
    {
      copyShortFile( from, to, fileSize );
    }
    else
    {
      Queue readyQueue;
      Queue doneQueue;

      std::unique_ptr<uint8_t> buffer {initialiseQueue( doneQueue )};

      FileSender sender {from, to, readyQueue, doneQueue};

      std::thread sending {sender};

      sender.readFile( fileSize );

      sending.join();
    }
  }

  void
  FileSender::send( AutoFd to, const std::string& file, size_t fileSize )
  {
    AutoFd from {open, file.c_str(), O_RDONLY};

    send( std::move(to), from, fileSize );
  }
}

#include "MD5.h"

namespace masuma::utils
{
  struct Summer
  {
    MD5Hash sum;

    using Queue = FileCommon::Queue;
    using Item  = FileCommon::Item;

    Queue& itemQueue;
    Queue& readyQueue;

    Summer( Queue& itemQueue, Queue& readyQueue )
      : itemQueue {itemQueue}, readyQueue {readyQueue} {}

    Summer( const Summer& ) = default;

    void operator()()
    {
      Item item {nullptr,42};

      while( item.second )
      {
        itemQueue.pend( item );

        if( item.second )
        {
          sum.update( item.first, item.second );
        }
        else
        {
          MD5Sum result;

          sum.sum( result );

          std::cout << result << std::endl;
        }

        readyQueue.post(item);
      }
    }

    void post( const Item& item ) { itemQueue.post( item ); }
  };

  Summer* SummingFileSender::summer {nullptr};

  void
  SummingFileSender::doneWith( Item item )
  {
    summer->post( item );
  }

  void
  SummingFileSender::send( AutoFd to, AutoFd from, size_t fileSize )
  {
    Queue readyQueue;
    Queue doneQueue;
    Queue summerQueue;

    std::unique_ptr<uint8_t> buffer {initialiseQueue( doneQueue )};

    Summer sum {summerQueue, readyQueue};

    summer = &sum;

    std::thread summing {sum};

    SummingFileSender sender {from, to, readyQueue, doneQueue};

    std::thread sending {sender};

    sender.readFile( fileSize );

    sending.join();
    summing.join();
  }

  void
  SummingFileSender::send( AutoFd to, const std::string& file, size_t fileSize )
  {
    AutoFd from {open, file.c_str(), O_RDONLY};

    send( to, from, fileSize );
  }
}
