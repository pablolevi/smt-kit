// Copyright 2013, Alex Horn. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef __SMT_CVC4_H_
#define __SMT_CVC4_H_

// Conflicts with gtest headers, include gtest/gtest.h _after_ smt_cvc4.h!
#undef EXPECT_TRUE
#undef EXPECT_FALSE
#undef Message

#include "smt.h"

// Include CVC4 headers _after_ smt.h due to dependency on cstddef with g++ 4.9
#include <cvc4/expr/expr.h>
#include <cvc4/expr/expr_manager.h>
#include <cvc4/smt/smt_engine.h>

#include <unordered_map>

namespace smt
{

class CVC4Solver : public Solver
{
private:
  CVC4::ExprManager m_expr_manager;

  // SmtEngine is not intended to be copy-constructed so
  // we keep a pointer in order to implement __reset()
  CVC4::SmtEngine* m_smt_engine;
  CVC4::Expr m_expr;

#ifdef ENABLE_HASH_CONS
  typedef std::unordered_map<uintptr_t, const CVC4::Expr> ExprMap;
#else
  // We should not use symbol names as key because these need not be unique
  // across different SMT-LIB 2.0 namespaces such as sorts, bindings etc.
  typedef std::unordered_map<std::string, const CVC4::Expr> ExprMap;
#endif

  // Cache CVC4 expressions
  ExprMap m_expr_map;

  void set_expr(const CVC4::Expr& expr)
  {
    m_expr = expr;
  }

  template<typename T>
  Error nostring_encode_number(const Sort& sort, T literal)
  {
    assert(!sort.is_bool());

    if (sort.is_bv()) {
      set_expr(m_expr_manager.mkConst(CVC4::BitVector(sort.bv_size(),
        CVC4::Integer(literal))));
    } else if (sort.is_int() || sort.is_real()) {
      set_expr(m_expr_manager.mkConst(CVC4::Rational(literal)));
    } else {
      return UNSUPPORT_ERROR;
    }

    return OK;
  }

  Error string_encode_number(const Sort& sort, std::string&& literal)
  {
    assert(!sort.is_bool());

    // Should use move semantics on the integer once CVC4 supports it
    const CVC4::Integer integer(std::move(literal));
    if (sort.is_bv()) {
      set_expr(m_expr_manager.mkConst(CVC4::BitVector(sort.bv_size(), integer)));
    } else if (sort.is_int() || sort.is_real()) {
      set_expr(m_expr_manager.mkConst(CVC4::Rational(integer)));
    } else {
      return UNSUPPORT_ERROR;
    }

    return OK;
  }

  virtual Error __encode_literal(
    const Expr* const expr,
    bool literal) override
  {
    assert(expr->sort().is_bool());

    set_expr(m_expr_manager.mkConst(literal));
    return OK;
  }

#define SMT_CVC4_NOSTRING_ENCODE_LITERAL(type)                            \
  virtual Error __encode_literal(                                         \
    const Expr* const expr,                                               \
    type literal) override                                                \
  {                                                                       \
    return nostring_encode_number<type>(expr->sort(), literal);           \
  }                                                                       \

#define SMT_CVC4_STRING_ENCODE_LITERAL(type)                              \
  virtual Error __encode_literal(                                         \
    const Expr* const expr,                                               \
    type literal) override                                                \
  {                                                                       \
    return string_encode_number(expr->sort(), std::to_string(literal));   \
  }                                                                       \

SMT_CVC4_NOSTRING_ENCODE_LITERAL(char)
SMT_CVC4_NOSTRING_ENCODE_LITERAL(signed char)
SMT_CVC4_NOSTRING_ENCODE_LITERAL(unsigned char)

SMT_CVC4_STRING_ENCODE_LITERAL(wchar_t)
SMT_CVC4_STRING_ENCODE_LITERAL(char16_t)
SMT_CVC4_STRING_ENCODE_LITERAL(char32_t)

SMT_CVC4_NOSTRING_ENCODE_LITERAL(short)
SMT_CVC4_NOSTRING_ENCODE_LITERAL(unsigned short)
SMT_CVC4_NOSTRING_ENCODE_LITERAL(int)
SMT_CVC4_NOSTRING_ENCODE_LITERAL(unsigned int)
SMT_CVC4_NOSTRING_ENCODE_LITERAL(long)
SMT_CVC4_NOSTRING_ENCODE_LITERAL(unsigned long)

SMT_CVC4_STRING_ENCODE_LITERAL(long long)
SMT_CVC4_STRING_ENCODE_LITERAL(unsigned long long)

  Error build_type(const Sort& sort, CVC4::Type& type)
  {
    if (sort.is_bool()) {
      type = m_expr_manager.booleanType();
    } else if (sort.is_int()) {
      type = m_expr_manager.integerType();
    } else if (sort.is_bv()) {
      type = m_expr_manager.mkBitVectorType(sort.bv_size());
    } else if (sort.is_func()) {
      Error err;
      const size_t sorts_size = sort.sorts_size();
      std::vector<CVC4::Type> types;
      types.reserve(sorts_size);
      for (int i = 0; i < sorts_size; i++) {
        CVC4::Type type;
        err = build_type(sort.sorts(i), type);
        if (err) {
          return err;
        }
        types.push_back(type);
      }

      type = m_expr_manager.mkFunctionType(types);
    } else if (sort.is_array()) {
      CVC4::Type domain_type;
      CVC4::Type range_type;

      Error err;
      err = build_type(sort.sorts(0), domain_type);
      if (err) {
        return err;
      }
      err = build_type(sort.sorts(1), range_type);
      if (err) {
        return err;
      }

      type = m_expr_manager.mkArrayType(domain_type, range_type);
    } else {
      return UNSUPPORT_ERROR;
    }

    return OK;
  }

  virtual Error __encode_constant(
    const Expr* const expr,
    const UnsafeDecl& decl) override
  {
    Error err;

    const std::string& const_symbol = decl.symbol();

#ifdef ENABLE_HASH_CONS
    uintptr_t key = reinterpret_cast<uintptr_t>(expr);
#else
    const std::string& key = const_symbol;
#endif

    ExprMap::const_iterator it = m_expr_map.find(key);
    if (it == m_expr_map.cend()) {
      CVC4::Type type;
      err = build_type(decl.sort(), type);
      if (err) {
        return err;
      }

      set_expr(m_expr_manager.mkVar(const_symbol, type));
      m_expr_map.insert(ExprMap::value_type(key, m_expr));
    } else {
      set_expr(it->second);
    }

    return OK;
  }

  virtual Error __encode_func_app(
    const Expr* const expr,
    const UnsafeDecl& decl,
    const size_t arity,
    const SharedExpr* const args) override
  {
    Error err;
    CVC4::Expr func_expr;
    const std::string& func_symbol = decl.symbol();

#ifdef ENABLE_HASH_CONS
    uintptr_t key = reinterpret_cast<uintptr_t>(expr);
#else
    const std::string& key = func_symbol;
#endif

    ExprMap::const_iterator it = m_expr_map.find(key);
    if (it == m_expr_map.cend()) {
      CVC4::Type func_type;
      err = build_type(decl.sort(), func_type);
      if (err) {
        return err;
      }
      func_expr = m_expr_manager.mkVar(func_symbol, func_type);
      m_expr_map.insert(ExprMap::value_type(key, func_expr));
    } else {
      func_expr = it->second;
    }

    std::vector<CVC4::Expr> exprs;
    exprs.reserve(arity);
    for (int i = 0; i < arity; i++) {
      err = args[i].encode(*this);
      if (err) {
        return err;
      }
      exprs.push_back(m_expr);
    }

    set_expr(m_expr_manager.mkExpr(CVC4::kind::APPLY_UF, func_expr, exprs));
    return OK;
  }

  virtual Error __encode_const_array(
    const Expr* const expr,
    const SharedExpr& init) override
  {
    Error err;

    CVC4::Type type;
    err = build_type(expr->sort(), type);
    if (err) {
      return err;
    }

    err = init.encode(*this);
    if (err) {
      return err;
    }

    const CVC4::ArrayType array_type(type);
    set_expr(m_expr_manager.mkConst(CVC4::ArrayStoreAll(array_type, m_expr)));
    return OK;
  }

  virtual Error __encode_array_select(
    const Expr* const expr,
    const SharedExpr& array,
    const SharedExpr& index) override
  {
    Error err;
    err = array.encode(*this);
    if (err) {
      return err;
    }
    const CVC4::Expr array_expr = m_expr;

    err = index.encode(*this);
    if (err) {
      return err;
    }
    const CVC4::Expr index_expr = m_expr;

    set_expr(m_expr_manager.mkExpr(CVC4::kind::SELECT,
      array_expr, index_expr));
    return OK;
  }

  virtual Error __encode_array_store(
    const Expr* const expr,
    const SharedExpr& array,
    const SharedExpr& index,
    const SharedExpr& value) override
  {
    Error err;
    err = array.encode(*this);
    if (err) {
      return err;
    }
    const CVC4::Expr array_expr = m_expr;

    err = index.encode(*this);
    if (err) {
      return err;
    }
    const CVC4::Expr index_expr = m_expr;

    err = value.encode(*this);
    if (err) {
      return err;
    }
    const CVC4::Expr value_expr = m_expr;

    set_expr(m_expr_manager.mkExpr(CVC4::kind::STORE,
      array_expr, index_expr, value_expr));
    return OK;
  }

  virtual Error __encode_unary_lnot(
    const Expr* const expr,
    const SharedExpr& arg) override
  {
    const Error err = arg.encode(*this);
    if (err)
      return err;

    set_expr(m_expr_manager.mkExpr(CVC4::kind::NOT, m_expr));
    return OK;
  }

  virtual Error __encode_unary_not(
    const Expr* const expr,
    const SharedExpr& arg) override
  {
    const Error err = arg.encode(*this);
    if (err)
      return err;

    set_expr(m_expr_manager.mkExpr(CVC4::kind::BITVECTOR_NOT, m_expr));
    return OK;
  }

  virtual Error __encode_unary_sub(
    const Expr* const expr,
    const SharedExpr& arg) override
  {
    const Error err = arg.encode(*this);
    if (err)
      return err;

    CVC4::kind::Kind_t kind;
    if (expr->sort().is_bv())
      kind = CVC4::kind::BITVECTOR_NEG;
    else
      kind = CVC4::kind::UMINUS;

    set_expr(m_expr_manager.mkExpr(kind, m_expr));
    return OK;
  }

  Error encode_binary(
    const Expr* const expr,
    CVC4::kind::Kind_t kind,
    const SharedExpr& larg,
    const SharedExpr& rarg)
  {
    Error err;
    err = larg.encode(*this);
    if (err)
      return err;

    const CVC4::Expr lexpr(m_expr);

    err = rarg.encode(*this);
    if (err)
      return err;

    const CVC4::Expr rexpr(m_expr);

    set_expr(m_expr_manager.mkExpr(kind, lexpr, rexpr));
    return OK;
  }

  virtual Error __encode_binary_sub(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    CVC4::kind::Kind_t kind = CVC4::kind::MINUS;

    if (expr->sort().is_bv())
      kind = CVC4::kind::BITVECTOR_SUB;

    return encode_binary(expr, kind, larg, rarg);
  }

  virtual Error __encode_binary_and(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    return encode_binary(expr,
      CVC4::kind::BITVECTOR_AND, larg, rarg);
  }

  virtual Error __encode_binary_or(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    return encode_binary(expr,
      CVC4::kind::BITVECTOR_OR, larg, rarg);
  }

  virtual Error __encode_binary_xor(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    return encode_binary(expr,
      CVC4::kind::BITVECTOR_XOR, larg, rarg);
  }

  virtual Error __encode_binary_lshl(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    return encode_binary(expr,
      CVC4::kind::BITVECTOR_SHL, larg, rarg);
  }

  virtual Error __encode_binary_lshr(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    return encode_binary(expr,
      CVC4::kind::BITVECTOR_LSHR, larg, rarg);
  }

  virtual Error __encode_binary_land(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    return encode_binary(expr,
      CVC4::kind::AND, larg, rarg);
  }

  virtual Error __encode_binary_lor(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    return encode_binary(expr,
      CVC4::kind::OR, larg, rarg);
  }

  virtual Error __encode_binary_imp(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    return encode_binary(expr,
      CVC4::kind::IMPLIES, larg, rarg);
  }

  virtual Error __encode_binary_eql(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    CVC4::kind::Kind_t kind = CVC4::kind::EQUAL;

    if (larg.sort().is_bool())
      kind = CVC4::kind::IFF;

    return encode_binary(expr, kind, larg, rarg);
  }

  virtual Error __encode_binary_add(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    CVC4::kind::Kind_t kind = CVC4::kind::PLUS;

    if (expr->sort().is_bv())
      kind = CVC4::kind::BITVECTOR_PLUS;

    return encode_binary(expr, kind, larg, rarg);
  }

  virtual Error __encode_binary_mul(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    CVC4::kind::Kind_t kind = CVC4::kind::MULT;

    if (expr->sort().is_bv())
      kind = CVC4::kind::BITVECTOR_MULT;

    return encode_binary(expr, kind, larg, rarg);
  }

  virtual Error __encode_binary_quo(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    CVC4::kind::Kind_t kind;

    const Sort& sort = expr->sort();
    if (sort.is_bv())
    {
      if (sort.is_signed())
        kind = CVC4::kind::BITVECTOR_SDIV;
      else
        kind = CVC4::kind::BITVECTOR_UDIV;
    }
    else if (sort.is_int())
    {
      kind = CVC4::kind::INTS_DIVISION;
    }
    else if (sort.is_real())
    {
      kind = CVC4::kind::DIVISION;
    }
    else
    {
      return UNSUPPORT_ERROR;
    }

    return encode_binary(expr, kind, larg, rarg);
  }

  virtual Error __encode_binary_rem(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    CVC4::kind::Kind_t kind;

    const Sort& sort = expr->sort();
    if (sort.is_bv())
    {
      if (sort.is_signed())
        kind = CVC4::kind::BITVECTOR_SREM;
      else
        kind = CVC4::kind::BITVECTOR_UREM;
    }
    else if (sort.is_int())
    {
      kind = CVC4::kind::INTS_MODULUS;
    }
    else
    {
      return UNSUPPORT_ERROR;
    }

    return encode_binary(expr, kind, larg, rarg);
  }

  virtual Error __encode_binary_lss(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    CVC4::kind::Kind_t kind;

    if (larg.sort().is_bv())
    {
      if (larg.sort().is_signed())
        kind = CVC4::kind::BITVECTOR_SLT;
      else
        kind = CVC4::kind::BITVECTOR_ULT;
    }
    else
    {
      kind = CVC4::kind::LT;
    }

    return encode_binary(expr, kind, larg, rarg);
  }

  virtual Error __encode_binary_gtr(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    CVC4::kind::Kind_t kind;

    if (larg.sort().is_bv())
    {
      if (larg.sort().is_signed())
        kind = CVC4::kind::BITVECTOR_SGT;
      else
        kind = CVC4::kind::BITVECTOR_UGT;
    }
    else
    {
      kind = CVC4::kind::GT;
    }

    return encode_binary(expr, kind, larg, rarg);
  }

  virtual Error __encode_binary_neq(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    return encode_binary(expr,
      CVC4::kind::DISTINCT, larg, rarg);
  }

  virtual Error __encode_binary_leq(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    CVC4::kind::Kind_t kind;

    if (larg.sort().is_bv())
    {
      if (larg.sort().is_signed())
        kind = CVC4::kind::BITVECTOR_SLE;
      else
        kind = CVC4::kind::BITVECTOR_ULE;
    }
    else
    {
      kind = CVC4::kind::LEQ;
    }

    return encode_binary(expr, kind, larg, rarg);
  }

  virtual Error __encode_binary_geq(
    const Expr* const expr,
    const SharedExpr& larg,
    const SharedExpr& rarg) override
  {
    CVC4::kind::Kind_t kind;

    if (larg.sort().is_bv())
    {
      if (larg.sort().is_signed())
        kind = CVC4::kind::BITVECTOR_SGE;
      else
        kind = CVC4::kind::BITVECTOR_UGE;
    }
    else
    {
      kind = CVC4::kind::GEQ;
    }

    return encode_binary(expr, kind, larg, rarg);
  }

  virtual Error __encode_nary(
    const Expr* const expr,
    Opcode opcode,
    const SharedExprs& args) override
  {
    switch (opcode)
    {
    case NEQ:
    case LAND:
    case LOR:
      break;

    default:
      return UNSUPPORT_ERROR;
    }

    Error err;
    if (args.size() == 1)
    {
      err = args.front().encode(*this);
      if (err)
        return err;

      set_expr(m_expr);
      return OK;
    }

    std::vector<CVC4::Expr> exprs;
    exprs.reserve(args.size());
    for (SharedExprs::const_reference arg : args)
    {
      err = arg.encode(*this);
      if (err)
        return err;

      exprs.push_back(m_expr);
    }

    if (opcode == NEQ)
      set_expr(m_expr_manager.mkExpr(CVC4::kind::DISTINCT, exprs));
    else if (opcode == LAND)
      set_expr(m_expr_manager.mkExpr(CVC4::kind::AND, exprs));
    else if (opcode == LOR)
      set_expr(m_expr_manager.mkExpr(CVC4::kind::OR, exprs));

    return OK;
  }

  virtual Error __encode_bv_zero_extend(
    const Expr* const expr,
    const SharedExpr& bv,
    const unsigned ext) override
  {
    const Error err = bv.encode(*this);
    if (err) {
      return err;
    }

    set_expr(m_expr_manager.mkExpr(CVC4::kind::BITVECTOR_ZERO_EXTEND,
      m_expr_manager.mkConst(CVC4::BitVectorZeroExtend(ext)), m_expr));
    return OK;
  }

  virtual Error __encode_bv_sign_extend(
    const Expr* const expr,
    const SharedExpr& bv,
    const unsigned ext) override
  {
    const Error err = bv.encode(*this);
    if (err) {
      return err;
    }

    set_expr(m_expr_manager.mkExpr(CVC4::kind::BITVECTOR_SIGN_EXTEND,
      m_expr_manager.mkConst(CVC4::BitVectorSignExtend(ext)), m_expr));
    return OK;
  }

  virtual Error __encode_bv_extract(
    const Expr* const expr,
    const SharedExpr& bv,
    const unsigned high,
    const unsigned low) override
  {
    const Error err = bv.encode(*this);
    if (err) {
      return err;
    }

    set_expr(m_expr_manager.mkExpr(CVC4::kind::BITVECTOR_EXTRACT,
      m_expr_manager.mkConst(CVC4::BitVectorExtract(high, low)), m_expr));
    return OK;
  }

  virtual void __notify_delete(const Expr* const expr) override
  {
#ifdef ENABLE_HASH_CONS
    m_expr_map.erase(reinterpret_cast<uintptr_t>(expr));
#endif
  }

  virtual void __reset() override
  {
    // free memory first
    m_expr_map.clear();
    m_expr = CVC4::Expr();

    delete m_smt_engine;
    m_smt_engine = new CVC4::SmtEngine(&m_expr_manager);
  }

  virtual void __push() override
  {
    m_smt_engine->push();
  }

  virtual void __pop() override
  {
    m_smt_engine->pop();
  }

  virtual Error __unsafe_add(const SharedExpr& condition) override
  {
    const Error err = condition.encode(*this);
    if (err) {
      return err;
    }
    // unclear how to use assertFormula()'s return value
    m_smt_engine->assertFormula(m_expr);
    return OK;
  }

  virtual Error __add(const Bool& condition) override
  {
    return __unsafe_add(condition);
  }

  virtual CheckResult __check() override
  {
    switch (m_smt_engine->checkSat().isSat()) {
    case CVC4::Result::Sat::UNSAT:
      return unsat;
    case CVC4::Result::Sat::SAT:
      return sat;
    case CVC4::Result::Sat::SAT_UNKNOWN:
      return unknown;
    }
  }

  virtual std::pair<CheckResult, SharedExprs::size_type>
  __check_assumptions(
    const SharedExprs& assumptions,
    SharedExprs& unsat_core) override
  {
    return {unknown, 0};
  }

public:
  /// Auto configure CVC4
  CVC4Solver()
  : Solver(),
    m_expr_manager(),
    m_smt_engine(new CVC4::SmtEngine(&m_expr_manager)),
    m_expr(),
    m_expr_map()
  {
    m_smt_engine->setOption("incremental", true);
    m_smt_engine->setOption("output-language", "smt2");
  }

  CVC4Solver(const CVC4::Options& options)
  : Solver(),
    m_expr_manager(options),
    m_smt_engine(new CVC4::SmtEngine(&m_expr_manager)),
    m_expr(),
    m_expr_map()
  {
    m_smt_engine->setOption("incremental", true);
  }

  CVC4Solver(Logic logic)
  : Solver(logic),
    m_expr_manager(),
    m_smt_engine(new CVC4::SmtEngine(&m_expr_manager)),
    m_expr(),
    m_expr_map()
  {
    m_smt_engine->setOption("incremental", true);
    m_smt_engine->setOption("output-language", "smt2");
    m_smt_engine->setLogic(Logics::acronyms[logic]);
  }

  ~CVC4Solver()
  {
    delete m_smt_engine;
  }

  CVC4::ExprManager& expr_manager()
  {
    return m_expr_manager;
  }

  CVC4::SmtEngine& smt_engine()
  {
    return *m_smt_engine;
  }

  CVC4::Expr expr() const
  {
    // copy
    return m_expr;
  }
};

}

#endif
