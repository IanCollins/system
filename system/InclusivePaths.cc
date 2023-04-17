/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2012.  All rights reserved.
 *
 *  MODULE:      zfs
 *
 ******************************************************************************/

#include "InclusivePaths.h"

#include <fstream>

namespace masuma::utils
{
  InclusivePaths::InclusivePaths( const Strings& paths )
  {
    for( const auto& path : paths )
    {
      (*this) << path;
    }
  }

  InclusivePaths::InclusivePaths( const Paths& paths )
  {
    for( const auto& path : paths )
    {
      (*this) << path;
    }
  }

  InclusivePaths&
  InclusivePaths::operator<<( const Path& path )
  {
    if( std::find_if( begin(), end(), IsParentOf(path) ) == end() )
    {
      auto i = std::find_if( begin(), end(), IsChildOf( path ) );

      while( i != end() )
      {
        erase( i );

        i = std::find_if( begin(), end(), IsChildOf( path ) );
      }

      emplace_back( path );

    }
    return *this;
  }

  InclusivePaths&
  InclusivePaths::operator<<( const Paths& paths )
  {
    for( const auto& path : paths )
    {
      (*this) << path;
    }

    return *this;
  }

  void
  InclusivePaths::removeChildrenOf( const Path& path )
  {
    auto last = std::remove_if( begin(), end(), IsChildOf(path) );

    erase( last, end() );
  }

  void
  InclusivePaths::removePathAndChildren( const Path& path )
  {
    auto last = std::remove_if( begin(), end(), IsSameAs(path) );

    last = std::remove_if( begin(), last, IsChildOf(path) );

    erase( last, end() );
  }

  void
  addToPathsFromFile( InclusivePaths& paths, const std::string& filename )
  {
    if( !filename.empty() )
    {
      std::ifstream in {filename};

      while( in )
      {
        std::string fs;
        in >> fs;

        if( !fs.empty() )
        {
          paths << fs;
        }
      }
    }
  }
}
