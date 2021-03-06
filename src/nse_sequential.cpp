#include "nse_sequential.h"

namespace crv
{

constexpr char internal::Inputs::s_prefix[];
internal::Inputs::Counter internal::Inputs::s_counter = 0;

void Checker::add_assertion(Internal<bool>&& assertion)
{
  assert(!assertion.is_literal() || assertion.literal());
  m_assertions.push_back(Internal<bool>::term(std::move(assertion)));
}

void Checker::add_error(Internal<bool>&& error)
{
  smt::Bool guard;
  if (guards().empty())
    guard = smt::literal<smt::Bool>(true);
  else if (guards().size() == 1)
    guard = guards().front();
  else
    guard = smt::conjunction(guards());

  if (error.is_literal())
  {
    if (error.literal())
      m_errors.push_back(std::move(guard));
    else
      m_errors.push_back(smt::literal<smt::Bool>(false));
  }
  else
  {
    m_errors.push_back(std::move(guard) and Internal<bool>::term(std::move(error)));
  }
}

SequentialDfsChecker& sequential_dfs_checker()
{
  static SequentialDfsChecker s_sequential_dfs_checker;
  return s_sequential_dfs_checker;
}

// maintainer note: recall Dfs::find_next_path(). If the newly alternated
// flip F causes the conjunction of guards to be unsatisfiable, then this
// will be detected here as soon as another branch condition is conjoined.
// In the case there is no such other branch condition, progress is still
// guaranteed because F is frozen and therefore will be popped by Dfs.
bool SequentialDfsChecker::branch(const Internal<bool>& g, const bool direction_hint)
{
  m_stats.branch_cnt++;
  assert(m_stats.branch_cnt != 0);

  if (g.is_literal())
  {
    m_stats.branch_literal_cnt++;
    assert(m_stats.branch_literal_cnt != 0);

    return g.literal();
  }

  if (!m_is_feasible)
    // exactly like NO_BRANCH in branch(smt::Bool&&, const bool)
    return false;

  return branch(Internal<bool>::term(g), direction_hint);
}

bool SequentialDfsChecker::branch(Internal<bool>&& g, const bool direction_hint)
{
  m_stats.branch_cnt++;
  assert(m_stats.branch_cnt != 0);

  if (g.is_literal())
  {
    m_stats.branch_literal_cnt++;
    assert(m_stats.branch_literal_cnt != 0);

    return g.literal();
  }

  if (!m_is_feasible)
    // exactly like NO_BRANCH in branch(smt::Bool&&, const bool)
    return false;

  return branch(Internal<bool>::term(std::move(g)), direction_hint);
}

BacktrackDfsChecker& backtrack_dfs_checker()
{
  static BacktrackDfsChecker s_backtrack_dfs_checker;
  return s_backtrack_dfs_checker;
}

}
