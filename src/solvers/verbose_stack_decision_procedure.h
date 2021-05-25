//
// Created by tspriggs on 25/05/2021.
//

#ifndef CBMC_VERBOSE_STACK_DECISION_PROCEDURE_H
#define CBMC_VERBOSE_STACK_DECISION_PROCEDURE_H

#include <solvers/stack_decision_procedure.h>

#include <iostream>

template <typename base_solvert>
class verbose_stack_decision_proceduret : public base_solvert
{
  using base_solvert::base_solvert;

public:
  exprt handle(const exprt &expr) override
  {
    std::cout << "`handle` called with expr:\n  " << expr.pretty(2,0) << "\n";
    auto result = base_solvert::handle(expr);
    std::cout << "Value returned from `handle`:\n  " << expr.pretty(2,0) << "\n";
    return result;
  }

  void set_to(const exprt &expr, bool value) override
  {
    std::cout << "`set_to` (" << (value ? "true" : "false") << ") with expr:\n  " << expr.pretty(2,0) << "\n";
    base_solvert::set_to(expr, value);
  }
};

#endif //CBMC_VERBOSE_STACK_DECISION_PROCEDURE_H
