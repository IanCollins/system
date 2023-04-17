/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2012.  All rights reserved.
 *
 *  MODULE:      Utilities
 *
 *  Created:     November 5, 2012, 1:20 PM
 *
 ******************************************************************************/

#include "Path.h"
#include "Exception.h"
#include <sys/stat.h>

#include <utility>

namespace masuma::utils
{
  void
  Path::fromString( std::string path )
  {
    std::string tail;

    while( !path.empty( ) )
    {
      split( path, '/', tail, path );
      push_back( tail );
    }
  }

  Path::Path( std::string path )
  {
    fromString( std::move(path) );
  }

  Path::Path( const char* path )
  {
    fromString( path );
  }

  bool
  Path::compare( const Path& other, size_t count, bool condition ) const
  {
    if( !condition ) return false;

    for( size_t n = 0; n < count; ++n )
    {
      if( at(n) != other.at(n) )
      {
        return false;
      }
    }

    return true;
  }

  Path::operator std::string() const
  {
    if( empty() ) return std::string();

    std::string out = at(0);

    for( size_t n = 1; n < size(); ++n )
    {
      out = out + '/' + at(n);
    }

    return out;
  }

  Path
  Path::tail() const
  {
    Path result(*this);
    result.pop_back();
    return result;
  }

  Path
  Path::headOf( const Path& root ) const
  {
    Path result(*this);

    for( const auto& bit : root )
    {
      if( bit == *result.begin())
      {
        result.erase(result.begin());
      }
      else
      {
        throw std::runtime_error( std::string(root)+" not parent of "+std::string(*this) );
      }
    }
    return result;
  }

  void
  Path::makeDirectoriesIfMissing( mode_t mode ) const
  {
    Path directories(tail());

    std::string path;

    for( const auto& bit : directories )
    {
      path += bit;
      int result = mkdir( path.c_str(), mode );

      CheckCondition( result == 0 || errno == EEXIST );
      path += '/';
    }
  }

  Path
  commonRootOf( const Path& a, const Path& b )
  {
    Path result("");

    const int toCheck = std::min( a.size(), b.size() );

    for( int n = 0; n < toCheck; ++n )
    {
      if( a[n] != b[n] ) return result;

      result << a[n];
    }

    return result;
  }

  Path operator+( const Path& lhs, const Path& rhs )
  {
    Path result(lhs);

    result.insert( result.end(), rhs.begin(), rhs.end() );

    return result;
  }

  Path operator+( const Path& lhs, Path&& rhs )
  {
    Path result(lhs);

    result.insert( result.end(), rhs.begin(), rhs.end() );

    return result;
  }

  Path operator+( const Path& lhs, std::string rhs )
  {
    return lhs+Path(std::move(rhs));
  }

  PathPair
  pathsForRename( const Path& from, const Path& to )
  {
    if( from.tail() == to.tail() )
    {
      return PathPair( from, to );
    }
    else
    {
      Path root = commonRootOf( from, to );

      const int commonBits = root.size();

      if( commonBits == 0 )
      {
        return PathPair( from, to );
      }
      else
      {
        PathPair result( root, root );
        result.first  << from[commonBits];
        result.second << to[commonBits];

        return result;
      }
    }
  }

  std::ostream&
  operator<<(std::ostream& out, const Path& path )
  {
    for( size_t n = 0; n < path.size(); ++n )
    {
      out << (n == 0 ? "" : "/") << path[n];
    }

    return out;
  }
}
