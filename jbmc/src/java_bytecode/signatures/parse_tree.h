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
#include <forward_list>

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

template <typename resultt>
std::shared_ptr<resultt>
checked_cast(const std::shared_ptr<java_signature_baset> &input)
{
  auto result = std::dynamic_pointer_cast<resultt>(input);
  INVARIANT(result, "Signature must have the expected type.");
  return result;
}

template <char signature_parameter>
struct java_signature_primitivet final : public java_signature_baset
{
  static_assert(
    signature_parameter == 'B' || signature_parameter == 'C' ||
      signature_parameter == 'D' || signature_parameter == 'F' ||
      signature_parameter == 'I' || signature_parameter == 'J' ||
      signature_parameter == 'S',
    "signature_parameter must be valid java primitive signature");

  static constexpr char signature() {
    return signature_parameter;
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

template <char indicator_parameter>
struct java_signature_wildcard_indicatort final : public java_signature_baset
{
  static_assert(
    indicator_parameter == '+' || indicator_parameter == '-' ||
      indicator_parameter == '*',
    "type_parameter must be valid primitive signature");
  static constexpr char indicator()
  {
    return indicator_parameter;
  }

  ~java_signature_wildcard_indicatort() override = default;
};

struct java_signature_type_argumentt final : public java_signature_baset
{
  std::shared_ptr<java_signature_baset> wildcard;
  std::shared_ptr<java_signature_baset> field_type;

  java_signature_type_argumentt(
    std::shared_ptr<java_signature_baset> wildcard,
    std::shared_ptr<java_signature_baset> field_type)
    : wildcard{std::move(wildcard)}, field_type{std::move(field_type)}
  {
  }

  ~java_signature_type_argumentt() override = default;
};

struct java_signature_type_argumentst final : public java_signature_baset
{
  std::forward_list<java_signature_type_argumentt> collection;

  explicit java_signature_type_argumentst(
    std::forward_list<java_signature_type_argumentt> collection)
    : collection{std::move(collection)}
  {
  }

  ~java_signature_type_argumentst() override = default;
};

struct java_signature_array_typet final : public java_signature_baset
{
  std::shared_ptr<java_signature_baset> element_type;

  explicit java_signature_array_typet(
    std::shared_ptr<java_signature_baset> element_type)
    : element_type{std::move(element_type)}
  {
  }

  ~java_signature_array_typet() override = default;
};

#endif // CPROVER_JAVA_BYTECODE_SIGNATURES_PARSE_TREE_H
