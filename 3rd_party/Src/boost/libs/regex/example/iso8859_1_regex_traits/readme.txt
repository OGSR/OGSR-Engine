I have written a class iso8859_1_regex_traits, which I enclose.  Any
comments will be very welcome.

The class is derived from boost::c_regex_traits<char>, and just redefines
the member function is_class so that it will handle the character classes
for all the 8 bit characters in the ISO8859-1 (Latin 1) alphabet.  The
enclosed file iso8859_1.txt lists the character classes for all the 256
characters.

For the 7 bit ASCII characters up to octal 177, the character classes are
identical to the ones returned by boost::c_regex_traits<char> and
boost::w32_regex_traits<char>.  (I have only tried this under Windows.)

For the characters above octal 177, iso8859_1_regex_traits differs from
boost::w32_regex_traits<char> in the following ways:

1) The characters between octal 200 and octal 237 all belong to the
character class cntrl only, whereas most of them belong to print graph punct
in Windows.  This reflects a true difference between the ISO8859-1 and
Windows character sets, so it should be pretty uncontroversial.

2) The superscript digits ¹²³ (octal 271, 262 and 263) belong to print graph
punct only in iso8859_1_regex_traits.  In w32_regex_traits these characters
are defined to belong to digit as well, which automatically makes them
members of alnum and word too.  I consider this to be a mistake made by
those who defined the Windows locale, as I can see few, if any, uses for a
character class defined in this manner, and the definition precludes the use
of the digit character class for many normal and useful purposes, such as
checking if a string contains an integer number.  (Please note that this
criticism is directed towards Microsoft itself rather than towards the regex
package, since this definition is a part of Visual C++.)

3) The non-breaking space character (octal 240) belongs to print graph punct
in iso8859_1_regex_traits.  In w32_regex_traits it is instead defined to
belong print blank space, i.e.: it is defined in exactly the same way as the
ordinary octal 40 space character.  Again I consider this to be a mistake in
the Windows locale, that renders the space character class fairly useless
for any applications where non-breaking spaces might occur.  After all, the
whole point in having both ordinary and non-breaking spaces in the character
set is that programs should be able to treat them differently, but with the
Windows locale, this is not possible.

As stated above I look forward to any comments, both relating to the
implementation as such and to the choices I have made in the definition of
the character classes.

Christian Engström


