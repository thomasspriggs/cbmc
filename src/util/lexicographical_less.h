//
// Created by tspriggs on 04/12/2020.
//

#ifndef PROVER_UTIL_LEXICOGRAPHICAL_LESS_H
#define PROVER_UTIL_LEXICOGRAPHICAL_LESS_H

#include <algorithm>
#include <iterator>

#include <util/as_const.h>

template <typename lesst>
struct lexicographical_lesst
{
  template <typename list1t, typename list2t>
  bool operator()(list1t &&left, list2t &&right) const
  {
    return std::lexicographical_compare(
      std::begin(as_const(left)),
      std::end(as_const(left)),
      std::begin(as_const(right)),
      std::end(as_const(right)),
      lesst{});
  }
};

#endif // PROVER_UTIL_LEXICOGRAPHICAL_LESS_H
