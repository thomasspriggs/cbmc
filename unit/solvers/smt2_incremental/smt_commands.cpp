// Author: Diffblue Ltd.

#include <testing-utils/use_catch.h>

#include <solvers/smt2_incremental/smt_commands.h>

TEST_CASE("smt_commandt upcasts.", "[core][smt2_incremental]")
{
  smt_commandt command{smt_exit_commandt{}};
  command = smt_exit_commandt{};
}

TEST_CASE("Test smt_commandt.pretty is accessible.", "[core][smt2_incremental]")
{
  const smt_commandt check_sat_command{smt_check_sat_commandt{}};
  REQUIRE(check_sat_command.pretty(0, 0) == "smt_check_sat_command");
}

TEST_CASE("smt_assert_commandt condition getter", "[core][smt2_incremental]")
{
  const smt_assert_commandt assert_command{smt_bool_literal_termt{true}};
  REQUIRE(assert_command.condition() == smt_bool_literal_termt{true});
}

TEST_CASE("smt_declare_function_commandt getters", "[core][smt2_incremental]")
{
  const smt_identifier_termt plus{"+", smt_bit_vector_sortt{9}};
  const smt_declare_function_commandt function_declaration{
    plus,
    std::vector<smt_sortt>{smt_bit_vector_sortt{8}, smt_bit_vector_sortt{7}}};
  CHECK(function_declaration.identifier() == plus);
  CHECK(function_declaration.return_sort() == smt_bit_vector_sortt{9});
  CHECK(
    function_declaration.parameter_sorts()[0].get() == smt_bit_vector_sortt{8});
  CHECK(
    function_declaration.parameter_sorts()[1].get() == smt_bit_vector_sortt{7});
}

TEST_CASE("smt_define_function_commandt getters", "[core][smt2_incremental]")
{
  const smt_define_function_commandt function_definition{
    "not first",
    {smt_identifier_termt{"x", smt_bool_sortt{}},
     smt_identifier_termt{"y", smt_bool_sortt{}}},
    smt_not_termt{smt_identifier_termt{"x", smt_bool_sortt{}}}};
  CHECK(function_definition.identifier() == "not first");
  CHECK(
    function_definition.parameters()[0].get() ==
    smt_identifier_termt{"x", smt_bool_sortt{}});
  CHECK(
    function_definition.parameters()[1].get() ==
    smt_identifier_termt{"y", smt_bool_sortt{}});
  CHECK(
    function_definition.definition() ==
    smt_not_termt{smt_identifier_termt{"x", smt_bool_sortt{}}});
}

TEST_CASE("smt_get_value_commandt getter", "[core][smt2_incremental]")
{
  const smt_get_value_commandt get_value_command{
    smt_identifier_termt{"x", smt_bool_sortt{}}};
  REQUIRE(
    get_value_command.descriptor() ==
    smt_identifier_termt{"x", smt_bool_sortt{}});
}

TEST_CASE("smt_commandt equality", "[core][smt2_incremental]")
{
  const smt_exit_commandt exit_command{};
  CHECK(exit_command == smt_exit_commandt{});
  const smt_check_sat_commandt check_sat_command{};
  CHECK(check_sat_command == smt_check_sat_commandt{});
  CHECK_FALSE(exit_command == check_sat_command);
  const smt_assert_commandt assert_true{smt_bool_literal_termt{true}};
  CHECK(assert_true == smt_assert_commandt{smt_bool_literal_termt{true}});
  const smt_assert_commandt assert_false{smt_bool_literal_termt{false}};
  CHECK_FALSE(assert_true == assert_false);
}

TEST_CASE("smt_push_commandt level", "[core][smt2_incremental]")
{
  CHECK(smt_push_commandt(1).levels() == 1);
  CHECK(smt_push_commandt(2).levels() == 2);
}

TEST_CASE("smt_pop_commandt level", "[core][smt2_incremental]")
{
  CHECK(smt_pop_commandt(1).levels() == 1);
  CHECK(smt_pop_commandt(2).levels() == 2);
}

TEST_CASE("smt_set_option_commandt getter", "[core][smt2_incremental]")
{
  const auto models_on = smt_option_produce_modelst{true};
  const auto models_off = smt_option_produce_modelst{false};
  CHECK(models_on.setting() == true);
  CHECK(models_off.setting() == false);
  CHECK(smt_set_option_commandt{models_on}.option() == models_on);
  CHECK(smt_set_option_commandt{models_off}.option() == models_off);
}
