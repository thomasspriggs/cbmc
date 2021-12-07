// Author: Diffblue Ltd.

#include <testing-utils/use_catch.h>

#include <solvers/smt2_incremental/construct_value_expr_from_smt.h>

#include <solvers/smt2_incremental/smt_terms.h>
#include <solvers/smt2_incremental/smt_to_smt2_string.h>

#include <util/arith_tools.h>
#include <util/bitvector_types.h>
#include <util/std_expr.h>
#include <util/std_types.h>

TEST_CASE("Value expr construction from smt.", "[core][smt2_incremental]")
{
  optionalt<smt_termt> input_term;
  optionalt<exprt> expected_result;

  using rowt = std::pair<smt_termt, exprt>;
  std::tie(input_term, expected_result) = GENERATE(
    // Boolean tests.
    rowt{smt_bool_literal_termt{true}, true_exprt{}},
    rowt{smt_bool_literal_termt{false}, false_exprt{}},
    // 8 bit integer tests.
    rowt{smt_bit_vector_constant_termt{0, 8},
         from_integer(0, unsignedbv_typet{8})},
    rowt{smt_bit_vector_constant_termt{255, 8},
         from_integer(255, unsignedbv_typet{8})},
    rowt{smt_bit_vector_constant_termt{255, 8},
         from_integer(-1, signedbv_typet{8})},
    rowt{smt_bit_vector_constant_termt{0, 8},
         from_integer(0, signedbv_typet{8})},
    rowt{smt_bit_vector_constant_termt{42, 8},
         from_integer(42, signedbv_typet{8})},
    // 16 bit integer tests.
    rowt{smt_bit_vector_constant_termt{0, 16},
         from_integer(0, unsignedbv_typet{16})},
    rowt{smt_bit_vector_constant_termt{65535, 16},
         from_integer(65535, unsignedbv_typet{16})},
    rowt{smt_bit_vector_constant_termt{65535, 16},
         from_integer(-1, signedbv_typet{16})},
    rowt{smt_bit_vector_constant_termt{0, 16},
         from_integer(0, signedbv_typet{16})},
    rowt{smt_bit_vector_constant_termt{42, 16},
         from_integer(42, signedbv_typet{16})});
  SECTION(
    "Construction of \"" + id2string(expected_result->type().id()) +
    "\" from \"" + smt_to_smt2_string(*input_term) + "\"")
  {
    REQUIRE(
      construct_value_expr_from_smt(*input_term, expected_result->type()) ==
      *expected_result);
  }
}
