/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2016.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: Send a file as fast as possible.
 *
 ******************************************************************************/

#ifndef _utils_FileSender_h_
#define _utils_FileSender_h_

#include "FileCommon.h"

#include <fcntl.h>
#include <unistd.h>

namespace masuma
{
  namespace system
  {
    class FileSender : public FileCommon
    {
    public:

      using FileCommon::FileCommon;

      static void send( AutoFd to, const std::string& from, size_t );
      static void send( AutoFd to, AutoFd from, size_t );
    };

//    template <typename process>
//    class ProcessingFileSender : public FileSender
//    {
//      static void send( AutoFd to, AutoFd from, size_t fileSize )
//      {
//        Queue readyQueue;
//        Queue doneQueue;
//
//        std::unique_ptr<uint8_t> buffer {initialiseQueue( doneQueue )};
//
//        FileSender sender {from, to, readyQueue, doneQueue};
//
//        std::thread sending {sender};
//
//        sender.readFile( fileSize );
//
//        sending.join();
//      }
//
//      static void send( AutoFd to, std::string from, size_t size )
//      {
//        send( to, {open, from.c_str(), O_RDONLY}, size );
//      }
//    };
  }
}

namespace masuma
{
  namespace system
  {
    struct Summer;

    class SummingFileSender : public FileSender
    {
      friend struct Summer;

      static Summer* summer;

      void doneWith( Item );

    public:

      using FileSender::FileSender;

      static void send( AutoFd to, const std::string& from, size_t );
      static void send( AutoFd to, AutoFd from, size_t );
    };
  }
}

#endif
