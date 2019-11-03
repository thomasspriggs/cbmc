/*******************************************************************\

Module:

Author: Diffblue Ltd.

\*******************************************************************/

#ifndef CPROVER_JAVA_BYTECODE_SIGNATURES_PARSE_TREE_H
#define CPROVER_JAVA_BYTECODE_SIGNATURES_PARSE_TREE_H

#include <memory>
#include <string>
#include <util/invariant.h>
#include <utility>

struct java_signature_baset
{
  virtual ~java_signature_baset() = default;
};

template <typename resultt>
resultt &checked_cast(java_signature_baset &input)
{
  auto result = dynamic_cast<resultt *>(&input);
  INVARIANT(result, "Signature must have the expected type.");
  return *result;
}

template <char type_parameter>
struct java_signature_primitivet final : public java_signature_baset
{
  static constexpr char signature() {
    return type_parameter;
  };

  ~java_signature_primitivet() override = default;
};

struct java_signature_identifiert : public java_signature_baset
{
  std::string identifier;

  explicit java_signature_identifiert(std::string identifier)
    : identifier{std::move(identifier)}
  {
  }

  explicit java_signature_identifiert(
    const std::shared_ptr<java_signature_baset> &identifier)
    : java_signature_identifiert{
        checked_cast<java_signature_identifiert>(*identifier)}
  {
  }

  ~java_signature_identifiert() override = default;
};

struct java_signature_class_typet final : public java_signature_identifiert
{
  using java_signature_identifiert::java_signature_identifiert;

  ~java_signature_class_typet() override = default;
};

struct java_signature_type_variablet final : public java_signature_identifiert
{
  using java_signature_identifiert::java_signature_identifiert;

  ~java_signature_type_variablet() override = default;
};

#endif // CPROVER_JAVA_BYTECODE_SIGNATURES_PARSE_TREE_H
