
C Grammar checker
Copyright (c) 2001-2003 Hartmut Kaiser
http://spirit.sourceforge.net/

Permission to copy, use, modify, sell and distribute this software is
granted provided this copyright notice appears in all copies. This
software is provided "as is" without express or implied warranty, and
with no claim as to its suitability for any purpose.

-------------------------------------------------------------------------------

The C grammar parser is a full working example of using the Spirit
library and is able to parse the full ANSI C language. 

The C grammar is adapted from
	http://www.lysator.liu.se/c/ANSI-C-grammar-y.html
	http://www.lysator.liu.se/c/ANSI-C-grammar-l.html 

Not implemented is the analysis of typedef's because it requires semantic
analysis of the parsed code, which is beyond the scope of this sample.

The test files in this directory are adapted from
	http://www.bagley.org/~doug/shootout
and where modified slightly to avoid preprocessing (#include's and 
#define's are commented out, where possible).

The test files are parsed correctly.

