/*******************************************************************\

Module:

Author: Diffblue Ltd.

\*******************************************************************/

#include <iostream>

#include <testing-utils/use_catch.h>
#include <java_bytecode/signatures/parser.h>
#include <java_bytecode/signatures/parse_tree.h>


TEST_CASE(
  "Parse java signature",
  "[core][java_bytecode][java_signature_parser]")
{
  SECTION("Primitive B")
  {
    REQUIRE(
      checked_cast<java_signature_primitivet<'B'>>(*java_signature_parse("B"))
        .type() == 'B');
  }

  SECTION("Type variable")
  {
    REQUIRE(
      checked_cast<java_signature_type_variablet>(
        *java_signature_parse("Tfish;"))
        .identifier == "fish");
  }
}
