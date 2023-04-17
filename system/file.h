/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2010-2014, all rights reserved
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: Filesystem odds and ends
 *
 ******************************************************************************/

#ifndef _utils_file_h_
#define _utils_file_h_

#include "Stat.h"
#include "String.h"

namespace masuma
{
  namespace utils
  {
    inline bool
    fileExists( const char* file )
    {
      struct stat buf;

      return (0 == lstat( file, &buf ));
    }

    inline bool
    fileExists( const std::string& file )
    {
      return fileExists(file.c_str() );
    }

    struct Filename
    {
      std::string name;
      std::string extension;
      std::string path;

      Filename( const std::string filename )
      {
        std::string tmp;

        split( filename, filename.find_last_of('.'), tmp, extension );
        split( tmp, filename.find_last_of('/'), path, name );
      }
    };
  }
}

#endif
