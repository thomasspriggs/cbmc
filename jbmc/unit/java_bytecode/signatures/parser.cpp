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
    java_signature_parse({test_typet::signature()})));
}
