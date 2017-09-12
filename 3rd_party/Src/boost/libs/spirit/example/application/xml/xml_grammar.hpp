#ifndef XML_GRAMMAR_HPP_
#define XML_GRAMMAR_HPP_

#include <boost/spirit/iterator/multi_pass.hpp>
#include <boost/spirit/utility/flush_multi_pass.hpp>
#include <boost/spirit.hpp>
#include <boost/spirit/tree/ast.hpp>

using namespace boost::spirit;

struct xml_grammar : public boost::spirit::grammar<xml_grammar>
{
    template <typename ScannerT>
    struct definition
    {
        typedef typename std::iterator_traits<typename ScannerT::iterator_t>::value_type char_t;
        typedef boost::spirit::chset<wchar_t> chset_t;

        // non-ast generating rules
        typedef typename ScannerT::iteration_policy_t iteration_policy_t;
        typedef match_policy match_policy_t;
        typedef typename ScannerT::action_policy_t action_policy_t;
        typedef scanner_policies<
            iteration_policy_t,
            match_policy_t,
            action_policy_t
        > policies_t;
        typedef scanner<typename ScannerT::iterator_t, policies_t> non_tree_scanner_t;

        boost::spirit::rule<non_tree_scanner_t>
            PEReference2, Reference2, EntityRef2, CharRef2, Name2;

        boost::spirit::rule<ScannerT>
            prolog, element, Misc, PITarget, CData, Reference, EntityRef,
        doctypedecl, XMLDecl, SDDecl, VersionInfo, EncodingDecl, VersionNum,
        Eq, DeclSep, ExternalID, markupdecl, NotationDecl, EntityDecl,
        AttlistDecl, elementdecl, TextDecl, extSubsetDecl, conditionalSect,
        EmptyElemTag, STag, content, ETag, Attribute, contentspec, Mixed,
        children, choice, seq, cp, AttDef, AttType, DefaultDecl, StringType,
        TokenizedType, EnumeratedType, NotationType, Enumeration, EntityValue,
        AttValue, SystemLiteral, PubidLiteral, CharData, Comment,
        PI, CDSect, extSubset, includeSect, ignoreSect, ignoreSectContents,
        Ignore, GEDecl, PEDecl, EntityDef, PEDef, CharRef, PEReference,
        NDataDecl, extParsedEnt, EncName, PublicID, document, S, Name, Names,
        Nmtoken, Nmtokens;

        definition(xml_grammar const& /*self*/)
        {

            document =
                prolog >> element >> *Misc;

// basically only gcc on linux has 4 byte wide chars
#if defined(__GNUC__) && defined(linux)
            chset_t Char(L"\x9\xA\xD\x20-\xD7FF\xE000-\xFFFD\x10000-\x10FFFF");
#else
            chset_t Char(L"\x9\xA\xD\x20-\xD7FF\xE000-\xFFFD");
#endif

            chset_t Sch(L"\x20\x9\xD\xA");
            S = 
                no_node_d[
                    +(Sch)
                ];

            chset_t BaseChar (
        L"\x41-\x5A\x61-\x7A\xC0-\xD6\xD8-\xF6\xF8-\xFF\x100-\x131\x134-\x13E"
        L"\x141-\x148\x14A-\x17E\x180-\x1C3\x1CD-\x1F0\x1F4-\x1F5\x1FA-\x217"
        L"\x250-\x2A8\x2BB-\x2C1\x386\x388-\x38A\x38C\x38E-\x3A1\x3A3-\x3CE"
        L"\x3D0-\x3D6\x3DA\x3DC\x3DE\x3E0\x3E2-\x3F3\x401-\x40C\x40E-\x44F"
        L"\x451-\x45C\x45E-\x481\x490-\x4C4\x4C7-\x4C8\x4CB-\x4CC\x4D0-\x4EB"
        L"\x4EE-\x4F5\x4F8-\x4F9\x531-\x556\x559\x561-\x586\x5D0-\x5EA"
        L"\x5F0-\x5F2\x621-\x63A\x641-\x64A\x671-\x6B7\x6BA-\x6BE\x6C0-\x6CE"
        L"\x6D0-\x6D3\x6D5\x6E5-\x6E6\x905-\x939\x93D\x958-\x961\x985-\x98C"
        L"\x98F-\x990\x993-\x9A8\x9AA-\x9B0\x9B2\x9B6-\x9B9\x9DC-\x9DD"
        L"\x9DF-\x9E1\x9F0-\x9F1\xA05-\xA0A\xA0F-\xA10\xA13-\xA28\xA2A-\xA30"
        L"\xA32-\xA33\xA35-\xA36\xA38-\xA39\xA59-\xA5C\xA5E\xA72-\xA74"
        L"\xA85-\xA8B\xA8D\xA8F-\xA91\xA93-\xAA8\xAAA-\xAB0\xAB2-\xAB3"
        L"\xAB5-\xAB9\xABD\xAE0\xB05-\xB0C\xB0F-\xB10\xB13-\xB28\xB2A-\xB30"
        L"\xB32-\xB33\xB36-\xB39\xB3D\xB5C-\xB5D\xB5F-\xB61\xB85-\xB8A"
        L"\xB8E-\xB90\xB92-\xB95\xB99-\xB9A\xB9C\xB9E-\xB9F\xBA3-\xBA4"
        L"\xBA8-\xBAA\xBAE-\xBB5\xBB7-\xBB9\xC05-\xC0C\xC0E-\xC10\xC12-\xC28"
        L"\xC2A-\xC33\xC35-\xC39\xC60-\xC61\xC85-\xC8C\xC8E-\xC90\xC92-\xCA8"
        L"\xCAA-\xCB3\xCB5-\xCB9\xCDE\xCE0-\xCE1\xD05-\xD0C\xD0E-\xD10"
        L"\xD12-\xD28\xD2A-\xD39\xD60-\xD61\xE01-\xE2E\xE30\xE32-\xE33"
        L"\xE40-\xE45\xE81-\xE82\xE84\xE87-\xE88\xE8A\xE8D\xE94-\xE97"
        L"\xE99-\xE9F\xEA1-\xEA3\xEA5\xEA7\xEAA-\xEAB\xEAD-\xEAE\xEB0"
        L"\xEB2-\xEB3\xEBD\xEC0-\xEC4\xF40-\xF47\xF49-\xF69\x10A0-\x10C5"
        L"\x10D0-\x10F6\x1100\x1102-\x1103\x1105-\x1107\x1109\x110B-\x110C"
        L"\x110E-\x1112\x113C\x113E\x1140\x114C\x114E\x1150\x1154-\x1155"
        L"\x1159\x115F-\x1161\x1163\x1165\x1167\x1169\x116D-\x116E"
        L"\x1172-\x1173\x1175\x119E\x11A8\x11AB\x11AE-\x11AF\x11B7-\x11B8"
        L"\x11BA\x11BC-\x11C2\x11EB\x11F0\x11F9\x1E00-\x1E9B\x1EA0-\x1EF9"
        L"\x1F00-\x1F15\x1F18-\x1F1D\x1F20-\x1F45\x1F48-\x1F4D\x1F50-\x1F57"
        L"\x1F59\x1F5B\x1F5D\x1F5F-\x1F7D\x1F80-\x1FB4\x1FB6-\x1FBC\x1FBE"
        L"\x1FC2-\x1FC4\x1FC6-\x1FCC\x1FD0-\x1FD3\x1FD6-\x1FDB\x1FE0-\x1FEC"
        L"\x1FF2-\x1FF4\x1FF6-\x1FFC\x2126\x212A-\x212B\x212E\x2180-\x2182"
        L"\x3041-\x3094\x30A1-\x30FA\x3105-\x312C\xAC00-\xD7A3");

            chset_t Ideographic(L"\x4E00-\x9FA5\x3007\x3021-\x3029");
            chset_t Letter =
                BaseChar | Ideographic;
            chset_t CombiningChar(
        L"\x0300-\x0345\x0360-\x0361\x0483-\x0486\x0591-\x05A1\x05A3-\x05B9"
        L"\x05BB-\x05BD\x05BF\x05C1-\x05C2\x05C4\x064B-\x0652\x0670"
        L"\x06D6-\x06DC\x06DD-\x06DF\x06E0-\x06E4\x06E7-\x06E8\x06EA-\x06ED"
        L"\x0901-\x0903\x093C\x093E-\x094C\x094D\x0951-\x0954\x0962-\x0963"
        L"\x0981-\x0983\x09BC\x09BE\x09BF\x09C0-\x09C4\x09C7-\x09C8"
        L"\x09CB-\x09CD\x09D7\x09E2-\x09E3\x0A02\x0A3C\x0A3E\x0A3F"
        L"\x0A40-\x0A42\x0A47-\x0A48\x0A4B-\x0A4D\x0A70-\x0A71\x0A81-\x0A83"
        L"\x0ABC\x0ABE-\x0AC5\x0AC7-\x0AC9\x0ACB-\x0ACD\x0B01-\x0B03\x0B3C"
        L"\x0B3E-\x0B43\x0B47-\x0B48\x0B4B-\x0B4D\x0B56-\x0B57\x0B82-\x0B83"
        L"\x0BBE-\x0BC2\x0BC6-\x0BC8\x0BCA-\x0BCD\x0BD7\x0C01-\x0C03"
        L"\x0C3E-\x0C44\x0C46-\x0C48\x0C4A-\x0C4D\x0C55-\x0C56\x0C82-\x0C83"
        L"\x0CBE-\x0CC4\x0CC6-\x0CC8\x0CCA-\x0CCD\x0CD5-\x0CD6\x0D02-\x0D03"
        L"\x0D3E-\x0D43\x0D46-\x0D48\x0D4A-\x0D4D\x0D57\x0E31\x0E34-\x0E3A"
        L"\x0E47-\x0E4E\x0EB1\x0EB4-\x0EB9\x0EBB-\x0EBC\x0EC8-\x0ECD"
        L"\x0F18-\x0F19\x0F35\x0F37\x0F39\x0F3E\x0F3F\x0F71-\x0F84"
        L"\x0F86-\x0F8B\x0F90-\x0F95\x0F97\x0F99-\x0FAD\x0FB1-\x0FB7\x0FB9"
        L"\x20D0-\x20DC\x20E1\x302A-\x302F\x3099\x309A");

            chset_t Digit(
        L"\x0030-\x0039\x0660-\x0669\x06F0-\x06F9\x0966-\x096F\x09E6-\x09EF"
        L"\x0A66-\x0A6F\x0AE6-\x0AEF\x0B66-\x0B6F\x0BE7-\x0BEF\x0C66-\x0C6F"
        L"\x0CE6-\x0CEF\x0D66-\x0D6F\x0E50-\x0E59\x0ED0-\x0ED9\x0F20-\x0F29");

            chset_t Extender(
        L"\x00B7\x02D0\x02D1\x0387\x0640\x0E46\x0EC6\x3005\x3031-\x3035"
        L"\x309D-\x309E\x30FC-\x30FE");

            chset_t NameChar =
                  Letter 
                | Digit 
                | L'.'
                | L'-'
                | L'_'
                | L':'
                | CombiningChar 
                | Extender;

            Name =
                no_node_d[
                    (Letter | L'_' | L':') >> *(NameChar)
                ];

            Name2 =
                (Letter | L'_' | L':') >> *(NameChar);

            Names =
                infix_node_d[
                    Name % S
                    //Name >> *(S >> Name)
                ];

            Nmtoken =
                no_node_d[
                    +NameChar
                ];

            Nmtokens =
                infix_node_d[
                    Nmtoken  % S
                    //Nmtoken >> *(S >> Nmtoken)
                ];

            EntityValue =
                no_node_d[
                      L'"' >> *(  (anychar_p - (chset_t(L"%&\""))) 
                                | PEReference2
                                | Reference2) 
                           >> L'"'
                    | L'\'' >> *(  (anychar_p - (chset_t(L"%&'"))) 
                                 | PEReference2
                                 | Reference2) 
                            >> L'\''
                ];

            AttValue = 
                no_node_d[
                      L'"' >> *(  (anychar_p - (chset_t(L"<&\""))) 
                                | Reference2) 
                           >> L'"'
                    | L'\'' >> *(  (anychar_p - (chset_t(L"<&'"))) 
                                 | Reference2) 
                            >> L'\''
                ];

            SystemLiteral= 
                no_node_d[
                      (L'"' >> *(anychar_p - L'"') >> L'"')
                    | (L'\'' >> *(anychar_p - L'\'') >> L'\'')
                ];

            chset_t PubidChar(L"\x20\xD\xA'a-zA-Z0-9()+,./:=?;!*#@$_%-");

            PubidLiteral = 
                no_node_d[
                      L'"' >> *PubidChar >> L'"' 
                    | L'\'' >> *(PubidChar - L'\'') >> L'\''
                ];

            chset_t CharDataChar = 
                anychar_p - (chset_t(L"<&"));

            CharData =
                no_node_d[
                    *(CharDataChar - L"]]>")
                ];

            Comment = 
                no_node_d[
                    L"<!--" >> 
                      *(
                          (Char - L'-') 
                        | (L'-' >> (Char - L'-'))
                       ) 
                      >> L"-->"
                ];

            PI = 
                token_node_d[
                    L"<?" >> PITarget >> !(S >> (*(Char - L"?>"))) >> L"?>"
                ];

            PITarget =
                Name - (as_lower_d[L"xml"]);

            CDSect =
                L"<![CDATA[" >> CData >> L"]]>";

            CData =
                no_node_d[
                    *(Char - L"]]>")
                ];

            prolog =
                !XMLDecl >> *Misc >> !(doctypedecl >> *Misc);

            XMLDecl =
                L"<?xml" >> VersionInfo >> !EncodingDecl >> !SDDecl 
                >> !S >> L"?>";

            VersionInfo = 
                S >> L"version" >> Eq >> 
                (
                   L'\'' >> VersionNum >> L'\''
                 | L'"' >> VersionNum >> L'"'
                );

            Eq =
                !S >> L'=' >> !S;

            chset_t VersionNumCh(L"a-zA-Z0-9_.:-");

            VersionNum =
                no_node_d[
                    +(VersionNumCh)
                ];

            Misc =
                  Comment 
                | PI 
                | S;

            doctypedecl =
                L"<!DOCTYPE" >> S >> Name >> !(S >> ExternalID) >> !S >> 
                !(
                  L'[' >> *(markupdecl | DeclSep) >> L']' >> !S
                ) 
                >> L'>';

            DeclSep =
                  PEReference
                | S;

            markupdecl =
                  elementdecl 
                | AttlistDecl 
                | EntityDecl 
                | NotationDecl 
                | PI 
                | Comment;

            extSubset =
                !TextDecl >> extSubsetDecl;

            extSubsetDecl =
                *(
                    markupdecl 
                  | conditionalSect 
                  | DeclSep
                );

            SDDecl = 
                S >> L"standalone" >> Eq >> 
                (
                   (L'\'' >> (str_p(L"yes") | L"no") >> L'\'')
                 | (L'"' >> (str_p(L"yes") | L"no") >> L'"')
                );

            element =
                  EmptyElemTag
                | STag >> boost::spirit::flush_multi_pass_p >> content >> ETag;

            STag =
                L'<' >> Name >> *(S >> Attribute) >> !S >> L'>';

            Attribute =
                Name >> Eq >> AttValue;

            ETag =
                L"</" >> Name >> !S >> L'>';

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
                 );

            EmptyElemTag =
                L'<' >> Name >> *(S >> Attribute) >> !S >> L"/>";

            elementdecl = 
                L"<!ELEMENT" >> S >> Name >> S >> contentspec >> !S >> L'>';

            contentspec = 
                  str_p(L"EMPTY") 
                | L"ANY" 
                | Mixed 
                | children;

            children =
                (choice | seq) >> !(ch_p(L'?') | L'*' | L'+');

            cp = 
                (Name | choice | seq) >> !(ch_p(L'?') | L'*' | L'+');

            choice = 
                L'(' >> !S >> cp 
                  >> +(!S >> L'|' >> !S >> cp) 
                  >> !S >> L')';

            seq =
                L'(' >> !S >> cp >> 
                  *(!S >> L',' >> !S >> cp) 
                  >> !S >> L')';

            Mixed =
                  L'(' >> !S >> L"#PCDATA" 
                      >> *(!S >> L'|' >> !S >> Name) 
                      >> !S >> L")*"
                | L'(' >> !S >> L"#PCDATA" >> !S >> L')';

            AttlistDecl =
                L"<!ATTLIST" >> S >> Name >> *AttDef >> !S >> L'>';

            AttDef =
                S >> Name >> S >> AttType >> S >> DefaultDecl;

            AttType =
                  StringType 
                | TokenizedType 
                | EnumeratedType;

            StringType =
                str_p(L"CDATA");

            TokenizedType =
                longest_d[ 
                      str_p(L"ID") 
                    | L"IDREF" 
                    | L"IDREFS" 
                    | L"ENTITY" 
                    | L"ENTITIES" 
                    | L"NMTOKEN"
                    | L"NMTOKENS" 
                ];

            EnumeratedType =
                  NotationType 
                | Enumeration;

            NotationType =
                L"NOTATION" >> S >> L'(' >> !S >> Name 
                  >> *(!S >> L'|' >> !S >> Name) 
                  >> !S >> L')';

            Enumeration = 
                L'(' >> !S >> Nmtoken 
                >> *(!S >> L'|' >> !S >> Nmtoken) 
                >> !S >> L')';

            DefaultDecl =
                  str_p(L"#REQUIRED") 
                | L"#IMPLIED" 
                | !(L"#FIXED" >> S) >> AttValue;

            conditionalSect =
                  includeSect 
                | ignoreSect;

            includeSect =
                L"<![" >> !S >> L"INCLUDE" >> !S 
                >> L'[' >> extSubsetDecl >> L"]]>";

            ignoreSect = 
                L"<![" >> !S >> L"IGNORE"  >> !S 
                >> L'[' >> *ignoreSectContents >> L"]]>";

            ignoreSectContents = 
                Ignore >> *(L"<![" >> ignoreSectContents >> L"]]>" >> Ignore);

            Ignore = 
                *(Char - (str_p(L"<![") | L"]]>"));

            CharRef = 
                token_node_d[
                      L"&#"  >> +digit_p  >> L';'
                    | L"&#x" >> +xdigit_p >> L';'
                ];

            CharRef2 = 
                  L"&#"  >> +digit_p  >> L';'
                | L"&#x" >> +xdigit_p >> L';';

            Reference =
                  EntityRef
                | CharRef;

            Reference2 =
                  EntityRef2
                | CharRef2;

            EntityRef =
                token_node_d[
                    L'&' >> Name >> L';'
                ];

            EntityRef2 =
                L'&' >> Name2 >> L';';

            PEReference =
                token_node_d[
                    L'%' >> Name >> L';'
                ];

            PEReference2 =
                L'%' >> Name2 >> L';';

            EntityDecl =
                  GEDecl 
                | PEDecl;

            GEDecl =
                L"<!ENTITY" >> S >> Name >> S >> EntityDef >> !S >> L'>';

            PEDecl =
                L"<!ENTITY" >> S >> L'%' >> S >> Name >> S >> PEDef 
                >> !S >> L'>';

            EntityDef =
                  EntityValue
                | ExternalID >> !NDataDecl;

            PEDef =
                  EntityValue 
                | ExternalID;

            ExternalID =
                  L"SYSTEM" >> S >> SystemLiteral
                | L"PUBLIC" >> S >> PubidLiteral >> S >> SystemLiteral;

            NDataDecl =
                S >> L"NDATA" >> S >> Name;

            TextDecl =
                L"<?xml" >> !VersionInfo >> EncodingDecl >> !S >> L"?>";

            extParsedEnt =
                !TextDecl >> content;

            EncodingDecl =
                S >> L"encoding" >> Eq 
                >> (  L'"' >> EncName >> L'"' 
                    | L'\'' >> EncName >> L'\''
                   );

            EncName =
                no_node_d[
                    alpha_p >> *(alnum_p | L'.' | L'_' | L'-')
                ];

            NotationDecl =
                L"<!NOTATION" >> S >> Name >> S 
                >> (ExternalID | PublicID) >> !S >> L'>';

            PublicID =
                L"PUBLIC" >> S >> PubidLiteral;


            BOOST_SPIRIT_DEBUG_RULE(document);
            BOOST_SPIRIT_DEBUG_RULE(prolog);
            BOOST_SPIRIT_DEBUG_RULE(element);
            BOOST_SPIRIT_DEBUG_RULE(Misc);
            BOOST_SPIRIT_DEBUG_RULE(PEReference);
            BOOST_SPIRIT_DEBUG_RULE(PEReference2);
            BOOST_SPIRIT_DEBUG_RULE(Reference);
            BOOST_SPIRIT_DEBUG_RULE(Reference2);
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
            BOOST_SPIRIT_DEBUG_RULE(CharRef2);
            BOOST_SPIRIT_DEBUG_RULE(EntityRef);
            BOOST_SPIRIT_DEBUG_RULE(EntityRef2);
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

        boost::spirit::rule<ScannerT> const&
        start() const
        {
            return document;
        }

    };
};

#endif

