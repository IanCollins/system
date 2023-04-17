/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2012.  All rights reserved.
 *
 *  MODULE:      Utils
 *
 *  Created:     July 25, 2013, 4:32 PM
 *
 ******************************************************************************/

#pragma once

#include <string>
#include "Time.h"

namespace masuma::utils
{
  std::string timestamp( const tm& now );

  std::string timestamp( time_t now );

  std::string timestamp();
}

