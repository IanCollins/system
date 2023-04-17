/******************************* C++ Header File *******************************
*
*   (c) Masuma Limited 2012.  All rights reserved.
*
*  PROJECT:     Utils
*
*******************************************************************************/
#ifndef _utils_log_h_
#define _utils_log_h_

#include <string>
#include <iomanip>

#include <mutex>
#include "Timestamp.h"
#include "Tee.h"

namespace masuma::system
{
  class Log
  {
  public:

    enum Level
    {
      Diarrhea,
      Debug,
      Info,
      Warn,
      Error,
      None
    };

  private:

    struct Entrails
    {
      Level      level {Warn};
      bool       timestamp {false};
      std::mutex guard;
      int64_t    start;
      int64_t    last;

      std::streambuf* buf;
      std::streambuf* errBuf;

      Entrails()
        : start(RealtimeClock()), last(start),
          buf(std::cout.rdbuf()), errBuf(buf) {}

      [[nodiscard]] double fromStart( int64_t now ) const
      {
        return (now - start) / 1000000000.0;
      }

      [[nodiscard]] double fromLast( int64_t now ) const
      {
        return (now - last) / 1000000000.0;
      }

      void rebase( int64_t now ) { last = now; }
    };

    static Entrails entrails;

    const bool streaming;

    std::streambuf* buf;

    static void lock()   { entrails.guard.lock(); }
    static void unlock() { entrails.guard.unlock(); }

    static std::ostream& out() { return std::cout; }

  public:

    Log() : streaming {true}
    {
      lock();
      buf = std::cout.rdbuf(entrails.buf);
    }

    explicit Log( Level loglevel ) : streaming{ loglevel >= entrails.level }
    {
      if( streaming )
      {
        lock();

        buf = std::cout.rdbuf(loglevel >= Warn ? entrails.errBuf : entrails.buf);

        if( entrails.timestamp )
        {
          out() << timestamp() << ' ';
        }
      }
    }

    ~Log()
    {
      if( streaming )
      {
        std::cout.rdbuf(buf);
        unlock();
      }
    }

    [[nodiscard]] bool wouldStream() const { return streaming; }

    static void setBuf( std::ostream& os, bool includeErr = true )
    {
      entrails.buf = os.rdbuf();

      if( includeErr ) entrails.errBuf = entrails.buf;
    }

    static void setErrorBuf( std::ostream& os )
    {
      entrails.errBuf = os.rdbuf();
    }

    static void logTo( int all, int warn )
    {
      OutTee::Fds fds;

      fds << all << warn;

      entrails.errBuf = new system::OutTee(fds );
    }

    static void useTimestamp( bool b ) { entrails.timestamp = b; }
    static bool usingTimestamp() { return entrails.timestamp; }

    static void setLevelTo( Level level ) { entrails.level = level; }
    static Level getLevel() { return entrails.level; }

    static void incrementLevel()
    {
      entrails.level = static_cast<Log::Level>(entrails.level-1);
    }

    static bool atLeast( Level level ) { return entrails.level <= level; }

    template <typename T> Log& operator<<( const T& t )
    {
      if( streaming )
      {
        out() << t;
      }

      return *this;
    }

    Log& operator<<( std::ostream& (*t)(std::ostream&) )
    {
      if( streaming )
      {
        out() << t;
      }

      return *this;
    }

    Log& operator<<( std::ios_base& (*t)(std::ios_base&) )
    {
      if( streaming )
      {
        out() << t;
      }

      return *this;
    }
  };

  template <Log::Level LogLevel>
  struct _log : Log
  {
    _log() : Log {LogLevel} {}
  };

  using DebugLog [[maybe_unused]] = _log<Log::Debug>;
  using InfoLog [[maybe_unused]] = _log<Log::Info>;
  using WarnLog [[maybe_unused]] = _log<Log::Warn>;
  using ErrorLog [[maybe_unused]] = _log<Log::Error>;

  struct [[maybe_unused]] LogPlus
  {
    void operator++(int) { Log::incrementLevel(); }
  };
}

#define LOG_ENTRAILS_DEF namespace masuma { namespace system { Log::Entrails Log::entrails; } }

#endif
