#include "gtest/gtest.h"

#include "smt.h"
#include "smt_msat.h"

#include <cstdint>

using namespace smt;

TEST(SmtMsatTest, PositiveBvLiteral)
{
  MsatSolver s;

  constexpr size_t long_bv_size = sizeof(long) * 8;
  const Bv<long> e0 = literal<Bv<long>>(42);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e0).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();

  EXPECT_TRUE(msat_term_is_number(env, t0));

  size_t bv_size;
  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bv_type(env, type, &bv_size));
  EXPECT_EQ(long_bv_size, bv_size);

  msat_term vc, t1, t2;

  EXPECT_EQ(0, msat_push_backtrack_point(env));
  {
    t1 = msat_make_bv_number(env, "42", long_bv_size, 10);
    vc = msat_make_equal(env, t0, t1);

    EXPECT_FALSE(MSAT_ERROR_TERM(t1));
    EXPECT_FALSE(MSAT_ERROR_TERM(vc));

    EXPECT_EQ(0, msat_assert_formula(env, vc));
    EXPECT_EQ(MSAT_SAT, msat_solve(env));
  }
  EXPECT_EQ(0, msat_pop_backtrack_point(env));

  EXPECT_EQ(0, msat_push_backtrack_point(env));
  {
    vc = msat_make_not(env, vc);
    EXPECT_FALSE(MSAT_ERROR_TERM(vc));

    EXPECT_EQ(0, msat_assert_formula(env, vc));
    EXPECT_EQ(MSAT_UNSAT, msat_solve(env));
  }
  EXPECT_EQ(0, msat_pop_backtrack_point(env));

  EXPECT_EQ(0, msat_push_backtrack_point(env));
  {
    t2 = msat_make_bv_number(env, "43", long_bv_size, 10);
    vc = msat_make_equal(env, t0, t2);
    EXPECT_FALSE(MSAT_ERROR_TERM(t2));
    EXPECT_FALSE(MSAT_ERROR_TERM(vc));

    EXPECT_EQ(0, msat_assert_formula(env, vc));
    EXPECT_EQ(MSAT_UNSAT, msat_solve(env));
  }
  EXPECT_EQ(0, msat_pop_backtrack_point(env));
}

TEST(SmtMsatTest, NegativeBvLiteral)
{
  MsatSolver s;

  constexpr size_t long_bv_size = sizeof(long) * 8;
  const Bv<long> e0 = literal<Bv<long>>(-42);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e0).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_FALSE(MSAT_ERROR_TERM(t0));

  EXPECT_TRUE(msat_term_is_number(env, t0));

  size_t bv_size;
  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bv_type(env, type, &bv_size));
  EXPECT_EQ(long_bv_size, bv_size);

  msat_term vc, t1, t2;

  EXPECT_EQ(0, msat_push_backtrack_point(env));
  {
    t1 = msat_make_bv_number(env, "42", long_bv_size, 10);
    vc = msat_make_equal(env, t0, t1);

    EXPECT_FALSE(MSAT_ERROR_TERM(t1));
    EXPECT_FALSE(MSAT_ERROR_TERM(vc));

    EXPECT_EQ(0, msat_assert_formula(env, vc));
    EXPECT_EQ(MSAT_UNSAT, msat_solve(env));
  }
  EXPECT_EQ(0, msat_pop_backtrack_point(env));

  EXPECT_EQ(0, msat_push_backtrack_point(env));
  {
    vc = msat_make_not(env, vc);
    EXPECT_FALSE(MSAT_ERROR_TERM(vc));

    EXPECT_EQ(0, msat_assert_formula(env, vc));
    EXPECT_EQ(MSAT_SAT, msat_solve(env));
  }
  EXPECT_EQ(0, msat_pop_backtrack_point(env));

  EXPECT_EQ(0, msat_push_backtrack_point(env));
  {
    t2 = msat_make_bv_neg(env, t1);
    vc = msat_make_equal(env, t0, t2);
    EXPECT_FALSE(MSAT_ERROR_TERM(t2));
    EXPECT_FALSE(MSAT_ERROR_TERM(vc));

    EXPECT_EQ(0, msat_assert_formula(env, vc));
    EXPECT_EQ(MSAT_SAT, msat_solve(env));
  }
  EXPECT_EQ(0, msat_pop_backtrack_point(env));

  EXPECT_EQ(0, msat_push_backtrack_point(env));
  {
    vc = msat_make_not(env, vc);
    EXPECT_FALSE(MSAT_ERROR_TERM(vc));

    EXPECT_EQ(0, msat_assert_formula(env, vc));
    EXPECT_EQ(MSAT_UNSAT, msat_solve(env));
  }
  EXPECT_EQ(0, msat_pop_backtrack_point(env));

  EXPECT_EQ(0, msat_push_backtrack_point(env));
  {
    const msat_term eq_term = msat_make_bv_plus(env, t0, t1);
    const msat_term zero_term = msat_make_bv_number(env, "0", long_bv_size, 10);
    vc = msat_make_equal(env, eq_term, zero_term);
    EXPECT_FALSE(MSAT_ERROR_TERM(vc));

    EXPECT_EQ(0, msat_assert_formula(env, vc));
    EXPECT_EQ(MSAT_SAT, msat_solve(env));
  }
  EXPECT_EQ(0, msat_pop_backtrack_point(env));

  EXPECT_EQ(0, msat_push_backtrack_point(env));
  {
    vc = msat_make_not(env, vc);
    EXPECT_EQ(0, msat_assert_formula(env, vc));
    EXPECT_EQ(MSAT_UNSAT, msat_solve(env));
  }
  EXPECT_EQ(0, msat_pop_backtrack_point(env));
}

TEST(SmtMsatTest, PositiveIntLiteral)
{
  MsatSolver s;

  const Int e0 = literal<Int>(42L);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e0).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_FALSE(MSAT_ERROR_TERM(t0));

  EXPECT_TRUE(msat_term_is_number(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_integer_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("42", std::string(str));
}

TEST(SmtMsatTest, PositiveRealLiteral)
{
  MsatSolver s;

  const Real e0 = literal<Real>(42L);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e0).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_FALSE(MSAT_ERROR_TERM(t0));

  EXPECT_TRUE(msat_term_is_number(env, t0));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("42", std::string(str));
}

TEST(SmtMsatTest, NegativeIntLiteral)
{
  MsatSolver s;

  const Int e0 = literal<Int>(-42L);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e0).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_FALSE(MSAT_ERROR_TERM(t0));

  EXPECT_TRUE(msat_term_is_number(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_integer_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("-42", std::string(str));
}

TEST(SmtMsatTest, NegativeRealLiteral)
{
  MsatSolver s;

  const Real e0 = literal<Real>(-42L);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e0).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_FALSE(MSAT_ERROR_TERM(t0));

  EXPECT_TRUE(msat_term_is_number(env, t0));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("-42", std::string(str));
}

TEST(SmtMsatTest, BoolFalseLiteral)
{
  MsatSolver s;

  const Bool e0 = literal<Bool>(false);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e0).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_FALSE(MSAT_ERROR_TERM(t0));

  EXPECT_TRUE(msat_term_is_false(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("`false`", std::string(str));
}

TEST(SmtMsatTest, BoolTrueLiteral)
{
  MsatSolver s;

  const Bool e0 = literal<Bool>(true);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e0).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_FALSE(MSAT_ERROR_TERM(t0));

  EXPECT_TRUE(msat_term_is_true(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("`true`", std::string(str));
}

TEST(SmtMsatTest, BvDeclExpr)
{
  MsatSolver s;

  const Bv<long> e0 = any<Bv<long>>("x");

  EXPECT_EQ(OK, static_cast<SharedExpr>(e0).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_constant(env, t0));

  const msat_type type = msat_term_get_type(t0);
  size_t bv_size;
  EXPECT_TRUE(msat_is_bv_type(env, type, &bv_size));
  EXPECT_EQ(sizeof(long) * 8, bv_size);

  EXPECT_EQ(sat, s.check());
  s.push();
  {
    s.add(42 == e0);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(42 != e0);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  // s.check() may return sat if there are no assertions
  s.add(literal<Bool>(true));

  msat_type bv_long_type;
  msat_decl x_decl;
  msat_term x_term, eq_term, neq_term;
  s.push();
  {
    bv_long_type = msat_get_bv_type(env, sizeof(long) * 8);
    EXPECT_FALSE(MSAT_ERROR_TYPE(bv_long_type));
    x_decl = msat_declare_function(env, "x", bv_long_type);
    EXPECT_FALSE(MSAT_ERROR_DECL(x_decl));
    x_term = msat_make_constant(env, x_decl);
    EXPECT_FALSE(MSAT_ERROR_TERM(x_term));
    eq_term = msat_make_equal(env, x_term, t0);
    EXPECT_FALSE(MSAT_ERROR_TERM(eq_term));
    EXPECT_EQ(0, msat_assert_formula(env, eq_term));
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    neq_term = msat_make_not(env, eq_term);
    EXPECT_FALSE(MSAT_ERROR_TERM(neq_term));
    EXPECT_EQ(0, msat_assert_formula(env, neq_term));
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    bv_long_type = msat_get_bv_type(env, sizeof(long) * 8);
    EXPECT_FALSE(MSAT_ERROR_TYPE(bv_long_type));
    x_decl = msat_declare_function(env, "x", bv_long_type);
    EXPECT_FALSE(MSAT_ERROR_DECL(x_decl));
    x_term = msat_make_constant(env, x_decl);
    EXPECT_FALSE(MSAT_ERROR_TERM(x_term));
    eq_term = msat_make_equal(env, x_term, t0);
    EXPECT_FALSE(MSAT_ERROR_TERM(eq_term));
    neq_term = msat_make_not(env, eq_term);
    EXPECT_FALSE(MSAT_ERROR_TERM(neq_term));
    EXPECT_EQ(0, msat_assert_formula(env, neq_term));
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();
}

TEST(SmtMsatTest, IntDeclExpr)
{
  MsatSolver s;

  const Int e0 = any<Int>("x");

  EXPECT_EQ(OK, static_cast<SharedExpr>(e0).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_constant(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_integer_type(env, type));

  EXPECT_EQ(sat, s.check());
  s.push();
  {
    s.add(42 == e0);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(42 != e0);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  // s.check() may return sat if there are no assertions
  s.add(literal<Bool>(true));

  msat_type int_type;
  msat_decl x_decl;
  msat_term x_term, eq_term, neq_term;
  s.push();
  {
    int_type = msat_get_integer_type(env);
    EXPECT_FALSE(MSAT_ERROR_TYPE(int_type));
    x_decl = msat_declare_function(env, "x", int_type);
    EXPECT_FALSE(MSAT_ERROR_DECL(x_decl));
    x_term = msat_make_constant(env, x_decl);
    EXPECT_FALSE(MSAT_ERROR_TERM(x_term));
    eq_term = msat_make_equal(env, x_term, t0);
    EXPECT_FALSE(MSAT_ERROR_TERM(eq_term));
    EXPECT_EQ(0, msat_assert_formula(env, eq_term));
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    neq_term = msat_make_not(env, eq_term);
    EXPECT_FALSE(MSAT_ERROR_TERM(neq_term));
    EXPECT_EQ(0, msat_assert_formula(env, neq_term));
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    int_type = msat_get_integer_type(env);
    EXPECT_FALSE(MSAT_ERROR_TYPE(int_type));
    x_decl = msat_declare_function(env, "x", int_type);
    EXPECT_FALSE(MSAT_ERROR_DECL(x_decl));
    x_term = msat_make_constant(env, x_decl);
    EXPECT_FALSE(MSAT_ERROR_TERM(x_term));
    eq_term = msat_make_equal(env, x_term, t0);
    EXPECT_FALSE(MSAT_ERROR_TERM(eq_term));
    neq_term = msat_make_not(env, eq_term);
    EXPECT_FALSE(MSAT_ERROR_TERM(neq_term));
    EXPECT_EQ(0, msat_assert_formula(env, neq_term));
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();
}

TEST(SmtMsatTest, RealDeclExpr)
{
  MsatSolver s;

  const Real e0 = any<Real>("x");

  EXPECT_EQ(OK, static_cast<SharedExpr>(e0).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_constant(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_rational_type(env, type));

  EXPECT_EQ(sat, s.check());
  s.push();
  {
    s.add(42 == e0);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(42 != e0);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  // s.check() may return sat if there are no assertions
  s.add(literal<Bool>(true));

  msat_type rational_type;
  msat_decl x_decl;
  msat_term x_term, eq_term, neq_term;
  s.push();
  {
    rational_type = msat_get_rational_type(env);
    EXPECT_FALSE(MSAT_ERROR_TYPE(rational_type));
    x_decl = msat_declare_function(env, "x", rational_type);
    EXPECT_FALSE(MSAT_ERROR_DECL(x_decl));
    x_term = msat_make_constant(env, x_decl);
    EXPECT_FALSE(MSAT_ERROR_TERM(x_term));
    eq_term = msat_make_equal(env, x_term, t0);
    EXPECT_FALSE(MSAT_ERROR_TERM(eq_term));
    EXPECT_EQ(0, msat_assert_formula(env, eq_term));
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    neq_term = msat_make_not(env, eq_term);
    EXPECT_FALSE(MSAT_ERROR_TERM(neq_term));
    EXPECT_EQ(0, msat_assert_formula(env, neq_term));
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    rational_type = msat_get_rational_type(env);
    EXPECT_FALSE(MSAT_ERROR_TYPE(rational_type));
    x_decl = msat_declare_function(env, "x", rational_type);
    EXPECT_FALSE(MSAT_ERROR_DECL(x_decl));
    x_term = msat_make_constant(env, x_decl);
    EXPECT_FALSE(MSAT_ERROR_TERM(x_term));
    eq_term = msat_make_equal(env, x_term, t0);
    EXPECT_FALSE(MSAT_ERROR_TERM(eq_term));
    neq_term = msat_make_not(env, eq_term);
    EXPECT_FALSE(MSAT_ERROR_TERM(neq_term));
    EXPECT_EQ(0, msat_assert_formula(env, neq_term));
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();
}

// Note that we have seen (Array Int Bool) to cause MathSAT5 to crash
TEST(SmtMsatTest, ArrayDecl)
{
  MsatSolver s;

  const Array<Bv<size_t>, Bv<int>> e0 = any<Array<Bv<size_t>, Bv<int>>>("array");

  EXPECT_EQ(OK, static_cast<SharedExpr>(e0).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();

  const msat_type type = msat_term_get_type(t0);
  msat_type domain_type, range_type;
  EXPECT_TRUE(msat_is_array_type(env, type, &domain_type, &range_type));

  size_t domain_bv_size;
  EXPECT_TRUE(msat_is_bv_type(env, domain_type, &domain_bv_size));
  EXPECT_EQ(sizeof(size_t) * 8, domain_bv_size);

  size_t range_bv_size;
  EXPECT_TRUE(msat_is_bv_type(env, range_type, &range_bv_size));
  EXPECT_EQ(sizeof(int) * 8, range_bv_size);
}

TEST(SmtMsatTest, UnaryFuncAppExpr)
{
  MsatSolver s;

  Decl<Func<Int, Bool>> func_decl("f");
  const Int e0 = any<Int>("x");
  const Bool e1 = apply(func_decl, e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_uf(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(f x)", std::string(str));
}

TEST(SmtMsatTest, BinaryFuncAppExpr)
{
  MsatSolver s;

  Decl<Func<Int, Bv<long>, Bool>> func_decl("f");
  const Int e0 = any<Int>("x");
  const Bv<long> e1 = any<Bv<long>>("y");
  const Bool e2 = apply(func_decl, e0, e1);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e2).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_uf(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(f x y)", std::string(str));
}

TEST(SmtMsatTest, ArraySelectExpr)
{
  MsatSolver s;

  const Array<Bv<uint64_t>, Int> e0 = any<Array<Bv<uint64_t>, Int>>("array");
  const Bv<uint64_t> e1 = any<Bv<uint64_t>>("x");
  const Int e2 = select(e0, e1);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e2).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_array_read(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_integer_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`read_<BitVec, 64, >_int` array x)", std::string(str));
}

TEST(SmtMsatTest, ArrayStoreExpr)
{
  MsatSolver s;

  const Array<Bv<uint64_t>, Int> e0 = any<Array<Bv<uint64_t>, Int>>("array");
  const Bv<uint64_t> e1 = any<Bv<uint64_t>>("x");
  const Int e2 = any<Int>("y");
  const Array<Bv<uint64_t>, Int> e3 = store(e0, e1, e2);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e3).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();

  const msat_type type = msat_term_get_type(t0);
  msat_type domain_type, range_type;
  EXPECT_TRUE(msat_is_array_type(env, type, &domain_type, &range_type));

  size_t domain_bv_size;
  EXPECT_TRUE(msat_is_bv_type(env, domain_type, &domain_bv_size));
  EXPECT_EQ(64, domain_bv_size);

  EXPECT_TRUE(msat_is_integer_type(env, range_type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`write_<BitVec, 64, >_int` array x y)", std::string(str));
}

TEST(SmtMsatTest, BvSignedOperatorNOT)
{
  MsatSolver s;

   constexpr size_t long_bv_size = sizeof(int64_t) * 8;
  const Bv<int64_t> e0 = any<Bv<int64_t>>("x");
  const Bv<int64_t> e1(~e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_bv_not(env, t0));

  const msat_type type = msat_term_get_type(t0);
  size_t bv_size;
  EXPECT_TRUE(msat_is_bv_type(env, type, &bv_size));
  EXPECT_EQ(long_bv_size, bv_size);

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`bvnot_64` x)", std::string(str));
}

TEST(SmtMsatTest, BvUnsignedOperatorNOT)
{
  MsatSolver s;

   constexpr size_t long_bv_size = sizeof(uint64_t) * 8;
  const Bv<uint64_t> e0 = any<Bv<uint64_t>>("x");
  const Bv<uint64_t> e1(~e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_bv_not(env, t0));

  const msat_type type = msat_term_get_type(t0);
  size_t bv_size;
  EXPECT_TRUE(msat_is_bv_type(env, type, &bv_size));
  EXPECT_EQ(long_bv_size, bv_size);

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`bvnot_64` x)", std::string(str));
}

#define SMT_MSAT_TEST_BV_BINARY_OP(op, opcode, opname, sign, literal)   \
  {                                                                     \
    MsatSolver s;                                                       \
                                                                        \
    constexpr size_t long_bv_size = sizeof(sign long) * 8;              \
    const Bv<sign long> e0 = any<Bv<sign long>>("x");                   \
    const Bv<sign long> e1(literal op e0);                              \
                                                                        \
    EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));               \
                                                                        \
    const msat_env env = s.env();                                       \
    const msat_term t0 = s.term();                                      \
    EXPECT_TRUE(msat_term_is_bv_##opname(env, t0));                     \
                                                                        \
    size_t bv_size;                                                     \
    const msat_type type = msat_term_get_type(t0);                      \
    EXPECT_TRUE(msat_is_bv_type(env, type, &bv_size));                  \
    EXPECT_EQ(long_bv_size, bv_size);                                   \
  }                                                                     \

#define SMT_MSAT_TEST_BV_SIGNED_BINARY_OP(op, opcode, opname)           \
  TEST(SmtMsatTest, BvSignedBinaryOperator##opcode)                     \
  SMT_MSAT_TEST_BV_BINARY_OP(op, opcode, opname, signed, 42L)           \

#define SMT_MSAT_TEST_BV_UNSIGNED_BINARY_OP(op, opcode, opname)         \
  TEST(SmtMsatTest, BvUnsignedBinaryOperator##opcode)                   \
  SMT_MSAT_TEST_BV_BINARY_OP(op, opcode, opname, unsigned, 42UL)        \

TEST(SmtMsatTest, BvSignedOperatorSUB)
{
  MsatSolver s;

  constexpr size_t long_bv_size = sizeof(int64_t) * 8;
  const Bv<int64_t> e0 = any<Bv<int64_t>>("x");
  const Bv<int64_t> e1(42L - e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();

  size_t bv_size;
  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bv_type(env, type, &bv_size));
  EXPECT_EQ(long_bv_size, bv_size);

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`bvadd_64` 42_64 (`bvneg_64` x))", std::string(str));
}

SMT_MSAT_TEST_BV_SIGNED_BINARY_OP(&, AND, and)
SMT_MSAT_TEST_BV_SIGNED_BINARY_OP(|, OR, or)
SMT_MSAT_TEST_BV_SIGNED_BINARY_OP(^, XOR, xor)
SMT_MSAT_TEST_BV_SIGNED_BINARY_OP(+, ADD, plus)
SMT_MSAT_TEST_BV_SIGNED_BINARY_OP(*, MUL, times)
SMT_MSAT_TEST_BV_SIGNED_BINARY_OP(/, QUO, sdiv)
SMT_MSAT_TEST_BV_SIGNED_BINARY_OP(%, REM, srem)

TEST(SmtMsatTest, BvUnsignedOperatorSUB)
{
  MsatSolver s;

  constexpr size_t long_bv_size = sizeof(uint64_t) * 8;
  const Bv<uint64_t> e0 = any<Bv<uint64_t>>("x");
  const Bv<uint64_t> e1(42L - e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();

  size_t bv_size;
  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bv_type(env, type, &bv_size));
  EXPECT_EQ(long_bv_size, bv_size);

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`bvadd_64` 42_64 (`bvneg_64` x))", std::string(str));
}

SMT_MSAT_TEST_BV_UNSIGNED_BINARY_OP(&, AND, and)
SMT_MSAT_TEST_BV_UNSIGNED_BINARY_OP(|, OR, or)
SMT_MSAT_TEST_BV_UNSIGNED_BINARY_OP(^, XOR, xor)
SMT_MSAT_TEST_BV_UNSIGNED_BINARY_OP(+, ADD, plus)
SMT_MSAT_TEST_BV_UNSIGNED_BINARY_OP(*, MUL, times)
SMT_MSAT_TEST_BV_UNSIGNED_BINARY_OP(/, QUO, udiv)
SMT_MSAT_TEST_BV_UNSIGNED_BINARY_OP(%, REM, urem)

TEST(SmtMsatTest, BvSignedBinaryOperatorEQL)
{
  MsatSolver s;

  const Bv<int64_t> e0 = any<Bv<int64_t>>("x");
  const Bool e1(42 == e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_equal(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`=_<BitVec, 64, >` x 42_64)", std::string(str));
}

TEST(SmtMsatTest, BvSignedBinaryOperatorLSS)
{
  MsatSolver s;

  const Bv<int64_t> e0 = any<Bv<int64_t>>("x");
  const Bool e1(42 < e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_bv_slt(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`bvslt_64` 42_64 x)", std::string(str));
}

TEST(SmtMsatTest, BvSignedBinaryOperatorGTR)
{
  MsatSolver s;

  const Bv<int64_t> e0 = any<Bv<int64_t>>("x");
  const Bool e1(42 > e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_bv_slt(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`bvslt_64` x 42_64)", std::string(str));
}

TEST(SmtMsatTest, BvSignedBinaryOperatorNEQ)
{
  MsatSolver s;

  const Bv<int64_t> e0 = any<Bv<int64_t>>("x");
  const Bool e1(42 != e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_not(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`not` (`=_<BitVec, 64, >` x 42_64))", std::string(str));
}

TEST(SmtMsatTest, BvSignedBinaryOperatorLEQ)
{
  MsatSolver s;

  const Bv<int64_t> e0 = any<Bv<int64_t>>("x");
  const Bool e1(42 <= e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_not(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`not` (`bvslt_64` x 42_64))", std::string(str));
}

TEST(SmtMsatTest, BvSignedBinaryOperatorGEQ)
{
  MsatSolver s;

  const Bv<int64_t> e0 = any<Bv<int64_t>>("x");
  const Bool e1(42 >= e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_not(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`not` (`bvslt_64` 42_64 x))", std::string(str));
}

TEST(SmtMsatTest, BvUnsignedBinaryOperatorEQL)
{
  MsatSolver s;

  const Bv<uint64_t> e0 = any<Bv<uint64_t>>("x");
  const Bool e1(42 == e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_equal(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`=_<BitVec, 64, >` x 42_64)", std::string(str));
}

TEST(SmtMsatTest, BvUnsignedBinaryOperatorLSS)
{
  MsatSolver s;

  const Bv<uint64_t> e0 = any<Bv<uint64_t>>("x");
  const Bool e1(42 < e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_bv_ult(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`bvult_64` 42_64 x)", std::string(str));
}

TEST(SmtMsatTest, BvUnsignedBinaryOperatorGTR)
{
  MsatSolver s;

  const Bv<uint64_t> e0 = any<Bv<uint64_t>>("x");
  const Bool e1(42 > e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_bv_ult(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`bvult_64` x 42_64)", std::string(str));
}

TEST(SmtMsatTest, BvUnsignedBinaryOperatorNEQ)
{
  MsatSolver s;

  const Bv<uint64_t> e0 = any<Bv<uint64_t>>("x");
  const Bool e1(42 != e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_not(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`not` (`=_<BitVec, 64, >` x 42_64))", std::string(str));
}

TEST(SmtMsatTest, BvUnsignedBinaryOperatorLEQ)
{
  MsatSolver s;

  const Bv<uint64_t> e0 = any<Bv<uint64_t>>("x");
  const Bool e1(42 <= e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_not(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`not` (`bvult_64` x 42_64))", std::string(str));
}

TEST(SmtMsatTest, BvUnsignedBinaryOperatorGEQ)
{
  MsatSolver s;

  const Bv<uint64_t> e0 = any<Bv<uint64_t>>("x");
  const Bool e1(42 >= e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_not(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`not` (`bvult_64` 42_64 x))", std::string(str));
}

#define SMT_MSAT_TEST_MATH_BINARY_OP_REAL(op, opcode, opname)           \
  TEST(SmtMsatTest, MathBinaryOperatorReal##opcode)                     \
  {                                                                     \
    MsatSolver s;                                                       \
                                                                        \
    const Real e0 = any<Real>("x");                                     \
    const Real e1(42 op e0);                                            \
                                                                        \
    EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));               \
                                                                        \
    const msat_env env = s.env();                                       \
    const msat_term t0 = s.term();                                      \
    EXPECT_TRUE(msat_term_is_##opname(env, t0));                        \
                                                                        \
    const msat_type type = msat_term_get_type(t0);                      \
    EXPECT_TRUE(msat_is_rational_type(env, type));                      \
                                                                        \
    char *str = msat_term_repr(t0);                                     \
    EXPECT_EQ("(`" #op "_rat` 42 x)", std::string(str));                \
  }                                                                     \

#define SMT_MSAT_TEST_MATH_BINARY_OP_INT(op, opcode, opname)            \
  TEST(SmtMsatTest, MathBinaryOperatorInt##opcode)                      \
  {                                                                     \
    MsatSolver s;                                                       \
                                                                        \
    const Int e0 = any<Int>("x");                                       \
    const Int e1(42 op e0);                                             \
                                                                        \
    EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));               \
                                                                        \
    const msat_env env = s.env();                                       \
    const msat_term t0 = s.term();                                      \
    EXPECT_TRUE(msat_term_is_##opname(env, t0));                        \
                                                                        \
    const msat_type type = msat_term_get_type(t0);                      \
    EXPECT_TRUE(msat_is_integer_type(env, type));                       \
                                                                        \
    char *str = msat_term_repr(t0);                                     \
    EXPECT_EQ("(`" #op "_int` 42 x)", std::string(str));                \
  }                                                                     \

// most other integer operators are unsupported by MathSAT5
SMT_MSAT_TEST_MATH_BINARY_OP_INT(+, ADD, plus)
SMT_MSAT_TEST_MATH_BINARY_OP_INT(*, MUL, times)

SMT_MSAT_TEST_MATH_BINARY_OP_REAL(+, ADD, plus)
SMT_MSAT_TEST_MATH_BINARY_OP_REAL(*, MUL, times)

TEST(SmtMsatTest, MathBinaryOperatorIntEQL)
{
  MsatSolver s;

  const Int e0 = any<Int>("x");
  const Bool e1(42 == e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_equal(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`=_int` x 42)", std::string(str));
}

TEST(SmtMsatTest, MathBinaryOperatorIntLSS)
{
  MsatSolver s;

  const Int e0 = any<Int>("x");
  const Bool e1(42 < e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_and(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`and` (`<=_int` 42 x) (`not` (`=_int` x 42)))", std::string(str));
}

TEST(SmtMsatTest, MathBinaryOperatorIntGTR)
{
  MsatSolver s;

  const Int e0 = any<Int>("x");
  const Bool e1(42 > e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_and(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`and` (`<=_int` x 42) (`not` (`=_int` x 42)))", std::string(str));
}

TEST(SmtMsatTest, MathBinaryOperatorIntNEQ)
{
  MsatSolver s;

  const Int e0 = any<Int>("x");
  const Bool e1(42 != e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_not(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`not` (`=_int` x 42))", std::string(str));
}

TEST(SmtMsatTest, MathBinaryOperatorIntLEQ)
{
  MsatSolver s;

  const Int e0 = any<Int>("x");
  const Bool e1(42 <= e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_leq(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`<=_int` 42 x)", std::string(str));
}

TEST(SmtMsatTest, MathBinaryOperatorIntGEQ)
{
  MsatSolver s;

  const Int e0 = any<Int>("x");
  const Bool e1(42 >= e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_leq(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`<=_int` x 42)", std::string(str));
}

TEST(SmtMsatTest, MathBinaryOperatorRealEQL)
{
  MsatSolver s;

  const Real e0 = any<Real>("x");
  const Bool e1(42 == e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_equal(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`=_rat` x 42)", std::string(str));
}

TEST(SmtMsatTest, MathBinaryOperatorRealLSS)
{
  MsatSolver s;

  const Real e0 = any<Real>("x");
  const Bool e1(42 < e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_and(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`and` (`<=_rat` 42 x) (`not` (`=_rat` x 42)))", std::string(str));
}

TEST(SmtMsatTest, MathBinaryOperatorRealGTR)
{
  MsatSolver s;

  const Real e0 = any<Real>("x");
  const Bool e1(42 > e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_and(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`and` (`<=_rat` x 42) (`not` (`=_rat` x 42)))", std::string(str));
}

TEST(SmtMsatTest, MathBinaryOperatorRealNEQ)
{
  MsatSolver s;

  const Real e0 = any<Real>("x");
  const Bool e1(42 != e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_not(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`not` (`=_rat` x 42))", std::string(str));
}

TEST(SmtMsatTest, MathBinaryOperatorRealLEQ)
{
  MsatSolver s;

  const Real e0 = any<Real>("x");
  const Bool e1(42 <= e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_leq(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`<=_rat` 42 x)", std::string(str));
}

TEST(SmtMsatTest, MathBinaryOperatorRealGEQ)
{
  MsatSolver s;

  const Real e0 = any<Real>("x");
  const Bool e1(42 >= e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_leq(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`<=_rat` x 42)", std::string(str));
}

TEST(SmtMsatTest, BoolUnaryOperatorLNOT)
{
  MsatSolver s;

  const Bool e0 = any<Bool>("x");
  const Bool e1(!e0);

  EXPECT_EQ(OK, static_cast<SharedExpr>(e1).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_not(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`not` x)", std::string(str));
}

#define SMT_MSAT_TEST_BOOL_BINARY_OP(op, opcode, opname)                \
  TEST(SmtMsatTest, BoolBinaryOperator##opcode)                         \
  {                                                                     \
    MsatSolver s;                                                       \
                                                                        \
    const Bool e0 = any<Bool>("x");                                     \
    const Bool e1 = any<Bool>("y");                                     \
    const Bool e2(e0 op e1);                                            \
                                                                        \
    EXPECT_EQ(OK, static_cast<SharedExpr>(e2).encode(s));               \
                                                                        \
    const msat_env env = s.env();                                       \
    const msat_term t0 = s.term();                                      \
    EXPECT_TRUE(msat_term_is_##opname(env, t0));                        \
                                                                        \
    const msat_type type = msat_term_get_type(t0);                      \
    EXPECT_TRUE(msat_is_bool_type(env, type));                          \
                                                                        \
    char *str = msat_term_repr(t0);                                     \
    EXPECT_EQ("(`" #opname "` x y)", std::string(str));                 \
  }                                                                     \

SMT_MSAT_TEST_BOOL_BINARY_OP(&&, LAND, and)
SMT_MSAT_TEST_BOOL_BINARY_OP(||, LOR, or)
SMT_MSAT_TEST_BOOL_BINARY_OP(==, EQL, iff)

TEST(SmtMsatTest, LogicalImplication)
{
  MsatSolver s;

  const Bool e0 = any<Bool>("x");
  const Bool e1 = any<Bool>("y");
  const Bool e2(implies(e0, e1));

  EXPECT_EQ(OK, static_cast<SharedExpr>(e2).encode(s));

  const msat_env env = s.env();
  const msat_term t0 = s.term();
  EXPECT_TRUE(msat_term_is_or(env, t0));

  const msat_type type = msat_term_get_type(t0);
  EXPECT_TRUE(msat_is_bool_type(env, type));

  char *str = msat_term_repr(t0);
  EXPECT_EQ("(`or` y (`not` x))", std::string(str));
}

TEST(SmtMsatTest, Reset)
{
  MsatSolver s;

  EXPECT_EQ(sat, s.check());
  s.add(literal<Bool>(false));
  EXPECT_EQ(unsat, s.check());
  s.reset();
  EXPECT_EQ(sat, s.check());
}

TEST(SmtMsatTest, Distinct)
{
  MsatSolver s;

  const Bv<long> x = any<Bv<long>>("x");
  const Bv<long> y = any<Bv<long>>("y");
  const Bv<long> z = any<Bv<long>>("z");
  const Bv<long> w = any<Bv<long>>("w");

  Terms<Bv<long>> operand_terms(3);
  operand_terms.push_back(x);
  operand_terms.push_back(y);
  operand_terms.push_back(z);

  Bool d(distinct(std::move(operand_terms)));

  static_cast<SharedExpr>(d).encode(s);
  char *str = msat_term_repr(s.term());
  EXPECT_EQ("(`and` (`and` (`not` (`=_<BitVec, 64, >` x y))"
            " (`not` (`=_<BitVec, 64, >` x z)))"
            " (`not` (`=_<BitVec, 64, >` y z)))", std::string(str));

  s.add(d);

  EXPECT_EQ(sat, s.check());

  s.push();
  {
    s.add(x == y);
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == z);
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(y == z);
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == w);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(y == w);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(z == w);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();
}

TEST(SmtMsatTest, Conjunction)
{
  MsatSolver s;

  const Bool x = any<Bool>("x");
  const Bool y = any<Bool>("y");
  const Bool z = any<Bool>("z");
  const Bool w = any<Bool>("w");

  Terms<Bool> operand_terms(3);
  operand_terms.push_back(x);
  operand_terms.push_back(y);
  operand_terms.push_back(z);

  Bool d(conjunction(std::move(operand_terms)));

  EXPECT_EQ(OK, static_cast<SharedExpr>(d).encode(s));
  s.add(d);

  EXPECT_EQ(sat, s.check());

  s.push();
  {
    s.add(not x);
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(not y);
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(not z);
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(not w);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();
}

TEST(SmtMsatTest, Disjunction)
{
  MsatSolver s;

  const Bool x = any<Bool>("x");
  const Bool y = any<Bool>("y");
  const Bool z = any<Bool>("z");
  const Bool w = any<Bool>("w");

  Terms<Bool> operand_terms(3);
  operand_terms.push_back(x);
  operand_terms.push_back(y);
  operand_terms.push_back(z);

  Bool d(disjunction(std::move(operand_terms)));

  EXPECT_EQ(OK, static_cast<SharedExpr>(d).encode(s));

  s.add(d);

  EXPECT_EQ(sat, s.check());

  s.push();
  {
    s.add(not x and not y and not z);
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(not x);
    s.add(not y);
    s.add(not z);
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(not x);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(not y);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(not z);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(not w);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();
}

TEST(SmtMsatTest, UnsafeAdd)
{
  MsatSolver s;

  const Sort& bv_sort = internal::sort<Bv<int64_t>>();
  const Sort& func_sort = internal::sort<Func<Bv<int64_t>, Bv<int64_t>>>();
  const UnsafeDecl const_decl("x", bv_sort);
  const UnsafeDecl func_decl("f", func_sort);
  const SharedExpr seven_term(literal(bv_sort, 7));
  const SharedExpr x_term(constant(const_decl));
  const SharedExpr app_term(apply(func_decl, seven_term));

  SharedExprs terms;
  terms.push_back(seven_term);
  terms.push_back(x_term);
  terms.push_back(app_term);

  const SharedExpr distinct_term(distinct(std::move(terms)));

  const Sort& array_sort = internal::sort<Array<Bv<uint32_t>, Bv<int64_t>>>();
  const Sort& index_sort = internal::sort<Bv<uint32_t>>();
  const UnsafeDecl array_decl("array", array_sort);
  const UnsafeDecl index_decl("index", index_sort);
  const SharedExpr array_term(constant(array_decl));
  const SharedExpr index_term(constant(index_decl));
  const SharedExpr store_term(store(array_term, index_term, app_term));
  const SharedExpr select_term(select(store_term, index_term));

  const SharedExpr eq_term(select_term == x_term);
  const SharedExpr and_term(eq_term && distinct_term);

  char *str;
  s.push();
  {
    s.unsafe_add(and_term);
    EXPECT_EQ(unsat, s.check());

    str = msat_term_repr(s.term());
    EXPECT_EQ(
      "(`and` (`=_<BitVec, 64, >` "
        "(`read_<BitVec, 32, >_<BitVec, 64, >` "
          "(`write_<BitVec, 32, >_<BitVec, 64, >` array index (f 7_64)) index) x) "
      "(`and` (`and` (`not` (`=_<BitVec, 64, >` x 7_64)) "
        "(`not` (`=_<BitVec, 64, >` (f 7_64) 7_64))) "
        "(`not` (`=_<BitVec, 64, >` (f 7_64) x))))", std::string(str));
  }
  s.pop();

  s.push();
  {
    s.unsafe_add(seven_term != 7);
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.unsafe_add(7 == seven_term);
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.unsafe_add(x_term == x_term + 1);
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.unsafe_add(x_term + 1 == x_term);
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();
}

// Usually there isn't a theory to mix bit vectors and integers.
// But MathSAT5 is likely to internally convert these sorts. This
// is generally only acceptable for early prototyping.
TEST(SmtMsatTest, AutoConfig)
{
  MsatSolver solver;

  auto x = any<Bv<long>>("x");
  solver.add(0 < x);

  auto y = any<Int>("y");
  solver.add(0 < y);

  EXPECT_EQ(smt::sat, solver.check());

  solver.push();
  {
    solver.add(3 == x);
    EXPECT_EQ(smt::sat, solver.check());
  }
  solver.pop();

  EXPECT_EQ(smt::sat, solver.check());

  solver.push();

  {
    solver.add(0 > x);
    EXPECT_EQ(smt::unsat, solver.check());
  }
  solver.pop();

  EXPECT_EQ(smt::sat, solver.check());

  solver.push();
  {
    solver.add(3 == y);
    EXPECT_EQ(smt::sat, solver.check());
  }
  solver.pop();

  EXPECT_EQ(smt::sat, solver.check());

  solver.push();
  {
    solver.add(0 > y);
    EXPECT_EQ(smt::unsat, solver.check());
  }
  solver.pop();

  EXPECT_EQ(smt::sat, solver.check());
}

TEST(SmtMsatTest, QF_BV)
{
  MsatSolver solver(QF_BV_LOGIC);

  auto x = any<Bv<long>>("x");
  solver.add(0 < x);

  EXPECT_EQ(smt::sat, solver.check());

  solver.push();
  {
    solver.add(3 == x);
    EXPECT_EQ(smt::sat, solver.check());
  }
  solver.pop();

  EXPECT_EQ(smt::sat, solver.check());

  solver.push();

  {
    solver.add(0 > x);
    EXPECT_EQ(smt::unsat, solver.check());
  }
  solver.pop();

  EXPECT_EQ(smt::sat, solver.check());
}

TEST(SmtMsatTest, QF_IDL)
{
  MsatSolver solver(QF_IDL_LOGIC);

  auto y = any<Int>("y");
  solver.add(0 < y);

  EXPECT_EQ(smt::sat, solver.check());

  solver.push();
  {
    solver.add(3 == y);
    EXPECT_EQ(smt::sat, solver.check());
  }
  solver.pop();

  EXPECT_EQ(smt::sat, solver.check());

  solver.push();
  {
    solver.add(0 > y);
    EXPECT_EQ(smt::unsat, solver.check());
  }
  solver.pop();

  EXPECT_EQ(smt::sat, solver.check());
}

TEST(SmtMsatTest, BvSignExtend)
{
  MsatSolver s(QF_BV_LOGIC);

  Bv<int8_t> x = any<Bv<int8_t>>("x");
  Bv<int16_t> y = any<Bv<int16_t>>("y");
  Bv<uint16_t> z = any<Bv<uint16_t>>("z");

  s.push();
  {
    s.add(x == 0x07);
    s.add(y == bv_cast<int16_t>(x));
    s.add(y == 0x0007);

    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == 0x07);
    s.add(y == bv_cast<int16_t>(x));
    s.add(y != 0x0007);

    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == 0x87);
    s.add(y == bv_cast<int16_t>(x));
    s.add(y == 0xff87);

    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == 0x87);
    s.add(y == bv_cast<int16_t>(x));
    s.add(y != 0xff87);

    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == 0x87);

    // we cast from a signed to an unsigned integer so sign extension is required,
    // see inline comments in bv_cast<T>(const Bv<S>&) about C++ specification
    s.add(z == bv_cast<uint16_t>(x));
    s.add(z == 0xff87U);

    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == 0x87);

    // we cast from a signed to an unsigned integer so sign extension is required,
    // see inline comments in bv_cast<T>(const Bv<S>&) about C++ specification
    s.add(z == bv_cast<uint16_t>(x));
    s.add(z != 0xff87U);

    EXPECT_EQ(unsat, s.check());
  }
  s.pop();
}

TEST(SmtMsatTest, BvZeroExtend)
{
  MsatSolver s(QF_BV_LOGIC);

  Bv<uint8_t> x = any<Bv<uint8_t>>("x");
  Bv<int16_t> y = any<Bv<int16_t>>("y");
  Bv<uint16_t> z = any<Bv<uint16_t>>("z");

  s.push();
  {
    s.add(x == 0x07);
    s.add(y == bv_cast<int16_t>(x));
    s.add(y == 0x0007);

    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == 0x07);
    s.add(y == bv_cast<int16_t>(x));
    s.add(y != 0x0007);

    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == 0x87);

    // we cast from an unsigned to a signed integer so extend by zeros.
    s.add(y == bv_cast<int16_t>(x));
    s.add(y == 0x0087);

    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == 0x87);

    // we cast from an unsigned to a signed integer so extend by zeros.
    s.add(y == bv_cast<int16_t>(x));
    s.add(y != 0x0087);

    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == 0x87);
    s.add(z == bv_cast<uint16_t>(x));
    s.add(z == 0x0087U);

    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == 0x87);
    s.add(z == bv_cast<uint16_t>(x));
    s.add(z != 0x0087U);

    EXPECT_EQ(unsat, s.check());
  }
  s.pop();
}

TEST(SmtMsatTest, BvTruncate)
{
  MsatSolver s(QF_BV_LOGIC);

  Bv<int16_t> x = any<Bv<int16_t>>("x");
  Bv<int8_t> y = any<Bv<int8_t>>("y");
  Bv<uint8_t> z = any<Bv<uint8_t>>("z");

  s.push();
  {
    s.add(x == 0xbeef);
    s.add(y == bv_cast<int8_t>(x));
    s.add(y == 0xef);

    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == 0xbeef);
    s.add(y == bv_cast<int8_t>(x));
    s.add(y != 0xefU);

    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == 0xbeef);
    s.add(z == bv_cast<uint8_t>(x));
    s.add(z != 0xefU);

    EXPECT_EQ(unsat, s.check());
  }
  s.pop();
}

TEST(SmtMsatTest, CheckAssumptions)
{
  MsatSolver s;
  std::pair<CheckResult, Bools::SizeType> r;

  // ignore
  Bools unsat_core;
  unsat_core.resize(0);

  Bool a;
  Bool b;
  Bool c;

  Int x = any<Int>("x");

  {
    a = x < 7;
    b = !a;

    Bools assumptions;
    assumptions.push_back(a);
    assumptions.push_back(b);

    r = s.check_assumptions(assumptions, unsat_core);
    EXPECT_EQ(unsat, r.first);

    assumptions.pop_back();
    assumptions.push_back(a);
    r = s.check_assumptions(assumptions, unsat_core);
    EXPECT_EQ(sat, r.first);
  }

  s.reset();

  {
    a = x < 7;
    b = !a;

    Bools assumptions;
    assumptions.push_back(b);

    s.add(a);
    r = s.check_assumptions(assumptions, unsat_core);
    EXPECT_EQ(unsat, r.first);

    assumptions.pop_back();
    assumptions.push_back(a);
    r = s.check_assumptions(assumptions, unsat_core);
    EXPECT_EQ(sat, r.first);
  }
}

TEST(SmtMsatTest, UnsatCore)
{
  MsatSolver s;
  std::pair<CheckResult, Bools::SizeType> r;

  Bool a = any<Bool>("a");
  Bool b = any<Bool>("b");
  Bool c = any<Bool>("c");
  Bool not_b = not b;
  Bool d = any<Bool>("d");

  Int x = any<Int>("x");
  Int y = any<Int>("y");
  Int z = any<Int>("z");

  Bools unsat_core;

  {
    Bools assumptions;
    assumptions.push_back(a);
    assumptions.push_back(b);
    assumptions.push_back(not_b);
    assumptions.push_back(d);

    unsat_core.resize(7);
    r = s.check_assumptions(assumptions, unsat_core);

    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(2, r.second);

    EXPECT_EQ(not_b.addr(), unsat_core.at(unsat_core.size() - 1).addr());
    EXPECT_EQ(b.addr(), unsat_core.at(unsat_core.size() - 2).addr());

    // singleton
    unsat_core.resize(1);
    r = s.check_assumptions(assumptions, unsat_core);
    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(1, r.second);
    EXPECT_EQ(not_b.addr(), unsat_core.back().addr());
  }

  s.reset();

  {
    // assertion will contradict assumption
    s.add(b);

    Bools assumptions;
    assumptions.push_back(a);
    assumptions.push_back(not_b);
    assumptions.push_back(c);
    assumptions.push_back(d);

    unsat_core.resize(7);
    r = s.check_assumptions(assumptions, unsat_core);

    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(1, r.second);

    EXPECT_EQ(not_b.addr(), unsat_core.back().addr());

    // singleton
    unsat_core.resize(1);
    r = s.check_assumptions(assumptions, unsat_core);
    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(1, r.second);
    EXPECT_EQ(not_b.addr(), unsat_core.back().addr());
  }

  s.reset();

  {
    // duplicate in assumption
    Bools assumptions;
    assumptions.push_back(a);
    assumptions.push_back(b);
    assumptions.push_back(not_b);
    assumptions.push_back(b);
    assumptions.push_back(d);

    unsat_core.resize(7);
    r = s.check_assumptions(assumptions, unsat_core);

    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(2, r.second);

    EXPECT_EQ(not_b.addr(), unsat_core.at(unsat_core.size() - 1).addr());
    EXPECT_EQ(b.addr(), unsat_core.at(unsat_core.size() - 2).addr());

    // singleton
    unsat_core.resize(1);
    r = s.check_assumptions(assumptions, unsat_core);
    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(1, r.second);
    EXPECT_EQ(not_b.addr(), unsat_core.back().addr());
  }

  s.reset();

  {
    s.add(smt::implies(a, x < 5));
    s.add(smt::implies(b, y < x));
    s.add(smt::implies(c, z < 2));
    s.add(smt::implies(d, 5 < y));

    Bools assumptions;
    assumptions.push_back(a);
    assumptions.push_back(b);
    assumptions.push_back(c);
    assumptions.push_back(d);

    unsat_core.resize(7);
    r = s.check_assumptions(assumptions, unsat_core);

    EXPECT_EQ(smt::unsat, r.first);

    unsat_core.resize(7);
    r = s.check_assumptions(assumptions, unsat_core);

    EXPECT_EQ(3, r.second);
    EXPECT_EQ(d.addr(), unsat_core.at(unsat_core.size() - 1).addr());
    EXPECT_EQ(b.addr(), unsat_core.at(unsat_core.size() - 2).addr());
    EXPECT_EQ(a.addr(), unsat_core.at(unsat_core.size() - 3).addr());

    // singleton
    unsat_core.resize(1);
    r = s.check_assumptions(assumptions, unsat_core);
    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(1, r.second);
    EXPECT_EQ(d.addr(), unsat_core.back().addr());
  }

  s.reset();

  {
    a = x < 5;
    b = y < x;
    c = z < 2;
    d = 5 < y;

    Bools assumptions;
    assumptions.push_back(a);
    assumptions.push_back(b);
    assumptions.push_back(c);
    assumptions.push_back(d);

    unsat_core.resize(7);
    r = s.check_assumptions(assumptions, unsat_core);

    EXPECT_EQ(smt::unsat, r.first);

    unsat_core.resize(7);
    r = s.check_assumptions(assumptions, unsat_core);

    EXPECT_EQ(3, r.second);
    EXPECT_EQ(d.addr(), unsat_core.at(unsat_core.size() - 1).addr());
    EXPECT_EQ(b.addr(), unsat_core.at(unsat_core.size() - 2).addr());
    EXPECT_EQ(a.addr(), unsat_core.at(unsat_core.size() - 3).addr());

    // singleton
    unsat_core.resize(1);
    r = s.check_assumptions(assumptions, unsat_core);
    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(1, r.second);
    EXPECT_EQ(d.addr(), unsat_core.back().addr());
  }

  s.reset();

  {
    a = x < 7;
    b = !(x < 7);
    c = !(x < 4);

    Bools assumptions;
    assumptions.push_back(b);
    assumptions.push_back(c);

    s.add(a);

    unsat_core.resize(7);
    r = s.check_assumptions(assumptions, unsat_core);

    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(1, r.second);
    EXPECT_EQ(b.addr(), unsat_core.back().addr());

    // singleton
    unsat_core.resize(1);
    r = s.check_assumptions(assumptions, unsat_core);

    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(1, r.second);
    EXPECT_EQ(b.addr(), unsat_core.back().addr());
  }

  Bv<char> letter = any<Bv<char>>("letter");

  s.reset();

  {
    a = letter <= 'A';
    b = 'Z' <= letter;
    c = 'Z' <= letter;
    Bools assumptions;
    assumptions.push_back(a);
    assumptions.push_back(b);
    assumptions.push_back(c);

    unsat_core.resize(7);
    r = s.check_assumptions(assumptions, unsat_core);

    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(2, r.second);
    EXPECT_EQ(b.addr(), unsat_core.back().addr());
  }

  s.reset();

  {
    a = letter <= 'A';
    b = 'Z' <= letter;
    c = 'Z' <= letter;
    Bools assumptions;
    assumptions.push_back(a);
    assumptions.push_back(b);
    assumptions.push_back(c);

    unsat_core.resize(2);
    r = s.check_assumptions(assumptions, unsat_core);

    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(2, r.second);
    EXPECT_EQ(b.addr(), unsat_core.back().addr());
  }

  s.reset();

  {
    Bool d;

    a = letter <= 'A';
    b = letter <= 'Z';
    c = (letter + '\1') > 'Z';
    d = (letter + '\2') > 'Z';

    Bools assumptions;
    assumptions.push_back(a);
    assumptions.push_back(b);
    assumptions.push_back(c);
    assumptions.push_back(d);

    unsat_core.resize(7);
    r = s.check_assumptions(assumptions, unsat_core);

    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(2, r.second);
    EXPECT_EQ(c.addr(), unsat_core.back().addr());
  }

  s.reset();

  {
    Bool d;

    a = letter <= 'A';
    b = letter <= 'Z';
    c = (letter + '\1') > 'Z';
    d = (letter + '\1') > 'Z';

    Bools assumptions;
    assumptions.push_back(a);
    assumptions.push_back(b);
    assumptions.push_back(c);
    assumptions.push_back(d);

    unsat_core.resize(7);
    r = s.check_assumptions(assumptions, unsat_core);

    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(2, r.second);
    EXPECT_EQ(c.addr(), unsat_core.back().addr());
  }

  s.reset();

  {
    Bool d, e;

    a = letter <= 'A';
    b = letter <= 'Z';
    c = (letter + '\1') > 'Z';
    d = (letter + '\2') > 'Z';
    e = (letter + '\2') > 'Z';

    Bools assumptions;
    assumptions.push_back(a);
    assumptions.push_back(b);
    assumptions.push_back(c);
    assumptions.push_back(d);
    assumptions.push_back(e);

    unsat_core.resize(7);
    r = s.check_assumptions(assumptions, unsat_core);

    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(2, r.second);
    EXPECT_EQ(c.addr(), unsat_core.back().addr());
  }

  s.reset();

  {
    Bool d, e;

    a = !('A' < letter);
    b = letter < 'Z';
    c = (letter + '\1') < 'Z';
    d = (letter + '\2') > 'Z';
    e = (letter + '\2') > 'Z';

    Bools assumptions;
    assumptions.push_back(a);
    assumptions.push_back(b);
    assumptions.push_back(c);
    assumptions.push_back(d);
    assumptions.push_back(e);

    unsat_core.resize(7);
    r = s.check_assumptions(assumptions, unsat_core);

    /// unsat_core.back() should be ideally "d"
    EXPECT_EQ(unsat, r.first);
    EXPECT_EQ(2, r.second);
    EXPECT_EQ(e.addr(), unsat_core.back().addr());
  }
}

TEST(SmtMsatTest, LogicalShifts)
{
  MsatSolver s;

  Bv<unsigned> x = any<Bv<unsigned>>("x");
  Bv<unsigned> one = literal<Bv<unsigned>>(1);
  Bv<unsigned> two = literal<Bv<unsigned>>(2);

  s.push();
  {
    s.add(two * x != (x << one));
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(two * x == (x << one));
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x != ((x << one) >> one));
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(x == ((x << one) >> one));
    EXPECT_EQ(sat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(one != (two >> one));
    EXPECT_EQ(unsat, s.check());
  }
  s.pop();

  s.push();
  {
    s.add(one == (two >> one));
    EXPECT_EQ(sat, s.check());
  }
  s.pop();
}
