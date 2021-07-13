// Author: Diffblue Ltd.

#ifndef CPROVER_SOLVERS_SMT2_INCREMENTAL_SMT_TERMS_H
#define CPROVER_SOLVERS_SMT2_INCREMENTAL_SMT_TERMS_H

#include <solvers/smt2_incremental/smt_sorts.h>
#include <util/irep.h>

#include <functional>

class BigInt;
using mp_integer = BigInt;

class smt_sortt;
class smt_term_const_downcast_visitort;

class smt_termt : protected irept, private smt_sortt::storert<smt_termt>
{
public:
  // smt_termt does not support the notion of an empty / null state. Use
  // optionalt<smt_termt> instead if an empty term is required.
  smt_termt() = delete;

  using irept::pretty;

  bool operator==(const smt_termt &) const;
  bool operator!=(const smt_termt &) const;

  const smt_sortt &get_sort() const;

  void accept(smt_term_const_downcast_visitort &) const;
  void accept(smt_term_const_downcast_visitort &&) const;

protected:
  smt_termt(irep_idt id, smt_sortt sort);
};

class smt_bool_literal_termt : public smt_termt
{
public:
  explicit smt_bool_literal_termt(bool value);
  bool value() const;
};

class smt_not_termt : public smt_termt
{
public:
  explicit smt_not_termt(smt_termt operand);
  const smt_termt &operand() const;
};

/// Stores identifiers in unescaped and unquoted form. Any escaping or quoting
/// required should be performed during printing.
class smt_identifier_termt : public smt_termt
{
public:
  /// \brief Constructs an identifier term with the given \p identifier and
  ///   \p sort.
  /// \param identifier: This should be the unescaped form of the identifier.
  ///   Escaping of the identifier if required is to be performed as part of the
  ///   printing operation. The given identifier is checked to ensure that it
  ///   has not already been escaped, in order to avoid escaping it twice.
  ///   Performing escaping during the printing will ensure that the identifiers
  ///   output conform to the specification of the concrete form.
  /// \param sort: The sort which this term will have. All terms in our abstract
  ///   form must be sorted, even if those sorts are not printed in all
  ///   contexts.
  explicit smt_identifier_termt(irep_idt identifier, smt_sortt sort);
  irep_idt identifier() const;
};

class smt_bit_vector_constant_termt : public smt_termt
{
public:
  smt_bit_vector_constant_termt(const mp_integer &value, smt_bit_vector_sortt);
  smt_bit_vector_constant_termt(const mp_integer &value, std::size_t bit_width);
  mp_integer value() const;

  // This deliberately hides smt_termt::get_sort, because bit_vector terms
  // always have bit_vector sorts. The same sort data is returned for both
  // functions either way. Therefore this hiding is benign if the kind of sort
  // is not important and useful for avoiding casts if the term is a known
  // smt_bit_vector_constant_termt at compile time and the `bit_width()` is
  // needed.
  const smt_bit_vector_sortt &get_sort() const;
};

class smt_function_application_termt : public smt_termt
{
public:
  smt_function_application_termt(
    smt_identifier_termt function_identifier,
    std::vector<smt_termt> arguments);
  const smt_identifier_termt &function_identifier() const;
  std::vector<std::reference_wrapper<const smt_termt>> arguments() const;
};

class smt_term_const_downcast_visitort
{
public:
#define TERM_ID(the_id) virtual void visit(const smt_##the_id##_termt &) = 0;
#include "smt_terms.def"
#undef TERM_ID
};

#endif // CPROVER_SOLVERS_SMT2_INCREMENTAL_SMT_TERMS_H
