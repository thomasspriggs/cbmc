/*******************************************************************\

Module:

Author: Diffblue Ltd.

\*******************************************************************/

#ifndef CPROVER_JAVA_BYTECODE_SIGNATURES_PARSER_H
#define CPROVER_JAVA_BYTECODE_SIGNATURES_PARSER_H

#include "parse_tree.h"

#include <string>
#include <memory>

int java_signature_error(const std::string &error);

std::shared_ptr<java_signature_baset>
java_signature_parse(const std::string &input);

#endif // CPROVER_JAVA_BYTECODE_SIGNATURES_PARSER_H
