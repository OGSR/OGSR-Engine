///////////////////////////////////////////////////////////////////////////////
//
//  Pascal Parser Grammar for Spirit (http://spirit.sourceforge.net/)
//
//  Adapted from,
//  Pascal User Manual And Report (Second Edition-1978)
//  Kathleen Jensen - Niklaus Wirth
//
//  Written by: Hakki Dogusan dogusanh@tr.net
//  Adapted by Joel de Guzman djowel@gmx.co.uk
//  ported to Spirit v1.5 [ JDG 9/16/2002 ]
//
///////////////////////////////////////////////////////////////////////////////
//#define BOOST_SPIRIT_DEBUG  ///$$$ DEFINE THIS WHEN DEBUGGING $$$///
#include <boost/spirit/core.hpp>
#include <boost/spirit/symbols/symbols.hpp>

#include <fstream>
#include <iostream>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
using namespace std;
using namespace boost::spirit;

///////////////////////////////////////////////////////////////////////////////
//
//  Our Pascal grammar
//
///////////////////////////////////////////////////////////////////////////////
struct pascal_grammar : public grammar<pascal_grammar>
{
    pascal_grammar() {}

    template <typename ScannerT>
    struct definition
    {
        definition(pascal_grammar const& /*self*/)
        {
            #ifdef BOOST_SPIRIT_DEBUG
            debug(); // define the debug names
            #endif

            //-----------------------------------------------------------------
            // KEYWORDS
            //-----------------------------------------------------------------
            keywords =
                "in", "div", "mod", "and", "or", "not", "nil", "goto",
                "if", "then", "else", "case", "while", "repeat", "until",
                "for", "do", "to", "downto", "with", "program", "label",
                "const", "type", "char", "boolean", "integer", "real",
                "packed", "array", "of", "record", "end", "set", "file",
                "var", "procedure", "function", "begin";

            //-----------------------------------------------------------------
            // OPERATORS
            //-----------------------------------------------------------------
            chlit<>     PLUS('+');
            chlit<>     MINUS('-');
            chlit<>     STAR('*');
            chlit<>     SLASH('/');
            strlit<>    ASSIGN(":=");
            chlit<>     COMMA(',');
            chlit<>     SEMI(';');
            chlit<>     COLON(':');
            chlit<>     EQUAL('=');
            strlit<>    NOT_EQUAL("<>");
            chlit<>     LT('<');
            strlit<>    LE("<=");
            strlit<>    GE(">=");
            chlit<>     GT('>');
            chlit<>     LPAREN('(');
            chlit<>     RPAREN(')');
            chlit<>     LBRACK('[');
            chlit<>     RBRACK(']');
            chlit<>     POINTER('^');
            chlit<>     DOT('.');
            strlit<>    DOTDOT("..");

            //-----------------------------------------------------------------
            // TOKENS
            //-----------------------------------------------------------------
            typedef inhibit_case<strlit<> > token_t;

            token_t IN_         = as_lower_d["in"];
            token_t DIV         = as_lower_d["div"];
            token_t MOD         = as_lower_d["mod"];
            token_t AND         = as_lower_d["and"];
            token_t OR          = as_lower_d["or"];
            token_t NOT         = as_lower_d["not"];
            token_t NIL         = as_lower_d["nil"];
            token_t GOTO        = as_lower_d["goto"];
            token_t IF          = as_lower_d["if"];
            token_t THEN        = as_lower_d["then"];
            token_t ELSE        = as_lower_d["else"];
            token_t CASE        = as_lower_d["case"];
            token_t WHILE       = as_lower_d["while"];
            token_t REPEAT      = as_lower_d["repeat"];
            token_t UNTIL       = as_lower_d["until"];
            token_t FOR         = as_lower_d["for"];
            token_t DO          = as_lower_d["do"];
            token_t TO          = as_lower_d["to"];
            token_t DOWNTO      = as_lower_d["downto"];
            token_t WITH        = as_lower_d["with"];
            token_t PROGRAM     = as_lower_d["program"];
            token_t LABEL       = as_lower_d["label"];
            token_t CONST_      = as_lower_d["const"];
            token_t TYPE        = as_lower_d["type"];
            token_t CHAR        = as_lower_d["char"];
            token_t BOOLEAN     = as_lower_d["boolean"];
            token_t INTEGER     = as_lower_d["integer"];
            token_t REAL        = as_lower_d["real"];
            token_t PACKED_     = as_lower_d["packed"];
            token_t ARRAY       = as_lower_d["array"];
            token_t OF          = as_lower_d["of"];
            token_t RECORD      = as_lower_d["record"];
            token_t END         = as_lower_d["end"];
            token_t SET         = as_lower_d["set"];
            token_t FILE        = as_lower_d["file"];
            token_t VAR         = as_lower_d["var"];
            token_t PROCEDURE   = as_lower_d["procedure"];
            token_t FUNCTION    = as_lower_d["function"];
            token_t BEGIN       = as_lower_d["begin"];

            //-----------------------------------------------------------------
            //  Start grammar definition
            //-----------------------------------------------------------------
            identifier
                = as_lower_d[
                    lexeme_d[
                        (alpha_p >> *(alnum_p | '_'))
                        - (keywords >> anychar_p - (alnum_p | '_'))
                    ]
                ];

            string_literal
                = lexeme_d[ chlit<>('\'') >>
                   +( strlit<>("\'\'") | anychar_p-chlit<>('\'') ) >>
                   chlit<>('\'') ];


            program
                =   programHeading >>
                    block >>
                    DOT
                ;

            programHeading
                =   PROGRAM >> identifier >>
                    LPAREN >> fileIdentifier
                    >> *( COMMA >> fileIdentifier ) >> RPAREN >>
                    SEMI
                ;

            fileIdentifier
                =   identifier
                ;

            block
                =  *(   labelDeclarationPart
                    |   constantDefinitionPart
                    |   typeDefinitionPart
                    |   variableDeclarationPart
                    |   procedureAndFunctionDeclarationPart
                    ) >>
                    statementPart
                ;

            labelDeclarationPart
                =   LABEL >> label >> *( COMMA >> label ) >> SEMI
                ;

            label
                =   unsignedInteger
                ;

            constantDefinitionPart
                =   CONST_ >> constantDefinition
                    >> *( SEMI >> constantDefinition ) >> SEMI
                ;

            constantDefinition
                =   identifier >> EQUAL >> constant
                ;

            constant
                =   unsignedNumber
                |   sign >> unsignedNumber
                |   constantIdentifier
                |   sign >> constantIdentifier
                |   string_literal
                ;

            unsignedNumber
                =   lexeme_d[uint_p
                >>  !('.' >> uint_p)
                >>  !(as_lower_d['e'] >> (ch_p('+') | '-') >> uint_p)]
                ;

            unsignedInteger
                =   uint_p
                ;

            unsignedReal
                =   ureal_p
                ;

            sign
                =   PLUS | MINUS
                ;

            constantIdentifier
                =   identifier
                ;

            typeDefinitionPart
                =   TYPE >> typeDefinition
                    >> *( SEMI >> typeDefinition ) >> SEMI
                ;

            typeDefinition
                =   identifier >> EQUAL >> type
                ;

            type
                =   simpleType
                |   structuredType
                |   pointerType
                ;

            simpleType
                =   scalarType
                |   subrangeType
                |   typeIdentifier
                ;

            scalarType
                =   LPAREN >> identifier
                    >> *( COMMA >> identifier ) >> RPAREN
                ;

            subrangeType
                =   constant >> DOTDOT >> constant
                ;

            typeIdentifier
                =   identifier
                |   CHAR
                |   BOOLEAN
                |   INTEGER
                |   REAL
                ;

            structuredType
                =   ( PACKED_
                    | empty
                    ) >>
                    unpackedStructuredType
                ;

            unpackedStructuredType
                =   arrayType
                |   recordType
                |   setType
                |   fileType
                ;

            arrayType
                =   ARRAY >> LBRACK >> indexType
                    >> *( COMMA >> indexType ) >> RBRACK >> OF >>
                    componentType
                ;

            indexType
                =   simpleType
                ;

            componentType
                =   type
                ;

            recordType
                =   RECORD >> fieldList >> END
                ;

            fieldList
                =   fixedPart >>
                    ( SEMI >> variantPart
                    | empty
                    )
                |   variantPart
                ;

            fixedPart
                =   recordSection >> *( SEMI >> recordSection )
                ;

            recordSection
                =   fieldIdentifier
                    >> *( COMMA >> fieldIdentifier ) >> COLON >> type
                |   empty
                ;

            variantPart
                =   CASE >> tagField >> typeIdentifier >> OF >>
                    variant >> *( SEMI >> variant )
                ;

            tagField
                =   fieldIdentifier >> COLON
                |   empty
                ;

            variant
                =   caseLabelList >> COLON >> LPAREN >> fieldList >> RPAREN
                |   empty
                ;

            caseLabelList
                =   caseLabel >> *( COMMA >> caseLabel )
                ;

            caseLabel
                =   constant
                ;

            setType
                =   SET >> OF >> baseType
                ;

            baseType
                =   simpleType
                ;

            fileType
                =   FILE >> OF >> type
                ;

            pointerType
                =   POINTER >> typeIdentifier
                ;

            variableDeclarationPart
                =   VAR >> variableDeclaration
                    >> *( SEMI >> variableDeclaration ) >> SEMI
                ;

            variableDeclaration
                =   identifier >> *( COMMA >> identifier ) >> COLON >> type
                ;

            procedureAndFunctionDeclarationPart
                =   procedureOrFunctionDeclaration >> SEMI
                ;

            procedureOrFunctionDeclaration
                =   procedureDeclaration
                |   functionDeclaration
                ;

            procedureDeclaration
                =   procedureHeading
                    >> block
                ;

            procedureHeading
                =   PROCEDURE >> identifier >> parameterList >> SEMI
                ;

            parameterList
                =   empty
                |   LPAREN >> formalParameterSection
                    >> *( SEMI >> formalParameterSection ) >> RPAREN
                ;

            formalParameterSection
                =   parameterGroup
                |   VAR >> parameterGroup
                |   FUNCTION >> parameterGroup
                |   PROCEDURE >> identifier >> *( COMMA >> identifier )
                ;

            parameterGroup
                =   identifier >> *( COMMA >> identifier ) >> COLON >> typeIdentifier
                ;

            functionDeclaration
                =   functionHeading >>
                    block
                ;

            functionHeading
                =   FUNCTION >> identifier >> parameterList
                    >> COLON >> resultType >> SEMI
                ;

            resultType
                =   typeIdentifier
                ;

            statementPart
                =   compoundStatement
                ;

            statement
                =   ( label >> COLON
                    | empty
                    ) >>
                    unlabelledStatement
                ;

            unlabelledStatement
                =   structuredStatement
                |   simpleStatement
                ;

            simpleStatement
                =   assignmentStatement
                |   procedureStatement
                |   gotoStatement
                |   emptyStatement
                ;

            assignmentStatement
                =   variable >> ASSIGN >> expression
                |   functionIdentifier >> ASSIGN >> expression
                ;

            variable
                =   componentVariable
                |   referencedVariable
                |   entireVariable
                ;

            entireVariable
                =   variableIdentifier
                ;

            variableIdentifier
                =   identifier
                ;

            componentVariable
                =   indexedVariable
                |   fieldDesignator
                |   fileBuffer
                ;

            indexedVariable
                =   arrayVariable >> LBRACK
                    >> expression >> *( COMMA >> expression) >> RBRACK
                ;

            arrayVariable
                =   identifier
                ;

            fieldDesignator
                =   recordVariable >> DOT >> fieldIdentifier
                ;

            recordVariable
                =   identifier
                ;

            fieldIdentifier
                =   identifier
                ;

            fileBuffer
                =   fileVariable >> POINTER
                ;

            fileVariable
                =   identifier
                ;

            referencedVariable
                =   pointerVariable >> POINTER
                ;

            pointerVariable
                =   identifier
                ;

            expression
                =   simpleExpression >>
                    ( relationalOperator >> simpleExpression
                    | empty
                    )
                ;

            relationalOperator
                =   EQUAL | NOT_EQUAL | GE | LE | LT | GT | IN_
                ;

            simpleExpression
                =   ( sign
                    | empty
                    ) >>
                    term >> *( addingOperator >> term )
                ;

            addingOperator
                =   PLUS | MINUS | OR
                ;

            term
                =   factor >> *( multiplyingOperator >> factor )
                ;

            multiplyingOperator
                =   STAR | SLASH | DIV | MOD | AND
                ;

            factor
                =   LPAREN >> expression >> RPAREN
                |   set
                |   longest_d[
                        variable
                    |   unsignedConstant
                    |   functionDesignator
                ]
                |   NOT >> factor
                ;

            unsignedConstant
                =   unsignedNumber
                |   string_literal
                |   constantIdentifier
                |   NIL
                ;

            functionDesignator
                =   functionIdentifier >>
                    ( LPAREN >> actualParameter
                        >> *( COMMA >> actualParameter ) >> RPAREN
                    | empty
                    )
                ;

            functionIdentifier
                =   identifier
                ;

            set
                =   LBRACK >> elementList >> RBRACK
                ;

            elementList
                =   element >> *( COMMA >> element )
                |   empty
                ;

            element
                =   expression >>
                    ( DOTDOT >> expression
                    | empty
                    )
                ;

            procedureStatement
                =   procedureIdentifier >>
                    ( LPAREN >> actualParameter
                        >> *( COMMA >> actualParameter ) >> RPAREN
                    | empty
                    )
                ;

            procedureIdentifier
                =   identifier
                ;

            actualParameter
                =   expression
                |   variable
                |   procedureIdentifier
                |   functionIdentifier
                ;

            gotoStatement
                =   GOTO >> label
                ;

            emptyStatement
                =   empty
                ;

            empty
                =   epsilon_p
                ;

            structuredStatement
                =   compoundStatement
                |   conditionalStatement
                |   repetetiveStatement
                |   withStatement
                ;

            compoundStatement
                =   BEGIN >>
                    statement >> *( SEMI >> statement ) >>
                    END
                ;

            conditionalStatement
                =   ifStatement
                |   caseStatement
                ;

            ifStatement
                =   IF >> expression >> THEN >> statement >>
                    ( ELSE >> statement
                    | empty
                    )
                ;

            caseStatement
                =   CASE >> expression >> OF >>
                    caseListElement >> *( SEMI >> caseListElement ) >>
                    END
                ;

            caseListElement
                =   caseLabelList >> COLON >> statement
                |   empty
                ;

            repetetiveStatement
                =   whileStatement
                |   repeatStatement
                |   forStatement
                ;

            whileStatement
                =   WHILE >> expression >> DO >>
                    statement
                ;

            repeatStatement
                =   REPEAT >>
                    statement >> *( SEMI >> statement ) >>
                    UNTIL >> expression
                ;

            forStatement
                =   FOR >> controlVariable >> ASSIGN >> forList >> DO >>
                    statement
                ;

            forList
                =   initialValue >> ( TO | DOWNTO ) >> finalValue
                ;

            controlVariable
                =   identifier
                ;

            initialValue
                =   expression
                ;

            finalValue
                =   expression
                ;

            withStatement
                =   WITH >> recordVariableList >> DO >>
                    statement
                ;

            recordVariableList
                =   recordVariable >> *( COMMA >> recordVariable )
                ;

            //-----------------------------------------------------------------
            //  End grammar definition
            //-----------------------------------------------------------------
        }

        #ifdef BOOST_SPIRIT_DEBUG
        void
        debug()
        {
            BOOST_SPIRIT_DEBUG_RULE(program);
            BOOST_SPIRIT_DEBUG_RULE(programHeading);
            BOOST_SPIRIT_DEBUG_RULE(fileIdentifier);
            BOOST_SPIRIT_DEBUG_RULE(identifier);
            BOOST_SPIRIT_DEBUG_RULE(block);
            BOOST_SPIRIT_DEBUG_RULE(labelDeclarationPart);
            BOOST_SPIRIT_DEBUG_RULE(label);
            BOOST_SPIRIT_DEBUG_RULE(constantDefinitionPart);
            BOOST_SPIRIT_DEBUG_RULE(constantDefinition);
            BOOST_SPIRIT_DEBUG_RULE(constant);
            BOOST_SPIRIT_DEBUG_RULE(unsignedNumber);
            BOOST_SPIRIT_DEBUG_RULE(unsignedInteger);
            BOOST_SPIRIT_DEBUG_RULE(unsignedReal);
            BOOST_SPIRIT_DEBUG_RULE(sign);
            BOOST_SPIRIT_DEBUG_RULE(constantIdentifier);
            BOOST_SPIRIT_DEBUG_RULE(string_literal);
            BOOST_SPIRIT_DEBUG_RULE(typeDefinitionPart);
            BOOST_SPIRIT_DEBUG_RULE(typeDefinition);
            BOOST_SPIRIT_DEBUG_RULE(type);
            BOOST_SPIRIT_DEBUG_RULE(simpleType);
            BOOST_SPIRIT_DEBUG_RULE(scalarType);
            BOOST_SPIRIT_DEBUG_RULE(subrangeType);
            BOOST_SPIRIT_DEBUG_RULE(typeIdentifier);
            BOOST_SPIRIT_DEBUG_RULE(structuredType);
            BOOST_SPIRIT_DEBUG_RULE(unpackedStructuredType);
            BOOST_SPIRIT_DEBUG_RULE(arrayType);
            BOOST_SPIRIT_DEBUG_RULE(indexType);
            BOOST_SPIRIT_DEBUG_RULE(componentType);
            BOOST_SPIRIT_DEBUG_RULE(recordType);
            BOOST_SPIRIT_DEBUG_RULE(fieldList);
            BOOST_SPIRIT_DEBUG_RULE(fixedPart);
            BOOST_SPIRIT_DEBUG_RULE(recordSection);
            BOOST_SPIRIT_DEBUG_RULE(variantPart);
            BOOST_SPIRIT_DEBUG_RULE(tagField);
            BOOST_SPIRIT_DEBUG_RULE(variant);
            BOOST_SPIRIT_DEBUG_RULE(caseLabelList);
            BOOST_SPIRIT_DEBUG_RULE(caseLabel);
            BOOST_SPIRIT_DEBUG_RULE(setType);
            BOOST_SPIRIT_DEBUG_RULE(baseType);
            BOOST_SPIRIT_DEBUG_RULE(fileType);
            BOOST_SPIRIT_DEBUG_RULE(pointerType);
            BOOST_SPIRIT_DEBUG_RULE(variableDeclarationPart);
            BOOST_SPIRIT_DEBUG_RULE(variableDeclaration);
            BOOST_SPIRIT_DEBUG_RULE(procedureAndFunctionDeclarationPart);
            BOOST_SPIRIT_DEBUG_RULE(procedureOrFunctionDeclaration);
            BOOST_SPIRIT_DEBUG_RULE(procedureDeclaration);
            BOOST_SPIRIT_DEBUG_RULE(procedureHeading);
            BOOST_SPIRIT_DEBUG_RULE(parameterList);
            BOOST_SPIRIT_DEBUG_RULE(formalParameterSection);
            BOOST_SPIRIT_DEBUG_RULE(parameterGroup);
            BOOST_SPIRIT_DEBUG_RULE(functionDeclaration);
            BOOST_SPIRIT_DEBUG_RULE(functionHeading);
            BOOST_SPIRIT_DEBUG_RULE(resultType);
            BOOST_SPIRIT_DEBUG_RULE(statementPart);
            BOOST_SPIRIT_DEBUG_RULE(statement);
            BOOST_SPIRIT_DEBUG_RULE(unlabelledStatement);
            BOOST_SPIRIT_DEBUG_RULE(simpleStatement);
            BOOST_SPIRIT_DEBUG_RULE(assignmentStatement);
            BOOST_SPIRIT_DEBUG_RULE(variable);
            BOOST_SPIRIT_DEBUG_RULE(entireVariable);
            BOOST_SPIRIT_DEBUG_RULE(variableIdentifier);
            BOOST_SPIRIT_DEBUG_RULE(componentVariable);
            BOOST_SPIRIT_DEBUG_RULE(indexedVariable);
            BOOST_SPIRIT_DEBUG_RULE(arrayVariable);
            BOOST_SPIRIT_DEBUG_RULE(fieldDesignator);
            BOOST_SPIRIT_DEBUG_RULE(recordVariable);
            BOOST_SPIRIT_DEBUG_RULE(fieldIdentifier);
            BOOST_SPIRIT_DEBUG_RULE(fileBuffer);
            BOOST_SPIRIT_DEBUG_RULE(fileVariable);
            BOOST_SPIRIT_DEBUG_RULE(referencedVariable);
            BOOST_SPIRIT_DEBUG_RULE(pointerVariable);
            BOOST_SPIRIT_DEBUG_RULE(expression);
            BOOST_SPIRIT_DEBUG_RULE(relationalOperator);
            BOOST_SPIRIT_DEBUG_RULE(simpleExpression);
            BOOST_SPIRIT_DEBUG_RULE(addingOperator);
            BOOST_SPIRIT_DEBUG_RULE(term);
            BOOST_SPIRIT_DEBUG_RULE(multiplyingOperator);
            BOOST_SPIRIT_DEBUG_RULE(factor);
            BOOST_SPIRIT_DEBUG_RULE(unsignedConstant);
            BOOST_SPIRIT_DEBUG_RULE(functionDesignator);
            BOOST_SPIRIT_DEBUG_RULE(functionIdentifier);
            BOOST_SPIRIT_DEBUG_RULE(set);
            BOOST_SPIRIT_DEBUG_RULE(elementList);
            BOOST_SPIRIT_DEBUG_RULE(element);
            BOOST_SPIRIT_DEBUG_RULE(procedureStatement);
            BOOST_SPIRIT_DEBUG_RULE(procedureIdentifier);
            BOOST_SPIRIT_DEBUG_RULE(actualParameter);
            BOOST_SPIRIT_DEBUG_RULE(gotoStatement);
            BOOST_SPIRIT_DEBUG_RULE(emptyStatement);
            BOOST_SPIRIT_DEBUG_RULE(empty);
            BOOST_SPIRIT_DEBUG_RULE(structuredStatement);
            BOOST_SPIRIT_DEBUG_RULE(compoundStatement);
            BOOST_SPIRIT_DEBUG_RULE(conditionalStatement);
            BOOST_SPIRIT_DEBUG_RULE(ifStatement);
            BOOST_SPIRIT_DEBUG_RULE(caseStatement);
            BOOST_SPIRIT_DEBUG_RULE(caseListElement);
            BOOST_SPIRIT_DEBUG_RULE(repetetiveStatement);
            BOOST_SPIRIT_DEBUG_RULE(whileStatement);
            BOOST_SPIRIT_DEBUG_RULE(repeatStatement);
            BOOST_SPIRIT_DEBUG_RULE(forStatement);
            BOOST_SPIRIT_DEBUG_RULE(forList);
            BOOST_SPIRIT_DEBUG_RULE(controlVariable);
            BOOST_SPIRIT_DEBUG_RULE(initialValue);
            BOOST_SPIRIT_DEBUG_RULE(finalValue);
            BOOST_SPIRIT_DEBUG_RULE(withStatement);
            BOOST_SPIRIT_DEBUG_RULE(recordVariableList);
        }
        #endif

        rule<ScannerT> const&
        start() const { return program; }

        symbols<> keywords;
        rule<ScannerT>
            string_literal, program, programHeading, fileIdentifier,
            identifier, block, labelDeclarationPart, label,
            constantDefinitionPart, constantDefinition, constant,
            unsignedNumber, unsignedInteger, unsignedReal, sign,
            constantIdentifier, typeDefinitionPart, typeDefinition, type,
            simpleType, scalarType, subrangeType, typeIdentifier,
            structuredType, unpackedStructuredType, arrayType, indexType,
            componentType, recordType, fieldList, fixedPart, recordSection,
            variantPart, tagField, variant, caseLabelList, caseLabel,
            setType, baseType, fileType, pointerType,
            variableDeclarationPart, variableDeclaration,
            procedureAndFunctionDeclarationPart,
            procedureOrFunctionDeclaration, procedureDeclaration,
            procedureHeading, parameterList, formalParameterSection,
            parameterGroup, functionDeclaration, functionHeading,
            resultType, statementPart, statement, unlabelledStatement,
            simpleStatement, assignmentStatement, variable, entireVariable,
            variableIdentifier, componentVariable, indexedVariable,
            arrayVariable, fieldDesignator, recordVariable,
            fieldIdentifier, fileBuffer, fileVariable, referencedVariable,
            pointerVariable, expression, relationalOperator,
            simpleExpression, addingOperator, term, multiplyingOperator,
            factor, unsignedConstant, functionDesignator,
            functionIdentifier, set, elementList, element,
            procedureStatement, procedureIdentifier, actualParameter,
            gotoStatement, emptyStatement, empty, structuredStatement,
            compoundStatement, conditionalStatement, ifStatement,
            caseStatement, caseListElement, repetetiveStatement,
            whileStatement, repeatStatement, forStatement, forList,
            controlVariable, initialValue, finalValue, withStatement,
            recordVariableList;
    };
};

///////////////////////////////////////////////////////////////////////////////
//
//  The Pascal White Space Skipper
//
///////////////////////////////////////////////////////////////////////////////
struct pascal_skipper : public grammar<pascal_skipper>
{
    pascal_skipper() {}

    template <typename ScannerT>
    struct definition
    {
        definition(pascal_skipper const& /*self*/)
        {
            skip
                =   space_p
                |   '{' >> (*(anychar_p - '}')) >> '}'      //  pascal comment 1
                |   "(*" >> (*(anychar_p - "*)")) >> "*)"   //  pascal comment 2
            ;

            #ifdef BOOST_SPIRIT_DEBUG
            BOOST_SPIRIT_DEBUG_RULE(skip);
            #endif
        }

        rule<ScannerT>  skip;
        rule<ScannerT> const&
        start() const { return skip; }
    };
};

///////////////////////////////////////////////////////////////////////////////
//
//  Parse a file
//
///////////////////////////////////////////////////////////////////////////////
static void
parse(char const* filename)
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

    vector<char>::const_iterator first = vec.begin();
    vector<char>::const_iterator last = vec.end();

    pascal_skipper skip_p;
    pascal_grammar p;

#ifdef BOOST_SPIRIT_DEBUG
    BOOST_SPIRIT_DEBUG_NODE(skip_p);
    BOOST_SPIRIT_DEBUG_NODE(p);
#endif

    parse_info<vector<char>::const_iterator> info =
        parse(first, last, p, skip_p);

    if (info.full)
    {
        cout << "\t\t" << filename << " Parses OK\n\n\n";
    }
    else
    {
        cerr << "---PARSING FAILURE---\n";
        cerr << string(info.stop, last);
    }
}

///////////////////////////////////////////////////////////////////////////////
//
//  Main program
//
///////////////////////////////////////////////////////////////////////////////
int
main(int argc, char* argv[])
{
    cout << "/////////////////////////////////////////////////////////\n\n";
    cout << "\t\tPascal Grammar For Spirit...\n\n";
    cout << "/////////////////////////////////////////////////////////\n\n";

    if (argc > 1)
    {
        for (int i = 1; i < argc; ++i)
        {
            cout << argv[i] << endl;
            parse(argv[i]);
        }
    }
    else
    {
        cerr << "---NO FILENAME GIVEN---" << endl;
    }

    return 0;
}

