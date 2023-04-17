/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2014, all rights reserved
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: mapped file odds and ends
 *
 ******************************************************************************/

#include "MappedFile.h"
#include "Stat.h"

#include <sys/mman.h>
#include <fcntl.h>

#include <utility>

namespace
{
  struct Deleter
  {
    size_t& mappedSize;

    explicit Deleter( size_t& mappedSize ) : mappedSize {mappedSize} {}

    void operator()( uint8_t* mapping ) const
    {
      munmap( mapping, mappedSize );
    }
  };
}

namespace masuma::utils
{
  MappedFile::MappedFile( AutoFd fd, size_t size, int prot, int flags )
    : fd {std::move(fd)},
      mapping{mapFile(size,prot,flags), Deleter{mappedSize}}
  {
  }

  MappedFile::MappedFile( const std::string& name, size_t size, int prot, int flags )
    : fd {open, name.c_str(), (prot==PROT_READ) ? O_RDONLY : O_RDWR|O_CREAT|O_TRUNC, 0600 },
      mapping{mapFile(size,prot,flags), Deleter{mappedSize}}
  {
  }

  uint8_t*
  MappedFile::mapFile( size_t fileSize, int prot, int flags )
  {
    if( fileSize == 0 )
    {
      Stat stat {fd};

      fileSize = stat.size();
    }

    mappedSize = fileSize;

    if( prot == PROT_WRITE )
    {
      fd.seekSet( mappedSize-1 );
      fd.write( "\0", 1);
      fd.seekSet( 0 );
    }

    void* pm = mmap( nullptr, mappedSize, prot, flags, fd.get(), 0 );

    if( pm == MAP_FAILED )
      throw MmapException( errno, "" );

    return static_cast<uint8_t*>(pm);
  }
}

namespace masuma::utils
{
  ReadOnlyMappedFile::ReadOnlyMappedFile( const std::string& file, size_t size )
    : MappedFile { file, size, PROT_READ, MAP_SHARED }
  {
  }

  ReadOnlyMappedFile::ReadOnlyMappedFile( AutoFd fd, size_t size )
    : MappedFile { std::move(fd), size, PROT_READ, MAP_SHARED }
  {
  }
}

namespace masuma::utils
{
  WriteOnlyMappedFile::WriteOnlyMappedFile( const std::string& file, size_t size )
    : MappedFile { file, size, PROT_WRITE, MAP_SHARED }
  {
  }

  WriteOnlyMappedFile::WriteOnlyMappedFile( AutoFd fd, size_t size )
    : MappedFile { std::move(fd), size, PROT_WRITE, MAP_SHARED }
  {
  }
}
