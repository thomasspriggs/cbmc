// Author: Diffblue Ltd.

#include <testing-utils/use_catch.h>

#include <solvers/smt2_incremental/smt_sorts.h>

TEST_CASE("Test upcasts of smt_bool_sortt.", "[core][smt2_incremental]")
{
  const smt_bool_sortt bool_sort;
  smt_sortt sort{bool_sort};
  sort = bool_sort;
}

TEST_CASE("Test upcasts of smt_bit_vec_sortt.", "[core][smt2_incremental]")
{
  const smt_bit_vector_sortt bit_vec_sort{32};
  smt_sortt sort{bit_vec_sort};
  sort = bit_vec_sort;
}

TEST_CASE("Test smt_sortt.pretty is accessible.", "[core][smt2_incremental]")
{
  const smt_sortt bool_sort = smt_bool_sortt{};
  REQUIRE(bool_sort.pretty(0, 0) == "smt_bool_sort");
}

TEST_CASE(
  "Test smt_bit_vec_sortt bit_width getter.",
  "[core][smt2_incremental]")
{
  REQUIRE(smt_bit_vector_sortt{32}.bit_width() == 32);
}

TEST_CASE("Visiting smt_bool_sortt.", "[core][smt2_incremental]")
{
  class : public smt_sort_const_downcast_visitort
  {
  public:
    bool bool_visited = false;
    bool bit_vec_visited = false;

    void visit(const smt_bool_sortt &) override
    {
      bool_visited = true;
    }

    void visit(const smt_bit_vector_sortt &) override
    {
      bit_vec_visited = true;
    }
  } visitor;
  smt_bool_sortt{}.accept(visitor);
  REQUIRE(visitor.bool_visited);
  REQUIRE_FALSE(visitor.bit_vec_visited);
}

TEST_CASE("Visiting smt_bit_vec_sortt.", "[core][smt2_incremental]")
{
  class : public smt_sort_const_downcast_visitort
  {
  public:
    bool bool_visited = false;
    bool bit_vec_visited = false;

    void visit(const smt_bool_sortt &) override
    {
      bool_visited = true;
    }

    void visit(const smt_bit_vector_sortt &bit_vec) override
    {
      bit_vec_visited = true;
      REQUIRE(bit_vec.bit_width() == 8);
    }
  } visitor;
  smt_bit_vector_sortt{8}.accept(visitor);
  REQUIRE_FALSE(visitor.bool_visited);
  REQUIRE(visitor.bit_vec_visited);
}
