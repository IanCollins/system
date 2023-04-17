/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2016, all rights reserved
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: File sender
 *
 ******************************************************************************/

#include "FileCommon.h"

#include <memory>
#include <utility>

#include <unistd.h>

namespace masuma::utils
{
  const size_t FileCommon::bufferSize = 8*1024*1024;

  uint8_t*
  FileCommon::initialiseQueue( Queue& queue )
  {
    constexpr size_t items = 8;

    CheckCondition( queue.empty() );

    uint8_t* buffer {new uint8_t[bufferSize*items]};

    for( size_t n = 0; n < items; ++n )
    {
      queue.post( Item {buffer+bufferSize*n,0} );
    }

    return buffer;
  }

  void
  FileCommon::operator()()
  {
    while( true )
    {
      Item item;

      readyQueue.pend( item );

      if( item.first )
      {
        to.write( item.first, item.second );

        doneQueue.post(item);
      }
      else
      {
        return;
      }
    }
  }

  void
  FileCommon::doneWith( Item item )
  {
    readyQueue.post( item );
  }

  void
  FileCommon::readFile( size_t fileSize )
  {
    size_t toRead {fileSize};

    while( toRead )
    {
      Item item;

      doneQueue.pend( item );

      item.second = std::min(toRead,bufferSize);

      readToItem( from, item );

      toRead -= item.second;

      doneWith( item );
    }

    doneWith( {nullptr,0} );
  }

  void
  FileCommon::readToItem( AutoFd in, Item& item )
  {
    size_t thisRead {item.second};
    size_t offset   {0};

    while( thisRead )
    {
      auto n = in.read( item.first+offset, thisRead );

      offset   += n;
      thisRead -= n;
    }
  }

  void
  FileCommon::readToBuffer( AutoFd from, uint8_t* buffer, size_t size )
  {
    Item item {buffer,size};

    readToItem( std::move(from), item );
  }

  void
  FileCommon::copyShortFile( AutoFd from, AutoFd to, size_t fileSize )
  {
    std::unique_ptr<uint8_t> buffer {new uint8_t[fileSize]};

    Item item {buffer.get(),fileSize};

    readToItem( std::move(from), item );

    to.write( item.first, item.second );
  }
}
