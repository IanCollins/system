/******************************* C++ Header File *******************************
*
*  Copyright (c) Masuma Ltd 2011.  All rights reserved.
*
*  MODULE:      utilities
*
*  DESCRIPTION: Directory scanner.
*
*******************************************************************************/

#ifndef _utils_Scanner_h_
#define _utils_Scanner_h_

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <algorithm>

#if !defined _POSIX_PTHREAD_SEMANTICS
 #define _POSIX_PTHREAD_SEMANTICS
#endif

#include "Exception.h"
#include "directory.h"
#include "String.h"

#include "Stat.h"

namespace masuma
{
  namespace utils
  {
    struct FilesystemTraits
    {
      static void getStatus( const std::string& filepath, struct stat* sb )
      {
        CheckSysM( lstat, (filepath.c_str(), sb), filepath );
      }

      using Directory = utils::PosixDirectory;
    };

    template <typename Actions, typename Traits = FilesystemTraits>
    struct Scanner : Actions
    {
      static unsigned files;
      static unsigned directories;

      using DataType   = typename Actions::DataType;
      using Directory  = typename Traits::Directory;
      using ReturnType = std::pair<bool,DataType>;

      using Actions::status;

      DataType data;

      void operator()( const typename Directory::Entry& entry )
      {
        const std::string filepath {entry.name()};

        try
        {
          status.reset( filepath );

          if( status.isRegular() )
          {
            ++files;

            Actions::processFile( entry, data );
          }
          else if( status.isDirectory() )
          {
            ++directories;

            Directory dir {entry};

            ReturnType dirResult {Actions::processDirectory( entry, dir, data )};

            if( dirResult.first )
            {
              //std::cout << dir.size() << " " << filepath << std::endl;

              std::for_each( dir.begin(), dir.end(), Scanner(dirResult.second) );
            }

            Actions::finishedDirectory( entry );
          }
          else
          {
            Actions::processOther( entry, data );
          }
        }
        catch( utils::Exception& e )
        {
          if( e.errVal() != ELOOP )
          {
            std::cerr << filepath << ' ' << e.what() << std::endl;
          }
        }
        catch( std::exception& e )
        {
          std::cerr << filepath << ' ' << e.what() << std::endl;
        }
      }

      Scanner() = delete;

      explicit Scanner( const DataType& data ) : data {data} {}

      void start()
      {
        files = 0;
        directories = 1;

        std::string dirPath {data};

        if( *dirPath.rbegin() == '/' )
        {
          dirPath.erase(dirPath.begin()+(dirPath.size()-1));
        }

        auto tailHead = utils::splitLast( dirPath, '/' );

        if( tailHead.first.empty() )
        {
          tailHead.first = "/";
        }

        const Directory dir {tailHead.first, NULL};

        auto target = dir.find( tailHead.second );

        if( target != dir.end() )
        {
          operator()( *target );
        }
      }
    };

    template <typename T, typename TT> unsigned Scanner<T,TT>::files;
    template <typename T, typename TT> unsigned Scanner<T,TT>::directories;

    // The following code is provided as an example Action class for a scanner.
    // It may be used as a base class.
    //
    struct BasicActions
    {
      utils::Stat status;

      using DataType   =  std::string;
      using ReturnType = std::pair<bool,DataType>;

      virtual void processFile(  const PosixDirectory::Entry&, const DataType& ) = 0;
      virtual void processOther( const PosixDirectory::Entry&, const DataType& ) = 0;

      virtual ReturnType processDirectory( const PosixDirectory::Entry&,
                                           PosixDirectory&,
                                           const DataType& ) = 0;

      virtual void finishedDirectory( const PosixDirectory::Entry& ) {}
    };

    struct ExampleActions : BasicActions
    {
      void processFile( const PosixDirectory::Entry& entry, const DataType& )
      {
        const std::string filepath {entry.name()};

        std::cout << filepath << std::endl;
      }

       void processOther( const PosixDirectory::Entry& entry, const DataType& )
      {
        const std::string filepath {entry.name()};

        std::cout << filepath << std::endl;
      }

     ReturnType processDirectory( const PosixDirectory::Entry& entry,
                                  PosixDirectory&,
                                  const DataType& )
      {
        const std::string filepath {entry.name()};

        std::cout << filepath << std::endl;

        return {true,filepath};
      }
    };
  }
}

#endif
