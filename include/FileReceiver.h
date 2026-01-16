/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2016.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: Receive a file as fast as possible.
 *
 ******************************************************************************/

#ifndef _utils_FileReceiver_h_
#define _utils_FileReceiver_h_

#include "FileCommon.h"

namespace masuma
{
  namespace system
  {
    class FileReceiver : public FileCommon
    {
    public:

      using FileCommon::FileCommon;

      static void receive( AutoFd from, const std::string&, size_t );
      static void receive( AutoFd from, AutoFd to, size_t );
    };
  }
}

#endif
