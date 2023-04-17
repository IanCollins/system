/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2010-2014.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: Directory related utility classes and functions.
 *
 ******************************************************************************/

#ifndef _utils_directory_h_
#define _utils_directory_h_

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>

#include <string>
#include <vector>
#include <algorithm>
#include <memory>

namespace masuma
{
  namespace utils
  {
    struct PosixDirectoryTraits
    {
      using Dir_t = DIR*;
      using Entry_t = dirent;

      static Dir_t open( std::string path )
      {
        return opendir( path.c_str() );
      }
      
      static void close( Dir_t dir )
      {
        closedir( dir );
      }
     
      static Entry_t* read( Dir_t dir )
      {
        return readdir( dir );
      }
      
      static std::string name( const Entry_t* entry )
      {
        return entry->d_name;
      }
      
      static void rewind( Dir_t dir )
      {
        rewinddir( dir );
      }
      
      static auto stat( std::string path, struct stat* st )
      {
        return ::stat( path.c_str(), st );
      }
    };
    
    template<typename Traits>
    class Directory
    {
      typedef std::vector<std::string> vector;

      typename Traits::Dir_t dir;
      
      char buf[1024];
      bool accessHasChanged {false};

      Directory( const Directory& );
      Directory& operator=( const Directory& );

    public:

      const std::string name;

      vector newFiles;

      class Entry
      {
        using Stat = struct stat;
        
        const Directory* const dir;
        const typename Traits::Entry_t* const ent;
        std::unique_ptr<Stat> st;
        
      public:

        Entry( const Directory* dir, const typename Traits::Entry_t* ent ) 
          : dir {dir}, ent {ent} {}

        std::string name() const { return dir->name+'/'+Traits::name(ent); }

        std::string entryName() const { return Traits::name(ent); }

        const auto& directory() const { return *dir; }

        auto isNewFile() const { return dir->isNewFile( entryName() ); }
        
        auto isDir() 
        {
          if( !st )
          {
            std::unique_ptr<Stat> _st {new Stat};
            
            if( Traits::stat(name(), _st.get() ) == 0 )
            {
              st = std::move(_st);
                        
              return S_ISDIR(st->st_mode);
            }
          }
  
          return false;
        }
      };

      class IteratorBase : public std::iterator<std::forward_iterator_tag,Entry>
      {
      protected:

        const Directory* const dir      {nullptr};
        const typename Traits::Entry_t* currentEntry {nullptr};

        void getNext()
        {
          do
          {
            currentEntry = Traits::read(dir->dir);
          }
          while( currentEntry &&
                 (Traits::name(currentEntry) == "." ||
                  Traits::name(currentEntry) == "..") );
        }
        
      public:

        IteratorBase() = default;

        IteratorBase( const Directory* d )
          : dir {d}
        {
          getNext();
        }

        IteratorBase( const IteratorBase& ) = default;
        IteratorBase( IteratorBase&& ) = default;

        void operator++() { getNext(); }

        bool operator==( const IteratorBase& other )
        {
          return currentEntry == other.currentEntry;
        }

        bool operator!=( const IteratorBase& other )
        {
          return currentEntry != other.currentEntry;
        }
      };

      struct iterator : IteratorBase
      {
        iterator() = default;

        iterator( const Directory* d ) : IteratorBase {d} {}

        Entry operator*() const { return {this->dir, this->currentEntry}; }
      };

      struct const_iterator : IteratorBase
      {
        const_iterator() = default;

        const_iterator( const Directory* d ) : IteratorBase {d} {}

        const Entry operator*() const { return {dir, this->currentEntry}; }
      };

      struct NameIterator : public iterator
      {
        NameIterator() = default;

        NameIterator( const Directory* d ) : iterator {d} {}

        std::string operator*() const{ return iterator::operator*().entryName(); }
      };

      Directory( const std::string& path, const Directory* )
        : dir {Traits::open( path )},
          name {path}
      {
      }

      explicit Directory( const Entry& entry )
        : Directory {entry.name(),nullptr} {}

      ~Directory() { Traits::close( dir ); }

      void rewind() { Traits::rewind( dir ); }

      bool isNewFile( const std::string& path ) const
      {
        return std::find(newFiles.begin(), newFiles.end(), path) != newFiles.end();
      }

      operator std::string() const { return name; }

      void hasChanged() { accessHasChanged = true; }

      bool hasAccessChanged() const { return accessHasChanged; }

      // Invalidates any iterators!
      //
      size_t size()
      {
        auto sz = std::distance(begin(), end());
        rewind();
        return sz;
      }

      iterator begin() { return this; }
      iterator end() { return iterator(); }
      const_iterator begin() const { return this; }
      const_iterator end() const { return const_iterator(); }
      NameIterator nameBegin() { return this; }
      NameIterator nameEnd() { return NameIterator(); }

      struct EntryByName
      {
        const std::string& name;

        EntryByName( const std::string& name ) : name(name) {}

        bool operator()( const Entry& entry ) const
        {
          return name == entry.entryName();
        }
      };

      iterator find( const std::string& entry )
      {
        return std::find_if( begin(), end(), EntryByName(entry) );
      }

      const_iterator find( const std::string& entry ) const
      {
        return std::find_if( begin(), end(), EntryByName(entry) );
      }
    };
    
    using PosixDirectory = Directory<PosixDirectoryTraits>;
  }
}

#endif
