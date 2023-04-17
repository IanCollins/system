/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2014, all rights reserved
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: sat wrapper
 *
 ******************************************************************************/

#pragma once

#include "Exception.h"
#include "AutoFd.h"

#if defined HAVE_JSON
# include <json/JSON.h>
#endif

#include <stdexcept>
#include <chrono>

#include <sys/stat.h>

namespace masuma::system
{
  struct MissingFileException : std::runtime_error
  {
    MissingFileException( const std::string& filename, int err )
      : std::runtime_error {filename+" "+strerror(err)} {}

    ~MissingFileException() override = default;
  };

  enum class FileType
  {
    Missing,
    Regular,
    Directory,
    Link,
    FifoSpecial,
    BlockSpecial,
    CharSpecial,
    Other
  };

  class Stat
  {
    struct stat info {};

#if defined HAVE_JSON
    friend json::Object& operator<<( json::Object&, const Stat& );
    friend json::Object& operator>>( json::Object&, Stat& );
    friend json::Object operator>>( json::ValueProxy value, Stat& st )
    {
      json::Object object(value);

      object >> st;

      return object;
    }
#endif

  public:

    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    Stat();
    explicit Stat( const std::string& file );
    explicit Stat( const AutoFd& );

    explicit Stat( const struct stat& );

    void reset( const std::string& file );

    [[nodiscard]] FileType fileType() const;

    [[nodiscard]] size_t size() const { return info.st_size; }
    [[nodiscard]] auto user()  const -> decltype(info.st_uid)  { return info.st_uid; }
    [[nodiscard]] auto group() const -> decltype(info.st_gid)  { return info.st_gid; }

    [[nodiscard]] auto modifyTime() const
    {
      const auto secs {info.st_mtim.tv_sec};
      const auto nsecs {secs*1'000'000'000 + info.st_ctim.tv_nsec};
      std::chrono::nanoseconds ns {nsecs};
      return TimePoint {ns};
    }

    [[nodiscard]] TimePoint changeTime() const
    {
      const auto secs  {info.st_ctim.tv_sec};
      const auto nsecs {secs*1'000'000'000 + info.st_ctim.tv_nsec};
      std::chrono::nanoseconds ns {nsecs};

      return TimePoint {ns};
    }

    [[nodiscard]] auto modifyTimeS() const { return info.st_mtim.tv_sec; }
    [[nodiscard]] auto changeTimeS() const { return info.st_ctim.tv_sec; }

    [[nodiscard]] auto sinceLastChange() const
    {
      const auto now {Clock::now()};
      return now - changeTime();
    }

    template<FileType N> [[nodiscard]] bool fileIs() const { return fileType() == N; }

    [[nodiscard]] bool isDirectory() const { return fileIs<FileType::Directory>(); }
    [[nodiscard]] bool isLink()      const { return fileIs<FileType::Link>(); }
    [[nodiscard]] bool isRegular()   const { return fileIs<FileType::Regular>(); }

    void applyTo( const std::string& file ) const;
    void applyTo( int ) const;
    void applyTo( const AutoFd& fd ) const { applyTo( fd.get()); }

    [[nodiscard]] const struct stat& get() const { return info; }

    explicit operator bool() const { return info.st_size >= 0; }

#if defined HAVE_JSON
    explicit Stat( json::Object );
    explicit Stat( const json::ValueProxy& );
    json::Object asJSON() const;
#endif
  };
}

