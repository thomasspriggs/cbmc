// Author: Diffblue Ltd.

#ifndef CPROVER_SOLVERS_SMT2_INCREMENTAL_CONSTRUCT_VALUE_EXPR_FROM_SMT_H
#define CPROVER_SOLVERS_SMT2_INCREMENTAL_CONSTRUCT_VALUE_EXPR_FROM_SMT_H

#include <util/expr.h>

class smt_termt;
class typet;

/// \brief Given a \p value_term and a \p type_to_construct, this function
/// constructs a value exprt with a value based on \p value_term and a type of
/// \p type_to_construct.
/// \note The type is required separately in order to carry out this conversion,
/// because the smt value term does not contain all the required information.
/// For example an 8 bit, bit vector with a value of 255 could be used to
/// construct an `unsigned char` with the value 255 or alternatively a
/// `signed char` with the value -1. So these alternatives are disambiguated
/// using the type.
exprt construct_value_expr_from_smt(
  const smt_termt &value_term,
  const typet &type_to_construct);

#endif // CPROVER_SOLVERS_SMT2_INCREMENTAL_CONSTRUCT_VALUE_EXPR_FROM_SMT_H
