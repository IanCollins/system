/******************************* C++ Header File *******************************
*
*  Copyright (c) Masuma Ltd 2009-2012.  All rights reserved.
*
*  MODULE:      system
*
*******************************************************************************/

#ifndef _utils_Options_h_
#define _utils_Options_h_

#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <memory>
#include "String.h"

namespace masuma
{
  namespace system
  {
    using ArgV = std::vector<std::string>;

    template <typename T>
    struct Option
    {
      typedef Option<T> OptionType;

      typedef std::shared_ptr<OptionType> OptionPtr;

      typedef std::map<char,OptionPtr>        Options;
      typedef std::map<std::string,OptionPtr> LongOptions;
      typedef std::vector<OptionPtr>          OptionList;

      static Options     options;
      static LongOptions longOptions;

      static const char* const info[];

      virtual Option* clone( ArgV::iterator& ) = 0;

      virtual std::string what() const = 0;
      virtual void process( T& ) const = 0;

      static void displayOptions( const std::string& requiredOptions = "" )
      {
        for( const auto& option : longOptions )
        {
          std::cerr << '\t' << option.second->what();

          if( requiredOptions.find(option.first) != requiredOptions.npos )
          {
            std::cerr << '*';
          }
          std::cerr << std::endl;
        }

        if( !requiredOptions.empty() )
        {
          std::cerr << "\t* = required" << std::endl;
        }
      }

      static void displayMissingOptions( const std::string& missingOptions )
      {
        for( const auto& option : options )
        {
          if( missingOptions.find(option.first) != missingOptions.npos )
          {
            std::cerr << '\t' << option.second->what() << std::endl;
          }
        }
      }

      static void
      loadShortOption( ArgV::iterator& i, T& setup, std::string& requiredOptions )
      {
        const std::string& arg = *i;

        std::string::const_iterator option = arg.begin()+1;

        while( option != arg.end() )
        {
          typename Options::iterator o = options.find( *option );

          if( o != options.end() )
          {
            size_t pos = requiredOptions.find( *option );

            if( pos != requiredOptions.npos )
            {
              requiredOptions.erase( requiredOptions.begin() + pos );
            }

            o->second->clone( i )->process( setup );
          }
          else
          {
            std::cerr << "unknown option " << *option << std::endl;
          }

          ++option;
        }
      }

      static void
      loadLongOption( ArgV::iterator& i, T& setup, std::string& requiredOptions )
      {
        const std::string& arg = *i;

        if( i->size() < 2 )
        {
          std::cerr << "missing option " << arg << std::endl;
          return;
        }

        const std::string& option = i->substr(2);

  #if defined HAVE_JSON
        if( option == "json" )
        {
          ++i;

          std::ifstream in(i->c_str());

          if( in )
          {
            json::Object object = json::Object::scan( in );
            setup.fromJSON( object );
            requiredOptions.clear();
            setup.jsonFile = *i;
          }
          else
          {
            std::cerr << "missing filename " << option << std::endl;
          }
          return;
        }
  #endif

        typename LongOptions::iterator o = longOptions.find( option );

        if( o != longOptions.end() )
        {
          o->second->clone( i )->process( setup );
        }
        else
        {
          std::cerr << "unknown option " << option << std::endl;
        }
      }

      static void
      load( ArgV& args, T& setup, std::string& requiredOptions )
      {
        for( ArgV::iterator i = args.begin(); i != args.end(); )
        {
          const std::string& arg = *i;
          std::string::const_iterator option = arg.begin();

  #if defined HAVE_JSON
          if( *option == '{' )
          {
            json::Object object = json::Object::scan( arg );
            setup.fromJSON( object );
            return;
          }
  #endif
          if( *option == '-' )
          {
            ArgV::iterator first(i);

            ++option;

            if( *option == '-' )
            {
              loadLongOption( i, setup, requiredOptions );
            }
            else
            {
              loadShortOption( i, setup, requiredOptions );
            }

            i = args.erase( first, i+1 );
          }
          else
          {
            ++i;
          }
        }
        if( !requiredOptions.empty() )
        {
          std::cerr << "Required option(s) missing " << requiredOptions
                    << std::endl;
          displayMissingOptions(requiredOptions);
          exit( EXIT_FAILURE );
        }
      }
    };

    // User defines enum OptionType {};
    //
    template <typename T, OptionType O>
    struct SimpleOption : Option<T>
    {
      Option<T>* clone( ArgV::iterator& ) { return new SimpleOption(); }
      std::string what() const { return  Option<T>::info[O]; }

      void process( T& ) const {}
    };

    template <typename ParamType> inline ParamType
    convert( const std::string& s )
    {
      ParamType param;
      std::istringstream in(s);
      in >> param;
      return param;
    }

    template <> inline std::string
    convert<std::string>( const std::string& s )
    {
      return s;
    }

    template <> inline Strings
    convert<Strings>( const std::string& s )
    {
      Strings strings;
      strings.push_back(s);
      return strings;
    }

    template <> inline Shorts
    convert<Shorts>( const std::string& s )
    {
      Shorts numbers;
      numbers.push_back(convert<Shorts::value_type>(s));
      return numbers;
    }

    template <> inline Ints
    convert<Ints>( const std::string& s )
    {
      Ints numbers;
      numbers.push_back(convert<Ints::value_type>(s));
      return numbers;
    }

    template <typename T, OptionType O, typename ParamType>
    struct ParameterisedOption : SimpleOption<T,O>
    {
      ParamType param0;

      ParameterisedOption() {}

      ParameterisedOption( ArgV::iterator& i )
      {
        param0 = convert<ParamType>(*++i);
      }

      Option<T>* clone( ArgV::iterator& i ) { return new ParameterisedOption(i); }

      void process( T& ) const {}
    };

    template <typename T, OptionType O, typename Type1, typename Type2>
    struct DoubleParameterisedOption : ParameterisedOption<T,O,Type1>
    {
      Type1 param1;

      DoubleParameterisedOption()
        : ParameterisedOption<T,O,Type1>() {}

      DoubleParameterisedOption( ArgV::iterator&  i )
        : ParameterisedOption<T,O,Type1>( i )
      {
        std::istringstream in(*++i);
        in >> param1;
      }

      Option<T>* clone( ArgV::iterator& i )
      {
        return new DoubleParameterisedOption(i);
      }

      void process( T& ) const;
    };

    template <char C, typename T, typename Opt> void
    loadOption( const std::string& s )
    {
      Option<Opt>::options[C]     = typename Option<Opt>::OptionPtr( new T() );
      Option<Opt>::longOptions[s] = typename Option<Opt>::OptionPtr( new T() );
    }

    template <typename T, typename Opt> void
    loadOption( const std::string& s )
    {
      Option<Opt>::longOptions[s] = typename Option<Opt>::OptionPtr( new T() );
    }

    extern void loadOptions();
  }
}

#define SIMPLE_OPTION( opt ) \
  typedef system::SimpleOption<OptionParamerType,system:: opt> opt ## Option

#define PARAMATER_OPTION( opt, param )                                    \
  typedef system::ParameterisedOption<OptionParamerType,system:: opt,param> opt ## Option

#define DOUBLE_PARAMATER_OPTION( opt, param1, param2)                                    \
  typedef system::DoubleParameterisedOption<OptionParamerType,system:: opt,param1,param2> opt ## Option

#endif
