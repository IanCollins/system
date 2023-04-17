/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2005-2013
 *
 *  PROJECT:     Utilities
 *
 *  DESCRIPTION: A fixed range pointer
 *
 ******************************************************************************/

#ifndef _utils_CircularPointer_h_
#define _utils_CircularPointer_h_

#include <stddef.h>
#include <array>

namespace masuma
{
  namespace system
  {
    template <typename T, int Size, typename Iterator = T*>
    class CircularPointer
    {
      Iterator const  base;
      Iterator const  end;
      Iterator        pointer;

    public:

      CircularPointer( T* b, T* p = NULL )
        : base( b ), end( b+Size ), pointer( p ? p : b ) {}

      CircularPointer( std::array<T,Size>& a )
        : base {a.begin()}, end {a.end()}, pointer {base} {}

      T& operator*() const { return *pointer; }

      T* operator->() const { return pointer; }

      CircularPointer& operator++()
      {
        ++pointer;
        pointer = (pointer == end) ? base : pointer;
        return *this;
      }

      CircularPointer operator++(int)
      {
        CircularPointer p( *this );
        operator++();
        return p;
      }

      CircularPointer& operator+=( int n )
      {
        pointer += (n%Size);
        return *this;
      }

      CircularPointer& operator--()
      {
        pointer = (pointer == base) ? end-1 : --pointer;
        return *this;
      }

      CircularPointer operator--(int)
      {
        CircularPointer p( *this );
        operator--();
        return p;
      }

      CircularPointer& operator-=( int n )
      {
        const size_t in   = pointer-base;
        const size_t free = end-pointer;

        if( pointer-n >= base )
        {
          pointer -= n;
        }
        else
        {
          n -= in;
          pointer = end-n;
        }

        return *this;
      }

      bool operator==( const CircularPointer& other ) const
      {
        CheckCondition( base == other.base );

        return pointer == other.pointer;
      }

      bool operator!=( const CircularPointer& other ) const
      {
        return !operator==(other);
      }

      ptrdiff_t operator-( const CircularPointer& other ) const
      {
        CheckCondition( base == other.base );

        return pointer - other.pointer;
      }
    };

    template <typename T, int Size>
    CircularPointer<T,Size> operator-( const CircularPointer<T,Size>& p, int n )
    {
      CircularPointer<T,Size> tmp(p);
      tmp -= n;
      return tmp;
    }
  }
}

#endif
