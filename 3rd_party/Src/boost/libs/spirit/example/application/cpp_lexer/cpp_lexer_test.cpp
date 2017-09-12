//
// C++ Lexer implemented with Spirit (http://spirit.sourceforge.net/)
//
// Copyright© 2002-2003 Juan Carlos Arevalo-Baeza, All rights reserved
// email: jcab@JCABs-Rumblings.com
// Created: 8-Nov-2002
//
// Example test. Shows how to use a lexer to feed a simple Spirit
// grammar that works on lexer tokens and uses Phoenix.
//

#include "cpp_lexer.hpp"    // The main lexer API include.

#include <iostream>

// Some necessary Spirit includes.
#include <boost/spirit/core.hpp>
#include <boost/spirit/utility/functor_parser.hpp>

// Some necessary Phoenix includes.
#include <boost/spirit/phoenix/primitives.hpp>
#include <boost/spirit/phoenix/special_ops.hpp>

///////////////////////////////////////////////////////////////////////////////
// Used namespaces and identifiers.

using namespace boost::spirit;

using phoenix::var;

using namespace cpp;

///////////////////////////////////////////////////////////////////////////////
// This is a custom parser that extracts the current file position into a
// variable. Typically, this variable will be just a temporary value to be
// used later on within the parser
//
// To use, for instance:
//
//    cpp::file_position tempFilePos;
//    ...
//    rule myrule = get_file_position_p(tempFilePos) >>
//        (parseSomething | error_p(tempFilePos, "parseSomething failed");
//
// Presumably (if error_p is implemented accordingly), this can result in the
// error message "parseSomething failed", prefixed with the file, line AND
// column where the parsing failed.

struct get_file_position_parser {
    cpp::file_position& filePos;
    get_file_position_parser(cpp::file_position& filePos_):
        filePos(filePos_)
    {}
    typedef nil_t result_t;
    template < typename ScannerT >
    int operator()(ScannerT const& scan, result_t& result) const {
        if (scan.at_end()) {
            return -1;
        }
        filePos = scan.first->filePos;
        return 0;
    }
};

boost::spirit::functor_parser<get_file_position_parser>
get_file_position_p(cpp::file_position& filePos)
{
    return get_file_position_parser(filePos);
}

///////////////////////////////////////////////////////////////////////////////
// Simple test grammar.
//
// This just outputs the input tokens just as they are found, prefixing each
// line with the file, line and column where they start.
//
// It also shows recognition of individual tokens by recognizing the "using"
// C++ keyword and outputing it surrounded by extra spaces.
//
// If the line prefixes are removed, the resulting output should be
// compilable, just like the input.

struct test_grammar: grammar<test_grammar> {
    template < typename ScannerT >
    struct definition {
        typedef rule<ScannerT> rule_t;
        rule_t main;

        rule_t const& start() const {
            return main;
        }

        cpp::file_position filePos;

        definition(test_grammar const& self) {
            main =
                *(
                    ch_p(Kwd_using)[var(std::cout) << "   using   "]
                  | ch_p(EOL_token) >> get_file_position_p(filePos)[var(std::cout) << "\n" << var(filePos) << ": "]
                  | anychar_p[OutToken][var(std::cout) << " "]
                );
        }
    };
};

///////////////////////////////////////////////////////////////////////////////
// Main entrypoint.
//
// Usage is:
//    cpp_lexer_test [<input filename>]
//
// If not given, the input filename will be cpp_lexer.cpp.

int
main(int argc, char* argv[])
{
    // Welcome message.
    std::cerr << "Parsaing test for the C++ Lexer implemented with Spirit ..." << std::endl;

    char const* fname = ""; // The input filename.

    // Parse command line arguments.
    if (2 == argc) {
        fname = argv[1];
    } else {
        std::cerr << "No filename given" << std::endl;
        fname = "cpp_lexer.cpp";
        std::cerr << "Parsing " << fname << std::endl;
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

    test_grammar grammar;   // The grammar object.

    // And do the parsing.
    // Note that comment tokens are skipped here.
    parse_info<cpp::lexer_iterator> result =
        parse(
            first, last,
            grammar,
            ch_p(Comment_token)
        )
        ;

    // Check the result of parsing. If an error is found, then print the
    // tokens following the error.
    if (result.full) {
        std::cerr << "\n" << fname << " Parses OK" << std::endl;
    } else {
        std::cerr << "\n" << fname << " Fails Parsing" << std::endl;

        std::cerr << "These tokens follow:\n";

        for (int i = 0; i < 10 && result.stop == last; ++i) {
            PrintToken(*result.stop++);
        }
    }

    return 0;
}
