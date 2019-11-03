/*******************************************************************\

Module:

Author: Diffblue Ltd.

\*******************************************************************/

#include <iostream>

#include <testing-utils/use_catch.h>
#include <java_bytecode/signatures/parser.h>
#include <java_bytecode/signatures/parse_tree.h>

TEST_CASE("Parse type variable java signature",
  "[core][java_bytecode][java_signature_parser]")
{
  REQUIRE(
    std::dynamic_pointer_cast<java_signature_type_variablet>(
      java_signature_parse("Tfish;"))
      ->identifier == "fish");
}

TEMPLATE_TEST_CASE(
  "Parse primitive java signatures",
  "[core][java_bytecode][java_signature_parser]",
  (java_signature_primitivet<'B'>),
  (java_signature_primitivet<'C'>),
  (java_signature_primitivet<'D'>),
  (java_signature_primitivet<'F'>),
  (java_signature_primitivet<'I'>),
  (java_signature_primitivet<'J'>),
  (java_signature_primitivet<'S'>))
{
  REQUIRE(std::dynamic_pointer_cast<test_typet>(
    java_signature_parse(std::string{test_typet::signature()})));
}

TEST_CASE("Parse non-generic class type java signature",
  "[core][java_bytecode][java_signature_parser]")
{
  const auto simple_class_type =
    std::dynamic_pointer_cast<java_signature_simple_class_typet>(
      java_signature_parse("Lcod;"));
  REQUIRE(simple_class_type);
  REQUIRE(simple_class_type->identifier == "cod");
  REQUIRE(simple_class_type->arguments.empty());
}

TEST_CASE("Parse generic class type java signature with one parameter",
  "[core][java_bytecode][java_signature_parser]")
{
  const auto simple_class_type =
    std::dynamic_pointer_cast<java_signature_simple_class_typet>(
      java_signature_parse("Lbadger<TT;>;"));
  REQUIRE(simple_class_type);
  REQUIRE(simple_class_type->identifier == "badger");
  REQUIRE(simple_class_type->arguments.size() == 1);
  const auto type_variable =
    std::dynamic_pointer_cast<java_signature_type_variablet>(
      simple_class_type->arguments[0].field_type);
  REQUIRE(type_variable);
  REQUIRE(type_variable->identifier == "T");
}

TEST_CASE("Parse generic class type java signature with one argument",
  "[core][java_bytecode][java_signature_parser]")
{
  const auto simple_class_type =
    std::dynamic_pointer_cast<java_signature_simple_class_typet>(
      java_signature_parse("Lbadger<Lpotato;>;"));
  REQUIRE(simple_class_type);
  REQUIRE(simple_class_type->identifier == "badger");
  REQUIRE(simple_class_type->arguments.size() == 1);
  CHECK_FALSE(simple_class_type->arguments[0].wildcard);
  const auto type_argument =
    std::dynamic_pointer_cast<java_signature_simple_class_typet>(
      simple_class_type->arguments[0].field_type);
  REQUIRE(type_argument);
  REQUIRE(type_argument->identifier == "potato");
}

TEST_CASE("Parse generic class type java signature with one * argument",
  "[core][java_bytecode][java_signature_parser]")
{
  const auto simple_class_type =
    std::dynamic_pointer_cast<java_signature_simple_class_typet>(
      java_signature_parse("Lbadger<*>;"));
  REQUIRE(simple_class_type);
  REQUIRE(simple_class_type->identifier == "badger");
  REQUIRE(simple_class_type->arguments.size() == 1);
  CHECK_FALSE(simple_class_type->arguments[0].field_type);
  REQUIRE(std::dynamic_pointer_cast<java_signature_wildcard_indicatort<'*'>>(
    simple_class_type->arguments[0].wildcard));
}

TEST_CASE("Parse generic class type java signature with two parameters",
  "[core][java_bytecode][java_signature_parser]")
{
  const auto simple_class_type =
    std::dynamic_pointer_cast<java_signature_simple_class_typet>(
      java_signature_parse("Lspam<TT;TK;>;"));
  REQUIRE(simple_class_type);
  REQUIRE(simple_class_type->identifier == "spam");
  REQUIRE(simple_class_type->arguments.size() == 2);
  CHECK_FALSE(simple_class_type->arguments[0].wildcard);
  const auto first_type_variable =
    std::dynamic_pointer_cast<java_signature_type_variablet>(
      simple_class_type->arguments[0].field_type);
  REQUIRE(first_type_variable);
  REQUIRE(first_type_variable->identifier == "T");
  CHECK_FALSE(simple_class_type->arguments[1].wildcard);
  const auto second_type_variable =
    std::dynamic_pointer_cast<java_signature_type_variablet>(
      simple_class_type->arguments[1].field_type);
  REQUIRE(second_type_variable);
  REQUIRE(second_type_variable->identifier == "K");
}

TEST_CASE("Parse generic class type java signature with two arguments",
  "[core][java_bytecode][java_signature_parser]")
{
  const auto simple_class_type =
    std::dynamic_pointer_cast<java_signature_simple_class_typet>(
      java_signature_parse("Lspam<Leggs;Lham;>;"));
  REQUIRE(simple_class_type);
  REQUIRE(simple_class_type->identifier == "spam");
  REQUIRE(simple_class_type->arguments.size() == 2);
  const auto first_type_argument =
    std::dynamic_pointer_cast<java_signature_simple_class_typet>(
      simple_class_type->arguments[0].field_type);
  REQUIRE(first_type_argument);
  REQUIRE(first_type_argument->identifier == "eggs");
  const auto second_type_argument =
    std::dynamic_pointer_cast<java_signature_simple_class_typet>(
      simple_class_type->arguments[1].field_type);
  REQUIRE(second_type_argument);
  REQUIRE(second_type_argument->identifier == "ham");
}

TEST_CASE("Parse generic class type java signature with wildcard parameters",
  "[core][java_bytecode][java_signature_parser]")
{
  const auto simple_class_type =
    std::dynamic_pointer_cast<java_signature_simple_class_typet>(
      java_signature_parse("Lspam<+TV;-TV;>;"));
  REQUIRE(simple_class_type);
  REQUIRE(simple_class_type->identifier == "spam");
  REQUIRE(simple_class_type->arguments.size() == 2);
  CHECK(std::dynamic_pointer_cast<java_signature_wildcard_indicatort<'+'>>(
    simple_class_type->arguments[0].wildcard));
  const auto first_type_variable =
    std::dynamic_pointer_cast<java_signature_type_variablet>(
      simple_class_type->arguments[0].field_type);
  REQUIRE(first_type_variable);
  REQUIRE(first_type_variable->identifier == "V");
  CHECK(std::dynamic_pointer_cast<java_signature_wildcard_indicatort<'-'>>(
    simple_class_type->arguments[1].wildcard));
  const auto second_type_variable =
    std::dynamic_pointer_cast<java_signature_type_variablet>(
      simple_class_type->arguments[1].field_type);
  REQUIRE(second_type_variable);
  REQUIRE(second_type_variable->identifier == "V");
}

TEST_CASE("Parse array of type variable java signature",
  "[core][java_bytecode][java_signature_parser]")
{
  REQUIRE(
    std::dynamic_pointer_cast<java_signature_type_variablet>(
      std::dynamic_pointer_cast<java_signature_array_typet>(
        java_signature_parse("[Tfish;"))
        ->element_type)
      ->identifier == "fish");
}

TEMPLATE_TEST_CASE(
  "Parse primitive array java signatures",
  "[core][java_bytecode][java_signature_parser]",
  (java_signature_primitivet<'B'>),
  (java_signature_primitivet<'C'>),
  (java_signature_primitivet<'D'>),
  (java_signature_primitivet<'F'>),
  (java_signature_primitivet<'I'>),
  (java_signature_primitivet<'J'>),
  (java_signature_primitivet<'S'>))
{
  REQUIRE(std::dynamic_pointer_cast<test_typet>(
    std::dynamic_pointer_cast<java_signature_array_typet>(
      java_signature_parse(std::string{"["} + test_typet::signature()))
      ->element_type));
}

TEST_CASE("Parse array of class java signature",
  "[core][java_bytecode][java_signature_parser]")
{
  REQUIRE(
    std::dynamic_pointer_cast<java_signature_simple_class_typet>(
      std::dynamic_pointer_cast<java_signature_array_typet>(
        java_signature_parse("[Lcod;"))
        ->element_type)
      ->identifier == "cod");
}

TEST_CASE("Parse array of array of type variable java signature",
  "[core][java_bytecode][java_signature_parser]")
{
  REQUIRE(
    std::dynamic_pointer_cast<java_signature_type_variablet>(
      std::dynamic_pointer_cast<java_signature_array_typet>(
        std::dynamic_pointer_cast<java_signature_array_typet>(
          java_signature_parse("[[Tfish;"))
          ->element_type)
        ->element_type)
      ->identifier == "fish");
}

TEMPLATE_TEST_CASE(
  "Parse array of primitive array java signatures",
  "[core][java_bytecode][java_signature_parser]",
  (java_signature_primitivet<'B'>),
  (java_signature_primitivet<'C'>),
  (java_signature_primitivet<'D'>),
  (java_signature_primitivet<'F'>),
  (java_signature_primitivet<'I'>),
  (java_signature_primitivet<'J'>),
  (java_signature_primitivet<'S'>))
{
  REQUIRE(std::dynamic_pointer_cast<test_typet>(
    std::dynamic_pointer_cast<java_signature_array_typet>(
      std::dynamic_pointer_cast<java_signature_array_typet>(
        java_signature_parse(std::string{"[["} + test_typet::signature()))
        ->element_type)
      ->element_type));
}

TEST_CASE(
  "Parse array of array of class java signature",
  "[core][java_bytecode][java_signature_parser]")
{
  REQUIRE(
    std::dynamic_pointer_cast<java_signature_simple_class_typet>(
      std::dynamic_pointer_cast<java_signature_array_typet>(
        std::dynamic_pointer_cast<java_signature_array_typet>(
          java_signature_parse("[[Lcod;"))
          ->element_type)
        ->element_type)
      ->identifier == "cod");
}
