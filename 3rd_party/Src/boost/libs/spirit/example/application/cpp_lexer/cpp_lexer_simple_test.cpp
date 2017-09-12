//
// C++ Lexer implemented with Spirit (http://spirit.sourceforge.net/)
//
// Copyright© 2003 Juan Carlos Arevalo-Baeza, All rights reserved
// email: jcab@JCABs-Rumblings.com
// Created: 5-Feb-2003
//
// Example test. Shows the minimum code needed to to use a lexer.
// Works on input stored in a memory buffer.
//
// Note that this file doesn't use nor does it include anything in
// Spirit and Phoenix.
//
// Indirectly, through the cpp_lexer.hpp file, it includes
// boost/spirit/iterator/multi_pass.hpp, but that one doesn't
// (shouldn't) include any of the parser code. It is needed to
// implement the lexer as a forward iterator.
//

#include "cpp_lexer.hpp"

#include <iostream>

///////////////////////////////////////////////////////////////////////////////
// main entrypoint

int
main(int argc, char* argv[])
{
    // Welcome message.
    std::cerr << "Simple test for the C++ Lexer implemented with Spirit ..." << std::endl;

    char const* fname = ""; // The input filename.

    // Parse command line arguments.
    if (2 == argc) {
        fname = argv[1];
    } else {
        std::cerr << "No filename given" << std::endl;
        fname = "cpp_lexer.cpp";
        std::cerr << "Lexing " << fname << std::endl;
    }

    // Read the input file into a buffer.
    FILE* f = fopen(fname, "rb");
    if (!f) {
        std::cerr << "Cannot open input file: " << fname << std::endl;
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    int const size = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *buf = new char[size];
    fread(buf, 1, size, f);
    fclose(f);

    std::cout << "File size: " << size << " bytes\n";

    // Initialize the lexer iterators.
    cpp::lexer_iterator first(cpp::NewLexer(buf, buf+size, fname));
    cpp::lexer_iterator last;

    // And just loop through the sequence printing the tokens.
    // PrintToken prefixes the tokens with the file, line and column.
    while (first != last) {
        cpp::Token const& token = *first;
        cpp::PrintToken(token);
        ++first;
    }

    return 0;
}
