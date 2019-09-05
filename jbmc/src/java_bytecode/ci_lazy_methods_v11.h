
#include <goto-programs/class_hierarchy.h>
#include <goto-programs/lazy_goto_model.h>
#include <java_bytecode/ci_lazy_methods.h> // for load_extra_methodst
#include <util/message.h>

void ci_lazy_methods_v11(
  lazy_goto_modelt &,
  const std::vector<load_extra_methodst> &,
  const class_hierarchyt &,
  const select_pointer_typet &,
  message_handlert &);
