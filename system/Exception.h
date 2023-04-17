/*******************************************************************************

  C++ header for exception related classes

  created 01.07.00                                                        ian

  (c) Masuma Limited 2000.  All rights reserved.

*******************************************************************************/
#ifndef _utils_exception_h_
#define _utils_exception_h_

#ifdef	__cplusplus

#include <cstring>
#include <cstdio>
#include <exception>
#include <iostream>
#include <string>
#include <cerrno>
#include <cstdio>
#include <utility>

namespace masuma::utils
{
  class Exception : public std::exception
  {
    static constexpr int doNotExpandErrno = 0;

    const std::string reason;
    const std::string file;
    const int         line;

  public:
    static constexpr size_t maxBacktraceDepth {16};
  private:

    void* backtraceData[maxBacktraceDepth];
    unsigned backtraceSize {};

  protected:

    const int err;

    [[nodiscard]] virtual std::string err2String() const { return strerror(err); }

  public:

    static int backtraceDepth;

    Exception( int          err,
               std::string  reason,
               std::string  file = "",
               int          line = 0 ) noexcept;

    explicit Exception( std::string  reason,
                        std::string  file = "",
                        int          line = 0 ) noexcept
      : Exception {0, std::move(reason), std::move(file), line} {}

    Exception(const Exception&) noexcept = default;

    ~Exception() noexcept override = default;

    [[nodiscard]] const char* what() const noexcept override;

    [[nodiscard]] int errVal() const { return err; }

    // Check that the result of fn is 0.  Uses return as value.
    //
    static int check( const char* fn,
                      int         result,
                      const char* file,
                      int         line,
                      const std::string& message = std::string() );

    static int check( const char* fn,
                      int         result,
                      const char* file,
                      int         line,
                      std::string (*errorFn)(int) );

    // Check that the result of fn is not NULL.
    //
    template <class T>
    static T* checkNull( const char* fn,
                         T*          result,
                         const char* file,
                         int         line,
                         const std::string& message = std::string() )
    {
      if( result == NULL )
      {
        if( message.empty( ) )
        {
          throw Exception( errno ? errno : EINVAL,
                           std::string( "checkNull: " ) + fn, file, line );
        }
        else
        {
          throw Exception( errno, message );
        }
      }

      return result;
    }

    template <class T>
    static T* checkNull( const char* fn,
                         T*          result,
                         int         error,
                         const char* file,
                         int         line )
    {
      if( result == NULL )
      {
        throw Exception( error ? error : EINVAL,
                         std::string( "checkNull: " ) + fn, file, line );
      }

      return result;
    }

    // Check that the result of a system call fn is not less than 0.  Uses
    // errno as value.
    //
    template <class T>
    static T checkSys( const char* fn,
                       T           result,
                       const char* file,
                       int         line,
                       const std::string& message = std::string() )
    {
      ssize_t n = result;
      if( n < 0 )
      {
        if( message.empty() )
        {
          throw Exception(errno, std::string("checkSys: ")+fn, file, line);
        }
        else
        {
          throw Exception( errno, message );
        }
      }

      return result;
    }

    template <class T>
    static T checkSys( const char* fn,
                       T     result,
                       const char* file,
                       int         line,
                       std::string (*errorFn)(T) )
    {
      ssize_t n = result;
      if( n < 0 )
      {
        throw Exception( doNotExpandErrno,
                         std::string("check: ")+fn+' '+errorFn(result),
                         file, line );
      }

      return result;
    }

    // Check that errno == 0.  Uses errno as value.
    //
    static int checkErrno( const char* fn,
                           int         result,
                           const char* file,
                           int         line,
                           const std::string& message = std::string() );

    // Check a condition, kind of an assert.
    //
    static void checkCondition( bool        condidtion,
                                const char* fn,
                                const char* file,
                                int         line ,
                                const std::string& message = std::string());
  };
}

// The only macros you'll find in this code!!
//
#define Throw( error, info ) \
        throw utils::Exception( (error), (info),  __FILE__, __LINE__  )

#define Whinge( type, info ) \
        throw type##Exception( (info), __FILE__, __LINE__ )

#define WhingeNI( type ) \
        throw type##Exception( __FILE__, __LINE__ )

#define Check( fn, args ) \
        masuma::utils::Exception::check( #fn, fn args, __FILE__, __LINE__ )

#define CheckM( fn, args, message ) \
        masuma::utils::Exception::check( #fn, fn args, __FILE__, __LINE__, message )

#define CheckFn( fn, args, errorHandler ) \
        masuma::utils::Exception::check( #fn, fn args, __FILE__, __LINE__, errorHandler )

#define CheckNull( fn, args ) \
        masuma::utils::Exception::checkNull( #fn, fn args, __FILE__, __LINE__ )

#define CheckNullError( fn, args, error ) \
        masuma::utils::Exception::checkNull( #fn, fn args, error, __FILE__, __LINE__ )

#define CheckNullM( fn, args, message ) \
        masuma::utils::Exception::checkNull( #fn, fn args, __FILE__, __LINE__, message )

#define CheckSys( fn, args ) \
        masuma::utils::Exception::checkSys( #fn, fn args, __FILE__, __LINE__ )

#define CheckSysM( fn, args, message ) \
        masuma::utils::Exception::checkSys( #fn, fn args, __FILE__, __LINE__, message )

#define CheckErrno( fn, args ) errno = 0; \
        masuma::utils::Exception::checkErrno( #fn, fn args, __FILE__, __LINE__ )

#define CheckErrnoM( fn, args, message ) errno = 0; \
        masuma::utils::Exception::checkErrno( #fn, fn args, __FILE__, __LINE__, message )

#define CheckSysFn( fn, args, errorHandler ) \
        masuma::utils::Exception::checkSys( #fn, fn args, __FILE__, __LINE__, errorHandler )

#define CheckSysNoThrow( fn, args ) \
        masuma::utils::Exception::checkSys( #fn, fn args, __FILE__, __LINE__, false )

#define CheckCondition( cond ) \
        masuma::utils::Exception::checkCondition( (cond), #cond, __FILE__, __LINE__ )

#define CheckConditionM( cond, message) \
        masuma::utils::Exception::checkCondition( (cond), #cond, __FILE__, __LINE__, message)
#endif
#endif
