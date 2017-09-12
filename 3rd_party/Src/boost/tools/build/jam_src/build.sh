#!/bin/sh
# Copyrigt (C) 2002-2003 Rene Rivera.
# Permission to copy, use, modify, sell and distribute this software
# is granted provided this copyright notice appears in all copies.
# This software is provided "as is" without express or implied
# warranty, and with no claim as to its suitability for any purpose.

# Reset the toolset.
BOOST_JAM_TOOLSET=

# Run a command, and echo before doing so. Also checks the exit
# status and quits if there was an error.
echo_run ()
{
    echo "$@"
    "$@"
    r=$?
    if test $r -ne 0 ; then
        exit $r
    fi
}

# Print an error message, and exit with a status of 1.
error_exit ()
{
    echo "###"
    echo "###" "$@"
    echo "###"
    echo "### You can specify the toolset as the argument, i.e.:"
    echo "###     ./build.sh gcc"
    echo "###"
    echo "### Toolsets supported by this script are:"
    echo "###     acc, como, darwin, gcc, intel-linux, kcc, kylix, mipspro,"
    echo "###     sunpro, tru64cxx, vacpp"
    echo "###"
    echo "### A special toolset; cc, is available which is used as a fallback"
    echo "### when a more specific toolset is not found and the cc command is"
    echo "### detected. The 'cc' toolset will use the CC, CFLAGS, and LIBS"
    echo "### envrironment variables, if present."
    echo "###"
    exit 1
}

# Check that a command is in the PATH.
test_path ()
{
    if `whence 1>/dev/null 2>/dev/null`; then
        whence $1 1>/dev/null 2>/dev/null
    else
        hash $1 1>/dev/null 2>/dev/null
    fi
}

# Check that the OS name, as returned by "uname", is as given.
test_uname ()
{
    if test_path uname; then
        test `uname` = $*
    fi
}

# Try and guess the toolset to bootstrap the build with...
Guess_Toolset ()
{
    if test_uname Darwin ; then BOOST_JAM_TOOLSET=darwin
    elif test_path gcc ; then BOOST_JAM_TOOLSET=gcc
    elif test_path icc ; then BOOST_JAM_TOOLSET=intel-linux
    elif test -r /opt/intel/compiler70/ia32/bin/iccvars.sh ; then
        BOOST_JAM_TOOLSET=intel-linux
        BOOST_JAM_TOOLSET_ROOT=/opt/intel/compiler70/ia32/
    elif test -r /opt/intel/compiler60/ia32/bin/iccvars.sh ; then
        BOOST_JAM_TOOLSET=intel-linux
        BOOST_JAM_TOOLSET_ROOT=/opt/intel/compiler60/ia32/
    elif test -r /opt/intel/compiler50/ia32/bin/iccvars.sh ; then
        BOOST_JAM_TOOLSET=intel-linux
        BOOST_JAM_TOOLSET_ROOT=/opt/intel/compiler50/ia32/
    elif test_path xlc ; then BOOST_JAM_TOOLSET=vacpp
    elif test_path como ; then BOOST_JAM_TOOLSET=como
    elif test_path KCC ; then BOOST_JAM_TOOLSET=kcc
    elif test_path bc++ ; then BOOST_JAM_TOOLSET=kylix
    elif test_path aCC ; then BOOST_JAM_TOOLSET=acc
    elif test_uname HP-UX ; then BOOST_JAM_TOOLSET=acc
    # Test for "cc" as the default fallback.
    elif test_path $CC ; then BOOST_JAM_TOOLSET=cc
    elif test_path cc ; then
        BOOST_JAM_TOOLSET=cc
        CC=cc
    fi
    if test "$BOOST_JAM_TOOLSET" = "" ; then
        error_exit "Could not find a suitable toolset."
    fi
}

# The one option we support in the invocation
# is the name of the toolset to force building
# with.
case "$1" in
    -*) Guess_Toolset ;;
    ?*) BOOST_JAM_TOOLSET=$1 ; shift ;;
    *) Guess_Toolset ;;
esac
BOOST_JAM_OPT_JAM="-o bootstrap.$BOOST_JAM_TOOLSET/jam0"
BOOST_JAM_OPT_MKJAMBASE="-o bootstrap.$BOOST_JAM_TOOLSET/mkjambase0"
BOOST_JAM_OPT_YYACC="-o bootstrap.$BOOST_JAM_TOOLSET/yyacc0"
case $BOOST_JAM_TOOLSET in
    gcc)
    BOOST_JAM_CC=gcc
    ;;
    
    darwin)
    BOOST_JAM_CC=cc
    ;;
    
    intel-linux)
    if test -r /opt/intel/compiler50/ia32/bin/iccvars.sh ; then
        BOOST_JAM_TOOLSET_ROOT=/opt/intel/compiler50/ia32/
    fi
    if test -r ${BOOST_JAM_TOOLSET_ROOT}bin/iccvars.sh ; then
        . ${BOOST_JAM_TOOLSET_ROOT}bin/iccvars.sh
    fi
    BOOST_JAM_CC=icc
    ;;
    
    vacpp)
    BOOST_JAM_CC=xlc
    ;;
    
    como)
    BOOST_JAM_CC=como
    ;;
    
    kcc)
    BOOST_JAM_CC=KCC
    ;;
    
    kylix)
    BOOST_JAM_CC=bc++
    ;;
    
    mipspro)
    BOOST_JAM_CC=cc
    ;;
    
    sunpro)
    BOOST_JAM_CC=CC
    ;;
    
    tru64cxx)
    BOOST_JAM_CC=cc
    ;;
    
    acc)
    BOOST_JAM_CC="cc -Ae"
    ;;
    
    cc)
    if test -z "$CC" ; then CC=cc ; fi
    BOOST_JAM_CC=$CC
    BOOST_JAM_OPT_JAM="$BOOST_JAM_OPT_JAM $CFLAGS $LIBS"
    BOOST_JAM_OPT_MKJAMBASE="$BOOST_JAM_OPT_MKJAMBASE $CFLAGS $LIBS"
    BOOST_JAM_OPT_YYACC="$BOOST_JAM_OPT_YYACC $CFLAGS $LIBS"
    ;;
   
    *)
    error_exit "Unknown toolset: $BOOST_JAM_TOOLSET"
    ;;
esac

echo "###"
echo "### Using '$BOOST_JAM_TOOLSET' toolset."
echo "###"

YYACC_SOURCES="yyacc.c"
MKJAMBASE_SOURCES="mkjambase.c"
BJAM_SOURCES="\
 command.c compile.c execnt.c execunix.c execvms.c expand.c\
 filent.c fileos2.c fileunix.c filevms.c glob.c hash.c\
 hdrmacro.c headers.c jam.c jambase.c jamgram.c lists.c make.c make1.c\
 newstr.c option.c parse.c pathunix.c pathvms.c regexp.c\
 rules.c scan.c search.c subst.c timestamp.c variable.c modules.c\
 strings.c filesys.c builtins.c pwd.c"

echo_run rm -rf bootstrap.$BOOST_JAM_TOOLSET
echo_run mkdir bootstrap.$BOOST_JAM_TOOLSET
if test ! -r jamgram.y -o ! -r jamgramtab.h ; then
    echo_run ${BOOST_JAM_CC} ${BOOST_JAM_OPT_YYACC} ${YYACC_SOURCES}
    if test -x "./bootstrap.$BOOST_JAM_TOOLSET/yyacc0" ; then
        echo_run ./bootstrap.$BOOST_JAM_TOOLSET/yyacc0 jamgram.y jamgramtab.h jamgram.yy
    fi
fi
if test ! -r jamgram.c -o ! -r jamgram.h ; then
    if test_path yacc ; then YACC="yacc -d"
    elif test_path bison ; then YACC="bison -y -d --yacc"
    fi
    echo_run $YACC jamgram.y
    mv -f y.tab.c jamgram.c
    mv -f y.tab.h jamgram.h
fi
if test ! -r jambase.c ; then
    echo_run ${BOOST_JAM_CC} ${BOOST_JAM_OPT_MKJAMBASE} ${MKJAMBASE_SOURCES}
    if test -x "./bootstrap.$BOOST_JAM_TOOLSET/mkjambase0" ; then
        echo_run ./bootstrap.$BOOST_JAM_TOOLSET/mkjambase0 jambase.c Jambase
    fi
fi
echo_run ${BOOST_JAM_CC} ${BOOST_JAM_OPT_JAM} ${BJAM_SOURCES}
if test -x "./bootstrap.$BOOST_JAM_TOOLSET/jam0" ; then
    echo_run ./bootstrap.$BOOST_JAM_TOOLSET/jam0 -f build.jam --toolset=$BOOST_JAM_TOOLSET "--toolset-root=$BOOST_JAM_TOOLSET_ROOT" "$@"
fi
