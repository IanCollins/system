/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2014, all rights reserved
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: mapped file odds and ends
 *
 ******************************************************************************/

#ifndef _utils_MappedFile_h_
#define _utils_MappedFile_h_

#include "AutoFd.h"

#include <memory>

namespace masuma::utils
{
  struct MmapException : Exception
  {
    MmapException( int info, std::string file )
      : Exception( info, "Mmap: "+file ) {}
  };

  class MappedFile
  {
    AutoFd fd;

    std::shared_ptr<uint8_t> mapping;

    size_t mappedSize{};

    uint8_t* mapFile( size_t, int prot, int flags );

  public:

    MappedFile() = default;
    MappedFile( const MappedFile& ) = default;
    MappedFile& operator=( const MappedFile& ) = default;

    MappedFile( AutoFd, size_t, int prot, int flags );
    MappedFile( const std::string&, size_t, int prot, int flags );

    virtual ~MappedFile() = default;

    typedef uint8_t*       iterator;
    typedef const uint8_t* const_iterator;

    size_t size() const { return mappedSize; }

    AutoFd descriptor() const { return fd; }

    iterator begin()             { return mapping.get(); }
    const_iterator begin() const { return mapping.get(); }

    iterator end()             { return mapping.get()+mappedSize; }
    const_iterator end() const { return mapping.get()+mappedSize; }
  };

  class ReadOnlyMappedFile : public MappedFile
  {
  public:

    ReadOnlyMappedFile( const std::string&, size_t = 0 );
    ReadOnlyMappedFile( AutoFd, size_t = 0 );
  };

  class WriteOnlyMappedFile : public MappedFile
  {
  public:

    WriteOnlyMappedFile() = default;
    WriteOnlyMappedFile( const std::string&, size_t = 0 );
    WriteOnlyMappedFile( AutoFd, size_t = 0 );
  };
}

#endif
