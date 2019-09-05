
#ifndef CPROVER_JAVA_BYTECODE_CI_LAZY_METHODS_V11_H
#define CPROVER_JAVA_BYTECODE_CI_LAZY_METHODS_V11_H

#include <goto-programs/class_hierarchy.h>
#include <goto-programs/lazy_goto_model.h>
#include <java_bytecode/java_bytecode_parse_tree.h>
#include <java_bytecode/select_pointer_type.h>
#include <util/message.h>

using load_extra_methodst =
  std::function<std::vector<irep_idt>(const symbol_tablet &)>;

// Map from method id to class_method_and_bytecodet
class method_bytecodet
{
public:
  /// Pair of class id and methodt
  struct class_method_and_bytecodet
  {
    irep_idt class_id;
    irep_idt method_id;
    const java_bytecode_parse_treet::methodt &method;
  };

  typedef optionalt<std::reference_wrapper<const class_method_and_bytecodet>>
    opt_reft;

private:
  typedef std::map<irep_idt, class_method_and_bytecodet> mapt;
  mapt map;

public:
  bool contains_method(const irep_idt &method_id) const
  {
    return map.count(method_id) != 0;
  }

  void add(const class_method_and_bytecodet &method_class_and_bytecode)
  {
    map.emplace(
      std::make_pair(
        method_class_and_bytecode.method_id, method_class_and_bytecode));
  }

  void add(
    const irep_idt &class_id,
    const irep_idt &method_id,
    const java_bytecode_parse_treet::methodt &method)
  {
    add(class_method_and_bytecodet{class_id, method_id, method});
  }

  mapt::const_iterator begin() const
  {
    return map.begin();
  }
  mapt::const_iterator end() const
  {
    return map.end();
  }

  opt_reft get(const irep_idt &method_id)
  {
    const auto it = map.find(method_id);
    if(it == map.end())
      return opt_reft();
    return std::cref(it->second);
  }
};

void ci_lazy_methods_v11(
  lazy_goto_modelt &,
  const std::vector<load_extra_methodst> &,
  const class_hierarchyt &,
  const select_pointer_typet &,
  message_handlert &);

#endif
