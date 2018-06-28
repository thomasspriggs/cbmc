/*******************************************************************\

 Module: Unit tests for converting annotations

 Author: Diffblue Ltd.

\*******************************************************************/

#include <java-testing-utils/load_java_class.h>
#include <java_bytecode/java_bytecode_convert_class.h>
#include <java_bytecode/java_bytecode_parse_tree.h>
#include <java_bytecode/java_types.h>
#include <testing-utils/catch.hpp>

// See
// https://docs.oracle.com/javase/specs/jvms/se8/html/jvms-4.html#jvms-4.7.16.1
SCENARIO(
  "java_bytecode_parse_attributes",
  "[core][java_bytecode][java_bytecode_parser]")
{
  GIVEN("Some class files in the class path")
  {
    WHEN("Parsing the InnerClasses attribute for a public inner class")
    {
      const symbol_tablet &new_symbol_table =
        load_java_class("InnerClasses", "./java_bytecode/java_bytecode_parser");

      THEN("The class should be marked as public")
      {
        const symbolt &class_symbol =
          *new_symbol_table.lookup("java::InnerClasses$PublicInnerClass");
        const java_class_typet java_class =
          to_java_class_type(class_symbol.type);
        REQUIRE(java_class.get_inner_class() == ID_is_inner_class);
        REQUIRE(java_class.get_access() == ID_public);
      }
    }
    WHEN("Parsing the InnerClasses attribute for a package private inner class")
    {
      const symbol_tablet &new_symbol_table =
        load_java_class("InnerClasses", "./java_bytecode/java_bytecode_parser");

      THEN("The class should be marked as default")
      {
        const symbolt &class_symbol =
          *new_symbol_table.lookup("java::InnerClasses$DefaultInnerClass");
        const java_class_typet java_class =
          to_java_class_type(class_symbol.type);
        REQUIRE(java_class.get_inner_class() == ID_is_inner_class);
        REQUIRE(java_class.get_access() == ID_default);
      }
    }
    WHEN("Parsing the InnerClasses attribute for a protected inner class")
    {
      const symbol_tablet &new_symbol_table =
        load_java_class("InnerClasses", "./java_bytecode/java_bytecode_parser");

      THEN("The class should be marked as protected")
      {
        const symbolt &class_symbol =
          *new_symbol_table.lookup("java::InnerClasses$ProtectedInnerClass");
        const java_class_typet java_class =
          to_java_class_type(class_symbol.type);
        REQUIRE(java_class.get_inner_class() == ID_is_inner_class);
        REQUIRE(java_class.get_access() == ID_protected);
      }
    }
    WHEN("Parsing the InnerClasses attribute for a private inner class")
    {
      const symbol_tablet &new_symbol_table =
        load_java_class("InnerClasses", "./java_bytecode/java_bytecode_parser");

      THEN("The class should be marked as private")
      {
        const symbolt &class_symbol =
          *new_symbol_table.lookup("java::InnerClasses$PrivateInnerClass");
        const java_class_typet java_class =
          to_java_class_type(class_symbol.type);
        REQUIRE(java_class.get_inner_class() == ID_is_inner_class);
        REQUIRE(java_class.get_access() == ID_private);
      }
    }
  }
}
