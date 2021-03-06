#include "cka.h"
#include "gtest/gtest.h"
#include <cstddef>

using namespace cka;

TEST(CkaTest, EmptyPartialString)
{
  EXPECT_EQ(0, PartialString::empty().length());
}

TEST(CkaTest, InitPartialString)
{
  PartialString x{'x'};

  EXPECT_EQ('x', x.label_function().at(0));
  EXPECT_TRUE(x.strict_partial_order().empty());
  EXPECT_EQ(1, x.length());
  EXPECT_TRUE(x.incomparables().empty());
  EXPECT_EQ(1, x.minimals().size());
  EXPECT_EQ(0, x.minimals().front());
  EXPECT_EQ(1, x.maximals().size());
  EXPECT_EQ(0, x.maximals().front());
  EXPECT_EQ('x', x.min_label());
  EXPECT_EQ('x', x.max_label());
}

/// `PartialString::empty()::min_label()` is undefined
TEST(CkaTest, EmptyPartialAndExtremalLabel)
{
  PartialString x{'x'};
  PartialString y{(PartialString::empty() | x)};
  PartialString z{(x | PartialString::empty())};

  EXPECT_EQ('x', y.min_label());
  EXPECT_EQ('x', y.max_label());

  EXPECT_EQ('x', z.min_label());
  EXPECT_EQ('x', z.max_label());
}

TEST(CkaTest, PartialStringNumberOfEventsWithLabel)
{
  PartialString x{'x'};
  PartialString y{'y'};
  PartialString z{'z'};

  EXPECT_EQ(1, x.number_of_events_with_label('x'));
  EXPECT_EQ(0, x.number_of_events_with_label('y'));
  EXPECT_EQ(0, x.number_of_events_with_label('z'));

  EXPECT_EQ(0, y.number_of_events_with_label('x'));
  EXPECT_EQ(1, y.number_of_events_with_label('y'));
  EXPECT_EQ(0, y.number_of_events_with_label('z'));

  EXPECT_EQ(0, z.number_of_events_with_label('x'));
  EXPECT_EQ(0, z.number_of_events_with_label('y'));
  EXPECT_EQ(1, z.number_of_events_with_label('z'));

  PartialString u{(x | z)};
  {
    EXPECT_EQ(0, u.number_of_events_with_label('x' - '\1'));
    EXPECT_EQ(1, u.number_of_events_with_label('x'));
    EXPECT_EQ(0, u.number_of_events_with_label('y'));
    EXPECT_EQ(1, u.number_of_events_with_label('z'));
    EXPECT_EQ(0, u.number_of_events_with_label('z' + '\1'));
  }

  PartialString v{(u , y)};
  {
    EXPECT_EQ(0, v.number_of_events_with_label('x' - '\1'));
    EXPECT_EQ(1, v.number_of_events_with_label('x'));
    EXPECT_EQ(1, v.number_of_events_with_label('y'));
    EXPECT_EQ(1, v.number_of_events_with_label('z'));
    EXPECT_EQ(0, v.number_of_events_with_label('z' + '\1'));
  }

  PartialString v_prime{(v , y)};
  {
    EXPECT_EQ(0, v_prime.number_of_events_with_label('x' - '\1'));
    EXPECT_EQ(1, v_prime.number_of_events_with_label('x'));
    EXPECT_EQ(2, v_prime.number_of_events_with_label('y'));
    EXPECT_EQ(1, v_prime.number_of_events_with_label('z'));
    EXPECT_EQ(0, v_prime.number_of_events_with_label('z' + '\1'));
  }
}

TEST(CkaTest, PartialStringConcurrentComposition)
{
  PartialString x{'x'};
  PartialString y{'y'};
  PartialString z{(x | y)};

  EXPECT_EQ(2, z.length());
  EXPECT_EQ(0, z.strict_partial_order().size());

  EXPECT_EQ('x', z.label_function().at(0));
  EXPECT_EQ('y', z.label_function().at(1));
  EXPECT_EQ(1, z.incomparables().size());
  EXPECT_EQ(2, z.minimals().size());
  EXPECT_EQ(0, z.minimals().front());
  EXPECT_EQ(1, z.minimals().back());
  EXPECT_EQ(2, z.maximals().size());
  EXPECT_EQ(0, z.maximals().front());
  EXPECT_EQ(1, z.maximals().back());
  EXPECT_EQ('x', z.min_label());
  EXPECT_EQ('y', z.max_label());
}

TEST(CkaTest, SequentialPartialStringComposition)
{
  PartialString x{'x'};
  PartialString y{'y'};
  PartialString z{(x , y)};

  EXPECT_EQ(2, z.length());
  EXPECT_EQ(1, z.strict_partial_order().size());
  EXPECT_EQ(0, z.strict_partial_order().front().first);
  EXPECT_EQ(1, z.strict_partial_order().front().second);

  EXPECT_EQ('x', z.label_function().at(0));
  EXPECT_EQ('y', z.label_function().at(1));
  EXPECT_TRUE(z.incomparables().empty());
  EXPECT_EQ(1, z.minimals().size());
  EXPECT_EQ(0, z.minimals().front());
  EXPECT_EQ(1, z.maximals().size());
  EXPECT_EQ(1, z.maximals().front());
  EXPECT_EQ('x', z.min_label());
  EXPECT_EQ('y', z.max_label());
}

TEST(CkaTest, PartialStringMinimals)
{
  PartialString x{'x'};
  EXPECT_EQ(1, x.minimals().size());
}

TEST(CkaTest, PartialStringMaximals)
{
  PartialString x{'x'};
  EXPECT_EQ(1, x.maximals().size());
}

TEST(CkaTest, PartialStringComposition)
{
  PartialString x{'x'};
  PartialString y{'y'};
  PartialString z{'z'};
  PartialString p{'p'};

  {
    p = (x | y);

    EXPECT_EQ(2, p.length());
    EXPECT_TRUE(p.strict_partial_order().empty());
    EXPECT_EQ(1, p.incomparables().size());

    PartialString::EventPairs::const_iterator citer;
    citer = p.incomparables().cbegin();

    EXPECT_EQ(0, citer->first);
    EXPECT_EQ(1, citer->second);

    ++citer;
    EXPECT_EQ(p.incomparables().cend(), citer);
  }

  {
    p = (x , y);

    EXPECT_EQ(2, p.length());
    EXPECT_EQ(1, p.strict_partial_order().size());

    PartialString::EventPairs::const_iterator citer;
    citer = p.strict_partial_order().cbegin();

    EXPECT_EQ(0, citer->first);
    EXPECT_EQ(1, citer->second);

    ++citer;
    EXPECT_EQ(p.strict_partial_order().cend(), citer);
  }

  {
    p = ((x | y) , z);

    EXPECT_EQ(3, p.length());
    EXPECT_EQ(2, p.strict_partial_order().size());

    PartialString::EventPairs::const_iterator citer;
    citer = p.strict_partial_order().cbegin();

    EXPECT_EQ(0, citer->first);
    EXPECT_EQ(2, citer->second);

    ++citer;
    EXPECT_EQ(1, citer->first);
    EXPECT_EQ(2, citer->second);

    ++citer;
    EXPECT_EQ(p.strict_partial_order().cend(), citer);
  }

  {
    p = (x , (y | z));

    EXPECT_EQ(3, p.length());
    EXPECT_EQ(2, p.strict_partial_order().size());

    PartialString::EventPairs::const_iterator citer;
    citer = p.strict_partial_order().cbegin();

    EXPECT_EQ(0, citer->first);
    EXPECT_EQ(1, citer->second);

    ++citer;
    EXPECT_EQ(0, citer->first);
    EXPECT_EQ(2, citer->second);

    ++citer;
    EXPECT_EQ(p.strict_partial_order().cend(), citer);
  }

  {
    p = ((x , y) | z);

    EXPECT_EQ(3, p.length());
    EXPECT_EQ(1, p.strict_partial_order().size());
    EXPECT_EQ(0, p.strict_partial_order().front().first);
    EXPECT_EQ(1, p.strict_partial_order().front().second);
  }

  {
    p = (x | (y , z));

    EXPECT_EQ(3, p.length());
    EXPECT_EQ(1, p.strict_partial_order().size());
    EXPECT_EQ(1, p.strict_partial_order().front().first);
    EXPECT_EQ(2, p.strict_partial_order().front().second);
  }

  {
    p = ((x , y) , z);

    EXPECT_EQ(3, p.length());
    EXPECT_EQ(2, p.strict_partial_order().size());

    PartialString::EventPairs::const_iterator citer;
    citer = p.strict_partial_order().cbegin();

    EXPECT_EQ(0, citer->first);
    EXPECT_EQ(1, citer->second);

    ++citer;
    EXPECT_EQ(1, citer->first);
    EXPECT_EQ(2, citer->second);

    ++citer;
    EXPECT_EQ(p.strict_partial_order().cend(), citer);
  }

  {
    p = (x , (y , z));

    EXPECT_EQ(3, p.length());
    EXPECT_EQ(2, p.strict_partial_order().size());

    PartialString::EventPairs::const_iterator citer;
    citer = p.strict_partial_order().cbegin();

    EXPECT_EQ(1, citer->first);
    EXPECT_EQ(2, citer->second);

    ++citer;
    EXPECT_EQ(0, citer->first);
    EXPECT_EQ(1, citer->second);

    ++citer;
    EXPECT_EQ(p.strict_partial_order().cend(), citer);
  }
}

TEST(CkaTest, PartialStringAssociativity)
{
  PartialString x{'x'};
  PartialString y{'y'};
  PartialString z{'z'};

  EXPECT_TRUE(((x | y) | z) <= (x | (y | z)));
  EXPECT_TRUE((x | (y | z)) <= ((x | y) | z));

  EXPECT_TRUE(((x , y) , z) <= (x , (y , z)));
  EXPECT_TRUE((x , (y , z)) <= ((x , y) , z));
}

TEST(CkaTest, PartialStringConcurrentCommutativity)
{
  PartialString x{'x'};
  PartialString y{'y'};

  EXPECT_TRUE((x | y) <= (y | x));
  EXPECT_TRUE((y | x) <= (x | y));
}

TEST(CkaTest, PartialStringExchangeLaw)
{
  PartialString u{'u'};
  PartialString v{'v'};
  PartialString x{'x'};
  PartialString y{'y'};

  PartialString p{((u | v) , (x | y))};
  PartialString q{((u , x) | (v , y))};

  EXPECT_TRUE(p <= q);
  EXPECT_FALSE(q <= p);
}

TEST(CkaTest, PartialStringRefineConcurrentAndSequential)
{
  PartialString x{'x'};
  PartialString y{'y'};
  PartialString z{'z'};

  EXPECT_FALSE(x <= (x , x));
  EXPECT_FALSE(x <= (x | x));

  EXPECT_FALSE((x | y) <= (x , y));
  EXPECT_TRUE((x , y) <= (x | y));

  EXPECT_FALSE((x | y | z) <= ((x , y) | z));
  EXPECT_TRUE(((x , y) | z) <= (x | y | z));

  EXPECT_FALSE(((x | y) , z) <= (x , y , z));
  EXPECT_TRUE((x , y , z) <= ((x | y) , z));
}

TEST(CkaTest, PartialStringComposite)
{
  PartialString x{'x'};
  PartialString y{'y'};
  PartialString z{'z'};

  PartialString p{(x | y)};
  PartialString q{(x , y)};

  EXPECT_TRUE((p , q) <= (p | q));
  EXPECT_TRUE((p , q) <= (q | p));
  EXPECT_TRUE((q , p) <= (p | q));
  EXPECT_TRUE((q , p) <= (q | p));

  EXPECT_FALSE((p | q) <= (p , q));
  EXPECT_FALSE((p | q) <= (q , p));
  EXPECT_FALSE((q | p) <= (p , q));
  EXPECT_FALSE((q | p) <= (q , p));
}

typedef std::vector<std::pair<PartialString, PartialString>> PartialStringTests;

TEST(CkaTest, PartialStringTransitiveReduction)
{
  PartialString x{'x'};
  PartialString y{'y'};
  PartialString z{'z'};

  EXPECT_TRUE((x , y , z) <= ((x , z) | y));
  EXPECT_TRUE((x , z , y) <= ((x , z) | y));
  EXPECT_TRUE(((x | y) , z) <= ((x , z) | y));

  EXPECT_FALSE((y , z , x) <= ((x , y) | z));
}

TEST(CkaTest, PartialStringYes)
{
  PartialString u{'u'};
  PartialString v{'v'};
  PartialString x{'x'};
  PartialString y{'y'};

  PartialStringTests tests = {
    {{ u }, { u }},
    {{ v }, { v }},
    {{ (v | v) }, { (v | v) }},
    {{ (v , v) | x }, { (v | v) | x }},
    {{ ((v , v) , x) }, { ((v | v) , x) }},
    {{ ((v , v) , x) }, { ((v | v) | x) }},
  };

  for (std::size_t n = 0; n < tests.size(); ++n)
    EXPECT_TRUE(tests[n].first <= tests[n].second);
}

TEST(CkaTest, PartialStringNo)
{
  PartialString u{'u'};
  PartialString v{'v'};
  PartialString x{'x'};
  PartialString y{'y'};

  PartialStringTests tests = {
    {{ (v | v) }, { (v , v) }},
    {{ ((v | v) | x) }, { ((v , v) | x) }},
    {{ ((v | v) , x) }, { (x | (v , v)) }},
  };

  for (std::size_t n = 0; n < tests.size(); ++n)
    EXPECT_FALSE(tests[n].first <= tests[n].second);
}

TEST(CkaTest, IncomparableMinimals)
{
  PartialString u{'u'};
  PartialString v{'v'};
  PartialString x{'x'};
  PartialString y{'y'};

  PartialString q{((u , x) | (v , y))};

  PartialString a{((x | q) , (x | y))};
  PartialString b{((x , x) | (q , y))};

  EXPECT_TRUE(a <= b);

  // "v ; x" in "a" but not in "b"
  EXPECT_FALSE(b <= a);
}

TEST(CkaTest, IncomparableMaximals)
{
  PartialString p{'p'};
  PartialString q{'q'};
  PartialString x{'x'};
  PartialString y{'y'};
  PartialString z{'z'};

  // "p | q" on LHS but "p ; q" on RHS
  EXPECT_FALSE(((x, p) | (y , z , q)) <= (x | (y , z , p , q)));
}

TEST(CkaTest, IncomparableSymmetric)
{
  PartialString x{'x'};
  PartialString y{'y'};
  PartialString p{((x | x) , y)};
  PartialString q{(y | (x , x))};

  EXPECT_FALSE(p <= q);
  EXPECT_FALSE(q <= p);
}

TEST(CkaTest, Equality)
{
  PartialString a{'x'};
  PartialString b{'x'};
  EXPECT_TRUE(a == b);

  PartialString c{(a , b)};
  PartialString d{(a , b)};
  EXPECT_TRUE(c == d);

  PartialString e{(a | b)};
  PartialString f{(a | b)};
  EXPECT_TRUE(e == f);
}

TEST(CkaTest, UintPower)
{
  EXPECT_EQ(5, internal::uint_pow(5, 1));
  EXPECT_EQ(25, internal::uint_pow(5, 2));
  EXPECT_EQ(125, internal::uint_pow(5, 3));
  EXPECT_EQ(625, internal::uint_pow(5, 4));
  EXPECT_EQ(3125, internal::uint_pow(5, 5));
}

TEST(CkaTest, ConvertPartialStringToProgram)
{
  PartialString x{'x'};
  PartialString y{'y'};
  PartialString p{((x | x) , y)};
  Program P{p};

  EXPECT_EQ(1, P.size());
  EXPECT_TRUE(p == P.partial_strings().front());
}

TEST(CkaTest, MovesPartialStringIntoProgram)
{
  PartialString x{'x'};
  PartialString y{'y'};
  Program P{((x | x) , y)};

  EXPECT_EQ(1, P.size());
  EXPECT_TRUE(((x | x) , y) == P.partial_strings().front());
}

TEST(CkaTest, ZeroProgram)
{
  EXPECT_EQ(0, Program::zero().size());
}

TEST(CkaTest, InitLazyProgram)
{
  Program X{'x'};
  Program Y{'y'};
  Program Z{'z'};

  EXPECT_EQ(1, X.size());
  EXPECT_EQ(1, Y.size());
  EXPECT_EQ(1, Z.size());

  PartialString x{'x'};
  PartialString y{'y'};
  PartialString z{'z'};

  // sanity check on `X`
  EXPECT_TRUE(x == X.partial_strings().front());

  Program P{X + (Y , Z)};
  EXPECT_EQ(2, P.size());

  internal::LazyProgram<','> Q{P};
  EXPECT_EQ(2, Q.size());

  internal::PartialStringIterator<','> iter{Q.partial_string_iterator()};
  EXPECT_TRUE(iter.has_next_partial_string());
  {
    PartialString p{iter.next_partial_string()};
    EXPECT_TRUE(x == p);
  }

  EXPECT_TRUE(iter.has_next_partial_string());
  {
    PartialString p{iter.next_partial_string()};
    EXPECT_TRUE(p == (y , z));
  }

  EXPECT_FALSE(iter.has_next_partial_string());

  // modify reference in `Q`
  P = P + (X | Z);
  EXPECT_EQ(3, P.size());
  EXPECT_EQ(3, Q.size());
}

TEST(CkaTest, BinaryLazyProgram)
{
  Program X{'x'};
  Program Y{'y'};
  Program Z{'z'};

  EXPECT_EQ(1, X.size());
  EXPECT_EQ(1, Y.size());
  EXPECT_EQ(1, Z.size());

  PartialString x{'x'};
  PartialString y{'y'};
  PartialString z{'z'};

  Program P{X + (Y , Z)};
  EXPECT_EQ(2, P.size());

  internal::LazyProgram<','> Q{P};
  Q.extend();

  EXPECT_EQ(4, Q.size());

  internal::PartialStringIterator<','> iter{Q.partial_string_iterator()};

  // 1
  EXPECT_TRUE(iter.has_next_partial_string());
  {
    PartialString p{iter.next_partial_string()};
    EXPECT_TRUE((x , x) == p);
  }

  // 2
  EXPECT_TRUE(iter.has_next_partial_string());
  {
    PartialString p{iter.next_partial_string()};
    EXPECT_TRUE(((y , z) , x) == p);
  }

  // 3
  EXPECT_TRUE(iter.has_next_partial_string());
  {
    PartialString p{iter.next_partial_string()};
    EXPECT_TRUE((x , (y , z)) == p);
  }

  // 4
  EXPECT_TRUE(iter.has_next_partial_string());
  {
    PartialString p{iter.next_partial_string()};
    EXPECT_TRUE(((y , z) , (y , z)) == p);
  }

  EXPECT_FALSE(iter.has_next_partial_string());
}

TEST(CkaTest, LazyProgramThree)
{
  Program X{'x'};
  Program Y{'y'};
  Program Z{'z'};

  EXPECT_EQ(1, X.size());
  EXPECT_EQ(1, Y.size());
  EXPECT_EQ(1, Z.size());

  PartialString x{'x'};
  PartialString y{'y'};
  PartialString z{'z'};

  Program P{X + (Y , Z)};
  EXPECT_EQ(2, P.size());

  internal::LazyProgram<','> Q{P};
  Q.extend();
  Q.extend();

  EXPECT_EQ(8, Q.size());

  internal::PartialStringIterator<','> iter{Q.partial_string_iterator()};

  // 1
  EXPECT_TRUE(iter.has_next_partial_string());
  {
    PartialString p{iter.next_partial_string()};
    EXPECT_TRUE(((x , x) , x) == p);
  }

  // 2
  EXPECT_TRUE(iter.has_next_partial_string());
  {
    PartialString p{iter.next_partial_string()};
    EXPECT_TRUE((((y , z) , x) , x) == p);
  }

  // 3
  EXPECT_TRUE(iter.has_next_partial_string());
  {
    PartialString p{iter.next_partial_string()};
    EXPECT_TRUE(((x , (y , z)) , x) == p);
  }

  // 4
  EXPECT_TRUE(iter.has_next_partial_string());
  {
    PartialString p{iter.next_partial_string()};
    EXPECT_TRUE((((y , z) , (y , z)) , x) == p);
  }

  // 5
  EXPECT_TRUE(iter.has_next_partial_string());
  {
    PartialString p{iter.next_partial_string()};
    EXPECT_TRUE(((x , x) , (y , z)) == p);
  }

  // 6
  EXPECT_TRUE(iter.has_next_partial_string());
  {
    PartialString p{iter.next_partial_string()};
    EXPECT_TRUE((((y , z) , x) , (y , z)) == p);
  }

  // 7
  EXPECT_TRUE(iter.has_next_partial_string());
  {
    PartialString p{iter.next_partial_string()};
    EXPECT_TRUE(((x , (y , z)) , (y , z)) == p);
  }

  // 8
  EXPECT_TRUE(iter.has_next_partial_string());
  {
    PartialString p{iter.next_partial_string()};
    EXPECT_TRUE((((y , z) , (y , z)) , (y , z)) == p);
  }

  EXPECT_FALSE(iter.has_next_partial_string());
}

TEST(CkaTest, LazyProgramSymbolic)
{
  Program X{'x'};
  Program Y{'y'};
  Program Z{'z'};

  Program P{X + (Y | Z) + (Z , X)};
  Program K{P};

  // Eager "P , P , P"
  K = (K , P);
  K = (K , P);

  // size of `P` (i.e. 3) cubed
  EXPECT_EQ(27, K.size());

  // Lazy "P , P , P"
  internal::LazyProgram<','> Q{P};
  Q.extend();
  Q.extend();

  EXPECT_EQ(K.size(), Q.size());

  Program R{Program::zero()};
  internal::PartialStringIterator<','> iter{Q.partial_string_iterator()};

  // `R` is the union of all partial strings in `Q`
  while (iter.has_next_partial_string())
    R = R + Program(iter.next_partial_string());

  EXPECT_EQ(K.size(), R.size());
  EXPECT_TRUE(K <= R);
  EXPECT_TRUE(R <= K);
}

TEST(CkaTest, ProgramAssociativity)
{
  Program X{'x'};
  Program Y{'y'};
  Program Z{'z'};

  EXPECT_TRUE(((X | Y) | Z) <= (X | (Y | Z)));
  EXPECT_TRUE((X | (Y | Z)) <= ((X | Y) | Z));

  EXPECT_TRUE(((X , Y) , Z) <= (X , (Y , Z)));
  EXPECT_TRUE((X , (Y , Z)) <= ((X , Y) , Z));

  EXPECT_TRUE(((X + Y) + Z) <= (X + (Y + Z)));
  EXPECT_TRUE((X + (Y + Z)) <= ((X + Y) + Z));
}

TEST(CkaTest, ProgramNondeterministicChoiceIdempotence)
{
  Program X{'x'};

  EXPECT_TRUE((X + X) <= X);
  EXPECT_TRUE(X <= (X + X));
}

TEST(CkaTest, ProgramNondeterministicChoiceCommutativity)
{
  Program X{'x'};
  Program Y{'y'};

  EXPECT_TRUE((X + Y) <= (Y + X));
  EXPECT_TRUE((Y + X) <= (X + Y));
}

TEST(CkaTest, ProgramNondeterministicChoiceIdentity)
{
  Program X{'x'};

  EXPECT_TRUE((X + Program::zero()) <= X);
  EXPECT_TRUE(X <= (X + Program::zero()));
}

TEST(CkaTest, ProgramSequentialAnnihilator)
{
  Program X{'x'};

  EXPECT_TRUE((X , Program::zero()) <= Program::zero());
  EXPECT_TRUE(Program::zero() <= (X , Program::zero()));
}

TEST(CkaTest, ProgramConcurrentCommutativity)
{
  Program X{'x'};
  Program Y{'y'};

  EXPECT_TRUE((X | Y) <= (Y | X));
  EXPECT_TRUE((Y | X) <= (X | Y));
}

TEST(CkaTest, ProgramExchangeLaw)
{
  Program U{'u'};
  Program V{'v'};
  Program X{'x'};
  Program Y{'y'};

  Program P{((U | V) , (X | Y))};
  Program Q{((U , X) | (V , Y))};

  EXPECT_TRUE(P <= Q);
  EXPECT_FALSE(Q <= P);

  // composites to test more throughly the encoder
  Program S{(X | Y)};
  Program T{(X , Y)};

  Program A{((P | Q) , (S | T))};
  Program B{((P , S) | (Q , T))};

  EXPECT_TRUE(A <= B);
  EXPECT_FALSE(B <= A);
}

TEST(CkaTest, WeakSequentialConsistency)
{
  Program X{'x'};
  Program Y{'y'};

  EXPECT_TRUE(((X , Y) + (Y , X)) <= (X | Y));
  EXPECT_FALSE((X | Y) <= ((X , Y) + (Y , X)));
}

TEST(CkaTest, LfpProgram)
{
  Program X{'x'};

  LfpProgram<'|'> P{lfp<'|'>(X)};
  LfpProgram<','> Q{lfp<','>(X)};
}

TEST(CkaTest, Reductions)
{
  Program X{'x'};
  Program Y{'y'};

  LfpProgram<','> lfpX{lfp<','>(X)};
  LfpProgram<','> lfpXX{lfp<','>((X , X))};
  LfpProgram<','> lfpXXX{lfp<','>((X , X , X))};
  LfpProgram<','> lfpXXXX{lfp<','>((X , X , X , X))};

  EXPECT_TRUE(lfpXX <= lfpX);
  EXPECT_TRUE(lfpXXX <= lfpX);
  EXPECT_TRUE(lfpXXXX <= lfpXX);
  EXPECT_FALSE(lfpX <= lfpXX);
  EXPECT_FALSE(lfpX <= lfpXXX);
  EXPECT_FALSE(lfpXX <= lfpXXX);
  EXPECT_FALSE(lfpX <= lfpXXX);
  EXPECT_FALSE(lfpX <= lfpXXXX);
  EXPECT_FALSE(lfpXX <= lfpXXXX);

  // `lfpXXX` contains "x ; x ; x" but `lfpXX`
  // contains only even number of "x".
  EXPECT_FALSE(lfpXXX <= lfpXX);

  // `lfpXX` contains "x ; x" but `lfpXXX` contains
  // "x" only in multiple of threes.
  EXPECT_FALSE(lfpXX <= lfpXXX);

  Program U{'u'};
  Program V{'v'};

  Program P{((U | V) , (X | Y))};
  Program Q{((U , X) | (V , Y))};

  EXPECT_TRUE(lfp<','>(P) <= lfp<','>(Q));
  EXPECT_TRUE(lfp<'|'>(P) <= lfp<'|'>(Q));
  EXPECT_FALSE(lfp<','>((P , P)) <= lfp<','>((Q , Q  , Q , Q)));
  EXPECT_TRUE(lfp<','>((P , P , P , P)) <= lfp<','>((Q , Q)));
}

TEST(CkaTest, ProgramSequentialLeftDistributivity)
{
  Program X{'x'};
  Program Y{'y'};
  Program Z{'z'};

  Program P{((X + Y) , Z)};
  Program Q{((X , Z) + (Y , Z))};

  EXPECT_TRUE(P <= Q);
  EXPECT_TRUE(Q <= P);
}

TEST(CkaTest, ProgramSequentialRightDistributivity)
{
  Program X{'x'};
  Program Y{'y'};
  Program Z{'z'};

  Program P{(Z , (X + Y))};
  Program Q{((Z , X) + (Z , Y))};

  EXPECT_TRUE(P <= Q);
  EXPECT_TRUE(Q <= P);
}

TEST(CkaTest, ProgramConcurrentLeftDistributivity)
{
  Program X{'x'};
  Program Y{'y'};
  Program Z{'z'};

  Program P{((X + Y) | Z)};
  Program Q{((X | Z) + (Y | Z))};
  Program R{((Z | X) + (Z | Y))};

  EXPECT_TRUE(P <= Q);
  EXPECT_TRUE(Q <= P);

  EXPECT_TRUE(P <= R);
  EXPECT_TRUE(R <= P);
}

TEST(CkaTest, ProgramConcurrentRightDistributivity)
{
  Program X{'x'};
  Program Y{'y'};
  Program Z{'z'};

  Program P{(Z | (X + Y))};
  Program Q{((X | Z) + (Y | Z))};
  Program R{((Z | X) + (Z | Y))};

  EXPECT_TRUE(P <= Q);
  EXPECT_TRUE(Q <= P);

  EXPECT_TRUE(P <= R);
  EXPECT_TRUE(R <= P);
}

TEST(CkaTest, ProgramSequentialNondistributivity)
{
  Program X{'x'};
  Program Y{'y'};
  Program Z{'z'};

  Program P{((X , Y) + Z)};
  Program Q{((X + Z) , (Y + Z))};

  // `P` contains "z" but `Q` contains only partial strings
  // whose length is at most two.
  EXPECT_FALSE(P <= Q);

  // `Q` contains "x , z" (among others) but `P` does not,
  // even partial strings such as "x | z".
  EXPECT_FALSE(Q <= P);
}

TEST(CkaTest, ProgramConcurentNondistributivity)
{
  Program X{'x'};
  Program Y{'y'};
  Program Z{'z'};

  Program P{((X | Y) + Z)};
  Program Q{((X + Z) | (Y + Z))};

  // `P` contains "z" but `Q` contains only partial strings
  // whose length is at most two.
  EXPECT_FALSE(P <= Q);

  // `Q` contains "x | z" (among others) but `P` does not.
  EXPECT_FALSE(Q <= P);
}

/*
 * Consider the following program consistent of two threads T1 and T2:
 * "[a] := 0; [b] := 0; (T1 || T2)" where 'a' and 'b' are shared memory
 * addresses, T1 = "[a] := 1; r1 := [b]" and T2 = "[b] := 1; r2 := [a]".
 *
 * Let P0 = "[a] := 0",
 *     P1 = "[b] := 0",
 *     P2 = "[a] := 1",
 *     P3 = "r1 := [b]",
 *     P4 = "[b] := 1",
 *     P5 = "r2 := [a]".
 *
 * In this test we ignore that "P2" is sequenced-before "P3",
 * and "P4" is sequenced-before "P5". In that case, we aim to
 * explicitly consider the potential executions of the system.
 */
TEST(CkaTest, HandwrittenProgramIgnorePerThreadOrdering)
{
  Program P0{'\0'};
  Program P1{'\1'};
  Program P2{'\2'};
  Program P3{'\3'};
  Program P4{'\4'};
  Program P5{'\5'};

  Program A{(P0 , P5 , P2) + (P0 , P2 , P5) + (P2 , P5 , P0) + (P2 , P0 , P5)};
  Program B{(P1 , P3 , P4) + (P1 , P4 , P3) + (P4 , P3 , P1) + (P4 , P1 , P3)};

  Program R0{((P0 , P1 ) , ((P2 , P3) | (P4 , P5)))};
  Program R1{(P0 | P1 | ((P2 , P3) | (P4 , P5)))};
  Program R2{(P0 | P1 | P2 | P3 | P4 | P5)};

  // For example, "P2" and "P3" are ordered in "R0" and "R1", but not in "A | B".
  EXPECT_FALSE((A | B) <= R0);
  EXPECT_FALSE((A | B) <= R1);

  EXPECT_TRUE((A | B) <= R2);

  // Since "P0" and "P2", as well as "P1" and "P4" can commute,
  // let's simplify "A" and "B (note that this would violate
  // the write serialization axiom which stipulates that all
  // writes to the same memory address are totally ordered).
  Program C{(P0 , P5 , P2) + ((P0 | P2) , P5) + (P2 , P5 , P0)};
  Program D{(P1 , P3 , P4) + ((P1 | P4) , P3) + (P4 , P3 , P1)};

  // For example, "P2" and "P3" are ordered in "R0" and "R1", but not in "C | D".
  EXPECT_FALSE((C | D) <= R0);
  EXPECT_FALSE((C | D) <= R1);

  EXPECT_TRUE((C | D) <= R2);
}

/*
 * Consider the following program consistent of two threads T1 and T2:
 * "[a] := 0; [b] := 0; (T1 || T2)" where 'a' and 'b' are shared memory
 * addresses, T1 = "[a] := 1; r1 := [b]" and T2 = "[b] := 1; r2 := [a]".
 *
 * Let P0 = "[a] := 0",
 *     P1 = "[b] := 0",
 *     P2 = "[a] := 1",
 *     P3 = "r1 := [b]",
 *     P4 = "[b] := 1",
 *     P5 = "r2 := [a]".
 *
 * Sequential consistency can be achieved by requiring that
 * "P2" is sequenced-before "P3", and "P4" is sequenced-before "P5".
 */
TEST(CkaTest, HandwrittenProgramSequentialConsistency)
{
  Program P0{'\0'};
  Program P1{'\1'};
  Program P2{'\2'};
  Program P3{'\3'};
  Program P4{'\4'};
  Program P5{'\5'};

  Program A{((P2 | P4) , (P3 | P5))};
  Program B{(P4 , P5 , P2 , P3)};
  Program C{(P2 , P3 , P4 , P5)};

  Program P{((P0 | P1) , (A + B + C))};
  Program Q{((P0 , P1) , (A + B + C))};

  Program R0{((P0 , P1 ) , ((P2 , P3) | (P4 , P5)))};
  Program R1{(P0 | P1 | ((P2 , P3) | (P4 , P5)))};
  Program R2{(P0 | P1 | P2 | P3 | P4 | P5)};

  // Both "P0" and "P1" are unsequenced in "P", but sequenced in "R0"
  EXPECT_FALSE(P <= R0);

  EXPECT_TRUE(P <= R1);
  EXPECT_TRUE(P <= R2);

  EXPECT_TRUE(Q <= R0);
  EXPECT_TRUE(Q <= R1);
  EXPECT_TRUE(Q <= R2);
}

TEST(CkaTest, ForAllThereExists)
{
  Program P{'\1'};
  Program Q{'\2'};

  EXPECT_TRUE(P <= (P + Q));
}

// Since we only keep the transitive reduction of a strict partial
// ordering, it would be wrong to optimize refinement checks based
// on the number of partial order constraints.
TEST(CkaTest, NumberOfStrictPartialOrderConstraints)
{
  PartialString u{'\0'};
  PartialString v{'\1'};
  PartialString x{'\2'};
  PartialString y{'\3'};

  // size of strict partial ordering on the LHS: 3
  // size of strict partial ordering on the RHS: 4
  EXPECT_TRUE((v , y , x , u) <= (v , (x | y) , u));
}

TEST(CkaTest, LfpWithNondeterministicChoice)
{
  Program P{'\1'};
  Program Q{'\2'};

  EXPECT_TRUE(lfp<','>(P) <= lfp<','>(P + Q));
  EXPECT_TRUE(lfp<','>(Q) <= lfp<','>(P + Q));
}

TEST(CkaTest, ResetPartialStringIterator)
{
  Program A{'\0'};
  Program B{'\1'};
  Program C{'\2'};
  Program D{'\3'};
  Program E{'\6'};
  Program F{'\7'};

  Program G{(((E | F), (C | D)) + (E, B, F, C) + (F, A, E, D) +
              (E, (C | B), F) + (A, (E | F), D) + ((A | B), (E | F)))};

  Program H{(F, E, C, D)};
  Program I{(E, B, C, F)};
  Program J{(F, E, C, D)};

  EXPECT_TRUE(lfp<','>(H + I + J) <= lfp<','>(G));
}

TEST(CkaTest, Statistics)
{
  Program P{'\0'};
  Program Q{'\1'};

  Refinement r;

  EXPECT_EQ(0, r.number_of_checks());
  EXPECT_EQ(0, r.number_of_shortcuts());
  EXPECT_EQ(0, r.number_of_solver_calls());

  EXPECT_FALSE(r.check(P, (P , Q)));

  EXPECT_EQ(1, r.number_of_checks());
  EXPECT_EQ(1, r.number_of_shortcuts());
  EXPECT_EQ(0, r.number_of_solver_calls());

  EXPECT_TRUE(r.check((P | Q), (Q | P)));

  EXPECT_EQ(2, r.number_of_checks());
  EXPECT_EQ(1, r.number_of_shortcuts());
  EXPECT_EQ(1, r.number_of_solver_calls());
}

/*
 * For illustrative purposes, we also used Seed to randomly
 * generate tests according to the context-free grammar shown
 * in "test/CKA-benchmark.xml".
 *
 * This tool can be downloaded from the following URL:
 *
 *   http://igm.univ-mlv.fr/~nicaud/seed/
 *
 * To generate a random CKA expression of length `N`, run
 * the following command:
 *
 *   java -jar seed.jar test/CKA-benchmark.xml N -text
 *
 * The accompanying tool paper is published in ICST'2011:
 *
 *   Heam, P.C.; Nicaud, C., "Seed: An Easy-to-Use Random Generator
 *   of Recursive Data Structures for Testing," Software Testing,
 *   2011 IEEE Fourth International Conference on Verification and
 *   Validation (ICST), pp.60,69, 21-25 March 2011
 *
 * Benchmarks can be generated by running "generate_cka_benchmarks.sh".
 */

TEST(CkaTest, Random)
{
  Program A{'a'};
  Program B{'b'};
  Program C{'c'};
  Program D{'d'};
  Program E{'e'};
  Program F{'f'};
  Program G{'g'};
  Program H{'h'};
  Program I{'i'};
  Program J{'j'};
  Program K{'k'};
  Program L{'l'};
  Program M{'m'};
  Program N{'n'};
  Program O{'o'};
  Program P{'p'};
  Program Q{'q'};
  Program R{'r'};
  Program S{'s'};
  Program T{'t'};
  Program U{'u'};
  Program V{'v'};
  Program W{'w'};
  Program X{'x'};
  Program Y{'y'};
  Program Z{'z'};

  EXPECT_FALSE((lfp<'|'>((U | Z)) <= lfp<'|'>(((E | (Q + W)) | (E | (W , (I + (M + X))))))));
  EXPECT_FALSE((lfp<','>((L , (G | ((A , D) | (P | ((L , J) + (Y | Q))))))) <= lfp<','>(W)));
  EXPECT_FALSE((lfp<','>(Q) <= lfp<','>((Z , ((H , (I , A)) | (D | (D + (V + (S | U)))))))));
  EXPECT_FALSE((lfp<'|'>((Y + (R + (J , (A , I))))) <= lfp<'|'>(((Z , P) | (J , (T + Y))))));
  EXPECT_FALSE((lfp<'|'>((W + (F | T))) <= lfp<'|'>((W , (C + (F , (G | (X + (Z | O)))))))));

  EXPECT_FALSE((lfp<'|'>(L) <= lfp<'|'>(((Z , (N | V)) | (Q | (J , (M + (S + (A | ((W | M) , (E , (N , (Y + L)))))))))))));
  EXPECT_FALSE((lfp<','>((A + ((C , (C | S)) | ((Q , (Z + (U , B))) + (A | ((Y | V) , (O , Z))))))) <= lfp<','>((S | R))));
  EXPECT_FALSE((lfp<'|'>(((G | A) + (W , (B | (G + (D + (C + ((W | (L + J)) + (V , (K + (D | S))))))))))) <= lfp<'|'>(H)));
  EXPECT_FALSE((lfp<','>(((O + V) + (G , H))) <= lfp<','>((A | (W , ((V | N) , ((B + C) , (J , (W , (F + (S | Q)))))))))));
  EXPECT_FALSE((lfp<'|'>(((D , (G , D)) , (V + ((Z | (W | O)) , (Q | (H + (Z , P))))))) <= lfp<'|'>((T + (P , (C | O))))));
}

TEST(CkaTest, MemoryLabels)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  EXPECT_EQ(0U, memory::none_store_label(a));
  EXPECT_EQ(1U, memory::none_load_label(a));
  EXPECT_EQ(2U, memory::release_store_label(a));
  EXPECT_EQ(3U, memory::acquire_load_label(a));
  EXPECT_EQ(15U, (memory::assume_acquire_eq_label(a, '\5')) & 0xFU);
  EXPECT_EQ(7U, (memory::assume_acquire_neq_label(a, '\5') & 0xFU));

  EXPECT_TRUE(std::numeric_limits<memory::Byte>::max() < memory::none_store_label(b));
  EXPECT_TRUE(std::numeric_limits<memory::Byte>::max() < memory::none_store_label(b, '\1'));
  EXPECT_TRUE(std::numeric_limits<memory::Byte>::max() < memory::release_store_label(b));
  EXPECT_TRUE(std::numeric_limits<memory::Byte>::max() < memory::release_store_label(b, '\1'));
  EXPECT_TRUE(std::numeric_limits<memory::Byte>::max() < memory::none_load_label(b));
  EXPECT_TRUE(std::numeric_limits<memory::Byte>::max() < memory::acquire_load_label(b));
  EXPECT_TRUE(std::numeric_limits<memory::Byte>::max() < memory::assume_acquire_eq_label(b, '\5'));
  EXPECT_TRUE(std::numeric_limits<memory::Byte>::max() < memory::assume_acquire_neq_label(b, '\5'));
}

TEST(CkaTest, MemoryNoneStoreAddressMonotonicity)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;

  EXPECT_TRUE(memory::none_store_label(a, '\0') < memory::none_store_label(b));
  EXPECT_TRUE(memory::none_store_label(a, '\1') < memory::none_store_label(b));
  EXPECT_TRUE(memory::none_store_label(a, '\2') < memory::none_store_label(b));
  EXPECT_TRUE(memory::none_store_label(a, '\3') < memory::none_store_label(b));

  EXPECT_TRUE(memory::none_store_label(b, '\0') < memory::none_store_label(c));
  EXPECT_TRUE(memory::none_store_label(b, '\1') < memory::none_store_label(c));
  EXPECT_TRUE(memory::none_store_label(b, '\2') < memory::none_store_label(c));
  EXPECT_TRUE(memory::none_store_label(b, '\3') < memory::none_store_label(c));
}

TEST(CkaTest, MemoryReleaseStoreAddressMonotonicity)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;

  EXPECT_TRUE(memory::release_store_label(a, '\0') < memory::release_store_label(b));
  EXPECT_TRUE(memory::release_store_label(a, '\1') < memory::release_store_label(b));
  EXPECT_TRUE(memory::release_store_label(a, '\2') < memory::release_store_label(b));
  EXPECT_TRUE(memory::release_store_label(a, '\3') < memory::release_store_label(b));

  EXPECT_TRUE(memory::release_store_label(b, '\0') < memory::release_store_label(c));
  EXPECT_TRUE(memory::release_store_label(b, '\1') < memory::release_store_label(c));
  EXPECT_TRUE(memory::release_store_label(b, '\2') < memory::release_store_label(c));
  EXPECT_TRUE(memory::release_store_label(b, '\3') < memory::release_store_label(c));
}

TEST(CkaTest, IsShared)
{
  EXPECT_TRUE(memory::is_shared(memory::release_store_label(0U), memory::acquire_load_label(0U)));
  EXPECT_TRUE(memory::is_shared(memory::release_store_label(1U), memory::acquire_load_label(1U)));
  EXPECT_TRUE(memory::is_shared(memory::release_store_label(2U), memory::acquire_load_label(2U)));

  EXPECT_FALSE(memory::is_shared(memory::release_store_label(0U), memory::acquire_load_label(1U)));
  EXPECT_FALSE(memory::is_shared(memory::release_store_label(1U), memory::acquire_load_label(3U)));
  EXPECT_FALSE(memory::is_shared(memory::release_store_label(2U), memory::acquire_load_label(4U)));
  EXPECT_FALSE(memory::is_shared(memory::release_store_label(3U), memory::acquire_load_label(4U)));
  EXPECT_FALSE(memory::is_shared(memory::release_store_label(4U), memory::acquire_load_label(5U)));

  // Would cause an assertion violation:
  //
  //   EXPECT_ANY_THROW((memory::is_shared(memory::none_store_label(0U), memory::acquire_load_label(0U))));
  //   EXPECT_ANY_THROW((memory::is_shared(memory::release_store_label(0U), memory::none_load_label(0U))));
}

TEST(CkaTest, IsNoneStore)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;

  EXPECT_TRUE(memory::is_none_store(memory::none_store_label(a)));
  EXPECT_TRUE(memory::is_none_store(memory::none_store_label(b)));
  EXPECT_TRUE(memory::is_none_store(memory::none_store_label(c)));

  EXPECT_TRUE(memory::is_store(memory::none_store_label(a)));
  EXPECT_TRUE(memory::is_store(memory::none_store_label(b)));
  EXPECT_TRUE(memory::is_store(memory::none_store_label(c)));

  EXPECT_FALSE(memory::is_none_store(memory::none_load_label(a)));
  EXPECT_FALSE(memory::is_none_store(memory::none_load_label(b)));
  EXPECT_FALSE(memory::is_none_store(memory::none_load_label(c)));

  EXPECT_FALSE(memory::is_none_store(memory::acquire_load_label(a)));
  EXPECT_FALSE(memory::is_none_store(memory::acquire_load_label(b)));
  EXPECT_FALSE(memory::is_none_store(memory::acquire_load_label(c)));
}

TEST(CkaTest, IsNoneLoad)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;

  EXPECT_TRUE(memory::is_none_load(memory::none_load_label(a)));
  EXPECT_TRUE(memory::is_none_load(memory::none_load_label(b)));
  EXPECT_TRUE(memory::is_none_load(memory::none_load_label(c)));

  EXPECT_TRUE(memory::is_load(memory::none_load_label(a)));
  EXPECT_TRUE(memory::is_load(memory::none_load_label(b)));
  EXPECT_TRUE(memory::is_load(memory::none_load_label(c)));

  EXPECT_FALSE(memory::is_none_load(memory::none_store_label(a)));
  EXPECT_FALSE(memory::is_none_load(memory::none_store_label(b)));
  EXPECT_FALSE(memory::is_none_load(memory::none_store_label(c)));

  EXPECT_FALSE(memory::is_none_load(memory::release_store_label(a)));
  EXPECT_FALSE(memory::is_none_load(memory::release_store_label(b)));
  EXPECT_FALSE(memory::is_none_load(memory::release_store_label(c)));
}

TEST(CkaTest, IsReleaseStore)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;

  EXPECT_TRUE(memory::is_release_store(memory::release_store_label(a)));
  EXPECT_TRUE(memory::is_release_store(memory::release_store_label(b)));
  EXPECT_TRUE(memory::is_release_store(memory::release_store_label(c)));

  EXPECT_TRUE(memory::is_store(memory::release_store_label(a)));
  EXPECT_TRUE(memory::is_store(memory::release_store_label(b)));
  EXPECT_TRUE(memory::is_store(memory::release_store_label(c)));

  EXPECT_FALSE(memory::is_release_store(memory::none_load_label(a)));
  EXPECT_FALSE(memory::is_release_store(memory::none_load_label(b)));
  EXPECT_FALSE(memory::is_release_store(memory::none_load_label(c)));

  EXPECT_FALSE(memory::is_release_store(memory::acquire_load_label(a)));
  EXPECT_FALSE(memory::is_release_store(memory::acquire_load_label(b)));
  EXPECT_FALSE(memory::is_release_store(memory::acquire_load_label(c)));
}

TEST(CkaTest, IsAcquireLoad)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;

  EXPECT_TRUE(memory::is_acquire_load(memory::acquire_load_label(a)));
  EXPECT_TRUE(memory::is_acquire_load(memory::acquire_load_label(b)));
  EXPECT_TRUE(memory::is_acquire_load(memory::acquire_load_label(c)));

  EXPECT_TRUE(memory::is_load(memory::acquire_load_label(a)));
  EXPECT_TRUE(memory::is_load(memory::acquire_load_label(b)));
  EXPECT_TRUE(memory::is_load(memory::acquire_load_label(c)));

  EXPECT_FALSE(memory::is_acquire_load(memory::none_store_label(a)));
  EXPECT_FALSE(memory::is_acquire_load(memory::none_store_label(b)));
  EXPECT_FALSE(memory::is_acquire_load(memory::none_store_label(c)));

  EXPECT_FALSE(memory::is_acquire_load(memory::release_store_label(a)));
  EXPECT_FALSE(memory::is_acquire_load(memory::release_store_label(b)));
  EXPECT_FALSE(memory::is_acquire_load(memory::release_store_label(c)));
}

TEST(CkaTest, IsAssert)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;

  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(a, '\0')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(b, '\0')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(c, '\0')));

  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(a, '\1')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(b, '\1')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(c, '\1')));

  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(a, '\2')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(b, '\2')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(c, '\2')));

  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(a, '\3')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(b, '\3')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(c, '\3')));

  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(a, '\4')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(b, '\4')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(c, '\4')));

  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(a, '\5')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(b, '\5')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_eq_label(c, '\5')));

  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(a, '\0')));
  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(b, '\0')));
  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(c, '\0')));

  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(a, '\1')));
  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(b, '\1')));
  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(c, '\1')));

  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(a, '\2')));
  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(b, '\2')));
  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(c, '\2')));

  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(a, '\3')));
  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(b, '\3')));
  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(c, '\3')));

  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(a, '\4')));
  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(b, '\4')));
  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(c, '\4')));

  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(a, '\5')));
  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(b, '\5')));
  EXPECT_TRUE(memory::is_assume_acquire_eq(memory::assume_acquire_eq_label(c, '\5')));

  // negation
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(a, '\0')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(b, '\0')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(c, '\0')));

  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(a, '\1')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(b, '\1')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(c, '\1')));

  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(a, '\2')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(b, '\2')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(c, '\2')));

  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(a, '\3')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(b, '\3')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(c, '\3')));

  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(a, '\4')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(b, '\4')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(c, '\4')));

  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(a, '\5')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(b, '\5')));
  EXPECT_TRUE(memory::is_assume(memory::assume_acquire_neq_label(c, '\5')));

  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(a, '\0')));
  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(b, '\0')));
  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(c, '\0')));

  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(a, '\1')));
  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(b, '\1')));
  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(c, '\1')));

  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(a, '\2')));
  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(b, '\2')));
  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(c, '\2')));

  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(a, '\3')));
  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(b, '\3')));
  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(c, '\3')));

  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(a, '\4')));
  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(b, '\4')));
  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(c, '\4')));

  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(a, '\5')));
  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(b, '\5')));
  EXPECT_TRUE(memory::is_assume_acquire_neq(memory::assume_acquire_neq_label(c, '\5')));
}

TEST(CkaTest, StoredByte)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;

  EXPECT_EQ('\1', memory::byte(memory::none_store_label(a, '\1')));
  EXPECT_EQ('\2', memory::byte(memory::none_store_label(a, '\2')));
  EXPECT_EQ('\3', memory::byte(memory::none_store_label(a, '\3')));
  EXPECT_EQ('\4', memory::byte(memory::none_store_label(a, '\4')));
  EXPECT_EQ('\5', memory::byte(memory::none_store_label(a, '\5')));

  EXPECT_EQ('\1', memory::byte(memory::none_store_label(b, '\1')));
  EXPECT_EQ('\2', memory::byte(memory::none_store_label(b, '\2')));
  EXPECT_EQ('\3', memory::byte(memory::none_store_label(b, '\3')));
  EXPECT_EQ('\4', memory::byte(memory::none_store_label(b, '\4')));
  EXPECT_EQ('\5', memory::byte(memory::none_store_label(b, '\5')));

  EXPECT_EQ('\1', memory::byte(memory::none_store_label(c, '\1')));
  EXPECT_EQ('\2', memory::byte(memory::none_store_label(c, '\2')));
  EXPECT_EQ('\3', memory::byte(memory::none_store_label(c, '\3')));
  EXPECT_EQ('\4', memory::byte(memory::none_store_label(c, '\4')));
  EXPECT_EQ('\5', memory::byte(memory::none_store_label(c, '\5')));

  EXPECT_EQ('\1', memory::byte(memory::release_store_label(a, '\1')));
  EXPECT_EQ('\2', memory::byte(memory::release_store_label(a, '\2')));
  EXPECT_EQ('\3', memory::byte(memory::release_store_label(a, '\3')));
  EXPECT_EQ('\4', memory::byte(memory::release_store_label(a, '\4')));
  EXPECT_EQ('\5', memory::byte(memory::release_store_label(a, '\5')));

  EXPECT_EQ('\1', memory::byte(memory::release_store_label(b, '\1')));
  EXPECT_EQ('\2', memory::byte(memory::release_store_label(b, '\2')));
  EXPECT_EQ('\3', memory::byte(memory::release_store_label(b, '\3')));
  EXPECT_EQ('\4', memory::byte(memory::release_store_label(b, '\4')));
  EXPECT_EQ('\5', memory::byte(memory::release_store_label(b, '\5')));

  EXPECT_EQ('\1', memory::byte(memory::release_store_label(c, '\1')));
  EXPECT_EQ('\2', memory::byte(memory::release_store_label(c, '\2')));
  EXPECT_EQ('\3', memory::byte(memory::release_store_label(c, '\3')));
  EXPECT_EQ('\4', memory::byte(memory::release_store_label(c, '\4')));
  EXPECT_EQ('\5', memory::byte(memory::release_store_label(c, '\5')));
}

TEST(CkaTest, AssertedEqualByte)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;

  EXPECT_EQ('\1', memory::byte(memory::assume_acquire_eq_label(a, '\1')));
  EXPECT_EQ('\2', memory::byte(memory::assume_acquire_eq_label(a, '\2')));
  EXPECT_EQ('\3', memory::byte(memory::assume_acquire_eq_label(a, '\3')));
  EXPECT_EQ('\4', memory::byte(memory::assume_acquire_eq_label(a, '\4')));
  EXPECT_EQ('\5', memory::byte(memory::assume_acquire_eq_label(a, '\5')));

  EXPECT_EQ('\1', memory::byte(memory::assume_acquire_eq_label(b, '\1')));
  EXPECT_EQ('\2', memory::byte(memory::assume_acquire_eq_label(b, '\2')));
  EXPECT_EQ('\3', memory::byte(memory::assume_acquire_eq_label(b, '\3')));
  EXPECT_EQ('\4', memory::byte(memory::assume_acquire_eq_label(b, '\4')));
  EXPECT_EQ('\5', memory::byte(memory::assume_acquire_eq_label(b, '\5')));

  EXPECT_EQ('\1', memory::byte(memory::assume_acquire_eq_label(c, '\1')));
  EXPECT_EQ('\2', memory::byte(memory::assume_acquire_eq_label(c, '\2')));
  EXPECT_EQ('\3', memory::byte(memory::assume_acquire_eq_label(c, '\3')));
  EXPECT_EQ('\4', memory::byte(memory::assume_acquire_eq_label(c, '\4')));
  EXPECT_EQ('\5', memory::byte(memory::assume_acquire_eq_label(c, '\5')));
}

TEST(CkaTest, AssertedNotEqualByte)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;

  EXPECT_EQ('\1', memory::byte(memory::assume_acquire_neq_label(a, '\1')));
  EXPECT_EQ('\2', memory::byte(memory::assume_acquire_neq_label(a, '\2')));
  EXPECT_EQ('\3', memory::byte(memory::assume_acquire_neq_label(a, '\3')));
  EXPECT_EQ('\4', memory::byte(memory::assume_acquire_neq_label(a, '\4')));
  EXPECT_EQ('\5', memory::byte(memory::assume_acquire_neq_label(a, '\5')));

  EXPECT_EQ('\1', memory::byte(memory::assume_acquire_neq_label(b, '\1')));
  EXPECT_EQ('\2', memory::byte(memory::assume_acquire_neq_label(b, '\2')));
  EXPECT_EQ('\3', memory::byte(memory::assume_acquire_neq_label(b, '\3')));
  EXPECT_EQ('\4', memory::byte(memory::assume_acquire_neq_label(b, '\4')));
  EXPECT_EQ('\5', memory::byte(memory::assume_acquire_neq_label(b, '\5')));

  EXPECT_EQ('\1', memory::byte(memory::assume_acquire_neq_label(c, '\1')));
  EXPECT_EQ('\2', memory::byte(memory::assume_acquire_neq_label(c, '\2')));
  EXPECT_EQ('\3', memory::byte(memory::assume_acquire_neq_label(c, '\3')));
  EXPECT_EQ('\4', memory::byte(memory::assume_acquire_neq_label(c, '\4')));
  EXPECT_EQ('\5', memory::byte(memory::assume_acquire_neq_label(c, '\5')));
}

TEST(CkaTest, MemoryAddress)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;
  constexpr memory::Address d = 3U;

  EXPECT_EQ(0U, memory::address(memory::none_load_label(a)));
  EXPECT_EQ(1U, memory::address(memory::none_load_label(b)));
  EXPECT_EQ(2U, memory::address(memory::none_load_label(c)));
  EXPECT_EQ(3U, memory::address(memory::none_load_label(d)));

  EXPECT_EQ(0U, memory::address(memory::none_store_label(a)));
  EXPECT_EQ(1U, memory::address(memory::none_store_label(b)));
  EXPECT_EQ(2U, memory::address(memory::none_store_label(c)));
  EXPECT_EQ(3U, memory::address(memory::none_store_label(d)));

  EXPECT_EQ(0U, memory::address(memory::acquire_load_label(a)));
  EXPECT_EQ(1U, memory::address(memory::acquire_load_label(b)));
  EXPECT_EQ(2U, memory::address(memory::acquire_load_label(c)));
  EXPECT_EQ(3U, memory::address(memory::acquire_load_label(d)));

  EXPECT_EQ(0U, memory::address(memory::release_store_label(a)));
  EXPECT_EQ(1U, memory::address(memory::release_store_label(b)));
  EXPECT_EQ(2U, memory::address(memory::release_store_label(c)));
  EXPECT_EQ(3U, memory::address(memory::release_store_label(d)));
}

TEST(CkaTest, MemoryRefinementReadFromSameAddress)
{
  constexpr memory::Address a = 0U;

  memory::Refinement r;

  PartialString x{memory::release_store_label(a, '\1')};
  PartialString y{memory::release_store_label(a, '\2')};
  PartialString z{memory::acquire_load_label(a)};

  PartialString p{((x , y) | z)};

  EXPECT_TRUE(r.check((x , z , y), p));
  EXPECT_TRUE(r.check((x , y , z), p));

  // LHS violates the sequenced-before ordering in "p"
  EXPECT_FALSE(r.check((y , x , z), p));

  // since "x" and "y" have different labels, LHS violates
  // the sequenced-before order of "x" and "y" in "p".
  EXPECT_FALSE(r.check((y , z , x), p));

  // But `alike_y` and `y` have the same label so the reordering is allowed.
  PartialString alike_y{memory::release_store_label(a, '\2')};
  EXPECT_TRUE(r.check((y , z , alike_y), ((alike_y , y) | z)));

  // Our memory model rules out that an acquire load happens-before
  // all release stores on the same memory address.
  EXPECT_FALSE(r.check((z , y , x), p));
  EXPECT_FALSE(r.check((z , x , y), p));
  EXPECT_FALSE(r.check((z , (x | y)), p));
}

TEST(CkaTest, MemoryRefinementReadFromDifferentAddress)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::Refinement r;

  PartialString x{memory::release_store_label(a)};
  PartialString y{memory::release_store_label(b)};
  PartialString z{memory::acquire_load_label(a)};

  PartialString p{((x , y) | z)};

  EXPECT_TRUE(r.check((x , z , y), p));
  EXPECT_TRUE(r.check((x , y , z), p));

  // LHS violates the sequenced-before ordering in "p"
  EXPECT_FALSE(r.check((y , x , z), p));
  EXPECT_FALSE(r.check((y , z , x), p));

  // "y" writes a different memory address from "x"
  // and both are unsequenced in RHS
  EXPECT_TRUE(r.check((y , x , z), (x | y | z)));
  EXPECT_FALSE(r.check((y , z , x), (x | y | z)));

  EXPECT_FALSE(r.check((z , y , x), p));
  EXPECT_FALSE(r.check((z , x , y), p));
  EXPECT_FALSE(r.check((z , (x | y)), p));
}

TEST(CkaTest, MemoryRefinementModificationOrderSameAddress)
{
  constexpr memory::Address a = 0U;

  memory::Refinement r;

  PartialString x{memory::release_store_label(a)};
  PartialString y{memory::release_store_label(a)};

  PartialString p{(x | y)};

  EXPECT_TRUE(r.check((x , y), p));
  EXPECT_TRUE(r.check((y , x), p));

  // Our memory model rules out that two atomic stores
  // on the same memory address happen concurrently even
  // when there are no atomic loads reading from these.
  EXPECT_FALSE(r.check((x | y), p));
}

TEST(CkaTest, MemoryRefinementModificationOrderDifferentAddress)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::Refinement r;

  PartialString x{memory::release_store_label(a)};
  PartialString y{memory::release_store_label(b)};

  PartialString p{(x | y)};

  EXPECT_TRUE(r.check((x , y), p));
  EXPECT_TRUE(r.check((y , x), p));
  EXPECT_TRUE(r.check((x | y), p));
}

TEST(CkaTest, MemoryRefinementRelaxedPartialString)
{
  constexpr memory::Address a = 0U;

  memory::Refinement r;

  PartialString u{memory::release_store_label(a, 1)};
  PartialString v{memory::release_store_label(a, 2)};
  PartialString x{memory::acquire_load_label(a)};
  PartialString y{memory::acquire_load_label(a)};

  PartialString p{((u , v) | (x , y))};

  EXPECT_FALSE(r.check(p, p));
}

TEST(CkaTest, MemoryRefinementRelaxedProgram)
{
  constexpr memory::Address a = 0U;

  memory::Refinement r;

  Program U{memory::release_store_label(a, 1)};
  Program V{memory::release_store_label(a, 2)};
  Program X{memory::acquire_load_label(a)};
  Program Y{memory::acquire_load_label(a)};

  Program P{((U , V) | (X , Y))};

  EXPECT_TRUE(r.check((U , X , Y , V), P));
  EXPECT_TRUE(r.check((U ,  X , V , Y), P));
  EXPECT_TRUE(r.check((U , V , X , Y), P));

  // since "Y" and "X" have the same label, they can be reordered
  EXPECT_TRUE(r.check((U , V , Y , X), P));
  EXPECT_TRUE(r.check((U , Y , X , V), P));

  EXPECT_FALSE(r.check(P, P));
  EXPECT_FALSE(r.check((V , X , Y , V), P));
  EXPECT_FALSE(r.check((U , X , Y , U), P));
  EXPECT_FALSE(r.check((V , X , Y , U), P));
  EXPECT_FALSE(r.check(((U | V) , X , Y), P));
  EXPECT_FALSE(r.check((U , V , (X | Y)), P));
  EXPECT_FALSE(r.check((X , U , Y , V), P));
  EXPECT_FALSE(r.check((X , Y , U , V), P));
  EXPECT_FALSE(r.check((X , U , V, Y), P));
  EXPECT_FALSE(r.check((Y , U , V, X), P));
  EXPECT_FALSE(r.check((Y , U , X , V), P));
  EXPECT_FALSE(r.check((Y , X , U , V), P));
}

/*
 * Consider the following program consistent of two threads T1 and T2:
 * "[a] := 0; [b] := 0; (T1 || T2)" where 'a' and 'b' are shared memory
 * addresses, T1 = "[a] := 1; r1 := [b]" and T2 = "[b] := 1; r2 := [a]".
 *
 * Let p0 = "[a] := 0",
 *     p1 = "[b] := 0",
 *     p2 = "[a] := 1",
 *     p3 = "r1 := [b]",
 *     p4 = "[b] := 1",
 *     p5 = "r2 := [a]".
 *
 * Sequential consistency can be achieved by requiring that
 * "p2" is sequenced-before "p3", and "p4" is sequenced-before "p5".
 */
TEST(CkaTest, MemoryHandwrittenPartialStringSequentialConsistency)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::Refinement r;

  PartialString p0{memory::release_store_label(a, '\0')};
  PartialString p1{memory::release_store_label(b, '\0')};
  PartialString p2{memory::release_store_label(a, '\1')};
  PartialString p3{memory::acquire_load_label(b)};
  PartialString p4{memory::release_store_label(b, '\1')};
  PartialString p5{memory::acquire_load_label(a)};

  PartialString v0{((p2 | p4) , (p3 | p5))};
  PartialString v1{(p4 , p5 , p2 , p3)};
  PartialString v2{(p2 , p3 , p4 , p5)};

  PartialString x0{((p0 | p1) , v0)};
  PartialString x1{((p0 | p1) , v1)};
  PartialString x2{((p0 | p1) , v2)};

  PartialString y0{((p0 , p1) , v0)};
  PartialString y1{((p0 , p1) , v1)};
  PartialString y2{((p0 , p1) , v2)};

  PartialString r0{((p0 , p1 ) , ((p2 , p3) | (p4 , p5)))};
  PartialString r1{(p0 | p1 | ((p2 , p3) | (p4 , p5)))};
  PartialString r2{(p0 | p1 | p2 | p3 | p4 | p5)};

  // Both "p0" and "p1" are unsequenced in "p", but sequenced in "r0"
  EXPECT_FALSE(r.check(x0, r0));
  EXPECT_FALSE(r.check(x1, r0));
  EXPECT_FALSE(r.check(x2, r0));

  EXPECT_TRUE(r.check(x0, r1));
  EXPECT_TRUE(r.check(x1, r1));
  EXPECT_TRUE(r.check(x2, r1));

  EXPECT_TRUE(r.check(x0, r2));
  EXPECT_TRUE(r.check(x1, r2));
  EXPECT_TRUE(r.check(x2, r2));

  EXPECT_TRUE(r.check(y0, r0));
  EXPECT_TRUE(r.check(y1, r0));
  EXPECT_TRUE(r.check(y2, r0));

  EXPECT_TRUE(r.check(y0, r1));
  EXPECT_TRUE(r.check(y1, r0));
  EXPECT_TRUE(r.check(y2, r0));

  EXPECT_TRUE(r.check(y0, r2));
  EXPECT_TRUE(r.check(y1, r2));
  EXPECT_TRUE(r.check(y2, r2));
}

/*
 * Consider the following program consistent of two threads T1 and T2:
 * "[a] := 0; [b] := 0; (T1 || T2)" where 'a' and 'b' are shared memory
 * addresses, T1 = "[a] := 1; r1 := [b]" and T2 = "[b] := 1; r2 := [a]".
 *
 * Let P0 = "[a] := 0",
 *     P1 = "[b] := 0",
 *     P2 = "[a] := 1",
 *     P3 = "r1 := [b]",
 *     P4 = "[b] := 1",
 *     P5 = "r2 := [a]".
 *
 * Sequential consistency can be achieved by requiring that
 * "P2" is sequenced-before "P3", and "P4" is sequenced-before "P5".
 */
TEST(CkaTest, MemoryHandwrittenProgramSequentialConsistency)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::Refinement r;

  Program P0{memory::release_store_label(a, '\0')};
  Program P1{memory::release_store_label(b, '\0')};
  Program P2{memory::release_store_label(a, '\1')};
  Program P3{memory::acquire_load_label(b)};
  Program P4{memory::release_store_label(b, '\1')};
  Program P5{memory::acquire_load_label(a)};

  Program A{((P2 | P4) , (P3 | P5))};
  Program B{(P4 , P5 , P2 , P3)};
  Program C{(P2 , P3 , P4 , P5)};

  Program P{((P0 | P1) , (A + B + C))};
  Program Q{((P0 , P1) , (A + B + C))};

  Program R0{((P0 , P1 ) , ((P2 , P3) | (P4 , P5)))};
  Program R1{(P0 | P1 | ((P2 , P3) | (P4 , P5)))};
  Program R2{(P0 | P1 | P2 | P3 | P4 | P5)};

  // Both "P0" and "P1" are unsequenced in "P", but sequenced in "R0"
  EXPECT_FALSE(r.check(P, R0));

  EXPECT_TRUE(r.check(P, R1));
  EXPECT_TRUE(r.check(P, R2));

  EXPECT_TRUE(r.check(Q, R0));
  EXPECT_TRUE(r.check(Q, R1));
  EXPECT_TRUE(r.check(Q, R2));
}

/// Consider the following concurrent program "P":
///
///   [x] := 0 ; [y] := 0 ; (r0 := [x] | r1 := [y] | [y] := 2 | [x] := 1)
///
/// By reflexivity, "P <= P" for the program refinement relation without
/// memory axioms.
///
/// As we know, if we add the memory axioms, then "P" on the RHS is made
/// more precise. Recall that this increase in precision has to be matched
/// on the LHS for the refinement relation to hold. After all, that's the
/// purpose of the memory axioms!
///
/// It is important to understand the ramifications of this. To illustrate
/// potential pitfalls, let's look at the effects of the From-Read axioms.
///
/// If we apply the memory axioms on the RHS without the From-Read axiom,
/// then "r.check(P, P)" holds because there exists a Read-From relation
/// in the RHS that is compatible with the LHS, namely that both registers
/// are set to zero by having both loads read-from the initial stores.
///
/// However, if we do add the From-Read axiom to the memory axiom encoding,
/// then "r.check(P, P)" fails because the From-Read axiom induces on the
/// previous Read-From relation the following happens-before orderings:
///
///   "[x] := 0" happens-before "r0 := [x]" happens-before "[x] := 1"
///
/// Similarly, for the "y" memory location.
///
/// Since this happens-before ordering is not compatible with the LHS,
/// the refinement fails, as desired.
TEST(CkaTest, MemoryAxiomWithFromReadAndMultipleWrites)
{
  constexpr memory::Address x = 0U;
  constexpr memory::Address y = 1U;

  Program initx{memory::release_store_label(x, '\0')};
  Program inity{memory::release_store_label(y, '\0')};

  Program w1y{memory::release_store_label(y, '\2')};
  Program w1x{memory::release_store_label(x, '\1')};

  Program r2x{memory::acquire_load_label(x)};
  Program r2y{memory::acquire_load_label(y)};

  Program P{(initx , inity, (r2x | r2y | w1y | w1x))};

  EXPECT_TRUE(P <= P);

  memory::Refinement r;
  EXPECT_FALSE(r.check(P, P));
}

TEST(CkaTest, MemoryAxiomWithFailingAssertion)
{
  constexpr memory::Address x = 0U;

  Program Wx{memory::release_store_label(x, '\0')};
  Program AnZx{memory::assume_acquire_neq_label(x, '\0')};
  Program AZx{memory::assume_acquire_eq_label(x, '\0')};

  Program P{(Wx , AnZx)};
  Program Q{(Wx , AZx)};

  EXPECT_TRUE(P <= P);
  EXPECT_TRUE(Q <= Q);

  EXPECT_FALSE(P <= Q);
  EXPECT_FALSE(Q <= P);

  memory::Refinement r;

  EXPECT_FALSE(r.check(P, P));
  EXPECT_FALSE(r.check(P, Q));
  EXPECT_FALSE(r.check(Q, P));

  EXPECT_TRUE(r.check(Q, Q));
}

TEST(CkaTest, MemoryAxiomWithAssertionsAndUnsequencedWrites)
{
  constexpr memory::Address x = 0U;

  Program W1x{memory::release_store_label(x, '\1')};
  Program W2x{memory::release_store_label(x, '\2')};

  Program A1x{memory::assume_acquire_eq_label(x, '\1')};
  Program A2x{memory::assume_acquire_eq_label(x, '\2')};

  Program U{((W1x | W2x) , (A1x + A2x))};
  Program V{(W2x , W1x , A1x)};
  Program X{(W2x , W1x , A2x)};
  Program Y{(W1x , W2x , A1x)};
  Program Z{(W1x , W2x , A2x)};

  EXPECT_TRUE(V <= U);
  EXPECT_TRUE(X <= U);
  EXPECT_TRUE(Y <= U);

  memory::Refinement r;

  EXPECT_TRUE(r.check(V, U));
  EXPECT_TRUE(r.check(Z, U));

  EXPECT_FALSE(r.check(X, U));
  EXPECT_FALSE(r.check(Y, U));
}

TEST(CkaTest, MemoryAxiomWithAssertionsAndSequencedWrites)
{
  constexpr memory::Address x = 0U;

  Program W1x{memory::release_store_label(x, '\1')};
  Program W2x{memory::release_store_label(x, '\2')};

  Program A1x{memory::assume_acquire_eq_label(x, '\1')};
  Program A2x{memory::assume_acquire_neq_label(x, '\1')};

  Program X{(W1x , (A1x + A2x) , W2x)};
  Program Y{(W1x , A1x , W2x)};
  Program Z{(W1x , A2x , W2x)};

  EXPECT_TRUE(Y <= X);
  EXPECT_TRUE(Z <= X);

  memory::Refinement r;

  EXPECT_TRUE(r.check(Y, X));
  EXPECT_FALSE(r.check(Z, X));
}

/// Consider the relaxed memory program shown in:
///
///   https://gcc.gnu.org/wiki/Atomic/GCCMM/AtomicSync
///
/// -Thread 1-       -Thread 2-                   -Thread 3-
/// y.store (20);    if (x.load() == 10) {        if (y.load() == 10)
/// x.store (10);      assert (y.load() == 20)      assert (x.load() == 10)
///                    y.store (10)
///                  }
TEST(CkaTest, GnuExampleOverApproximation)
{
  Program initx{1};
  Program inity{2};

  Program w1y{3};
  Program w1x{4};

  Program if2f{5};
  Program if2t{6};
  Program a2f{7};
  Program a2t{8};
  Program w2y{9};

  Program if3f{10};
  Program if3t{11};
  Program a3f{12};
  Program a3t{13};

  Program R{((initx , inity) ,
              ( (w1y | w1x)
              | ((if2t , (a2f + (a2t , w2y))) + if2f)
              | ((if3t , (a3t + a3f)) + if3f)))};

  Program SC{((initx , inity) ,
              ( ((if2f | if3f | w1y) , w1x)
              + (w1y , w1x ,
                  ( (((if2t , a2t) | if3f) , w2y)
                  + (if2t , a2t , w2y , if3t , a3t)))))};

  EXPECT_TRUE(SC <= R);
}

TEST(CkaTest, GnuExampleOverApproximationWithMemoryLabels)
{
  constexpr memory::Address x = 0U;
  constexpr memory::Address y = 1U;

  Program initx{memory::release_store_label(x, '\0')};
  Program inity{memory::release_store_label(y, '\0')};

  Program w1y{memory::release_store_label(y, '\20')};
  Program w1x{memory::release_store_label(x, '\10')};

  Program if2f{memory::acquire_load_label(x)};
  Program if2t{memory::acquire_load_label(x)};
  Program a2f{memory::acquire_load_label(y)};
  Program a2t{memory::acquire_load_label(y)};
  Program w2y{memory::release_store_label(y, '\10')};

  Program if3f{memory::acquire_load_label(y)};
  Program if3t{memory::acquire_load_label(y)};
  Program a3f{memory::acquire_load_label(x)};
  Program a3t{memory::acquire_load_label(x)};

  Program R{((initx , inity) ,
              ( (w1y | w1x)
              | ((if2t , (a2f + (a2t , w2y))) + if2f)
              | ((if3t , (a3t + a3f)) + if3f)))};

  Program SCOverApproximation{((initx , inity) ,
              ( ((if2f | if3f | w1y) , w1x)
              + (w1y , w1x ,
                  ( (((if2t , a2t) | if3f) , w2y)
                  + (if2t , a2t , w2y , if3t , a3t)))))};

  EXPECT_TRUE(SCOverApproximation <= R);
}

TEST(CkaTest, GnuExampleOverApproximationWithMemoryAndAssertionLabels)
{
  constexpr memory::Address x = 0U;
  constexpr memory::Address y = 1U;

  Program initx{memory::release_store_label(x, '\0')};
  Program inity{memory::release_store_label(y, '\0')};

  Program w1y{memory::release_store_label(y, '\20')};
  Program w1x{memory::release_store_label(x, '\10')};

  Program if2f{memory::assume_acquire_neq_label(x, '\10')};
  Program if2t{memory::assume_acquire_eq_label(x, '\10')};
  Program a2f{memory::assume_acquire_neq_label(y, '\20')};
  Program a2t{memory::assume_acquire_eq_label(y, '\20')};
  Program w2y{memory::release_store_label(y, '\10')};

  Program if3f{memory::assume_acquire_neq_label(y, '\10')};
  Program if3t{memory::assume_acquire_eq_label(y, '\10')};
  Program a3f{memory::assume_acquire_neq_label(x, '\10')};
  Program a3t{memory::assume_acquire_eq_label(x, '\10')};

  Program R{((initx , inity) ,
              ( (w1y | w1x)
              | ((if2t , (a2f + (a2t , w2y))) + if2f)
              | ((if3t , (a3t + a3f)) + if3f)))};

  Program SCOverApproximation{((initx , inity) ,
              ( ((if2f | if3f | w1y) , w1x)
              + (w1y , w1x ,
                  ( (((if2t , a2t) | if3f) , w2y)
                  + (if2t , a2t , w2y , if3t , a3t)))))};

  EXPECT_TRUE(SCOverApproximation <= R);
}

TEST(CkaTest, GnuExampleOverApproximationWithAssertionFreeMemoryAxioms)
{
  constexpr memory::Address x = 0U;
  constexpr memory::Address y = 1U;

  Program initx{memory::release_store_label(x, '\0')};
  Program inity{memory::release_store_label(y, '\0')};

  Program w1y{memory::release_store_label(y, '\20')};
  Program w1x{memory::release_store_label(x, '\10')};

  Program if2f{memory::acquire_load_label(x)};
  Program if2t{memory::acquire_load_label(x)};
  Program a2f{memory::acquire_load_label(y)};
  Program a2t{memory::acquire_load_label(y)};
  Program w2y{memory::release_store_label(y, '\10')};

  Program if3f{memory::acquire_load_label(y)};
  Program if3t{memory::acquire_load_label(y)};
  Program a3f{memory::acquire_load_label(x)};
  Program a3t{memory::acquire_load_label(x)};

  Program R{((initx , inity) ,
              ( (w1y | w1x)
              | ((if2t , (a2f + (a2t , w2y))) + if2f)
              | ((if3t , (a3t + a3f)) + if3f)))};

  Program SCOverApproximation{((initx , inity) ,
              ( ((if2f | if3f | w1y) , w1x)
              + (w1y , w1x ,
                  ( (((if2t , a2t) | if3f) , w2y)
                  + (if2t , a2t , w2y , if3t , a3t)))))};

  Program Not_SC{((initx , inity) ,
              ( ((if2f | ((if3f , w1y) + (w1y , if3f))) , w1x)
              + (w1y , w1x ,
                  ( ((if2t , a2f) | if3f)
                  + (if2t , a2t , w2y , if3t , a3t)))))};

  EXPECT_TRUE(SCOverApproximation <= R);

  memory::Refinement r;

  EXPECT_FALSE(r.check(SCOverApproximation, R));

  // Since no assertion labels are used, the memory axioms do not
  // take into account the values, leading to a coarser abstraction.
  EXPECT_TRUE(r.check(Not_SC, R));
}

TEST(CkaTest, GnuExampleOverApproximationWithAssertionsAndMemoryAxioms)
{
  constexpr memory::Address x = 0U;
  constexpr memory::Address y = 1U;

  Program initx{memory::release_store_label(x, '\0')};
  Program inity{memory::release_store_label(y, '\0')};

  Program w1y{memory::release_store_label(y, '\20')};
  Program w1x{memory::release_store_label(x, '\10')};

  Program if2f{memory::assume_acquire_neq_label(x, '\10')};
  Program if2t{memory::assume_acquire_eq_label(x, '\10')};
  Program a2f{memory::assume_acquire_neq_label(y, '\20')};
  Program a2t{memory::assume_acquire_eq_label(y, '\20')};
  Program w2y{memory::release_store_label(y, '\10')};

  Program if3f{memory::assume_acquire_neq_label(y, '\10')};
  Program if3t{memory::assume_acquire_eq_label(y, '\10')};
  Program a3f{memory::assume_acquire_neq_label(x, '\10')};
  Program a3t{memory::assume_acquire_eq_label(x, '\10')};

  Program R{((initx , inity) ,
              ( (w1y | w1x)
              | ((if2t , (a2f + (a2t , w2y))) + if2f)
              | ((if3t , (a3t + a3f)) + if3f)))};

  Program SCHigherOverApproximation{(initx, inity,
        ( (w1y, w1x)
        | ((if2t, a2t, w2y) + if2f)
        | ((if3t, a3t) + if3f)))};

  Program SCLowerOverApproximation{((initx , inity) ,
              ( ((if2f | if3f | w1y) , w1x)
              + (w1y , w1x ,
                  ( (((if2t , a2t) | if3f) , w2y)
                  + (if2t , a2t , w2y , if3t , a3t)))))};

  Program SC{((initx , inity) ,
              ( ((if2f | ((if3f , w1y) + (w1y , if3f))) , w1x)
              + (w1y , w1x ,
                  ( (((if2t , a2t) | if3f) , w2y)
                  + (if2t , a2t , w2y , if3t , a3t)))))};

  EXPECT_TRUE(SCHigherOverApproximation <= R);
  EXPECT_TRUE(SCLowerOverApproximation <= R);
  EXPECT_TRUE(SCLowerOverApproximation <= SCHigherOverApproximation);
  EXPECT_TRUE(SC <= R);
  EXPECT_TRUE(SC <= SCLowerOverApproximation);

  EXPECT_FALSE(SCHigherOverApproximation <= SC);
  EXPECT_FALSE(SCLowerOverApproximation <= SC);
  EXPECT_FALSE(R <= SCHigherOverApproximation);
  EXPECT_FALSE(R <= SCLowerOverApproximation);
  EXPECT_FALSE(SCHigherOverApproximation <= SCLowerOverApproximation);

  memory::Refinement r;

  // refinement check fails because loads and stores on the same
  // memory location do not form a total order in the SC models
  EXPECT_FALSE(r.check(SCHigherOverApproximation, R));
  EXPECT_FALSE(r.check(SCLowerOverApproximation, R));
  EXPECT_FALSE(r.check(SCLowerOverApproximation, SCHigherOverApproximation));
  EXPECT_FALSE(r.check(SCHigherOverApproximation, SCLowerOverApproximation));

  EXPECT_TRUE(r.check(SC, R));
  EXPECT_TRUE(r.check(SC, SCLowerOverApproximation));
  EXPECT_TRUE(r.check(SC, SCHigherOverApproximation));
}

TEST(CkaTest, GnuExampleWithAssertionsAndMemoryAxiomsNotSC)
{
  constexpr memory::Address x = 0U;
  constexpr memory::Address y = 1U;

  Program initx{memory::release_store_label(x, '\0')};
  Program inity{memory::release_store_label(y, '\0')};

  Program w1y{memory::release_store_label(y, '\20')};
  Program w1x{memory::release_store_label(x, '\10')};

  Program if2f{memory::assume_acquire_neq_label(x, '\10')};
  Program if2t{memory::assume_acquire_eq_label(x, '\10')};
  Program a2f{memory::assume_acquire_neq_label(y, '\20')};
  Program a2t{memory::assume_acquire_eq_label(y, '\20')};
  Program w2y{memory::release_store_label(y, '\10')};

  Program if3f{memory::assume_acquire_neq_label(y, '\10')};
  Program if3t{memory::assume_acquire_eq_label(y, '\10')};
  Program a3f{memory::assume_acquire_neq_label(x, '\10')};
  Program a3t{memory::assume_acquire_eq_label(x, '\10')};

  Program R{((initx , inity) ,
              ( (w1y | w1x)
              | ((if2t , (a2f + (a2t , w2y))) + if2f)
              | ((if3t , (a3t + a3f)) + if3f)))};

  // loads and stores on the same memory address must
  // be totally ordered in sequential consistency models,
  // and our refinement check exposes this violation
  Program Wrong_SC{((initx , inity) ,
              ( ((if2f | if3f | w1y) , w1x)
              + (w1y , w1x ,
                  ( (((if2t , a2t) | if3f) , w2y)
                  + (if2t , a2t , w2y , if3t , a3t)))))};

  // events for assertion failures
  Program Not_SC0{((initx , inity) ,
              ( ((if2f | ((if3f , w1y) + (w1y , if3f))) , w1x)
              + (w1y , w1x ,
                  ( ((if2t , a2f) | if3f)
                  + (if2t , a2f , ((if3t , a3f) + (if3f)))))))};

  Program Not_SC1{((initx , inity) ,
              ( ((if2f | ((if3f , w1y) + (w1y , if3f))) , w1x)
              + (w1y , w1x ,
                  ( ((if2t , a2f) | if3f)
                  + (if2t , a2f , if3t , a3t)))))};

  Program Not_SC2{((initx , inity) ,
              ( ((if2f | ((if3f , w1y) + (w1y , if3f))) , w1x)
              + (w1y , w1x ,
                  ( ((if2t , a2f) | if3f)
                  + (if2t , (a2f + (a2t , w2y)) , if3t , a3f)))))};

  // By reflexivity, there exist other memory models that are
  // not SC but which refine "R". For example, one such model
  // violates the total ordering of loads and stores on the
  // same memory location and another violates the assertions
  // that do hold in the "SC" program.
  EXPECT_TRUE(Wrong_SC <= R);
  EXPECT_TRUE(Not_SC0 <= R);
  EXPECT_TRUE(Not_SC1 <= R);
  EXPECT_TRUE(Not_SC2 <= R);

  memory::Refinement r;

  // By adding the memory axioms including the From-Read axiom,
  // we can enforce that every read-from relation induces the
  // correct happens-before ordering in the relaxed model "R".
  EXPECT_FALSE(r.check(Wrong_SC, R));
  EXPECT_FALSE(r.check(Not_SC0, R));
  EXPECT_FALSE(r.check(Not_SC1, R));
  EXPECT_FALSE(r.check(Not_SC2, R));
}

TEST(CkaTest, GnuExampleWithAssertionsAndMemoryAxioms)
{
  constexpr memory::Address x = 0U;
  constexpr memory::Address y = 1U;

  Program initx{memory::release_store_label(x, '\0')};
  Program inity{memory::release_store_label(y, '\0')};

  Program w1y{memory::release_store_label(y, '\20')};
  Program w1x{memory::release_store_label(x, '\10')};

  Program if2f{memory::assume_acquire_neq_label(x, '\10')};
  Program if2t{memory::assume_acquire_eq_label(x, '\10')};
  Program a2f{memory::assume_acquire_neq_label(y, '\20')};
  Program a2t{memory::assume_acquire_eq_label(y, '\20')};
  Program w2y{memory::release_store_label(y, '\10')};

  Program if3f{memory::assume_acquire_neq_label(y, '\10')};
  Program if3t{memory::assume_acquire_eq_label(y, '\10')};
  Program a3f{memory::assume_acquire_neq_label(x, '\10')};
  Program a3t{memory::assume_acquire_eq_label(x, '\10')};

  Program R{((initx , inity) ,
              ( (w1y | w1x)
              | ((if2t , (a2f + (a2t , w2y))) + if2f)
              | ((if3t , (a3t + a3f)) + if3f)))};

  Program SC0{((initx , inity) ,
              ( (w1y , w1x)
              | ((if2t , (a2f + (a2t , w2y))) + if2f)
              | ((if3t , (a3t + a3f)) + if3f)))};

  Program SC1{((initx , inity) ,
              ( ((if2f | ((if3f , w1y) + (w1y , if3f))) , w1x)
              + (w1y , w1x ,
                  ( (((if2t , a2t) | if3f) , w2y)
                  + (if2t , a2t , w2y , if3t , a3t)))))};

  Program SC2{((initx , inity) ,
              ( ((if2f | if3f | w1y) , w1x)
              + (w1y , w1x ,
                  ( (((if2t , a2t) | if3f) , w2y)
                  + (if2t , a2t , w2y , if3t , a3t)))))};

  EXPECT_TRUE(SC0 <= R);
  EXPECT_TRUE(SC1 <= R);
  EXPECT_TRUE(SC1 <= SC0);
  EXPECT_TRUE(SC2 <= R);
  EXPECT_TRUE(SC2 <= SC0);

  memory::Refinement r;
  EXPECT_TRUE(r.check(SC1, R));
  EXPECT_TRUE(r.check(SC1, SC0));

  // writes and reads on the same memory address
  // are not totally ordered in "SC0" and "SC2"
  EXPECT_FALSE(r.check(SC2, R));
  EXPECT_FALSE(r.check(SC2, SC0));
  EXPECT_FALSE(r.check(SC0, R));
}

/// This is the shortest example:
///
///   https://gcc.gnu.org/wiki/Atomic/GCCMM/AtomicSync
///
/// -Thread 1-       -Thread 2-                   -Thread 3-
/// y.store (20);    if (x.load() == 10) {        if (y.load() == 10)
/// x.store (10);      assert (y.load() == 20)      assert (x.load() == 10)
///                    y.store (10)
///                  }
TEST(CkaTest, GnuExampleSmallWithAssertionsAndMemoryAxioms)
{
  constexpr memory::Address x = 0U;
  constexpr memory::Address y = 1U;

  Program initx{memory::release_store_label(x, '\0')};
  Program inity{memory::release_store_label(y, '\0')};

  Program w1y{memory::release_store_label(y, '\20')};
  Program w1x{memory::release_store_label(x, '\10')};

  Program if2f{memory::assume_acquire_neq_label(x, '\10')};
  Program if2t{memory::assume_acquire_eq_label(x, '\10')};
  Program a2f{memory::assume_acquire_neq_label(y, '\20')};
  Program a2t{memory::assume_acquire_eq_label(y, '\20')};
  Program w2y{memory::release_store_label(y, '\10')};

  Program if3f{memory::assume_acquire_neq_label(y, '\10')};
  Program if3t{memory::assume_acquire_eq_label(y, '\10')};
  Program a3f{memory::assume_acquire_neq_label(x, '\10')};
  Program a3t{memory::assume_acquire_eq_label(x, '\10')};

  Program R{((initx , inity) ,
              ( (w1y | w1x)
              | ((if2t , (a2f + (a2t , w2y))) + if2f)
              | ((if3t , (a3t + a3f)) + if3f)))};

  Program SC0{((initx , inity) ,
              ( (w1y , w1x)
              | ((if2t , a2t , w2y) + if2f)
              | ((if3t , a3t) + if3f)))};

  Program SC1{((initx , inity) , (if2f | ((if3f , w1y) + (w1y , if3f))) , w1x)};
  Program SC2{((initx , inity) , (if2f | if3f | w1y) , w1x)};

  EXPECT_TRUE(SC0 <= R);
  EXPECT_TRUE(SC1 <= R);
  EXPECT_TRUE(SC1 <= SC0);
  EXPECT_TRUE(SC2 <= R);
  EXPECT_TRUE(SC2 <= SC0);


  EXPECT_FALSE(R <= SC0);

  memory::Refinement r;
  EXPECT_TRUE(r.check(SC1, R));
  EXPECT_TRUE(r.check(SC1, SC0));

  // writes and reads on the same memory address
  // are not totally ordered in "SC0" and "SC2"
  EXPECT_FALSE(r.check(SC2, R));
  EXPECT_FALSE(r.check(SC2, SC0));
  EXPECT_FALSE(r.check(SC0, R));
}

TEST(CkaTest, RacyOnlyStores)
{
  constexpr memory::Address a = 0U;

  PartialString p0{memory::none_store_label(a, '\0')};
  PartialString p1{memory::none_store_label(a, '\0')};

  PartialString x{(p0 | p1)};
  PartialString y{(p0 , p1)};
  PartialString z{(p1 , p0)};

  memory::DataRaceDetector drd;

  EXPECT_FALSE(drd.is_racy(p0));
  EXPECT_FALSE(drd.is_racy(p1));

  EXPECT_TRUE(drd.is_racy(x));
  EXPECT_FALSE(drd.is_racy(y));
  EXPECT_FALSE(drd.is_racy(z));
}

TEST(CkaTest, NonRacyOnlyReleaseStores)
{
  constexpr memory::Address a = 0U;

  PartialString p0{memory::release_store_label(a, '\0')};
  PartialString p1{memory::release_store_label(a, '\0')};

  PartialString x{(p0 | p1)};
  PartialString y{(p0 , p1)};
  PartialString z{(p1 , p0)};

  memory::DataRaceDetector drd;

  EXPECT_FALSE(drd.is_racy(p0));
  EXPECT_FALSE(drd.is_racy(p1));

  EXPECT_FALSE(drd.is_racy(x));
  EXPECT_FALSE(drd.is_racy(y));
  EXPECT_FALSE(drd.is_racy(z));
}

/// At least one conflicting event must be a non-atomic write
TEST(CkaTest, NonRacyOnlyLoads)
{
  constexpr memory::Address a = 0U;

  PartialString p0{memory::none_load_label(a)};
  PartialString p1{memory::none_load_label(a)};

  PartialString x{(p0 | p1)};
  PartialString y{(p0 , p1)};
  PartialString z{(p1 , p0)};

  memory::DataRaceDetector drd;

  EXPECT_FALSE(drd.is_racy(p0));
  EXPECT_FALSE(drd.is_racy(p1));

  EXPECT_FALSE(drd.is_racy(x));
  EXPECT_FALSE(drd.is_racy(y));
  EXPECT_FALSE(drd.is_racy(z));
}

TEST(CkaTest, RacyNonatomic)
{
  constexpr memory::Address a = 0U;

  PartialString p0{memory::none_store_label(a, '\0')};
  PartialString p1{memory::none_load_label(a)};

  PartialString x{(p0 | p1)};
  PartialString y{(p0 , p1)};
  PartialString z{(p1 , p0)};

  memory::DataRaceDetector drd;

  EXPECT_TRUE(drd.is_racy(x));

  EXPECT_FALSE(drd.is_racy(y));
  EXPECT_FALSE(drd.is_racy(z));
}

/// This example illustrates a data race. The partial string corresponds to
/// the "MP+na_rel+acq_na.c" scenario in the "CppMem" tool developed by
/// Mark Batty, Scott Owens, Jean Pichon, Susmit Sarkar and Peter Sewell.
///
/// John Wickerson's tool demo at MSR Cambridge on 22nd October, 2014
/// featured the example in the context of another research tool that
/// explicitly enumerates all relations which satisfy various versions
/// of the weak memory axioms:
///
///   https://github.com/herd/herdtools
///
/// This sparked the idea to try a symbolic technique instead.
///
/// Example:
///
/// Let T1 be "[a]_none := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; r1 := [a]_none;"
/// where both memory locations "a" and "b" are initially zero.
///
/// The data race occurs when "r0 := [b]_acquire" reads the initial value
/// of "b" and "r1 := [a]_none" reads from "[a]_none := 1". In that case,
/// no synchronize-with relation is induced between the writing and
/// reading of memory location "a", a data race.
TEST(CkaTest, RacyMessagePassingWithInit)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  PartialString p0{memory::release_store_label(a, '\0')};
  PartialString p1{memory::release_store_label(b, '\0')};

  PartialString p2{memory::none_store_label(a, '\1')};
  PartialString p3{memory::release_store_label(b, '\1')};

  PartialString p4{memory::acquire_load_label(b)};
  PartialString p5{memory::none_load_label(a)};

  memory::DataRaceDetector drd;

  PartialString x{((p0 | p1) , ((p2 , p3) | (p4 , p5)))};

  EXPECT_TRUE(drd.is_racy(x));
}

TEST(CkaTest, RacyMessagePassingWithoutInit)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  PartialString p2{memory::none_store_label(a, '\1')};
  PartialString p3{memory::release_store_label(b, '\1')};

  PartialString p4{memory::acquire_load_label(b)};
  PartialString p5{memory::none_load_label(a)};

  memory::DataRaceDetector drd;

  PartialString x{((p2 , p3) | (p4 , p5))};

  // For example, "p4 , p2 , p3 , p5" is a data race
  // but it does not satisfy the axiom that every load
  // happens-before a store to the same memory address.
  EXPECT_FALSE(drd.is_racy(x));
}

/// By changing the previous example so that the code only
/// uses release-acquire, it is clearly not racy anymore.
///
/// Let T1 be "[a]_release := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; r1 := [a]_acquire;"
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, NonRacyMessagePassingUsingAlwaysReleaseAcquire)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  PartialString p0{memory::release_store_label(a, '\0')};
  PartialString p1{memory::release_store_label(b, '\0')};

  PartialString p2{memory::release_store_label(a, '\1')};
  PartialString p3{memory::release_store_label(b, '\1')};

  PartialString p4{memory::acquire_load_label(b)};
  PartialString p5{memory::acquire_load_label(a)};

  memory::DataRaceDetector drd;

  PartialString x{((p0 | p1) , ((p2 , p3) | (p4 , p5)))};

  EXPECT_FALSE(drd.is_racy(x));
}

/// One way of fixing the racy code from RacyMessagePassingWithInit
/// is by checking whether a certain value was indeed read:
///
/// Let T1 be "[a]_none := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end"
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, NonRacyMessagePassingUsingBranch)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  PartialString p0{memory::release_store_label(a, '\0')};
  PartialString p1{memory::release_store_label(b, '\0')};

  PartialString p2{memory::none_store_label(a, '\1')};
  PartialString p3{memory::release_store_label(b, '\1')};

  PartialString p4{memory::assume_acquire_eq_label(b, '\1')};
  PartialString p5{memory::none_load_label(a)};

  PartialString p6{memory::assume_acquire_neq_label(b, '\1')};

  memory::DataRaceDetector drd;

  PartialString x{((p0 | p1) , ((p2 , p3) | (p4 , p5)))};
  PartialString y{((p0 | p1) , ((p2 , p3) | p6))};

  EXPECT_FALSE(drd.is_racy(x));
  EXPECT_FALSE(drd.is_racy(y));
}

/// Even if we check that a certain value was written, we still
/// have a data race if the written value can come from multiple threads:
///
/// Let T1 be "[a]_none := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T1' be "[a]_none := 1; [b]_release := 1"
/// where both memory locations "a" and "b" are initially zero.
///
/// Clearly "T1 | T2 | T1'" is also racy for the reason that
/// there are two "[a]_none := 1" instructions.
TEST(CkaTest, RacyDespiteBranch)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  PartialString p0{memory::release_store_label(a, '\0')};
  PartialString p1{memory::release_store_label(b, '\0')};

  PartialString p2{memory::none_store_label(a, '\1')};
  PartialString p3{memory::release_store_label(b, '\1')};

  PartialString p4{memory::assume_acquire_eq_label(b, '\1')};
  PartialString p5{memory::none_load_label(a)};

  PartialString p6{memory::assume_acquire_neq_label(b, '\1')};

  PartialString p7{memory::none_store_label(a, '\1')};
  PartialString p8{memory::release_store_label(b, '\1')};

  memory::DataRaceDetector drd;

  PartialString x{((p0 | p1) , ((p2 , p3) | (p4 , p5) | (p7 , p8)))};
  PartialString y{((p0 | p1) , ((p2 , p3) | p6 | (p7 , p8)))};

  EXPECT_TRUE(drd.is_racy(x));

  // two "[a]_none := 1"
  EXPECT_TRUE(drd.is_racy(y));
}

/// Furthermore, even if we wrap the non-synchronizing writes inside
/// release-acquire, and we check that a certain value was written,
/// we have still a data race:
///
/// Let T1 be "r0 := [b]_acquire; [a]_none := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T1' be "r0 := [b]_acquire; [a]_none := 1; [b]_release := 1"
/// where both memory locations "a" and "b" are initially zero.
///
/// Clearly "T1 | T2 | T1'" is also racy for the reason that
/// there are two "[a]_none := 1" instructions.
TEST(CkaTest, RacyDespiteBranchAndReleaseAcquireWrap)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  PartialString t0_0{memory::release_store_label(a, '\0')};
  PartialString t0_1{memory::release_store_label(b, '\0')};

  PartialString t1_0{memory::acquire_load_label(b)};
  PartialString t1_1{memory::none_store_label(a, '\1')};
  PartialString t1_2{memory::release_store_label(b, '\1')};

  PartialString t2_0{memory::assume_acquire_eq_label(b, '\1')};
  PartialString t2_1{memory::none_load_label(a)};

  PartialString t2_2{memory::assume_acquire_neq_label(b, '\1')};

  PartialString t3_0{memory::acquire_load_label(b)};
  PartialString t3_1{memory::none_store_label(a, '\1')};
  PartialString t3_2{memory::release_store_label(b, '\1')};

  memory::DataRaceDetector drd;

  PartialString x{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | (t2_0 , t2_1) | (t3_0 , t3_1 , t3_2)))};
  PartialString y{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | t2_2 | (t3_0 , t3_1 , t3_2)))};

  EXPECT_TRUE(drd.is_racy(x));
  EXPECT_TRUE(drd.is_racy(y));
}

/// So let's refine RacyDespiteBranchAndReleaseAcquireWrap as follows:
///
/// Let T1 be "r0 := [b]_acquire; [a]_none := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 0) then r1 := [a]_none; end",
/// and T1' be "r0 := [b]_acquire; if (r0 == 1) then [a]_none := 2; [b]_release := 2; end"
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, RacyDespiteBranchCheckZero)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  PartialString t0_0{memory::release_store_label(a, '\0')};
  PartialString t0_1{memory::release_store_label(b, '\0')};

  PartialString t1_0{memory::acquire_load_label(b)};
  PartialString t1_1{memory::none_store_label(a, '\1')};
  PartialString t1_2{memory::release_store_label(b, '\1')};

  PartialString t2_0{memory::assume_acquire_eq_label(b, '\0')};
  PartialString t2_1{memory::none_load_label(a)};

  PartialString t2_2{memory::assume_acquire_neq_label(b, '\0')};

  PartialString t3_0{memory::assume_acquire_eq_label(b, '\1')};
  PartialString t3_1{memory::none_store_label(a, '\2')};
  PartialString t3_2{memory::release_store_label(b, '\2')};

  PartialString t3_3{memory::assume_acquire_neq_label(b, '\1')};

  memory::DataRaceDetector drd;

  PartialString u{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | t2_2 | t3_3))};
  PartialString v{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | (t2_0 , t2_1) | t3_3))};
  PartialString v_prime{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | (t2_0 , t2_1) | t3_0))};
  PartialString x{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | (t2_0 , t2_1) | (t3_0 , t3_1 , t3_2)))};
  PartialString y{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | t2_2 | (t3_0 , t3_1 , t3_2)))};

  EXPECT_FALSE(drd.is_racy(u));

  EXPECT_TRUE(drd.is_racy(v));
  EXPECT_TRUE(drd.is_racy(v_prime));
  EXPECT_TRUE(drd.is_racy(x));

  EXPECT_FALSE(drd.is_racy(y));
}

/// Let T1 be "r0 := [b]_acquire; [a]_none := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T1' be "r0 := [b]_acquire; if (r0 == 1) then [a]_none := 2; [b]_release := 2; end"
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, RacyDespiteBranchCheckOne)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  PartialString t0_0{memory::release_store_label(a, '\0')};
  PartialString t0_1{memory::release_store_label(b, '\0')};

  PartialString t1_0{memory::acquire_load_label(b)};
  PartialString t1_1{memory::none_store_label(a, '\1')};
  PartialString t1_2{memory::release_store_label(b, '\1')};

  PartialString t2_0{memory::assume_acquire_eq_label(b, '\1')};
  PartialString t2_1{memory::none_load_label(a)};

  PartialString t2_2{memory::assume_acquire_neq_label(b, '\1')};

  PartialString t3_0{memory::assume_acquire_eq_label(b, '\1')};
  PartialString t3_1{memory::none_store_label(a, '\2')};
  PartialString t3_2{memory::release_store_label(b, '\2')};

  PartialString t3_3{memory::assume_acquire_neq_label(b, '\1')};

  memory::DataRaceDetector drd;

  PartialString u{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | t2_2 | t3_3))};
  PartialString v{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | (t2_0 , t2_1) | t3_3))};
  PartialString x{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | (t2_0 , t2_1) | (t3_0 , t3_1 , t3_2)))};
  PartialString y{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | t2_2 | (t3_0 , t3_1 , t3_2)))};

  EXPECT_FALSE(drd.is_racy(u));
  EXPECT_FALSE(drd.is_racy(v));

  EXPECT_TRUE(drd.is_racy(x));
  EXPECT_FALSE(drd.is_racy(y));
}

/// Let T1 be "r0 := [b]_acquire; if r0 == 2 then [a]_none := 1; [b]_release := 1; end",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T1' be "r0 := [b]_acquire; [a]_none := 2; [b]_release := 2"
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, NonRacyWithBranches)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  PartialString t0_0{memory::release_store_label(a, '\0')};
  PartialString t0_1{memory::release_store_label(b, '\0')};

  PartialString t1_0{memory::assume_acquire_eq_label(b, '\2')};
  PartialString t1_1{memory::none_store_label(a, '\1')};
  PartialString t1_2{memory::release_store_label(b, '\1')};

  PartialString t1_3{memory::assume_acquire_neq_label(b, '\2')};

  PartialString t2_0{memory::assume_acquire_eq_label(b, '\1')};
  PartialString t2_1{memory::none_load_label(a)};

  PartialString t2_2{memory::assume_acquire_neq_label(b, '\1')};
  PartialString t2_3{memory::assume_acquire_eq_label(b, '\0')};

  PartialString t3_0{memory::acquire_load_label(b)};
  PartialString t3_1{memory::none_store_label(a, '\2')};
  PartialString t3_2{memory::release_store_label(b, '\2')};

  memory::DataRaceDetector drd;

  PartialString c{((t0_0 | t0_1) , ((t3_0 , t3_1 , t3_2) | t2_2 | t1_3))};
  PartialString d{((t0_0 | t0_1) , ((t3_0 , t3_1 , t3_2) | (t2_0 , t2_1) | t1_3))};
  PartialString e{((t0_0 | t0_1) , ((t3_0 , t3_1 , t3_2) | (t2_0 , t2_1) | (t1_0 , t1_1 , t1_2)))};
  PartialString f{((t0_0 | t0_1) , ((t3_0 , t3_1 , t3_2) | t2_2 | (t1_0 , t1_1 , t1_2)))};

  EXPECT_FALSE(drd.is_racy(c));
  EXPECT_FALSE(drd.is_racy(d));
  EXPECT_FALSE(drd.is_racy(e));
  EXPECT_FALSE(drd.is_racy(f));

  PartialString u{((t0_0 | t0_1) , (t1_3 | t2_2 | (t3_0 , t3_1 , t3_2)))};
  PartialString v{((t0_0 | t0_1) , (t1_3 | (t2_0 , t2_1) | (t3_0 , t3_1 , t3_2)))};
  PartialString x{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | (t2_0 , t2_1) | (t3_0 , t3_1 , t3_2)))};
  PartialString y{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | t2_2 | (t3_0 , t3_1 , t3_2)))};
  PartialString y_prime{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | t2_3 | (t3_0 , t3_1 , t3_2)))};

  EXPECT_FALSE(drd.is_racy(u));
  EXPECT_FALSE(drd.is_racy(v));
  EXPECT_FALSE(drd.is_racy(x));
  EXPECT_FALSE(drd.is_racy(y));
  EXPECT_FALSE(drd.is_racy(y_prime));
}

/// Unlike RacyDespiteBranchCheckOne, in this test T1' does not write "a":
///
/// Let T1 be "r0 := [b]_acquire; [a]_none := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T1' be "r0 := [b]_acquire; if (r0 == 1) then [b]_release := 2; end"
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, NonRacyBranchCheckOne)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  PartialString t0_0{memory::release_store_label(a, '\0')};
  PartialString t0_1{memory::release_store_label(b, '\0')};

  PartialString t1_0{memory::acquire_load_label(b)};
  PartialString t1_1{memory::none_store_label(a, '\1')};
  PartialString t1_2{memory::release_store_label(b, '\1')};

  PartialString t2_0{memory::assume_acquire_eq_label(b, '\1')};
  PartialString t2_1{memory::none_load_label(a)};

  PartialString t2_2{memory::assume_acquire_neq_label(b, '\1')};

  PartialString t3_0{memory::assume_acquire_eq_label(b, '\1')};
  PartialString t3_1{memory::release_store_label(b, '\2')};

  PartialString t3_2{memory::assume_acquire_neq_label(b, '\1')};

  memory::DataRaceDetector drd;

  PartialString u{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | t2_2 | t3_2))};
  PartialString v{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | (t2_0 , t2_1) | t3_2))};
  PartialString x{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | (t2_0 , t2_1) | (t3_0 , t3_1)))};
  PartialString y{((t0_0 | t0_1) , ((t1_0 , t1_1 , t1_2) | t2_2 | (t3_0 , t3_1)))};

  EXPECT_FALSE(drd.is_racy(u));
  EXPECT_FALSE(drd.is_racy(v));
  EXPECT_FALSE(drd.is_racy(x));
  EXPECT_FALSE(drd.is_racy(y));
}

TEST(CkaTest, SymbolicProgramSingle)
{
  memory::SymbolicProgram P{'\1'};
  EXPECT_EQ(1, P.assume_map().size());
  EXPECT_EQ(1, P.unguarded_events().size());
  EXPECT_EQ(1, P.p().length());

  EXPECT_EQ(0, P.assume_map().at(0));
  EXPECT_EQ(0, P.unguarded_events().front());
  EXPECT_EQ('\1', P.p().label_function().at(0));
}

TEST(CkaTest, SymbolicProgramTwoUnguarded)
{
  memory::SymbolicProgram X{'\1'};
  memory::SymbolicProgram Y{'\2'};
  memory::SymbolicProgram Z{X, Y, (X.p() | Y.p())};

  EXPECT_EQ(2, Z.assume_map().size());
  EXPECT_EQ(2, Z.unguarded_events().size());
  EXPECT_EQ(2, Z.p().length());

  EXPECT_EQ(0, Z.assume_map().at(0));
  EXPECT_EQ(1, Z.assume_map().at(1));
  EXPECT_EQ(0, Z.unguarded_events().front());
  EXPECT_EQ(1, Z.unguarded_events().back());

  EXPECT_EQ('\1', Z.p().label_function().at(0));
  EXPECT_EQ('\2', Z.p().label_function().at(1));
}

TEST(CkaTest, SymbolicProgramSequential)
{
  memory::SymbolicProgram X{'\1'};
  memory::SymbolicProgram Y{'\2'};
  memory::SymbolicProgram Z{(X , Y)};

  EXPECT_EQ(2, Z.assume_map().size());
  EXPECT_EQ(2, Z.unguarded_events().size());
  EXPECT_EQ(2, Z.p().length());

  EXPECT_EQ(0, Z.assume_map().at(0));
  EXPECT_EQ(1, Z.assume_map().at(1));
  EXPECT_EQ(0, Z.unguarded_events().front());
  EXPECT_EQ(1, Z.unguarded_events().back());

  EXPECT_EQ('\1', Z.p().label_function().at(0));
  EXPECT_EQ('\2', Z.p().label_function().at(1));
}

TEST(CkaTest, SymbolicProgramConcurrent)
{
  memory::SymbolicProgram X{'\1'};
  memory::SymbolicProgram Y{'\2'};
  memory::SymbolicProgram Z{(X , Y)};

  EXPECT_EQ(2, Z.assume_map().size());
  EXPECT_EQ(2, Z.unguarded_events().size());
  EXPECT_EQ(2, Z.p().length());

  EXPECT_EQ(0, Z.assume_map().at(0));
  EXPECT_EQ(1, Z.assume_map().at(1));
  EXPECT_EQ(0, Z.unguarded_events().front());
  EXPECT_EQ(1, Z.unguarded_events().back());

  EXPECT_EQ('\1', Z.p().label_function().at(0));
  EXPECT_EQ('\2', Z.p().label_function().at(1));
}

TEST(CkaTest, SymbolicProgramSingleIfThen)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Byte b = '\1';

  memory::SymbolicProgram X{'\2'};
  memory::SymbolicProgram Y{if_then(a, b, X)};

  EXPECT_EQ(2, Y.assume_map().size());
  EXPECT_EQ(1, Y.unguarded_events().size());
  EXPECT_EQ(2, Y.p().length());

  EXPECT_EQ(0, Y.assume_map().at(0));
  EXPECT_EQ(0, Y.assume_map().at(1));
  EXPECT_EQ(0, Y.unguarded_events().front());

  EXPECT_TRUE(memory::is_assume_acquire_eq(Y.p().label_function().at(0)));
  EXPECT_EQ(a, memory::address(Y.p().label_function().at(0)));
  EXPECT_EQ(b, memory::byte(Y.p().label_function().at(0)));

  EXPECT_EQ('\2', Y.p().label_function().at(1));
}

TEST(CkaTest, SymbolicProgramSingleIfThenFollowedBy)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Byte b = '\1';

  memory::SymbolicProgram X{'\2'};
  memory::SymbolicProgram Y{'\3'};
  memory::SymbolicProgram Z{(if_then(a, b, X) , Y)};

  EXPECT_EQ(3, Z.assume_map().size());
  EXPECT_EQ(2, Z.unguarded_events().size());
  EXPECT_EQ(3, Z.p().length());

  EXPECT_EQ(0, Z.assume_map().at(0));
  EXPECT_EQ(0, Z.assume_map().at(1));
  EXPECT_EQ(2, Z.assume_map().at(2));
  EXPECT_EQ(0, Z.unguarded_events().front());
  EXPECT_EQ(2, Z.unguarded_events().back());

  EXPECT_TRUE(memory::is_assume_acquire_eq(Z.p().label_function().at(0)));
  EXPECT_EQ(a, memory::address(Z.p().label_function().at(0)));
  EXPECT_EQ(b, memory::byte(Z.p().label_function().at(0)));

  EXPECT_EQ('\2', Z.p().label_function().at(1));
  EXPECT_EQ('\3', Z.p().label_function().at(2));
}

TEST(CkaTest, SymbolicProgramSingleIfThenFollowedByInner)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Byte b = '\1';

  memory::SymbolicProgram X{'\2'};
  memory::SymbolicProgram Z{(if_then(a, b, X) , X)};

  EXPECT_EQ(3, Z.assume_map().size());
  EXPECT_EQ(2, Z.unguarded_events().size());
  EXPECT_EQ(3, Z.p().length());

  EXPECT_EQ(0, Z.assume_map().at(0));
  EXPECT_EQ(0, Z.assume_map().at(1));
  EXPECT_EQ(2, Z.assume_map().at(2));
  EXPECT_EQ(0, Z.unguarded_events().front());
  EXPECT_EQ(2, Z.unguarded_events().back());

  EXPECT_TRUE(memory::is_assume_acquire_eq(Z.p().label_function().at(0)));
  EXPECT_EQ(a, memory::address(Z.p().label_function().at(0)));
  EXPECT_EQ(b, memory::byte(Z.p().label_function().at(0)));

  EXPECT_EQ('\2', Z.p().label_function().at(1));
  EXPECT_EQ('\2', Z.p().label_function().at(2));
}

TEST(CkaTest, SymbolicProgramTwoConsecutiveIfThen)
{
  constexpr memory::Address a_u = 0U;
  constexpr memory::Address a_v = 1U;

  constexpr memory::Byte b_u = '\1';
  constexpr memory::Byte b_v = '\2';

  memory::SymbolicProgram U{'\3'};
  memory::SymbolicProgram V{'\4'};
  memory::SymbolicProgram X{if_then(a_u, b_u, U)};
  memory::SymbolicProgram Y{if_then(a_v, b_v, V)};
  memory::SymbolicProgram Z{(X , Y)};

  EXPECT_EQ(4, Z.assume_map().size());
  EXPECT_EQ(2, Z.unguarded_events().size());
  EXPECT_EQ(4, Z.p().length());

  EXPECT_EQ(0, Z.assume_map().at(0));
  EXPECT_EQ(0, Z.assume_map().at(1));
  EXPECT_EQ(2, Z.assume_map().at(2));
  EXPECT_EQ(2, Z.assume_map().at(3));

  EXPECT_EQ(0, Z.unguarded_events().front());
  EXPECT_EQ(2, Z.unguarded_events().back());

  EXPECT_TRUE(memory::is_assume_acquire_eq(Z.p().label_function().at(0)));
  EXPECT_EQ(a_u, memory::address(Z.p().label_function().at(0)));
  EXPECT_EQ(b_u, memory::byte(Z.p().label_function().at(0)));

  EXPECT_EQ('\3', Z.p().label_function().at(1));

  EXPECT_TRUE(memory::is_assume_acquire_eq(Z.p().label_function().at(2)));
  EXPECT_EQ(a_v, memory::address(Z.p().label_function().at(2)));
  EXPECT_EQ(b_v, memory::byte(Z.p().label_function().at(2)));

  EXPECT_EQ('\4', Z.p().label_function().at(3));
}

TEST(CkaTest, SymbolicProgramNestedIfThen)
{
  constexpr memory::Address a_x = 0U;
  constexpr memory::Address a_y = 1U;

  constexpr memory::Byte b_x = '\1';
  constexpr memory::Byte b_y = '\2';

  memory::SymbolicProgram X{'\3'};
  memory::SymbolicProgram Y{if_then(a_x, b_x, X)};
  memory::SymbolicProgram Z{if_then(a_y, b_y, Y)};

  EXPECT_EQ(3, Z.assume_map().size());
  EXPECT_EQ(1, Z.unguarded_events().size());
  EXPECT_EQ(3, Z.p().length());

  EXPECT_EQ(0, Z.assume_map().at(0));
  EXPECT_EQ(0, Z.assume_map().at(1));
  EXPECT_EQ(1, Z.assume_map().at(2));

  EXPECT_EQ(0, Z.unguarded_events().front());

  EXPECT_TRUE(memory::is_assume_acquire_eq(Z.p().label_function().at(0)));
  EXPECT_EQ(a_y, memory::address(Z.p().label_function().at(0)));
  EXPECT_EQ(b_y, memory::byte(Z.p().label_function().at(0)));

  EXPECT_TRUE(memory::is_assume_acquire_eq(Z.p().label_function().at(1)));
  EXPECT_EQ(a_x, memory::address(Z.p().label_function().at(1)));
  EXPECT_EQ(b_x, memory::byte(Z.p().label_function().at(1)));

  EXPECT_EQ('\3', Z.p().label_function().at(2));
}

TEST(CkaTest, SymbolicProgramNestedConsecutiveIfThen)
{
  constexpr memory::Address a_u = 0U;
  constexpr memory::Address a_v = 1U;
  constexpr memory::Address a_w = 2U;

  constexpr memory::Byte b_u = '\1';
  constexpr memory::Byte b_v = '\2';
  constexpr memory::Byte b_w = '\3';

  memory::SymbolicProgram U{'\3'};
  memory::SymbolicProgram V{'\4'};
  memory::SymbolicProgram X{if_then(a_u, b_u, U)};
  memory::SymbolicProgram Y{if_then(a_v, b_v, V)};
  memory::SymbolicProgram Z{if_then(a_w, b_w, (X | Y))};

  EXPECT_EQ(5, Z.assume_map().size());
  EXPECT_EQ(1, Z.unguarded_events().size());
  EXPECT_EQ(5, Z.p().length());

  EXPECT_EQ(0, Z.assume_map().at(0));
  EXPECT_EQ(0, Z.assume_map().at(1));
  EXPECT_EQ(1, Z.assume_map().at(2));
  EXPECT_EQ(0, Z.assume_map().at(3));
  EXPECT_EQ(3, Z.assume_map().at(4));

  EXPECT_EQ(0, Z.unguarded_events().front());

  EXPECT_TRUE(memory::is_assume_acquire_eq(Z.p().label_function().at(0)));
  EXPECT_EQ(a_w, memory::address(Z.p().label_function().at(0)));
  EXPECT_EQ(b_w, memory::byte(Z.p().label_function().at(0)));

  EXPECT_TRUE(memory::is_assume_acquire_eq(Z.p().label_function().at(1)));
  EXPECT_EQ(a_u, memory::address(Z.p().label_function().at(1)));
  EXPECT_EQ(b_u, memory::byte(Z.p().label_function().at(1)));

  EXPECT_EQ('\3', Z.p().label_function().at(2));

  EXPECT_TRUE(memory::is_assume_acquire_eq(Z.p().label_function().at(3)));
  EXPECT_EQ(a_v, memory::address(Z.p().label_function().at(3)));
  EXPECT_EQ(b_v, memory::byte(Z.p().label_function().at(3)));

  EXPECT_EQ('\4', Z.p().label_function().at(4));
}

TEST(CkaTest, SymbolicProgramGuards)
{
  constexpr memory::Address a_u = 0U;
  constexpr memory::Address a_v = 1U;
  constexpr memory::Address a_w = 2U;

  constexpr memory::Byte b_u = '\1';
  constexpr memory::Byte b_v = '\2';
  constexpr memory::Byte b_w = '\3';

  memory::SymbolicProgram P{'\1'};
  memory::SymbolicProgram Q{'\2'};
  memory::SymbolicProgram U{'\3'};
  memory::SymbolicProgram V{'\4'};
  memory::SymbolicProgram X{if_then(a_u, b_u, U)};
  memory::SymbolicProgram Y{if_then(a_v, b_v, V)};
  memory::SymbolicProgram Z{(P , if_then(a_w, b_w, (X | Y)) , Q)};

  memory::ReleaseAcquireModel<smt::Int, smt::Int> rel_acq_model;

  EXPECT_FALSE(rel_acq_model.is_guarded(Z.assume_map(), 0));
  EXPECT_FALSE(rel_acq_model.is_guarded(Z.assume_map(), 1));

  EXPECT_TRUE(rel_acq_model.is_guarded(Z.assume_map(), 2));
  EXPECT_TRUE(rel_acq_model.is_guarded(Z.assume_map(), 3));
  EXPECT_TRUE(rel_acq_model.is_guarded(Z.assume_map(), 4));
  EXPECT_TRUE(rel_acq_model.is_guarded(Z.assume_map(), 5));

  EXPECT_FALSE(rel_acq_model.is_guarded(Z.assume_map(), 6));

  // 7 and 8 are not in `Z.p()`
  EXPECT_FALSE(rel_acq_model.is_guarded(Z.assume_map(), 7));
  EXPECT_FALSE(rel_acq_model.is_guarded(Z.assume_map(), 8));

  smt::Bool g2{rel_acq_model.guard(Z.assume_map(), Z.p(), 2)};
  smt::Bool g3{rel_acq_model.guard(Z.assume_map(), Z.p(), 3)};
  smt::Bool g4{rel_acq_model.guard(Z.assume_map(), Z.p(), 4)};
  smt::Bool g5{rel_acq_model.guard(Z.assume_map(), Z.p(), 5)};

  EXPECT_FALSE(g2.is_null());
  EXPECT_FALSE(g3.is_null());
  EXPECT_FALSE(g4.is_null());
  EXPECT_FALSE(g5.is_null());

  EXPECT_EQ(smt::BINARY_EXPR_KIND, g2.expr_kind());
  EXPECT_EQ(smt::NARY_EXPR_KIND, g3.expr_kind());
  EXPECT_EQ(smt::BINARY_EXPR_KIND, g4.expr_kind());
  EXPECT_EQ(smt::NARY_EXPR_KIND, g5.expr_kind());

  const smt::NaryExpr<smt::LAND>& n3 = static_cast<const smt::NaryExpr<smt::LAND>&>(g3.ref());
  const smt::NaryExpr<smt::LAND>& n5 = static_cast<const smt::NaryExpr<smt::LAND>&>(g5.ref());

  EXPECT_EQ(2, n3.size());
  EXPECT_EQ(2, n5.size());
}

TEST(CkaTest, SymbolicProgramRacyOnlyStores)
{
  constexpr memory::Address a = 0U;

  memory::SymbolicProgram P0{memory::none_store_label(a, '\0')};
  memory::SymbolicProgram P1{memory::none_store_label(a, '\0')};

  memory::SymbolicProgram X{(P0 | P1)};
  memory::SymbolicProgram Y{(P0 , P1)};
  memory::SymbolicProgram Z{(P1 , P0)};

  memory::DataRaceDetector drd;

  EXPECT_FALSE(drd.is_racy(P0));
  EXPECT_FALSE(drd.is_racy(P1));

  EXPECT_TRUE(drd.is_racy(X));
  EXPECT_FALSE(drd.is_racy(Y));
  EXPECT_FALSE(drd.is_racy(Z));
}

TEST(CkaTest, SymbolicProgramNonRacyOnlyReleaseStores)
{
  constexpr memory::Address a = 0U;

  memory::SymbolicProgram P0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram P1{memory::release_store_label(a, '\0')};

  memory::SymbolicProgram X{(P0 | P1)};
  memory::SymbolicProgram Y{(P0 , P1)};
  memory::SymbolicProgram Z{(P1 , P0)};

  memory::DataRaceDetector drd;

  EXPECT_FALSE(drd.is_racy(P0));
  EXPECT_FALSE(drd.is_racy(P1));

  EXPECT_FALSE(drd.is_racy(X));
  EXPECT_FALSE(drd.is_racy(Y));
  EXPECT_FALSE(drd.is_racy(Z));
}

/// At least one conflicting event must be a non-atomic write
TEST(CkaTest, SymbolicProgramNonRacyOnlyLoads)
{
  constexpr memory::Address a = 0U;

  memory::SymbolicProgram P0{memory::none_load_label(a)};
  memory::SymbolicProgram P1{memory::none_load_label(a)};

  memory::SymbolicProgram X{(P0 | P1)};
  memory::SymbolicProgram Y{(P0 , P1)};
  memory::SymbolicProgram Z{(P1 , P0)};

  memory::DataRaceDetector drd;

  EXPECT_FALSE(drd.is_racy(P0));
  EXPECT_FALSE(drd.is_racy(P1));

  EXPECT_FALSE(drd.is_racy(X));
  EXPECT_FALSE(drd.is_racy(Y));
  EXPECT_FALSE(drd.is_racy(Z));
}

TEST(CkaTest, SymbolicProgramRacyNonatomic)
{
  constexpr memory::Address a = 0U;

  memory::SymbolicProgram P0{memory::none_store_label(a, '\0')};
  memory::SymbolicProgram P1{memory::none_load_label(a)};

  memory::SymbolicProgram X{(P0 | P1)};
  memory::SymbolicProgram Y{(P0 , P1)};
  memory::SymbolicProgram Z{(P1 , P0)};

  memory::DataRaceDetector drd;

  EXPECT_TRUE(drd.is_racy(X));

  EXPECT_FALSE(drd.is_racy(Y));
  EXPECT_FALSE(drd.is_racy(Z));
}

/// Let T1 be "[a]_none := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; r1 := [a]_none;"
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramRacyMessagePassingWithInit)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_1{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T1_2{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T1{(T1_1 , T1_2)};

  memory::SymbolicProgram T2_1{memory::acquire_load_label(b)};
  memory::SymbolicProgram T2_2{memory::none_load_label(a)};
  memory::SymbolicProgram T2{(T2_1 , T2_2)};

  memory::SymbolicProgram P{(T0 , (T1 | T2))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

/// Unlike `SymbolicProgramRacyMessagePassingWithInit`, we do
/// not model the initialization of the two memory locations.
/// As expected, this hides the data race because the axiom
/// for the reading of initial values cannot be satisfied.
/// The take-away message is: always model the initialization
/// of all memory locations as illustrated by "T0" above.
TEST(CkaTest, SymbolicProgramRacyMessagePassingWithoutInit)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T1_1{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T1_2{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T1{(T1_1 , T1_2)};

  memory::SymbolicProgram T2_1{memory::acquire_load_label(b)};
  memory::SymbolicProgram T2_2{memory::none_load_label(a)};
  memory::SymbolicProgram T2{(T2_1 , T2_2)};

  memory::SymbolicProgram P{(T1 | T2)};

  memory::DataRaceDetector drd;

  // There is a race but it does not satisfy the axiom that every
  // load happens-before a store to the same memory address.
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "[a]_release := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; r1 := [a]_acquire;"
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramNonRacyMessagePassingUsingAlwaysReleaseAcquire)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_1{memory::release_store_label(a, '\1')}; //none_store_label still race free, correct?
  memory::SymbolicProgram T1_2{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T1{(T1_1 , T1_2)};

  memory::SymbolicProgram T2_1{memory::acquire_load_label(b)};
  memory::SymbolicProgram T2_2{memory::acquire_load_label(a)};
  memory::SymbolicProgram T2{(T2_1 , T2_2)};

  memory::SymbolicProgram P{(T0 , (T1 | T2))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end"
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramNonRacyMessagePassingUsingBranch)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_1{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T1_2{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T1{(T1_1 , T1_2)};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_load_from_a)};

  memory::SymbolicProgram P{(T0 , (T1 | T2))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T1' be "[a]_none := 1; [b]_release := 1"
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramRacyDespiteBranch)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_1{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T1_2{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T1{(T1_1 , T1_2)};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_load_from_a)};

  memory::SymbolicProgram T3_1{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T3_2{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T3{(T3_1 | T3_2)};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

/// Let T1 be "r0 := [b]_acquire; [a]_none := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T1' be "r0 := [b]_acquire; [a]_none := 1; [b]_release := 1"
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramRacyDespiteBranchAndReleaseAcquireWrap)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_0{memory::acquire_load_label(b)};
  memory::SymbolicProgram T1_1{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T1_2{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T1{(T1_0 , T1_1 , T1_2)};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_load_from_a)};

  memory::SymbolicProgram T3_0{memory::acquire_load_label(b)};
  memory::SymbolicProgram T3_1{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T3_2{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T3{(T3_0 | T3_1 | T3_2)};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

/// Let T1 be "r0 := [b]_acquire; [a]_none := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 0) then r1 := [a]_none; end",
/// and T1' be "r0 := [b]_acquire; if (r0 == 1) then [a]_none := 2; [b]_release := 2; end"
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramRacyDespiteBranchCheckZero)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_0{memory::acquire_load_label(b)};
  memory::SymbolicProgram T1_1{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T1_2{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T1{(T1_0 , T1_1 , T1_2)};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\0', T2_none_load_from_a)};

  memory::SymbolicProgram T3_none_store_to_a{memory::none_store_label(a, '\2')};
  memory::SymbolicProgram T3_release_store_to_b{memory::release_store_label(b, '\2')};
  memory::SymbolicProgram T3{if_then(b, '\1',
    (T3_none_store_to_a , T3_release_store_to_b))};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

/// Let T1 be "r0 := [b]_acquire; [a]_none := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T1' be "r0 := [b]_acquire; if (r0 == 1) then [a]_none := 2; [b]_release := 2; end"
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramRacyDespiteBranchCheckOne)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_0{memory::acquire_load_label(b)};
  memory::SymbolicProgram T1_1{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T1_2{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T1{(T1_0 , T1_1 , T1_2)};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_load_from_a)};

  memory::SymbolicProgram T3_none_store_to_a{memory::none_store_label(a, '\2')};
  memory::SymbolicProgram T3_release_store_to_b{memory::release_store_label(b, '\2')};
  memory::SymbolicProgram T3{if_then(b, '\1',
    (T3_none_store_to_a , T3_release_store_to_b))};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

/// Let T1 be "r0 := [b]_acquire; [a]_none := 1; [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T1' be "r0 := [b]_acquire; if (r0 == 1) then [b]_release := 2; end"
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramNonRacyBranchCheckOne)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_0{memory::acquire_load_label(b)};
  memory::SymbolicProgram T1_1{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T1_2{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T1{(T1_0 , T1_1 , T1_2)};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_load_from_a)};

  memory::SymbolicProgram T3_release_store_to_b{memory::release_store_label(b, '\2')};
  memory::SymbolicProgram T3{if_then(b, '\1', T3_release_store_to_b)};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramInfeasibleDataRace)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1{memory::none_store_label(a, '\1')};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_load_from_a)};

  memory::SymbolicProgram P{(T0 , (T1 | T2))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 0) then r1 := [a]_none; end",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramFeasibleDataRace)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1{memory::none_store_label(a, '\1')};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\0', T2_none_load_from_a)};

  memory::SymbolicProgram P{(T0 , (T1 | T2))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end ; r1 := [a]_none",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramFeasibleDataRaceDespiteInfeasibleBranch)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1{memory::none_store_label(a, '\1')};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{(if_then(b, '\1', T2_none_load_from_a) , T2_none_load_from_a)};

  memory::SymbolicProgram P{(T0 , (T1 | T2))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [b]_acquire; if (r1 == 0) then r2 := [a]_none; end end",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramInfeasibleDataRaceBecauseOutermostBranchInfeasible)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1{memory::none_store_label(a, '\1')};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2_innermost_if{if_then(b, '\0', T2_none_load_from_a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_innermost_if)};

  memory::SymbolicProgram P{(T0 , (T1 | T2))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 0) then r1 := [b]_acquire; if (r1 == 1) then r2 := [a]_none; end end",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramInfeasibleDataRaceBecauseInnermostBranchInfeasible)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1{memory::none_store_label(a, '\1')};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2_innermost_if{if_then(b, '\1', T2_none_load_from_a)};
  memory::SymbolicProgram T2{if_then(b, '\0', T2_innermost_if)};

  memory::SymbolicProgram P{(T0 , (T1 | T2))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1 | [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 0) then r1 := [b]_acquire; if (r1 == 1) then r2 := [a]_none; end end",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramFeasibleDataRaceWithNestedBranches)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_0{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T1_1{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T1{(T1_0 | T1_1)};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2_innermost_if{if_then(b, '\1', T2_none_load_from_a)};
  memory::SymbolicProgram T2{if_then(b, '\0', T2_innermost_if)};

  memory::SymbolicProgram P{(T0 , (T1 | T2))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1 , [b]_release := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 0) then r1 := [b]_acquire; if (r1 == 1) then r2 := [a]_none; end end",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramAvoidedDataRaceWithNestedBranches)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_0{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T1_1{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T1{(T1_0 , T1_1)};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2_innermost_if{if_then(b, '\1', T2_none_load_from_a)};
  memory::SymbolicProgram T2{if_then(b, '\0', T2_innermost_if)};

  memory::SymbolicProgram P{(T0 , (T1 | T2))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 0) then r1 := [b]_acquire; if (r1 == 1) then r2 := [a]_none; end end",
/// where both memory locations "a" is initially zero, and
/// "b" is nondeterministically initialized to 0 or 1.
TEST(CkaTest, SymbolicProgramInfeasibleDataRaceDueToContradictoryBranches)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0_2{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T0{(T0_0 | T0_1 | T0_2)};

  memory::SymbolicProgram T1{memory::none_store_label(a, '\1')};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2_innermost_if{if_then(b, '\1', T2_none_load_from_a)};
  memory::SymbolicProgram T2{if_then(b, '\0', T2_innermost_if)};

  memory::SymbolicProgram P{(T0 , (T1 | T2))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 0) then r1 := [b]_acquire; if (r1 == 1) then r2 := [a]_none; end end",
/// where both memory locations "a" is initially zero, and
/// "b" is nondeterministically initialized to 0 or 1.
TEST(CkaTest, SymbolicProgramFeasibleDataRaceNestedBranches)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0_2{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T0{(T0_0 | T0_1 | T0_2)};

  memory::SymbolicProgram T1{memory::none_store_label(a, '\1')};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2_innermost_if{if_then(b, '\1', T2_none_load_from_a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_innermost_if)};

  memory::SymbolicProgram P{(T0 , (T1 | T2))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T3 be "r0 := [b]_acquire; if (r0 == 2) then [b]_release := 1; end",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramInfeasibleDataRaceDueToTrivialInfeasibleReleaseStore)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1{memory::none_store_label(a, '\1')};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_load_from_a)};

  memory::SymbolicProgram T3_release_store_to_b{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T3{if_then(b, '\2', T3_release_store_to_b)};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "r0 := [b]_acquire; if (r0 == 2) then [a]_none := 1; end",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T3 be "r0 := [b]_acquire; if (r0 == 0) then [b]_release := 1; end",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramInfeasibleDataRaceDueToTrivialInfeasibleNoneStore)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_none_store_to_a{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T1{if_then(b, '\2', T1_none_store_to_a)};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_load_from_a)};

  memory::SymbolicProgram T3_release_store_to_b{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T3{if_then(b, '\0', T3_release_store_to_b)};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T3 be "r0 := [b]_acquire; if (r0 == 2) then [b]_release := 1; end",
/// and T4 be "[b]_release := 2",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramInfeasibleDataRaceDueToInfeasibleReleaseStore)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1{memory::none_store_label(a, '\1')};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_load_from_a)};

  memory::SymbolicProgram T3_release_store_to_b{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T3{if_then(b, '\2', T3_release_store_to_b)};

  memory::SymbolicProgram T4{memory::release_store_label(b, '\2')};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3) , T4)};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "r0 := [b]_acquire; if (r0 == 2) then [a]_none := 1; end",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T3 be "r0 := [b]_acquire; if (r0 == 0) then [b]_release := 1; end",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramInfeasibleDataRaceDueToInfeasibleNoneStore)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_none_store_to_a{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T1{if_then(b, '\2', T1_none_store_to_a)};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_load_from_a)};

  memory::SymbolicProgram T3_release_store_to_b{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T3{if_then(b, '\0', T3_release_store_to_b)};

  memory::SymbolicProgram T4{memory::release_store_label(b, '\2')};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3) , T4)};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "r0 := [b]_acquire; if (r0 == 0) then r1 := [a]_none; end",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then [a]_none := 1; end",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramInfeasibleDataRaceInFeasibleNoneLoadButInfeasibleNoneStore)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T1{if_then(b, '\0', T1_none_load_from_a)};

  memory::SymbolicProgram T2_none_store_to_a{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_store_to_a)};

  memory::SymbolicProgram P{(T0 , (T1 | T2))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "r0 := [b]_acquire; if (r0 == 0) then r1 := [a]_none; end",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [b]_acquire; if (r1 == 0) then [a]_none := 1; end end",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramInfeasibleDataRaceDueToNestedInfeasibleNoneStore)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T1{if_then(b, '\0', T1_none_load_from_a)};

  memory::SymbolicProgram T2_none_store_to_a{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T2_innermost_if{if_then(b, '\0', T2_none_store_to_a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_innermost_if)};

  memory::SymbolicProgram P{(T0 , (T1 | T2))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T3 be "r0 := [b]_acquire; if (r0 == 2) then [b]_release := 1; end",
/// and T4 be "[b]_release := 2",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramFeasibleDataRaceWithNestedReleaseStore)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1{memory::none_store_label(a, '\1')};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_load_from_a)};

  memory::SymbolicProgram T3_release_store_to_b{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T3{if_then(b, '\2', T3_release_store_to_b)};

  memory::SymbolicProgram T4{memory::release_store_label(b, '\2')};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3 | T4))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

/// Let T1 be "r0 := [b]_acquire; if (r0 == 2) then [a]_none := 1; end",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T3 be "r0 := [b]_acquire; if (r0 == 0) then [b]_release := 1; end",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramFeasibleDataWithNestedNoneStore)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_none_store_to_a{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T1{if_then(b, '\2', T1_none_store_to_a)};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_load_from_a)};

  memory::SymbolicProgram T3_release_store_to_b{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T3{if_then(b, '\0', T3_release_store_to_b)};

  memory::SymbolicProgram T4{memory::release_store_label(b, '\2')};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3 | T4))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

/// Let T1 be "r0 := [b]_acquire; if (r0 == 1) then [a]_none := 1; end",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T3 be "r0 := [b]_acquire; if (r0 == 0) then [b]_release := 1; end",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramInfeasibleDataRaceSameBranchCondition)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1_none_store_to_a{memory::none_store_label(a, '\1')};
  memory::SymbolicProgram T1{if_then(b, '\1', T1_none_store_to_a)};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_load_from_a)};

  memory::SymbolicProgram T3_release_store_to_b{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T3{if_then(b, '\0', T3_release_store_to_b)};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; end",
/// and T3 be "r0 := [b]_acquire; if (r0 == 0) then [b]_release := 1; end",
/// where both memory locations "a" and "b" are initially zero.
TEST(CkaTest, SymbolicProgramFeasibleDataRaceAfterGuardedStore)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1)};

  memory::SymbolicProgram T1{memory::none_store_label(a, '\1')};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2{if_then(b, '\1', T2_none_load_from_a)};

  memory::SymbolicProgram T3_release_store_to_b{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T3{if_then(b, '\0', T3_release_store_to_b)};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; [c]_release := 1 end",
/// and T3 be "r0 := [c]_acquire; if (r0 == 1) then [b]_release := 1; end",
/// where both memory locations "a", "b" and "c" are initially zero.
TEST(CkaTest, SymbolicProgramInfeasibleDataRaceBecauseCausalityIsCyclic)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0_2{memory::release_store_label(c, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1 | T0_2)};

  memory::SymbolicProgram T1{memory::none_store_label(a, '\1')};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2_release_store_to_c{memory::release_store_label(c, '\1')};
  memory::SymbolicProgram T2{if_then(b, '\1', (T2_none_load_from_a , T2_release_store_to_c))};

  memory::SymbolicProgram T3_release_store_to_b{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T3{if_then(c, '\1', T3_release_store_to_b)};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

/// Let T1 be "[a]_none := 1",
/// and T2 be "r0 := [b]_acquire; if (r0 == 1) then r1 := [a]_none; [c]_release := 1 end",
/// and T3 be "r0 := [c]_acquire; if (r0 == 0) then [b]_release := 1; end",
/// where both memory locations "a", "b" and "c" are initially zero.
TEST(CkaTest, SymbolicProgramFeasibleDataRaceDueToAcyclicCausality)
{
  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;

  memory::SymbolicProgram T0_0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram T0_2{memory::release_store_label(c, '\0')};
  memory::SymbolicProgram T0{(T0_0 | T0_1 | T0_2)};

  memory::SymbolicProgram T1{memory::none_store_label(a, '\1')};

  memory::SymbolicProgram T2_none_load_from_a{memory::none_load_label(a)};
  memory::SymbolicProgram T2_release_store_to_c{memory::release_store_label(c, '\1')};
  memory::SymbolicProgram T2{if_then(b, '\1', (T2_none_load_from_a , T2_release_store_to_c))};

  memory::SymbolicProgram T3_release_store_to_b{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram T3{if_then(c, '\0', T3_release_store_to_b)};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T3))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

TEST(CkaTest, SymbolicProgramInfeasibleDataRaceNestedContradictoryIfThenDueToModificationOrder)
{
  constexpr memory::Address bait = 0U;
  constexpr memory::Address v0 = 1U;
  constexpr memory::Address c0 = 2U;
  constexpr memory::Address c1 = 3U;

  memory::SymbolicProgram T0_0{memory::release_store_label(v0, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(v0, '\1')};

  memory::SymbolicProgram T0_2{memory::release_store_label(bait, '\0')};
  memory::SymbolicProgram T0_3{memory::release_store_label(c0, '\0')};
  memory::SymbolicProgram T0_4{memory::release_store_label(c1, '\0')};

  memory::SymbolicProgram T0{(T0_0 | T0_1 | T0_2 | T0_3 | T0_4)};

  memory::SymbolicProgram T1_release_store_to_c0{memory::release_store_label(c0, '\1')};
  memory::SymbolicProgram T1{if_then(v0, '\0', T1_release_store_to_c0)};

  memory::SymbolicProgram T2_release_store_to_c1{memory::release_store_label(c1, '\1')};
  memory::SymbolicProgram T2{if_then(v0, '\1', T2_release_store_to_c1)};

  memory::SymbolicProgram T3_none_store_to_bait{memory::none_store_label(bait, '\0')};
  memory::SymbolicProgram T3_innermost_if{if_then(c1, '\1', T3_none_store_to_bait)};
  memory::SymbolicProgram T3{if_then(c0, '\1', T3_innermost_if)};

  memory::SymbolicProgram T4{memory::none_store_label(bait, '\1')};

  memory::SymbolicProgram P{(T0 , (T1 | T2) , (T3 | T4))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

TEST(CkaTest, SymbolicProgramFeasibleDataRaceEvenThoughModificationOrder)
{
  constexpr memory::Address bait = 0U;
  constexpr memory::Address v0 = 1U;
  constexpr memory::Address c0 = 2U;
  constexpr memory::Address c1 = 3U;

  memory::SymbolicProgram T0_0{memory::release_store_label(v0, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(v0, '\1')};

  memory::SymbolicProgram T0_2{memory::release_store_label(bait, '\0')};
  memory::SymbolicProgram T0_3{memory::release_store_label(c0, '\0')};
  memory::SymbolicProgram T0_4{memory::release_store_label(c1, '\0')};

  memory::SymbolicProgram T0{(T0_0 | T0_1 | T0_2 | T0_3 | T0_4)};

  memory::SymbolicProgram T1_release_store_to_c0{memory::release_store_label(c0, '\1')};
  memory::SymbolicProgram T1{if_then(v0, '\1', T1_release_store_to_c0)};

  memory::SymbolicProgram T2_release_store_to_c1{memory::release_store_label(c1, '\1')};
  memory::SymbolicProgram T2{if_then(v0, '\1', T2_release_store_to_c1)};

  memory::SymbolicProgram T3_none_store_to_bait{memory::none_store_label(bait, '\0')};
  memory::SymbolicProgram T3_innermost_if{if_then(c1, '\1', T3_none_store_to_bait)};
  memory::SymbolicProgram T3{if_then(c0, '\1', T3_innermost_if)};

  memory::SymbolicProgram T4{memory::none_store_label(bait, '\1')};

  memory::SymbolicProgram P{(T0 , (T1 | T2) , (T3 | T4))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

TEST(CkaTest, SymbolicProgramInfeasibleDataRaceNestedContradictoryIfThenDueToModificationOrderExtra)
{
  constexpr memory::Address bait = 0U;
  constexpr memory::Address v0 = 1U;
  constexpr memory::Address v1 = 2U;
  constexpr memory::Address c0 = 3U;
  constexpr memory::Address c1 = 4U;
  constexpr memory::Address c2 = 5U;

  memory::SymbolicProgram T0_0{memory::release_store_label(v0, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(v0, '\1')};

  memory::SymbolicProgram T0_0_prime{memory::release_store_label(v1, '\0')};
  memory::SymbolicProgram T0_1_prime{memory::release_store_label(v1, '\1')};

  memory::SymbolicProgram T0_2{memory::release_store_label(bait, '\0')};
  memory::SymbolicProgram T0_3{memory::release_store_label(c0, '\0')};
  memory::SymbolicProgram T0_4{memory::release_store_label(c1, '\0')};
  memory::SymbolicProgram T0_5{memory::release_store_label(c2, '\0')};

  memory::SymbolicProgram T0{(T0_0 | T0_1 | T0_0_prime | T0_1_prime | T0_2 | T0_3 | T0_4 | T0_5)};

  memory::SymbolicProgram T1_release_store_to_c0{memory::release_store_label(c0, '\1')};
  memory::SymbolicProgram T1{if_then(v0, '\0', T1_release_store_to_c0)};

  memory::SymbolicProgram T2_release_store_to_c1{memory::release_store_label(c1, '\1')};
  memory::SymbolicProgram T2{if_then(v0, '\1', T2_release_store_to_c1)};

  memory::SymbolicProgram T2_prime_release_store_to_c2{memory::release_store_label(c2, '\1')};
  memory::SymbolicProgram T2_prime{if_then(v1, '\1', T2_prime_release_store_to_c2)};

  memory::SymbolicProgram T3_none_store_to_bait{memory::none_store_label(bait, '\0')};
  memory::SymbolicProgram T3_innermost_if{if_then(c1, '\1', T3_none_store_to_bait)};
  memory::SymbolicProgram T3_inner_if{if_then(c0, '\1', T3_innermost_if)};
  memory::SymbolicProgram T3{if_then(c2, '\1', T3_inner_if)};

  memory::SymbolicProgram T4{memory::none_store_label(bait, '\1')};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T2_prime) , (T3 | T4))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}

TEST(CkaTest, SymbolicProgramFeasibleDataRaceEvenThoughModificationOrderExtra)
{
  constexpr memory::Address bait = 0U;
  constexpr memory::Address v0 = 1U;
  constexpr memory::Address v1 = 2U;
  constexpr memory::Address c0 = 3U;
  constexpr memory::Address c1 = 4U;
  constexpr memory::Address c2 = 5U;

  memory::SymbolicProgram T0_0{memory::release_store_label(v0, '\0')};
  memory::SymbolicProgram T0_1{memory::release_store_label(v0, '\1')};

  memory::SymbolicProgram T0_0_prime{memory::release_store_label(v1, '\0')};
  memory::SymbolicProgram T0_1_prime{memory::release_store_label(v1, '\1')};

  memory::SymbolicProgram T0_2{memory::release_store_label(bait, '\0')};
  memory::SymbolicProgram T0_3{memory::release_store_label(c0, '\0')};
  memory::SymbolicProgram T0_4{memory::release_store_label(c1, '\0')};
  memory::SymbolicProgram T0_5{memory::release_store_label(c2, '\0')};

  memory::SymbolicProgram T0{(T0_0 | T0_1 | T0_0_prime | T0_1_prime | T0_2 | T0_3 | T0_4 | T0_5)};

  memory::SymbolicProgram T1_release_store_to_c0{memory::release_store_label(c0, '\1')};
  memory::SymbolicProgram T1{if_then(v0, '\1', T1_release_store_to_c0)};

  memory::SymbolicProgram T2_release_store_to_c1{memory::release_store_label(c1, '\1')};
  memory::SymbolicProgram T2{if_then(v0, '\1', T2_release_store_to_c1)};

  memory::SymbolicProgram T2_prime_release_store_to_c2{memory::release_store_label(c2, '\1')};
  memory::SymbolicProgram T2_prime{if_then(v1, '\1', T2_prime_release_store_to_c2)};

  memory::SymbolicProgram T3_none_store_to_bait{memory::none_store_label(bait, '\0')};
  memory::SymbolicProgram T3_innermost_if{if_then(c1, '\1', T3_none_store_to_bait)};
  memory::SymbolicProgram T3_inner_if{if_then(c0, '\1', T3_innermost_if)};
  memory::SymbolicProgram T3{if_then(c2, '\1', T3_inner_if)};

  memory::SymbolicProgram T4{memory::none_store_label(bait, '\1')};

  memory::SymbolicProgram P{(T0 , (T1 | T2 | T2_prime) , (T3 | T4))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

/// Suppose we want to decide the satisfiability of the following
/// 3SAT formula where x, y and z are Boolean variables:
///
///   (x or ~y or z) and (~x or ~y or ~z) and (~x or ~y or z)
///
/// For example, a satisfying assignment is:
///
///   x=1, y=0, z=1
///
/// In contrast, the following 3SAT formula is unsatisfiable:
///
///   (x or ~y or ~z) and (z) and (~z)
///
/// More generally, we can encode any 3SAT formula into a symbolic program.
/// This proves that data race detection is NP-hard.
TEST(CkaTest, SymbolicProgramFeasibleDataRaceWithReductionFrom3SAT)
{
  // verify assumptions
  {
    smt::Bool x{smt::any<smt::Bool>("x")};
    smt::Bool y{smt::any<smt::Bool>("y")};
    smt::Bool z{smt::any<smt::Bool>("z")};

    smt::Z3Solver solver;

    solver.add((x or not y or z) and (not x or not y or not z) and (not x or not y or z));
    EXPECT_EQ(smt::sat, solver.check());
  }

  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;

  constexpr memory::Address clause0 = 3U;
  constexpr memory::Address clause1 = 4U;
  constexpr memory::Address clause2 = 5U;

  // data race bait
  constexpr memory::Address bait = 6U;

  memory::SymbolicProgram Init_clause0{memory::release_store_label(clause0, '\0')};
  memory::SymbolicProgram Init_clause1{memory::release_store_label(clause1, '\0')};
  memory::SymbolicProgram Init_clause2{memory::release_store_label(clause2, '\0')};

  memory::SymbolicProgram Init_a0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram Init_b0{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram Init_c0{memory::release_store_label(c, '\0')};

  memory::SymbolicProgram Init_a1{memory::release_store_label(a, '\1')};
  memory::SymbolicProgram Init_b1{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram Init_c1{memory::release_store_label(c, '\1')};

  memory::SymbolicProgram Init_bait{memory::release_store_label(bait, '\0')};

  memory::SymbolicProgram Init{(Init_clause0 | Init_clause1 | Init_clause2
                             | Init_a0 | Init_b0 | Init_c0 | Init_a1 | Init_b1 | Init_c1
                             | Init_bait)};

  memory::SymbolicProgram release_store_to_clause0{memory::release_store_label(clause0, '\1')};
  memory::SymbolicProgram release_store_to_clause1{memory::release_store_label(clause1, '\1')};
  memory::SymbolicProgram release_store_to_clause2{memory::release_store_label(clause2, '\1')};

  memory::SymbolicProgram Clause0_a{if_then(a, '\1', release_store_to_clause0)};
  memory::SymbolicProgram Clause0_b{if_then(b, '\0', release_store_to_clause0)};
  memory::SymbolicProgram Clause0_c{if_then(c, '\1', release_store_to_clause0)};

  memory::SymbolicProgram Clause1_a{if_then(a, '\0', release_store_to_clause1)};
  memory::SymbolicProgram Clause1_b{if_then(b, '\0', release_store_to_clause1)};
  memory::SymbolicProgram Clause1_c{if_then(c, '\0', release_store_to_clause1)};

  memory::SymbolicProgram Clause2_a{if_then(a, '\0', release_store_to_clause2)};
  memory::SymbolicProgram Clause2_b{if_then(b, '\0', release_store_to_clause2)};
  memory::SymbolicProgram Clause2_c{if_then(c, '\1', release_store_to_clause2)};

  memory::SymbolicProgram Choices{(Clause0_a | Clause0_b | Clause0_c
                                  | Clause1_a | Clause1_b | Clause1_c
                                  | Clause2_a | Clause2_b | Clause2_c)};

  memory::SymbolicProgram none_store_to_bait{memory::none_store_label(bait, '\1')};

  // nesting of if-then statements to encode conjunction of clauses
  memory::SymbolicProgram Checker_clause2{if_then(clause2, '\1', none_store_to_bait)};
  memory::SymbolicProgram Checker_clause1{if_then(clause1, '\1', Checker_clause2)};
  memory::SymbolicProgram Checker_clause0{if_then(clause0, '\1', Checker_clause1)};
  memory::SymbolicProgram Checker{Checker_clause0};

  memory::SymbolicProgram P{(Init , Choices , (Checker | none_store_to_bait))};

  memory::DataRaceDetector drd;
  EXPECT_TRUE(drd.is_racy(P));
}

TEST(CkaTest, SymbolicProgramInfeasibleDataRaceWithReductionFrom3SAT)
{
  // verify assumptions
  {
    smt::Bool x{smt::any<smt::Bool>("x")};
    smt::Bool y{smt::any<smt::Bool>("y")};
    smt::Bool z{smt::any<smt::Bool>("z")};

    smt::Z3Solver solver;

    solver.add((x or not y or not z) and z and not z);
    EXPECT_EQ(smt::unsat, solver.check());
  }

  constexpr memory::Address a = 0U;
  constexpr memory::Address b = 1U;
  constexpr memory::Address c = 2U;

  constexpr memory::Address clause0 = 3U;
  constexpr memory::Address clause1 = 4U;
  constexpr memory::Address clause2 = 5U;

  // data race bait
  constexpr memory::Address bait = 6U;

  memory::SymbolicProgram Init_clause0{memory::release_store_label(clause0, '\0')};
  memory::SymbolicProgram Init_clause1{memory::release_store_label(clause1, '\0')};
  memory::SymbolicProgram Init_clause2{memory::release_store_label(clause2, '\0')};

  memory::SymbolicProgram Init_a0{memory::release_store_label(a, '\0')};
  memory::SymbolicProgram Init_b0{memory::release_store_label(b, '\0')};
  memory::SymbolicProgram Init_c0{memory::release_store_label(c, '\0')};

  memory::SymbolicProgram Init_a1{memory::release_store_label(a, '\1')};
  memory::SymbolicProgram Init_b1{memory::release_store_label(b, '\1')};
  memory::SymbolicProgram Init_c1{memory::release_store_label(c, '\1')};

  memory::SymbolicProgram Init_bait{memory::release_store_label(bait, '\0')};

  memory::SymbolicProgram Init{(Init_clause0 | Init_clause1 | Init_clause2
                             | Init_a0 | Init_b0 | Init_c0 | Init_a1 | Init_b1 | Init_c1
                             | Init_bait)};

  memory::SymbolicProgram release_store_to_clause0{memory::release_store_label(clause0, '\1')};
  memory::SymbolicProgram release_store_to_clause1{memory::release_store_label(clause1, '\1')};
  memory::SymbolicProgram release_store_to_clause2{memory::release_store_label(clause2, '\1')};

  memory::SymbolicProgram Clause0_a{if_then(a, '\1', release_store_to_clause0)};
  memory::SymbolicProgram Clause0_b{if_then(b, '\0', release_store_to_clause0)};
  memory::SymbolicProgram Clause0_c{if_then(c, '\0', release_store_to_clause0)};

  memory::SymbolicProgram Clause1_c{if_then(c, '\1', release_store_to_clause1)};

  memory::SymbolicProgram Clause2_c{if_then(c, '\0', release_store_to_clause2)};

  memory::SymbolicProgram Choices{(Clause0_a | Clause0_b | Clause0_c
                                  | Clause1_c
                                  | Clause2_c)};

  memory::SymbolicProgram none_store_to_bait{memory::none_store_label(bait, '\1')};

  // nesting of if-then statements to encode conjunction of clauses
  memory::SymbolicProgram Checker_clause2{if_then(clause2, '\1', none_store_to_bait)};
  memory::SymbolicProgram Checker_clause1{if_then(clause1, '\1', Checker_clause2)};
  memory::SymbolicProgram Checker{if_then(clause0, '\1', Checker_clause1)};

  memory::SymbolicProgram P{(Init , Choices , (Checker | none_store_to_bait))};

  memory::DataRaceDetector drd;
  EXPECT_FALSE(drd.is_racy(P));
}
