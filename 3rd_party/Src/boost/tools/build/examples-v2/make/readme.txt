
Example of a simple project, which builds an executable using g++.
All the transformations are specified explicitly.
It illustrates the use of project identifiers to refer to other targets
and also project requirements.


Interesting commands would be

    bjam 
    bjam release
    bjam release clean
    bjam release debug-symbols=on

Also, you can use jam's "-n" option to check if "debug"/"release" have any
effect on commands executed. (Note that "-n" should go before any non-option
elements on command line).
