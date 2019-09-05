
#include <goto-programs/class_hierarchy.h>
#include <goto-programs/lazy_goto_model.h>
#include <java_bytecode/select_pointer_type.h>
#include <util/message.h>

using load_extra_methodst =
  std::function<std::vector<irep_idt>(const symbol_tablet &)>;

void ci_lazy_methods_v11(
  lazy_goto_modelt &,
  const std::vector<load_extra_methodst> &,
  const class_hierarchyt &,
  const select_pointer_typet &,
  message_handlert &);
