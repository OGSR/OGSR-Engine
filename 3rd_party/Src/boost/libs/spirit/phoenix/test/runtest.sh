#!/bin/sh
#./test1 << EOS || exit 1
#123321
#EOS
./binary_tests || exit 1
./binders_tests || exit 1
./functors_tests || exit 1
./iostream_tests << EOS || exit 1
123321
EOS
./mixed_binary_tests || exit 1
./more_expressions_tests || exit 1
./primitives_tests || exit 1
./statements_tests || exit 1
./stl_tests || exit 1
./tuples_tests || exit 1
./unary_tests || exit 1
