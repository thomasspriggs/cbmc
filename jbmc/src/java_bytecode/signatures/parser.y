
%name-prefix "java_signature_"

%{

#include "parser.h"
#include "parse_tree.h"
#include "java_signature_lex.yy.h"
#include <memory>

#define YYSTYPE std::shared_ptr<java_signature_baset>
extern YYSTYPE java_signature_parsed;

%}

%token TOK_JAVA_SIGNATURE_IDENTIFIER

%%
TopLevel
  : FieldTypeSignature { java_signature_parsed = $1; }
  ;

FieldTypeSignature
  : /*ClassTypeSignature
  | */ ArrayTypeSignature
  | TypeVariableSignature
  ;

/*ClassTypeSignature
  : 'L' SimpleClassTypeSignature {ClassTypeSignatureSuffix} ';'*/

//SimpleClassTypeSignature
//  : Identifier
//  ;

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
