#ifndef ISO8859_XML_GRAMMAR_HPP_
#define ISO8859_XML_GRAMMAR_HPP_

#include <boost/spirit/iterator/multi_pass.hpp>

using namespace boost::spirit;

struct xml_grammar : public boost::spirit::grammar<xml_grammar>
{
    template <typename IteratorT, typename MatchPolicyT = boost::spirit::match_policy>
    struct definition
    {
        typedef typename std::iterator_traits<IteratorT>::value_type char_t;
        typedef boost::spirit::chset<unsigned char> chset_t;

        boost::spirit::rule<IteratorT, MatchPolicyT>
            prolog, element, Misc, PEReference, Reference, PITarget, CData,
        doctypedecl, XMLDecl, SDDecl, VersionInfo, EncodingDecl, VersionNum,
        Eq, DeclSep, ExternalID, markupdecl, NotationDecl, EntityDecl,
        AttlistDecl, elementdecl, TextDecl, extSubsetDecl, conditionalSect,
        EmptyElemTag, STag, content, ETag, Attribute, contentspec, Mixed,
        children, choice, seq, cp, AttDef, AttType, DefaultDecl, StringType,
        TokenizedType, EnumeratedType, NotationType, Enumeration, EntityValue,
        AttValue, SystemLiteral, PubidLiteral, CharDataChar, CharData, Comment,
        PI, CDSect, extSubset, includeSect, ignoreSect, ignoreSectContents,
        Ignore, CharRef, EntityRef, GEDecl, PEDecl, EntityDef, PEDef,
        NDataDecl, extParsedEnt, EncName, PublicID, document, S, Name, Names,
        Nmtoken, Nmtokens;

        definition()
        {
            // XML Char sets
            chset_t Char("\x9\xA\xD\x20-\xFF");
            chset_t Sch("\x20\x9\xD\xA");
            chset_t Letter("\x41-\x5A\x61-\x7A\xC0-\xD6\xD8-\xF6\xF8-\xFF");
            chset_t Digit("\x30-\x39");
            chset_t Extender("\xB7");
            chset_t NameChar =
                  Letter 
                | Digit 
                | (unsigned char)'.'
                | (unsigned char)'-'
                | (unsigned char)'_'
                | (unsigned char)':'
                | Extender;

            document =
                prolog >> element >> *Misc
            ;

            S = 
                +(Sch)
            ;

            Name =
                (Letter | '_' | ':') 
                >> *(NameChar)
            ;

            Names =
                Name >> *(S >> Name)
            ;

            Nmtoken =
                +NameChar
            ;

            Nmtokens =
                Nmtoken >> *(S >> Nmtoken)
            ;

            EntityValue =
                  '"'  >> *(  (anychar - (chset_t("%&\""))) 
                             | PEReference
                             | Reference) 
                       >> '"'
                | '\'' >> *(  (anychar - (chset_t("%&'"))) 
                             | PEReference
                             | Reference) 
                        >> '\''
            ;

            AttValue = 
                  '"' >> *(  (anychar - (chset_t("<&\""))) 
                            | Reference) 
                       >> '"'
                | '\'' >> *(  (anychar - (chset_t("<&'"))) 
                             | Reference) 
                        >> '\''
            ;

            SystemLiteral= 
                  ('"' >> *(anychar - '"') >> '"')
                | ('\'' >> *(anychar - '\'') >> '\'')
            ;

            chset_t PubidChar("\x20\xD\xA'a-zA-Z0-9()+,./:=?;!*#@$_%-");

            PubidLiteral = 
                  '"' >> *PubidChar >> '"' 
                | '\'' >> *(PubidChar - '\'') >> '\''
            ;

            CharDataChar = 
                anychar - (chset_t("<&"))
            ;

            CharData =
                *(CharDataChar - "]]>")
            ;

            Comment = 
                "<!--" >> 
                  *(
                      (Char - '-') 
                    | ('-' >> (Char - '-'))
                   ) 
                  >> "-->"
            ;

            PI = 
                "<?" >> PITarget >> !(S >> (*(Char - "?>"))) >> "?>"
            ;

            PITarget =
                Name - (as_lower_d["xml"])
            ;

            CDSect =
                "<![CDATA[" >> CData >> "]]>"
            ;

            CData =
                *(Char - "]]>")
            ;

            prolog =
                !XMLDecl >> *Misc >> !(doctypedecl >> *Misc)
            ;

            XMLDecl =
                "<?xml" >> VersionInfo >> !EncodingDecl >> !SDDecl 
                >> !S >> "?>"
            ;

            VersionInfo = 
                S >> "version" >> Eq >> 
                (
                   '\'' >> VersionNum >> '\''
                 | '"' >> VersionNum >> '"'
                )
            ;

            Eq =
                !S >> '=' >> !S
            ;

            chset_t VersionNumCh("a-zA-Z0-9_.:-");

            VersionNum =
                +(VersionNumCh)
            ;

            Misc =
                  Comment 
                | PI 
                | S
            ;

            doctypedecl =
                "<!DOCTYPE" >> S >> Name >> !(S >> ExternalID) >> !S >> 
                !(
                  '[' >> *(markupdecl | DeclSep) >> ']' >> !S
                ) 
                >> '>'
            ;

            DeclSep =
                  PEReference 
                | S
            ;

            markupdecl =
                  elementdecl 
                | AttlistDecl 
                | EntityDecl 
                | NotationDecl 
                | PI 
                | Comment
            ;

            extSubset =
                !TextDecl >> extSubsetDecl
            ;

            extSubsetDecl =
                *(
                    markupdecl 
                  | conditionalSect 
                  | DeclSep
                )
            ;

            SDDecl = 
                S >> "standalone" >> Eq >> 
                (
                   ('\'' >> (str_p("yes") | "no") >> '\'')
                 | ('"' >> (str_p("yes") | "no") >> '"')
                )
            ;

            element =
                  EmptyElemTag
                | STag >> content >> ETag
            ;

            STag =
                '<' >> Name >> *(S >> Attribute) >> !S >> '>'
            ;

            Attribute =
                Name >> Eq >> AttValue
            ;

            ETag =
                "</" >> Name >> !S >> '>'
            ;

            content =
                !CharData >> 
                *(
                  (
                     element 
                   | Reference 
                   | CDSect 
                   | PI 
                   | Comment
                  ) >> !CharData
                 )
            ;

            EmptyElemTag =
                '<' >> Name >> *(S >> Attribute) >> !S >> "/>"
            ;

            elementdecl = 
                "<!ELEMENT" >> S >> Name >> S >> contentspec >> !S >> '>'
            ;

            contentspec = 
                  str_p("EMPTY") 
                | "ANY" 
                | Mixed 
                | children
            ;

            children =
                (choice | seq) >> !(ch_p('?') | '*' | '+')
            ;

            cp = 
                (Name | choice | seq) >> !(ch_p('?') | '*' | '+')
            ;

            choice = 
                '(' >> !S >> cp 
                  >> +(!S >> '|' >> !S >> cp) 
                  >> !S >> ')'
            ;

            seq =
                '(' >> !S >> cp >> 
                  *(!S >> ',' >> !S >> cp) 
                  >> !S >> ')'
            ;

            Mixed =
                  '(' >> !S >> "#PCDATA" 
                      >> *(!S >> '|' >> !S >> Name) 
                      >> !S >> ")*"
                | '(' >> !S >> "#PCDATA" >> !S >> ')'
            ;

            AttlistDecl =
                "<!ATTLIST" >> S >> Name >> *AttDef >> !S >> '>'
            ;

            AttDef =
                S >> Name >> S >> AttType >> S >> DefaultDecl
            ;

            AttType =
                  StringType 
                | TokenizedType 
                | EnumeratedType
            ;

            StringType =
                str_p("CDATA")
            ;

            TokenizedType =
                longest_d[ 
                      str_p("ID") 
                    | "IDREF" 
                    | "IDREFS" 
                    | "ENTITY" 
                    | "ENTITIES" 
                    | "NMTOKEN"
                    | "NMTOKENS" 
                ]
            ;

            EnumeratedType =
                  NotationType 
                | Enumeration
            ;

            NotationType =
                "NOTATION" >> S >> '(' >> !S >> Name 
                  >> *(!S >> '|' >> !S >> Name) 
                  >> !S >> ')'
            ;

            Enumeration = 
                '(' >> !S >> Nmtoken 
                >> *(!S >> '|' >> !S >> Nmtoken) 
                >> !S >> ')'
            ;

            DefaultDecl =
                  str_p("#REQUIRED") 
                | "#IMPLIED" 
                | !("#FIXED" >> S) >> AttValue
            ;

            conditionalSect =
                  includeSect 
                | ignoreSect
            ;

            includeSect =
                "<![" >> !S >> "INCLUDE" >> !S 
                >> '[' >> extSubsetDecl >> "]]>"
            ;

            ignoreSect = 
                "<![" >> !S >> "IGNORE"  >> !S 
                >> '[' >> *ignoreSectContents >> "]]>"
            ;

            ignoreSectContents = 
                Ignore >> *("<![" >> ignoreSectContents >> "]]>" >> Ignore)
            ;

            Ignore = 
                *(Char - (str_p("<![") | "]]>"))
            ;

            CharRef = 
                  "&#"  >> +digit  >> ';'
                | "&#x" >> +xdigit >> ';'
            ;

            Reference =
                  EntityRef 
                | CharRef
            ;

            EntityRef =
                '&' >> Name >> ';'
            ;

            PEReference =
                '%' >> Name >> ';'
            ;

            EntityDecl =
                  GEDecl 
                | PEDecl
            ;

            GEDecl =
                "<!ENTITY" >> S >> Name >> S >> EntityDef >> !S >> '>'
            ;

            PEDecl =
                "<!ENTITY" >> S >> '%' >> S >> Name >> S >> PEDef 
                >> !S >> '>'
            ;

            EntityDef =
                  EntityValue
                | ExternalID >> !NDataDecl
            ;

            PEDef =
                  EntityValue 
                | ExternalID
            ;

            ExternalID =
                  "SYSTEM" >> S >> SystemLiteral
                | "PUBLIC" >> S >> PubidLiteral >> S >> SystemLiteral
            ;

            NDataDecl =
                S >> "NDATA" >> S >> Name
            ;

            TextDecl =
                "<?xml" >> !VersionInfo >> EncodingDecl >> !S >> "?>"
            ;

            extParsedEnt =
                !TextDecl >> content
            ;

            EncodingDecl =
                S >> "encoding" >> Eq 
                >> (  '"' >> EncName >> '"' 
                    | '\'' >> EncName >> '\''
                   )
            ;

            EncName =
                alpha >> *(alnum | '.' | '_' | '-')
            ;

            NotationDecl =
                "<!NOTATION" >> S >> Name >> S 
                >> (ExternalID | PublicID) >> !S >> '>'
            ;

            PublicID =
                "PUBLIC" >> S >> PubidLiteral
            ;


            BOOST_SPIRIT_DEBUG_RULE(document);
            BOOST_SPIRIT_DEBUG_RULE(prolog);
            BOOST_SPIRIT_DEBUG_RULE(element);
            BOOST_SPIRIT_DEBUG_RULE(Misc);
            BOOST_SPIRIT_DEBUG_RULE(PEReference);
            BOOST_SPIRIT_DEBUG_RULE(Reference);
            BOOST_SPIRIT_DEBUG_RULE(PITarget);
            BOOST_SPIRIT_DEBUG_RULE(CData);
            BOOST_SPIRIT_DEBUG_RULE(doctypedecl);
            BOOST_SPIRIT_DEBUG_RULE(XMLDecl);
            BOOST_SPIRIT_DEBUG_RULE(SDDecl);
            BOOST_SPIRIT_DEBUG_RULE(VersionInfo);
            BOOST_SPIRIT_DEBUG_RULE(EncodingDecl);
            BOOST_SPIRIT_DEBUG_RULE(VersionNum);
            BOOST_SPIRIT_DEBUG_RULE(Eq);
            BOOST_SPIRIT_DEBUG_RULE(DeclSep);
            BOOST_SPIRIT_DEBUG_RULE(ExternalID);
            BOOST_SPIRIT_DEBUG_RULE(markupdecl);
            BOOST_SPIRIT_DEBUG_RULE(NotationDecl);
            BOOST_SPIRIT_DEBUG_RULE(EntityDecl);
            BOOST_SPIRIT_DEBUG_RULE(AttlistDecl);
            BOOST_SPIRIT_DEBUG_RULE(elementdecl);
            BOOST_SPIRIT_DEBUG_RULE(TextDecl);
            BOOST_SPIRIT_DEBUG_RULE(extSubsetDecl);
            BOOST_SPIRIT_DEBUG_RULE(conditionalSect);
            BOOST_SPIRIT_DEBUG_RULE(EmptyElemTag);
            BOOST_SPIRIT_DEBUG_RULE(STag);
            BOOST_SPIRIT_DEBUG_RULE(content);
            BOOST_SPIRIT_DEBUG_RULE(ETag);
            BOOST_SPIRIT_DEBUG_RULE(Attribute);
            BOOST_SPIRIT_DEBUG_RULE(contentspec);
            BOOST_SPIRIT_DEBUG_RULE(Mixed);
            BOOST_SPIRIT_DEBUG_RULE(children);
            BOOST_SPIRIT_DEBUG_RULE(choice);
            BOOST_SPIRIT_DEBUG_RULE(seq);
            BOOST_SPIRIT_DEBUG_RULE(cp);
            BOOST_SPIRIT_DEBUG_RULE(AttDef);
            BOOST_SPIRIT_DEBUG_RULE(AttType);
            BOOST_SPIRIT_DEBUG_RULE(DefaultDecl);
            BOOST_SPIRIT_DEBUG_RULE(StringType);
            BOOST_SPIRIT_DEBUG_RULE(TokenizedType);
            BOOST_SPIRIT_DEBUG_RULE(EnumeratedType);
            BOOST_SPIRIT_DEBUG_RULE(NotationType);
            BOOST_SPIRIT_DEBUG_RULE(Enumeration);
            BOOST_SPIRIT_DEBUG_RULE(EntityValue);
            BOOST_SPIRIT_DEBUG_RULE(AttValue);
            BOOST_SPIRIT_DEBUG_RULE(SystemLiteral);
            BOOST_SPIRIT_DEBUG_RULE(PubidLiteral);
            BOOST_SPIRIT_DEBUG_RULE(CharDataChar);
            BOOST_SPIRIT_DEBUG_RULE(CharData);
            BOOST_SPIRIT_DEBUG_RULE(Comment);
            BOOST_SPIRIT_DEBUG_RULE(PI);
            BOOST_SPIRIT_DEBUG_RULE(CDSect);
            BOOST_SPIRIT_DEBUG_RULE(extSubset);
            BOOST_SPIRIT_DEBUG_RULE(includeSect);
            BOOST_SPIRIT_DEBUG_RULE(ignoreSect);
            BOOST_SPIRIT_DEBUG_RULE(ignoreSectContents);
            BOOST_SPIRIT_DEBUG_RULE(Ignore);
            BOOST_SPIRIT_DEBUG_RULE(CharRef);
            BOOST_SPIRIT_DEBUG_RULE(EntityRef);
            BOOST_SPIRIT_DEBUG_RULE(GEDecl);
            BOOST_SPIRIT_DEBUG_RULE(PEDecl);
            BOOST_SPIRIT_DEBUG_RULE(EntityDef);
            BOOST_SPIRIT_DEBUG_RULE(PEDef);
            BOOST_SPIRIT_DEBUG_RULE(NDataDecl);
            BOOST_SPIRIT_DEBUG_RULE(extParsedEnt);
            BOOST_SPIRIT_DEBUG_RULE(EncName);
            BOOST_SPIRIT_DEBUG_RULE(PublicID);
            BOOST_SPIRIT_DEBUG_RULE(document);
            BOOST_SPIRIT_DEBUG_RULE(S);
            BOOST_SPIRIT_DEBUG_RULE(Name);
            BOOST_SPIRIT_DEBUG_RULE(Names);
            BOOST_SPIRIT_DEBUG_RULE(Nmtoken);
            BOOST_SPIRIT_DEBUG_RULE(Nmtokens);

        }

        boost::spirit::rule<IteratorT, MatchPolicyT> const&
        start() const
        {
            return document;
        }

    };
};

#endif

