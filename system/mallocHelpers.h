/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2009-2010.  All rights reserved.
 *
 *  MODULE:      system
 *
 *******************************************************************************/

#ifndef _utils_mallocHelpers_h_
#define _utils_mallocHelpers_h_

#include <cstdlib>
#include <vector>
#include <string>
#include <string.h>

namespace masuma
{
  namespace system
  {
    template <typename T>
    struct MallocVector : std::vector<T*>
    {
      using ItemType = std::vector<T*>;

      MallocVector() {}

      ~MallocVector()
      {
        std::for_each( ItemType::begin(), ItemType::end(), std::free );
      }

      T* add( T* p )
      {
        ItemType::push_back( p );
        return p;
      }

      T* add( size_t num = 1 )
      {
        T* p = static_cast<T*>(std::calloc( num, sizeof *p ));

        return add(p);
      }

      T* set( size_t n, size_t num = 1 )
      {
        ItemType::at(n) = static_cast<T*>(std::calloc( num, sizeof(T) ));

        return ItemType::at(n);
      }
    };

    struct CharVector : MallocVector<char>
    {
      char* addString( const std::string& s )
      {
        char* p = add( s.size()+1 );
        strcpy( p, s.c_str() );
        return p;
      }

      char* setString( size_t n, const std::string& s )
      {
        char* p = set( n, s.size()+1 );
        strcpy( p, s.c_str() );
        return p;
      }
    };
  }
}

#endif
