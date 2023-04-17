/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2016, all rights reserved
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: File receiver
 *
 ******************************************************************************/

#include "FileReceiver.h"

#include <thread>
#include <memory>
#include <utility>

#include <fcntl.h>
#include <unistd.h>

namespace masuma::utils
{
  void
  FileReceiver::receive( AutoFd from, AutoFd to, size_t fileSize )
  {
    if( fileSize < bufferSize )
    {
      copyShortFile( from, to, fileSize );
    }
    else
    {
      Queue readyQueue;
      Queue doneQueue;

      std::unique_ptr<uint8_t> buffer {initialiseQueue( readyQueue )};

      FileReceiver receiver {from, to, readyQueue, doneQueue};

      std::thread receiving {receiver};

      receiver.readFile( fileSize );

      receiving.join();
    }
  }

  void
  FileReceiver::receive( AutoFd from, const std::string& file, size_t fileSize )
  {
    AutoFd to {open, file.c_str(), O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU|S_IRWXG};

    receive( std::move(from), to, fileSize );
  }
}
