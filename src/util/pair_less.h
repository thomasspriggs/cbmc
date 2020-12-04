//
// Created by tspriggs on 03/12/2020.
//

#ifndef CPROVER_UTIL_PAIR_LESS_H
#define CPROVER_UTIL_PAIR_LESS_H

template<typename less1t, typename less2t>
struct pair_lesst
{
  template<typename pair1t, typename pair2t>
  bool operator()(pair1t && left, pair2t && right) const
  {
    if(less1t{}(left.first, right.first))
      return true;
    if(less1t{}(right.first, left.first))
      return false;
    if(less2t{}(left.second, right.second))
      return true;
    else
      return false;
  }
};

#endif // CPROVER_UTIL_PAIR_LESS_H
