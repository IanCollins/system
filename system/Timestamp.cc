/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2012.  All rights reserved.
 *
 *  MODULE:      Utils
 *
 *  Created:     July 25, 2013, 4:32 PM
 *
 ******************************************************************************/

#include "Timestamp.h"
#include "Exception.h"

#include <sstream>

#include <cstdio>

namespace masuma::utils
{
  std::string
  timestamp( const tm& now )
  {
    const size_t bufSize = 32;

    char buf[bufSize];

    strftime( buf, bufSize, "%d-%m-%YT%H:%M:%S", &now );

    return buf;
  }

  std::string
  timestamp( time_t now )
  {
    tm timeNow{};

    CheckNull( localtime_r, ( &now, &timeNow ) );

    return timestamp( timeNow );
  }

  std::string
  timestamp()
  {
    timespec tp = now<timespec>();

    tm timeNow{};

    CheckNull( localtime_r, ( &tp.tv_sec, &timeNow ) );

    char buf[16];

    sprintf( buf, ".%03ld", tp.tv_nsec/1000000 );

    return timestamp( timeNow )+buf+' ';
  }
}
