/******************************* C++ Header File *******************************
 *
 *  Copyright (c) Masuma Ltd 2016.  All rights reserved.
 *
 *  MODULE:      Utilities
 *
 ******************************************************************************/

#pragma once

#include <tuple>

namespace masuma::utils
{
  template <typename T, typename Tuple>
  struct IndexOfIn;

  template <typename T, typename... Types>
  struct IndexOfIn<T, std::tuple<T, Types...>>
  {
    static constexpr size_t value = 0;
  };

  template <typename T, typename U, typename... Types>
  struct IndexOfIn<T, std::tuple<U, Types...>>
  {
    static constexpr size_t value = 1 + IndexOfIn<T, std::tuple<Types...>>::value;
  };
}

