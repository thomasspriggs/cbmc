/*******************************************************************\

Module: Remove Dead Global Variables

Author: Diffblue Ltd.

\*******************************************************************/

/// \file
/// Remove Dead Global Variables

#ifndef CPROVER_GOTO_PROGRAMS_REMOVE_DEAD_GLOBALS_H
#define CPROVER_GOTO_PROGRAMS_REMOVE_DEAD_GLOBALS_H

#include <goto-programs/lazy_goto_model.h>
#include <util/message.h>

void remove_dead_globals(lazy_goto_modelt &, message_handlert &, const std::vector<irep_idt> &force_keep_globals);

#endif
