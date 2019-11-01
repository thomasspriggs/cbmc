/*******************************************************************\

Module:

Author: Diffblue Ltd.

\*******************************************************************/

#include "parser.h"
#include "java_signature_lex.yy.h"
#include "java_signature_y.tab.h"

#include <java_bytecode/java_types.h>

#include <vector>

int java_signature_error(const std::string &error)
{
  throw unsupported_java_class_signature_exceptiont{error};
}

struct buffer_state_deletert final
{
  void operator() (YY_BUFFER_STATE buffer) const
  {
    java_signature__delete_buffer(buffer);
  }
};

std::shared_ptr<java_signature_baset> java_signature_parsed;

std::shared_ptr<java_signature_baset>
java_signature_parse(const std::string &input)
{
  // Running the generated parser against a string in memory rather than a file
  // requires a pointer to mutable chars, terminated by two nulls. `std::string`
  // does not expose a mutable interface like this. So `std::vector` is used, so
  // that the lifespan of the raw pointer can be managed automatically.
  std::vector<char> buffer{input.begin(), input.end()};
  buffer.push_back('\0');
  buffer.push_back('\0');
  java_signature_restart(nullptr);
  const std::unique_ptr<
    std::remove_pointer<YY_BUFFER_STATE>::type,
    buffer_state_deletert>
    buffer_state{java_signature__scan_buffer(buffer.data(), buffer.size())};
  if(!buffer_state)
    return {};
  java_signature__switch_to_buffer(buffer_state.get());
  java_signature_parsed = {};
  if(java_signature_parse() != 0)
    return {};
  std::shared_ptr<java_signature_baset> result =
    std::move(java_signature_parsed);
  java_signature_parsed = {};
  return result;
}
