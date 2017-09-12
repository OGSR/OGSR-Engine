#!/usr/bin/python
import os

import string

# clear environment for testing
#
for s in (
    'BOOST_ROOT','BOOST_BUILD_PATH','JAM_TOOLSET','BCCROOT',
    'MSVCDir','MSVC','MSVCNT','MINGW','watcom'
    ):
    
    try:
        del os.environ[s]
    except:
        pass

def run_tests(critical_tests, other_tests):
    """Runs first critical tests and then other_tests.

       Stops on first error, and write the name of failed test to
       test_results.txt. Critical tests are run in the specified order,
       other tests are run starting with the one that failed the last time.
    """
    last_failed = last_failed_test()
    other_tests = reorder_tests(other_tests, last_failed)
    all_tests = critical_tests + other_tests

    invocation_dir = os.getcwd()

    for i in all_tests:
        print ("%-25s : " %(i)),
        try:
            __import__(i)
        except:
            print "FAILED"
            f = open(os.path.join(invocation_dir, 'test_results.txt'), 'w')
            f.write(i)
            f.close()
            raise
        print "PASSED"
    # Erase the file on success
    open('test_results.txt', 'w')
        

def last_failed_test():
    "Returns the name of last failed test or None"
    try:
        f = open("test_results.txt")
        s = string.strip(f.read())
        return s
    except:
        return None

def reorder_tests(tests, first_test):
    try:
        n = tests.index(first_test)
        return [first_test] + tests[:n] + tests[n+1:]
    except ValueError:
        return tests

            
critical_tests = ["unit_tests", "module_actions", "startup_v1", "startup_v2"]
tests = [ "project_test1",
          "project_test3",
          "project_test4",
          "generators_test",
          "dependency_test",
          "direct_request_test",
          "path_features",
          "relative_sources",
          "no_type",
          "chain",
          "default_build",
          "main_properties",
          "use_requirements",
          "conditionals",
          "stage",
          "prebuilt",
          "project_dependencies",
          "build_dir",
          "searched_lib",
          "make_rule",
          "alias",
          "alternatives",
          "unused",
	  ]

if os.name == 'posix':
    tests.append("symlink")

run_tests(critical_tests, tests)
