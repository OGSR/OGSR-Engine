#
# C++ Lexer example implemented with Spirit (http://spirit.sourceforge.net/)
#
# Copyright© 2002-2003 Juan Carlos Arevalo-Baeza, All rights reserved
# email: jcab@JCABs-Rumblings.com
# Created: 8-Nov-2002
#

CC = g++ -O3 -ftemplate-depth-500 -I../../../../.. -fomit-frame-pointer
LD = g++ -O3

default: cpp_lexer_test.exe cpp_lexer_simple_test.exe

cpp_lexer_test.exe: cpp_lexer_test.o cpp_lexer_token.o cpp_lexer.o
	$(LD) -o $@ $^

cpp_lexer_simple_test.exe: cpp_lexer_simple_test.o cpp_lexer_token.o cpp_lexer.o
	$(LD) -o $@ $^

cpp_lexer.o				: cpp_lexer.cpp cpp_lexer.hpp lexer_base.hpp cpp_lexer_token.hpp lexer_token_base.hpp
cpp_lexer_token.o		: cpp_lexer_token.cpp cpp_lexer_token.hpp lexer_token_base.hpp
cpp_lexer_test.o		: cpp_lexer_test.cpp cpp_lexer.hpp lexer_base.hpp cpp_lexer_token.hpp lexer_token_base.hpp
cpp_lexer_simple_test.o	: cpp_lexer_simple_test.cpp cpp_lexer.hpp lexer_base.hpp cpp_lexer_token.hpp lexer_token_base.hpp
cpp_preprocessor.o		: cpp_preprocessor.cpp cpp_preprocessor.hpp lexer_base.hpp

%.o: %.cpp
	$(CC) -o $@ -c $<
