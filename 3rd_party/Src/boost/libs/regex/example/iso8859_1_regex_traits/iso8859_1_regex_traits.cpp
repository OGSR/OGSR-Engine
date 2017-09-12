/*
 * (C) Copyright Christain Engstrom 2001.
 * Permission to copy, use, modify, sell and distribute this software
 * is granted provided this copyright notice appears in all copies.
 * This software is provided "as is" without express or implied
 * warranty, and with no claim as to its suitability for any purpose.
 */
 
#include "iso8859_1_regex_traits.hpp"

bool iso8859_1_regex_traits::is_class(char c, boost::uint_fast32_t f) const
{
  static const boost::uint_fast32_t cntrl =      base::char_class_cntrl;
  static const boost::uint_fast32_t space =      base::char_class_space;
  static const boost::uint_fast32_t blank =      base::char_class_blank;
  static const boost::uint_fast32_t digit =      base::char_class_digit;
  static const boost::uint_fast32_t xdigit =     base::char_class_xdigit;
  static const boost::uint_fast32_t underscore = base::char_class_underscore;
  static const boost::uint_fast32_t punct =      base::char_class_punct;
  static const boost::uint_fast32_t upper =      base::char_class_upper |
                                                 base::char_class_alpha;
  static const boost::uint_fast32_t lower =      base::char_class_lower |
                                                 base::char_class_alpha;


  static const boost::uint_fast32_t flag_table[UCHAR_MAX + 1] = {
//                            Hex  Oct  Dec  Char
  cntrl                  , //   0    0   0
  cntrl                  , //   1    1   1
  cntrl                  , //   2    2   2
  cntrl                  , //   3    3   3
  cntrl                  , //   4    4   4
  cntrl                  , //   5    5   5
  cntrl                  , //   6    6   6
  cntrl                  , //   7    7   7
  cntrl                  , //   8   10   8
  cntrl | space | blank  , //   9   11   9  <HT>
  cntrl | space          , //   a   12  10  <LF>
  cntrl | space          , //   b   13  11  <VT>
  cntrl | space          , //   c   14  12  <FF>
  cntrl | space          , //   d   15  13  <CR>
  cntrl                  , //   e   16  14
  cntrl                  , //   f   17  15
  cntrl                  , //  10   20  16
  cntrl                  , //  11   21  17
  cntrl                  , //  12   22  18
  cntrl                  , //  13   23  19
  cntrl                  , //  14   24  20
  cntrl                  , //  15   25  21
  cntrl                  , //  16   26  22
  cntrl                  , //  17   27  23
  cntrl                  , //  18   30  24
  cntrl                  , //  19   31  25
  cntrl                  , //  1a   32  26
  cntrl                  , //  1b   33  27
  cntrl                  , //  1c   34  28
  cntrl                  , //  1d   35  29
  cntrl                  , //  1e   36  30
  cntrl                  , //  1f   37  31
  space | blank          , //  20   40  32       |  Space
  punct                  , //  21   41  33  !
  punct                  , //  22   42  34  "
  punct                  , //  23   43  35  #
  punct                  , //  24   44  36  $
  punct                  , //  25   45  37  %
  punct                  , //  26   46  38  &
  punct                  , //  27   47  39  '
  punct                  , //  28   50  40  (
  punct                  , //  29   51  41  )
  punct                  , //  2a   52  42  *
  punct                  , //  2b   53  43  +
  punct                  , //  2c   54  44
  punct                  , //  2d   55  45  -
  punct                  , //  2e   56  46  .
  punct                  , //  2f   57  47  /
  digit | xdigit         , //  30   60  48  0
  digit | xdigit         , //  31   61  49  1
  digit | xdigit         , //  32   62  50  2
  digit | xdigit         , //  33   63  51  3
  digit | xdigit         , //  34   64  52  4
  digit | xdigit         , //  35   65  53  5
  digit | xdigit         , //  36   66  54  6
  digit | xdigit         , //  37   67  55  7
  digit | xdigit         , //  38   70  56  8
  digit | xdigit         , //  39   71  57  9
  punct                  , //  3a   72  58  :
  punct                  , //  3b   73  59  ;
  punct                  , //  3c   74  60  <
  punct                  , //  3d   75  61  =
  punct                  , //  3e   76  62  >
  punct                  , //  3f   77  63  ?
  punct                  , //  40  100  64  @
  upper | xdigit         , //  41  101  65  A
  upper | xdigit         , //  42  102  66  B
  upper | xdigit         , //  43  103  67  C
  upper | xdigit         , //  44  104  68  D
  upper | xdigit         , //  45  105  69  E
  upper | xdigit         , //  46  106  70  F
  upper                  , //  47  107  71  G
  upper                  , //  48  110  72  H
  upper                  , //  49  111  73  I
  upper                  , //  4a  112  74  J
  upper                  , //  4b  113  75  K
  upper                  , //  4c  114  76  L
  upper                  , //  4d  115  77  M
  upper                  , //  4e  116  78  N
  upper                  , //  4f  117  79  O
  upper                  , //  50  120  80  P
  upper                  , //  51  121  81  Q
  upper                  , //  52  122  82  R
  upper                  , //  53  123  83  S
  upper                  , //  54  124  84  T
  upper                  , //  55  125  85  U
  upper                  , //  56  126  86  V
  upper                  , //  57  127  87  W
  upper                  , //  58  130  88  X
  upper                  , //  59  131  89  Y
  upper                  , //  5a  132  90  Z
  punct                  , //  5b  133  91  [    |  Left square bracket
  punct                  , //  5c  134  92  \    |  Backslash
  punct                  , //  5d  135  93  ]    |  Right square bracket
  punct                  , //  5e  136  94  ^    |  Circumflex
  punct | underscore     , //  5f  137  95  _    |  Underscore
  punct                  , //  60  140  96  `
  lower | xdigit         , //  61  141  97  a
  lower | xdigit         , //  62  142  98  b
  lower | xdigit         , //  63  143  99  c
  lower | xdigit         , //  64  144 100  d
  lower | xdigit         , //  65  145 101  e
  lower | xdigit         , //  66  146 102  f
  lower                  , //  67  147 103  g
  lower                  , //  68  150 104  h
  lower                  , //  69  151 105  i
  lower                  , //  6a  152 106  j
  lower                  , //  6b  153 107  k
  lower                  , //  6c  154 108  l
  lower                  , //  6d  155 109  m
  lower                  , //  6e  156 110  n
  lower                  , //  6f  157 111  o
  lower                  , //  70  160 112  p
  lower                  , //  71  161 113  q
  lower                  , //  72  162 114  r
  lower                  , //  73  163 115  s
  lower                  , //  74  164 116  t
  lower                  , //  75  165 117  u
  lower                  , //  76  166 118  v
  lower                  , //  77  167 119  w
  lower                  , //  78  170 120  x
  lower                  , //  79  171 121  y
  lower                  , //  7a  172 122  z
  punct                  , //  7b  173 123  {
  punct                  , //  7c  174 124  |
  punct                  , //  7d  175 125  }
  punct                  , //  7e  176 126  ~
  cntrl                  , //  7f  177 127
  cntrl                  , //  80  200 128
  cntrl                  , //  81  201 129
  cntrl                  , //  82  202 130
  cntrl                  , //  83  203 131
  cntrl                  , //  84  204 132
  cntrl                  , //  85  205 133
  cntrl                  , //  86  206 134
  cntrl                  , //  87  207 135
  cntrl                  , //  88  210 136
  cntrl                  , //  89  211 137
  cntrl                  , //  8a  212 138
  cntrl                  , //  8b  213 139
  cntrl                  , //  8c  214 140
  cntrl                  , //  8d  215 141
  cntrl                  , //  8e  216 142
  cntrl                  , //  8f  217 143
  cntrl                  , //  90  220 144
  cntrl                  , //  91  221 145
  cntrl                  , //  92  222 146
  cntrl                  , //  93  223 147
  cntrl                  , //  94  224 148
  cntrl                  , //  95  225 149
  cntrl                  , //  96  226 150
  cntrl                  , //  97  227 151
  cntrl                  , //  98  230 152
  cntrl                  , //  99  231 153
  cntrl                  , //  9a  232 154
  cntrl                  , //  9b  233 155
  cntrl                  , //  9c  234 156
  cntrl                  , //  9d  235 157
  cntrl                  , //  9e  236 158
  cntrl                  , //  9f  237 159
  punct                  , //  a0  240 160       |  Non-breaking space
  punct                  , //  a1  241 161  ¡    |  Inverted exclamation mark
  punct                  , //  a2  242 162  ¢    |  Cent sign
  punct                  , //  a3  243 163  £    |  Pound sign
  punct                  , //  a4  244 164  ¤    |  Currency sign
  punct                  , //  a5  245 165  ¥    |  Yen sign
  punct                  , //  a6  246 166  ¦    |  Broken bar
  punct                  , //  a7  247 167  §    |  Section sign
  punct                  , //  a8  250 168  ¨    |  Diaeresis
  punct                  , //  a9  251 169  ©    |  Copyright sign
  punct                  , //  aa  252 170  ª    |  Feminine ordinal indicator
  punct                  , //  ab  253 171  «    |  Left-pointing double angle
  punct                  , //  ac  254 172  ¬    |  Not sign
  punct                  , //  ad  255 173  ­    |  Soft hyphen
  punct                  , //  ae  256 174  ®    |  Registered trademark sign
  punct                  , //  af  257 175  ¯    |  Macron
  punct                  , //  b0  260 176  °    |  Degree sign
  punct                  , //  b1  261 177  ±    |  Plus-minus sign
  punct                  , //  b2  262 178  ²    |  Superscript two
  punct                  , //  b3  263 179  ³    |  Superscript three
  punct                  , //  b4  264 180  ´    |  Acute accent
  punct                  , //  b5  265 181  µ    |  Micro sign
  punct                  , //  b6  266 182  ¶    |  Pilcrow sign
  punct                  , //  b7  267 183  ·    |  Middle dot
  punct                  , //  b8  270 184  ¸    |  Cedilla
  punct                  , //  b9  271 185  ¹    |  Superscript one
  punct                  , //  ba  272 186  º    |  Masculine ordinal indicator
  punct                  , //  bb  273 187  »    |  Right-pointing double angle
  punct                  , //  bc  274 188  ¼    |  Fraction one quarter
  punct                  , //  bd  275 189  ½    |  Fraction one half
  punct                  , //  be  276 190  ¾    |  Fraction three quarters
  punct                  , //  bf  277 191  ¿    |  Inverted question mark
  upper                  , //  c0  300 192  À    |  "A" with grave accent
  upper                  , //  c1  301 193  Á    |  "A" with acute accent
  upper                  , //  c2  302 194  Â    |  "A" with circumflex
  upper                  , //  c3  303 195  Ã    |  "A" with tilde
  upper                  , //  c4  304 196  Ä    |  "A" with diaeresis
  upper                  , //  c5  305 197  Å    |  "A" with ring above
  upper                  , //  c6  306 198  Æ    |  "AE" ligature
  upper                  , //  c7  307 199  Ç    |  "C" with cedilla
  upper                  , //  c8  310 200  È    |  "E" with grave accent
  upper                  , //  c9  311 201  É    |  "E" with acute accent
  upper                  , //  ca  312 202  Ê    |  "E" with circumflex
  upper                  , //  cb  313 203  Ë    |  "E" with diaeresis
  upper                  , //  cc  314 204  Ì    |  "I" with grave accent
  upper                  , //  cd  315 205  Í    |  "I" with acute accent
  upper                  , //  ce  316 206  Î    |  "I" with circumflex
  upper                  , //  cf  317 207  Ï    |  "I" with diaeresis
  upper                  , //  d0  320 208  Ğ    |  Upper Icelandic D (Eth)
  upper                  , //  d1  321 209  Ñ    |  "N" with tilde
  upper                  , //  d2  322 210  Ò    |  "O" with grave accent
  upper                  , //  d3  323 211  Ó    |  "O" with acute accent
  upper                  , //  d4  324 212  Ô    |  "O" with circumflex
  upper                  , //  d5  325 213  Õ    |  "O" with tilde
  upper                  , //  d6  326 214  Ö    |  "O" with diaeresis
  punct                  , //  d7  327 215  ×    |  Multiplication sign
  upper                  , //  d8  330 216  Ø    |  "O" with stroke
  upper                  , //  d9  331 217  Ù    |  "U" with grave accent
  upper                  , //  da  332 218  Ú    |  "U" with acute accent
  upper                  , //  db  333 219  Û    |  "U" with circumflex
  upper                  , //  dc  334 220  Ü    |  "U" with diaeresis
  upper                  , //  dd  335 221  İ    |  "Y" with acute accent
  upper                  , //  de  336 222  Ş    |  Upper Icelandic T (Thorn)
  lower                  , //  df  337 223  ß    |  German sharp s
  lower                  , //  e0  340 224  à    |  "a" with grave accent
  lower                  , //  e1  341 225  á    |  "a" with acute accent
  lower                  , //  e2  342 226  â    |  "a" with circumflex
  lower                  , //  e3  343 227  ã    |  "a" with tilde
  lower                  , //  e4  344 228  ä    |  "a" with diaeresis
  lower                  , //  e5  345 229  å    |  "a" with ring above
  lower                  , //  e6  346 230  æ    |  Latin small ligature ae
  lower                  , //  e7  347 231  ç    |  "c" with cedilla
  lower                  , //  e8  350 232  è    |  "e" with grave accent
  lower                  , //  e9  351 233  é    |  "e" with acute accent
  lower                  , //  ea  352 234  ê    |  "e" with circumflex
  lower                  , //  eb  353 235  ë    |  "e" with diaeresis
  lower                  , //  ec  354 236  ì    |  "i" with grave accent
  lower                  , //  ed  355 237  í    |  "i" with acute accent
  lower                  , //  ee  356 238  î    |  "i" with circumflex
  lower                  , //  ef  357 239  ï    |  "i" with diaeresis
  lower                  , //  f0  360 240  ğ    |  Lower Icelandic d (eth)
  lower                  , //  f1  361 241  ñ    |  "n" with tilde
  lower                  , //  f2  362 242  ò    |  "o" with grave accent
  lower                  , //  f3  363 243  ó    |  "o" with acute accent
  lower                  , //  f4  364 244  ô    |  "o" with circumflex
  lower                  , //  f5  365 245  õ    |  "o" with tilde
  lower                  , //  f6  366 246  ö    |  "o" with diaeresis
  punct                  , //  f7  367 247  ÷    |  Division sign
  lower                  , //  f8  370 248  ø    |  "o" with oblique bar
  lower                  , //  f9  371 249  ù    |  "u" with grave accent
  lower                  , //  fa  372 250  ú    |  "u" with acute accent
  lower                  , //  fb  373 251  û    |  "u" with circumflex
  lower                  , //  fc  374 252  ü    |  "u" with diaeresis
  lower                  , //  fd  375 253  ı    |  "y" with acute accent
  lower                  , //  fe  376 254  ş    |  Lower Icelandic t (thorn)
  lower                    //  ff  377 255  ÿ    |  "y" with diaeresis
  };

  return (flag_table[unsigned char(c)] & f) != 0;
}


//============================================================================
char iso8859_1_regex_traits::translate(char c, bool icase) const
{
//----------------------------------------------------------------------------
//      "ABCDEFGHIJKLMNOPQRSTUVWXYZ" -> "abcdefghijklmnopqrstuvwxyz"
//  "ÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÏĞÑÒÓÔÕÖØÙÚÛÜİŞ" -> "àáâãäåæçèéêëìíîïğñòóôõöøùúûüış"
//----------------------------------------------------------------------------
  static const char lower_case_map[UCHAR_MAX + 1] = {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15,
   16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
   32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
   48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
   64, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122, 91, 92, 93, 94, 95,
   96, 97, 98, 99,100,101,102,103,104,105,106,107,108,109,110,111,
  112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,
  128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,
  144,145,146,147,148,149,150,151,152,153,154,155,156,157,158,159,
  160,161,162,163,164,165,166,167,168,169,170,171,172,173,174,175,
  176,177,178,179,180,181,182,183,184,185,186,187,188,189,190,191,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,215,248,249,250,251,252,253,254,223,
  224,225,226,227,228,229,230,231,232,233,234,235,236,237,238,239,
  240,241,242,243,244,245,246,247,248,249,250,251,252,253,254,255
};

  return icase ? lower_case_map[unsigned char(c)] : c;
}

