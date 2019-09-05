/*******************************************************************\

Module: Remove Dead Global Variables

Author: Diffblue Ltd.

\*******************************************************************/

/// \file
/// Remove Dead Global Variables

#include "remove_dead_globals.h"

#include <linking/static_lifetime_init.h>
#include <test-gen-util/algorithm/insert_all.h>
#include <test-gen-util/as.h>
#include <util/expr_iterator.h>
#include <util/graph.h>

/// Run a function for every global symbol reference in the given expression
/// \param expr: expression we'll search for global symbol references
/// \param instruction: instruction that expr occurs within
/// \param symbol_table: Symbol table
/// \param func: Will be executed for every symbol reference in expr that
///   has the `is_static_lifetime` flag set.
static void for_each_global_symbol(
  const exprt &expr,
  const goto_programt::instructiont &instruction,
  const symbol_tablet &symbol_table,
  std::function<
    void(const goto_programt::instructiont &, const irep_idt &)> func)
{
  for(auto it = expr.depth_begin(), itend = expr.depth_end(); it != itend; ++it)
  {
    if(it->id() == ID_symbol)
    {
      const irep_idt &id = to_symbol_expr(*it).get_identifier();
      if(symbol_table.lookup_ref(id).is_static_lifetime)
        func(instruction, id);
    }
  }
}

/// Run a function for every global symbol reference in the given program
/// \param program: GOTO program whose instructions we'll iterate over
/// \param symbol_table: Symbol table corresponding to program
/// \param func: Will be executed for every symbol reference in program
///   that has the `is_static_lifetime` flag set.
static void for_each_global_symbol(
  const goto_programt &program,
  const symbol_tablet &symbol_table,
  std::function<
    void(const goto_programt::instructiont &, const irep_idt &)> func)
{
  for(const auto &instruction : program.instructions)
  {
    for_each_global_symbol(instruction.code, instruction, symbol_table, func);
    for_each_global_symbol(instruction.guard, instruction, symbol_table, func);
  }
}

/// Node in the global variable dependency graph -- just a symbol name.
struct global_dependency_graph_nodet : graph_nodet<empty_edget>
{
  irep_idt id;

  global_dependency_graph_nodet() : id()
  {
  }
};

/// Global variable dependency graph. An edge from x -> y means that if global
/// x is used, then so is y. For example, x might be initialised to carry
/// pointers to y, or perhaps just copies its initialiser value out of y.
class global_dependency_grapht : grapht<global_dependency_graph_nodet>
{
  /// Map from global names to node indices
  std::unordered_map<irep_idt, node_indext> nodes_by_name;

public:
  /// \param id: Global ID to get or create a node for
  /// \return Corresponding node index
  node_indext get_or_create_node(const irep_idt &id)
  {
    auto insert_result = nodes_by_name.insert({id, 0});
    if(insert_result.second)
    {
      auto new_node = add_node();
      (*this)[new_node].id = id;
      insert_result.first->second = new_node;
    }
    return insert_result.first->second;
  }

  /// Adds a graph edge indicating `from` depends on `to`.
  void add_dependency(const irep_idt &from, const irep_idt &to)
  {
    add_edge(get_or_create_node(from), get_or_create_node(to));
  }

  /// Gets the globals that are depended on by a set of root globals.
  /// In graph terms this is just the set of transitively reachable nodes
  /// starting from the given root nodes.
  /// \param roots: globals known to be needed
  /// \return set of globals that must also be retained if roots are
  ///   all retained.
  std::unordered_set<irep_idt> get_dependents(
    const std::unordered_set<irep_idt> &roots) const
  {
    std::vector<node_indext> root_indices;
    root_indices.reserve(roots.size());
    for(const auto &id : roots)
      root_indices.push_back(nodes_by_name.at(id));

    std::vector<node_indext> dependent_indices =
      get_reachable(root_indices, true);

    std::unordered_set<irep_idt> result;
    for(const auto index : dependent_indices)
      result.insert((*this)[index].id);

    return result;
  }
};

/// Get the global variable initialized by an instruction, if any. This assumes
/// that function calls are side-effect free initializers that target their
/// first argument, which are currently the only kinds of calls that can occur
/// in `__CPROVER_initialize`.
/// \param code: instruction that may be a global initializer
/// \param symbol_table: global symbol table
/// \return identifier of the initialized global if any, otherwise an empty
///   optionalt.
static optionalt<irep_idt> get_initialized_global(
  const codet &code, const symbol_tablet &symbol_table)
{
  if(code.is_nil())
    return {};

  optionalt<irep_idt> initialized_id;

  if(code.get_statement() == ID_assign)
  {
    exprt lhs = to_code_assign(code).lhs();
    while(lhs.id() == ID_member)
      lhs = lhs.op0();
    if(lhs.id() == ID_symbol)
      initialized_id = to_symbol_expr(lhs).get_identifier();
  }
  else if(const auto call = as<code_function_callt>(code))
  {
    if(const auto function = as<symbol_exprt>(call->function()))
    {
      // Assume init function initializes its first argument.
      // Any function call within __CPROVER_initialize is an initializer which
      // sets up its first (`this`) argument and does not otherwise have
      // side-effects. The only such things at the moment are
      // java.lang.Class.cproverInitializeClassLiteral calls generated by the
      // Java frontend.
      INVARIANT(
        to_code_type(function->type()).has_this(),
        "initialize function must have a 'this' argument");
      const auto &init_symbol =
        to_symbol_expr(to_address_of_expr(call->arguments()[0]).op());
      initialized_id = init_symbol.get_identifier();
    }
  }
  else if(code.get_statement() == ID_expression)
  {
    // String literal initialisation generates `code_expressiont` statements
    // that simply name a symbol to ensure it does not appear to be dead.
    if(code.op0().id() == ID_symbol)
      initialized_id = to_symbol_expr(code.op0()).get_identifier();
  }

  INVARIANT(
    initialized_id, "Which variable is being initialized must be determined.");

  return symbol_table.lookup_ref(*initialized_id).is_static_lifetime
           ? initialized_id
           : optionalt<irep_idt>{};
}

/// Remove dead global variables, which we take to be variables only referenced
/// within __CPROVER_initialize or entirely unreferenced. Their initialization
/// statements in __CPROVER_initialize are erased (made into skip instructions);
/// their symbols are removed from the symbol table.
void remove_dead_globals(
  lazy_goto_modelt &goto_model, message_handlert &message_handler,
  const std::vector<irep_idt> &force_keep_globals)
{
  // Gather globals referenced from non-__CPROVER_initialize functions:

  global_dependency_grapht dependency_graph;
  std::unordered_set<irep_idt> referenced_global_symbols;

  auto add_to_referenced_globals =
    [&referenced_global_symbols, &dependency_graph]
    (const goto_programt::instructiont &, const irep_idt &id)
    {
      referenced_global_symbols.insert(id);
      dependency_graph.get_or_create_node(id);
    };

  for(const auto &keep_global : force_keep_globals)
    add_to_referenced_globals({}, keep_global);

  for(const auto &id_function : goto_model.get_goto_functions().function_map)
  {
    if(id_function.first == INITIALIZE_FUNCTION)
      continue;
    for_each_global_symbol(
      id_function.second.body,
      goto_model.symbol_table,
      add_to_referenced_globals);
  }

  // Gather inter-global dependencies (for example, x = 1; y = &x; implies that
  // if y is alive, so is x)

  // TODO: decide how to mediate accessing / mutating a GOTO function with a
  // lazy goto model. Probably enough to just allow a non-const get_goto_function
  // on abstract_goto_modelt.
  auto &initialize_function =
    goto_model.get_goto_function_writable(INITIALIZE_FUNCTION);

  {
    auto add_to_global_dependencies =
      [&dependency_graph, &goto_model]
      (const goto_programt::instructiont &instruction, const irep_idt &id)
      {
        optionalt<irep_idt> lhs_global =
          get_initialized_global(instruction.code, goto_model.symbol_table);
        if(lhs_global)
          dependency_graph.add_dependency(*lhs_global, id);
      };

    for_each_global_symbol(
      initialize_function.body,
      goto_model.symbol_table,
      add_to_global_dependencies);

    // Hack: string initializers use a comma expression to insist that if
    // Some_String is needed, then so is Some_String_return_value. However
    // the comma expression has already been decomposed into two different
    // GOTO instructions by now, so this relationship is not picked up by the
    // dependency walk above.

    const struct_tag_typet string_type("java::java.lang.String");
    for(const auto &id_symbol : goto_model.symbol_table.symbols)
    {
      const auto &symbol = id_symbol.second;
      if(symbol.is_static_lifetime && symbol.type == string_type)
      {
        irep_idt return_value_id = id2string(symbol.name) + "_return_value";
        if(goto_model.symbol_table.has_symbol(return_value_id))
          dependency_graph.add_dependency(symbol.name, return_value_id);
      }
    }

    // Propagate dependencies:
    const std::unordered_set<irep_idt> transitive_references =
      dependency_graph.get_dependents(referenced_global_symbols);
    insert_all(referenced_global_symbols, transitive_references);
  }

  messaget log(message_handler);
  std::size_t instructions_removed = 0;

  std::unordered_set<irep_idt> remove_symbols;

  for(auto &instruction : initialize_function.body.instructions)
  {
    const optionalt<irep_idt> global =
      get_initialized_global(instruction.code, goto_model.symbol_table);
    if(global && referenced_global_symbols.count(*global) == 0)
    {
      ++instructions_removed;
      instruction.make_skip();
      remove_symbols.insert(*global);
    }
  }

  std::size_t old_symbol_table_size = goto_model.symbol_table.symbols.size();
  for(const auto &id : remove_symbols)
  {
    bool remove_failed = goto_model.symbol_table.remove(id);
    INVARIANT(
      !remove_failed,
      "tried to remove symbol " + id2string(id) +
      " but it was not in the symbol table");
  }

  log.status() << "Remove dead globals erased " << instructions_removed <<
    " / " << initialize_function.body.instructions.size() << " instructions" <<
    " and " << remove_symbols.size() << " / " << old_symbol_table_size <<
    " symbols" << messaget::eom;
}
