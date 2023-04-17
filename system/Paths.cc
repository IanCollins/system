/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2012.  All rights reserved.
 *
 *  MODULE:      zfs
 *
 *  Created:     September 24, 2012, 8:12 PM
 *
 ******************************************************************************/

#include "String.h"
#include "Path.h"
#include "InclusivePaths.h"

namespace masuma::system
{
  void
  Paths::removeChildrenOf( const Path& path )
  {
    auto last = std::remove_if( begin(), end(), IsChildOf(path) );

    erase( last, end() );
  }

  void
  Paths::removePathAndChildren( const Path& path )
  {
    auto last = std::remove_if( begin(), end(), IsSameAs(path) );

    last = std::remove_if( begin(), last, IsChildOf(path) );

    erase( last, end() );
  }

  void
  Paths::removePathAndParents( const Path& path )
  {
    auto last = std::remove_if( begin(), end(), IsSameAs(path) );

    last = std::remove_if( begin(), last, IsParentOf(path) );

    erase( last, end() );
  }

  Paths&
  Paths::operator<<( const Paths& paths )
  {
    for(const auto & path : paths)
    {
      (*this) << path;
    }

    return *this;
  }

  void
  filterPaths( const Paths& source, const InclusivePaths& targets,
               Paths& paths, bool include )
  {
    for(const auto & target : targets)
    {
      for(const auto & path : source)
      {
        if( target == path || target.isParentOf( path ) )
        {
          if( include )
          {
            paths << path;
          }
          else
          {
            paths.erase( std::find( paths.begin(), paths.end(), path ) );
          }
        }
      }
    }
  }
}
