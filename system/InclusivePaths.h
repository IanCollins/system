/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2012.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 ******************************************************************************/

#ifndef _utils_InclusivePaths_h_
#define	_utils_InclusivePaths_h_

#include "Path.h"

#include <vector>

namespace masuma
{
  namespace utils
  {
    class InclusivePaths : std::vector<Path>
    {
      using Base = std::vector<Path>;

    public:

      using Base::const_iterator;
      using Base::size;
      using Base::empty;
      using Base::operator[];
      using Base::begin;
      using Base::end;
      using Base::clear;

      InclusivePaths() = default;
      virtual ~InclusivePaths() = default;

      InclusivePaths( const InclusivePaths& ) = default;
      InclusivePaths( InclusivePaths&& ) = default;
      InclusivePaths& operator=( const InclusivePaths& ) = default;
      InclusivePaths& operator=( InclusivePaths&& ) = default;

      explicit InclusivePaths( const Strings& );
      explicit InclusivePaths( const Paths& );

      InclusivePaths& operator<<( const Path& );

      InclusivePaths& operator<<( const std::string& path )
      {
        return (*this) << Path(path);
      }

      InclusivePaths& operator<<( const Paths& );

      void removeChildrenOf( const Path& path );
      void removePathAndChildren( const Path& path );
    };

    void addToPathsFromFile( InclusivePaths& paths, const std::string& filename );

    void filterPaths( const Paths& source, const InclusivePaths& targets,
                      Paths& paths, bool include );
  }
}

#endif

