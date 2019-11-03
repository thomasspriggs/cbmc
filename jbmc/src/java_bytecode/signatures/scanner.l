
%option nounput
%option noinput
%option nounistd
%option never-interactive
%option noyywrap
%option prefix="java_signature_"

%{

#include "java_signature_y.tab.h"

#include <java_bytecode/java_types.h>

#include <string>

%}

%x INDENTIFIER_EXPECTED
%%

<INDENTIFIER_EXPECTED>[^;]* {
  BEGIN(INITIAL);
  return TOK_JAVA_SIGNATURE_IDENTIFIER;
}

"L"|"T" {
  BEGIN(INDENTIFIER_EXPECTED);
  return yytext[0];
}

"<"|">"|"B"|"C"|"D"|"F"|"I"|"J"|"S"|"["|";"|"+"|"-"|"*" {
  return yytext[0];
}

<*>. {
  throw unsupported_java_class_signature_exceptiont{
    std::string{"unexpected character \'"} + yytext[0] + '\''};
}
