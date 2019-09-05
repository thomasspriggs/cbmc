
#include "ci_lazy_methods_v11.h"
#include "remove_dead_globals.h"

#include <goto-programs/resolve_inherited_component.h>
#include <java_bytecode/ci_lazy_methods_needed.h>
#include <java_bytecode/remove_exceptions.h>
#include <linking/static_lifetime_init.h>
#include <test-gen-util/algorithm/insert_all.h>
#include <test-gen-util/as.h>
#include <test-gen-util/functors/pair.h>
#include <test-java-gen/smart_select_pointer_type.h>
#include <util/range.h>

bool add_instantiated_type(
  const irep_idt &instantiated_type,
  std::unordered_set<irep_idt> &methods_to_convert_later,
  std::unordered_set<irep_idt> &instantiated_classes,
  const symbol_tablet &symbol_table,
  const class_hierarchyt &class_hierarchy)
{
  if(!instantiated_classes.insert(instantiated_type).second)
    return false;

  const std::string &class_name_string = id2string(instantiated_type);
  const irep_idt cprover_validate(
    class_name_string + ".cproverNondetInitialize:()V");
  if(symbol_table.symbols.count(cprover_validate))
    methods_to_convert_later.insert(cprover_validate);

  // Special case for enums. We may want to generalise this, the comment in
  // \ref java_object_factoryt::gen_nondet_pointer_init (TG-4689).
  // This happens when an Enum type is returned from a stub or similar: we want
  // its VALUES array to be preserved (referred to in its clinit method) so we
  // can initialise nondet instances.
  // Once our stubs are created as we go (cf. making them after loading is
  // complete) this can go away.
  namespacet ns(symbol_table);
  const auto &class_type =
    to_java_class_type(ns.lookup(instantiated_type).type);
  if(class_type.get_base("java::java.lang.Enum"))
  {
    const irep_idt &clinit_wrapper = clinit_wrapper_name(instantiated_type);
    if(symbol_table.has_symbol(clinit_wrapper))
      methods_to_convert_later.insert(clinit_wrapper);
  }

  return true;
}

bool gather_needed_methods_and_types(
  const codet &method_body,
  std::unordered_set<irep_idt> &methods_to_convert_later,
  std::unordered_set<irep_idt> &instantiated_classes,
  std::unordered_set<exprt, irep_hash> &virtual_function_calls,
  const symbol_tablet &symbol_table,
  const class_hierarchyt &class_hierarchy)
{
  bool anything_changed = false;

  method_body.visit_pre([&](const exprt &subexpr)
  {
    if(const auto function_call = as<code_function_callt>(subexpr))
    {
      const auto &callee = function_call->function();
      if(const auto symbol = as<symbol_exprt>(callee))
      {
        anything_changed |=
          methods_to_convert_later.insert(symbol->get_identifier()).second;
      }
      else if(callee.id() == ID_virtual_function)
      {
        anything_changed |= virtual_function_calls.insert(callee).second;
      }
    }
    else if(const auto side_effect = as<side_effect_exprt>(subexpr))
    {
      if(side_effect->get_statement() == ID_java_new ||
        side_effect->get_statement() == ID_allocate)
      {
        if(const auto instantiated_type = as<struct_tag_typet>(
             to_pointer_type(side_effect->type()).subtype()))
        {
          anything_changed |= add_instantiated_type(
            instantiated_type->get_identifier(), methods_to_convert_later, instantiated_classes, symbol_table, class_hierarchy);
        }
      } else if(side_effect->get_statement() == ID_function_call)
      {
        const auto &call_expr = to_side_effect_expr_function_call(*side_effect);
        const symbol_exprt &fn_sym =
          expr_dynamic_cast<symbol_exprt>(call_expr.function());
        methods_to_convert_later.insert(fn_sym.get_identifier());
      }
    }
  });

  return anything_changed;
}

bool convert_and_analyze_method(
  lazy_goto_modelt &goto_model,
  const irep_idt &method_name,
  std::unordered_set<irep_idt> &methods_to_convert_later,
  std::unordered_set<irep_idt> &instantiated_classes,
  std::unordered_set<exprt, irep_hash> &virtual_function_calls,
  const class_hierarchyt &class_hierarchy)
{
  const auto &symbol_table = goto_model.get_symbol_table();

  // Called to cause conversion; we don't actually need the GOTO version of this
  // method body right now...
  (void)goto_model.get_goto_function(method_name);

  const auto &method_symbol = symbol_table.lookup_ref(method_name);
  const auto method_code = as<codet>(method_symbol.value);

  bool anything_new = false;
  if(method_code)
  {
    anything_new = gather_needed_methods_and_types(
      *method_code, methods_to_convert_later, instantiated_classes, virtual_function_calls,
    symbol_table, class_hierarchy);
  }
  else
  {
    // Once we're producing stubs as we go, we won't need this special case, but
    // for now we make some assumptions about what the eventually-produced stub
    // might do:
    if(const auto return_pointer = as<pointer_typet>(
         to_code_type(method_symbol.type).return_type()))
    {
      smart_select_pointer_typet select_pointer_type{true};
      ci_lazy_methods_neededt stub_methods_and_types{
        methods_to_convert_later, instantiated_classes, symbol_table, select_pointer_type};
      stub_methods_and_types.add_all_needed_classes(*return_pointer);
      anything_new = true;
    }
  }

  return anything_new;
}

optionalt<irep_idt> get_virtual_method_target(
  const std::unordered_set<irep_idt> &instantiated_classes,
  const irep_idt &call_basename,
  const irep_idt &classname,
  const symbol_tablet &symbol_table,
  const class_hierarchyt &class_hierarchy)
{
  // Program-wide, is this class ever instantiated?
  if(!instantiated_classes.count(classname))
    return {};

  resolve_inherited_componentt call_resolver(symbol_table, class_hierarchy);
  const auto resolved_call = call_resolver(classname, call_basename, false);

  if(resolved_call)
    return resolved_call->get_full_component_identifier();
  else
    return {};
}

void get_virtual_method_targets(
  const exprt &called_function,
  const std::unordered_set<irep_idt> &instantiated_classes,
  std::unordered_set<irep_idt> &callable_methods,
  const symbol_tablet &symbol_table,
  const class_hierarchyt &class_hierarchy)
{
  PRECONDITION(called_function.id()==ID_virtual_function);

  const auto &call_class=called_function.get(ID_C_class);
  INVARIANT(
    !call_class.empty(), "All virtual calls should be aimed at a class");
  const auto &call_basename=called_function.get(ID_component_name);
  INVARIANT(
    !call_basename.empty(),
    "Virtual function must have a reasonable name after removing class");

  class_hierarchyt::idst self_and_child_classes =
    class_hierarchy.get_children_trans(call_class);
  self_and_child_classes.push_back(call_class);

  for(const irep_idt &class_name : self_and_child_classes)
  {
    const auto method_name = get_virtual_method_target(
      instantiated_classes, call_basename, class_name, symbol_table, class_hierarchy);
    if(method_name)
      callable_methods.insert(*method_name);
  }
}

/// Look for virtual callsites with no candidate targets. If we have
/// invokevirtual A.f and we don't believe either A or any of its children
/// may exist, we assume specifically A is somehow instantiated. Note this
/// may result in an abstract class being classified as instantiated, which
/// stands in for some unknown concrete subclass: in this case the called
/// method will be a stub.
/// \return whether a new class was encountered
static bool handle_virtual_methods_with_no_callees(
  std::unordered_set<irep_idt> &methods_to_convert_later,
  std::unordered_set<irep_idt> &instantiated_classes,
  const std::unordered_set<exprt, irep_hash> &virtual_function_calls,
  symbol_tablet &symbol_table,
  const class_hierarchyt &class_hierarchy)
{
  smart_select_pointer_typet select_pointer_type{true};
  ci_lazy_methods_neededt lazy_methods_loader(
    methods_to_convert_later,
    instantiated_classes,
    symbol_table,
    select_pointer_type);

  bool any_new_classes = false;
  for(const exprt &virtual_function_call : virtual_function_calls)
  {
    std::unordered_set<irep_idt> candidate_target_methods;
    get_virtual_method_targets(
      virtual_function_call,
      instantiated_classes,
      candidate_target_methods,
      symbol_table,
      class_hierarchy);

    if(!candidate_target_methods.empty())
      continue;

    const java_method_typet &java_method_type =
      to_java_method_type(virtual_function_call.type());

    // Add the call class to instantiated_classes and assert that it
    // didn't already exist. It can't be instantiated already, otherwise it
    // would give a concrete definition of the called method, and
    // candidate_target_methods would be non-empty.
    const irep_idt &call_class = virtual_function_call.get(ID_C_class);
    bool was_missing = instantiated_classes.count(call_class) == 0;
    CHECK_RETURN(was_missing);
    any_new_classes = true;

    const typet &this_type = java_method_type.get_this()->type();
    if(
      const pointer_typet *this_pointer_type =
        type_try_dynamic_cast<pointer_typet>(this_type))
    {
      lazy_methods_loader.add_all_needed_classes(*this_pointer_type);
    }

    // That should in particular have added call_class to the possibly
    // instantiated types.
    bool still_missing = instantiated_classes.count(call_class) == 0;
    CHECK_RETURN(!still_missing);

    // Make sure we add our return type as required, as we may not have
    // seen any concrete instances of it being created.
    const typet &return_type = java_method_type.return_type();
    if(
      const pointer_typet *return_pointer_type =
        type_try_dynamic_cast<pointer_typet>(return_type))
    {
      lazy_methods_loader.add_all_needed_classes(*return_pointer_type);
    }

    // Check that `get_virtual_method_target` returns a method now
    const irep_idt &call_basename =
      virtual_function_call.get(ID_component_name);
    const auto method_name = get_virtual_method_target(
      instantiated_classes, call_basename, call_class, symbol_table, class_hierarchy);
    CHECK_RETURN(method_name.has_value());

    // Add what it returns to methods_to_convert_later
    methods_to_convert_later.insert(*method_name);
  }
  return any_new_classes;
}

static void initialize_instantiated_classes(
  const std::unordered_set<irep_idt> &entry_points,
  const namespacet &ns,
  ci_lazy_methods_neededt &needed_lazy_methods)
{
  for(const auto &mname : entry_points)
  {
    const auto &symbol=ns.lookup(mname);
    const auto &mtype = to_java_method_type(symbol.type);
    for(const auto &param : mtype.parameters())
    {
      if(param.type().id()==ID_pointer)
      {
        const pointer_typet &original_pointer = to_pointer_type(param.type());
        needed_lazy_methods.add_all_needed_classes(original_pointer);
      }
    }
  }
}

void ci_lazy_methods_v11(
  lazy_goto_modelt &goto_model,
  const std::vector<load_extra_methodst> &lazy_methods_extra_entry_points,
  const class_hierarchyt &class_hierarchy,
  message_handlert &message_handler)
{
  messaget log(message_handler);

  std::unordered_set<irep_idt> methods_to_convert_later =
    { goto_functionst::entry_point() };

  // Add any extra entry points specified; we should elaborate these in the
  // same way as the main function.
  for(const auto &extra_function_generator : lazy_methods_extra_entry_points)
  {
    std::vector<irep_idt> extra_methods =
      extra_function_generator(goto_model.symbol_table);
    insert_all(methods_to_convert_later, extra_methods);
  }

  std::unordered_set<irep_idt> instantiated_classes = {
    "java::java.lang.String",
    "java::java.lang.Class",
    "java::java.lang.Object"
  };

  {
    // This is only necessary while we have extra entry points with parameters
    // that we have to assume somehow their argument types were created by code
    // we can't yet see. For example, if we're told to load f(A, B) then we
    // assume that someone in the future will produce code that somehow creates
    // and instance of A (and any field types A might require), and similarly
    // B. Examples of such magically produced code currently in use include the
    // input factory, which makes its own __CPROVER_start but doesn't do that
    // until long after the load phase is complete.
    // Once there are no such magically occurring functions outside our control
    // (i.e. we can see all object instantiation) then we should delete this
    // code.
    std::unordered_set<irep_idt> initial_callable_methods;
    smart_select_pointer_typet pointer_selector{true};
    ci_lazy_methods_neededt initial_lazy_methods(
      initial_callable_methods,
      instantiated_classes,
      goto_model.symbol_table,
      pointer_selector);
    initialize_instantiated_classes(
      methods_to_convert_later, namespacet(goto_model.symbol_table), initial_lazy_methods);
    methods_to_convert_later.insert(
      initial_callable_methods.begin(), initial_callable_methods.end());
  }

  auto &symbol_table = goto_model.symbol_table;

  std::unordered_set<irep_idt> methods_already_populated;
  std::unordered_set<exprt, irep_hash> virtual_function_calls;

  methods_already_populated.insert(INITIALIZE_FUNCTION);
  bool initialize_converted = false;

  bool any_new_classes = true;
  while(any_new_classes)
  {
    bool any_new_methods = true;
    while(any_new_methods)
    {
      any_new_methods = false;
      while(!methods_to_convert_later.empty())
      {
        std::unordered_set<irep_idt> methods_to_convert;
        std::swap(methods_to_convert, methods_to_convert_later);
        for(const auto &mname : methods_to_convert)
        {
          if(!methods_already_populated.insert(mname).second)
            continue;

          any_new_methods |= convert_and_analyze_method(
            goto_model,
            mname,
            methods_to_convert_later,
            instantiated_classes,
            virtual_function_calls,
            class_hierarchy);
        }
      }

      // Given the object types we now know may be created, populate more
      // possible virtual function call targets:

      log.debug() << "CI lazy methods: add virtual method targets ("
                  << virtual_function_calls.size() << " callsites)" << messaget::eom;

      for(const exprt &function : virtual_function_calls)
      {
        get_virtual_method_targets(
          function,
          instantiated_classes,
          methods_to_convert_later,
          symbol_table,
          class_hierarchy);
      }
    }

    any_new_classes = handle_virtual_methods_with_no_callees(
      methods_to_convert_later,
      instantiated_classes,
      virtual_function_calls,
      symbol_table,
      class_hierarchy);

    if(!initialize_converted && !any_new_classes)
    {
      initialize_converted = true;
      any_new_classes = true;
      convert_and_analyze_method(
            goto_model,
            INITIALIZE_FUNCTION,
            methods_to_convert_later,
            instantiated_classes,
            virtual_function_calls,
            class_hierarchy);
    }
  }

  const std::vector<irep_idt> functions_not_needed =
    make_range(symbol_table.symbols)
      .filter([&](decltype(*symbol_table.symbols.cbegin()) symbol_pair) {
        return !symbol_pair.second.is_static_lifetime &&
               as<code_typet>(symbol_pair.second.type) &&
               methods_already_populated.count(symbol_pair.first) == 0;
      })
      .map(first);
  for(const auto &not_needed : functions_not_needed)
    symbol_table.remove(not_needed);

  log.debug() << "CI lazy methods: removed " << functions_not_needed.size()
              << " unreachable methods" << messaget::eom;

  remove_dead_globals(
    goto_model,
    message_handler,
    {INFLIGHT_EXCEPTION_VARIABLE_NAME, CPROVER_PREFIX "rounding_mode"});
}
