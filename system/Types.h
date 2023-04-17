/******************************* C++ Header File *******************************
*
*  Copyright (c) Masuma Ltd 2013.  All rights reserved.
*
*  MODULE:      utils
*
*******************************************************************************/

#pragma once

#if defined HAVE_JSON
# include <json/JSON.h>
#endif

#include <vector>

namespace masuma::utils
{
  template <typename T>
  struct VectorWithJSON : std::vector<T>
  {
    typedef std::vector<T> Base;

    using typename Base::const_iterator;

#if defined HAVE_JSON
    json::Array toJSONArray() const
    {
      json::Array array;

      for( typename Base::const_iterator i = Base::begin(); i != Base::end(); ++i )
      {
        array << *i;
      }

      return array;
    }
    void fromJSONArray( json::Array array )
    {
      for( json::Array::const_iterator i = array.begin(); i != array.end(); ++i )
      {
        Base::push_back( *i );
      }
    }
#endif

    VectorWithJSON() : std::vector<T>() {}

    VectorWithJSON( const std::vector<T>& v ) : std::vector<T>(v) {}

    VectorWithJSON& operator=( const std::vector<T>& v )
    {
      std::vector<T>::operator=(v);
      return *this;
    }

    friend VectorWithJSON& operator<<( VectorWithJSON& v, const T& t )
    {
      v.push_back(t);
      return v;
    }
  };

  typedef VectorWithJSON<uint16_t> Shorts;
  typedef VectorWithJSON<uint32_t> Ints;
}
