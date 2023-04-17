/******************************* C++ Source File *******************************
 *
 *  Copyright (c) Masuma Ltd 2014, all rights reserved
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: sat wrapper
 *
 ******************************************************************************/

#include "Stat.h"
#include "AutoFd.h"
#include "Time.h"

#include <utime.h>
#include <unistd.h>

namespace masuma::utils
{
  Stat::Stat()
  {
    info.st_mode = 0;
    info.st_size = -1;
  }

  Stat::Stat( const struct stat& s )
  {
    info = s;
  }

  Stat::Stat( const std::string& file )
  {
    CheckConditionM( !file.empty(), file );

    reset( file );
  }

  Stat::Stat( const AutoFd& fd )
  {
    if( fstat( fd.get(), &info ) != 0 )
    {
      throw MissingFileException( "<fd>", errno );
    }
  }

  void
  Stat::reset( const std::string& file )
  {
    if( lstat( file.c_str(), &info ) != 0 )
    {
      throw MissingFileException( file, errno );
    }
  }

  FileType
  Stat::fileType() const
  {
    if( info.st_mode == 0 )      return FileType::Missing;
    if( S_ISREG(info.st_mode) )  return FileType::Regular;
    if( S_ISDIR(info.st_mode) )  return FileType::Directory;
    if( S_ISLNK(info.st_mode))   return FileType::Link;
    if( S_ISFIFO(info.st_mode) ) return FileType::FifoSpecial;
    if( S_ISBLK(info.st_mode) )  return FileType::BlockSpecial;
    if( S_ISCHR(info.st_mode) )  return FileType::CharSpecial;

    return FileType::Other;
  }

  void
  Stat::applyTo( const std::string& file ) const
  {
    CheckSys( chown, ( file.c_str(), info.st_uid, info.st_gid ) );
    CheckSys( chmod, ( file.c_str(), info.st_mode ) );

    utimbuf timbuf;
    timbuf.actime  = info.st_atime;
    timbuf.modtime = info.st_mtime;

    CheckSys( utime, ( file.c_str(), &timbuf ) );
  }

  void
  Stat::applyTo( int fd ) const
  {
    CheckSys( fchown, ( fd, info.st_uid, info.st_gid ) );
    CheckSys( fchmod, ( fd, info.st_mode ) );

    timespec times[2] = {{0}};

    times[0].tv_sec = info.st_atime;
    times[1].tv_sec = info.st_mtime;

    CheckSys( futimens, ( fd, times ) );
  }

#if defined HAVE_JSON
  json::Object&
  operator<<( json::Object& object, const Stat& stat )
  {
    object["atime"] = stat.info.st_atime;
    object["ctime"] = stat.info.st_ctime;
    object["mtime"] = stat.info.st_mtime;
    object["uid"]   = stat.info.st_uid;
    object["gid"]   = stat.info.st_gid;
    object["size"]  = stat.info.st_size;
    object["mode"]  = stat.info.st_mode;

    return object;
  }

  json::Object&
  operator>>( json::Object& object, Stat& stat )
  {
    stat.info.st_atime = object["atime"];
    stat.info.st_ctime = object["ctime"];
    stat.info.st_mtime = object["mtime"];
    stat.info.st_uid   = object["uid"];
    stat.info.st_gid   = object["gid"];
    stat.info.st_size  = object["size"];
    stat.info.st_mode  = object["mode"];

    return object;
  }

  Stat::Stat( json::Object object )
  {
    object >> *this;
  }

  Stat::Stat( const json::ValueProxy& proxy )
  {
    proxy >> *this;
  }

  json::Object
  Stat::asJSON() const
  {
    json::Object obj;
    obj << *this;
    return obj;
  }
#endif
}
