/*=============================================================================
    Spirit v1.6.0
    
    C Grammar checker
    
    Copyright (c) 2001-2003 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Adapted from:
        http://www.lysator.liu.se/c/ANSI-C-grammar-y.html
        http://www.lysator.liu.se/c/ANSI-C-grammar-l.html

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/

///////////////////////////////////////////////////////////////////////////////
// version history
//
// TODO:    Handling typedef's, without that we can't correctly parse
//            auxilliary valid C sources. But hey it's a sample.
//
// Modified: 20.09.2002 21:58:45
//  Ported to Spirit V1.5
//
// Modified: 18.10.2001 16:36:07
//  Corrected: String literals may be splitted in several strings: "abc" "def"
//  Corrected: String literals may have length 0 ("")
//
// Modified: 17.10.2001 10:56:57
//  Rewritten:  *_CONSTANT rules
//  Introduced: operator '||' where possible
//  Corrected:  IDENTIFIER can start with '_' and can contain '$'
//  Added:      Skipping of '#line' and '#pragma' directives, which contained
//                in preprocessed files of the Intel V5.0.1 compiler
//
// Modified: 16.10.2001 21:12:05
//  Corrected: missing lexeme_d[] statements for *_CONSTANT
//  Corrected: missing longest_d[] for CONSTANT

#include <iostream>
#include <fstream>
#include <vector>

#if defined(_DEBUG)
//#define BOOST_SPIRIT_DEBUG
#endif // defined(_DEBUG)

#include <boost/spirit/core.hpp>
#include <boost/spirit/utility.hpp>
#include <boost/spirit/symbols.hpp>

///////////////////////////////////////////////////////////////////////////////
// used namespaces
using namespace boost::spirit;
using namespace std;

///////////////////////////////////////////////////////////////////////////////
// parsing helper function

//  Here's our comment rule
struct skip_grammar : public grammar<skip_grammar>
{
    template <typename ScannerT>
    struct definition
    {
        definition(skip_grammar const& /*self*/)
        {
#if defined(BOOST_MSVC) && (BOOST_MSVC <= 1200)

            skip
                =   "//" >> *(anychar_p - '\n') >> '\n'     // C++ comment
                |   "/*" >> *(anychar_p - "*/") >> "*/"     // C comment
                |   "#line" >> *(anychar_p - '\n') >> '\n'  // added for correctly
                                                            // handling preprocessed
                |   "#pragma" >> *(anychar_p - '\n') >> '\n'// files from Intel V5.0.1
                                                            // on W2K
                ;
#else
            skip
                =   space_p
                |   comment_p("//")                 // C++ comment
                |   comment_p("/*", "*/")           // C comment
                |   comment_p("#line")              // added for correctly
                                                    // handling preprocessed
                |   comment_p("#pragma")            // files from Intel V5.0.1
                                                    // on W2K
                ;
#endif
        }

        rule<ScannerT> skip;

        rule<ScannerT> const&
        start() const { return skip; }
    };
};

template<typename GrammarT>
void
parse(GrammarT const& g, char const* filename)
{
    ifstream in(filename);

    if (!in)
    {
        cerr << "Could not open input file: " << filename << endl;
        return;
    }

    in.unsetf(ios::skipws); //  Turn of white space skipping on the stream

    vector<char> vec;
    std::copy(
        istream_iterator<char>(in),
        istream_iterator<char>(),
        std::back_inserter(vec));

    vector<char>::const_iterator start = vec.begin();
    vector<char>::const_iterator end = vec.end();

    skip_grammar skip;

    parse_info<vector<char>::const_iterator> result =
        parse(start, end, g, skip);

    if (result.full)
        cerr << filename << " Parses OK" << endl;
    else {
        cerr << filename << " Fails Parsing" << endl;
        for (int i = 0; i < 50; i++)
        {
            if (result.stop == end)
                break;
            cerr << *result.stop++;
        }
        cerr << endl;
    }
}

struct c_grammar : public grammar<c_grammar>
{
    template <typename ScannerT>
    struct definition
    {
        definition(c_grammar const& /*self*/) :
            ELLIPSIS("..."), RIGHT_ASSIGN(">>="), LEFT_ASSIGN("<<="),
            ADD_ASSIGN("+="), SUB_ASSIGN("-="), MUL_ASSIGN("*="),
            DIV_ASSIGN("/="), MOD_ASSIGN("%="), AND_ASSIGN("&="),
            XOR_ASSIGN("^="), OR_ASSIGN("|="), RIGHT_OP(">>"), LEFT_OP("<<"),
            INC_OP("++"), DEC_OP("--"), PTR_OP("->"), AND_OP("&&"),
            OR_OP("||"), LE_OP("<="), GE_OP(">="), EQ_OP("=="), NE_OP("!="),
            SEMICOLON(';'),
            COMMA(','), COLON(':'), ASSIGN('='), LEFT_PAREN('('),
            RIGHT_PAREN(')'), DOT('.'), ADDROF('&'), BANG('!'), TILDE('~'),
            MINUS('-'), PLUS('+'), STAR('*'), SLASH('/'), PERCENT('%'),
            LT_OP('<'), GT_OP('>'), XOR('^'), OR('|'), QUEST('?')
        {
        // C keywords
            keywords =
                "auto", "break", "case", "char", "const", "continue", "default",
                "do", "double", "else", "enum", "extern", "float", "for",
                "goto", "if", "int", "long", "register", "return", "short",
                "signed", "sizeof", "static", "struct", "switch", "typedef",
                "union", "unsigned", "void", "volatile", "while";

        // C operators
            LEFT_BRACE = chlit<>('{') | strlit<>("<%");
            RIGHT_BRACE = chlit<>('}') | strlit<>("%>");
            LEFT_BRACKET = chlit<>('[') | strlit<>("<:");
            RIGHT_BRACKET = chlit<>(']') | strlit<>(":>");

        // Tokens
            AUTO =       strlit<>("auto");
            BREAK =      strlit<>("break");
            CASE =       strlit<>("case");
            CHAR =       strlit<>("char");
            CONST =      strlit<>("const");
            CONTINUE =   strlit<>("continue");
            DEFAULT =    strlit<>("default");
            DO =         strlit<>("do");
            DOUBLE =     strlit<>("double");
            ELSE =       strlit<>("else");
            ENUM =       strlit<>("enum");
            EXTERN =     strlit<>("extern");
            FOR =        strlit<>("for");
            FLOAT =      strlit<>("float");
            GOTO =       strlit<>("goto");
            IF =         strlit<>("if");
            INT =        strlit<>("int");
            LONG =       strlit<>("long");
            REGISTER =   strlit<>("register");
            RETURN =     strlit<>("return");
            SHORT =      strlit<>("short");
            SIGNED =     strlit<>("signed");
            SIZEOF =     strlit<>("sizeof");
            STATIC =     strlit<>("static");
            STRUCT =     strlit<>("struct");
            SWITCH =     strlit<>("switch");
            TYPEDEF =    strlit<>("typedef");
            UNION =      strlit<>("union");
            UNSIGNED =   strlit<>("unsigned");
            VOID =       strlit<>("void");
            VOLATILE =   strlit<>("volatile");
            WHILE =      strlit<>("while");

        // C identifiers
            IDENTIFIER =
                lexeme_d[
                    ((alpha_p | '_' | '$') >> *(alnum_p | '_' | '$'))
                    - (keywords >> anychar_p - (alnum_p | '_' | '$'))
                ]
                ;

        // string literals
            STRING_LITERAL_PART =
                lexeme_d[
                    !chlit<>('L') >> chlit<>('\"') >>
                    *( strlit<>("\\\"") | anychar_p - chlit<>('\"') ) >>
                    chlit<>('\"')
                ]
                ;

            STRING_LITERAL = +STRING_LITERAL_PART;

        // integer constants
            INT_CONSTANT_HEX
                = lexeme_d[
                    chlit<>('0')
                    >> as_lower_d[chlit<>('x')]
                    >> +xdigit_p
                    >> !as_lower_d[chlit<>('l') | chlit<>('u')]
                ]
                ;

            INT_CONSTANT_OCT
                = lexeme_d[
                    chlit<>('0')
                    >> +range<>('0', '7')
                    >> !as_lower_d[chlit<>('l') | chlit<>('u')]
                ]
                ;

            INT_CONSTANT_DEC
                = lexeme_d[
                    +digit_p
                    >> !as_lower_d[chlit<>('l') | chlit<>('u')]
                ]
                ;

            INT_CONSTANT_CHAR
                = lexeme_d[
                    !chlit<>('L') >> chlit<>('\'') >>
                    longest_d[
                            anychar_p
                        |   (   chlit<>('\\')
                                >> chlit<>('0')
                                >> repeat_p(0, 2)[range<>('0', '7')]
                            )
                        |   (chlit<>('\\') >> anychar_p)
                    ] >>
                    chlit<>('\'')
                ]
                ;

            INT_CONSTANT =
                    INT_CONSTANT_HEX
                |   INT_CONSTANT_OCT
                |   INT_CONSTANT_DEC
                |   INT_CONSTANT_CHAR
                ;

        // float constants
            FLOAT_CONSTANT_1    // 12345[eE][+-]123[lLfF]?
                = lexeme_d[
                    +digit_p
                    >> (chlit<>('e') | chlit<>('E'))
                    >> !(chlit<>('+') | chlit<>('-'))
                    >> +digit_p
                    >> !as_lower_d[chlit<>('l') | chlit<>('f')]
                ]
                ;

            FLOAT_CONSTANT_2    // .123([[eE][+-]123)?[lLfF]?
                = lexeme_d[
                    *digit_p
                    >> chlit<>('.')
                    >> +digit_p
                    >> !(   (chlit<>('e') | chlit<>('E'))
                            >> !(chlit<>('+') | chlit<>('-'))
                            >> +digit_p
                        )
                    >> !as_lower_d[chlit<>('l') | chlit<>('f')]
                ]
                ;

            FLOAT_CONSTANT_3    // 12345.([[eE][+-]123)?[lLfF]?
                = lexeme_d[
                    +digit_p
                    >> chlit<>('.')
                    >> *digit_p
                    >> !(   (chlit<>('e') | chlit<>('E'))
                            >> !(chlit<>('+') | chlit<>('-'))
                            >> +digit_p
                        )
                    >> !as_lower_d[chlit<>('l') | chlit<>('f')]
                ]
                ;

            FLOAT_CONSTANT =
                    FLOAT_CONSTANT_1
                |   FLOAT_CONSTANT_2
                |   FLOAT_CONSTANT_3
                ;

            CONSTANT = longest_d[FLOAT_CONSTANT | INT_CONSTANT];

        // debug support for terminals
            BOOST_SPIRIT_DEBUG_RULE(AUTO);
            BOOST_SPIRIT_DEBUG_RULE(BREAK);
            BOOST_SPIRIT_DEBUG_RULE(CASE);
            BOOST_SPIRIT_DEBUG_RULE(CHAR);
            BOOST_SPIRIT_DEBUG_RULE(CONST);
            BOOST_SPIRIT_DEBUG_RULE(CONTINUE);
            BOOST_SPIRIT_DEBUG_RULE(DEFAULT);
            BOOST_SPIRIT_DEBUG_RULE(DO);
            BOOST_SPIRIT_DEBUG_RULE(DOUBLE);
            BOOST_SPIRIT_DEBUG_RULE(ELSE);
            BOOST_SPIRIT_DEBUG_RULE(ENUM);
            BOOST_SPIRIT_DEBUG_RULE(EXTERN);
            BOOST_SPIRIT_DEBUG_RULE(FOR);
            BOOST_SPIRIT_DEBUG_RULE(FLOAT);
            BOOST_SPIRIT_DEBUG_RULE(GOTO);
            BOOST_SPIRIT_DEBUG_RULE(IF);
            BOOST_SPIRIT_DEBUG_RULE(INT);
            BOOST_SPIRIT_DEBUG_RULE(LONG);
            BOOST_SPIRIT_DEBUG_RULE(REGISTER);
            BOOST_SPIRIT_DEBUG_RULE(RETURN);
            BOOST_SPIRIT_DEBUG_RULE(SHORT);
            BOOST_SPIRIT_DEBUG_RULE(SIGNED);
            BOOST_SPIRIT_DEBUG_RULE(SIZEOF);
            BOOST_SPIRIT_DEBUG_RULE(STATIC);
            BOOST_SPIRIT_DEBUG_RULE(STRUCT);
            BOOST_SPIRIT_DEBUG_RULE(SWITCH);
            BOOST_SPIRIT_DEBUG_RULE(TYPEDEF);
            BOOST_SPIRIT_DEBUG_RULE(UNION);
            BOOST_SPIRIT_DEBUG_RULE(UNSIGNED);
            BOOST_SPIRIT_DEBUG_RULE(VOID);
            BOOST_SPIRIT_DEBUG_RULE(VOLATILE);
            BOOST_SPIRIT_DEBUG_RULE(WHILE);
            BOOST_SPIRIT_DEBUG_RULE(IDENTIFIER);
            BOOST_SPIRIT_DEBUG_RULE(STRING_LITERAL);
            BOOST_SPIRIT_DEBUG_RULE(INT_CONSTANT_HEX);
            BOOST_SPIRIT_DEBUG_RULE(INT_CONSTANT_OCT);
            BOOST_SPIRIT_DEBUG_RULE(INT_CONSTANT_DEC);
            BOOST_SPIRIT_DEBUG_RULE(INT_CONSTANT_CHAR);
            BOOST_SPIRIT_DEBUG_RULE(INT_CONSTANT);
            BOOST_SPIRIT_DEBUG_RULE(FLOAT_CONSTANT_1);
            BOOST_SPIRIT_DEBUG_RULE(FLOAT_CONSTANT_2);
            BOOST_SPIRIT_DEBUG_RULE(FLOAT_CONSTANT_3);
            BOOST_SPIRIT_DEBUG_RULE(FLOAT_CONSTANT);
            BOOST_SPIRIT_DEBUG_RULE(CONSTANT);

        // debug support for non terminals
            BOOST_SPIRIT_DEBUG_RULE(primary_expression);
            BOOST_SPIRIT_DEBUG_RULE(postfix_expression);
            BOOST_SPIRIT_DEBUG_RULE(postfix_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(argument_expression_list);
            BOOST_SPIRIT_DEBUG_RULE(unary_expression);
            BOOST_SPIRIT_DEBUG_RULE(unary_operator);
            BOOST_SPIRIT_DEBUG_RULE(cast_expression);
            BOOST_SPIRIT_DEBUG_RULE(multiplicative_expression);
            BOOST_SPIRIT_DEBUG_RULE(multiplicative_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(additive_expression);
            BOOST_SPIRIT_DEBUG_RULE(additive_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(shift_expression);
            BOOST_SPIRIT_DEBUG_RULE(shift_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(relational_expression);
            BOOST_SPIRIT_DEBUG_RULE(relational_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(equality_expression);
            BOOST_SPIRIT_DEBUG_RULE(equality_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(and_expression);
            BOOST_SPIRIT_DEBUG_RULE(and_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(exclusive_or_expression);
            BOOST_SPIRIT_DEBUG_RULE(exclusive_or_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(inclusive_or_expression);
            BOOST_SPIRIT_DEBUG_RULE(inclusive_or_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(logical_and_expression);
            BOOST_SPIRIT_DEBUG_RULE(logical_and_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(logical_or_expression);
            BOOST_SPIRIT_DEBUG_RULE(logical_or_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(conditional_expression);
            BOOST_SPIRIT_DEBUG_RULE(conditional_expression_helper);
            BOOST_SPIRIT_DEBUG_RULE(assignment_expression);
            BOOST_SPIRIT_DEBUG_RULE(assignment_operator);
            BOOST_SPIRIT_DEBUG_RULE(expression);
            BOOST_SPIRIT_DEBUG_RULE(constant_expression);
            BOOST_SPIRIT_DEBUG_RULE(declaration);
            BOOST_SPIRIT_DEBUG_RULE(declaration_specifiers);
            BOOST_SPIRIT_DEBUG_RULE(init_declarator_list);
            BOOST_SPIRIT_DEBUG_RULE(init_declarator);
            BOOST_SPIRIT_DEBUG_RULE(storage_class_specifier);
            BOOST_SPIRIT_DEBUG_RULE(type_specifier);
            BOOST_SPIRIT_DEBUG_RULE(struct_or_union_specifier);
            BOOST_SPIRIT_DEBUG_RULE(struct_or_union);
            BOOST_SPIRIT_DEBUG_RULE(struct_declaration_list);
            BOOST_SPIRIT_DEBUG_RULE(struct_declaration);
            BOOST_SPIRIT_DEBUG_RULE(specifier_qualifier_list);
            BOOST_SPIRIT_DEBUG_RULE(struct_declarator_list);
            BOOST_SPIRIT_DEBUG_RULE(struct_declarator);
            BOOST_SPIRIT_DEBUG_RULE(enum_specifier);
            BOOST_SPIRIT_DEBUG_RULE(enumerator_list);
            BOOST_SPIRIT_DEBUG_RULE(enumerator);
            BOOST_SPIRIT_DEBUG_RULE(type_qualifier);
            BOOST_SPIRIT_DEBUG_RULE(declarator);
            BOOST_SPIRIT_DEBUG_RULE(direct_declarator);
            BOOST_SPIRIT_DEBUG_RULE(direct_declarator_helper);
            BOOST_SPIRIT_DEBUG_RULE(pointer);
            BOOST_SPIRIT_DEBUG_RULE(type_qualifier_list);
            BOOST_SPIRIT_DEBUG_RULE(parameter_type_list);
            BOOST_SPIRIT_DEBUG_RULE(parameter_list);
            BOOST_SPIRIT_DEBUG_RULE(parameter_declaration);
            BOOST_SPIRIT_DEBUG_RULE(identifier_list);
            BOOST_SPIRIT_DEBUG_RULE(type_name);
            BOOST_SPIRIT_DEBUG_RULE(abstract_declarator);
            BOOST_SPIRIT_DEBUG_RULE(direct_abstract_declarator);
            BOOST_SPIRIT_DEBUG_RULE(direct_abstract_declarator_helper);
            BOOST_SPIRIT_DEBUG_RULE(initializer);
            BOOST_SPIRIT_DEBUG_RULE(initializer_list);
            BOOST_SPIRIT_DEBUG_RULE(statement);
            BOOST_SPIRIT_DEBUG_RULE(labeled_statement);
            BOOST_SPIRIT_DEBUG_RULE(compound_statement);
            BOOST_SPIRIT_DEBUG_RULE(declaration_list);
            BOOST_SPIRIT_DEBUG_RULE(statement_list);
            BOOST_SPIRIT_DEBUG_RULE(expression_statement);
            BOOST_SPIRIT_DEBUG_RULE(selection_statement);
            BOOST_SPIRIT_DEBUG_RULE(iteration_statement);
            BOOST_SPIRIT_DEBUG_RULE(jump_statement);
            BOOST_SPIRIT_DEBUG_RULE(translation_unit);
            BOOST_SPIRIT_DEBUG_RULE(external_declaration);
            BOOST_SPIRIT_DEBUG_RULE(function_definition);

        // Rules
            primary_expression
                = IDENTIFIER
                | CONSTANT
                | STRING_LITERAL
                | LEFT_PAREN >> expression >> RIGHT_PAREN
                ;

        // left recursion --> right recursion
        //    postfix_expression
        //        = primary_expression
        //        | postfix_expression >>
        //            (
        //                LEFT_BRACKET >> expression >> RIGHT_BRACKET
        //            |   LEFT_PAREN >> !argument_expression_list >> RIGHT_PAREN
        //            |   DOT >> IDENTIFIER
        //            |   PTR_OP >> IDENTIFIER
        //            |   INC_OP
        //            |   DEC_OP
        //            )
        //        ;
            postfix_expression
                = primary_expression >> postfix_expression_helper
                ;

            postfix_expression_helper
                =   (
                        LEFT_BRACKET >> expression >> RIGHT_BRACKET
                    |   LEFT_PAREN >> !argument_expression_list >> RIGHT_PAREN
                    |   DOT >> IDENTIFIER
                    |   PTR_OP >> IDENTIFIER
                    |   INC_OP
                    |   DEC_OP
                    ) >>
                    postfix_expression_helper
                | epsilon_p
                ;

            argument_expression_list
                = assignment_expression >> *(COMMA >> assignment_expression)
                ;

            unary_expression
                = postfix_expression
                | INC_OP >> unary_expression
                | DEC_OP >> unary_expression
                | unary_operator >> cast_expression
                | SIZEOF >>
                    (
                        unary_expression
                    |   LEFT_PAREN >> type_name >> RIGHT_PAREN
                    )
                ;

            unary_operator
                = ADDROF
                | STAR
                | PLUS
                | MINUS
                | TILDE
                | BANG
                ;

            cast_expression
                = LEFT_PAREN >> type_name >> RIGHT_PAREN >> cast_expression
                | unary_expression
                ;

        // left recursion --> right recursion
        //    multiplicative_expression
        //        = cast_expression
        //        | multiplicative_expression >>
        //        (
        //                STAR >> cast_expression
        //            |   SLASH >> cast_expression
        //            |   PERCENT >> cast_expression
        //        )
        //        ;
            multiplicative_expression
                = cast_expression >> multiplicative_expression_helper
                ;

            multiplicative_expression_helper
                =   (
                        STAR >> cast_expression
                    |   SLASH >> cast_expression
                    |   PERCENT >> cast_expression
                    ) >>
                    multiplicative_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    additive_expression
        //        = multiplicative_expression
        //        | additive_expression >>
        //        (
        //                PLUS >> multiplicative_expression
        //            |   MINUS >> multiplicative_expression
        //        )
        //        ;
            additive_expression
                = multiplicative_expression >> additive_expression_helper
                ;

            additive_expression_helper
                =   (
                        PLUS >> multiplicative_expression
                    |   MINUS >> multiplicative_expression
                    ) >>
                    additive_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    shift_expression
        //        = additive_expression
        //        | shift_expression >>
        //        (
        //                LEFT_OP >> additive_expression
        //            |   RIGHT_OP >> additive_expression
        //        )
        //        ;
            shift_expression
                = additive_expression >> shift_expression_helper
                ;

            shift_expression_helper
                =   (
                        LEFT_OP >> additive_expression
                    |   RIGHT_OP >> additive_expression
                    ) >>
                    shift_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    relational_expression
        //        = shift_expression
        //        | relational_expression >>
        //        (
        //                LT_OP >> shift_expression
        //            |   GT_OP >> shift_expression
        //            |   LE_OP >> shift_expression
        //            |   GE_OP >> shift_expression
        //        )
        //        ;
            relational_expression
                = shift_expression >> relational_expression_helper
                ;

            relational_expression_helper
                =   (
                        LT_OP >> shift_expression
                    |   GT_OP >> shift_expression
                    |   LE_OP >> shift_expression
                    |   GE_OP >> shift_expression
                    ) >>
                    relational_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    equality_expression
        //        = relational_expression
        //        | equality_expression >>
        //        (
        //                EQ_OP >> relational_expression
        //            |   NE_OP >> relational_expression
        //        )
        //        ;
            equality_expression
                = relational_expression >> equality_expression_helper
                ;

            equality_expression_helper
                =   (
                        EQ_OP >> relational_expression
                    |   NE_OP >> relational_expression
                    ) >>
                    equality_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    and_expression
        //        = equality_expression
        //        | and_expression >> ADDROF >> equality_expression
        //        ;
            and_expression
                = equality_expression >> and_expression_helper
                ;

            and_expression_helper
                = ADDROF >> equality_expression >> and_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    exclusive_or_expression
        //        = and_expression
        //        | exclusive_or_expression >> XOR >> and_expression
        //        ;
            exclusive_or_expression
                = and_expression >> exclusive_or_expression_helper
                ;

            exclusive_or_expression_helper
                = XOR >> and_expression >> exclusive_or_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    inclusive_or_expression
        //        = exclusive_or_expression
        //        | inclusive_or_expression >> OR >> exclusive_or_expression
        //        ;
            inclusive_or_expression
                = exclusive_or_expression >> inclusive_or_expression_helper
                ;

            inclusive_or_expression_helper
                = OR >> exclusive_or_expression >> inclusive_or_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    logical_and_expression
        //        = inclusive_or_expression
        //        | logical_and_expression >> AND_OP >> inclusive_or_expression
        //        ;
            logical_and_expression
                = inclusive_or_expression >> logical_and_expression_helper
                ;

            logical_and_expression_helper
                = AND_OP >> inclusive_or_expression >> logical_and_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    logical_or_expression
        //        = logical_and_expression
        //        | logical_or_expression >> OR_OP >> logical_and_expression
        //        ;
            logical_or_expression
                = logical_and_expression >> logical_or_expression_helper
                ;

            logical_or_expression_helper
                = OR_OP >> logical_and_expression >> logical_or_expression_helper
                | epsilon_p
                ;

        // left recursion --> right recursion
        //    conditional_expression
        //        = logical_or_expression
        //        | logical_or_expression >> QUEST >> expression >> COLON >>
        //        conditional_expression
        //        ;
            conditional_expression
                = logical_or_expression >> conditional_expression_helper
                ;

            conditional_expression_helper
                = QUEST >> expression >> COLON
                    >> conditional_expression >> conditional_expression_helper
                | epsilon_p
                ;

            assignment_expression
                = unary_expression >> assignment_operator >> assignment_expression
                | conditional_expression
                ;

            assignment_operator
                = ASSIGN
                | MUL_ASSIGN
                | DIV_ASSIGN
                | MOD_ASSIGN
                | ADD_ASSIGN
                | SUB_ASSIGN
                | LEFT_ASSIGN
                | RIGHT_ASSIGN
                | AND_ASSIGN
                | XOR_ASSIGN
                | OR_ASSIGN
                ;

        // left recursion --> right recursion
        //    expression
        //        = assignment_expression
        //        | expression >> COMMA >> assignment_expression
        //        ;
            expression
                = assignment_expression >> expression_helper
                ;

            expression_helper
                = COMMA >> assignment_expression >> expression_helper
                | epsilon_p
                ;

            constant_expression
                = conditional_expression
                ;

            declaration
                = declaration_specifiers >> !init_declarator_list >> SEMICOLON
                ;

            declaration_specifiers
                =   (
                        storage_class_specifier
                    |   type_specifier
                    |   type_qualifier
                    ) >>
                    !declaration_specifiers
                ;

            init_declarator_list
                = init_declarator >> *(COMMA >> init_declarator)
                ;

            init_declarator
                = declarator >> !(ASSIGN >> initializer)
                ;

            storage_class_specifier
                = TYPEDEF
                | EXTERN
                | STATIC
                | AUTO
                | REGISTER
                ;

            type_specifier
                = VOID
                | CHAR
                | SHORT
                | INT
                | LONG
                | FLOAT
                | DOUBLE
                | SIGNED
                | UNSIGNED
                | struct_or_union_specifier
                | enum_specifier
        //        | TYPE_NAME
                ;

            struct_or_union_specifier
                = struct_or_union >>
                    (
                        IDENTIFIER >> !(LEFT_BRACE >> struct_declaration_list
                            >> RIGHT_BRACE)
                    |   LEFT_BRACE >> struct_declaration_list >> RIGHT_BRACE
                    )
                ;

            struct_or_union
                = STRUCT
                | UNION
                ;

            struct_declaration_list
                = +struct_declaration
                ;

            struct_declaration
                = specifier_qualifier_list >> struct_declarator_list >> SEMICOLON
                ;

            specifier_qualifier_list
                =   (
                        type_specifier
                    |   type_qualifier
                    ) >>
                    !specifier_qualifier_list
                ;

            struct_declarator_list
                = struct_declarator >> *(COMMA >> struct_declarator)
                ;

            struct_declarator
                = declarator || (COLON >> constant_expression)
                ;

            enum_specifier
                = ENUM >> !IDENTIFIER >> LEFT_BRACE >> enumerator_list >> RIGHT_BRACE
                ;

            enumerator_list
                = enumerator >> *(COMMA >> enumerator)
                ;

            enumerator
                = IDENTIFIER >> !(ASSIGN >> constant_expression)
                ;

            type_qualifier
                = CONST
                | VOLATILE
                ;

            declarator
                = !pointer >> direct_declarator
                ;

        // left recursion --> right recursion
        //    direct_declarator
        //        = IDENTIFIER
        //        | LEFT_PAREN >> declarator >> RIGHT_PAREN
        //        | direct_declarator >>
        //            (
        //                LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
        //            |   LEFT_PAREN >>
        //                (
        //                    parameter_type_list >> RIGHT_PAREN
        //                |   identifier_list >> RIGHT_PAREN
        //                |   RIGHT_PAREN
        //                )
        //            )
        //        ;
            direct_declarator
                =   (
                        IDENTIFIER
                    |   LEFT_PAREN >> declarator >> RIGHT_PAREN
                    ) >>
                    direct_declarator_helper
                ;

            direct_declarator_helper
                =   (
                        LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
                    |   LEFT_PAREN >>
                    !   (
                            parameter_type_list
                        |   identifier_list
                        ) >> RIGHT_PAREN
                    ) >> direct_declarator_helper
                | epsilon_p
                ;

            pointer
                = STAR >> !(type_qualifier_list || pointer)
                ;

            type_qualifier_list
                = +type_qualifier
                ;

            parameter_type_list
                = parameter_list >> !(COMMA >> ELLIPSIS)
                ;

            parameter_list
                = parameter_declaration >> *(COMMA >> parameter_declaration)
                ;

            parameter_declaration
                = declaration_specifiers >>
                   !(
                        declarator
                    |   abstract_declarator
                    )
                ;

            identifier_list
                = IDENTIFIER >> *(COMMA >> IDENTIFIER)
                ;

            type_name
                = specifier_qualifier_list >> !abstract_declarator
                ;

            abstract_declarator
                = pointer || direct_abstract_declarator
                ;

        // left recursion --> right recursion
        //    direct_abstract_declarator
        //        = LEFT_PAREN >>
        //            (
        //                abstract_declarator >> RIGHT_PAREN
        //            |   !parameter_type_list >> RIGHT_PAREN
        //            )
        //        | LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
        //        | direct_abstract_declarator >>
        //            (
        //                LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
        //            |   LEFT_PAREN >> !parameter_type_list >> RIGHT_PAREN
        //            )
        //        ;
            direct_abstract_declarator
                =   (
                        LEFT_PAREN >>
                        (
                            abstract_declarator >> RIGHT_PAREN
                        |   !parameter_type_list >> RIGHT_PAREN
                        )
                    |    LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
                    ) >>
                    direct_abstract_declarator_helper
                ;

            direct_abstract_declarator_helper
                =   (
                        LEFT_BRACKET >> !constant_expression >> RIGHT_BRACKET
                    |   LEFT_PAREN >> !parameter_type_list >> RIGHT_PAREN
                    ) >>
                    direct_abstract_declarator_helper
                | epsilon_p
                ;

            initializer
                = assignment_expression
                | LEFT_BRACE >> initializer_list >> !COMMA >> RIGHT_BRACE
                ;

            initializer_list
                = initializer >> *(COMMA >> initializer)
                ;

            statement
                = labeled_statement
                | compound_statement
                | expression_statement
                | selection_statement
                | iteration_statement
                | jump_statement
                ;

            labeled_statement
                = IDENTIFIER >> COLON >> statement
                | CASE >> constant_expression >> COLON >> statement
                | DEFAULT >> COLON >> statement
                ;

            compound_statement
                = LEFT_BRACE >> !(declaration_list || statement_list) >> RIGHT_BRACE
                ;

            declaration_list
                = +declaration
                ;

            statement_list
                = +statement
                ;

            expression_statement
                = !expression >> SEMICOLON
                ;

            selection_statement
                = IF >> LEFT_PAREN >> expression >> RIGHT_PAREN >> statement
                    >> !(ELSE >> statement)
                | SWITCH >> LEFT_PAREN >> expression >> RIGHT_PAREN >> statement
                ;

            iteration_statement
                = WHILE >> LEFT_PAREN >> expression >> RIGHT_PAREN >>  statement
                | DO >> statement >> WHILE >> LEFT_PAREN >> expression
                    >> RIGHT_PAREN >> SEMICOLON
                | FOR >> LEFT_PAREN >> expression_statement >> expression_statement
                    >> !expression >> RIGHT_PAREN >> statement
                ;

            jump_statement
                = GOTO >> IDENTIFIER >> SEMICOLON
                | CONTINUE >> SEMICOLON
                | BREAK >> SEMICOLON
                | RETURN >> !expression >> SEMICOLON
                ;

            function_definition
                =  !declaration_specifiers
                >>  declarator
                >> !declaration_list
                >>  compound_statement
                ;

            external_declaration
                = function_definition
                | declaration
                ;

        // parser start symbol
            translation_unit
                = *external_declaration
                ;
        }

    // keywords
        symbols<> keywords;

    // operators
        strlit<>
                ELLIPSIS, RIGHT_ASSIGN, LEFT_ASSIGN, ADD_ASSIGN, SUB_ASSIGN,
                MUL_ASSIGN, DIV_ASSIGN, MOD_ASSIGN, AND_ASSIGN, XOR_ASSIGN,
                OR_ASSIGN, RIGHT_OP, LEFT_OP, INC_OP, DEC_OP, PTR_OP, AND_OP,
                OR_OP, LE_OP, GE_OP, EQ_OP, NE_OP;
        chlit<>
                SEMICOLON, COMMA, COLON, ASSIGN, LEFT_PAREN, RIGHT_PAREN,
                DOT, ADDROF, BANG, TILDE, MINUS, PLUS, STAR, SLASH, PERCENT,
                LT_OP, GT_OP, XOR, OR, QUEST;

        rule<ScannerT>
                LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET;

    // terminals
        rule<ScannerT>
                AUTO, BREAK, CASE, CHAR, CONST, CONTINUE, DEFAULT, DO, DOUBLE,
                ELSE, ENUM, EXTERN, FOR, FLOAT, GOTO, IF, INT, LONG, REGISTER,
                RETURN, SHORT, SIGNED, SIZEOF, STATIC, STRUCT, SWITCH, TYPEDEF,
                UNION, UNSIGNED, VOID, VOLATILE, WHILE, IDENTIFIER,
                STRING_LITERAL_PART, STRING_LITERAL, INT_CONSTANT_HEX, INT_CONSTANT,
                INT_CONSTANT_OCT, INT_CONSTANT_DEC, INT_CONSTANT_CHAR,
                FLOAT_CONSTANT,FLOAT_CONSTANT_1, FLOAT_CONSTANT_2, FLOAT_CONSTANT_3,
                CONSTANT;

    // nonterminals
        rule<ScannerT>
                primary_expression, postfix_expression, postfix_expression_helper,
                argument_expression_list, unary_expression, unary_operator,
                cast_expression,
                multiplicative_expression, multiplicative_expression_helper,
                additive_expression, additive_expression_helper,
                shift_expression, shift_expression_helper,
                relational_expression, relational_expression_helper,
                equality_expression, equality_expression_helper,
                and_expression, and_expression_helper,
                exclusive_or_expression, exclusive_or_expression_helper,
                inclusive_or_expression, inclusive_or_expression_helper,
                logical_and_expression, logical_and_expression_helper,
                logical_or_expression, logical_or_expression_helper,
                conditional_expression, conditional_expression_helper,
                assignment_expression, assignment_operator,
                expression, expression_helper, constant_expression, declaration,
                declaration_specifiers, init_declarator_list, init_declarator,
                storage_class_specifier, type_specifier, struct_or_union_specifier,
                struct_or_union, struct_declaration_list, struct_declaration,
                specifier_qualifier_list, struct_declarator_list,
                struct_declarator, enum_specifier, enumerator_list, enumerator,
                type_qualifier, declarator,
                direct_declarator, direct_declarator_helper, pointer,
                type_qualifier_list, parameter_type_list, parameter_list,
                parameter_declaration, identifier_list, type_name,
                abstract_declarator,
                direct_abstract_declarator, direct_abstract_declarator_helper,
                initializer, initializer_list, statement, labeled_statement,
                compound_statement, declaration_list, statement_list,
                expression_statement, selection_statement, iteration_statement,
                jump_statement, translation_unit, external_declaration,
                function_definition;

        rule<ScannerT> const&
        start() const { return translation_unit; }
    };
};

///////////////////////////////////////////////////////////////////////////////
// main entrypoint
int
main(int argc, char* argv[])
{
// Start grammar definition
    cerr << "C Grammar checker implemented with Spirit ..." << endl;

// main driver code
c_grammar g;

    if (2 == argc)
        parse(g, argv[1]);
    else
        cerr << "No filename given" << endl;

    return 0;
}
