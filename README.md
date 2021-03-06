# SMT Kit

SMT Kit is a C++11 library for many-sorted logics. It particularly
targets quantifier-free SMT-LIB 2.0 formulas. Currently, SMT Kit
supports CVC4, MathSAT5 and Z3.

## API Usage

To use SMT Kit, `#include <smt>`. Here's an API usage example that
encodes and checks De Morgan's law:

```C++
// Symbols (e.g. "x") must be globally unique!
smt::Bool x = smt::any<smt::Bool>("x");
smt::Bool y = smt::any<smt::Bool>("y");
smt::Bool lhs = !(x && y);
smt::Bool rhs = !x || !y;

// Other solvers include smt::MsatSolver and smt::Z3Solver 
smt::CVC4Solver solver;
solver.add(lhs != rhs);
assert(smt::unsat == solver.check());
```

The compiler will check the arguments of SMT functions at compile-time.
For example, the above example will not compile if `x` is a `smt::Int`.
However, these compile-time checks do not apply to logic signatures.
For example, when `smt::QF_BV_LOGIC` is specified for the solver but
an `smt::Array` is also used, there won't be a compile-time error.

Several more examples including incremental solving, function applications
and array logic expressions can be found in the [functional tests][api].

[api]: https://github.com/ahorn/smt-kit/blob/master/test/smt_functional_test.cpp

## Installation

For SMT Kit to work, [CVC4][cvc4], [MathSAT5][msat] and [Z3][z3] must be installed
separately. Read their software licences carefully. The SMT solver's installation
instructions can be found in the `solvers` [directory][solvers].

To build SMT Kit on a (mostly) POSIX-compliant operating system,
execute the following commands from the `smt-kit` directory:

    $ ./autogen.sh
    $ ./configure
    $ make
    $ make test
    $ make install

If `./configure` fails, you may have to set environment variables. For example,
to compile on OS X with clang++ use the command `./configure CXX=clang++`.
But see also the troubleshooting section below.

If `make test` fails, you can still install, but it is likely that some
features of this library will not work correctly on your system.
Proceed at your own risk.

Note that `make install` may require superuser privileges.

For advanced usage information on other configure options refer to the
[Autoconf documentation][autoconf].

[autoconf]: http://www.gnu.org/software/autoconf/
[cvc4]: http://cvc4.cs.nyu.edu/
[msat]: http://mathsat.fbk.eu/
[z3]: http://z3.codeplex.com/
[solvers]: https://github.com/ahorn/smt-kit/tree/master/solvers

## Native Symbolic Execution 

As a practical application, SMT Kit is a component in CRV, a research
tool for symbolic reasoning about sequential and concurrent C++11 code
using a new technique called <em>native symbolic execution</em>.

In a nutshell, CRV aims to combine a new lightweight [KLEE][klee]-style
symbolic execution technique with existing decision procedures for
partial orders. The partial orders are used to semi-automatically
find concurrency bugs and this sets CRV apart from say [KLEE][klee].

Furthermore, unlike tools such as [KLEE][klee] or [CBMC][cbmc], CRV
is designed to be a library that must be first linked with a given
program under scrutiny. This design allows CRV to leverage advanced C++11
template and meta-programming features to extract accurate
compile-time information about types as well as achieve good
runtime performance during symbolic execution.

But currently not all programming constructs are supported. For example,
pointer arithmetic is still unsupported. If you are nevertheless
interested in trying out CRV, there is a [Clang front-end][clang-crv]
to make the required source-to-source transformations easier.

[performance-tests]: https://github.com/ahorn/smt-kit/blob/master/test/crv_performance_test.cpp
[clang-crv]: https://github.com/ahorn/clang-crv
[klee]: http://klee.github.io/klee/
[cbmc]: http://www.cprover.org/cbmc/

## Troubleshooting

Since SMT Kit uses advanced C++11 language features, older compiler
versions are likely to be troublesome. To date, we have successfully
compiled and tested the code on OS X with g++ 4.8.2 and clang++ 4.2.
You should also always use the most recent version of the SMT solvers.

If `make test` fails with an error that indicates that `libstdc++.so.6`
or a specific version of `GLIBCXX` cannot be found, then check out GCC's
[FAQ][libstdcxx-faq].

[libstdcxx-faq]: http://gcc.gnu.org/onlinedocs/libstdc++/faq.html#faq.how_to_set_paths

## Bug Reports

You are warmly invited to submit patches as Github pull request,
formatted according to the existing coding style.
