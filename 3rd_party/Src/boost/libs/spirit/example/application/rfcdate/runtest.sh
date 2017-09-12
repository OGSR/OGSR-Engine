TESTS="timestamp_compile_test rfcdate_compile_test rfcskipper_compile_test skipper_test rfcdate_test"

echo -n Running regression tests:
for n in $TESTS; do
    echo -n " $n";
    ./$n >/dev/null || exit 1;
done
echo

