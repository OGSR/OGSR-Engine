#!/bin/sh

Exit()
{
    echo "Failure!"
    exit $1
}

for x in *.lex; do
    NAME=`basename $x .lex`
    FLIST=`ls $NAME-*.in 2>/dev/null`
    for y in $FLIST; do
        IN=$y
        OUT=`basename $y .in`.out
        echo "Testing $x against $IN, diff against $OUT"
        ../lextest $x $IN | diff -upN - $OUT || Exit 1
    done
done
    
echo "All tests pass"
