/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2010-2014.  All rights reserved.
 *
 *  MODULE:      utilities
 *
 *  DESCRIPTION: String related utiliy functions.
 *
 ******************************************************************************/

#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <cstring>
#include "Exception.h"
#include "Types.h"

namespace masuma::utils
{
  using Strings = VectorWithJSON<std::string>;

  struct NameValuePair
  {
    std::string name;
    std::string value;

    void build( const std::string& in, size_t pos, size_t delimSize = 1 )
    {
      name = in.substr(0,pos);

      if( pos != std::string::npos )
      {
        value = in.substr(pos+delimSize);
      }
      else
      {
        value.clear();
      }
    }

    NameValuePair() = default;

    NameValuePair( const std::string& in, char delim )
    {
      build( in, in.find(delim) );
    }

    NameValuePair( const std::string& in, const char* delim )
    {
      build( in, in.find(delim), strlen(delim) );
    }

    [[nodiscard]] bool valid() const { return !name.empty(); }

    void nextPair( char delim )
    {
      build( value, value.find(delim) );
    }
  };

  template <typename T> inline size_t sizeOf(T);
  template <> inline size_t sizeOf<char>(char) { return 1; }
  template <> inline size_t sizeOf<const char*>(const char* s) {return strlen(s);}
  template <> inline size_t sizeOf<std::string>(std::string s) {return s.size();}

  inline void
  split( const std::string& in, size_t pos, std::string& lhs, std::string& rhs,
         size_t size = 1 )
  {
    lhs = in.substr(0,pos);

    if( pos != std::string::npos )
    {
      rhs = in.substr(pos+size);
    }
    else
    {
      rhs.clear();
    }
  }

  template <typename T> void
  splitLast( const std::string& in, T delim, std::string& lhs, std::string& rhs )
  {
    split( in, in.find_last_of(delim), lhs, rhs, sizeOf(delim) );
  }

  typedef std::pair<std::string,std::string> StringPair;

  template <typename T> StringPair
  split( const std::string& in, T delim )
  {
    StringPair result;
    split( in, in.find(delim), result.first, result.second, sizeOf(delim) );
    return result;
  }

  template <typename T> StringPair
  splitLast( const std::string& in, T delim )
  {
    StringPair result;
    split( in, in.find_last_of(delim), result.first, result.second, sizeOf(delim) );
    return result;
  }

  inline void
  split( const std::string& in, char delim, std::string& lhs, std::string& rhs )
  {
    split( in, in.find(delim), lhs, rhs );
  }

  inline void
  split( const std::string& in, const std::string& delim,
         std::string& lhs, std::string& rhs )
  {
    split( in, in.find(delim), lhs, rhs, sizeOf(delim) );
  }

  struct Tokens : Strings
  {
    Tokens() : Strings() {}

    template <typename Delim>
    Tokens( const std::string& in, Delim delim )
    {
      std::string rest(in);
      std::string token;

      do
      {
        split( rest, delim, token, rest );
        push_back(token);

      } while( !rest.empty() );
    }
  };

  inline Tokens
  tokenise( const std::string& in, char delim )
  {
    std::string rest(in);
    std::string token;
    Tokens result;

    do
    {
      split( rest, delim, token, rest );
      result.push_back(token);

    } while( !rest.empty() );

    return result;
  }

  inline void
  removeSpace( std::string& line )
  {
    for( std::string::iterator c = line.begin(); c !=line.end(); )
    {
      if( std::isspace( *c ) )
      {
        line.erase( c );
      }
      else
      {
        ++c;
      }
    }
  }

  inline std::string
  trimQuotes( const std::string& name )
  {
    return name.substr(1,name.size()-2);
  }

  inline std::string
  trimQuotesIfPresent( const std::string& name )
  {
    char c = *name.begin();

    if( (c == '\'' || c == '"') && c == *name.rbegin() )
    {
      return trimQuotes( name );
    }
    else
    {
      return name;
    }
  }

  inline std::string
  trim( const std::string& s)
  {
    std::string::size_type first = s.find_first_not_of(' ');

    if(first == std::string::npos)
      return "";
    std::string::size_type last = s.find_last_not_of(' ');
    if(last != std::string::npos)
      return s.substr(first, last + 1 - first);
    else
      return s.substr(first);
  }

  inline std::string
  trim( const std::string& s, const char* toTrim )
  {
    std::string::size_type first = s.find_first_not_of(toTrim);

    if(first == std::string::npos)
      return "";
    std::string::size_type last = s.find_last_not_of(toTrim);
    if(last != std::string::npos)
      return s.substr(first, last + 1 - first);
    else
      return s.substr(first);
  }

  inline bool
  icompare( const std::string& lhs, const std::string& rhs )
  {
    if( lhs.size() != rhs.size() ) return false;

    const size_t size(rhs.size());

    for( size_t n = 0; n < size; ++n )
    {
      if( std::toupper(lhs[n]) != std::toupper(rhs[n]) )
        return false;
    }
    return true;
  }

  inline std::string
  readQuotedString( std::istream& in, const char quote = '"' )
  {
    char c(0);

    while( in && c != quote ) in >> c;

    std::string s;

    in.read( &c, 1 );

    while( in && c != quote )
    {
      s += c;
      in.read( &c, 1 );
    }

    CheckConditionM( (c == '"'), "Bad string" );

    return s;
  }

  inline bool
  caseCompare( const std::string& lhs, const std::string & rhs )
  {
    return lhs < rhs;
  }

  inline bool
  anyCaseCompare( std::string lhs, std::string rhs )
  {
    std::transform( lhs.begin(), lhs.end(), lhs.begin(), tolower);
    std::transform( rhs.begin(), rhs.end(), rhs.begin(), tolower);

    return caseCompare( lhs, rhs );
  }

  inline bool
  caseEqual( const std::string& lhs, const std::string & rhs )
  {
    return lhs == rhs;
  }

  inline bool
  anyCaseEqual( std::string lhs, std::string rhs )
  {
    std::transform( lhs.begin(), lhs.end(), lhs.begin(), tolower);
    std::transform( rhs.begin(), rhs.end(), rhs.begin(), tolower);

    return caseEqual( lhs, rhs );
  }

  inline std::string
  capitalise( const std::string& s )
  {
    std::string result(s);
    result[0] = toupper(s[0]);
    return result;
  }

  inline std::string
  uncapitalise( const std::string& s )
  {
    std::string result(s);
    result[0] = tolower(s[0]);
    return result;
  }

  template <typename T> T
  fromString( const std::string& s )
  {
    std::istringstream in(s);

    T tmp;

    in >> tmp;

    return tmp;
  }

  template <> inline std::string
  fromString( const std::string& s )
  {
    return s;
  }

  template <typename T> std::string
  toString( const T& t )
  {
    std::ostringstream out;

    out << t;

    return out.str();
  }

  template <> inline std::string
  toString( const std::string& t )
  {
    return t;
  }
}

