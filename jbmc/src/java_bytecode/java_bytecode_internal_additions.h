/*******************************************************************\

Module:

Author: Daniel Kroening, kroening@kroening.com

\*******************************************************************/


#ifndef CPROVER_JAVA_BYTECODE_JAVA_BYTECODE_INTERNAL_ADDITIONS_H
#define CPROVER_JAVA_BYTECODE_JAVA_BYTECODE_INTERNAL_ADDITIONS_H

#include <util/symbol_table_base.h>

/// Adds `__cprover_initialize` to the \p symbol_table but does not generate
/// code for it yet.
void create_java_initialize(symbol_table_baset &symbol_table);

void java_internal_additions(symbol_table_baset &dest);

#endif // CPROVER_JAVA_BYTECODE_JAVA_BYTECODE_INTERNAL_ADDITIONS_H
