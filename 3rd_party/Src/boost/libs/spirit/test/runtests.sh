#! /bin/sh

# Spirit.Core.Kernel
./match_tests || exit 1

# Spirit.Core.Scanner
./scanner_tests || exit 1

# Spirit.Core.Primitive
./primitives_tests || exit 1
./numerics_tests || exit 1

# Spirit.Core.Meta
./fundamental_tests || exit 1
./parser_traits_tests || exit 1
./traverse_tests || exit 1

# Spirit.Core.Composite
./composite_tests || exit 1
./epsilon_tests || exit 1
./actions_tests || exit 1
./operators_tests || exit 1
./directives_tests || exit 1

# Spirit.Core.Non_Terminal
./rule_tests || exit 1
./grammar_tests || exit 1
./subrule_tests || exit 1
./owi_st_tests || exit 1
./owi_mt_tests || exit 1

# Spirit.Attribute
./closure_tests || exit 1
./parametric_tests || exit 1

# Spirit.Error_Handling
./exception_tests || exit 1

# Spirit.Dynamic
./if_tests || exit 1
./for_tests || exit 1
./while_tests || exit 1

# Spirit.Utility
./chset_tests || exit 1
./loops_tests || exit 1
./symbols_tests || exit 1
./escape_char_parser_tests || exit 1
./scoped_lock_tests || exit 1

# Spirit.Iterator
./fixed_size_queue_tests || exit 1
./multi_pass_tests || exit 1
