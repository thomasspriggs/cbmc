
%name-prefix "java_signature_"

%code requires {
#include "parse_tree.h"
#include <memory>
}

%parse-param {std::shared_ptr<java_signature_baset> &result}
%initial-action {result = {};}

%{

#include "parser.h"
#include "java_signature_lex.yy.h"
#include <memory>

#define YYSTYPE std::shared_ptr<java_signature_baset>

%}

%token TOK_JAVA_SIGNATURE_IDENTIFIER

%%
TopLevel
  : FieldTypeSignature { result = $1; }
  ;

FieldTypeSignature
  : ClassTypeSignature
  | ArrayTypeSignature
  | TypeVariableSignature
  ;

ClassTypeSignature
//  : 'L' SimpleClassTypeSignature {ClassTypeSignatureSuffix} ';'
  : 'L' SimpleClassTypeSignature ';' { $$ = $2; }
  ;

SimpleClassTypeSignature
  : Identifier TypeArgumentsOptional
  {
    $$ = std::make_shared<java_signature_simple_class_typet>
    (
      std::move($1),
      checked_cast<java_signature_type_argumentst>(*$2)
    );
  }
  ;

TypeVariableSignature
  : 'T' Identifier ';'
  {
    $$ = std::make_shared<java_signature_type_variablet>(std::move($2));
  }
  ;

Identifier
  : TOK_JAVA_SIGNATURE_IDENTIFIER
  {
    $$ = std::make_shared<java_signature_identifiert>(java_signature_text);
  }
  ;

TypeArgumentsOptional
  : '<' TypeArguments '>'
  {
    $$ = $2;
  }
  | %empty
  {
    $$ = std::make_shared<java_signature_type_argumentst>
    (
      std::forward_list<java_signature_type_argumentt>{}
    );
  }
  ;

TypeArguments
  : TypeArgument
  {
    $$ = std::make_shared<java_signature_type_argumentst>
    (
      std::forward_list<java_signature_type_argumentt>
      {
        std::move(checked_cast<java_signature_type_argumentt>(*$1))
      }
    );
  }
  | TypeArgument TypeArguments
  {
    auto arguments = checked_cast<java_signature_type_argumentst>($2);
    arguments->collection.push_front
    (
      std::move(checked_cast<java_signature_type_argumentt>(*$1))
    );
    $$ = std::move(arguments);
  }
  ;

TypeArgument
  : WildcardIndicatorOptional FieldTypeSignature
  {
    $$ = std::make_shared<java_signature_type_argumentt>
    (
      std::move($1),
      std::move($2)
    );
  }
  | '*'
  {
    $$ = std::make_shared<java_signature_type_argumentt>
    (
      std::make_shared<java_signature_wildcard_indicatort<'*'>>(),
      nullptr
    );
  }
  ;

WildcardIndicatorOptional
  : '+' { $$ = std::make_shared<java_signature_wildcard_indicatort<'+'>>(); }
  | '-' { $$ = std::make_shared<java_signature_wildcard_indicatort<'-'>>(); }
  | %empty { $$ = {}; }
  ;

ArrayTypeSignature
  : '[' TypeSignature
  {
    $$ = std::make_shared<java_signature_array_typet>(std::move($2));
  }
  ;

TypeSignature
  : FieldTypeSignature
  | BaseType
  ;

BaseType
  : 'B' { $$ = std::make_shared<java_signature_primitivet<'B'>>(); }
  | 'C' { $$ = std::make_shared<java_signature_primitivet<'C'>>(); }
  | 'D' { $$ = std::make_shared<java_signature_primitivet<'D'>>(); }
  | 'F' { $$ = std::make_shared<java_signature_primitivet<'F'>>(); }
  | 'I' { $$ = std::make_shared<java_signature_primitivet<'I'>>(); }
  | 'J' { $$ = std::make_shared<java_signature_primitivet<'J'>>(); }
  | 'S' { $$ = std::make_shared<java_signature_primitivet<'S'>>(); }
  ;
