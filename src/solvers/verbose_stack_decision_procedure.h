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
    std::cout << "`handle` called with expr:\n  " << expr.pretty(2, 0) << "\n";
    auto result = base_solvert::handle(expr);
    std::cout << "Value returned from `handle`:\n  " << result.pretty(2, 0)
              << "\n";
    return result;
  }

  void set_to(const exprt &expr, bool value) override
  {
    std::cout << "`set_to` (" << (value ? "true" : "false")
              << ") with expr:\n  " << expr.pretty(2, 0) << "\n";
    base_solvert::set_to(expr, value);
  }

  exprt get(const exprt &expr) const override
  {
    std::cout << "`get` called with expr:\n  " << expr.pretty(2, 0) << "\n";
    auto result = base_solvert::get(expr);
    std::cout << "Value returned from `get`:\n  " << result.pretty(2, 0)
              << "\n";
    return result;
  }

  std::string decision_procedure_text() const override
  {
    std::cout << "`decision_procedure_text` called\n";
    auto result = base_solvert::decision_procedure_text();
    std::cout << "Value returned from `decision_procedure_text`:\n  " << result
              << "\n";
    // Do we want to change the test to reflect the verbose wrapper?
    return result;
  }

  void print_assignment(std::ostream &out) const override
  {
    // It's not clear we want to over-ride this.
    base_solvert::print_assignment(out);
  }

  void push() override
  {
    std::cout << "`push()` called\n";
    base_solvert::push();
  }

  void push(const std::vector<exprt> &_assumptions) override
  {
    std::cout << "`push` called with exprs:\n  ";
    for(auto _ass : _assumptions)
    {
      std::cout << _ass.pretty(2, 0) << "\n";
    }
    base_solvert::push(_assumptions);
  }

  void pop() override
  {
    std::cout << "`pop()` called\n";
    base_solvert::pop();
  }

  std::size_t get_number_of_solver_calls() const override
  {
    std::cout << "`get_number_of_solver_calls` called\n  ";
    auto result = base_solvert::get_number_of_solver_calls();
    std::cout << "Value returned from `get_number_of_solver_calls`:\n  "
              << result << "\n";
    return result;
  }
};

#endif //CBMC_VERBOSE_STACK_DECISION_PROCEDURE_H
