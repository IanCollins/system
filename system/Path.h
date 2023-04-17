/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2012.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 ******************************************************************************/

#ifndef _utils_Path_h_
#define	_utils_Path_h_

#include "String.h"

namespace masuma
{
  namespace system
  {
    class Path : public Strings
    {
      bool compare( const Path& other, size_t count, bool condition ) const;

      void fromString( std::string );

    public:

      using Strings::size;
      using Strings::operator[];
      using Strings::const_iterator;
      using Strings::begin;
      using Strings::end;

      Path() {}
      Path( const Path& other ) : system::Strings(other) {}
      Path( Path&& other ) : system::Strings(other) {}

      explicit Path( std::string );
      explicit Path( const char* );

      Path& operator=( const Path& other ) = default;
      Path& operator=( Path&& other ) = default;

      bool operator==( const Path& other ) const
      {
        return compare( other, size(), size() == other.size() );
      }

      bool operator!=( const Path& other ) const
      {
        return !operator==( other );
      }

      bool isChildOf( const Path& other ) const
      {
        return compare( other, other.size(), size() > other.size() );
      }

      bool isParentOf( const Path& other ) const
      {
        return compare( other, size(), size() < other.size() );
      }

      std::string head() const { return *rbegin(); }

      Path tail() const;
      Path headOf( const Path& ) const;
      Path headOf( const std::string& s ) const { return headOf( Path(s)); }

      void makeDirectoriesIfMissing( mode_t mode = 0755 ) const;

      operator std::string() const;

      friend std::ostream& operator<<( std::ostream&, const Path& );
    };

    Path operator+( const Path&, const Path& );
    Path operator+( const Path&, Path&& );
    Path operator+( const Path&, std::string );

    Path commonRootOf( const Path&, const Path& );

    typedef std::pair<Path,Path> PathPair;

    PathPair pathsForRename( const Path& from, const Path& to );

    class Paths : std::vector<Path>
    {
      typedef std::vector<Path> Base;

    public:

      using Base::const_iterator;
      using Base::begin;
      using Base::end;
      using Base::erase;
      using Base::size;
      using Base::operator[];
      using Base::clear;

      Paths& operator<<( const Path& path )
      {
        push_back( path );
        return *this;
      }

      Paths& operator<<( const std::string& path )
      {
        return (*this) << Path(path);
      }

      Paths& operator<<( const Paths& );

      void removeChildrenOf( const Path& path );
      void removePathAndChildren( const Path& path );
      void removePathAndParents( const Path& path );
    };

    inline void
    print( const Path& path )
    {
      std::cout << path << std::endl;
    }

    struct PrintPath
    {
      std::ostream& out;

      PrintPath( std::ostream& out ) : out(out) {}

      void operator()( const Path& path ) { out << path << std::endl; }
    };

    inline std::ostream&
    operator<<( std::ostream& out, const Paths& paths )
    {
      return std::for_each( paths.begin(), paths.end(), PrintPath( out ) ).out;
    }

    struct IsParentOf
    {
      const Path& path;

      IsParentOf( const Path & path ) : path( path ){ }

      bool operator()(const Path & other)
      {
        return other.isParentOf( path );
      }
    };

    struct IsChildOf
    {
      const Path& path;

      IsChildOf( const Path & path ) : path( path ){ }

      bool operator()(const Path & other)
      {
        return other.isChildOf( path );
      }
    };

    struct IsSameAs
    {
      const Path& path;

      IsSameAs( const Path & path ) : path( path ){ }

      bool operator()(const Path & other)
      {
        return other == path;
      }
    };
  }
}

#endif

