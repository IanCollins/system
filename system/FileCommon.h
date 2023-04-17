/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2016.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: Send a file as fast as possible.
 *
 ******************************************************************************/

#ifndef _utils_FileCommon_h_
#define _utils_FileCommon_h_

#include "MessageQueue.h"
#include "AutoFd.h"

namespace masuma
{
  namespace utils
  {
    class FileCommon
    {
    public:
      
      using Item  = std::pair<uint8_t*,size_t>;
      using Queue = StaticMessageQueue<Item>;

    protected:

      AutoFd& from;
      AutoFd& to;

      Queue& readyQueue;
      Queue& doneQueue;

     static uint8_t* initialiseQueue( Queue& );

      void readFile( size_t );

      virtual void doneWith( Item );

      static void readToItem( AutoFd, Item& );

      static void copyShortFile( AutoFd, AutoFd, size_t );

    public:

      FileCommon( AutoFd& from, AutoFd& to, Queue& ready, Queue& done )
        : from {from}, to {to}, readyQueue {ready}, doneQueue {done} {}

      FileCommon( const FileCommon& ) = default;

      static const size_t bufferSize;

      static void readToBuffer( AutoFd, uint8_t*, size_t );

      void operator()();
    };
  }
}

#endif
