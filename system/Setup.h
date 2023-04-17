/******************************* C++ Header File *******************************
*
*  Copyright (c) Masuma Ltd 2012.  All rights reserved.
*
*  MODULE:      utils
*
*******************************************************************************/

#ifndef _utils_Setup_h_
#define _utils_Setup_h_

#include <string>
#include <vector>

#include "Log.h"
#include "String.h"

namespace masuma::utils
{
  template <typename T> bool isSet( const T& t ) { return t != 0; }

  template <> inline bool isSet( const std::string& s ) { return !s.empty(); }
  template <> inline bool isSet( const Strings& s )     { return !s.empty(); }
  template <> inline bool isSet( const Shorts& s )      { return !s.empty(); }
  template <> inline bool isSet( const Ints& s )        { return !s.empty(); }
}

#endif
