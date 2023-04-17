/******************************* C++ Header File *******************************
*
*  Copyright (c) Masuma Ltd 2012
*
*  MODULE:      utils
*
*  DESCRIPTION: Streamable types.
*
*******************************************************************************/

#ifndef _utils_Streamable_h_
#define _utils_Streamable_h_

#include <iostream>

namespace masuma
{
  namespace utils
  {
    struct Streamable
    {
      virtual void stream( std::ostream& ) const = 0;

      friend std::ostream& operator<<( std::ostream& out, const Streamable& t )
      {
        t.stream( out );
        return out;
      }
    };
  }
}

#endif
