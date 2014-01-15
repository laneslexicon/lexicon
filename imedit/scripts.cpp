#include <QString>
#include <QChar>
#include <QStringList>
#include "scripts.h"
void UcdScripts::getScripts(QStringList & x) {
  x <<  "Arabic";
  x <<  "Armenian";
  x <<  "Avestan";
  x <<  "Balinese";
  x <<  "Bamum";
  x <<  "Batak";
  x <<  "Bengali";
  x <<  "Bopomofo";
  x <<  "Brahmi";
  x <<  "Braille";
  x <<  "Buginese";
  x <<  "Buhid";
  x <<  "Canadian_Aboriginal";
  x <<  "Carian";
  x <<  "Chakma";
  x <<  "Cham";
  x <<  "Cherokee";
  x <<  "Common";
  x <<  "Coptic";
  x <<  "Cuneiform";
  x <<  "Cypriot";
  x <<  "Cyrillic";
  x <<  "Deseret";
  x <<  "Devanagari";
  x <<  "Egyptian_Hieroglyphs";
  x <<  "Ethiopic";
  x <<  "Georgian";
  x <<  "Glagolitic";
  x <<  "Gothic";
  x <<  "Greek";
  x <<  "Gujarati";
  x <<  "Gurmukhi";
  x <<  "Han";
  x <<  "Hangul";
  x <<  "Hanunoo";
  x <<  "Hebrew";
  x <<  "Hiragana";
  x <<  "Imperial_Aramaic";
  x <<  "Inherited";
  x <<  "Inscriptional_Pahlavi";
  x <<  "Inscriptional_Parthian";
  x <<  "Javanese";
  x <<  "Kaithi";
  x <<  "Kannada";
  x <<  "Katakana";
  x <<  "Kayah_Li";
  x <<  "Kharoshthi";
  x <<  "Khmer";
  x <<  "Lao";
  x <<  "Latin";
  x <<  "Lepcha";
  x <<  "Limbu";
  x <<  "Linear_B";
  x <<  "Lisu";
  x <<  "Lycian";
  x <<  "Lydian";
  x <<  "Malayalam";
  x <<  "Mandaic";
  x <<  "Meetei_Mayek";
  x <<  "Meroitic_Cursive";
  x <<  "Meroitic_Hieroglyphs";
  x <<  "Miao";
  x <<  "Mongolian";
  x <<  "Myanmar";
  x <<  "New_Tai_Lue";
  x <<  "Nko";
  x <<  "Ogham";
  x <<  "Ol_Chiki";
  x <<  "Old_Italic";
  x <<  "Old_Persian";
  x <<  "Old_South_Arabian";
  x <<  "Old_Turkic";
  x <<  "Oriya";
  x <<  "Osmanya";
  x <<  "Phags_Pa";
  x <<  "Phoenician";
  x <<  "Rejang";
  x <<  "Runic";
  x <<  "Samaritan";
  x <<  "Saurashtra";
  x <<  "Sharada";
  x <<  "Shavian";
  x <<  "Sinhala";
  x <<  "Sora_Sompeng";
  x <<  "Sundanese";
  x <<  "Syloti_Nagri";
  x <<  "Syriac";
  x <<  "Tagalog";
  x <<  "Tagbanwa";
  x <<  "Tai_Le";
  x <<  "Tai_Tham";
  x <<  "Tai_Viet";
  x <<  "Takri";
  x <<  "Tamil";
  x <<  "Telugu";
  x <<  "Thaana";
  x <<  "Thai";
  x <<  "Tibetan";
  x <<  "Tifinagh";
  x <<  "Ugaritic";
  x <<  "Unknown";
  x <<  "Vai";
  x <<  "Yi";
}
QString UcdScripts::getScript(int x) {
  // Scripts-6.2.0.txt
  // Date: 2012-06-04, 17:21:29 GMT [MD]
  //
  // Unicode Character Database
  // Copyright (c) 1991-2012 Unicode, Inc.
  // For terms of use, see http://www.unicode.org/terms_of_use.html
  // For documentation, see http://www.unicode.org/reports/tr44/
  // ================================================
  // Property:	Script
  //  All code points not explicitly listed for Script
  //  have the value Unknown (Zzzz).
  // @missing: 0000..10FFFF; Unknown
  // ================================================
  if ((x >= 0x0000) && (x <= 0x001F)) { // Cc  [32] <control-0000>..<control-001F>
    return "Common";
  }
  if (x == 0x0020 ) {//  Zs       SPACE
    return "Common";
  }
  if ((x >= 0x0021) && (x <= 0x0023)) { // Po   [3] EXCLAMATION MARK..NUMBER SIGN
    return "Common";
  }
  if (x == 0x0024 ) {//  Sc       DOLLAR SIGN
    return "Common";
  }
  if ((x >= 0x0025) && (x <= 0x0027)) { // Po   [3] PERCENT SIGN..APOSTROPHE
    return "Common";
  }
  if (x == 0x0028 ) {//  Ps       LEFT PARENTHESIS
    return "Common";
  }
  if (x == 0x0029 ) {//  Pe       RIGHT PARENTHESIS
    return "Common";
  }
  if (x == 0x002A ) {//  Po       ASTERISK
    return "Common";
  }
  if (x == 0x002B ) {//  Sm       PLUS SIGN
    return "Common";
  }
  if (x == 0x002C ) {//  Po       COMMA
    return "Common";
  }
  if (x == 0x002D ) {//  Pd       HYPHEN-MINUS
    return "Common";
  }
  if ((x >= 0x002E) && (x <= 0x002F)) { // Po   [2] FULL STOP..SOLIDUS
    return "Common";
  }
  if ((x >= 0x0030) && (x <= 0x0039)) { // Nd  [10] DIGIT ZERO..DIGIT NINE
    return "Common";
  }
  if ((x >= 0x003A) && (x <= 0x003B)) { // Po   [2] COLON..SEMICOLON
    return "Common";
  }
  if ((x >= 0x003C) && (x <= 0x003E)) { // Sm   [3] LESS-THAN SIGN..GREATER-THAN SIGN
    return "Common";
  }
  if ((x >= 0x003F) && (x <= 0x0040)) { // Po   [2] QUESTION MARK..COMMERCIAL AT
    return "Common";
  }
  if (x == 0x005B ) {//  Ps       LEFT SQUARE BRACKET
    return "Common";
  }
  if (x == 0x005C ) {//  Po       REVERSE SOLIDUS
    return "Common";
  }
  if (x == 0x005D ) {//  Pe       RIGHT SQUARE BRACKET
    return "Common";
  }
  if (x == 0x005E ) {//  Sk       CIRCUMFLEX ACCENT
    return "Common";
  }
  if (x == 0x005F ) {//  Pc       LOW LINE
    return "Common";
  }
  if (x == 0x0060 ) {//  Sk       GRAVE ACCENT
    return "Common";
  }
  if (x == 0x007B ) {//  Ps       LEFT CURLY BRACKET
    return "Common";
  }
  if (x == 0x007C ) {//  Sm       VERTICAL LINE
    return "Common";
  }
  if (x == 0x007D ) {//  Pe       RIGHT CURLY BRACKET
    return "Common";
  }
  if (x == 0x007E ) {//  Sm       TILDE
    return "Common";
  }
  if ((x >= 0x007F) && (x <= 0x009F)) { // Cc  [33] <control-007F>..<control-009F>
    return "Common";
  }
  if (x == 0x00A0 ) {//  Zs       NO-BREAK SPACE
    return "Common";
  }
  if (x == 0x00A1 ) {//  Po       INVERTED EXCLAMATION MARK
    return "Common";
  }
  if ((x >= 0x00A2) && (x <= 0x00A5)) { // Sc   [4] CENT SIGN..YEN SIGN
    return "Common";
  }
  if (x == 0x00A6 ) {//  So       BROKEN BAR
    return "Common";
  }
  if (x == 0x00A7 ) {//  Po       SECTION SIGN
    return "Common";
  }
  if (x == 0x00A8 ) {//  Sk       DIAERESIS
    return "Common";
  }
  if (x == 0x00A9 ) {//  So       COPYRIGHT SIGN
    return "Common";
  }
  if (x == 0x00AB ) {//  Pi       LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    return "Common";
  }
  if (x == 0x00AC ) {//  Sm       NOT SIGN
    return "Common";
  }
  if (x == 0x00AD ) {//  Cf       SOFT HYPHEN
    return "Common";
  }
  if (x == 0x00AE ) {//  So       REGISTERED SIGN
    return "Common";
  }
  if (x == 0x00AF ) {//  Sk       MACRON
    return "Common";
  }
  if (x == 0x00B0 ) {//  So       DEGREE SIGN
    return "Common";
  }
  if (x == 0x00B1 ) {//  Sm       PLUS-MINUS SIGN
    return "Common";
  }
  if ((x >= 0x00B2) && (x <= 0x00B3)) { // No   [2] SUPERSCRIPT TWO..SUPERSCRIPT THREE
    return "Common";
  }
  if (x == 0x00B4 ) {//  Sk       ACUTE ACCENT
    return "Common";
  }
  if (x == 0x00B5 ) {//  L&       MICRO SIGN
    return "Common";
  }
  if ((x >= 0x00B6) && (x <= 0x00B7)) { // Po   [2] PILCROW SIGN..MIDDLE DOT
    return "Common";
  }
  if (x == 0x00B8 ) {//  Sk       CEDILLA
    return "Common";
  }
  if (x == 0x00B9 ) {//  No       SUPERSCRIPT ONE
    return "Common";
  }
  if (x == 0x00BB ) {//  Pf       RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
    return "Common";
  }
  if ((x >= 0x00BC) && (x <= 0x00BE)) { // No   [3] VULGAR FRACTION ONE QUARTER..VULGAR FRACTION THREE QUARTERS
    return "Common";
  }
  if (x == 0x00BF ) {//  Po       INVERTED QUESTION MARK
    return "Common";
  }
  if (x == 0x00D7 ) {//  Sm       MULTIPLICATION SIGN
    return "Common";
  }
  if (x == 0x00F7 ) {//  Sm       DIVISION SIGN
    return "Common";
  }
  if ((x >= 0x02B9) && (x <= 0x02C1)) { // Lm   [9] MODIFIER LETTER PRIME..MODIFIER LETTER REVERSED GLOTTAL STOP
    return "Common";
  }
  if ((x >= 0x02C2) && (x <= 0x02C5)) { // Sk   [4] MODIFIER LETTER LEFT ARROWHEAD..MODIFIER LETTER DOWN ARROWHEAD
    return "Common";
  }
  if ((x >= 0x02C6) && (x <= 0x02D1)) { // Lm  [12] MODIFIER LETTER CIRCUMFLEX ACCENT..MODIFIER LETTER HALF TRIANGULAR COLON
    return "Common";
  }
  if ((x >= 0x02D2) && (x <= 0x02DF)) { // Sk  [14] MODIFIER LETTER CENTRED RIGHT HALF RING..MODIFIER LETTER CROSS ACCENT
    return "Common";
  }
  if ((x >= 0x02E5) && (x <= 0x02E9)) { // Sk   [5] MODIFIER LETTER EXTRA-HIGH TONE BAR..MODIFIER LETTER EXTRA-LOW TONE BAR
    return "Common";
  }
  if (x == 0x02EC ) {//  Lm       MODIFIER LETTER VOICING
    return "Common";
  }
  if (x == 0x02ED ) {//  Sk       MODIFIER LETTER UNASPIRATED
    return "Common";
  }
  if (x == 0x02EE ) {//  Lm       MODIFIER LETTER DOUBLE APOSTROPHE
    return "Common";
  }
  if ((x >= 0x02EF) && (x <= 0x02FF)) { // Sk  [17] MODIFIER LETTER LOW DOWN ARROWHEAD..MODIFIER LETTER LOW LEFT ARROW
    return "Common";
  }
  if (x == 0x0374 ) {//  Lm       GREEK NUMERAL SIGN
    return "Common";
  }
  if (x == 0x037E ) {//  Po       GREEK QUESTION MARK
    return "Common";
  }
  if (x == 0x0385 ) {//  Sk       GREEK DIALYTIKA TONOS
    return "Common";
  }
  if (x == 0x0387 ) {//  Po       GREEK ANO TELEIA
    return "Common";
  }
  if (x == 0x0589 ) {//  Po       ARMENIAN FULL STOP
    return "Common";
  }
  if (x == 0x060C ) {//  Po       ARABIC COMMA
    return "Common";
  }
  if (x == 0x061B ) {//  Po       ARABIC SEMICOLON
    return "Common";
  }
  if (x == 0x061F ) {//  Po       ARABIC QUESTION MARK
    return "Common";
  }
  if (x == 0x0640 ) {//  Lm       ARABIC TATWEEL
    return "Common";
  }
  if ((x >= 0x0660) && (x <= 0x0669)) { // Nd  [10] ARABIC-INDIC DIGIT ZERO..ARABIC-INDIC DIGIT NINE
    return "Common";
  }
  if (x == 0x06DD ) {//  Cf       ARABIC END OF AYAH
    return "Common";
  }
  if ((x >= 0x0964) && (x <= 0x0965)) { // Po   [2] DEVANAGARI DANDA..DEVANAGARI DOUBLE DANDA
    return "Common";
  }
  if (x == 0x0E3F ) {//  Sc       THAI CURRENCY SYMBOL BAHT
    return "Common";
  }
  if ((x >= 0x0FD5) && (x <= 0x0FD8)) { // So   [4] RIGHT-FACING SVASTI SIGN..LEFT-FACING SVASTI SIGN WITH DOTS
    return "Common";
  }
  if (x == 0x10FB ) {//  Po       GEORGIAN PARAGRAPH SEPARATOR
    return "Common";
  }
  if ((x >= 0x16EB) && (x <= 0x16ED)) { // Po   [3] RUNIC SINGLE PUNCTUATION..RUNIC CROSS PUNCTUATION
    return "Common";
  }
  if ((x >= 0x1735) && (x <= 0x1736)) { // Po   [2] PHILIPPINE SINGLE PUNCTUATION..PHILIPPINE DOUBLE PUNCTUATION
    return "Common";
  }
  if ((x >= 0x1802) && (x <= 0x1803)) { // Po   [2] MONGOLIAN COMMA..MONGOLIAN FULL STOP
    return "Common";
  }
  if (x == 0x1805 ) {//  Po       MONGOLIAN FOUR DOTS
    return "Common";
  }
  if (x == 0x1CD3 ) {//  Po       VEDIC SIGN NIHSHVASA
    return "Common";
  }
  if (x == 0x1CE1 ) {//  Mc       VEDIC TONE ATHARVAVEDIC INDEPENDENT SVARITA
    return "Common";
  }
  if ((x >= 0x1CE9) && (x <= 0x1CEC)) { // Lo   [4] VEDIC SIGN ANUSVARA ANTARGOMUKHA..VEDIC SIGN ANUSVARA VAMAGOMUKHA WITH TAIL
    return "Common";
  }
  if ((x >= 0x1CEE) && (x <= 0x1CF1)) { // Lo   [4] VEDIC SIGN HEXIFORM LONG ANUSVARA..VEDIC SIGN ANUSVARA UBHAYATO MUKHA
    return "Common";
  }
  if ((x >= 0x1CF2) && (x <= 0x1CF3)) { // Mc   [2] VEDIC SIGN ARDHAVISARGA..VEDIC SIGN ROTATED ARDHAVISARGA
    return "Common";
  }
  if ((x >= 0x1CF5) && (x <= 0x1CF6)) { // Lo   [2] VEDIC SIGN JIHVAMULIYA..VEDIC SIGN UPADHMANIYA
    return "Common";
  }
  if ((x >= 0x2000) && (x <= 0x200A)) { // Zs  [11] EN QUAD..HAIR SPACE
    return "Common";
  }
  if (x == 0x200B ) {//  Cf       ZERO WIDTH SPACE
    return "Common";
  }
  if ((x >= 0x200E) && (x <= 0x200F)) { // Cf   [2] LEFT-TO-RIGHT MARK..RIGHT-TO-LEFT MARK
    return "Common";
  }
  if ((x >= 0x2010) && (x <= 0x2015)) { // Pd   [6] HYPHEN..HORIZONTAL BAR
    return "Common";
  }
  if ((x >= 0x2016) && (x <= 0x2017)) { // Po   [2] DOUBLE VERTICAL LINE..DOUBLE LOW LINE
    return "Common";
  }
  if (x == 0x2018 ) {//  Pi       LEFT SINGLE QUOTATION MARK
    return "Common";
  }
  if (x == 0x2019 ) {//  Pf       RIGHT SINGLE QUOTATION MARK
    return "Common";
  }
  if (x == 0x201A ) {//  Ps       SINGLE LOW-9 QUOTATION MARK
    return "Common";
  }
  if ((x >= 0x201B) && (x <= 0x201C)) { // Pi   [2] SINGLE HIGH-REVERSED-9 QUOTATION MARK..LEFT DOUBLE QUOTATION MARK
    return "Common";
  }
  if (x == 0x201D ) {//  Pf       RIGHT DOUBLE QUOTATION MARK
    return "Common";
  }
  if (x == 0x201E ) {//  Ps       DOUBLE LOW-9 QUOTATION MARK
    return "Common";
  }
  if (x == 0x201F ) {//  Pi       DOUBLE HIGH-REVERSED-9 QUOTATION MARK
    return "Common";
  }
  if ((x >= 0x2020) && (x <= 0x2027)) { // Po   [8] DAGGER..HYPHENATION POINT
    return "Common";
  }
  if (x == 0x2028 ) {//  Zl       LINE SEPARATOR
    return "Common";
  }
  if (x == 0x2029 ) {//  Zp       PARAGRAPH SEPARATOR
    return "Common";
  }
  if ((x >= 0x202A) && (x <= 0x202E)) { // Cf   [5] LEFT-TO-RIGHT EMBEDDING..RIGHT-TO-LEFT OVERRIDE
    return "Common";
  }
  if (x == 0x202F ) {//  Zs       NARROW NO-BREAK SPACE
    return "Common";
  }
  if ((x >= 0x2030) && (x <= 0x2038)) { // Po   [9] PER MILLE SIGN..CARET
    return "Common";
  }
  if (x == 0x2039 ) {//  Pi       SINGLE LEFT-POINTING ANGLE QUOTATION MARK
    return "Common";
  }
  if (x == 0x203A ) {//  Pf       SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
    return "Common";
  }
  if ((x >= 0x203B) && (x <= 0x203E)) { // Po   [4] REFERENCE MARK..OVERLINE
    return "Common";
  }
  if ((x >= 0x203F) && (x <= 0x2040)) { // Pc   [2] UNDERTIE..CHARACTER TIE
    return "Common";
  }
  if ((x >= 0x2041) && (x <= 0x2043)) { // Po   [3] CARET INSERTION POINT..HYPHEN BULLET
    return "Common";
  }
  if (x == 0x2044 ) {//  Sm       FRACTION SLASH
    return "Common";
  }
  if (x == 0x2045 ) {//  Ps       LEFT SQUARE BRACKET WITH QUILL
    return "Common";
  }
  if (x == 0x2046 ) {//  Pe       RIGHT SQUARE BRACKET WITH QUILL
    return "Common";
  }
  if ((x >= 0x2047) && (x <= 0x2051)) { // Po  [11] DOUBLE QUESTION MARK..TWO ASTERISKS ALIGNED VERTICALLY
    return "Common";
  }
  if (x == 0x2052 ) {//  Sm       COMMERCIAL MINUS SIGN
    return "Common";
  }
  if (x == 0x2053 ) {//  Po       SWUNG DASH
    return "Common";
  }
  if (x == 0x2054 ) {//  Pc       INVERTED UNDERTIE
    return "Common";
  }
  if ((x >= 0x2055) && (x <= 0x205E)) { // Po  [10] FLOWER PUNCTUATION MARK..VERTICAL FOUR DOTS
    return "Common";
  }
  if (x == 0x205F ) {//  Zs       MEDIUM MATHEMATICAL SPACE
    return "Common";
  }
  if ((x >= 0x2060) && (x <= 0x2064)) { // Cf   [5] WORD JOINER..INVISIBLE PLUS
    return "Common";
  }
  if ((x >= 0x206A) && (x <= 0x206F)) { // Cf   [6] INHIBIT SYMMETRIC SWAPPING..NOMINAL DIGIT SHAPES
    return "Common";
  }
  if (x == 0x2070 ) {//  No       SUPERSCRIPT ZERO
    return "Common";
  }
  if ((x >= 0x2074) && (x <= 0x2079)) { // No   [6] SUPERSCRIPT FOUR..SUPERSCRIPT NINE
    return "Common";
  }
  if ((x >= 0x207A) && (x <= 0x207C)) { // Sm   [3] SUPERSCRIPT PLUS SIGN..SUPERSCRIPT EQUALS SIGN
    return "Common";
  }
  if (x == 0x207D ) {//  Ps       SUPERSCRIPT LEFT PARENTHESIS
    return "Common";
  }
  if (x == 0x207E ) {//  Pe       SUPERSCRIPT RIGHT PARENTHESIS
    return "Common";
  }
  if ((x >= 0x2080) && (x <= 0x2089)) { // No  [10] SUBSCRIPT ZERO..SUBSCRIPT NINE
    return "Common";
  }
  if ((x >= 0x208A) && (x <= 0x208C)) { // Sm   [3] SUBSCRIPT PLUS SIGN..SUBSCRIPT EQUALS SIGN
    return "Common";
  }
  if (x == 0x208D ) {//  Ps       SUBSCRIPT LEFT PARENTHESIS
    return "Common";
  }
  if (x == 0x208E ) {//  Pe       SUBSCRIPT RIGHT PARENTHESIS
    return "Common";
  }
  if ((x >= 0x20A0) && (x <= 0x20BA)) { // Sc  [27] EURO-CURRENCY SIGN..TURKISH LIRA SIGN
    return "Common";
  }
  if ((x >= 0x2100) && (x <= 0x2101)) { // So   [2] ACCOUNT OF..ADDRESSED TO THE SUBJECT
    return "Common";
  }
  if (x == 0x2102 ) {//  L&       DOUBLE-STRUCK CAPITAL C
    return "Common";
  }
  if ((x >= 0x2103) && (x <= 0x2106)) { // So   [4] DEGREE CELSIUS..CADA UNA
    return "Common";
  }
  if (x == 0x2107 ) {//  L&       EULER CONSTANT
    return "Common";
  }
  if ((x >= 0x2108) && (x <= 0x2109)) { // So   [2] SCRUPLE..DEGREE FAHRENHEIT
    return "Common";
  }
  if ((x >= 0x210A) && (x <= 0x2113)) { // L&  [10] SCRIPT SMALL G..SCRIPT SMALL L
    return "Common";
  }
  if (x == 0x2114 ) {//  So       L B BAR SYMBOL
    return "Common";
  }
  if (x == 0x2115 ) {//  L&       DOUBLE-STRUCK CAPITAL N
    return "Common";
  }
  if ((x >= 0x2116) && (x <= 0x2117)) { // So   [2] NUMERO SIGN..SOUND RECORDING COPYRIGHT
    return "Common";
  }
  if (x == 0x2118 ) {//  Sm       SCRIPT CAPITAL P
    return "Common";
  }
  if ((x >= 0x2119) && (x <= 0x211D)) { // L&   [5] DOUBLE-STRUCK CAPITAL P..DOUBLE-STRUCK CAPITAL R
    return "Common";
  }
  if ((x >= 0x211E) && (x <= 0x2123)) { // So   [6] PRESCRIPTION TAKE..VERSICLE
    return "Common";
  }
  if (x == 0x2124 ) {//  L&       DOUBLE-STRUCK CAPITAL Z
    return "Common";
  }
  if (x == 0x2125 ) {//  So       OUNCE SIGN
    return "Common";
  }
  if (x == 0x2127 ) {//  So       INVERTED OHM SIGN
    return "Common";
  }
  if (x == 0x2128 ) {//  L&       BLACK-LETTER CAPITAL Z
    return "Common";
  }
  if (x == 0x2129 ) {//  So       TURNED GREEK SMALL LETTER IOTA
    return "Common";
  }
  if ((x >= 0x212C) && (x <= 0x212D)) { // L&   [2] SCRIPT CAPITAL B..BLACK-LETTER CAPITAL C
    return "Common";
  }
  if (x == 0x212E ) {//  So       ESTIMATED SYMBOL
    return "Common";
  }
  if ((x >= 0x212F) && (x <= 0x2131)) { // L&   [3] SCRIPT SMALL E..SCRIPT CAPITAL F
    return "Common";
  }
  if ((x >= 0x2133) && (x <= 0x2134)) { // L&   [2] SCRIPT CAPITAL M..SCRIPT SMALL O
    return "Common";
  }
  if ((x >= 0x2135) && (x <= 0x2138)) { // Lo   [4] ALEF SYMBOL..DALET SYMBOL
    return "Common";
  }
  if (x == 0x2139 ) {//  L&       INFORMATION SOURCE
    return "Common";
  }
  if ((x >= 0x213A) && (x <= 0x213B)) { // So   [2] ROTATED CAPITAL Q..FACSIMILE SIGN
    return "Common";
  }
  if ((x >= 0x213C) && (x <= 0x213F)) { // L&   [4] DOUBLE-STRUCK SMALL PI..DOUBLE-STRUCK CAPITAL PI
    return "Common";
  }
  if ((x >= 0x2140) && (x <= 0x2144)) { // Sm   [5] DOUBLE-STRUCK N-ARY SUMMATION..TURNED SANS-SERIF CAPITAL Y
    return "Common";
  }
  if ((x >= 0x2145) && (x <= 0x2149)) { // L&   [5] DOUBLE-STRUCK ITALIC CAPITAL D..DOUBLE-STRUCK ITALIC SMALL J
    return "Common";
  }
  if (x == 0x214A ) {//  So       PROPERTY LINE
    return "Common";
  }
  if (x == 0x214B ) {//  Sm       TURNED AMPERSAND
    return "Common";
  }
  if ((x >= 0x214C) && (x <= 0x214D)) { // So   [2] PER SIGN..AKTIESELSKAB
    return "Common";
  }
  if (x == 0x214F ) {//  So       SYMBOL FOR SAMARITAN SOURCE
    return "Common";
  }
  if ((x >= 0x2150) && (x <= 0x215F)) { // No  [16] VULGAR FRACTION ONE SEVENTH..FRACTION NUMERATOR ONE
    return "Common";
  }
  if (x == 0x2189 ) {//  No       VULGAR FRACTION ZERO THIRDS
    return "Common";
  }
  if ((x >= 0x2190) && (x <= 0x2194)) { // Sm   [5] LEFTWARDS ARROW..LEFT RIGHT ARROW
    return "Common";
  }
  if ((x >= 0x2195) && (x <= 0x2199)) { // So   [5] UP DOWN ARROW..SOUTH WEST ARROW
    return "Common";
  }
  if ((x >= 0x219A) && (x <= 0x219B)) { // Sm   [2] LEFTWARDS ARROW WITH STROKE..RIGHTWARDS ARROW WITH STROKE
    return "Common";
  }
  if ((x >= 0x219C) && (x <= 0x219F)) { // So   [4] LEFTWARDS WAVE ARROW..UPWARDS TWO HEADED ARROW
    return "Common";
  }
  if (x == 0x21A0 ) {//  Sm       RIGHTWARDS TWO HEADED ARROW
    return "Common";
  }
  if ((x >= 0x21A1) && (x <= 0x21A2)) { // So   [2] DOWNWARDS TWO HEADED ARROW..LEFTWARDS ARROW WITH TAIL
    return "Common";
  }
  if (x == 0x21A3 ) {//  Sm       RIGHTWARDS ARROW WITH TAIL
    return "Common";
  }
  if ((x >= 0x21A4) && (x <= 0x21A5)) { // So   [2] LEFTWARDS ARROW FROM BAR..UPWARDS ARROW FROM BAR
    return "Common";
  }
  if (x == 0x21A6 ) {//  Sm       RIGHTWARDS ARROW FROM BAR
    return "Common";
  }
  if ((x >= 0x21A7) && (x <= 0x21AD)) { // So   [7] DOWNWARDS ARROW FROM BAR..LEFT RIGHT WAVE ARROW
    return "Common";
  }
  if (x == 0x21AE ) {//  Sm       LEFT RIGHT ARROW WITH STROKE
    return "Common";
  }
  if ((x >= 0x21AF) && (x <= 0x21CD)) { // So  [31] DOWNWARDS ZIGZAG ARROW..LEFTWARDS DOUBLE ARROW WITH STROKE
    return "Common";
  }
  if ((x >= 0x21CE) && (x <= 0x21CF)) { // Sm   [2] LEFT RIGHT DOUBLE ARROW WITH STROKE..RIGHTWARDS DOUBLE ARROW WITH STROKE
    return "Common";
  }
  if ((x >= 0x21D0) && (x <= 0x21D1)) { // So   [2] LEFTWARDS DOUBLE ARROW..UPWARDS DOUBLE ARROW
    return "Common";
  }
  if (x == 0x21D2 ) {//  Sm       RIGHTWARDS DOUBLE ARROW
    return "Common";
  }
  if (x == 0x21D3 ) {//  So       DOWNWARDS DOUBLE ARROW
    return "Common";
  }
  if (x == 0x21D4 ) {//  Sm       LEFT RIGHT DOUBLE ARROW
    return "Common";
  }
  if ((x >= 0x21D5) && (x <= 0x21F3)) { // So  [31] UP DOWN DOUBLE ARROW..UP DOWN WHITE ARROW
    return "Common";
  }
  if ((x >= 0x21F4) && (x <= 0x22FF)) { // Sm [268] RIGHT ARROW WITH SMALL CIRCLE..Z NOTATION BAG MEMBERSHIP
    return "Common";
  }
  if ((x >= 0x2300) && (x <= 0x2307)) { // So   [8] DIAMETER SIGN..WAVY LINE
    return "Common";
  }
  if ((x >= 0x2308) && (x <= 0x230B)) { // Sm   [4] LEFT CEILING..RIGHT FLOOR
    return "Common";
  }
  if ((x >= 0x230C) && (x <= 0x231F)) { // So  [20] BOTTOM RIGHT CROP..BOTTOM RIGHT CORNER
    return "Common";
  }
  if ((x >= 0x2320) && (x <= 0x2321)) { // Sm   [2] TOP HALF INTEGRAL..BOTTOM HALF INTEGRAL
    return "Common";
  }
  if ((x >= 0x2322) && (x <= 0x2328)) { // So   [7] FROWN..KEYBOARD
    return "Common";
  }
  if (x == 0x2329 ) {//  Ps       LEFT-POINTING ANGLE BRACKET
    return "Common";
  }
  if (x == 0x232A ) {//  Pe       RIGHT-POINTING ANGLE BRACKET
    return "Common";
  }
  if ((x >= 0x232B) && (x <= 0x237B)) { // So  [81] ERASE TO THE LEFT..NOT CHECK MARK
    return "Common";
  }
  if (x == 0x237C ) {//  Sm       RIGHT ANGLE WITH DOWNWARDS ZIGZAG ARROW
    return "Common";
  }
  if ((x >= 0x237D) && (x <= 0x239A)) { // So  [30] SHOULDERED OPEN BOX..CLEAR SCREEN SYMBOL
    return "Common";
  }
  if ((x >= 0x239B) && (x <= 0x23B3)) { // Sm  [25] LEFT PARENTHESIS UPPER HOOK..SUMMATION BOTTOM
    return "Common";
  }
  if ((x >= 0x23B4) && (x <= 0x23DB)) { // So  [40] TOP SQUARE BRACKET..FUSE
    return "Common";
  }
  if ((x >= 0x23DC) && (x <= 0x23E1)) { // Sm   [6] TOP PARENTHESIS..BOTTOM TORTOISE SHELL BRACKET
    return "Common";
  }
  if ((x >= 0x23E2) && (x <= 0x23F3)) { // So  [18] WHITE TRAPEZIUM..HOURGLASS WITH FLOWING SAND
    return "Common";
  }
  if ((x >= 0x2400) && (x <= 0x2426)) { // So  [39] SYMBOL FOR NULL..SYMBOL FOR SUBSTITUTE FORM TWO
    return "Common";
  }
  if ((x >= 0x2440) && (x <= 0x244A)) { // So  [11] OCR HOOK..OCR DOUBLE BACKSLASH
    return "Common";
  }
  if ((x >= 0x2460) && (x <= 0x249B)) { // No  [60] CIRCLED DIGIT ONE..NUMBER TWENTY FULL STOP
    return "Common";
  }
  if ((x >= 0x249C) && (x <= 0x24E9)) { // So  [78] PARENTHESIZED LATIN SMALL LETTER A..CIRCLED LATIN SMALL LETTER Z
    return "Common";
  }
  if ((x >= 0x24EA) && (x <= 0x24FF)) { // No  [22] CIRCLED DIGIT ZERO..NEGATIVE CIRCLED DIGIT ZERO
    return "Common";
  }
  if ((x >= 0x2500) && (x <= 0x25B6)) { // So [183] BOX DRAWINGS LIGHT HORIZONTAL..BLACK RIGHT-POINTING TRIANGLE
    return "Common";
  }
  if (x == 0x25B7 ) {//  Sm       WHITE RIGHT-POINTING TRIANGLE
    return "Common";
  }
  if ((x >= 0x25B8) && (x <= 0x25C0)) { // So   [9] BLACK RIGHT-POINTING SMALL TRIANGLE..BLACK LEFT-POINTING TRIANGLE
    return "Common";
  }
  if (x == 0x25C1 ) {//  Sm       WHITE LEFT-POINTING TRIANGLE
    return "Common";
  }
  if ((x >= 0x25C2) && (x <= 0x25F7)) { // So  [54] BLACK LEFT-POINTING SMALL TRIANGLE..WHITE CIRCLE WITH UPPER RIGHT QUADRANT
    return "Common";
  }
  if ((x >= 0x25F8) && (x <= 0x25FF)) { // Sm   [8] UPPER LEFT TRIANGLE..LOWER RIGHT TRIANGLE
    return "Common";
  }
  if ((x >= 0x2600) && (x <= 0x266E)) { // So [111] BLACK SUN WITH RAYS..MUSIC NATURAL SIGN
    return "Common";
  }
  if (x == 0x266F ) {//  Sm       MUSIC SHARP SIGN
    return "Common";
  }
  if ((x >= 0x2670) && (x <= 0x26FF)) { // So [144] WEST SYRIAC CROSS..WHITE FLAG WITH HORIZONTAL MIDDLE BLACK STRIPE
    return "Common";
  }
  if ((x >= 0x2701) && (x <= 0x2767)) { // So [103] UPPER BLADE SCISSORS..ROTATED FLORAL HEART BULLET
    return "Common";
  }
  if (x == 0x2768 ) {//  Ps       MEDIUM LEFT PARENTHESIS ORNAMENT
    return "Common";
  }
  if (x == 0x2769 ) {//  Pe       MEDIUM RIGHT PARENTHESIS ORNAMENT
    return "Common";
  }
  if (x == 0x276A ) {//  Ps       MEDIUM FLATTENED LEFT PARENTHESIS ORNAMENT
    return "Common";
  }
  if (x == 0x276B ) {//  Pe       MEDIUM FLATTENED RIGHT PARENTHESIS ORNAMENT
    return "Common";
  }
  if (x == 0x276C ) {//  Ps       MEDIUM LEFT-POINTING ANGLE BRACKET ORNAMENT
    return "Common";
  }
  if (x == 0x276D ) {//  Pe       MEDIUM RIGHT-POINTING ANGLE BRACKET ORNAMENT
    return "Common";
  }
  if (x == 0x276E ) {//  Ps       HEAVY LEFT-POINTING ANGLE QUOTATION MARK ORNAMENT
    return "Common";
  }
  if (x == 0x276F ) {//  Pe       HEAVY RIGHT-POINTING ANGLE QUOTATION MARK ORNAMENT
    return "Common";
  }
  if (x == 0x2770 ) {//  Ps       HEAVY LEFT-POINTING ANGLE BRACKET ORNAMENT
    return "Common";
  }
  if (x == 0x2771 ) {//  Pe       HEAVY RIGHT-POINTING ANGLE BRACKET ORNAMENT
    return "Common";
  }
  if (x == 0x2772 ) {//  Ps       LIGHT LEFT TORTOISE SHELL BRACKET ORNAMENT
    return "Common";
  }
  if (x == 0x2773 ) {//  Pe       LIGHT RIGHT TORTOISE SHELL BRACKET ORNAMENT
    return "Common";
  }
  if (x == 0x2774 ) {//  Ps       MEDIUM LEFT CURLY BRACKET ORNAMENT
    return "Common";
  }
  if (x == 0x2775 ) {//  Pe       MEDIUM RIGHT CURLY BRACKET ORNAMENT
    return "Common";
  }
  if ((x >= 0x2776) && (x <= 0x2793)) { // No  [30] DINGBAT NEGATIVE CIRCLED DIGIT ONE..DINGBAT NEGATIVE CIRCLED SANS-SERIF NUMBER TEN
    return "Common";
  }
  if ((x >= 0x2794) && (x <= 0x27BF)) { // So  [44] HEAVY WIDE-HEADED RIGHTWARDS ARROW..DOUBLE CURLY LOOP
    return "Common";
  }
  if ((x >= 0x27C0) && (x <= 0x27C4)) { // Sm   [5] THREE DIMENSIONAL ANGLE..OPEN SUPERSET
    return "Common";
  }
  if (x == 0x27C5 ) {//  Ps       LEFT S-SHAPED BAG DELIMITER
    return "Common";
  }
  if (x == 0x27C6 ) {//  Pe       RIGHT S-SHAPED BAG DELIMITER
    return "Common";
  }
  if ((x >= 0x27C7) && (x <= 0x27E5)) { // Sm  [31] OR WITH DOT INSIDE..WHITE SQUARE WITH RIGHTWARDS TICK
    return "Common";
  }
  if (x == 0x27E6 ) {//  Ps       MATHEMATICAL LEFT WHITE SQUARE BRACKET
    return "Common";
  }
  if (x == 0x27E7 ) {//  Pe       MATHEMATICAL RIGHT WHITE SQUARE BRACKET
    return "Common";
  }
  if (x == 0x27E8 ) {//  Ps       MATHEMATICAL LEFT ANGLE BRACKET
    return "Common";
  }
  if (x == 0x27E9 ) {//  Pe       MATHEMATICAL RIGHT ANGLE BRACKET
    return "Common";
  }
  if (x == 0x27EA ) {//  Ps       MATHEMATICAL LEFT DOUBLE ANGLE BRACKET
    return "Common";
  }
  if (x == 0x27EB ) {//  Pe       MATHEMATICAL RIGHT DOUBLE ANGLE BRACKET
    return "Common";
  }
  if (x == 0x27EC ) {//  Ps       MATHEMATICAL LEFT WHITE TORTOISE SHELL BRACKET
    return "Common";
  }
  if (x == 0x27ED ) {//  Pe       MATHEMATICAL RIGHT WHITE TORTOISE SHELL BRACKET
    return "Common";
  }
  if (x == 0x27EE ) {//  Ps       MATHEMATICAL LEFT FLATTENED PARENTHESIS
    return "Common";
  }
  if (x == 0x27EF ) {//  Pe       MATHEMATICAL RIGHT FLATTENED PARENTHESIS
    return "Common";
  }
  if ((x >= 0x27F0) && (x <= 0x27FF)) { // Sm  [16] UPWARDS QUADRUPLE ARROW..LONG RIGHTWARDS SQUIGGLE ARROW
    return "Common";
  }
  if ((x >= 0x2900) && (x <= 0x2982)) { // Sm [131] RIGHTWARDS TWO-HEADED ARROW WITH VERTICAL STROKE..Z NOTATION TYPE COLON
    return "Common";
  }
  if (x == 0x2983 ) {//  Ps       LEFT WHITE CURLY BRACKET
    return "Common";
  }
  if (x == 0x2984 ) {//  Pe       RIGHT WHITE CURLY BRACKET
    return "Common";
  }
  if (x == 0x2985 ) {//  Ps       LEFT WHITE PARENTHESIS
    return "Common";
  }
  if (x == 0x2986 ) {//  Pe       RIGHT WHITE PARENTHESIS
    return "Common";
  }
  if (x == 0x2987 ) {//  Ps       Z NOTATION LEFT IMAGE BRACKET
    return "Common";
  }
  if (x == 0x2988 ) {//  Pe       Z NOTATION RIGHT IMAGE BRACKET
    return "Common";
  }
  if (x == 0x2989 ) {//  Ps       Z NOTATION LEFT BINDING BRACKET
    return "Common";
  }
  if (x == 0x298A ) {//  Pe       Z NOTATION RIGHT BINDING BRACKET
    return "Common";
  }
  if (x == 0x298B ) {//  Ps       LEFT SQUARE BRACKET WITH UNDERBAR
    return "Common";
  }
  if (x == 0x298C ) {//  Pe       RIGHT SQUARE BRACKET WITH UNDERBAR
    return "Common";
  }
  if (x == 0x298D ) {//  Ps       LEFT SQUARE BRACKET WITH TICK IN TOP CORNER
    return "Common";
  }
  if (x == 0x298E ) {//  Pe       RIGHT SQUARE BRACKET WITH TICK IN BOTTOM CORNER
    return "Common";
  }
  if (x == 0x298F ) {//  Ps       LEFT SQUARE BRACKET WITH TICK IN BOTTOM CORNER
    return "Common";
  }
  if (x == 0x2990 ) {//  Pe       RIGHT SQUARE BRACKET WITH TICK IN TOP CORNER
    return "Common";
  }
  if (x == 0x2991 ) {//  Ps       LEFT ANGLE BRACKET WITH DOT
    return "Common";
  }
  if (x == 0x2992 ) {//  Pe       RIGHT ANGLE BRACKET WITH DOT
    return "Common";
  }
  if (x == 0x2993 ) {//  Ps       LEFT ARC LESS-THAN BRACKET
    return "Common";
  }
  if (x == 0x2994 ) {//  Pe       RIGHT ARC GREATER-THAN BRACKET
    return "Common";
  }
  if (x == 0x2995 ) {//  Ps       DOUBLE LEFT ARC GREATER-THAN BRACKET
    return "Common";
  }
  if (x == 0x2996 ) {//  Pe       DOUBLE RIGHT ARC LESS-THAN BRACKET
    return "Common";
  }
  if (x == 0x2997 ) {//  Ps       LEFT BLACK TORTOISE SHELL BRACKET
    return "Common";
  }
  if (x == 0x2998 ) {//  Pe       RIGHT BLACK TORTOISE SHELL BRACKET
    return "Common";
  }
  if ((x >= 0x2999) && (x <= 0x29D7)) { // Sm  [63] DOTTED FENCE..BLACK HOURGLASS
    return "Common";
  }
  if (x == 0x29D8 ) {//  Ps       LEFT WIGGLY FENCE
    return "Common";
  }
  if (x == 0x29D9 ) {//  Pe       RIGHT WIGGLY FENCE
    return "Common";
  }
  if (x == 0x29DA ) {//  Ps       LEFT DOUBLE WIGGLY FENCE
    return "Common";
  }
  if (x == 0x29DB ) {//  Pe       RIGHT DOUBLE WIGGLY FENCE
    return "Common";
  }
  if ((x >= 0x29DC) && (x <= 0x29FB)) { // Sm  [32] INCOMPLETE INFINITY..TRIPLE PLUS
    return "Common";
  }
  if (x == 0x29FC ) {//  Ps       LEFT-POINTING CURVED ANGLE BRACKET
    return "Common";
  }
  if (x == 0x29FD ) {//  Pe       RIGHT-POINTING CURVED ANGLE BRACKET
    return "Common";
  }
  if ((x >= 0x29FE) && (x <= 0x2AFF)) { // Sm [258] TINY..N-ARY WHITE VERTICAL BAR
    return "Common";
  }
  if ((x >= 0x2B00) && (x <= 0x2B2F)) { // So  [48] NORTH EAST WHITE ARROW..WHITE VERTICAL ELLIPSE
    return "Common";
  }
  if ((x >= 0x2B30) && (x <= 0x2B44)) { // Sm  [21] LEFT ARROW WITH SMALL CIRCLE..RIGHTWARDS ARROW THROUGH SUPERSET
    return "Common";
  }
  if ((x >= 0x2B45) && (x <= 0x2B46)) { // So   [2] LEFTWARDS QUADRUPLE ARROW..RIGHTWARDS QUADRUPLE ARROW
    return "Common";
  }
  if ((x >= 0x2B47) && (x <= 0x2B4C)) { // Sm   [6] REVERSE TILDE OPERATOR ABOVE RIGHTWARDS ARROW..RIGHTWARDS ARROW ABOVE REVERSE TILDE OPERATOR
    return "Common";
  }
  if ((x >= 0x2B50) && (x <= 0x2B59)) { // So  [10] WHITE MEDIUM STAR..HEAVY CIRCLED SALTIRE
    return "Common";
  }
  if ((x >= 0x2E00) && (x <= 0x2E01)) { // Po   [2] RIGHT ANGLE SUBSTITUTION MARKER..RIGHT ANGLE DOTTED SUBSTITUTION MARKER
    return "Common";
  }
  if (x == 0x2E02 ) {//  Pi       LEFT SUBSTITUTION BRACKET
    return "Common";
  }
  if (x == 0x2E03 ) {//  Pf       RIGHT SUBSTITUTION BRACKET
    return "Common";
  }
  if (x == 0x2E04 ) {//  Pi       LEFT DOTTED SUBSTITUTION BRACKET
    return "Common";
  }
  if (x == 0x2E05 ) {//  Pf       RIGHT DOTTED SUBSTITUTION BRACKET
    return "Common";
  }
  if ((x >= 0x2E06) && (x <= 0x2E08)) { // Po   [3] RAISED INTERPOLATION MARKER..DOTTED TRANSPOSITION MARKER
    return "Common";
  }
  if (x == 0x2E09 ) {//  Pi       LEFT TRANSPOSITION BRACKET
    return "Common";
  }
  if (x == 0x2E0A ) {//  Pf       RIGHT TRANSPOSITION BRACKET
    return "Common";
  }
  if (x == 0x2E0B ) {//  Po       RAISED SQUARE
    return "Common";
  }
  if (x == 0x2E0C ) {//  Pi       LEFT RAISED OMISSION BRACKET
    return "Common";
  }
  if (x == 0x2E0D ) {//  Pf       RIGHT RAISED OMISSION BRACKET
    return "Common";
  }
  if ((x >= 0x2E0E) && (x <= 0x2E16)) { // Po   [9] EDITORIAL CORONIS..DOTTED RIGHT-POINTING ANGLE
    return "Common";
  }
  if (x == 0x2E17 ) {//  Pd       DOUBLE OBLIQUE HYPHEN
    return "Common";
  }
  if ((x >= 0x2E18) && (x <= 0x2E19)) { // Po   [2] INVERTED INTERROBANG..PALM BRANCH
    return "Common";
  }
  if (x == 0x2E1A ) {//  Pd       HYPHEN WITH DIAERESIS
    return "Common";
  }
  if (x == 0x2E1B ) {//  Po       TILDE WITH RING ABOVE
    return "Common";
  }
  if (x == 0x2E1C ) {//  Pi       LEFT LOW PARAPHRASE BRACKET
    return "Common";
  }
  if (x == 0x2E1D ) {//  Pf       RIGHT LOW PARAPHRASE BRACKET
    return "Common";
  }
  if ((x >= 0x2E1E) && (x <= 0x2E1F)) { // Po   [2] TILDE WITH DOT ABOVE..TILDE WITH DOT BELOW
    return "Common";
  }
  if (x == 0x2E20 ) {//  Pi       LEFT VERTICAL BAR WITH QUILL
    return "Common";
  }
  if (x == 0x2E21 ) {//  Pf       RIGHT VERTICAL BAR WITH QUILL
    return "Common";
  }
  if (x == 0x2E22 ) {//  Ps       TOP LEFT HALF BRACKET
    return "Common";
  }
  if (x == 0x2E23 ) {//  Pe       TOP RIGHT HALF BRACKET
    return "Common";
  }
  if (x == 0x2E24 ) {//  Ps       BOTTOM LEFT HALF BRACKET
    return "Common";
  }
  if (x == 0x2E25 ) {//  Pe       BOTTOM RIGHT HALF BRACKET
    return "Common";
  }
  if (x == 0x2E26 ) {//  Ps       LEFT SIDEWAYS U BRACKET
    return "Common";
  }
  if (x == 0x2E27 ) {//  Pe       RIGHT SIDEWAYS U BRACKET
    return "Common";
  }
  if (x == 0x2E28 ) {//  Ps       LEFT DOUBLE PARENTHESIS
    return "Common";
  }
  if (x == 0x2E29 ) {//  Pe       RIGHT DOUBLE PARENTHESIS
    return "Common";
  }
  if ((x >= 0x2E2A) && (x <= 0x2E2E)) { // Po   [5] TWO DOTS OVER ONE DOT PUNCTUATION..REVERSED QUESTION MARK
    return "Common";
  }
  if (x == 0x2E2F ) {//  Lm       VERTICAL TILDE
    return "Common";
  }
  if ((x >= 0x2E30) && (x <= 0x2E39)) { // Po  [10] RING POINT..TOP HALF SECTION SIGN
    return "Common";
  }
  if ((x >= 0x2E3A) && (x <= 0x2E3B)) { // Pd   [2] TWO-EM DASH..THREE-EM DASH
    return "Common";
  }
  if ((x >= 0x2FF0) && (x <= 0x2FFB)) { // So  [12] IDEOGRAPHIC DESCRIPTION CHARACTER LEFT TO RIGHT..IDEOGRAPHIC DESCRIPTION CHARACTER OVERLAID
    return "Common";
  }
  if (x == 0x3000 ) {//  Zs       IDEOGRAPHIC SPACE
    return "Common";
  }
  if ((x >= 0x3001) && (x <= 0x3003)) { // Po   [3] IDEOGRAPHIC COMMA..DITTO MARK
    return "Common";
  }
  if (x == 0x3004 ) {//  So       JAPANESE INDUSTRIAL STANDARD SYMBOL
    return "Common";
  }
  if (x == 0x3006 ) {//  Lo       IDEOGRAPHIC CLOSING MARK
    return "Common";
  }
  if (x == 0x3008 ) {//  Ps       LEFT ANGLE BRACKET
    return "Common";
  }
  if (x == 0x3009 ) {//  Pe       RIGHT ANGLE BRACKET
    return "Common";
  }
  if (x == 0x300A ) {//  Ps       LEFT DOUBLE ANGLE BRACKET
    return "Common";
  }
  if (x == 0x300B ) {//  Pe       RIGHT DOUBLE ANGLE BRACKET
    return "Common";
  }
  if (x == 0x300C ) {//  Ps       LEFT CORNER BRACKET
    return "Common";
  }
  if (x == 0x300D ) {//  Pe       RIGHT CORNER BRACKET
    return "Common";
  }
  if (x == 0x300E ) {//  Ps       LEFT WHITE CORNER BRACKET
    return "Common";
  }
  if (x == 0x300F ) {//  Pe       RIGHT WHITE CORNER BRACKET
    return "Common";
  }
  if (x == 0x3010 ) {//  Ps       LEFT BLACK LENTICULAR BRACKET
    return "Common";
  }
  if (x == 0x3011 ) {//  Pe       RIGHT BLACK LENTICULAR BRACKET
    return "Common";
  }
  if ((x >= 0x3012) && (x <= 0x3013)) { // So   [2] POSTAL MARK..GETA MARK
    return "Common";
  }
  if (x == 0x3014 ) {//  Ps       LEFT TORTOISE SHELL BRACKET
    return "Common";
  }
  if (x == 0x3015 ) {//  Pe       RIGHT TORTOISE SHELL BRACKET
    return "Common";
  }
  if (x == 0x3016 ) {//  Ps       LEFT WHITE LENTICULAR BRACKET
    return "Common";
  }
  if (x == 0x3017 ) {//  Pe       RIGHT WHITE LENTICULAR BRACKET
    return "Common";
  }
  if (x == 0x3018 ) {//  Ps       LEFT WHITE TORTOISE SHELL BRACKET
    return "Common";
  }
  if (x == 0x3019 ) {//  Pe       RIGHT WHITE TORTOISE SHELL BRACKET
    return "Common";
  }
  if (x == 0x301A ) {//  Ps       LEFT WHITE SQUARE BRACKET
    return "Common";
  }
  if (x == 0x301B ) {//  Pe       RIGHT WHITE SQUARE BRACKET
    return "Common";
  }
  if (x == 0x301C ) {//  Pd       WAVE DASH
    return "Common";
  }
  if (x == 0x301D ) {//  Ps       REVERSED DOUBLE PRIME QUOTATION MARK
    return "Common";
  }
  if ((x >= 0x301E) && (x <= 0x301F)) { // Pe   [2] DOUBLE PRIME QUOTATION MARK..LOW DOUBLE PRIME QUOTATION MARK
    return "Common";
  }
  if (x == 0x3020 ) {//  So       POSTAL MARK FACE
    return "Common";
  }
  if (x == 0x3030 ) {//  Pd       WAVY DASH
    return "Common";
  }
  if ((x >= 0x3031) && (x <= 0x3035)) { // Lm   [5] VERTICAL KANA REPEAT MARK..VERTICAL KANA REPEAT MARK LOWER HALF
    return "Common";
  }
  if ((x >= 0x3036) && (x <= 0x3037)) { // So   [2] CIRCLED POSTAL MARK..IDEOGRAPHIC TELEGRAPH LINE FEED SEPARATOR SYMBOL
    return "Common";
  }
  if (x == 0x303C ) {//  Lo       MASU MARK
    return "Common";
  }
  if (x == 0x303D ) {//  Po       PART ALTERNATION MARK
    return "Common";
  }
  if ((x >= 0x303E) && (x <= 0x303F)) { // So   [2] IDEOGRAPHIC VARIATION INDICATOR..IDEOGRAPHIC HALF FILL SPACE
    return "Common";
  }
  if ((x >= 0x309B) && (x <= 0x309C)) { // Sk   [2] KATAKANA-HIRAGANA VOICED SOUND MARK..KATAKANA-HIRAGANA SEMI-VOICED SOUND MARK
    return "Common";
  }
  if (x == 0x30A0 ) {//  Pd       KATAKANA-HIRAGANA DOUBLE HYPHEN
    return "Common";
  }
  if (x == 0x30FB ) {//  Po       KATAKANA MIDDLE DOT
    return "Common";
  }
  if (x == 0x30FC ) {//  Lm       KATAKANA-HIRAGANA PROLONGED SOUND MARK
    return "Common";
  }
  if ((x >= 0x3190) && (x <= 0x3191)) { // So   [2] IDEOGRAPHIC ANNOTATION LINKING MARK..IDEOGRAPHIC ANNOTATION REVERSE MARK
    return "Common";
  }
  if ((x >= 0x3192) && (x <= 0x3195)) { // No   [4] IDEOGRAPHIC ANNOTATION ONE MARK..IDEOGRAPHIC ANNOTATION FOUR MARK
    return "Common";
  }
  if ((x >= 0x3196) && (x <= 0x319F)) { // So  [10] IDEOGRAPHIC ANNOTATION TOP MARK..IDEOGRAPHIC ANNOTATION MAN MARK
    return "Common";
  }
  if ((x >= 0x31C0) && (x <= 0x31E3)) { // So  [36] CJK STROKE T..CJK STROKE Q
    return "Common";
  }
  if ((x >= 0x3220) && (x <= 0x3229)) { // No  [10] PARENTHESIZED IDEOGRAPH ONE..PARENTHESIZED IDEOGRAPH TEN
    return "Common";
  }
  if ((x >= 0x322A) && (x <= 0x3247)) { // So  [30] PARENTHESIZED IDEOGRAPH MOON..CIRCLED IDEOGRAPH KOTO
    return "Common";
  }
  if ((x >= 0x3248) && (x <= 0x324F)) { // No   [8] CIRCLED NUMBER TEN ON BLACK SQUARE..CIRCLED NUMBER EIGHTY ON BLACK SQUARE
    return "Common";
  }
  if (x == 0x3250 ) {//  So       PARTNERSHIP SIGN
    return "Common";
  }
  if ((x >= 0x3251) && (x <= 0x325F)) { // No  [15] CIRCLED NUMBER TWENTY ONE..CIRCLED NUMBER THIRTY FIVE
    return "Common";
  }
  if (x == 0x327F ) {//  So       KOREAN STANDARD SYMBOL
    return "Common";
  }
  if ((x >= 0x3280) && (x <= 0x3289)) { // No  [10] CIRCLED IDEOGRAPH ONE..CIRCLED IDEOGRAPH TEN
    return "Common";
  }
  if ((x >= 0x328A) && (x <= 0x32B0)) { // So  [39] CIRCLED IDEOGRAPH MOON..CIRCLED IDEOGRAPH NIGHT
    return "Common";
  }
  if ((x >= 0x32B1) && (x <= 0x32BF)) { // No  [15] CIRCLED NUMBER THIRTY SIX..CIRCLED NUMBER FIFTY
    return "Common";
  }
  if ((x >= 0x32C0) && (x <= 0x32CF)) { // So  [16] IDEOGRAPHIC TELEGRAPH SYMBOL FOR JANUARY..LIMITED LIABILITY SIGN
    return "Common";
  }
  if ((x >= 0x3358) && (x <= 0x33FF)) { // So [168] IDEOGRAPHIC TELEGRAPH SYMBOL FOR HOUR ZERO..SQUARE GAL
    return "Common";
  }
  if ((x >= 0x4DC0) && (x <= 0x4DFF)) { // So  [64] HEXAGRAM FOR THE CREATIVE HEAVEN..HEXAGRAM FOR BEFORE COMPLETION
    return "Common";
  }
  if ((x >= 0xA700) && (x <= 0xA716)) { // Sk  [23] MODIFIER LETTER CHINESE TONE YIN PING..MODIFIER LETTER EXTRA-LOW LEFT-STEM TONE BAR
    return "Common";
  }
  if ((x >= 0xA717) && (x <= 0xA71F)) { // Lm   [9] MODIFIER LETTER DOT VERTICAL BAR..MODIFIER LETTER LOW INVERTED EXCLAMATION MARK
    return "Common";
  }
  if ((x >= 0xA720) && (x <= 0xA721)) { // Sk   [2] MODIFIER LETTER STRESS AND HIGH TONE..MODIFIER LETTER STRESS AND LOW TONE
    return "Common";
  }
  if (x == 0xA788 ) {//  Lm       MODIFIER LETTER LOW CIRCUMFLEX ACCENT
    return "Common";
  }
  if ((x >= 0xA789) && (x <= 0xA78A)) { // Sk   [2] MODIFIER LETTER COLON..MODIFIER LETTER SHORT EQUALS SIGN
    return "Common";
  }
  if ((x >= 0xA830) && (x <= 0xA835)) { // No   [6] NORTH INDIC FRACTION ONE QUARTER..NORTH INDIC FRACTION THREE SIXTEENTHS
    return "Common";
  }
  if ((x >= 0xA836) && (x <= 0xA837)) { // So   [2] NORTH INDIC QUARTER MARK..NORTH INDIC PLACEHOLDER MARK
    return "Common";
  }
  if (x == 0xA838 ) {//  Sc       NORTH INDIC RUPEE MARK
    return "Common";
  }
  if (x == 0xA839 ) {//  So       NORTH INDIC QUANTITY MARK
    return "Common";
  }
  if (x == 0xFD3E ) {//  Ps       ORNATE LEFT PARENTHESIS
    return "Common";
  }
  if (x == 0xFD3F ) {//  Pe       ORNATE RIGHT PARENTHESIS
    return "Common";
  }
  if (x == 0xFDFD ) {//  So       ARABIC LIGATURE BISMILLAH AR-RAHMAN AR-RAHEEM
    return "Common";
  }
  if ((x >= 0xFE10) && (x <= 0xFE16)) { // Po   [7] PRESENTATION FORM FOR VERTICAL COMMA..PRESENTATION FORM FOR VERTICAL QUESTION MARK
    return "Common";
  }
  if (x == 0xFE17 ) {//  Ps       PRESENTATION FORM FOR VERTICAL LEFT WHITE LENTICULAR BRACKET
    return "Common";
  }
  if (x == 0xFE18 ) {//  Pe       PRESENTATION FORM FOR VERTICAL RIGHT WHITE LENTICULAR BRAKCET
    return "Common";
  }
  if (x == 0xFE19 ) {//  Po       PRESENTATION FORM FOR VERTICAL HORIZONTAL ELLIPSIS
    return "Common";
  }
  if (x == 0xFE30 ) {//  Po       PRESENTATION FORM FOR VERTICAL TWO DOT LEADER
    return "Common";
  }
  if ((x >= 0xFE31) && (x <= 0xFE32)) { // Pd   [2] PRESENTATION FORM FOR VERTICAL EM DASH..PRESENTATION FORM FOR VERTICAL EN DASH
    return "Common";
  }
  if ((x >= 0xFE33) && (x <= 0xFE34)) { // Pc   [2] PRESENTATION FORM FOR VERTICAL LOW LINE..PRESENTATION FORM FOR VERTICAL WAVY LOW LINE
    return "Common";
  }
  if (x == 0xFE35 ) {//  Ps       PRESENTATION FORM FOR VERTICAL LEFT PARENTHESIS
    return "Common";
  }
  if (x == 0xFE36 ) {//  Pe       PRESENTATION FORM FOR VERTICAL RIGHT PARENTHESIS
    return "Common";
  }
  if (x == 0xFE37 ) {//  Ps       PRESENTATION FORM FOR VERTICAL LEFT CURLY BRACKET
    return "Common";
  }
  if (x == 0xFE38 ) {//  Pe       PRESENTATION FORM FOR VERTICAL RIGHT CURLY BRACKET
    return "Common";
  }
  if (x == 0xFE39 ) {//  Ps       PRESENTATION FORM FOR VERTICAL LEFT TORTOISE SHELL BRACKET
    return "Common";
  }
  if (x == 0xFE3A ) {//  Pe       PRESENTATION FORM FOR VERTICAL RIGHT TORTOISE SHELL BRACKET
    return "Common";
  }
  if (x == 0xFE3B ) {//  Ps       PRESENTATION FORM FOR VERTICAL LEFT BLACK LENTICULAR BRACKET
    return "Common";
  }
  if (x == 0xFE3C ) {//  Pe       PRESENTATION FORM FOR VERTICAL RIGHT BLACK LENTICULAR BRACKET
    return "Common";
  }
  if (x == 0xFE3D ) {//  Ps       PRESENTATION FORM FOR VERTICAL LEFT DOUBLE ANGLE BRACKET
    return "Common";
  }
  if (x == 0xFE3E ) {//  Pe       PRESENTATION FORM FOR VERTICAL RIGHT DOUBLE ANGLE BRACKET
    return "Common";
  }
  if (x == 0xFE3F ) {//  Ps       PRESENTATION FORM FOR VERTICAL LEFT ANGLE BRACKET
    return "Common";
  }
  if (x == 0xFE40 ) {//  Pe       PRESENTATION FORM FOR VERTICAL RIGHT ANGLE BRACKET
    return "Common";
  }
  if (x == 0xFE41 ) {//  Ps       PRESENTATION FORM FOR VERTICAL LEFT CORNER BRACKET
    return "Common";
  }
  if (x == 0xFE42 ) {//  Pe       PRESENTATION FORM FOR VERTICAL RIGHT CORNER BRACKET
    return "Common";
  }
  if (x == 0xFE43 ) {//  Ps       PRESENTATION FORM FOR VERTICAL LEFT WHITE CORNER BRACKET
    return "Common";
  }
  if (x == 0xFE44 ) {//  Pe       PRESENTATION FORM FOR VERTICAL RIGHT WHITE CORNER BRACKET
    return "Common";
  }
  if ((x >= 0xFE45) && (x <= 0xFE46)) { // Po   [2] SESAME DOT..WHITE SESAME DOT
    return "Common";
  }
  if (x == 0xFE47 ) {//  Ps       PRESENTATION FORM FOR VERTICAL LEFT SQUARE BRACKET
    return "Common";
  }
  if (x == 0xFE48 ) {//  Pe       PRESENTATION FORM FOR VERTICAL RIGHT SQUARE BRACKET
    return "Common";
  }
  if ((x >= 0xFE49) && (x <= 0xFE4C)) { // Po   [4] DASHED OVERLINE..DOUBLE WAVY OVERLINE
    return "Common";
  }
  if ((x >= 0xFE4D) && (x <= 0xFE4F)) { // Pc   [3] DASHED LOW LINE..WAVY LOW LINE
    return "Common";
  }
  if ((x >= 0xFE50) && (x <= 0xFE52)) { // Po   [3] SMALL COMMA..SMALL FULL STOP
    return "Common";
  }
  if ((x >= 0xFE54) && (x <= 0xFE57)) { // Po   [4] SMALL SEMICOLON..SMALL EXCLAMATION MARK
    return "Common";
  }
  if (x == 0xFE58 ) {//  Pd       SMALL EM DASH
    return "Common";
  }
  if (x == 0xFE59 ) {//  Ps       SMALL LEFT PARENTHESIS
    return "Common";
  }
  if (x == 0xFE5A ) {//  Pe       SMALL RIGHT PARENTHESIS
    return "Common";
  }
  if (x == 0xFE5B ) {//  Ps       SMALL LEFT CURLY BRACKET
    return "Common";
  }
  if (x == 0xFE5C ) {//  Pe       SMALL RIGHT CURLY BRACKET
    return "Common";
  }
  if (x == 0xFE5D ) {//  Ps       SMALL LEFT TORTOISE SHELL BRACKET
    return "Common";
  }
  if (x == 0xFE5E ) {//  Pe       SMALL RIGHT TORTOISE SHELL BRACKET
    return "Common";
  }
  if ((x >= 0xFE5F) && (x <= 0xFE61)) { // Po   [3] SMALL NUMBER SIGN..SMALL ASTERISK
    return "Common";
  }
  if (x == 0xFE62 ) {//  Sm       SMALL PLUS SIGN
    return "Common";
  }
  if (x == 0xFE63 ) {//  Pd       SMALL HYPHEN-MINUS
    return "Common";
  }
  if ((x >= 0xFE64) && (x <= 0xFE66)) { // Sm   [3] SMALL LESS-THAN SIGN..SMALL EQUALS SIGN
    return "Common";
  }
  if (x == 0xFE68 ) {//  Po       SMALL REVERSE SOLIDUS
    return "Common";
  }
  if (x == 0xFE69 ) {//  Sc       SMALL DOLLAR SIGN
    return "Common";
  }
  if ((x >= 0xFE6A) && (x <= 0xFE6B)) { // Po   [2] SMALL PERCENT SIGN..SMALL COMMERCIAL AT
    return "Common";
  }
  if (x == 0xFEFF ) {//  Cf       ZERO WIDTH NO-BREAK SPACE
    return "Common";
  }
  if ((x >= 0xFF01) && (x <= 0xFF03)) { // Po   [3] FULLWIDTH EXCLAMATION MARK..FULLWIDTH NUMBER SIGN
    return "Common";
  }
  if (x == 0xFF04 ) {//  Sc       FULLWIDTH DOLLAR SIGN
    return "Common";
  }
  if ((x >= 0xFF05) && (x <= 0xFF07)) { // Po   [3] FULLWIDTH PERCENT SIGN..FULLWIDTH APOSTROPHE
    return "Common";
  }
  if (x == 0xFF08 ) {//  Ps       FULLWIDTH LEFT PARENTHESIS
    return "Common";
  }
  if (x == 0xFF09 ) {//  Pe       FULLWIDTH RIGHT PARENTHESIS
    return "Common";
  }
  if (x == 0xFF0A ) {//  Po       FULLWIDTH ASTERISK
    return "Common";
  }
  if (x == 0xFF0B ) {//  Sm       FULLWIDTH PLUS SIGN
    return "Common";
  }
  if (x == 0xFF0C ) {//  Po       FULLWIDTH COMMA
    return "Common";
  }
  if (x == 0xFF0D ) {//  Pd       FULLWIDTH HYPHEN-MINUS
    return "Common";
  }
  if ((x >= 0xFF0E) && (x <= 0xFF0F)) { // Po   [2] FULLWIDTH FULL STOP..FULLWIDTH SOLIDUS
    return "Common";
  }
  if ((x >= 0xFF10) && (x <= 0xFF19)) { // Nd  [10] FULLWIDTH DIGIT ZERO..FULLWIDTH DIGIT NINE
    return "Common";
  }
  if ((x >= 0xFF1A) && (x <= 0xFF1B)) { // Po   [2] FULLWIDTH COLON..FULLWIDTH SEMICOLON
    return "Common";
  }
  if ((x >= 0xFF1C) && (x <= 0xFF1E)) { // Sm   [3] FULLWIDTH LESS-THAN SIGN..FULLWIDTH GREATER-THAN SIGN
    return "Common";
  }
  if ((x >= 0xFF1F) && (x <= 0xFF20)) { // Po   [2] FULLWIDTH QUESTION MARK..FULLWIDTH COMMERCIAL AT
    return "Common";
  }
  if (x == 0xFF3B ) {//  Ps       FULLWIDTH LEFT SQUARE BRACKET
    return "Common";
  }
  if (x == 0xFF3C ) {//  Po       FULLWIDTH REVERSE SOLIDUS
    return "Common";
  }
  if (x == 0xFF3D ) {//  Pe       FULLWIDTH RIGHT SQUARE BRACKET
    return "Common";
  }
  if (x == 0xFF3E ) {//  Sk       FULLWIDTH CIRCUMFLEX ACCENT
    return "Common";
  }
  if (x == 0xFF3F ) {//  Pc       FULLWIDTH LOW LINE
    return "Common";
  }
  if (x == 0xFF40 ) {//  Sk       FULLWIDTH GRAVE ACCENT
    return "Common";
  }
  if (x == 0xFF5B ) {//  Ps       FULLWIDTH LEFT CURLY BRACKET
    return "Common";
  }
  if (x == 0xFF5C ) {//  Sm       FULLWIDTH VERTICAL LINE
    return "Common";
  }
  if (x == 0xFF5D ) {//  Pe       FULLWIDTH RIGHT CURLY BRACKET
    return "Common";
  }
  if (x == 0xFF5E ) {//  Sm       FULLWIDTH TILDE
    return "Common";
  }
  if (x == 0xFF5F ) {//  Ps       FULLWIDTH LEFT WHITE PARENTHESIS
    return "Common";
  }
  if (x == 0xFF60 ) {//  Pe       FULLWIDTH RIGHT WHITE PARENTHESIS
    return "Common";
  }
  if (x == 0xFF61 ) {//  Po       HALFWIDTH IDEOGRAPHIC FULL STOP
    return "Common";
  }
  if (x == 0xFF62 ) {//  Ps       HALFWIDTH LEFT CORNER BRACKET
    return "Common";
  }
  if (x == 0xFF63 ) {//  Pe       HALFWIDTH RIGHT CORNER BRACKET
    return "Common";
  }
  if ((x >= 0xFF64) && (x <= 0xFF65)) { // Po   [2] HALFWIDTH IDEOGRAPHIC COMMA..HALFWIDTH KATAKANA MIDDLE DOT
    return "Common";
  }
  if (x == 0xFF70 ) {//  Lm       HALFWIDTH KATAKANA-HIRAGANA PROLONGED SOUND MARK
    return "Common";
  }
  if ((x >= 0xFF9E) && (x <= 0xFF9F)) { // Lm   [2] HALFWIDTH KATAKANA VOICED SOUND MARK..HALFWIDTH KATAKANA SEMI-VOICED SOUND MARK
    return "Common";
  }
  if ((x >= 0xFFE0) && (x <= 0xFFE1)) { // Sc   [2] FULLWIDTH CENT SIGN..FULLWIDTH POUND SIGN
    return "Common";
  }
  if (x == 0xFFE2 ) {//  Sm       FULLWIDTH NOT SIGN
    return "Common";
  }
  if (x == 0xFFE3 ) {//  Sk       FULLWIDTH MACRON
    return "Common";
  }
  if (x == 0xFFE4 ) {//  So       FULLWIDTH BROKEN BAR
    return "Common";
  }
  if ((x >= 0xFFE5) && (x <= 0xFFE6)) { // Sc   [2] FULLWIDTH YEN SIGN..FULLWIDTH WON SIGN
    return "Common";
  }
  if (x == 0xFFE8 ) {//  So       HALFWIDTH FORMS LIGHT VERTICAL
    return "Common";
  }
  if ((x >= 0xFFE9) && (x <= 0xFFEC)) { // Sm   [4] HALFWIDTH LEFTWARDS ARROW..HALFWIDTH DOWNWARDS ARROW
    return "Common";
  }
  if ((x >= 0xFFED) && (x <= 0xFFEE)) { // So   [2] HALFWIDTH BLACK SQUARE..HALFWIDTH WHITE CIRCLE
    return "Common";
  }
  if ((x >= 0xFFF9) && (x <= 0xFFFB)) { // Cf   [3] INTERLINEAR ANNOTATION ANCHOR..INTERLINEAR ANNOTATION TERMINATOR
    return "Common";
  }
  if ((x >= 0xFFFC) && (x <= 0xFFFD)) { // So   [2] OBJECT REPLACEMENT CHARACTER..REPLACEMENT CHARACTER
    return "Common";
  }
  if ((x >= 0x10100) && (x <= 0x10102)) { // Po   [3] AEGEAN WORD SEPARATOR LINE..AEGEAN CHECK MARK
    return "Common";
  }
  if ((x >= 0x10107) && (x <= 0x10133)) { // No  [45] AEGEAN NUMBER ONE..AEGEAN NUMBER NINETY THOUSAND
    return "Common";
  }
  if ((x >= 0x10137) && (x <= 0x1013F)) { // So   [9] AEGEAN WEIGHT BASE UNIT..AEGEAN MEASURE THIRD SUBUNIT
    return "Common";
  }
  if ((x >= 0x10190) && (x <= 0x1019B)) { // So  [12] ROMAN SEXTANS SIGN..ROMAN CENTURIAL SIGN
    return "Common";
  }
  if ((x >= 0x101D0) && (x <= 0x101FC)) { // So  [45] PHAISTOS DISC SIGN PEDESTRIAN..PHAISTOS DISC SIGN WAVY BAND
    return "Common";
  }
  if ((x >= 0x1D000) && (x <= 0x1D0F5)) { // So [246] BYZANTINE MUSICAL SYMBOL PSILI..BYZANTINE MUSICAL SYMBOL GORGON NEO KATO
    return "Common";
  }
  if ((x >= 0x1D100) && (x <= 0x1D126)) { // So  [39] MUSICAL SYMBOL SINGLE BARLINE..MUSICAL SYMBOL DRUM CLEF-2
    return "Common";
  }
  if ((x >= 0x1D129) && (x <= 0x1D164)) { // So  [60] MUSICAL SYMBOL MULTIPLE MEASURE REST..MUSICAL SYMBOL ONE HUNDRED TWENTY-EIGHTH NOTE
    return "Common";
  }
  if ((x >= 0x1D165) && (x <= 0x1D166)) { // Mc   [2] MUSICAL SYMBOL COMBINING STEM..MUSICAL SYMBOL COMBINING SPRECHGESANG STEM
    return "Common";
  }
  if ((x >= 0x1D16A) && (x <= 0x1D16C)) { // So   [3] MUSICAL SYMBOL FINGERED TREMOLO-1..MUSICAL SYMBOL FINGERED TREMOLO-3
    return "Common";
  }
  if ((x >= 0x1D16D) && (x <= 0x1D172)) { // Mc   [6] MUSICAL SYMBOL COMBINING AUGMENTATION DOT..MUSICAL SYMBOL COMBINING FLAG-5
    return "Common";
  }
  if ((x >= 0x1D173) && (x <= 0x1D17A)) { // Cf   [8] MUSICAL SYMBOL BEGIN BEAM..MUSICAL SYMBOL END PHRASE
    return "Common";
  }
  if ((x >= 0x1D183) && (x <= 0x1D184)) { // So   [2] MUSICAL SYMBOL ARPEGGIATO UP..MUSICAL SYMBOL ARPEGGIATO DOWN
    return "Common";
  }
  if ((x >= 0x1D18C) && (x <= 0x1D1A9)) { // So  [30] MUSICAL SYMBOL RINFORZANDO..MUSICAL SYMBOL DEGREE SLASH
    return "Common";
  }
  if ((x >= 0x1D1AE) && (x <= 0x1D1DD)) { // So  [48] MUSICAL SYMBOL PEDAL MARK..MUSICAL SYMBOL PES SUBPUNCTIS
    return "Common";
  }
  if ((x >= 0x1D300) && (x <= 0x1D356)) { // So  [87] MONOGRAM FOR EARTH..TETRAGRAM FOR FOSTERING
    return "Common";
  }
  if ((x >= 0x1D360) && (x <= 0x1D371)) { // No  [18] COUNTING ROD UNIT DIGIT ONE..COUNTING ROD TENS DIGIT NINE
    return "Common";
  }
  if ((x >= 0x1D400) && (x <= 0x1D454)) { // L&  [85] MATHEMATICAL BOLD CAPITAL A..MATHEMATICAL ITALIC SMALL G
    return "Common";
  }
  if ((x >= 0x1D456) && (x <= 0x1D49C)) { // L&  [71] MATHEMATICAL ITALIC SMALL I..MATHEMATICAL SCRIPT CAPITAL A
    return "Common";
  }
  if ((x >= 0x1D49E) && (x <= 0x1D49F)) { // L&   [2] MATHEMATICAL SCRIPT CAPITAL C..MATHEMATICAL SCRIPT CAPITAL D
    return "Common";
  }
  if (x == 0x1D4A2 ) {//  L&       MATHEMATICAL SCRIPT CAPITAL G
    return "Common";
  }
  if ((x >= 0x1D4A5) && (x <= 0x1D4A6)) { // L&   [2] MATHEMATICAL SCRIPT CAPITAL J..MATHEMATICAL SCRIPT CAPITAL K
    return "Common";
  }
  if ((x >= 0x1D4A9) && (x <= 0x1D4AC)) { // L&   [4] MATHEMATICAL SCRIPT CAPITAL N..MATHEMATICAL SCRIPT CAPITAL Q
    return "Common";
  }
  if ((x >= 0x1D4AE) && (x <= 0x1D4B9)) { // L&  [12] MATHEMATICAL SCRIPT CAPITAL S..MATHEMATICAL SCRIPT SMALL D
    return "Common";
  }
  if (x == 0x1D4BB ) {//  L&       MATHEMATICAL SCRIPT SMALL F
    return "Common";
  }
  if ((x >= 0x1D4BD) && (x <= 0x1D4C3)) { // L&   [7] MATHEMATICAL SCRIPT SMALL H..MATHEMATICAL SCRIPT SMALL N
    return "Common";
  }
  if ((x >= 0x1D4C5) && (x <= 0x1D505)) { // L&  [65] MATHEMATICAL SCRIPT SMALL P..MATHEMATICAL FRAKTUR CAPITAL B
    return "Common";
  }
  if ((x >= 0x1D507) && (x <= 0x1D50A)) { // L&   [4] MATHEMATICAL FRAKTUR CAPITAL D..MATHEMATICAL FRAKTUR CAPITAL G
    return "Common";
  }
  if ((x >= 0x1D50D) && (x <= 0x1D514)) { // L&   [8] MATHEMATICAL FRAKTUR CAPITAL J..MATHEMATICAL FRAKTUR CAPITAL Q
    return "Common";
  }
  if ((x >= 0x1D516) && (x <= 0x1D51C)) { // L&   [7] MATHEMATICAL FRAKTUR CAPITAL S..MATHEMATICAL FRAKTUR CAPITAL Y
    return "Common";
  }
  if ((x >= 0x1D51E) && (x <= 0x1D539)) { // L&  [28] MATHEMATICAL FRAKTUR SMALL A..MATHEMATICAL DOUBLE-STRUCK CAPITAL B
    return "Common";
  }
  if ((x >= 0x1D53B) && (x <= 0x1D53E)) { // L&   [4] MATHEMATICAL DOUBLE-STRUCK CAPITAL D..MATHEMATICAL DOUBLE-STRUCK CAPITAL G
    return "Common";
  }
  if ((x >= 0x1D540) && (x <= 0x1D544)) { // L&   [5] MATHEMATICAL DOUBLE-STRUCK CAPITAL I..MATHEMATICAL DOUBLE-STRUCK CAPITAL M
    return "Common";
  }
  if (x == 0x1D546 ) {//  L&       MATHEMATICAL DOUBLE-STRUCK CAPITAL O
    return "Common";
  }
  if ((x >= 0x1D54A) && (x <= 0x1D550)) { // L&   [7] MATHEMATICAL DOUBLE-STRUCK CAPITAL S..MATHEMATICAL DOUBLE-STRUCK CAPITAL Y
    return "Common";
  }
  if ((x >= 0x1D552) && (x <= 0x1D6A5)) { // L& [340] MATHEMATICAL DOUBLE-STRUCK SMALL A..MATHEMATICAL ITALIC SMALL DOTLESS J
    return "Common";
  }
  if ((x >= 0x1D6A8) && (x <= 0x1D6C0)) { // L&  [25] MATHEMATICAL BOLD CAPITAL ALPHA..MATHEMATICAL BOLD CAPITAL OMEGA
    return "Common";
  }
  if (x == 0x1D6C1 ) {//  Sm       MATHEMATICAL BOLD NABLA
    return "Common";
  }
  if ((x >= 0x1D6C2) && (x <= 0x1D6DA)) { // L&  [25] MATHEMATICAL BOLD SMALL ALPHA..MATHEMATICAL BOLD SMALL OMEGA
    return "Common";
  }
  if (x == 0x1D6DB ) {//  Sm       MATHEMATICAL BOLD PARTIAL DIFFERENTIAL
    return "Common";
  }
  if ((x >= 0x1D6DC) && (x <= 0x1D6FA)) { // L&  [31] MATHEMATICAL BOLD EPSILON SYMBOL..MATHEMATICAL ITALIC CAPITAL OMEGA
    return "Common";
  }
  if (x == 0x1D6FB ) {//  Sm       MATHEMATICAL ITALIC NABLA
    return "Common";
  }
  if ((x >= 0x1D6FC) && (x <= 0x1D714)) { // L&  [25] MATHEMATICAL ITALIC SMALL ALPHA..MATHEMATICAL ITALIC SMALL OMEGA
    return "Common";
  }
  if (x == 0x1D715 ) {//  Sm       MATHEMATICAL ITALIC PARTIAL DIFFERENTIAL
    return "Common";
  }
  if ((x >= 0x1D716) && (x <= 0x1D734)) { // L&  [31] MATHEMATICAL ITALIC EPSILON SYMBOL..MATHEMATICAL BOLD ITALIC CAPITAL OMEGA
    return "Common";
  }
  if (x == 0x1D735 ) {//  Sm       MATHEMATICAL BOLD ITALIC NABLA
    return "Common";
  }
  if ((x >= 0x1D736) && (x <= 0x1D74E)) { // L&  [25] MATHEMATICAL BOLD ITALIC SMALL ALPHA..MATHEMATICAL BOLD ITALIC SMALL OMEGA
    return "Common";
  }
  if (x == 0x1D74F ) {//  Sm       MATHEMATICAL BOLD ITALIC PARTIAL DIFFERENTIAL
    return "Common";
  }
  if ((x >= 0x1D750) && (x <= 0x1D76E)) { // L&  [31] MATHEMATICAL BOLD ITALIC EPSILON SYMBOL..MATHEMATICAL SANS-SERIF BOLD CAPITAL OMEGA
    return "Common";
  }
  if (x == 0x1D76F ) {//  Sm       MATHEMATICAL SANS-SERIF BOLD NABLA
    return "Common";
  }
  if ((x >= 0x1D770) && (x <= 0x1D788)) { // L&  [25] MATHEMATICAL SANS-SERIF BOLD SMALL ALPHA..MATHEMATICAL SANS-SERIF BOLD SMALL OMEGA
    return "Common";
  }
  if (x == 0x1D789 ) {//  Sm       MATHEMATICAL SANS-SERIF BOLD PARTIAL DIFFERENTIAL
    return "Common";
  }
  if ((x >= 0x1D78A) && (x <= 0x1D7A8)) { // L&  [31] MATHEMATICAL SANS-SERIF BOLD EPSILON SYMBOL..MATHEMATICAL SANS-SERIF BOLD ITALIC CAPITAL OMEGA
    return "Common";
  }
  if (x == 0x1D7A9 ) {//  Sm       MATHEMATICAL SANS-SERIF BOLD ITALIC NABLA
    return "Common";
  }
  if ((x >= 0x1D7AA) && (x <= 0x1D7C2)) { // L&  [25] MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL ALPHA..MATHEMATICAL SANS-SERIF BOLD ITALIC SMALL OMEGA
    return "Common";
  }
  if (x == 0x1D7C3 ) {//  Sm       MATHEMATICAL SANS-SERIF BOLD ITALIC PARTIAL DIFFERENTIAL
    return "Common";
  }
  if ((x >= 0x1D7C4) && (x <= 0x1D7CB)) { // L&   [8] MATHEMATICAL SANS-SERIF BOLD ITALIC EPSILON SYMBOL..MATHEMATICAL BOLD SMALL DIGAMMA
    return "Common";
  }
  if ((x >= 0x1D7CE) && (x <= 0x1D7FF)) { // Nd  [50] MATHEMATICAL BOLD DIGIT ZERO..MATHEMATICAL MONOSPACE DIGIT NINE
    return "Common";
  }
  if ((x >= 0x1F000) && (x <= 0x1F02B)) { // So  [44] MAHJONG TILE EAST WIND..MAHJONG TILE BACK
    return "Common";
  }
  if ((x >= 0x1F030) && (x <= 0x1F093)) { // So [100] DOMINO TILE HORIZONTAL BACK..DOMINO TILE VERTICAL-06-06
    return "Common";
  }
  if ((x >= 0x1F0A0) && (x <= 0x1F0AE)) { // So  [15] PLAYING CARD BACK..PLAYING CARD KING OF SPADES
    return "Common";
  }
  if ((x >= 0x1F0B1) && (x <= 0x1F0BE)) { // So  [14] PLAYING CARD ACE OF HEARTS..PLAYING CARD KING OF HEARTS
    return "Common";
  }
  if ((x >= 0x1F0C1) && (x <= 0x1F0CF)) { // So  [15] PLAYING CARD ACE OF DIAMONDS..PLAYING CARD BLACK JOKER
    return "Common";
  }
  if ((x >= 0x1F0D1) && (x <= 0x1F0DF)) { // So  [15] PLAYING CARD ACE OF CLUBS..PLAYING CARD WHITE JOKER
    return "Common";
  }
  if ((x >= 0x1F100) && (x <= 0x1F10A)) { // No  [11] DIGIT ZERO FULL STOP..DIGIT NINE COMMA
    return "Common";
  }
  if ((x >= 0x1F110) && (x <= 0x1F12E)) { // So  [31] PARENTHESIZED LATIN CAPITAL LETTER A..CIRCLED WZ
    return "Common";
  }
  if ((x >= 0x1F130) && (x <= 0x1F16B)) { // So  [60] SQUARED LATIN CAPITAL LETTER A..RAISED MD SIGN
    return "Common";
  }
  if ((x >= 0x1F170) && (x <= 0x1F19A)) { // So  [43] NEGATIVE SQUARED LATIN CAPITAL LETTER A..SQUARED VS
    return "Common";
  }
  if ((x >= 0x1F1E6) && (x <= 0x1F1FF)) { // So  [26] REGIONAL INDICATOR SYMBOL LETTER A..REGIONAL INDICATOR SYMBOL LETTER Z
    return "Common";
  }
  if ((x >= 0x1F201) && (x <= 0x1F202)) { // So   [2] SQUARED KATAKANA KOKO..SQUARED KATAKANA SA
    return "Common";
  }
  if ((x >= 0x1F210) && (x <= 0x1F23A)) { // So  [43] SQUARED CJK UNIFIED IDEOGRAPH-624B..SQUARED CJK UNIFIED IDEOGRAPH-55B6
    return "Common";
  }
  if ((x >= 0x1F240) && (x <= 0x1F248)) { // So   [9] TORTOISE SHELL BRACKETED CJK UNIFIED IDEOGRAPH-672C..TORTOISE SHELL BRACKETED CJK UNIFIED IDEOGRAPH-6557
    return "Common";
  }
  if ((x >= 0x1F250) && (x <= 0x1F251)) { // So   [2] CIRCLED IDEOGRAPH ADVANTAGE..CIRCLED IDEOGRAPH ACCEPT
    return "Common";
  }
  if ((x >= 0x1F300) && (x <= 0x1F320)) { // So  [33] CYCLONE..SHOOTING STAR
    return "Common";
  }
  if ((x >= 0x1F330) && (x <= 0x1F335)) { // So   [6] CHESTNUT..CACTUS
    return "Common";
  }
  if ((x >= 0x1F337) && (x <= 0x1F37C)) { // So  [70] TULIP..BABY BOTTLE
    return "Common";
  }
  if ((x >= 0x1F380) && (x <= 0x1F393)) { // So  [20] RIBBON..GRADUATION CAP
    return "Common";
  }
  if ((x >= 0x1F3A0) && (x <= 0x1F3C4)) { // So  [37] CAROUSEL HORSE..SURFER
    return "Common";
  }
  if ((x >= 0x1F3C6) && (x <= 0x1F3CA)) { // So   [5] TROPHY..SWIMMER
    return "Common";
  }
  if ((x >= 0x1F3E0) && (x <= 0x1F3F0)) { // So  [17] HOUSE BUILDING..EUROPEAN CASTLE
    return "Common";
  }
  if ((x >= 0x1F400) && (x <= 0x1F43E)) { // So  [63] RAT..PAW PRINTS
    return "Common";
  }
  if (x == 0x1F440 ) {//  So       EYES
    return "Common";
  }
  if ((x >= 0x1F442) && (x <= 0x1F4F7)) { // So [182] EAR..CAMERA
    return "Common";
  }
  if ((x >= 0x1F4F9) && (x <= 0x1F4FC)) { // So   [4] VIDEO CAMERA..VIDEOCASSETTE
    return "Common";
  }
  if ((x >= 0x1F500) && (x <= 0x1F53D)) { // So  [62] TWISTED RIGHTWARDS ARROWS..DOWN-POINTING SMALL RED TRIANGLE
    return "Common";
  }
  if ((x >= 0x1F540) && (x <= 0x1F543)) { // So   [4] CIRCLED CROSS POMMEE..NOTCHED LEFT SEMICIRCLE WITH THREE DOTS
    return "Common";
  }
  if ((x >= 0x1F550) && (x <= 0x1F567)) { // So  [24] CLOCK FACE ONE OCLOCK..CLOCK FACE TWELVE-THIRTY
    return "Common";
  }
  if ((x >= 0x1F5FB) && (x <= 0x1F640)) { // So  [70] MOUNT FUJI..WEARY CAT FACE
    return "Common";
  }
  if ((x >= 0x1F645) && (x <= 0x1F64F)) { // So  [11] FACE WITH NO GOOD GESTURE..PERSON WITH FOLDED HANDS
    return "Common";
  }
  if ((x >= 0x1F680) && (x <= 0x1F6C5)) { // So  [70] ROCKET..LEFT LUGGAGE
    return "Common";
  }
  if ((x >= 0x1F700) && (x <= 0x1F773)) { // So [116] ALCHEMICAL SYMBOL FOR QUINTESSENCE..ALCHEMICAL SYMBOL FOR HALF OUNCE
    return "Common";
  }
  if (x == 0xE0001 ) {//  Cf       LANGUAGE TAG
    return "Common";
  }
  if ((x >= 0xE0020) && (x <= 0xE007F)) { // Cf  [96] TAG SPACE..CANCEL TAG
    return "Common";
  }
  // Total code points: 6413
  // ================================================
  if ((x >= 0x0041) && (x <= 0x005A)) { // L&  [26] LATIN CAPITAL LETTER A..LATIN CAPITAL LETTER Z
    return "Latin";
  }
  if ((x >= 0x0061) && (x <= 0x007A)) { // L&  [26] LATIN SMALL LETTER A..LATIN SMALL LETTER Z
    return "Latin";
  }
  if (x == 0x00AA ) {//  Lo       FEMININE ORDINAL INDICATOR
    return "Latin";
  }
  if (x == 0x00BA ) {//  Lo       MASCULINE ORDINAL INDICATOR
    return "Latin";
  }
  if ((x >= 0x00C0) && (x <= 0x00D6)) { // L&  [23] LATIN CAPITAL LETTER A WITH GRAVE..LATIN CAPITAL LETTER O WITH DIAERESIS
    return "Latin";
  }
  if ((x >= 0x00D8) && (x <= 0x00F6)) { // L&  [31] LATIN CAPITAL LETTER O WITH STROKE..LATIN SMALL LETTER O WITH DIAERESIS
    return "Latin";
  }
  if ((x >= 0x00F8) && (x <= 0x01BA)) { // L& [195] LATIN SMALL LETTER O WITH STROKE..LATIN SMALL LETTER EZH WITH TAIL
    return "Latin";
  }
  if (x == 0x01BB ) {//  Lo       LATIN LETTER TWO WITH STROKE
    return "Latin";
  }
  if ((x >= 0x01BC) && (x <= 0x01BF)) { // L&   [4] LATIN CAPITAL LETTER TONE FIVE..LATIN LETTER WYNN
    return "Latin";
  }
  if ((x >= 0x01C0) && (x <= 0x01C3)) { // Lo   [4] LATIN LETTER DENTAL CLICK..LATIN LETTER RETROFLEX CLICK
    return "Latin";
  }
  if ((x >= 0x01C4) && (x <= 0x0293)) { // L& [208] LATIN CAPITAL LETTER DZ WITH CARON..LATIN SMALL LETTER EZH WITH CURL
    return "Latin";
  }
  if (x == 0x0294 ) {//  Lo       LATIN LETTER GLOTTAL STOP
    return "Latin";
  }
  if ((x >= 0x0295) && (x <= 0x02AF)) { // L&  [27] LATIN LETTER PHARYNGEAL VOICED FRICATIVE..LATIN SMALL LETTER TURNED H WITH FISHHOOK AND TAIL
    return "Latin";
  }
  if ((x >= 0x02B0) && (x <= 0x02B8)) { // Lm   [9] MODIFIER LETTER SMALL H..MODIFIER LETTER SMALL Y
    return "Latin";
  }
  if ((x >= 0x02E0) && (x <= 0x02E4)) { // Lm   [5] MODIFIER LETTER SMALL GAMMA..MODIFIER LETTER SMALL REVERSED GLOTTAL STOP
    return "Latin";
  }
  if ((x >= 0x1D00) && (x <= 0x1D25)) { // L&  [38] LATIN LETTER SMALL CAPITAL A..LATIN LETTER AIN
    return "Latin";
  }
  if ((x >= 0x1D2C) && (x <= 0x1D5C)) { // Lm  [49] MODIFIER LETTER CAPITAL A..MODIFIER LETTER SMALL AIN
    return "Latin";
  }
  if ((x >= 0x1D62) && (x <= 0x1D65)) { // Lm   [4] LATIN SUBSCRIPT SMALL LETTER I..LATIN SUBSCRIPT SMALL LETTER V
    return "Latin";
  }
  if ((x >= 0x1D6B) && (x <= 0x1D77)) { // L&  [13] LATIN SMALL LETTER UE..LATIN SMALL LETTER TURNED G
    return "Latin";
  }
  if ((x >= 0x1D79) && (x <= 0x1D9A)) { // L&  [34] LATIN SMALL LETTER INSULAR G..LATIN SMALL LETTER EZH WITH RETROFLEX HOOK
    return "Latin";
  }
  if ((x >= 0x1D9B) && (x <= 0x1DBE)) { // Lm  [36] MODIFIER LETTER SMALL TURNED ALPHA..MODIFIER LETTER SMALL EZH
    return "Latin";
  }
  if ((x >= 0x1E00) && (x <= 0x1EFF)) { // L& [256] LATIN CAPITAL LETTER A WITH RING BELOW..LATIN SMALL LETTER Y WITH LOOP
    return "Latin";
  }
  if (x == 0x2071 ) {//  Lm       SUPERSCRIPT LATIN SMALL LETTER I
    return "Latin";
  }
  if (x == 0x207F ) {//  Lm       SUPERSCRIPT LATIN SMALL LETTER N
    return "Latin";
  }
  if ((x >= 0x2090) && (x <= 0x209C)) { // Lm  [13] LATIN SUBSCRIPT SMALL LETTER A..LATIN SUBSCRIPT SMALL LETTER T
    return "Latin";
  }
  if ((x >= 0x212A) && (x <= 0x212B)) { // L&   [2] KELVIN SIGN..ANGSTROM SIGN
    return "Latin";
  }
  if (x == 0x2132 ) {//  L&       TURNED CAPITAL F
    return "Latin";
  }
  if (x == 0x214E ) {//  L&       TURNED SMALL F
    return "Latin";
  }
  if ((x >= 0x2160) && (x <= 0x2182)) { // Nl  [35] ROMAN NUMERAL ONE..ROMAN NUMERAL TEN THOUSAND
    return "Latin";
  }
  if ((x >= 0x2183) && (x <= 0x2184)) { // L&   [2] ROMAN NUMERAL REVERSED ONE HUNDRED..LATIN SMALL LETTER REVERSED C
    return "Latin";
  }
  if ((x >= 0x2185) && (x <= 0x2188)) { // Nl   [4] ROMAN NUMERAL SIX LATE FORM..ROMAN NUMERAL ONE HUNDRED THOUSAND
    return "Latin";
  }
  if ((x >= 0x2C60) && (x <= 0x2C7B)) { // L&  [28] LATIN CAPITAL LETTER L WITH DOUBLE BAR..LATIN LETTER SMALL CAPITAL TURNED E
    return "Latin";
  }
  if ((x >= 0x2C7C) && (x <= 0x2C7D)) { // Lm   [2] LATIN SUBSCRIPT SMALL LETTER J..MODIFIER LETTER CAPITAL V
    return "Latin";
  }
  if ((x >= 0x2C7E) && (x <= 0x2C7F)) { // L&   [2] LATIN CAPITAL LETTER S WITH SWASH TAIL..LATIN CAPITAL LETTER Z WITH SWASH TAIL
    return "Latin";
  }
  if ((x >= 0xA722) && (x <= 0xA76F)) { // L&  [78] LATIN CAPITAL LETTER EGYPTOLOGICAL ALEF..LATIN SMALL LETTER CON
    return "Latin";
  }
  if (x == 0xA770 ) {//  Lm       MODIFIER LETTER US
    return "Latin";
  }
  if ((x >= 0xA771) && (x <= 0xA787)) { // L&  [23] LATIN SMALL LETTER DUM..LATIN SMALL LETTER INSULAR T
    return "Latin";
  }
  if ((x >= 0xA78B) && (x <= 0xA78E)) { // L&   [4] LATIN CAPITAL LETTER SALTILLO..LATIN SMALL LETTER L WITH RETROFLEX HOOK AND BELT
    return "Latin";
  }
  if ((x >= 0xA790) && (x <= 0xA793)) { // L&   [4] LATIN CAPITAL LETTER N WITH DESCENDER..LATIN SMALL LETTER C WITH BAR
    return "Latin";
  }
  if ((x >= 0xA7A0) && (x <= 0xA7AA)) { // L&  [11] LATIN CAPITAL LETTER G WITH OBLIQUE STROKE..LATIN CAPITAL LETTER H WITH HOOK
    return "Latin";
  }
  if ((x >= 0xA7F8) && (x <= 0xA7F9)) { // Lm   [2] MODIFIER LETTER CAPITAL H WITH STROKE..MODIFIER LETTER SMALL LIGATURE OE
    return "Latin";
  }
  if (x == 0xA7FA ) {//  L&       LATIN LETTER SMALL CAPITAL TURNED M
    return "Latin";
  }
  if ((x >= 0xA7FB) && (x <= 0xA7FF)) { // Lo   [5] LATIN EPIGRAPHIC LETTER REVERSED F..LATIN EPIGRAPHIC LETTER ARCHAIC M
    return "Latin";
  }
  if ((x >= 0xFB00) && (x <= 0xFB06)) { // L&   [7] LATIN SMALL LIGATURE FF..LATIN SMALL LIGATURE ST
    return "Latin";
  }
  if ((x >= 0xFF21) && (x <= 0xFF3A)) { // L&  [26] FULLWIDTH LATIN CAPITAL LETTER A..FULLWIDTH LATIN CAPITAL LETTER Z
    return "Latin";
  }
  if ((x >= 0xFF41) && (x <= 0xFF5A)) { // L&  [26] FULLWIDTH LATIN SMALL LETTER A..FULLWIDTH LATIN SMALL LETTER Z
    return "Latin";
  }
  // Total code points: 1272
  // ================================================
  if ((x >= 0x0370) && (x <= 0x0373)) { // L&   [4] GREEK CAPITAL LETTER HETA..GREEK SMALL LETTER ARCHAIC SAMPI
    return "Greek";
  }
  if (x == 0x0375 ) {//  Sk       GREEK LOWER NUMERAL SIGN
    return "Greek";
  }
  if ((x >= 0x0376) && (x <= 0x0377)) { // L&   [2] GREEK CAPITAL LETTER PAMPHYLIAN DIGAMMA..GREEK SMALL LETTER PAMPHYLIAN DIGAMMA
    return "Greek";
  }
  if (x == 0x037A ) {//  Lm       GREEK YPOGEGRAMMENI
    return "Greek";
  }
  if ((x >= 0x037B) && (x <= 0x037D)) { // L&   [3] GREEK SMALL REVERSED LUNATE SIGMA SYMBOL..GREEK SMALL REVERSED DOTTED LUNATE SIGMA SYMBOL
    return "Greek";
  }
  if (x == 0x0384 ) {//  Sk       GREEK TONOS
    return "Greek";
  }
  if (x == 0x0386 ) {//  L&       GREEK CAPITAL LETTER ALPHA WITH TONOS
    return "Greek";
  }
  if ((x >= 0x0388) && (x <= 0x038A)) { // L&   [3] GREEK CAPITAL LETTER EPSILON WITH TONOS..GREEK CAPITAL LETTER IOTA WITH TONOS
    return "Greek";
  }
  if (x == 0x038C ) {//  L&       GREEK CAPITAL LETTER OMICRON WITH TONOS
    return "Greek";
  }
  if ((x >= 0x038E) && (x <= 0x03A1)) { // L&  [20] GREEK CAPITAL LETTER UPSILON WITH TONOS..GREEK CAPITAL LETTER RHO
    return "Greek";
  }
  if ((x >= 0x03A3) && (x <= 0x03E1)) { // L&  [63] GREEK CAPITAL LETTER SIGMA..GREEK SMALL LETTER SAMPI
    return "Greek";
  }
  if ((x >= 0x03F0) && (x <= 0x03F5)) { // L&   [6] GREEK KAPPA SYMBOL..GREEK LUNATE EPSILON SYMBOL
    return "Greek";
  }
  if (x == 0x03F6 ) {//  Sm       GREEK REVERSED LUNATE EPSILON SYMBOL
    return "Greek";
  }
  if ((x >= 0x03F7) && (x <= 0x03FF)) { // L&   [9] GREEK CAPITAL LETTER SHO..GREEK CAPITAL REVERSED DOTTED LUNATE SIGMA SYMBOL
    return "Greek";
  }
  if ((x >= 0x1D26) && (x <= 0x1D2A)) { // L&   [5] GREEK LETTER SMALL CAPITAL GAMMA..GREEK LETTER SMALL CAPITAL PSI
    return "Greek";
  }
  if ((x >= 0x1D5D) && (x <= 0x1D61)) { // Lm   [5] MODIFIER LETTER SMALL BETA..MODIFIER LETTER SMALL CHI
    return "Greek";
  }
  if ((x >= 0x1D66) && (x <= 0x1D6A)) { // Lm   [5] GREEK SUBSCRIPT SMALL LETTER BETA..GREEK SUBSCRIPT SMALL LETTER CHI
    return "Greek";
  }
  if (x == 0x1DBF ) {//  Lm       MODIFIER LETTER SMALL THETA
    return "Greek";
  }
  if ((x >= 0x1F00) && (x <= 0x1F15)) { // L&  [22] GREEK SMALL LETTER ALPHA WITH PSILI..GREEK SMALL LETTER EPSILON WITH DASIA AND OXIA
    return "Greek";
  }
  if ((x >= 0x1F18) && (x <= 0x1F1D)) { // L&   [6] GREEK CAPITAL LETTER EPSILON WITH PSILI..GREEK CAPITAL LETTER EPSILON WITH DASIA AND OXIA
    return "Greek";
  }
  if ((x >= 0x1F20) && (x <= 0x1F45)) { // L&  [38] GREEK SMALL LETTER ETA WITH PSILI..GREEK SMALL LETTER OMICRON WITH DASIA AND OXIA
    return "Greek";
  }
  if ((x >= 0x1F48) && (x <= 0x1F4D)) { // L&   [6] GREEK CAPITAL LETTER OMICRON WITH PSILI..GREEK CAPITAL LETTER OMICRON WITH DASIA AND OXIA
    return "Greek";
  }
  if ((x >= 0x1F50) && (x <= 0x1F57)) { // L&   [8] GREEK SMALL LETTER UPSILON WITH PSILI..GREEK SMALL LETTER UPSILON WITH DASIA AND PERISPOMENI
    return "Greek";
  }
  if (x == 0x1F59 ) {//  L&       GREEK CAPITAL LETTER UPSILON WITH DASIA
    return "Greek";
  }
  if (x == 0x1F5B ) {//  L&       GREEK CAPITAL LETTER UPSILON WITH DASIA AND VARIA
    return "Greek";
  }
  if (x == 0x1F5D ) {//  L&       GREEK CAPITAL LETTER UPSILON WITH DASIA AND OXIA
    return "Greek";
  }
  if ((x >= 0x1F5F) && (x <= 0x1F7D)) { // L&  [31] GREEK CAPITAL LETTER UPSILON WITH DASIA AND PERISPOMENI..GREEK SMALL LETTER OMEGA WITH OXIA
    return "Greek";
  }
  if ((x >= 0x1F80) && (x <= 0x1FB4)) { // L&  [53] GREEK SMALL LETTER ALPHA WITH PSILI AND YPOGEGRAMMENI..GREEK SMALL LETTER ALPHA WITH OXIA AND YPOGEGRAMMENI
    return "Greek";
  }
  if ((x >= 0x1FB6) && (x <= 0x1FBC)) { // L&   [7] GREEK SMALL LETTER ALPHA WITH PERISPOMENI..GREEK CAPITAL LETTER ALPHA WITH PROSGEGRAMMENI
    return "Greek";
  }
  if (x == 0x1FBD ) {//  Sk       GREEK KORONIS
    return "Greek";
  }
  if (x == 0x1FBE ) {//  L&       GREEK PROSGEGRAMMENI
    return "Greek";
  }
  if ((x >= 0x1FBF) && (x <= 0x1FC1)) { // Sk   [3] GREEK PSILI..GREEK DIALYTIKA AND PERISPOMENI
    return "Greek";
  }
  if ((x >= 0x1FC2) && (x <= 0x1FC4)) { // L&   [3] GREEK SMALL LETTER ETA WITH VARIA AND YPOGEGRAMMENI..GREEK SMALL LETTER ETA WITH OXIA AND YPOGEGRAMMENI
    return "Greek";
  }
  if ((x >= 0x1FC6) && (x <= 0x1FCC)) { // L&   [7] GREEK SMALL LETTER ETA WITH PERISPOMENI..GREEK CAPITAL LETTER ETA WITH PROSGEGRAMMENI
    return "Greek";
  }
  if ((x >= 0x1FCD) && (x <= 0x1FCF)) { // Sk   [3] GREEK PSILI AND VARIA..GREEK PSILI AND PERISPOMENI
    return "Greek";
  }
  if ((x >= 0x1FD0) && (x <= 0x1FD3)) { // L&   [4] GREEK SMALL LETTER IOTA WITH VRACHY..GREEK SMALL LETTER IOTA WITH DIALYTIKA AND OXIA
    return "Greek";
  }
  if ((x >= 0x1FD6) && (x <= 0x1FDB)) { // L&   [6] GREEK SMALL LETTER IOTA WITH PERISPOMENI..GREEK CAPITAL LETTER IOTA WITH OXIA
    return "Greek";
  }
  if ((x >= 0x1FDD) && (x <= 0x1FDF)) { // Sk   [3] GREEK DASIA AND VARIA..GREEK DASIA AND PERISPOMENI
    return "Greek";
  }
  if ((x >= 0x1FE0) && (x <= 0x1FEC)) { // L&  [13] GREEK SMALL LETTER UPSILON WITH VRACHY..GREEK CAPITAL LETTER RHO WITH DASIA
    return "Greek";
  }
  if ((x >= 0x1FED) && (x <= 0x1FEF)) { // Sk   [3] GREEK DIALYTIKA AND VARIA..GREEK VARIA
    return "Greek";
  }
  if ((x >= 0x1FF2) && (x <= 0x1FF4)) { // L&   [3] GREEK SMALL LETTER OMEGA WITH VARIA AND YPOGEGRAMMENI..GREEK SMALL LETTER OMEGA WITH OXIA AND YPOGEGRAMMENI
    return "Greek";
  }
  if ((x >= 0x1FF6) && (x <= 0x1FFC)) { // L&   [7] GREEK SMALL LETTER OMEGA WITH PERISPOMENI..GREEK CAPITAL LETTER OMEGA WITH PROSGEGRAMMENI
    return "Greek";
  }
  if ((x >= 0x1FFD) && (x <= 0x1FFE)) { // Sk   [2] GREEK OXIA..GREEK DASIA
    return "Greek";
  }
  if (x == 0x2126 ) {//  L&       OHM SIGN
    return "Greek";
  }
  if ((x >= 0x10140) && (x <= 0x10174)) { // Nl  [53] GREEK ACROPHONIC ATTIC ONE QUARTER..GREEK ACROPHONIC STRATIAN FIFTY MNAS
    return "Greek";
  }
  if ((x >= 0x10175) && (x <= 0x10178)) { // No   [4] GREEK ONE HALF SIGN..GREEK THREE QUARTERS SIGN
    return "Greek";
  }
  if ((x >= 0x10179) && (x <= 0x10189)) { // So  [17] GREEK YEAR SIGN..GREEK TRYBLION BASE SIGN
    return "Greek";
  }
  if (x == 0x1018A ) {//  No       GREEK ZERO SIGN
    return "Greek";
  }
  if ((x >= 0x1D200) && (x <= 0x1D241)) { // So  [66] GREEK VOCAL NOTATION SYMBOL-1..GREEK INSTRUMENTAL NOTATION SYMBOL-54
    return "Greek";
  }
  if ((x >= 0x1D242) && (x <= 0x1D244)) { // Mn   [3] COMBINING GREEK MUSICAL TRISEME..COMBINING GREEK MUSICAL PENTASEME
    return "Greek";
  }
  if (x == 0x1D245 ) {//  So       GREEK MUSICAL LEIMMA
    return "Greek";
  }
  // Total code points: 511
  // ================================================
  if ((x >= 0x0400) && (x <= 0x0481)) { // L& [130] CYRILLIC CAPITAL LETTER IE WITH GRAVE..CYRILLIC SMALL LETTER KOPPA
    return "Cyrillic";
  }
  if (x == 0x0482 ) {//  So       CYRILLIC THOUSANDS SIGN
    return "Cyrillic";
  }
  if ((x >= 0x0483) && (x <= 0x0484)) { // Mn   [2] COMBINING CYRILLIC TITLO..COMBINING CYRILLIC PALATALIZATION
    return "Cyrillic";
  }
  if (x == 0x0487 ) {//  Mn       COMBINING CYRILLIC POKRYTIE
    return "Cyrillic";
  }
  if ((x >= 0x0488) && (x <= 0x0489)) { // Me   [2] COMBINING CYRILLIC HUNDRED THOUSANDS SIGN..COMBINING CYRILLIC MILLIONS SIGN
    return "Cyrillic";
  }
  if ((x >= 0x048A) && (x <= 0x0527)) { // L& [158] CYRILLIC CAPITAL LETTER SHORT I WITH TAIL..CYRILLIC SMALL LETTER SHHA WITH DESCENDER
    return "Cyrillic";
  }
  if (x == 0x1D2B ) {//  L&       CYRILLIC LETTER SMALL CAPITAL EL
    return "Cyrillic";
  }
  if (x == 0x1D78 ) {//  Lm       MODIFIER LETTER CYRILLIC EN
    return "Cyrillic";
  }
  if ((x >= 0x2DE0) && (x <= 0x2DFF)) { // Mn  [32] COMBINING CYRILLIC LETTER BE..COMBINING CYRILLIC LETTER IOTIFIED BIG YUS
    return "Cyrillic";
  }
  if ((x >= 0xA640) && (x <= 0xA66D)) { // L&  [46] CYRILLIC CAPITAL LETTER ZEMLYA..CYRILLIC SMALL LETTER DOUBLE MONOCULAR O
    return "Cyrillic";
  }
  if (x == 0xA66E ) {//  Lo       CYRILLIC LETTER MULTIOCULAR O
    return "Cyrillic";
  }
  if (x == 0xA66F ) {//  Mn       COMBINING CYRILLIC VZMET
    return "Cyrillic";
  }
  if ((x >= 0xA670) && (x <= 0xA672)) { // Me   [3] COMBINING CYRILLIC TEN MILLIONS SIGN..COMBINING CYRILLIC THOUSAND MILLIONS SIGN
    return "Cyrillic";
  }
  if (x == 0xA673 ) {//  Po       SLAVONIC ASTERISK
    return "Cyrillic";
  }
  if ((x >= 0xA674) && (x <= 0xA67D)) { // Mn  [10] COMBINING CYRILLIC LETTER UKRAINIAN IE..COMBINING CYRILLIC PAYEROK
    return "Cyrillic";
  }
  if (x == 0xA67E ) {//  Po       CYRILLIC KAVYKA
    return "Cyrillic";
  }
  if (x == 0xA67F ) {//  Lm       CYRILLIC PAYEROK
    return "Cyrillic";
  }
  if ((x >= 0xA680) && (x <= 0xA697)) { // L&  [24] CYRILLIC CAPITAL LETTER DWE..CYRILLIC SMALL LETTER SHWE
    return "Cyrillic";
  }
  if (x == 0xA69F ) {//  Mn       COMBINING CYRILLIC LETTER IOTIFIED E
    return "Cyrillic";
  }
  // Total code points: 417
  // ================================================
  if ((x >= 0x0531) && (x <= 0x0556)) { // L&  [38] ARMENIAN CAPITAL LETTER AYB..ARMENIAN CAPITAL LETTER FEH
    return "Armenian";
  }
  if (x == 0x0559 ) {//  Lm       ARMENIAN MODIFIER LETTER LEFT HALF RING
    return "Armenian";
  }
  if ((x >= 0x055A) && (x <= 0x055F)) { // Po   [6] ARMENIAN APOSTROPHE..ARMENIAN ABBREVIATION MARK
    return "Armenian";
  }
  if ((x >= 0x0561) && (x <= 0x0587)) { // L&  [39] ARMENIAN SMALL LETTER AYB..ARMENIAN SMALL LIGATURE ECH YIWN
    return "Armenian";
  }
  if (x == 0x058A ) {//  Pd       ARMENIAN HYPHEN
    return "Armenian";
  }
  if (x == 0x058F ) {//  Sc       ARMENIAN DRAM SIGN
    return "Armenian";
  }
  if ((x >= 0xFB13) && (x <= 0xFB17)) { // L&   [5] ARMENIAN SMALL LIGATURE MEN NOW..ARMENIAN SMALL LIGATURE MEN XEH
    return "Armenian";
  }
  // Total code points: 91
  // ================================================
  if ((x >= 0x0591) && (x <= 0x05BD)) { // Mn  [45] HEBREW ACCENT ETNAHTA..HEBREW POINT METEG
    return "Hebrew";
  }
  if (x == 0x05BE ) {//  Pd       HEBREW PUNCTUATION MAQAF
    return "Hebrew";
  }
  if (x == 0x05BF ) {//  Mn       HEBREW POINT RAFE
    return "Hebrew";
  }
  if (x == 0x05C0 ) {//  Po       HEBREW PUNCTUATION PASEQ
    return "Hebrew";
  }
  if ((x >= 0x05C1) && (x <= 0x05C2)) { // Mn   [2] HEBREW POINT SHIN DOT..HEBREW POINT SIN DOT
    return "Hebrew";
  }
  if (x == 0x05C3 ) {//  Po       HEBREW PUNCTUATION SOF PASUQ
    return "Hebrew";
  }
  if ((x >= 0x05C4) && (x <= 0x05C5)) { // Mn   [2] HEBREW MARK UPPER DOT..HEBREW MARK LOWER DOT
    return "Hebrew";
  }
  if (x == 0x05C6 ) {//  Po       HEBREW PUNCTUATION NUN HAFUKHA
    return "Hebrew";
  }
  if (x == 0x05C7 ) {//  Mn       HEBREW POINT QAMATS QATAN
    return "Hebrew";
  }
  if ((x >= 0x05D0) && (x <= 0x05EA)) { // Lo  [27] HEBREW LETTER ALEF..HEBREW LETTER TAV
    return "Hebrew";
  }
  if ((x >= 0x05F0) && (x <= 0x05F2)) { // Lo   [3] HEBREW LIGATURE YIDDISH DOUBLE VAV..HEBREW LIGATURE YIDDISH DOUBLE YOD
    return "Hebrew";
  }
  if ((x >= 0x05F3) && (x <= 0x05F4)) { // Po   [2] HEBREW PUNCTUATION GERESH..HEBREW PUNCTUATION GERSHAYIM
    return "Hebrew";
  }
  if (x == 0xFB1D ) {//  Lo       HEBREW LETTER YOD WITH HIRIQ
    return "Hebrew";
  }
  if (x == 0xFB1E ) {//  Mn       HEBREW POINT JUDEO-SPANISH VARIKA
    return "Hebrew";
  }
  if ((x >= 0xFB1F) && (x <= 0xFB28)) { // Lo  [10] HEBREW LIGATURE YIDDISH YOD YOD PATAH..HEBREW LETTER WIDE TAV
    return "Hebrew";
  }
  if (x == 0xFB29 ) {//  Sm       HEBREW LETTER ALTERNATIVE PLUS SIGN
    return "Hebrew";
  }
  if ((x >= 0xFB2A) && (x <= 0xFB36)) { // Lo  [13] HEBREW LETTER SHIN WITH SHIN DOT..HEBREW LETTER ZAYIN WITH DAGESH
    return "Hebrew";
  }
  if ((x >= 0xFB38) && (x <= 0xFB3C)) { // Lo   [5] HEBREW LETTER TET WITH DAGESH..HEBREW LETTER LAMED WITH DAGESH
    return "Hebrew";
  }
  if (x == 0xFB3E ) {//  Lo       HEBREW LETTER MEM WITH DAGESH
    return "Hebrew";
  }
  if ((x >= 0xFB40) && (x <= 0xFB41)) { // Lo   [2] HEBREW LETTER NUN WITH DAGESH..HEBREW LETTER SAMEKH WITH DAGESH
    return "Hebrew";
  }
  if ((x >= 0xFB43) && (x <= 0xFB44)) { // Lo   [2] HEBREW LETTER FINAL PE WITH DAGESH..HEBREW LETTER PE WITH DAGESH
    return "Hebrew";
  }
  if ((x >= 0xFB46) && (x <= 0xFB4F)) { // Lo  [10] HEBREW LETTER TSADI WITH DAGESH..HEBREW LIGATURE ALEF LAMED
    return "Hebrew";
  }
  // Total code points: 133
  // ================================================
  if ((x >= 0x0600) && (x <= 0x0604)) { // Cf   [5] ARABIC NUMBER SIGN..ARABIC SIGN SAMVAT
    return "Arabic";
  }
  if ((x >= 0x0606) && (x <= 0x0608)) { // Sm   [3] ARABIC-INDIC CUBE ROOT..ARABIC RAY
    return "Arabic";
  }
  if ((x >= 0x0609) && (x <= 0x060A)) { // Po   [2] ARABIC-INDIC PER MILLE SIGN..ARABIC-INDIC PER TEN THOUSAND SIGN
    return "Arabic";
  }
  if (x == 0x060B ) {//  Sc       AFGHANI SIGN
    return "Arabic";
  }
  if (x == 0x060D ) {//  Po       ARABIC DATE SEPARATOR
    return "Arabic";
  }
  if ((x >= 0x060E) && (x <= 0x060F)) { // So   [2] ARABIC POETIC VERSE SIGN..ARABIC SIGN MISRA
    return "Arabic";
  }
  if ((x >= 0x0610) && (x <= 0x061A)) { // Mn  [11] ARABIC SIGN SALLALLAHOU ALAYHE WASSALLAM..ARABIC SMALL KASRA
    return "Arabic";
  }
  if (x == 0x061E ) {//  Po       ARABIC TRIPLE DOT PUNCTUATION MARK
    return "Arabic";
  }
  if ((x >= 0x0620) && (x <= 0x063F)) { // Lo  [32] ARABIC LETTER KASHMIRI YEH..ARABIC LETTER FARSI YEH WITH THREE DOTS ABOVE
    return "Arabic";
  }
  if ((x >= 0x0641) && (x <= 0x064A)) { // Lo  [10] ARABIC LETTER FEH..ARABIC LETTER YEH
    return "Arabic";
  }
  if ((x >= 0x0656) && (x <= 0x065F)) { // Mn  [10] ARABIC SUBSCRIPT ALEF..ARABIC WAVY HAMZA BELOW
    return "Arabic";
  }
  if ((x >= 0x066A) && (x <= 0x066D)) { // Po   [4] ARABIC PERCENT SIGN..ARABIC FIVE POINTED STAR
    return "Arabic";
  }
  if ((x >= 0x066E) && (x <= 0x066F)) { // Lo   [2] ARABIC LETTER DOTLESS BEH..ARABIC LETTER DOTLESS QAF
    return "Arabic";
  }
  if ((x >= 0x0671) && (x <= 0x06D3)) { // Lo  [99] ARABIC LETTER ALEF WASLA..ARABIC LETTER YEH BARREE WITH HAMZA ABOVE
    return "Arabic";
  }
  if (x == 0x06D4 ) {//  Po       ARABIC FULL STOP
    return "Arabic";
  }
  if (x == 0x06D5 ) {//  Lo       ARABIC LETTER AE
    return "Arabic";
  }
  if ((x >= 0x06D6) && (x <= 0x06DC)) { // Mn   [7] ARABIC SMALL HIGH LIGATURE SAD WITH LAM WITH ALEF MAKSURA..ARABIC SMALL HIGH SEEN
    return "Arabic";
  }
  if (x == 0x06DE ) {//  So       ARABIC START OF RUB EL HIZB
    return "Arabic";
  }
  if ((x >= 0x06DF) && (x <= 0x06E4)) { // Mn   [6] ARABIC SMALL HIGH ROUNDED ZERO..ARABIC SMALL HIGH MADDA
    return "Arabic";
  }
  if ((x >= 0x06E5) && (x <= 0x06E6)) { // Lm   [2] ARABIC SMALL WAW..ARABIC SMALL YEH
    return "Arabic";
  }
  if ((x >= 0x06E7) && (x <= 0x06E8)) { // Mn   [2] ARABIC SMALL HIGH YEH..ARABIC SMALL HIGH NOON
    return "Arabic";
  }
  if (x == 0x06E9 ) {//  So       ARABIC PLACE OF SAJDAH
    return "Arabic";
  }
  if ((x >= 0x06EA) && (x <= 0x06ED)) { // Mn   [4] ARABIC EMPTY CENTRE LOW STOP..ARABIC SMALL LOW MEEM
    return "Arabic";
  }
  if ((x >= 0x06EE) && (x <= 0x06EF)) { // Lo   [2] ARABIC LETTER DAL WITH INVERTED V..ARABIC LETTER REH WITH INVERTED V
    return "Arabic";
  }
  if ((x >= 0x06F0) && (x <= 0x06F9)) { // Nd  [10] EXTENDED ARABIC-INDIC DIGIT ZERO..EXTENDED ARABIC-INDIC DIGIT NINE
    return "Arabic";
  }
  if ((x >= 0x06FA) && (x <= 0x06FC)) { // Lo   [3] ARABIC LETTER SHEEN WITH DOT BELOW..ARABIC LETTER GHAIN WITH DOT BELOW
    return "Arabic";
  }
  if ((x >= 0x06FD) && (x <= 0x06FE)) { // So   [2] ARABIC SIGN SINDHI AMPERSAND..ARABIC SIGN SINDHI POSTPOSITION MEN
    return "Arabic";
  }
  if (x == 0x06FF ) {//  Lo       ARABIC LETTER HEH WITH INVERTED V
    return "Arabic";
  }
  if ((x >= 0x0750) && (x <= 0x077F)) { // Lo  [48] ARABIC LETTER BEH WITH THREE DOTS HORIZONTALLY BELOW..ARABIC LETTER KAF WITH TWO DOTS ABOVE
    return "Arabic";
  }
  if (x == 0x08A0 ) {//  Lo       ARABIC LETTER BEH WITH SMALL V BELOW
    return "Arabic";
  }
  if ((x >= 0x08A2) && (x <= 0x08AC)) { // Lo  [11] ARABIC LETTER JEEM WITH TWO DOTS ABOVE..ARABIC LETTER ROHINGYA YEH
    return "Arabic";
  }
  if ((x >= 0x08E4) && (x <= 0x08FE)) { // Mn  [27] ARABIC CURLY FATHA..ARABIC DAMMA WITH DOT
    return "Arabic";
  }
  if ((x >= 0xFB50) && (x <= 0xFBB1)) { // Lo  [98] ARABIC LETTER ALEF WASLA ISOLATED FORM..ARABIC LETTER YEH BARREE WITH HAMZA ABOVE FINAL FORM
    return "Arabic";
  }
  if ((x >= 0xFBB2) && (x <= 0xFBC1)) { // Sk  [16] ARABIC SYMBOL DOT ABOVE..ARABIC SYMBOL SMALL TAH BELOW
    return "Arabic";
  }
  if ((x >= 0xFBD3) && (x <= 0xFD3D)) { // Lo [363] ARABIC LETTER NG ISOLATED FORM..ARABIC LIGATURE ALEF WITH FATHATAN ISOLATED FORM
    return "Arabic";
  }
  if ((x >= 0xFD50) && (x <= 0xFD8F)) { // Lo  [64] ARABIC LIGATURE TEH WITH JEEM WITH MEEM INITIAL FORM..ARABIC LIGATURE MEEM WITH KHAH WITH MEEM INITIAL FORM
    return "Arabic";
  }
  if ((x >= 0xFD92) && (x <= 0xFDC7)) { // Lo  [54] ARABIC LIGATURE MEEM WITH JEEM WITH KHAH INITIAL FORM..ARABIC LIGATURE NOON WITH JEEM WITH YEH FINAL FORM
    return "Arabic";
  }
  if ((x >= 0xFDF0) && (x <= 0xFDFB)) { // Lo  [12] ARABIC LIGATURE SALLA USED AS KORANIC STOP SIGN ISOLATED FORM..ARABIC LIGATURE JALLAJALALOUHOU
    return "Arabic";
  }
  if (x == 0xFDFC ) {//  Sc       RIAL SIGN
    return "Arabic";
  }
  if ((x >= 0xFE70) && (x <= 0xFE74)) { // Lo   [5] ARABIC FATHATAN ISOLATED FORM..ARABIC KASRATAN ISOLATED FORM
    return "Arabic";
  }
  if ((x >= 0xFE76) && (x <= 0xFEFC)) { // Lo [135] ARABIC FATHA ISOLATED FORM..ARABIC LIGATURE LAM WITH ALEF FINAL FORM
    return "Arabic";
  }
  if ((x >= 0x10E60) && (x <= 0x10E7E)) { // No  [31] RUMI DIGIT ONE..RUMI FRACTION TWO THIRDS
    return "Arabic";
  }
  if ((x >= 0x1EE00) && (x <= 0x1EE03)) { // Lo   [4] ARABIC MATHEMATICAL ALEF..ARABIC MATHEMATICAL DAL
    return "Arabic";
  }
  if ((x >= 0x1EE05) && (x <= 0x1EE1F)) { // Lo  [27] ARABIC MATHEMATICAL WAW..ARABIC MATHEMATICAL DOTLESS QAF
    return "Arabic";
  }
  if ((x >= 0x1EE21) && (x <= 0x1EE22)) { // Lo   [2] ARABIC MATHEMATICAL INITIAL BEH..ARABIC MATHEMATICAL INITIAL JEEM
    return "Arabic";
  }
  if (x == 0x1EE24 ) {//  Lo       ARABIC MATHEMATICAL INITIAL HEH
    return "Arabic";
  }
  if (x == 0x1EE27 ) {//  Lo       ARABIC MATHEMATICAL INITIAL HAH
    return "Arabic";
  }
  if ((x >= 0x1EE29) && (x <= 0x1EE32)) { // Lo  [10] ARABIC MATHEMATICAL INITIAL YEH..ARABIC MATHEMATICAL INITIAL QAF
    return "Arabic";
  }
  if ((x >= 0x1EE34) && (x <= 0x1EE37)) { // Lo   [4] ARABIC MATHEMATICAL INITIAL SHEEN..ARABIC MATHEMATICAL INITIAL KHAH
    return "Arabic";
  }
  if (x == 0x1EE39 ) {//  Lo       ARABIC MATHEMATICAL INITIAL DAD
    return "Arabic";
  }
  if (x == 0x1EE3B ) {//  Lo       ARABIC MATHEMATICAL INITIAL GHAIN
    return "Arabic";
  }
  if (x == 0x1EE42 ) {//  Lo       ARABIC MATHEMATICAL TAILED JEEM
    return "Arabic";
  }
  if (x == 0x1EE47 ) {//  Lo       ARABIC MATHEMATICAL TAILED HAH
    return "Arabic";
  }
  if (x == 0x1EE49 ) {//  Lo       ARABIC MATHEMATICAL TAILED YEH
    return "Arabic";
  }
  if (x == 0x1EE4B ) {//  Lo       ARABIC MATHEMATICAL TAILED LAM
    return "Arabic";
  }
  if ((x >= 0x1EE4D) && (x <= 0x1EE4F)) { // Lo   [3] ARABIC MATHEMATICAL TAILED NOON..ARABIC MATHEMATICAL TAILED AIN
    return "Arabic";
  }
  if ((x >= 0x1EE51) && (x <= 0x1EE52)) { // Lo   [2] ARABIC MATHEMATICAL TAILED SAD..ARABIC MATHEMATICAL TAILED QAF
    return "Arabic";
  }
  if (x == 0x1EE54 ) {//  Lo       ARABIC MATHEMATICAL TAILED SHEEN
    return "Arabic";
  }
  if (x == 0x1EE57 ) {//  Lo       ARABIC MATHEMATICAL TAILED KHAH
    return "Arabic";
  }
  if (x == 0x1EE59 ) {//  Lo       ARABIC MATHEMATICAL TAILED DAD
    return "Arabic";
  }
  if (x == 0x1EE5B ) {//  Lo       ARABIC MATHEMATICAL TAILED GHAIN
    return "Arabic";
  }
  if (x == 0x1EE5D ) {//  Lo       ARABIC MATHEMATICAL TAILED DOTLESS NOON
    return "Arabic";
  }
  if (x == 0x1EE5F ) {//  Lo       ARABIC MATHEMATICAL TAILED DOTLESS QAF
    return "Arabic";
  }
  if ((x >= 0x1EE61) && (x <= 0x1EE62)) { // Lo   [2] ARABIC MATHEMATICAL STRETCHED BEH..ARABIC MATHEMATICAL STRETCHED JEEM
    return "Arabic";
  }
  if (x == 0x1EE64 ) {//  Lo       ARABIC MATHEMATICAL STRETCHED HEH
    return "Arabic";
  }
  if ((x >= 0x1EE67) && (x <= 0x1EE6A)) { // Lo   [4] ARABIC MATHEMATICAL STRETCHED HAH..ARABIC MATHEMATICAL STRETCHED KAF
    return "Arabic";
  }
  if ((x >= 0x1EE6C) && (x <= 0x1EE72)) { // Lo   [7] ARABIC MATHEMATICAL STRETCHED MEEM..ARABIC MATHEMATICAL STRETCHED QAF
    return "Arabic";
  }
  if ((x >= 0x1EE74) && (x <= 0x1EE77)) { // Lo   [4] ARABIC MATHEMATICAL STRETCHED SHEEN..ARABIC MATHEMATICAL STRETCHED KHAH
    return "Arabic";
  }
  if ((x >= 0x1EE79) && (x <= 0x1EE7C)) { // Lo   [4] ARABIC MATHEMATICAL STRETCHED DAD..ARABIC MATHEMATICAL STRETCHED DOTLESS BEH
    return "Arabic";
  }
  if (x == 0x1EE7E ) {//  Lo       ARABIC MATHEMATICAL STRETCHED DOTLESS FEH
    return "Arabic";
  }
  if ((x >= 0x1EE80) && (x <= 0x1EE89)) { // Lo  [10] ARABIC MATHEMATICAL LOOPED ALEF..ARABIC MATHEMATICAL LOOPED YEH
    return "Arabic";
  }
  if ((x >= 0x1EE8B) && (x <= 0x1EE9B)) { // Lo  [17] ARABIC MATHEMATICAL LOOPED LAM..ARABIC MATHEMATICAL LOOPED GHAIN
    return "Arabic";
  }
  if ((x >= 0x1EEA1) && (x <= 0x1EEA3)) { // Lo   [3] ARABIC MATHEMATICAL DOUBLE-STRUCK BEH..ARABIC MATHEMATICAL DOUBLE-STRUCK DAL
    return "Arabic";
  }
  if ((x >= 0x1EEA5) && (x <= 0x1EEA9)) { // Lo   [5] ARABIC MATHEMATICAL DOUBLE-STRUCK WAW..ARABIC MATHEMATICAL DOUBLE-STRUCK YEH
    return "Arabic";
  }
  if ((x >= 0x1EEAB) && (x <= 0x1EEBB)) { // Lo  [17] ARABIC MATHEMATICAL DOUBLE-STRUCK LAM..ARABIC MATHEMATICAL DOUBLE-STRUCK GHAIN
    return "Arabic";
  }
  if ((x >= 0x1EEF0) && (x <= 0x1EEF1)) { // Sm   [2] ARABIC MATHEMATICAL OPERATOR MEEM WITH HAH WITH TATWEEL..ARABIC MATHEMATICAL OPERATOR HAH WITH DAL
    return "Arabic";
  }
  // Total code points: 1235
  // ================================================
  if ((x >= 0x0700) && (x <= 0x070D)) { // Po  [14] SYRIAC END OF PARAGRAPH..SYRIAC HARKLEAN ASTERISCUS
    return "Syriac";
  }
  if (x == 0x070F ) {//  Cf       SYRIAC ABBREVIATION MARK
    return "Syriac";
  }
  if (x == 0x0710 ) {//  Lo       SYRIAC LETTER ALAPH
    return "Syriac";
  }
  if (x == 0x0711 ) {//  Mn       SYRIAC LETTER SUPERSCRIPT ALAPH
    return "Syriac";
  }
  if ((x >= 0x0712) && (x <= 0x072F)) { // Lo  [30] SYRIAC LETTER BETH..SYRIAC LETTER PERSIAN DHALATH
    return "Syriac";
  }
  if ((x >= 0x0730) && (x <= 0x074A)) { // Mn  [27] SYRIAC PTHAHA ABOVE..SYRIAC BARREKH
    return "Syriac";
  }
  if ((x >= 0x074D) && (x <= 0x074F)) { // Lo   [3] SYRIAC LETTER SOGDIAN ZHAIN..SYRIAC LETTER SOGDIAN FE
    return "Syriac";
  }
  // Total code points: 77
  // ================================================
  if ((x >= 0x0780) && (x <= 0x07A5)) { // Lo  [38] THAANA LETTER HAA..THAANA LETTER WAAVU
    return "Thaana";
  }
  if ((x >= 0x07A6) && (x <= 0x07B0)) { // Mn  [11] THAANA ABAFILI..THAANA SUKUN
    return "Thaana";
  }
  if (x == 0x07B1 ) {//  Lo       THAANA LETTER NAA
    return "Thaana";
  }
  // Total code points: 50
  // ================================================
  if ((x >= 0x0900) && (x <= 0x0902)) { // Mn   [3] DEVANAGARI SIGN INVERTED CANDRABINDU..DEVANAGARI SIGN ANUSVARA
    return "Devanagari";
  }
  if (x == 0x0903 ) {//  Mc       DEVANAGARI SIGN VISARGA
    return "Devanagari";
  }
  if ((x >= 0x0904) && (x <= 0x0939)) { // Lo  [54] DEVANAGARI LETTER SHORT A..DEVANAGARI LETTER HA
    return "Devanagari";
  }
  if (x == 0x093A ) {//  Mn       DEVANAGARI VOWEL SIGN OE
    return "Devanagari";
  }
  if (x == 0x093B ) {//  Mc       DEVANAGARI VOWEL SIGN OOE
    return "Devanagari";
  }
  if (x == 0x093C ) {//  Mn       DEVANAGARI SIGN NUKTA
    return "Devanagari";
  }
  if (x == 0x093D ) {//  Lo       DEVANAGARI SIGN AVAGRAHA
    return "Devanagari";
  }
  if ((x >= 0x093E) && (x <= 0x0940)) { // Mc   [3] DEVANAGARI VOWEL SIGN AA..DEVANAGARI VOWEL SIGN II
    return "Devanagari";
  }
  if ((x >= 0x0941) && (x <= 0x0948)) { // Mn   [8] DEVANAGARI VOWEL SIGN U..DEVANAGARI VOWEL SIGN AI
    return "Devanagari";
  }
  if ((x >= 0x0949) && (x <= 0x094C)) { // Mc   [4] DEVANAGARI VOWEL SIGN CANDRA O..DEVANAGARI VOWEL SIGN AU
    return "Devanagari";
  }
  if (x == 0x094D ) {//  Mn       DEVANAGARI SIGN VIRAMA
    return "Devanagari";
  }
  if ((x >= 0x094E) && (x <= 0x094F)) { // Mc   [2] DEVANAGARI VOWEL SIGN PRISHTHAMATRA E..DEVANAGARI VOWEL SIGN AW
    return "Devanagari";
  }
  if (x == 0x0950 ) {//  Lo       DEVANAGARI OM
    return "Devanagari";
  }
  if ((x >= 0x0953) && (x <= 0x0957)) { // Mn   [5] DEVANAGARI GRAVE ACCENT..DEVANAGARI VOWEL SIGN UUE
    return "Devanagari";
  }
  if ((x >= 0x0958) && (x <= 0x0961)) { // Lo  [10] DEVANAGARI LETTER QA..DEVANAGARI LETTER VOCALIC LL
    return "Devanagari";
  }
  if ((x >= 0x0962) && (x <= 0x0963)) { // Mn   [2] DEVANAGARI VOWEL SIGN VOCALIC L..DEVANAGARI VOWEL SIGN VOCALIC LL
    return "Devanagari";
  }
  if ((x >= 0x0966) && (x <= 0x096F)) { // Nd  [10] DEVANAGARI DIGIT ZERO..DEVANAGARI DIGIT NINE
    return "Devanagari";
  }
  if (x == 0x0970 ) {//  Po       DEVANAGARI ABBREVIATION SIGN
    return "Devanagari";
  }
  if (x == 0x0971 ) {//  Lm       DEVANAGARI SIGN HIGH SPACING DOT
    return "Devanagari";
  }
  if ((x >= 0x0972) && (x <= 0x0977)) { // Lo   [6] DEVANAGARI LETTER CANDRA A..DEVANAGARI LETTER UUE
    return "Devanagari";
  }
  if ((x >= 0x0979) && (x <= 0x097F)) { // Lo   [7] DEVANAGARI LETTER ZHA..DEVANAGARI LETTER BBA
    return "Devanagari";
  }
  if ((x >= 0xA8E0) && (x <= 0xA8F1)) { // Mn  [18] COMBINING DEVANAGARI DIGIT ZERO..COMBINING DEVANAGARI SIGN AVAGRAHA
    return "Devanagari";
  }
  if ((x >= 0xA8F2) && (x <= 0xA8F7)) { // Lo   [6] DEVANAGARI SIGN SPACING CANDRABINDU..DEVANAGARI SIGN CANDRABINDU AVAGRAHA
    return "Devanagari";
  }
  if ((x >= 0xA8F8) && (x <= 0xA8FA)) { // Po   [3] DEVANAGARI SIGN PUSHPIKA..DEVANAGARI CARET
    return "Devanagari";
  }
  if (x == 0xA8FB ) {//  Lo       DEVANAGARI HEADSTROKE
    return "Devanagari";
  }
  // Total code points: 151
  // ================================================
  if (x == 0x0981 ) {//  Mn       BENGALI SIGN CANDRABINDU
    return "Bengali";
  }
  if ((x >= 0x0982) && (x <= 0x0983)) { // Mc   [2] BENGALI SIGN ANUSVARA..BENGALI SIGN VISARGA
    return "Bengali";
  }
  if ((x >= 0x0985) && (x <= 0x098C)) { // Lo   [8] BENGALI LETTER A..BENGALI LETTER VOCALIC L
    return "Bengali";
  }
  if ((x >= 0x098F) && (x <= 0x0990)) { // Lo   [2] BENGALI LETTER E..BENGALI LETTER AI
    return "Bengali";
  }
  if ((x >= 0x0993) && (x <= 0x09A8)) { // Lo  [22] BENGALI LETTER O..BENGALI LETTER NA
    return "Bengali";
  }
  if ((x >= 0x09AA) && (x <= 0x09B0)) { // Lo   [7] BENGALI LETTER PA..BENGALI LETTER RA
    return "Bengali";
  }
  if (x == 0x09B2 ) {//  Lo       BENGALI LETTER LA
    return "Bengali";
  }
  if ((x >= 0x09B6) && (x <= 0x09B9)) { // Lo   [4] BENGALI LETTER SHA..BENGALI LETTER HA
    return "Bengali";
  }
  if (x == 0x09BC ) {//  Mn       BENGALI SIGN NUKTA
    return "Bengali";
  }
  if (x == 0x09BD ) {//  Lo       BENGALI SIGN AVAGRAHA
    return "Bengali";
  }
  if ((x >= 0x09BE) && (x <= 0x09C0)) { // Mc   [3] BENGALI VOWEL SIGN AA..BENGALI VOWEL SIGN II
    return "Bengali";
  }
  if ((x >= 0x09C1) && (x <= 0x09C4)) { // Mn   [4] BENGALI VOWEL SIGN U..BENGALI VOWEL SIGN VOCALIC RR
    return "Bengali";
  }
  if ((x >= 0x09C7) && (x <= 0x09C8)) { // Mc   [2] BENGALI VOWEL SIGN E..BENGALI VOWEL SIGN AI
    return "Bengali";
  }
  if ((x >= 0x09CB) && (x <= 0x09CC)) { // Mc   [2] BENGALI VOWEL SIGN O..BENGALI VOWEL SIGN AU
    return "Bengali";
  }
  if (x == 0x09CD ) {//  Mn       BENGALI SIGN VIRAMA
    return "Bengali";
  }
  if (x == 0x09CE ) {//  Lo       BENGALI LETTER KHANDA TA
    return "Bengali";
  }
  if (x == 0x09D7 ) {//  Mc       BENGALI AU LENGTH MARK
    return "Bengali";
  }
  if ((x >= 0x09DC) && (x <= 0x09DD)) { // Lo   [2] BENGALI LETTER RRA..BENGALI LETTER RHA
    return "Bengali";
  }
  if ((x >= 0x09DF) && (x <= 0x09E1)) { // Lo   [3] BENGALI LETTER YYA..BENGALI LETTER VOCALIC LL
    return "Bengali";
  }
  if ((x >= 0x09E2) && (x <= 0x09E3)) { // Mn   [2] BENGALI VOWEL SIGN VOCALIC L..BENGALI VOWEL SIGN VOCALIC LL
    return "Bengali";
  }
  if ((x >= 0x09E6) && (x <= 0x09EF)) { // Nd  [10] BENGALI DIGIT ZERO..BENGALI DIGIT NINE
    return "Bengali";
  }
  if ((x >= 0x09F0) && (x <= 0x09F1)) { // Lo   [2] BENGALI LETTER RA WITH MIDDLE DIAGONAL..BENGALI LETTER RA WITH LOWER DIAGONAL
    return "Bengali";
  }
  if ((x >= 0x09F2) && (x <= 0x09F3)) { // Sc   [2] BENGALI RUPEE MARK..BENGALI RUPEE SIGN
    return "Bengali";
  }
  if ((x >= 0x09F4) && (x <= 0x09F9)) { // No   [6] BENGALI CURRENCY NUMERATOR ONE..BENGALI CURRENCY DENOMINATOR SIXTEEN
    return "Bengali";
  }
  if (x == 0x09FA ) {//  So       BENGALI ISSHAR
    return "Bengali";
  }
  if (x == 0x09FB ) {//  Sc       BENGALI GANDA MARK
    return "Bengali";
  }
  // Total code points: 92
  // ================================================
  if ((x >= 0x0A01) && (x <= 0x0A02)) { // Mn   [2] GURMUKHI SIGN ADAK BINDI..GURMUKHI SIGN BINDI
    return "Gurmukhi";
  }
  if (x == 0x0A03 ) {//  Mc       GURMUKHI SIGN VISARGA
    return "Gurmukhi";
  }
  if ((x >= 0x0A05) && (x <= 0x0A0A)) { // Lo   [6] GURMUKHI LETTER A..GURMUKHI LETTER UU
    return "Gurmukhi";
  }
  if ((x >= 0x0A0F) && (x <= 0x0A10)) { // Lo   [2] GURMUKHI LETTER EE..GURMUKHI LETTER AI
    return "Gurmukhi";
  }
  if ((x >= 0x0A13) && (x <= 0x0A28)) { // Lo  [22] GURMUKHI LETTER OO..GURMUKHI LETTER NA
    return "Gurmukhi";
  }
  if ((x >= 0x0A2A) && (x <= 0x0A30)) { // Lo   [7] GURMUKHI LETTER PA..GURMUKHI LETTER RA
    return "Gurmukhi";
  }
  if ((x >= 0x0A32) && (x <= 0x0A33)) { // Lo   [2] GURMUKHI LETTER LA..GURMUKHI LETTER LLA
    return "Gurmukhi";
  }
  if ((x >= 0x0A35) && (x <= 0x0A36)) { // Lo   [2] GURMUKHI LETTER VA..GURMUKHI LETTER SHA
    return "Gurmukhi";
  }
  if ((x >= 0x0A38) && (x <= 0x0A39)) { // Lo   [2] GURMUKHI LETTER SA..GURMUKHI LETTER HA
    return "Gurmukhi";
  }
  if (x == 0x0A3C ) {//  Mn       GURMUKHI SIGN NUKTA
    return "Gurmukhi";
  }
  if ((x >= 0x0A3E) && (x <= 0x0A40)) { // Mc   [3] GURMUKHI VOWEL SIGN AA..GURMUKHI VOWEL SIGN II
    return "Gurmukhi";
  }
  if ((x >= 0x0A41) && (x <= 0x0A42)) { // Mn   [2] GURMUKHI VOWEL SIGN U..GURMUKHI VOWEL SIGN UU
    return "Gurmukhi";
  }
  if ((x >= 0x0A47) && (x <= 0x0A48)) { // Mn   [2] GURMUKHI VOWEL SIGN EE..GURMUKHI VOWEL SIGN AI
    return "Gurmukhi";
  }
  if ((x >= 0x0A4B) && (x <= 0x0A4D)) { // Mn   [3] GURMUKHI VOWEL SIGN OO..GURMUKHI SIGN VIRAMA
    return "Gurmukhi";
  }
  if (x == 0x0A51 ) {//  Mn       GURMUKHI SIGN UDAAT
    return "Gurmukhi";
  }
  if ((x >= 0x0A59) && (x <= 0x0A5C)) { // Lo   [4] GURMUKHI LETTER KHHA..GURMUKHI LETTER RRA
    return "Gurmukhi";
  }
  if (x == 0x0A5E ) {//  Lo       GURMUKHI LETTER FA
    return "Gurmukhi";
  }
  if ((x >= 0x0A66) && (x <= 0x0A6F)) { // Nd  [10] GURMUKHI DIGIT ZERO..GURMUKHI DIGIT NINE
    return "Gurmukhi";
  }
  if ((x >= 0x0A70) && (x <= 0x0A71)) { // Mn   [2] GURMUKHI TIPPI..GURMUKHI ADDAK
    return "Gurmukhi";
  }
  if ((x >= 0x0A72) && (x <= 0x0A74)) { // Lo   [3] GURMUKHI IRI..GURMUKHI EK ONKAR
    return "Gurmukhi";
  }
  if (x == 0x0A75 ) {//  Mn       GURMUKHI SIGN YAKASH
    return "Gurmukhi";
  }
  // Total code points: 79
  // ================================================
  if ((x >= 0x0A81) && (x <= 0x0A82)) { // Mn   [2] GUJARATI SIGN CANDRABINDU..GUJARATI SIGN ANUSVARA
    return "Gujarati";
  }
  if (x == 0x0A83 ) {//  Mc       GUJARATI SIGN VISARGA
    return "Gujarati";
  }
  if ((x >= 0x0A85) && (x <= 0x0A8D)) { // Lo   [9] GUJARATI LETTER A..GUJARATI VOWEL CANDRA E
    return "Gujarati";
  }
  if ((x >= 0x0A8F) && (x <= 0x0A91)) { // Lo   [3] GUJARATI LETTER E..GUJARATI VOWEL CANDRA O
    return "Gujarati";
  }
  if ((x >= 0x0A93) && (x <= 0x0AA8)) { // Lo  [22] GUJARATI LETTER O..GUJARATI LETTER NA
    return "Gujarati";
  }
  if ((x >= 0x0AAA) && (x <= 0x0AB0)) { // Lo   [7] GUJARATI LETTER PA..GUJARATI LETTER RA
    return "Gujarati";
  }
  if ((x >= 0x0AB2) && (x <= 0x0AB3)) { // Lo   [2] GUJARATI LETTER LA..GUJARATI LETTER LLA
    return "Gujarati";
  }
  if ((x >= 0x0AB5) && (x <= 0x0AB9)) { // Lo   [5] GUJARATI LETTER VA..GUJARATI LETTER HA
    return "Gujarati";
  }
  if (x == 0x0ABC ) {//  Mn       GUJARATI SIGN NUKTA
    return "Gujarati";
  }
  if (x == 0x0ABD ) {//  Lo       GUJARATI SIGN AVAGRAHA
    return "Gujarati";
  }
  if ((x >= 0x0ABE) && (x <= 0x0AC0)) { // Mc   [3] GUJARATI VOWEL SIGN AA..GUJARATI VOWEL SIGN II
    return "Gujarati";
  }
  if ((x >= 0x0AC1) && (x <= 0x0AC5)) { // Mn   [5] GUJARATI VOWEL SIGN U..GUJARATI VOWEL SIGN CANDRA E
    return "Gujarati";
  }
  if ((x >= 0x0AC7) && (x <= 0x0AC8)) { // Mn   [2] GUJARATI VOWEL SIGN E..GUJARATI VOWEL SIGN AI
    return "Gujarati";
  }
  if (x == 0x0AC9 ) {//  Mc       GUJARATI VOWEL SIGN CANDRA O
    return "Gujarati";
  }
  if ((x >= 0x0ACB) && (x <= 0x0ACC)) { // Mc   [2] GUJARATI VOWEL SIGN O..GUJARATI VOWEL SIGN AU
    return "Gujarati";
  }
  if (x == 0x0ACD ) {//  Mn       GUJARATI SIGN VIRAMA
    return "Gujarati";
  }
  if (x == 0x0AD0 ) {//  Lo       GUJARATI OM
    return "Gujarati";
  }
  if ((x >= 0x0AE0) && (x <= 0x0AE1)) { // Lo   [2] GUJARATI LETTER VOCALIC RR..GUJARATI LETTER VOCALIC LL
    return "Gujarati";
  }
  if ((x >= 0x0AE2) && (x <= 0x0AE3)) { // Mn   [2] GUJARATI VOWEL SIGN VOCALIC L..GUJARATI VOWEL SIGN VOCALIC LL
    return "Gujarati";
  }
  if ((x >= 0x0AE6) && (x <= 0x0AEF)) { // Nd  [10] GUJARATI DIGIT ZERO..GUJARATI DIGIT NINE
    return "Gujarati";
  }
  if (x == 0x0AF0 ) {//  Po       GUJARATI ABBREVIATION SIGN
    return "Gujarati";
  }
  if (x == 0x0AF1 ) {//  Sc       GUJARATI RUPEE SIGN
    return "Gujarati";
  }
  // Total code points: 84
  // ================================================
  if (x == 0x0B01 ) {//  Mn       ORIYA SIGN CANDRABINDU
    return "Oriya";
  }
  if ((x >= 0x0B02) && (x <= 0x0B03)) { // Mc   [2] ORIYA SIGN ANUSVARA..ORIYA SIGN VISARGA
    return "Oriya";
  }
  if ((x >= 0x0B05) && (x <= 0x0B0C)) { // Lo   [8] ORIYA LETTER A..ORIYA LETTER VOCALIC L
    return "Oriya";
  }
  if ((x >= 0x0B0F) && (x <= 0x0B10)) { // Lo   [2] ORIYA LETTER E..ORIYA LETTER AI
    return "Oriya";
  }
  if ((x >= 0x0B13) && (x <= 0x0B28)) { // Lo  [22] ORIYA LETTER O..ORIYA LETTER NA
    return "Oriya";
  }
  if ((x >= 0x0B2A) && (x <= 0x0B30)) { // Lo   [7] ORIYA LETTER PA..ORIYA LETTER RA
    return "Oriya";
  }
  if ((x >= 0x0B32) && (x <= 0x0B33)) { // Lo   [2] ORIYA LETTER LA..ORIYA LETTER LLA
    return "Oriya";
  }
  if ((x >= 0x0B35) && (x <= 0x0B39)) { // Lo   [5] ORIYA LETTER VA..ORIYA LETTER HA
    return "Oriya";
  }
  if (x == 0x0B3C ) {//  Mn       ORIYA SIGN NUKTA
    return "Oriya";
  }
  if (x == 0x0B3D ) {//  Lo       ORIYA SIGN AVAGRAHA
    return "Oriya";
  }
  if (x == 0x0B3E ) {//  Mc       ORIYA VOWEL SIGN AA
    return "Oriya";
  }
  if (x == 0x0B3F ) {//  Mn       ORIYA VOWEL SIGN I
    return "Oriya";
  }
  if (x == 0x0B40 ) {//  Mc       ORIYA VOWEL SIGN II
    return "Oriya";
  }
  if ((x >= 0x0B41) && (x <= 0x0B44)) { // Mn   [4] ORIYA VOWEL SIGN U..ORIYA VOWEL SIGN VOCALIC RR
    return "Oriya";
  }
  if ((x >= 0x0B47) && (x <= 0x0B48)) { // Mc   [2] ORIYA VOWEL SIGN E..ORIYA VOWEL SIGN AI
    return "Oriya";
  }
  if ((x >= 0x0B4B) && (x <= 0x0B4C)) { // Mc   [2] ORIYA VOWEL SIGN O..ORIYA VOWEL SIGN AU
    return "Oriya";
  }
  if (x == 0x0B4D ) {//  Mn       ORIYA SIGN VIRAMA
    return "Oriya";
  }
  if (x == 0x0B56 ) {//  Mn       ORIYA AI LENGTH MARK
    return "Oriya";
  }
  if (x == 0x0B57 ) {//  Mc       ORIYA AU LENGTH MARK
    return "Oriya";
  }
  if ((x >= 0x0B5C) && (x <= 0x0B5D)) { // Lo   [2] ORIYA LETTER RRA..ORIYA LETTER RHA
    return "Oriya";
  }
  if ((x >= 0x0B5F) && (x <= 0x0B61)) { // Lo   [3] ORIYA LETTER YYA..ORIYA LETTER VOCALIC LL
    return "Oriya";
  }
  if ((x >= 0x0B62) && (x <= 0x0B63)) { // Mn   [2] ORIYA VOWEL SIGN VOCALIC L..ORIYA VOWEL SIGN VOCALIC LL
    return "Oriya";
  }
  if ((x >= 0x0B66) && (x <= 0x0B6F)) { // Nd  [10] ORIYA DIGIT ZERO..ORIYA DIGIT NINE
    return "Oriya";
  }
  if (x == 0x0B70 ) {//  So       ORIYA ISSHAR
    return "Oriya";
  }
  if (x == 0x0B71 ) {//  Lo       ORIYA LETTER WA
    return "Oriya";
  }
  if ((x >= 0x0B72) && (x <= 0x0B77)) { // No   [6] ORIYA FRACTION ONE QUARTER..ORIYA FRACTION THREE SIXTEENTHS
    return "Oriya";
  }
  // Total code points: 90
  // ================================================
  if (x == 0x0B82 ) {//  Mn       TAMIL SIGN ANUSVARA
    return "Tamil";
  }
  if (x == 0x0B83 ) {//  Lo       TAMIL SIGN VISARGA
    return "Tamil";
  }
  if ((x >= 0x0B85) && (x <= 0x0B8A)) { // Lo   [6] TAMIL LETTER A..TAMIL LETTER UU
    return "Tamil";
  }
  if ((x >= 0x0B8E) && (x <= 0x0B90)) { // Lo   [3] TAMIL LETTER E..TAMIL LETTER AI
    return "Tamil";
  }
  if ((x >= 0x0B92) && (x <= 0x0B95)) { // Lo   [4] TAMIL LETTER O..TAMIL LETTER KA
    return "Tamil";
  }
  if ((x >= 0x0B99) && (x <= 0x0B9A)) { // Lo   [2] TAMIL LETTER NGA..TAMIL LETTER CA
    return "Tamil";
  }
  if (x == 0x0B9C ) {//  Lo       TAMIL LETTER JA
    return "Tamil";
  }
  if ((x >= 0x0B9E) && (x <= 0x0B9F)) { // Lo   [2] TAMIL LETTER NYA..TAMIL LETTER TTA
    return "Tamil";
  }
  if ((x >= 0x0BA3) && (x <= 0x0BA4)) { // Lo   [2] TAMIL LETTER NNA..TAMIL LETTER TA
    return "Tamil";
  }
  if ((x >= 0x0BA8) && (x <= 0x0BAA)) { // Lo   [3] TAMIL LETTER NA..TAMIL LETTER PA
    return "Tamil";
  }
  if ((x >= 0x0BAE) && (x <= 0x0BB9)) { // Lo  [12] TAMIL LETTER MA..TAMIL LETTER HA
    return "Tamil";
  }
  if ((x >= 0x0BBE) && (x <= 0x0BBF)) { // Mc   [2] TAMIL VOWEL SIGN AA..TAMIL VOWEL SIGN I
    return "Tamil";
  }
  if (x == 0x0BC0 ) {//  Mn       TAMIL VOWEL SIGN II
    return "Tamil";
  }
  if ((x >= 0x0BC1) && (x <= 0x0BC2)) { // Mc   [2] TAMIL VOWEL SIGN U..TAMIL VOWEL SIGN UU
    return "Tamil";
  }
  if ((x >= 0x0BC6) && (x <= 0x0BC8)) { // Mc   [3] TAMIL VOWEL SIGN E..TAMIL VOWEL SIGN AI
    return "Tamil";
  }
  if ((x >= 0x0BCA) && (x <= 0x0BCC)) { // Mc   [3] TAMIL VOWEL SIGN O..TAMIL VOWEL SIGN AU
    return "Tamil";
  }
  if (x == 0x0BCD ) {//  Mn       TAMIL SIGN VIRAMA
    return "Tamil";
  }
  if (x == 0x0BD0 ) {//  Lo       TAMIL OM
    return "Tamil";
  }
  if (x == 0x0BD7 ) {//  Mc       TAMIL AU LENGTH MARK
    return "Tamil";
  }
  if ((x >= 0x0BE6) && (x <= 0x0BEF)) { // Nd  [10] TAMIL DIGIT ZERO..TAMIL DIGIT NINE
    return "Tamil";
  }
  if ((x >= 0x0BF0) && (x <= 0x0BF2)) { // No   [3] TAMIL NUMBER TEN..TAMIL NUMBER ONE THOUSAND
    return "Tamil";
  }
  if ((x >= 0x0BF3) && (x <= 0x0BF8)) { // So   [6] TAMIL DAY SIGN..TAMIL AS ABOVE SIGN
    return "Tamil";
  }
  if (x == 0x0BF9 ) {//  Sc       TAMIL RUPEE SIGN
    return "Tamil";
  }
  if (x == 0x0BFA ) {//  So       TAMIL NUMBER SIGN
    return "Tamil";
  }
  // Total code points: 72
  // ================================================
  if ((x >= 0x0C01) && (x <= 0x0C03)) { // Mc   [3] TELUGU SIGN CANDRABINDU..TELUGU SIGN VISARGA
    return "Telugu";
  }
  if ((x >= 0x0C05) && (x <= 0x0C0C)) { // Lo   [8] TELUGU LETTER A..TELUGU LETTER VOCALIC L
    return "Telugu";
  }
  if ((x >= 0x0C0E) && (x <= 0x0C10)) { // Lo   [3] TELUGU LETTER E..TELUGU LETTER AI
    return "Telugu";
  }
  if ((x >= 0x0C12) && (x <= 0x0C28)) { // Lo  [23] TELUGU LETTER O..TELUGU LETTER NA
    return "Telugu";
  }
  if ((x >= 0x0C2A) && (x <= 0x0C33)) { // Lo  [10] TELUGU LETTER PA..TELUGU LETTER LLA
    return "Telugu";
  }
  if ((x >= 0x0C35) && (x <= 0x0C39)) { // Lo   [5] TELUGU LETTER VA..TELUGU LETTER HA
    return "Telugu";
  }
  if (x == 0x0C3D ) {//  Lo       TELUGU SIGN AVAGRAHA
    return "Telugu";
  }
  if ((x >= 0x0C3E) && (x <= 0x0C40)) { // Mn   [3] TELUGU VOWEL SIGN AA..TELUGU VOWEL SIGN II
    return "Telugu";
  }
  if ((x >= 0x0C41) && (x <= 0x0C44)) { // Mc   [4] TELUGU VOWEL SIGN U..TELUGU VOWEL SIGN VOCALIC RR
    return "Telugu";
  }
  if ((x >= 0x0C46) && (x <= 0x0C48)) { // Mn   [3] TELUGU VOWEL SIGN E..TELUGU VOWEL SIGN AI
    return "Telugu";
  }
  if ((x >= 0x0C4A) && (x <= 0x0C4D)) { // Mn   [4] TELUGU VOWEL SIGN O..TELUGU SIGN VIRAMA
    return "Telugu";
  }
  if ((x >= 0x0C55) && (x <= 0x0C56)) { // Mn   [2] TELUGU LENGTH MARK..TELUGU AI LENGTH MARK
    return "Telugu";
  }
  if ((x >= 0x0C58) && (x <= 0x0C59)) { // Lo   [2] TELUGU LETTER TSA..TELUGU LETTER DZA
    return "Telugu";
  }
  if ((x >= 0x0C60) && (x <= 0x0C61)) { // Lo   [2] TELUGU LETTER VOCALIC RR..TELUGU LETTER VOCALIC LL
    return "Telugu";
  }
  if ((x >= 0x0C62) && (x <= 0x0C63)) { // Mn   [2] TELUGU VOWEL SIGN VOCALIC L..TELUGU VOWEL SIGN VOCALIC LL
    return "Telugu";
  }
  if ((x >= 0x0C66) && (x <= 0x0C6F)) { // Nd  [10] TELUGU DIGIT ZERO..TELUGU DIGIT NINE
    return "Telugu";
  }
  if ((x >= 0x0C78) && (x <= 0x0C7E)) { // No   [7] TELUGU FRACTION DIGIT ZERO FOR ODD POWERS OF FOUR..TELUGU FRACTION DIGIT THREE FOR EVEN POWERS OF FOUR
    return "Telugu";
  }
  if (x == 0x0C7F ) {//  So       TELUGU SIGN TUUMU
    return "Telugu";
  }
  // Total code points: 93
  // ================================================
  if ((x >= 0x0C82) && (x <= 0x0C83)) { // Mc   [2] KANNADA SIGN ANUSVARA..KANNADA SIGN VISARGA
    return "Kannada";
  }
  if ((x >= 0x0C85) && (x <= 0x0C8C)) { // Lo   [8] KANNADA LETTER A..KANNADA LETTER VOCALIC L
    return "Kannada";
  }
  if ((x >= 0x0C8E) && (x <= 0x0C90)) { // Lo   [3] KANNADA LETTER E..KANNADA LETTER AI
    return "Kannada";
  }
  if ((x >= 0x0C92) && (x <= 0x0CA8)) { // Lo  [23] KANNADA LETTER O..KANNADA LETTER NA
    return "Kannada";
  }
  if ((x >= 0x0CAA) && (x <= 0x0CB3)) { // Lo  [10] KANNADA LETTER PA..KANNADA LETTER LLA
    return "Kannada";
  }
  if ((x >= 0x0CB5) && (x <= 0x0CB9)) { // Lo   [5] KANNADA LETTER VA..KANNADA LETTER HA
    return "Kannada";
  }
  if (x == 0x0CBC ) {//  Mn       KANNADA SIGN NUKTA
    return "Kannada";
  }
  if (x == 0x0CBD ) {//  Lo       KANNADA SIGN AVAGRAHA
    return "Kannada";
  }
  if (x == 0x0CBE ) {//  Mc       KANNADA VOWEL SIGN AA
    return "Kannada";
  }
  if (x == 0x0CBF ) {//  Mn       KANNADA VOWEL SIGN I
    return "Kannada";
  }
  if ((x >= 0x0CC0) && (x <= 0x0CC4)) { // Mc   [5] KANNADA VOWEL SIGN II..KANNADA VOWEL SIGN VOCALIC RR
    return "Kannada";
  }
  if (x == 0x0CC6 ) {//  Mn       KANNADA VOWEL SIGN E
    return "Kannada";
  }
  if ((x >= 0x0CC7) && (x <= 0x0CC8)) { // Mc   [2] KANNADA VOWEL SIGN EE..KANNADA VOWEL SIGN AI
    return "Kannada";
  }
  if ((x >= 0x0CCA) && (x <= 0x0CCB)) { // Mc   [2] KANNADA VOWEL SIGN O..KANNADA VOWEL SIGN OO
    return "Kannada";
  }
  if ((x >= 0x0CCC) && (x <= 0x0CCD)) { // Mn   [2] KANNADA VOWEL SIGN AU..KANNADA SIGN VIRAMA
    return "Kannada";
  }
  if ((x >= 0x0CD5) && (x <= 0x0CD6)) { // Mc   [2] KANNADA LENGTH MARK..KANNADA AI LENGTH MARK
    return "Kannada";
  }
  if (x == 0x0CDE ) {//  Lo       KANNADA LETTER FA
    return "Kannada";
  }
  if ((x >= 0x0CE0) && (x <= 0x0CE1)) { // Lo   [2] KANNADA LETTER VOCALIC RR..KANNADA LETTER VOCALIC LL
    return "Kannada";
  }
  if ((x >= 0x0CE2) && (x <= 0x0CE3)) { // Mn   [2] KANNADA VOWEL SIGN VOCALIC L..KANNADA VOWEL SIGN VOCALIC LL
    return "Kannada";
  }
  if ((x >= 0x0CE6) && (x <= 0x0CEF)) { // Nd  [10] KANNADA DIGIT ZERO..KANNADA DIGIT NINE
    return "Kannada";
  }
  if ((x >= 0x0CF1) && (x <= 0x0CF2)) { // Lo   [2] KANNADA SIGN JIHVAMULIYA..KANNADA SIGN UPADHMANIYA
    return "Kannada";
  }
  // Total code points: 86
  // ================================================
  if ((x >= 0x0D02) && (x <= 0x0D03)) { // Mc   [2] MALAYALAM SIGN ANUSVARA..MALAYALAM SIGN VISARGA
    return "Malayalam";
  }
  if ((x >= 0x0D05) && (x <= 0x0D0C)) { // Lo   [8] MALAYALAM LETTER A..MALAYALAM LETTER VOCALIC L
    return "Malayalam";
  }
  if ((x >= 0x0D0E) && (x <= 0x0D10)) { // Lo   [3] MALAYALAM LETTER E..MALAYALAM LETTER AI
    return "Malayalam";
  }
  if ((x >= 0x0D12) && (x <= 0x0D3A)) { // Lo  [41] MALAYALAM LETTER O..MALAYALAM LETTER TTTA
    return "Malayalam";
  }
  if (x == 0x0D3D ) {//  Lo       MALAYALAM SIGN AVAGRAHA
    return "Malayalam";
  }
  if ((x >= 0x0D3E) && (x <= 0x0D40)) { // Mc   [3] MALAYALAM VOWEL SIGN AA..MALAYALAM VOWEL SIGN II
    return "Malayalam";
  }
  if ((x >= 0x0D41) && (x <= 0x0D44)) { // Mn   [4] MALAYALAM VOWEL SIGN U..MALAYALAM VOWEL SIGN VOCALIC RR
    return "Malayalam";
  }
  if ((x >= 0x0D46) && (x <= 0x0D48)) { // Mc   [3] MALAYALAM VOWEL SIGN E..MALAYALAM VOWEL SIGN AI
    return "Malayalam";
  }
  if ((x >= 0x0D4A) && (x <= 0x0D4C)) { // Mc   [3] MALAYALAM VOWEL SIGN O..MALAYALAM VOWEL SIGN AU
    return "Malayalam";
  }
  if (x == 0x0D4D ) {//  Mn       MALAYALAM SIGN VIRAMA
    return "Malayalam";
  }
  if (x == 0x0D4E ) {//  Lo       MALAYALAM LETTER DOT REPH
    return "Malayalam";
  }
  if (x == 0x0D57 ) {//  Mc       MALAYALAM AU LENGTH MARK
    return "Malayalam";
  }
  if ((x >= 0x0D60) && (x <= 0x0D61)) { // Lo   [2] MALAYALAM LETTER VOCALIC RR..MALAYALAM LETTER VOCALIC LL
    return "Malayalam";
  }
  if ((x >= 0x0D62) && (x <= 0x0D63)) { // Mn   [2] MALAYALAM VOWEL SIGN VOCALIC L..MALAYALAM VOWEL SIGN VOCALIC LL
    return "Malayalam";
  }
  if ((x >= 0x0D66) && (x <= 0x0D6F)) { // Nd  [10] MALAYALAM DIGIT ZERO..MALAYALAM DIGIT NINE
    return "Malayalam";
  }
  if ((x >= 0x0D70) && (x <= 0x0D75)) { // No   [6] MALAYALAM NUMBER TEN..MALAYALAM FRACTION THREE QUARTERS
    return "Malayalam";
  }
  if (x == 0x0D79 ) {//  So       MALAYALAM DATE MARK
    return "Malayalam";
  }
  if ((x >= 0x0D7A) && (x <= 0x0D7F)) { // Lo   [6] MALAYALAM LETTER CHILLU NN..MALAYALAM LETTER CHILLU K
    return "Malayalam";
  }
  // Total code points: 98
  // ================================================
  if ((x >= 0x0D82) && (x <= 0x0D83)) { // Mc   [2] SINHALA SIGN ANUSVARAYA..SINHALA SIGN VISARGAYA
    return "Sinhala";
  }
  if ((x >= 0x0D85) && (x <= 0x0D96)) { // Lo  [18] SINHALA LETTER AYANNA..SINHALA LETTER AUYANNA
    return "Sinhala";
  }
  if ((x >= 0x0D9A) && (x <= 0x0DB1)) { // Lo  [24] SINHALA LETTER ALPAPRAANA KAYANNA..SINHALA LETTER DANTAJA NAYANNA
    return "Sinhala";
  }
  if ((x >= 0x0DB3) && (x <= 0x0DBB)) { // Lo   [9] SINHALA LETTER SANYAKA DAYANNA..SINHALA LETTER RAYANNA
    return "Sinhala";
  }
  if (x == 0x0DBD ) {//  Lo       SINHALA LETTER DANTAJA LAYANNA
    return "Sinhala";
  }
  if ((x >= 0x0DC0) && (x <= 0x0DC6)) { // Lo   [7] SINHALA LETTER VAYANNA..SINHALA LETTER FAYANNA
    return "Sinhala";
  }
  if (x == 0x0DCA ) {//  Mn       SINHALA SIGN AL-LAKUNA
    return "Sinhala";
  }
  if ((x >= 0x0DCF) && (x <= 0x0DD1)) { // Mc   [3] SINHALA VOWEL SIGN AELA-PILLA..SINHALA VOWEL SIGN DIGA AEDA-PILLA
    return "Sinhala";
  }
  if ((x >= 0x0DD2) && (x <= 0x0DD4)) { // Mn   [3] SINHALA VOWEL SIGN KETTI IS-PILLA..SINHALA VOWEL SIGN KETTI PAA-PILLA
    return "Sinhala";
  }
  if (x == 0x0DD6 ) {//  Mn       SINHALA VOWEL SIGN DIGA PAA-PILLA
    return "Sinhala";
  }
  if ((x >= 0x0DD8) && (x <= 0x0DDF)) { // Mc   [8] SINHALA VOWEL SIGN GAETTA-PILLA..SINHALA VOWEL SIGN GAYANUKITTA
    return "Sinhala";
  }
  if ((x >= 0x0DF2) && (x <= 0x0DF3)) { // Mc   [2] SINHALA VOWEL SIGN DIGA GAETTA-PILLA..SINHALA VOWEL SIGN DIGA GAYANUKITTA
    return "Sinhala";
  }
  if (x == 0x0DF4 ) {//  Po       SINHALA PUNCTUATION KUNDDALIYA
    return "Sinhala";
  }
  // Total code points: 80
  // ================================================
  if ((x >= 0x0E01) && (x <= 0x0E30)) { // Lo  [48] THAI CHARACTER KO KAI..THAI CHARACTER SARA A
    return "Thai";
  }
  if (x == 0x0E31 ) {//  Mn       THAI CHARACTER MAI HAN-AKAT
    return "Thai";
  }
  if ((x >= 0x0E32) && (x <= 0x0E33)) { // Lo   [2] THAI CHARACTER SARA AA..THAI CHARACTER SARA AM
    return "Thai";
  }
  if ((x >= 0x0E34) && (x <= 0x0E3A)) { // Mn   [7] THAI CHARACTER SARA I..THAI CHARACTER PHINTHU
    return "Thai";
  }
  if ((x >= 0x0E40) && (x <= 0x0E45)) { // Lo   [6] THAI CHARACTER SARA E..THAI CHARACTER LAKKHANGYAO
    return "Thai";
  }
  if (x == 0x0E46 ) {//  Lm       THAI CHARACTER MAIYAMOK
    return "Thai";
  }
  if ((x >= 0x0E47) && (x <= 0x0E4E)) { // Mn   [8] THAI CHARACTER MAITAIKHU..THAI CHARACTER YAMAKKAN
    return "Thai";
  }
  if (x == 0x0E4F ) {//  Po       THAI CHARACTER FONGMAN
    return "Thai";
  }
  if ((x >= 0x0E50) && (x <= 0x0E59)) { // Nd  [10] THAI DIGIT ZERO..THAI DIGIT NINE
    return "Thai";
  }
  if ((x >= 0x0E5A) && (x <= 0x0E5B)) { // Po   [2] THAI CHARACTER ANGKHANKHU..THAI CHARACTER KHOMUT
    return "Thai";
  }
  // Total code points: 86
  // ================================================
  if ((x >= 0x0E81) && (x <= 0x0E82)) { // Lo   [2] LAO LETTER KO..LAO LETTER KHO SUNG
    return "Lao";
  }
  if (x == 0x0E84 ) {//  Lo       LAO LETTER KHO TAM
    return "Lao";
  }
  if ((x >= 0x0E87) && (x <= 0x0E88)) { // Lo   [2] LAO LETTER NGO..LAO LETTER CO
    return "Lao";
  }
  if (x == 0x0E8A ) {//  Lo       LAO LETTER SO TAM
    return "Lao";
  }
  if (x == 0x0E8D ) {//  Lo       LAO LETTER NYO
    return "Lao";
  }
  if ((x >= 0x0E94) && (x <= 0x0E97)) { // Lo   [4] LAO LETTER DO..LAO LETTER THO TAM
    return "Lao";
  }
  if ((x >= 0x0E99) && (x <= 0x0E9F)) { // Lo   [7] LAO LETTER NO..LAO LETTER FO SUNG
    return "Lao";
  }
  if ((x >= 0x0EA1) && (x <= 0x0EA3)) { // Lo   [3] LAO LETTER MO..LAO LETTER LO LING
    return "Lao";
  }
  if (x == 0x0EA5 ) {//  Lo       LAO LETTER LO LOOT
    return "Lao";
  }
  if (x == 0x0EA7 ) {//  Lo       LAO LETTER WO
    return "Lao";
  }
  if ((x >= 0x0EAA) && (x <= 0x0EAB)) { // Lo   [2] LAO LETTER SO SUNG..LAO LETTER HO SUNG
    return "Lao";
  }
  if ((x >= 0x0EAD) && (x <= 0x0EB0)) { // Lo   [4] LAO LETTER O..LAO VOWEL SIGN A
    return "Lao";
  }
  if (x == 0x0EB1 ) {//  Mn       LAO VOWEL SIGN MAI KAN
    return "Lao";
  }
  if ((x >= 0x0EB2) && (x <= 0x0EB3)) { // Lo   [2] LAO VOWEL SIGN AA..LAO VOWEL SIGN AM
    return "Lao";
  }
  if ((x >= 0x0EB4) && (x <= 0x0EB9)) { // Mn   [6] LAO VOWEL SIGN I..LAO VOWEL SIGN UU
    return "Lao";
  }
  if ((x >= 0x0EBB) && (x <= 0x0EBC)) { // Mn   [2] LAO VOWEL SIGN MAI KON..LAO SEMIVOWEL SIGN LO
    return "Lao";
  }
  if (x == 0x0EBD ) {//  Lo       LAO SEMIVOWEL SIGN NYO
    return "Lao";
  }
  if ((x >= 0x0EC0) && (x <= 0x0EC4)) { // Lo   [5] LAO VOWEL SIGN E..LAO VOWEL SIGN AI
    return "Lao";
  }
  if (x == 0x0EC6 ) {//  Lm       LAO KO LA
    return "Lao";
  }
  if ((x >= 0x0EC8) && (x <= 0x0ECD)) { // Mn   [6] LAO TONE MAI EK..LAO NIGGAHITA
    return "Lao";
  }
  if ((x >= 0x0ED0) && (x <= 0x0ED9)) { // Nd  [10] LAO DIGIT ZERO..LAO DIGIT NINE
    return "Lao";
  }
  if ((x >= 0x0EDC) && (x <= 0x0EDF)) { // Lo   [4] LAO HO NO..LAO LETTER KHMU NYO
    return "Lao";
  }
  // Total code points: 67
  // ================================================
  if (x == 0x0F00 ) {//  Lo       TIBETAN SYLLABLE OM
    return "Tibetan";
  }
  if ((x >= 0x0F01) && (x <= 0x0F03)) { // So   [3] TIBETAN MARK GTER YIG MGO TRUNCATED A..TIBETAN MARK GTER YIG MGO -UM GTER TSHEG MA
    return "Tibetan";
  }
  if ((x >= 0x0F04) && (x <= 0x0F12)) { // Po  [15] TIBETAN MARK INITIAL YIG MGO MDUN MA..TIBETAN MARK RGYA GRAM SHAD
    return "Tibetan";
  }
  if (x == 0x0F13 ) {//  So       TIBETAN MARK CARET -DZUD RTAGS ME LONG CAN
    return "Tibetan";
  }
  if (x == 0x0F14 ) {//  Po       TIBETAN MARK GTER TSHEG
    return "Tibetan";
  }
  if ((x >= 0x0F15) && (x <= 0x0F17)) { // So   [3] TIBETAN LOGOTYPE SIGN CHAD RTAGS..TIBETAN ASTROLOGICAL SIGN SGRA GCAN -CHAR RTAGS
    return "Tibetan";
  }
  if ((x >= 0x0F18) && (x <= 0x0F19)) { // Mn   [2] TIBETAN ASTROLOGICAL SIGN -KHYUD PA..TIBETAN ASTROLOGICAL SIGN SDONG TSHUGS
    return "Tibetan";
  }
  if ((x >= 0x0F1A) && (x <= 0x0F1F)) { // So   [6] TIBETAN SIGN RDEL DKAR GCIG..TIBETAN SIGN RDEL DKAR RDEL NAG
    return "Tibetan";
  }
  if ((x >= 0x0F20) && (x <= 0x0F29)) { // Nd  [10] TIBETAN DIGIT ZERO..TIBETAN DIGIT NINE
    return "Tibetan";
  }
  if ((x >= 0x0F2A) && (x <= 0x0F33)) { // No  [10] TIBETAN DIGIT HALF ONE..TIBETAN DIGIT HALF ZERO
    return "Tibetan";
  }
  if (x == 0x0F34 ) {//  So       TIBETAN MARK BSDUS RTAGS
    return "Tibetan";
  }
  if (x == 0x0F35 ) {//  Mn       TIBETAN MARK NGAS BZUNG NYI ZLA
    return "Tibetan";
  }
  if (x == 0x0F36 ) {//  So       TIBETAN MARK CARET -DZUD RTAGS BZHI MIG CAN
    return "Tibetan";
  }
  if (x == 0x0F37 ) {//  Mn       TIBETAN MARK NGAS BZUNG SGOR RTAGS
    return "Tibetan";
  }
  if (x == 0x0F38 ) {//  So       TIBETAN MARK CHE MGO
    return "Tibetan";
  }
  if (x == 0x0F39 ) {//  Mn       TIBETAN MARK TSA -PHRU
    return "Tibetan";
  }
  if (x == 0x0F3A ) {//  Ps       TIBETAN MARK GUG RTAGS GYON
    return "Tibetan";
  }
  if (x == 0x0F3B ) {//  Pe       TIBETAN MARK GUG RTAGS GYAS
    return "Tibetan";
  }
  if (x == 0x0F3C ) {//  Ps       TIBETAN MARK ANG KHANG GYON
    return "Tibetan";
  }
  if (x == 0x0F3D ) {//  Pe       TIBETAN MARK ANG KHANG GYAS
    return "Tibetan";
  }
  if ((x >= 0x0F3E) && (x <= 0x0F3F)) { // Mc   [2] TIBETAN SIGN YAR TSHES..TIBETAN SIGN MAR TSHES
    return "Tibetan";
  }
  if ((x >= 0x0F40) && (x <= 0x0F47)) { // Lo   [8] TIBETAN LETTER KA..TIBETAN LETTER JA
    return "Tibetan";
  }
  if ((x >= 0x0F49) && (x <= 0x0F6C)) { // Lo  [36] TIBETAN LETTER NYA..TIBETAN LETTER RRA
    return "Tibetan";
  }
  if ((x >= 0x0F71) && (x <= 0x0F7E)) { // Mn  [14] TIBETAN VOWEL SIGN AA..TIBETAN SIGN RJES SU NGA RO
    return "Tibetan";
  }
  if (x == 0x0F7F ) {//  Mc       TIBETAN SIGN RNAM BCAD
    return "Tibetan";
  }
  if ((x >= 0x0F80) && (x <= 0x0F84)) { // Mn   [5] TIBETAN VOWEL SIGN REVERSED I..TIBETAN MARK HALANTA
    return "Tibetan";
  }
  if (x == 0x0F85 ) {//  Po       TIBETAN MARK PALUTA
    return "Tibetan";
  }
  if ((x >= 0x0F86) && (x <= 0x0F87)) { // Mn   [2] TIBETAN SIGN LCI RTAGS..TIBETAN SIGN YANG RTAGS
    return "Tibetan";
  }
  if ((x >= 0x0F88) && (x <= 0x0F8C)) { // Lo   [5] TIBETAN SIGN LCE TSA CAN..TIBETAN SIGN INVERTED MCHU CAN
    return "Tibetan";
  }
  if ((x >= 0x0F8D) && (x <= 0x0F97)) { // Mn  [11] TIBETAN SUBJOINED SIGN LCE TSA CAN..TIBETAN SUBJOINED LETTER JA
    return "Tibetan";
  }
  if ((x >= 0x0F99) && (x <= 0x0FBC)) { // Mn  [36] TIBETAN SUBJOINED LETTER NYA..TIBETAN SUBJOINED LETTER FIXED-FORM RA
    return "Tibetan";
  }
  if ((x >= 0x0FBE) && (x <= 0x0FC5)) { // So   [8] TIBETAN KU RU KHA..TIBETAN SYMBOL RDO RJE
    return "Tibetan";
  }
  if (x == 0x0FC6 ) {//  Mn       TIBETAN SYMBOL PADMA GDAN
    return "Tibetan";
  }
  if ((x >= 0x0FC7) && (x <= 0x0FCC)) { // So   [6] TIBETAN SYMBOL RDO RJE RGYA GRAM..TIBETAN SYMBOL NOR BU BZHI -KHYIL
    return "Tibetan";
  }
  if ((x >= 0x0FCE) && (x <= 0x0FCF)) { // So   [2] TIBETAN SIGN RDEL NAG RDEL DKAR..TIBETAN SIGN RDEL NAG GSUM
    return "Tibetan";
  }
  if ((x >= 0x0FD0) && (x <= 0x0FD4)) { // Po   [5] TIBETAN MARK BSKA- SHOG GI MGO RGYAN..TIBETAN MARK CLOSING BRDA RNYING YIG MGO SGAB MA
    return "Tibetan";
  }
  if ((x >= 0x0FD9) && (x <= 0x0FDA)) { // Po   [2] TIBETAN MARK LEADING MCHAN RTAGS..TIBETAN MARK TRAILING MCHAN RTAGS
    return "Tibetan";
  }
  // Total code points: 207
  // ================================================
  if ((x >= 0x1000) && (x <= 0x102A)) { // Lo  [43] MYANMAR LETTER KA..MYANMAR LETTER AU
    return "Myanmar";
  }
  if ((x >= 0x102B) && (x <= 0x102C)) { // Mc   [2] MYANMAR VOWEL SIGN TALL AA..MYANMAR VOWEL SIGN AA
    return "Myanmar";
  }
  if ((x >= 0x102D) && (x <= 0x1030)) { // Mn   [4] MYANMAR VOWEL SIGN I..MYANMAR VOWEL SIGN UU
    return "Myanmar";
  }
  if (x == 0x1031 ) {//  Mc       MYANMAR VOWEL SIGN E
    return "Myanmar";
  }
  if ((x >= 0x1032) && (x <= 0x1037)) { // Mn   [6] MYANMAR VOWEL SIGN AI..MYANMAR SIGN DOT BELOW
    return "Myanmar";
  }
  if (x == 0x1038 ) {//  Mc       MYANMAR SIGN VISARGA
    return "Myanmar";
  }
  if ((x >= 0x1039) && (x <= 0x103A)) { // Mn   [2] MYANMAR SIGN VIRAMA..MYANMAR SIGN ASAT
    return "Myanmar";
  }
  if ((x >= 0x103B) && (x <= 0x103C)) { // Mc   [2] MYANMAR CONSONANT SIGN MEDIAL YA..MYANMAR CONSONANT SIGN MEDIAL RA
    return "Myanmar";
  }
  if ((x >= 0x103D) && (x <= 0x103E)) { // Mn   [2] MYANMAR CONSONANT SIGN MEDIAL WA..MYANMAR CONSONANT SIGN MEDIAL HA
    return "Myanmar";
  }
  if (x == 0x103F ) {//  Lo       MYANMAR LETTER GREAT SA
    return "Myanmar";
  }
  if ((x >= 0x1040) && (x <= 0x1049)) { // Nd  [10] MYANMAR DIGIT ZERO..MYANMAR DIGIT NINE
    return "Myanmar";
  }
  if ((x >= 0x104A) && (x <= 0x104F)) { // Po   [6] MYANMAR SIGN LITTLE SECTION..MYANMAR SYMBOL GENITIVE
    return "Myanmar";
  }
  if ((x >= 0x1050) && (x <= 0x1055)) { // Lo   [6] MYANMAR LETTER SHA..MYANMAR LETTER VOCALIC LL
    return "Myanmar";
  }
  if ((x >= 0x1056) && (x <= 0x1057)) { // Mc   [2] MYANMAR VOWEL SIGN VOCALIC R..MYANMAR VOWEL SIGN VOCALIC RR
    return "Myanmar";
  }
  if ((x >= 0x1058) && (x <= 0x1059)) { // Mn   [2] MYANMAR VOWEL SIGN VOCALIC L..MYANMAR VOWEL SIGN VOCALIC LL
    return "Myanmar";
  }
  if ((x >= 0x105A) && (x <= 0x105D)) { // Lo   [4] MYANMAR LETTER MON NGA..MYANMAR LETTER MON BBE
    return "Myanmar";
  }
  if ((x >= 0x105E) && (x <= 0x1060)) { // Mn   [3] MYANMAR CONSONANT SIGN MON MEDIAL NA..MYANMAR CONSONANT SIGN MON MEDIAL LA
    return "Myanmar";
  }
  if (x == 0x1061 ) {//  Lo       MYANMAR LETTER SGAW KAREN SHA
    return "Myanmar";
  }
  if ((x >= 0x1062) && (x <= 0x1064)) { // Mc   [3] MYANMAR VOWEL SIGN SGAW KAREN EU..MYANMAR TONE MARK SGAW KAREN KE PHO
    return "Myanmar";
  }
  if ((x >= 0x1065) && (x <= 0x1066)) { // Lo   [2] MYANMAR LETTER WESTERN PWO KAREN THA..MYANMAR LETTER WESTERN PWO KAREN PWA
    return "Myanmar";
  }
  if ((x >= 0x1067) && (x <= 0x106D)) { // Mc   [7] MYANMAR VOWEL SIGN WESTERN PWO KAREN EU..MYANMAR SIGN WESTERN PWO KAREN TONE-5
    return "Myanmar";
  }
  if ((x >= 0x106E) && (x <= 0x1070)) { // Lo   [3] MYANMAR LETTER EASTERN PWO KAREN NNA..MYANMAR LETTER EASTERN PWO KAREN GHWA
    return "Myanmar";
  }
  if ((x >= 0x1071) && (x <= 0x1074)) { // Mn   [4] MYANMAR VOWEL SIGN GEBA KAREN I..MYANMAR VOWEL SIGN KAYAH EE
    return "Myanmar";
  }
  if ((x >= 0x1075) && (x <= 0x1081)) { // Lo  [13] MYANMAR LETTER SHAN KA..MYANMAR LETTER SHAN HA
    return "Myanmar";
  }
  if (x == 0x1082 ) {//  Mn       MYANMAR CONSONANT SIGN SHAN MEDIAL WA
    return "Myanmar";
  }
  if ((x >= 0x1083) && (x <= 0x1084)) { // Mc   [2] MYANMAR VOWEL SIGN SHAN AA..MYANMAR VOWEL SIGN SHAN E
    return "Myanmar";
  }
  if ((x >= 0x1085) && (x <= 0x1086)) { // Mn   [2] MYANMAR VOWEL SIGN SHAN E ABOVE..MYANMAR VOWEL SIGN SHAN FINAL Y
    return "Myanmar";
  }
  if ((x >= 0x1087) && (x <= 0x108C)) { // Mc   [6] MYANMAR SIGN SHAN TONE-2..MYANMAR SIGN SHAN COUNCIL TONE-3
    return "Myanmar";
  }
  if (x == 0x108D ) {//  Mn       MYANMAR SIGN SHAN COUNCIL EMPHATIC TONE
    return "Myanmar";
  }
  if (x == 0x108E ) {//  Lo       MYANMAR LETTER RUMAI PALAUNG FA
    return "Myanmar";
  }
  if (x == 0x108F ) {//  Mc       MYANMAR SIGN RUMAI PALAUNG TONE-5
    return "Myanmar";
  }
  if ((x >= 0x1090) && (x <= 0x1099)) { // Nd  [10] MYANMAR SHAN DIGIT ZERO..MYANMAR SHAN DIGIT NINE
    return "Myanmar";
  }
  if ((x >= 0x109A) && (x <= 0x109C)) { // Mc   [3] MYANMAR SIGN KHAMTI TONE-1..MYANMAR VOWEL SIGN AITON A
    return "Myanmar";
  }
  if (x == 0x109D ) {//  Mn       MYANMAR VOWEL SIGN AITON AI
    return "Myanmar";
  }
  if ((x >= 0x109E) && (x <= 0x109F)) { // So   [2] MYANMAR SYMBOL SHAN ONE..MYANMAR SYMBOL SHAN EXCLAMATION
    return "Myanmar";
  }
  if ((x >= 0xAA60) && (x <= 0xAA6F)) { // Lo  [16] MYANMAR LETTER KHAMTI GA..MYANMAR LETTER KHAMTI FA
    return "Myanmar";
  }
  if (x == 0xAA70 ) {//  Lm       MYANMAR MODIFIER LETTER KHAMTI REDUPLICATION
    return "Myanmar";
  }
  if ((x >= 0xAA71) && (x <= 0xAA76)) { // Lo   [6] MYANMAR LETTER KHAMTI XA..MYANMAR LOGOGRAM KHAMTI HM
    return "Myanmar";
  }
  if ((x >= 0xAA77) && (x <= 0xAA79)) { // So   [3] MYANMAR SYMBOL AITON EXCLAMATION..MYANMAR SYMBOL AITON TWO
    return "Myanmar";
  }
  if (x == 0xAA7A ) {//  Lo       MYANMAR LETTER AITON RA
    return "Myanmar";
  }
  if (x == 0xAA7B ) {//  Mc       MYANMAR SIGN PAO KAREN TONE
    return "Myanmar";
  }
  // Total code points: 188
  // ================================================
  if ((x >= 0x10A0) && (x <= 0x10C5)) { // L&  [38] GEORGIAN CAPITAL LETTER AN..GEORGIAN CAPITAL LETTER HOE
    return "Georgian";
  }
  if (x == 0x10C7 ) {//  L&       GEORGIAN CAPITAL LETTER YN
    return "Georgian";
  }
  if (x == 0x10CD ) {//  L&       GEORGIAN CAPITAL LETTER AEN
    return "Georgian";
  }
  if ((x >= 0x10D0) && (x <= 0x10FA)) { // Lo  [43] GEORGIAN LETTER AN..GEORGIAN LETTER AIN
    return "Georgian";
  }
  if (x == 0x10FC ) {//  Lm       MODIFIER LETTER GEORGIAN NAR
    return "Georgian";
  }
  if ((x >= 0x10FD) && (x <= 0x10FF)) { // Lo   [3] GEORGIAN LETTER AEN..GEORGIAN LETTER LABIAL SIGN
    return "Georgian";
  }
  if ((x >= 0x2D00) && (x <= 0x2D25)) { // L&  [38] GEORGIAN SMALL LETTER AN..GEORGIAN SMALL LETTER HOE
    return "Georgian";
  }
  if (x == 0x2D27 ) {//  L&       GEORGIAN SMALL LETTER YN
    return "Georgian";
  }
  if (x == 0x2D2D ) {//  L&       GEORGIAN SMALL LETTER AEN
    return "Georgian";
  }
  // Total code points: 127
  // ================================================
  if ((x >= 0x1100) && (x <= 0x11FF)) { // Lo [256] HANGUL CHOSEONG KIYEOK..HANGUL JONGSEONG SSANGNIEUN
    return "Hangul";
  }
  if ((x >= 0x302E) && (x <= 0x302F)) { // Mc   [2] HANGUL SINGLE DOT TONE MARK..HANGUL DOUBLE DOT TONE MARK
    return "Hangul";
  }
  if ((x >= 0x3131) && (x <= 0x318E)) { // Lo  [94] HANGUL LETTER KIYEOK..HANGUL LETTER ARAEAE
    return "Hangul";
  }
  if ((x >= 0x3200) && (x <= 0x321E)) { // So  [31] PARENTHESIZED HANGUL KIYEOK..PARENTHESIZED KOREAN CHARACTER O HU
    return "Hangul";
  }
  if ((x >= 0x3260) && (x <= 0x327E)) { // So  [31] CIRCLED HANGUL KIYEOK..CIRCLED HANGUL IEUNG U
    return "Hangul";
  }
  if ((x >= 0xA960) && (x <= 0xA97C)) { // Lo  [29] HANGUL CHOSEONG TIKEUT-MIEUM..HANGUL CHOSEONG SSANGYEORINHIEUH
    return "Hangul";
  }
  if ((x >= 0xAC00) && (x <= 0xD7A3)) { // Lo [11172] HANGUL SYLLABLE GA..HANGUL SYLLABLE HIH
    return "Hangul";
  }
  if ((x >= 0xD7B0) && (x <= 0xD7C6)) { // Lo  [23] HANGUL JUNGSEONG O-YEO..HANGUL JUNGSEONG ARAEA-E
    return "Hangul";
  }
  if ((x >= 0xD7CB) && (x <= 0xD7FB)) { // Lo  [49] HANGUL JONGSEONG NIEUN-RIEUL..HANGUL JONGSEONG PHIEUPH-THIEUTH
    return "Hangul";
  }
  if ((x >= 0xFFA0) && (x <= 0xFFBE)) { // Lo  [31] HALFWIDTH HANGUL FILLER..HALFWIDTH HANGUL LETTER HIEUH
    return "Hangul";
  }
  if ((x >= 0xFFC2) && (x <= 0xFFC7)) { // Lo   [6] HALFWIDTH HANGUL LETTER A..HALFWIDTH HANGUL LETTER E
    return "Hangul";
  }
  if ((x >= 0xFFCA) && (x <= 0xFFCF)) { // Lo   [6] HALFWIDTH HANGUL LETTER YEO..HALFWIDTH HANGUL LETTER OE
    return "Hangul";
  }
  if ((x >= 0xFFD2) && (x <= 0xFFD7)) { // Lo   [6] HALFWIDTH HANGUL LETTER YO..HALFWIDTH HANGUL LETTER YU
    return "Hangul";
  }
  if ((x >= 0xFFDA) && (x <= 0xFFDC)) { // Lo   [3] HALFWIDTH HANGUL LETTER EU..HALFWIDTH HANGUL LETTER I
    return "Hangul";
  }
  // Total code points: 11739
  // ================================================
  if ((x >= 0x1200) && (x <= 0x1248)) { // Lo  [73] ETHIOPIC SYLLABLE HA..ETHIOPIC SYLLABLE QWA
    return "Ethiopic";
  }
  if ((x >= 0x124A) && (x <= 0x124D)) { // Lo   [4] ETHIOPIC SYLLABLE QWI..ETHIOPIC SYLLABLE QWE
    return "Ethiopic";
  }
  if ((x >= 0x1250) && (x <= 0x1256)) { // Lo   [7] ETHIOPIC SYLLABLE QHA..ETHIOPIC SYLLABLE QHO
    return "Ethiopic";
  }
  if (x == 0x1258 ) {//  Lo       ETHIOPIC SYLLABLE QHWA
    return "Ethiopic";
  }
  if ((x >= 0x125A) && (x <= 0x125D)) { // Lo   [4] ETHIOPIC SYLLABLE QHWI..ETHIOPIC SYLLABLE QHWE
    return "Ethiopic";
  }
  if ((x >= 0x1260) && (x <= 0x1288)) { // Lo  [41] ETHIOPIC SYLLABLE BA..ETHIOPIC SYLLABLE XWA
    return "Ethiopic";
  }
  if ((x >= 0x128A) && (x <= 0x128D)) { // Lo   [4] ETHIOPIC SYLLABLE XWI..ETHIOPIC SYLLABLE XWE
    return "Ethiopic";
  }
  if ((x >= 0x1290) && (x <= 0x12B0)) { // Lo  [33] ETHIOPIC SYLLABLE NA..ETHIOPIC SYLLABLE KWA
    return "Ethiopic";
  }
  if ((x >= 0x12B2) && (x <= 0x12B5)) { // Lo   [4] ETHIOPIC SYLLABLE KWI..ETHIOPIC SYLLABLE KWE
    return "Ethiopic";
  }
  if ((x >= 0x12B8) && (x <= 0x12BE)) { // Lo   [7] ETHIOPIC SYLLABLE KXA..ETHIOPIC SYLLABLE KXO
    return "Ethiopic";
  }
  if (x == 0x12C0 ) {//  Lo       ETHIOPIC SYLLABLE KXWA
    return "Ethiopic";
  }
  if ((x >= 0x12C2) && (x <= 0x12C5)) { // Lo   [4] ETHIOPIC SYLLABLE KXWI..ETHIOPIC SYLLABLE KXWE
    return "Ethiopic";
  }
  if ((x >= 0x12C8) && (x <= 0x12D6)) { // Lo  [15] ETHIOPIC SYLLABLE WA..ETHIOPIC SYLLABLE PHARYNGEAL O
    return "Ethiopic";
  }
  if ((x >= 0x12D8) && (x <= 0x1310)) { // Lo  [57] ETHIOPIC SYLLABLE ZA..ETHIOPIC SYLLABLE GWA
    return "Ethiopic";
  }
  if ((x >= 0x1312) && (x <= 0x1315)) { // Lo   [4] ETHIOPIC SYLLABLE GWI..ETHIOPIC SYLLABLE GWE
    return "Ethiopic";
  }
  if ((x >= 0x1318) && (x <= 0x135A)) { // Lo  [67] ETHIOPIC SYLLABLE GGA..ETHIOPIC SYLLABLE FYA
    return "Ethiopic";
  }
  if ((x >= 0x135D) && (x <= 0x135F)) { // Mn   [3] ETHIOPIC COMBINING GEMINATION AND VOWEL LENGTH MARK..ETHIOPIC COMBINING GEMINATION MARK
    return "Ethiopic";
  }
  if ((x >= 0x1360) && (x <= 0x1368)) { // Po   [9] ETHIOPIC SECTION MARK..ETHIOPIC PARAGRAPH SEPARATOR
    return "Ethiopic";
  }
  if ((x >= 0x1369) && (x <= 0x137C)) { // No  [20] ETHIOPIC DIGIT ONE..ETHIOPIC NUMBER TEN THOUSAND
    return "Ethiopic";
  }
  if ((x >= 0x1380) && (x <= 0x138F)) { // Lo  [16] ETHIOPIC SYLLABLE SEBATBEIT MWA..ETHIOPIC SYLLABLE PWE
    return "Ethiopic";
  }
  if ((x >= 0x1390) && (x <= 0x1399)) { // So  [10] ETHIOPIC TONAL MARK YIZET..ETHIOPIC TONAL MARK KURT
    return "Ethiopic";
  }
  if ((x >= 0x2D80) && (x <= 0x2D96)) { // Lo  [23] ETHIOPIC SYLLABLE LOA..ETHIOPIC SYLLABLE GGWE
    return "Ethiopic";
  }
  if ((x >= 0x2DA0) && (x <= 0x2DA6)) { // Lo   [7] ETHIOPIC SYLLABLE SSA..ETHIOPIC SYLLABLE SSO
    return "Ethiopic";
  }
  if ((x >= 0x2DA8) && (x <= 0x2DAE)) { // Lo   [7] ETHIOPIC SYLLABLE CCA..ETHIOPIC SYLLABLE CCO
    return "Ethiopic";
  }
  if ((x >= 0x2DB0) && (x <= 0x2DB6)) { // Lo   [7] ETHIOPIC SYLLABLE ZZA..ETHIOPIC SYLLABLE ZZO
    return "Ethiopic";
  }
  if ((x >= 0x2DB8) && (x <= 0x2DBE)) { // Lo   [7] ETHIOPIC SYLLABLE CCHA..ETHIOPIC SYLLABLE CCHO
    return "Ethiopic";
  }
  if ((x >= 0x2DC0) && (x <= 0x2DC6)) { // Lo   [7] ETHIOPIC SYLLABLE QYA..ETHIOPIC SYLLABLE QYO
    return "Ethiopic";
  }
  if ((x >= 0x2DC8) && (x <= 0x2DCE)) { // Lo   [7] ETHIOPIC SYLLABLE KYA..ETHIOPIC SYLLABLE KYO
    return "Ethiopic";
  }
  if ((x >= 0x2DD0) && (x <= 0x2DD6)) { // Lo   [7] ETHIOPIC SYLLABLE XYA..ETHIOPIC SYLLABLE XYO
    return "Ethiopic";
  }
  if ((x >= 0x2DD8) && (x <= 0x2DDE)) { // Lo   [7] ETHIOPIC SYLLABLE GYA..ETHIOPIC SYLLABLE GYO
    return "Ethiopic";
  }
  if ((x >= 0xAB01) && (x <= 0xAB06)) { // Lo   [6] ETHIOPIC SYLLABLE TTHU..ETHIOPIC SYLLABLE TTHO
    return "Ethiopic";
  }
  if ((x >= 0xAB09) && (x <= 0xAB0E)) { // Lo   [6] ETHIOPIC SYLLABLE DDHU..ETHIOPIC SYLLABLE DDHO
    return "Ethiopic";
  }
  if ((x >= 0xAB11) && (x <= 0xAB16)) { // Lo   [6] ETHIOPIC SYLLABLE DZU..ETHIOPIC SYLLABLE DZO
    return "Ethiopic";
  }
  if ((x >= 0xAB20) && (x <= 0xAB26)) { // Lo   [7] ETHIOPIC SYLLABLE CCHHA..ETHIOPIC SYLLABLE CCHHO
    return "Ethiopic";
  }
  if ((x >= 0xAB28) && (x <= 0xAB2E)) { // Lo   [7] ETHIOPIC SYLLABLE BBA..ETHIOPIC SYLLABLE BBO
    return "Ethiopic";
  }
  // Total code points: 495
  // ================================================
  if ((x >= 0x13A0) && (x <= 0x13F4)) { // Lo  [85] CHEROKEE LETTER A..CHEROKEE LETTER YV
    return "Cherokee";
  }
  // Total code points: 85
  // ================================================
  if (x == 0x1400 ) {//  Pd       CANADIAN SYLLABICS HYPHEN
    return "Canadian_Aboriginal";
  }
  if ((x >= 0x1401) && (x <= 0x166C)) { // Lo [620] CANADIAN SYLLABICS E..CANADIAN SYLLABICS CARRIER TTSA
    return "Canadian_Aboriginal";
  }
  if ((x >= 0x166D) && (x <= 0x166E)) { // Po   [2] CANADIAN SYLLABICS CHI SIGN..CANADIAN SYLLABICS FULL STOP
    return "Canadian_Aboriginal";
  }
  if ((x >= 0x166F) && (x <= 0x167F)) { // Lo  [17] CANADIAN SYLLABICS QAI..CANADIAN SYLLABICS BLACKFOOT W
    return "Canadian_Aboriginal";
  }
  if ((x >= 0x18B0) && (x <= 0x18F5)) { // Lo  [70] CANADIAN SYLLABICS OY..CANADIAN SYLLABICS CARRIER DENTAL S
    return "Canadian_Aboriginal";
  }
  // Total code points: 710
  // ================================================
  if (x == 0x1680 ) {//  Zs       OGHAM SPACE MARK
    return "Ogham";
  }
  if ((x >= 0x1681) && (x <= 0x169A)) { // Lo  [26] OGHAM LETTER BEITH..OGHAM LETTER PEITH
    return "Ogham";
  }
  if (x == 0x169B ) {//  Ps       OGHAM FEATHER MARK
    return "Ogham";
  }
  if (x == 0x169C ) {//  Pe       OGHAM REVERSED FEATHER MARK
    return "Ogham";
  }
  // Total code points: 29
  // ================================================
  if ((x >= 0x16A0) && (x <= 0x16EA)) { // Lo  [75] RUNIC LETTER FEHU FEOH FE F..RUNIC LETTER X
    return "Runic";
  }
  if ((x >= 0x16EE) && (x <= 0x16F0)) { // Nl   [3] RUNIC ARLAUG SYMBOL..RUNIC BELGTHOR SYMBOL
    return "Runic";
  }
  // Total code points: 78
  // ================================================
  if ((x >= 0x1780) && (x <= 0x17B3)) { // Lo  [52] KHMER LETTER KA..KHMER INDEPENDENT VOWEL QAU
    return "Khmer";
  }
  if ((x >= 0x17B4) && (x <= 0x17B5)) { // Mn   [2] KHMER VOWEL INHERENT AQ..KHMER VOWEL INHERENT AA
    return "Khmer";
  }
  if (x == 0x17B6 ) {//  Mc       KHMER VOWEL SIGN AA
    return "Khmer";
  }
  if ((x >= 0x17B7) && (x <= 0x17BD)) { // Mn   [7] KHMER VOWEL SIGN I..KHMER VOWEL SIGN UA
    return "Khmer";
  }
  if ((x >= 0x17BE) && (x <= 0x17C5)) { // Mc   [8] KHMER VOWEL SIGN OE..KHMER VOWEL SIGN AU
    return "Khmer";
  }
  if (x == 0x17C6 ) {//  Mn       KHMER SIGN NIKAHIT
    return "Khmer";
  }
  if ((x >= 0x17C7) && (x <= 0x17C8)) { // Mc   [2] KHMER SIGN REAHMUK..KHMER SIGN YUUKALEAPINTU
    return "Khmer";
  }
  if ((x >= 0x17C9) && (x <= 0x17D3)) { // Mn  [11] KHMER SIGN MUUSIKATOAN..KHMER SIGN BATHAMASAT
    return "Khmer";
  }
  if ((x >= 0x17D4) && (x <= 0x17D6)) { // Po   [3] KHMER SIGN KHAN..KHMER SIGN CAMNUC PII KUUH
    return "Khmer";
  }
  if (x == 0x17D7 ) {//  Lm       KHMER SIGN LEK TOO
    return "Khmer";
  }
  if ((x >= 0x17D8) && (x <= 0x17DA)) { // Po   [3] KHMER SIGN BEYYAL..KHMER SIGN KOOMUUT
    return "Khmer";
  }
  if (x == 0x17DB ) {//  Sc       KHMER CURRENCY SYMBOL RIEL
    return "Khmer";
  }
  if (x == 0x17DC ) {//  Lo       KHMER SIGN AVAKRAHASANYA
    return "Khmer";
  }
  if (x == 0x17DD ) {//  Mn       KHMER SIGN ATTHACAN
    return "Khmer";
  }
  if ((x >= 0x17E0) && (x <= 0x17E9)) { // Nd  [10] KHMER DIGIT ZERO..KHMER DIGIT NINE
    return "Khmer";
  }
  if ((x >= 0x17F0) && (x <= 0x17F9)) { // No  [10] KHMER SYMBOL LEK ATTAK SON..KHMER SYMBOL LEK ATTAK PRAM-BUON
    return "Khmer";
  }
  if ((x >= 0x19E0) && (x <= 0x19FF)) { // So  [32] KHMER SYMBOL PATHAMASAT..KHMER SYMBOL DAP-PRAM ROC
    return "Khmer";
  }
  // Total code points: 146
  // ================================================
  if ((x >= 0x1800) && (x <= 0x1801)) { // Po   [2] MONGOLIAN BIRGA..MONGOLIAN ELLIPSIS
    return "Mongolian";
  }
  if (x == 0x1804 ) {//  Po       MONGOLIAN COLON
    return "Mongolian";
  }
  if (x == 0x1806 ) {//  Pd       MONGOLIAN TODO SOFT HYPHEN
    return "Mongolian";
  }
  if ((x >= 0x1807) && (x <= 0x180A)) { // Po   [4] MONGOLIAN SIBE SYLLABLE BOUNDARY MARKER..MONGOLIAN NIRUGU
    return "Mongolian";
  }
  if ((x >= 0x180B) && (x <= 0x180D)) { // Mn   [3] MONGOLIAN FREE VARIATION SELECTOR ONE..MONGOLIAN FREE VARIATION SELECTOR THREE
    return "Mongolian";
  }
  if (x == 0x180E ) {//  Zs       MONGOLIAN VOWEL SEPARATOR
    return "Mongolian";
  }
  if ((x >= 0x1810) && (x <= 0x1819)) { // Nd  [10] MONGOLIAN DIGIT ZERO..MONGOLIAN DIGIT NINE
    return "Mongolian";
  }
  if ((x >= 0x1820) && (x <= 0x1842)) { // Lo  [35] MONGOLIAN LETTER A..MONGOLIAN LETTER CHI
    return "Mongolian";
  }
  if (x == 0x1843 ) {//  Lm       MONGOLIAN LETTER TODO LONG VOWEL SIGN
    return "Mongolian";
  }
  if ((x >= 0x1844) && (x <= 0x1877)) { // Lo  [52] MONGOLIAN LETTER TODO E..MONGOLIAN LETTER MANCHU ZHA
    return "Mongolian";
  }
  if ((x >= 0x1880) && (x <= 0x18A8)) { // Lo  [41] MONGOLIAN LETTER ALI GALI ANUSVARA ONE..MONGOLIAN LETTER MANCHU ALI GALI BHA
    return "Mongolian";
  }
  if (x == 0x18A9 ) {//  Mn       MONGOLIAN LETTER ALI GALI DAGALGA
    return "Mongolian";
  }
  if (x == 0x18AA ) {//  Lo       MONGOLIAN LETTER MANCHU ALI GALI LHA
    return "Mongolian";
  }
  // Total code points: 153
  // ================================================
  if ((x >= 0x3041) && (x <= 0x3096)) { // Lo  [86] HIRAGANA LETTER SMALL A..HIRAGANA LETTER SMALL KE
    return "Hiragana";
  }
  if ((x >= 0x309D) && (x <= 0x309E)) { // Lm   [2] HIRAGANA ITERATION MARK..HIRAGANA VOICED ITERATION MARK
    return "Hiragana";
  }
  if (x == 0x309F ) {//  Lo       HIRAGANA DIGRAPH YORI
    return "Hiragana";
  }
  if (x == 0x1B001 ) {//  Lo       HIRAGANA LETTER ARCHAIC YE
    return "Hiragana";
  }
  if (x == 0x1F200 ) {//  So       SQUARE HIRAGANA HOKA
    return "Hiragana";
  }
  // Total code points: 91
  // ================================================
  if ((x >= 0x30A1) && (x <= 0x30FA)) { // Lo  [90] KATAKANA LETTER SMALL A..KATAKANA LETTER VO
    return "Katakana";
  }
  if ((x >= 0x30FD) && (x <= 0x30FE)) { // Lm   [2] KATAKANA ITERATION MARK..KATAKANA VOICED ITERATION MARK
    return "Katakana";
  }
  if (x == 0x30FF ) {//  Lo       KATAKANA DIGRAPH KOTO
    return "Katakana";
  }
  if ((x >= 0x31F0) && (x <= 0x31FF)) { // Lo  [16] KATAKANA LETTER SMALL KU..KATAKANA LETTER SMALL RO
    return "Katakana";
  }
  if ((x >= 0x32D0) && (x <= 0x32FE)) { // So  [47] CIRCLED KATAKANA A..CIRCLED KATAKANA WO
    return "Katakana";
  }
  if ((x >= 0x3300) && (x <= 0x3357)) { // So  [88] SQUARE APAATO..SQUARE WATTO
    return "Katakana";
  }
  if ((x >= 0xFF66) && (x <= 0xFF6F)) { // Lo  [10] HALFWIDTH KATAKANA LETTER WO..HALFWIDTH KATAKANA LETTER SMALL TU
    return "Katakana";
  }
  if ((x >= 0xFF71) && (x <= 0xFF9D)) { // Lo  [45] HALFWIDTH KATAKANA LETTER A..HALFWIDTH KATAKANA LETTER N
    return "Katakana";
  }
  if (x == 0x1B000 ) {//  Lo       KATAKANA LETTER ARCHAIC E
    return "Katakana";
  }
  // Total code points: 300
  // ================================================
  if ((x >= 0x02EA) && (x <= 0x02EB)) { // Sk   [2] MODIFIER LETTER YIN DEPARTING TONE MARK..MODIFIER LETTER YANG DEPARTING TONE MARK
    return "Bopomofo";
  }
  if ((x >= 0x3105) && (x <= 0x312D)) { // Lo  [41] BOPOMOFO LETTER B..BOPOMOFO LETTER IH
    return "Bopomofo";
  }
  if ((x >= 0x31A0) && (x <= 0x31BA)) { // Lo  [27] BOPOMOFO LETTER BU..BOPOMOFO LETTER ZY
    return "Bopomofo";
  }
  // Total code points: 70
  // ================================================
  if ((x >= 0x2E80) && (x <= 0x2E99)) { // So  [26] CJK RADICAL REPEAT..CJK RADICAL RAP
    return "Han";
  }
  if ((x >= 0x2E9B) && (x <= 0x2EF3)) { // So  [89] CJK RADICAL CHOKE..CJK RADICAL C-SIMPLIFIED TURTLE
    return "Han";
  }
  if ((x >= 0x2F00) && (x <= 0x2FD5)) { // So [214] KANGXI RADICAL ONE..KANGXI RADICAL FLUTE
    return "Han";
  }
  if (x == 0x3005 ) {//  Lm       IDEOGRAPHIC ITERATION MARK
    return "Han";
  }
  if (x == 0x3007 ) {//  Nl       IDEOGRAPHIC NUMBER ZERO
    return "Han";
  }
  if ((x >= 0x3021) && (x <= 0x3029)) { // Nl   [9] HANGZHOU NUMERAL ONE..HANGZHOU NUMERAL NINE
    return "Han";
  }
  if ((x >= 0x3038) && (x <= 0x303A)) { // Nl   [3] HANGZHOU NUMERAL TEN..HANGZHOU NUMERAL THIRTY
    return "Han";
  }
  if (x == 0x303B ) {//  Lm       VERTICAL IDEOGRAPHIC ITERATION MARK
    return "Han";
  }
  if ((x >= 0x3400) && (x <= 0x4DB5)) { // Lo [6582] CJK UNIFIED IDEOGRAPH-3400..CJK UNIFIED IDEOGRAPH-4DB5
    return "Han";
  }
  if ((x >= 0x4E00) && (x <= 0x9FCC)) { // Lo [20941] CJK UNIFIED IDEOGRAPH-4E00..CJK UNIFIED IDEOGRAPH-9FCC
    return "Han";
  }
  if ((x >= 0xF900) && (x <= 0xFA6D)) { // Lo [366] CJK COMPATIBILITY IDEOGRAPH-F900..CJK COMPATIBILITY IDEOGRAPH-FA6D
    return "Han";
  }
  if ((x >= 0xFA70) && (x <= 0xFAD9)) { // Lo [106] CJK COMPATIBILITY IDEOGRAPH-FA70..CJK COMPATIBILITY IDEOGRAPH-FAD9
    return "Han";
  }
  if ((x >= 0x20000) && (x <= 0x2A6D6)) { // Lo [42711] CJK UNIFIED IDEOGRAPH-20000..CJK UNIFIED IDEOGRAPH-2A6D6
    return "Han";
  }
  if ((x >= 0x2A700) && (x <= 0x2B734)) { // Lo [4149] CJK UNIFIED IDEOGRAPH-2A700..CJK UNIFIED IDEOGRAPH-2B734
    return "Han";
  }
  if ((x >= 0x2B740) && (x <= 0x2B81D)) { // Lo [222] CJK UNIFIED IDEOGRAPH-2B740..CJK UNIFIED IDEOGRAPH-2B81D
    return "Han";
  }
  if ((x >= 0x2F800) && (x <= 0x2FA1D)) { // Lo [542] CJK COMPATIBILITY IDEOGRAPH-2F800..CJK COMPATIBILITY IDEOGRAPH-2FA1D
    return "Han";
  }
  // Total code points: 75963
  // ================================================
  if ((x >= 0xA000) && (x <= 0xA014)) { // Lo  [21] YI SYLLABLE IT..YI SYLLABLE E
    return "Yi";
  }
  if (x == 0xA015 ) {//  Lm       YI SYLLABLE WU
    return "Yi";
  }
  if ((x >= 0xA016) && (x <= 0xA48C)) { // Lo [1143] YI SYLLABLE BIT..YI SYLLABLE YYR
    return "Yi";
  }
  if ((x >= 0xA490) && (x <= 0xA4C6)) { // So  [55] YI RADICAL QOT..YI RADICAL KE
    return "Yi";
  }
  // Total code points: 1220
  // ================================================
  if ((x >= 0x10300) && (x <= 0x1031E)) { // Lo  [31] OLD ITALIC LETTER A..OLD ITALIC LETTER UU
    return "Old_Italic";
  }
  if ((x >= 0x10320) && (x <= 0x10323)) { // No   [4] OLD ITALIC NUMERAL ONE..OLD ITALIC NUMERAL FIFTY
    return "Old_Italic";
  }
  // Total code points: 35
  // ================================================
  if ((x >= 0x10330) && (x <= 0x10340)) { // Lo  [17] GOTHIC LETTER AHSA..GOTHIC LETTER PAIRTHRA
    return "Gothic";
  }
  if (x == 0x10341 ) {//  Nl       GOTHIC LETTER NINETY
    return "Gothic";
  }
  if ((x >= 0x10342) && (x <= 0x10349)) { // Lo   [8] GOTHIC LETTER RAIDA..GOTHIC LETTER OTHAL
    return "Gothic";
  }
  if (x == 0x1034A ) {//  Nl       GOTHIC LETTER NINE HUNDRED
    return "Gothic";
  }
  // Total code points: 27
  // ================================================
  if ((x >= 0x10400) && (x <= 0x1044F)) { // L&  [80] DESERET CAPITAL LETTER LONG I..DESERET SMALL LETTER EW
    return "Deseret";
  }
  // Total code points: 80
  // ================================================
  if ((x >= 0x0300) && (x <= 0x036F)) { // Mn [112] COMBINING GRAVE ACCENT..COMBINING LATIN SMALL LETTER X
    return "Inherited";
  }
  if ((x >= 0x0485) && (x <= 0x0486)) { // Mn   [2] COMBINING CYRILLIC DASIA PNEUMATA..COMBINING CYRILLIC PSILI PNEUMATA
    return "Inherited";
  }
  if ((x >= 0x064B) && (x <= 0x0655)) { // Mn  [11] ARABIC FATHATAN..ARABIC HAMZA BELOW
    return "Inherited";
  }
  if (x == 0x0670 ) {//  Mn       ARABIC LETTER SUPERSCRIPT ALEF
    return "Inherited";
  }
  if ((x >= 0x0951) && (x <= 0x0952)) { // Mn   [2] DEVANAGARI STRESS SIGN UDATTA..DEVANAGARI STRESS SIGN ANUDATTA
    return "Inherited";
  }
  if ((x >= 0x1CD0) && (x <= 0x1CD2)) { // Mn   [3] VEDIC TONE KARSHANA..VEDIC TONE PRENKHA
    return "Inherited";
  }
  if ((x >= 0x1CD4) && (x <= 0x1CE0)) { // Mn  [13] VEDIC SIGN YAJURVEDIC MIDLINE SVARITA..VEDIC TONE RIGVEDIC KASHMIRI INDEPENDENT SVARITA
    return "Inherited";
  }
  if ((x >= 0x1CE2) && (x <= 0x1CE8)) { // Mn   [7] VEDIC SIGN VISARGA SVARITA..VEDIC SIGN VISARGA ANUDATTA WITH TAIL
    return "Inherited";
  }
  if (x == 0x1CED ) {//  Mn       VEDIC SIGN TIRYAK
    return "Inherited";
  }
  if (x == 0x1CF4 ) {//  Mn       VEDIC TONE CANDRA ABOVE
    return "Inherited";
  }
  if ((x >= 0x1DC0) && (x <= 0x1DE6)) { // Mn  [39] COMBINING DOTTED GRAVE ACCENT..COMBINING LATIN SMALL LETTER Z
    return "Inherited";
  }
  if ((x >= 0x1DFC) && (x <= 0x1DFF)) { // Mn   [4] COMBINING DOUBLE INVERTED BREVE BELOW..COMBINING RIGHT ARROWHEAD AND DOWN ARROWHEAD BELOW
    return "Inherited";
  }
  if ((x >= 0x200C) && (x <= 0x200D)) { // Cf   [2] ZERO WIDTH NON-JOINER..ZERO WIDTH JOINER
    return "Inherited";
  }
  if ((x >= 0x20D0) && (x <= 0x20DC)) { // Mn  [13] COMBINING LEFT HARPOON ABOVE..COMBINING FOUR DOTS ABOVE
    return "Inherited";
  }
  if ((x >= 0x20DD) && (x <= 0x20E0)) { // Me   [4] COMBINING ENCLOSING CIRCLE..COMBINING ENCLOSING CIRCLE BACKSLASH
    return "Inherited";
  }
  if (x == 0x20E1 ) {//  Mn       COMBINING LEFT RIGHT ARROW ABOVE
    return "Inherited";
  }
  if ((x >= 0x20E2) && (x <= 0x20E4)) { // Me   [3] COMBINING ENCLOSING SCREEN..COMBINING ENCLOSING UPWARD POINTING TRIANGLE
    return "Inherited";
  }
  if ((x >= 0x20E5) && (x <= 0x20F0)) { // Mn  [12] COMBINING REVERSE SOLIDUS OVERLAY..COMBINING ASTERISK ABOVE
    return "Inherited";
  }
  if ((x >= 0x302A) && (x <= 0x302D)) { // Mn   [4] IDEOGRAPHIC LEVEL TONE MARK..IDEOGRAPHIC ENTERING TONE MARK
    return "Inherited";
  }
  if ((x >= 0x3099) && (x <= 0x309A)) { // Mn   [2] COMBINING KATAKANA-HIRAGANA VOICED SOUND MARK..COMBINING KATAKANA-HIRAGANA SEMI-VOICED SOUND MARK
    return "Inherited";
  }
  if ((x >= 0xFE00) && (x <= 0xFE0F)) { // Mn  [16] VARIATION SELECTOR-1..VARIATION SELECTOR-16
    return "Inherited";
  }
  if ((x >= 0xFE20) && (x <= 0xFE26)) { // Mn   [7] COMBINING LIGATURE LEFT HALF..COMBINING CONJOINING MACRON
    return "Inherited";
  }
  if (x == 0x101FD ) {//  Mn       PHAISTOS DISC SIGN COMBINING OBLIQUE STROKE
    return "Inherited";
  }
  if ((x >= 0x1D167) && (x <= 0x1D169)) { // Mn   [3] MUSICAL SYMBOL COMBINING TREMOLO-1..MUSICAL SYMBOL COMBINING TREMOLO-3
    return "Inherited";
  }
  if ((x >= 0x1D17B) && (x <= 0x1D182)) { // Mn   [8] MUSICAL SYMBOL COMBINING ACCENT..MUSICAL SYMBOL COMBINING LOURE
    return "Inherited";
  }
  if ((x >= 0x1D185) && (x <= 0x1D18B)) { // Mn   [7] MUSICAL SYMBOL COMBINING DOIT..MUSICAL SYMBOL COMBINING TRIPLE TONGUE
    return "Inherited";
  }
  if ((x >= 0x1D1AA) && (x <= 0x1D1AD)) { // Mn   [4] MUSICAL SYMBOL COMBINING DOWN BOW..MUSICAL SYMBOL COMBINING SNAP PIZZICATO
    return "Inherited";
  }
  if ((x >= 0xE0100) && (x <= 0xE01EF)) { // Mn [240] VARIATION SELECTOR-17..VARIATION SELECTOR-256
    return "Inherited";
  }
  // Total code points: 523
  // ================================================
  if ((x >= 0x1700) && (x <= 0x170C)) { // Lo  [13] TAGALOG LETTER A..TAGALOG LETTER YA
    return "Tagalog";
  }
  if ((x >= 0x170E) && (x <= 0x1711)) { // Lo   [4] TAGALOG LETTER LA..TAGALOG LETTER HA
    return "Tagalog";
  }
  if ((x >= 0x1712) && (x <= 0x1714)) { // Mn   [3] TAGALOG VOWEL SIGN I..TAGALOG SIGN VIRAMA
    return "Tagalog";
  }
  // Total code points: 20
  // ================================================
  if ((x >= 0x1720) && (x <= 0x1731)) { // Lo  [18] HANUNOO LETTER A..HANUNOO LETTER HA
    return "Hanunoo";
  }
  if ((x >= 0x1732) && (x <= 0x1734)) { // Mn   [3] HANUNOO VOWEL SIGN I..HANUNOO SIGN PAMUDPOD
    return "Hanunoo";
  }
  // Total code points: 21
  // ================================================
  if ((x >= 0x1740) && (x <= 0x1751)) { // Lo  [18] BUHID LETTER A..BUHID LETTER HA
    return "Buhid";
  }
  if ((x >= 0x1752) && (x <= 0x1753)) { // Mn   [2] BUHID VOWEL SIGN I..BUHID VOWEL SIGN U
    return "Buhid";
  }
  // Total code points: 20
  // ================================================
  if ((x >= 0x1760) && (x <= 0x176C)) { // Lo  [13] TAGBANWA LETTER A..TAGBANWA LETTER YA
    return "Tagbanwa";
  }
  if ((x >= 0x176E) && (x <= 0x1770)) { // Lo   [3] TAGBANWA LETTER LA..TAGBANWA LETTER SA
    return "Tagbanwa";
  }
  if ((x >= 0x1772) && (x <= 0x1773)) { // Mn   [2] TAGBANWA VOWEL SIGN I..TAGBANWA VOWEL SIGN U
    return "Tagbanwa";
  }
  // Total code points: 18
  // ================================================
  if ((x >= 0x1900) && (x <= 0x191C)) { // Lo  [29] LIMBU VOWEL-CARRIER LETTER..LIMBU LETTER HA
    return "Limbu";
  }
  if ((x >= 0x1920) && (x <= 0x1922)) { // Mn   [3] LIMBU VOWEL SIGN A..LIMBU VOWEL SIGN U
    return "Limbu";
  }
  if ((x >= 0x1923) && (x <= 0x1926)) { // Mc   [4] LIMBU VOWEL SIGN EE..LIMBU VOWEL SIGN AU
    return "Limbu";
  }
  if ((x >= 0x1927) && (x <= 0x1928)) { // Mn   [2] LIMBU VOWEL SIGN E..LIMBU VOWEL SIGN O
    return "Limbu";
  }
  if ((x >= 0x1929) && (x <= 0x192B)) { // Mc   [3] LIMBU SUBJOINED LETTER YA..LIMBU SUBJOINED LETTER WA
    return "Limbu";
  }
  if ((x >= 0x1930) && (x <= 0x1931)) { // Mc   [2] LIMBU SMALL LETTER KA..LIMBU SMALL LETTER NGA
    return "Limbu";
  }
  if (x == 0x1932 ) {//  Mn       LIMBU SMALL LETTER ANUSVARA
    return "Limbu";
  }
  if ((x >= 0x1933) && (x <= 0x1938)) { // Mc   [6] LIMBU SMALL LETTER TA..LIMBU SMALL LETTER LA
    return "Limbu";
  }
  if ((x >= 0x1939) && (x <= 0x193B)) { // Mn   [3] LIMBU SIGN MUKPHRENG..LIMBU SIGN SA-I
    return "Limbu";
  }
  if (x == 0x1940 ) {//  So       LIMBU SIGN LOO
    return "Limbu";
  }
  if ((x >= 0x1944) && (x <= 0x1945)) { // Po   [2] LIMBU EXCLAMATION MARK..LIMBU QUESTION MARK
    return "Limbu";
  }
  if ((x >= 0x1946) && (x <= 0x194F)) { // Nd  [10] LIMBU DIGIT ZERO..LIMBU DIGIT NINE
    return "Limbu";
  }
  // Total code points: 66
  // ================================================
  if ((x >= 0x1950) && (x <= 0x196D)) { // Lo  [30] TAI LE LETTER KA..TAI LE LETTER AI
    return "Tai_Le";
  }
  if ((x >= 0x1970) && (x <= 0x1974)) { // Lo   [5] TAI LE LETTER TONE-2..TAI LE LETTER TONE-6
    return "Tai_Le";
  }
  // Total code points: 35
  // ================================================
  if ((x >= 0x10000) && (x <= 0x1000B)) { // Lo  [12] LINEAR B SYLLABLE B008 A..LINEAR B SYLLABLE B046 JE
    return "Linear_B";
  }
  if ((x >= 0x1000D) && (x <= 0x10026)) { // Lo  [26] LINEAR B SYLLABLE B036 JO..LINEAR B SYLLABLE B032 QO
    return "Linear_B";
  }
  if ((x >= 0x10028) && (x <= 0x1003A)) { // Lo  [19] LINEAR B SYLLABLE B060 RA..LINEAR B SYLLABLE B042 WO
    return "Linear_B";
  }
  if ((x >= 0x1003C) && (x <= 0x1003D)) { // Lo   [2] LINEAR B SYLLABLE B017 ZA..LINEAR B SYLLABLE B074 ZE
    return "Linear_B";
  }
  if ((x >= 0x1003F) && (x <= 0x1004D)) { // Lo  [15] LINEAR B SYLLABLE B020 ZO..LINEAR B SYLLABLE B091 TWO
    return "Linear_B";
  }
  if ((x >= 0x10050) && (x <= 0x1005D)) { // Lo  [14] LINEAR B SYMBOL B018..LINEAR B SYMBOL B089
    return "Linear_B";
  }
  if ((x >= 0x10080) && (x <= 0x100FA)) { // Lo [123] LINEAR B IDEOGRAM B100 MAN..LINEAR B IDEOGRAM VESSEL B305
    return "Linear_B";
  }
  // Total code points: 211
  // ================================================
  if ((x >= 0x10380) && (x <= 0x1039D)) { // Lo  [30] UGARITIC LETTER ALPA..UGARITIC LETTER SSU
    return "Ugaritic";
  }
  if (x == 0x1039F ) {//  Po       UGARITIC WORD DIVIDER
    return "Ugaritic";
  }
  // Total code points: 31
  // ================================================
  if ((x >= 0x10450) && (x <= 0x1047F)) { // Lo  [48] SHAVIAN LETTER PEEP..SHAVIAN LETTER YEW
    return "Shavian";
  }
  // Total code points: 48
  // ================================================
  if ((x >= 0x10480) && (x <= 0x1049D)) { // Lo  [30] OSMANYA LETTER ALEF..OSMANYA LETTER OO
    return "Osmanya";
  }
  if ((x >= 0x104A0) && (x <= 0x104A9)) { // Nd  [10] OSMANYA DIGIT ZERO..OSMANYA DIGIT NINE
    return "Osmanya";
  }
  // Total code points: 40
  // ================================================
  if ((x >= 0x10800) && (x <= 0x10805)) { // Lo   [6] CYPRIOT SYLLABLE A..CYPRIOT SYLLABLE JA
    return "Cypriot";
  }
  if (x == 0x10808 ) {//  Lo       CYPRIOT SYLLABLE JO
    return "Cypriot";
  }
  if ((x >= 0x1080A) && (x <= 0x10835)) { // Lo  [44] CYPRIOT SYLLABLE KA..CYPRIOT SYLLABLE WO
    return "Cypriot";
  }
  if ((x >= 0x10837) && (x <= 0x10838)) { // Lo   [2] CYPRIOT SYLLABLE XA..CYPRIOT SYLLABLE XE
    return "Cypriot";
  }
  if (x == 0x1083C ) {//  Lo       CYPRIOT SYLLABLE ZA
    return "Cypriot";
  }
  if (x == 0x1083F ) {//  Lo       CYPRIOT SYLLABLE ZO
    return "Cypriot";
  }
  // Total code points: 55
  // ================================================
  if ((x >= 0x2800) && (x <= 0x28FF)) { // So [256] BRAILLE PATTERN BLANK..BRAILLE PATTERN DOTS-12345678
    return "Braille";
  }
  // Total code points: 256
  // ================================================
  if ((x >= 0x1A00) && (x <= 0x1A16)) { // Lo  [23] BUGINESE LETTER KA..BUGINESE LETTER HA
    return "Buginese";
  }
  if ((x >= 0x1A17) && (x <= 0x1A18)) { // Mn   [2] BUGINESE VOWEL SIGN I..BUGINESE VOWEL SIGN U
    return "Buginese";
  }
  if ((x >= 0x1A19) && (x <= 0x1A1B)) { // Mc   [3] BUGINESE VOWEL SIGN E..BUGINESE VOWEL SIGN AE
    return "Buginese";
  }
  if ((x >= 0x1A1E) && (x <= 0x1A1F)) { // Po   [2] BUGINESE PALLAWA..BUGINESE END OF SECTION
    return "Buginese";
  }
  // Total code points: 30
  // ================================================
  if ((x >= 0x03E2) && (x <= 0x03EF)) { // L&  [14] COPTIC CAPITAL LETTER SHEI..COPTIC SMALL LETTER DEI
    return "Coptic";
  }
  if ((x >= 0x2C80) && (x <= 0x2CE4)) { // L& [101] COPTIC CAPITAL LETTER ALFA..COPTIC SYMBOL KAI
    return "Coptic";
  }
  if ((x >= 0x2CE5) && (x <= 0x2CEA)) { // So   [6] COPTIC SYMBOL MI RO..COPTIC SYMBOL SHIMA SIMA
    return "Coptic";
  }
  if ((x >= 0x2CEB) && (x <= 0x2CEE)) { // L&   [4] COPTIC CAPITAL LETTER CRYPTOGRAMMIC SHEI..COPTIC SMALL LETTER CRYPTOGRAMMIC GANGIA
    return "Coptic";
  }
  if ((x >= 0x2CEF) && (x <= 0x2CF1)) { // Mn   [3] COPTIC COMBINING NI ABOVE..COPTIC COMBINING SPIRITUS LENIS
    return "Coptic";
  }
  if ((x >= 0x2CF2) && (x <= 0x2CF3)) { // L&   [2] COPTIC CAPITAL LETTER BOHAIRIC KHEI..COPTIC SMALL LETTER BOHAIRIC KHEI
    return "Coptic";
  }
  if ((x >= 0x2CF9) && (x <= 0x2CFC)) { // Po   [4] COPTIC OLD NUBIAN FULL STOP..COPTIC OLD NUBIAN VERSE DIVIDER
    return "Coptic";
  }
  if (x == 0x2CFD ) {//  No       COPTIC FRACTION ONE HALF
    return "Coptic";
  }
  if ((x >= 0x2CFE) && (x <= 0x2CFF)) { // Po   [2] COPTIC FULL STOP..COPTIC MORPHOLOGICAL DIVIDER
    return "Coptic";
  }
  // Total code points: 137
  // ================================================
  if ((x >= 0x1980) && (x <= 0x19AB)) { // Lo  [44] NEW TAI LUE LETTER HIGH QA..NEW TAI LUE LETTER LOW SUA
    return "New_Tai_Lue";
  }
  if ((x >= 0x19B0) && (x <= 0x19C0)) { // Mc  [17] NEW TAI LUE VOWEL SIGN VOWEL SHORTENER..NEW TAI LUE VOWEL SIGN IY
    return "New_Tai_Lue";
  }
  if ((x >= 0x19C1) && (x <= 0x19C7)) { // Lo   [7] NEW TAI LUE LETTER FINAL V..NEW TAI LUE LETTER FINAL B
    return "New_Tai_Lue";
  }
  if ((x >= 0x19C8) && (x <= 0x19C9)) { // Mc   [2] NEW TAI LUE TONE MARK-1..NEW TAI LUE TONE MARK-2
    return "New_Tai_Lue";
  }
  if ((x >= 0x19D0) && (x <= 0x19D9)) { // Nd  [10] NEW TAI LUE DIGIT ZERO..NEW TAI LUE DIGIT NINE
    return "New_Tai_Lue";
  }
  if (x == 0x19DA ) {//  No       NEW TAI LUE THAM DIGIT ONE
    return "New_Tai_Lue";
  }
  if ((x >= 0x19DE) && (x <= 0x19DF)) { // So   [2] NEW TAI LUE SIGN LAE..NEW TAI LUE SIGN LAEV
    return "New_Tai_Lue";
  }
  // Total code points: 83
  // ================================================
  if ((x >= 0x2C00) && (x <= 0x2C2E)) { // L&  [47] GLAGOLITIC CAPITAL LETTER AZU..GLAGOLITIC CAPITAL LETTER LATINATE MYSLITE
    return "Glagolitic";
  }
  if ((x >= 0x2C30) && (x <= 0x2C5E)) { // L&  [47] GLAGOLITIC SMALL LETTER AZU..GLAGOLITIC SMALL LETTER LATINATE MYSLITE
    return "Glagolitic";
  }
  // Total code points: 94
  // ================================================
  if ((x >= 0x2D30) && (x <= 0x2D67)) { // Lo  [56] TIFINAGH LETTER YA..TIFINAGH LETTER YO
    return "Tifinagh";
  }
  if (x == 0x2D6F ) {//  Lm       TIFINAGH MODIFIER LETTER LABIALIZATION MARK
    return "Tifinagh";
  }
  if (x == 0x2D70 ) {//  Po       TIFINAGH SEPARATOR MARK
    return "Tifinagh";
  }
  if (x == 0x2D7F ) {//  Mn       TIFINAGH CONSONANT JOINER
    return "Tifinagh";
  }
  // Total code points: 59
  // ================================================
  if ((x >= 0xA800) && (x <= 0xA801)) { // Lo   [2] SYLOTI NAGRI LETTER A..SYLOTI NAGRI LETTER I
    return "Syloti_Nagri";
  }
  if (x == 0xA802 ) {//  Mn       SYLOTI NAGRI SIGN DVISVARA
    return "Syloti_Nagri";
  }
  if ((x >= 0xA803) && (x <= 0xA805)) { // Lo   [3] SYLOTI NAGRI LETTER U..SYLOTI NAGRI LETTER O
    return "Syloti_Nagri";
  }
  if (x == 0xA806 ) {//  Mn       SYLOTI NAGRI SIGN HASANTA
    return "Syloti_Nagri";
  }
  if ((x >= 0xA807) && (x <= 0xA80A)) { // Lo   [4] SYLOTI NAGRI LETTER KO..SYLOTI NAGRI LETTER GHO
    return "Syloti_Nagri";
  }
  if (x == 0xA80B ) {//  Mn       SYLOTI NAGRI SIGN ANUSVARA
    return "Syloti_Nagri";
  }
  if ((x >= 0xA80C) && (x <= 0xA822)) { // Lo  [23] SYLOTI NAGRI LETTER CO..SYLOTI NAGRI LETTER HO
    return "Syloti_Nagri";
  }
  if ((x >= 0xA823) && (x <= 0xA824)) { // Mc   [2] SYLOTI NAGRI VOWEL SIGN A..SYLOTI NAGRI VOWEL SIGN I
    return "Syloti_Nagri";
  }
  if ((x >= 0xA825) && (x <= 0xA826)) { // Mn   [2] SYLOTI NAGRI VOWEL SIGN U..SYLOTI NAGRI VOWEL SIGN E
    return "Syloti_Nagri";
  }
  if (x == 0xA827 ) {//  Mc       SYLOTI NAGRI VOWEL SIGN OO
    return "Syloti_Nagri";
  }
  if ((x >= 0xA828) && (x <= 0xA82B)) { // So   [4] SYLOTI NAGRI POETRY MARK-1..SYLOTI NAGRI POETRY MARK-4
    return "Syloti_Nagri";
  }
  // Total code points: 44
  // ================================================
  if ((x >= 0x103A0) && (x <= 0x103C3)) { // Lo  [36] OLD PERSIAN SIGN A..OLD PERSIAN SIGN HA
    return "Old_Persian";
  }
  if ((x >= 0x103C8) && (x <= 0x103CF)) { // Lo   [8] OLD PERSIAN SIGN AURAMAZDAA..OLD PERSIAN SIGN BUUMISH
    return "Old_Persian";
  }
  if (x == 0x103D0 ) {//  Po       OLD PERSIAN WORD DIVIDER
    return "Old_Persian";
  }
  if ((x >= 0x103D1) && (x <= 0x103D5)) { // Nl   [5] OLD PERSIAN NUMBER ONE..OLD PERSIAN NUMBER HUNDRED
    return "Old_Persian";
  }
  // Total code points: 50
  // ================================================
  if (x == 0x10A00 ) {//  Lo       KHAROSHTHI LETTER A
    return "Kharoshthi";
  }
  if ((x >= 0x10A01) && (x <= 0x10A03)) { // Mn   [3] KHAROSHTHI VOWEL SIGN I..KHAROSHTHI VOWEL SIGN VOCALIC R
    return "Kharoshthi";
  }
  if ((x >= 0x10A05) && (x <= 0x10A06)) { // Mn   [2] KHAROSHTHI VOWEL SIGN E..KHAROSHTHI VOWEL SIGN O
    return "Kharoshthi";
  }
  if ((x >= 0x10A0C) && (x <= 0x10A0F)) { // Mn   [4] KHAROSHTHI VOWEL LENGTH MARK..KHAROSHTHI SIGN VISARGA
    return "Kharoshthi";
  }
  if ((x >= 0x10A10) && (x <= 0x10A13)) { // Lo   [4] KHAROSHTHI LETTER KA..KHAROSHTHI LETTER GHA
    return "Kharoshthi";
  }
  if ((x >= 0x10A15) && (x <= 0x10A17)) { // Lo   [3] KHAROSHTHI LETTER CA..KHAROSHTHI LETTER JA
    return "Kharoshthi";
  }
  if ((x >= 0x10A19) && (x <= 0x10A33)) { // Lo  [27] KHAROSHTHI LETTER NYA..KHAROSHTHI LETTER TTTHA
    return "Kharoshthi";
  }
  if ((x >= 0x10A38) && (x <= 0x10A3A)) { // Mn   [3] KHAROSHTHI SIGN BAR ABOVE..KHAROSHTHI SIGN DOT BELOW
    return "Kharoshthi";
  }
  if (x == 0x10A3F ) {//  Mn       KHAROSHTHI VIRAMA
    return "Kharoshthi";
  }
  if ((x >= 0x10A40) && (x <= 0x10A47)) { // No   [8] KHAROSHTHI DIGIT ONE..KHAROSHTHI NUMBER ONE THOUSAND
    return "Kharoshthi";
  }
  if ((x >= 0x10A50) && (x <= 0x10A58)) { // Po   [9] KHAROSHTHI PUNCTUATION DOT..KHAROSHTHI PUNCTUATION LINES
    return "Kharoshthi";
  }
  // Total code points: 65
  // ================================================
  if ((x >= 0x1B00) && (x <= 0x1B03)) { // Mn   [4] BALINESE SIGN ULU RICEM..BALINESE SIGN SURANG
    return "Balinese";
  }
  if (x == 0x1B04 ) {//  Mc       BALINESE SIGN BISAH
    return "Balinese";
  }
  if ((x >= 0x1B05) && (x <= 0x1B33)) { // Lo  [47] BALINESE LETTER AKARA..BALINESE LETTER HA
    return "Balinese";
  }
  if (x == 0x1B34 ) {//  Mn       BALINESE SIGN REREKAN
    return "Balinese";
  }
  if (x == 0x1B35 ) {//  Mc       BALINESE VOWEL SIGN TEDUNG
    return "Balinese";
  }
  if ((x >= 0x1B36) && (x <= 0x1B3A)) { // Mn   [5] BALINESE VOWEL SIGN ULU..BALINESE VOWEL SIGN RA REPA
    return "Balinese";
  }
  if (x == 0x1B3B ) {//  Mc       BALINESE VOWEL SIGN RA REPA TEDUNG
    return "Balinese";
  }
  if (x == 0x1B3C ) {//  Mn       BALINESE VOWEL SIGN LA LENGA
    return "Balinese";
  }
  if ((x >= 0x1B3D) && (x <= 0x1B41)) { // Mc   [5] BALINESE VOWEL SIGN LA LENGA TEDUNG..BALINESE VOWEL SIGN TALING REPA TEDUNG
    return "Balinese";
  }
  if (x == 0x1B42 ) {//  Mn       BALINESE VOWEL SIGN PEPET
    return "Balinese";
  }
  if ((x >= 0x1B43) && (x <= 0x1B44)) { // Mc   [2] BALINESE VOWEL SIGN PEPET TEDUNG..BALINESE ADEG ADEG
    return "Balinese";
  }
  if ((x >= 0x1B45) && (x <= 0x1B4B)) { // Lo   [7] BALINESE LETTER KAF SASAK..BALINESE LETTER ASYURA SASAK
    return "Balinese";
  }
  if ((x >= 0x1B50) && (x <= 0x1B59)) { // Nd  [10] BALINESE DIGIT ZERO..BALINESE DIGIT NINE
    return "Balinese";
  }
  if ((x >= 0x1B5A) && (x <= 0x1B60)) { // Po   [7] BALINESE PANTI..BALINESE PAMENENG
    return "Balinese";
  }
  if ((x >= 0x1B61) && (x <= 0x1B6A)) { // So  [10] BALINESE MUSICAL SYMBOL DONG..BALINESE MUSICAL SYMBOL DANG GEDE
    return "Balinese";
  }
  if ((x >= 0x1B6B) && (x <= 0x1B73)) { // Mn   [9] BALINESE MUSICAL SYMBOL COMBINING TEGEH..BALINESE MUSICAL SYMBOL COMBINING GONG
    return "Balinese";
  }
  if ((x >= 0x1B74) && (x <= 0x1B7C)) { // So   [9] BALINESE MUSICAL SYMBOL RIGHT-HAND OPEN DUG..BALINESE MUSICAL SYMBOL LEFT-HAND OPEN PING
    return "Balinese";
  }
  // Total code points: 121
  // ================================================
  if ((x >= 0x12000) && (x <= 0x1236E)) { // Lo [879] CUNEIFORM SIGN A..CUNEIFORM SIGN ZUM
    return "Cuneiform";
  }
  if ((x >= 0x12400) && (x <= 0x12462)) { // Nl  [99] CUNEIFORM NUMERIC SIGN TWO ASH..CUNEIFORM NUMERIC SIGN OLD ASSYRIAN ONE QUARTER
    return "Cuneiform";
  }
  if ((x >= 0x12470) && (x <= 0x12473)) { // Po   [4] CUNEIFORM PUNCTUATION SIGN OLD ASSYRIAN WORD DIVIDER..CUNEIFORM PUNCTUATION SIGN DIAGONAL TRICOLON
    return "Cuneiform";
  }
  // Total code points: 982
  // ================================================
  if ((x >= 0x10900) && (x <= 0x10915)) { // Lo  [22] PHOENICIAN LETTER ALF..PHOENICIAN LETTER TAU
    return "Phoenician";
  }
  if ((x >= 0x10916) && (x <= 0x1091B)) { // No   [6] PHOENICIAN NUMBER ONE..PHOENICIAN NUMBER THREE
    return "Phoenician";
  }
  if (x == 0x1091F ) {//  Po       PHOENICIAN WORD SEPARATOR
    return "Phoenician";
  }
  // Total code points: 29
  // ================================================
  if ((x >= 0xA840) && (x <= 0xA873)) { // Lo  [52] PHAGS-PA LETTER KA..PHAGS-PA LETTER CANDRABINDU
    return "Phags_Pa";
  }
  if ((x >= 0xA874) && (x <= 0xA877)) { // Po   [4] PHAGS-PA SINGLE HEAD MARK..PHAGS-PA MARK DOUBLE SHAD
    return "Phags_Pa";
  }
  // Total code points: 56
  // ================================================
  if ((x >= 0x07C0) && (x <= 0x07C9)) { // Nd  [10] NKO DIGIT ZERO..NKO DIGIT NINE
    return "Nko";
  }
  if ((x >= 0x07CA) && (x <= 0x07EA)) { // Lo  [33] NKO LETTER A..NKO LETTER JONA RA
    return "Nko";
  }
  if ((x >= 0x07EB) && (x <= 0x07F3)) { // Mn   [9] NKO COMBINING SHORT HIGH TONE..NKO COMBINING DOUBLE DOT ABOVE
    return "Nko";
  }
  if ((x >= 0x07F4) && (x <= 0x07F5)) { // Lm   [2] NKO HIGH TONE APOSTROPHE..NKO LOW TONE APOSTROPHE
    return "Nko";
  }
  if (x == 0x07F6 ) {//  So       NKO SYMBOL OO DENNEN
    return "Nko";
  }
  if ((x >= 0x07F7) && (x <= 0x07F9)) { // Po   [3] NKO SYMBOL GBAKURUNEN..NKO EXCLAMATION MARK
    return "Nko";
  }
  if (x == 0x07FA ) {//  Lm       NKO LAJANYALAN
    return "Nko";
  }
  // Total code points: 59
  // ================================================
  if ((x >= 0x1B80) && (x <= 0x1B81)) { // Mn   [2] SUNDANESE SIGN PANYECEK..SUNDANESE SIGN PANGLAYAR
    return "Sundanese";
  }
  if (x == 0x1B82 ) {//  Mc       SUNDANESE SIGN PANGWISAD
    return "Sundanese";
  }
  if ((x >= 0x1B83) && (x <= 0x1BA0)) { // Lo  [30] SUNDANESE LETTER A..SUNDANESE LETTER HA
    return "Sundanese";
  }
  if (x == 0x1BA1 ) {//  Mc       SUNDANESE CONSONANT SIGN PAMINGKAL
    return "Sundanese";
  }
  if ((x >= 0x1BA2) && (x <= 0x1BA5)) { // Mn   [4] SUNDANESE CONSONANT SIGN PANYAKRA..SUNDANESE VOWEL SIGN PANYUKU
    return "Sundanese";
  }
  if ((x >= 0x1BA6) && (x <= 0x1BA7)) { // Mc   [2] SUNDANESE VOWEL SIGN PANAELAENG..SUNDANESE VOWEL SIGN PANOLONG
    return "Sundanese";
  }
  if ((x >= 0x1BA8) && (x <= 0x1BA9)) { // Mn   [2] SUNDANESE VOWEL SIGN PAMEPET..SUNDANESE VOWEL SIGN PANEULEUNG
    return "Sundanese";
  }
  if (x == 0x1BAA ) {//  Mc       SUNDANESE SIGN PAMAAEH
    return "Sundanese";
  }
  if (x == 0x1BAB ) {//  Mn       SUNDANESE SIGN VIRAMA
    return "Sundanese";
  }
  if ((x >= 0x1BAC) && (x <= 0x1BAD)) { // Mc   [2] SUNDANESE CONSONANT SIGN PASANGAN MA..SUNDANESE CONSONANT SIGN PASANGAN WA
    return "Sundanese";
  }
  if ((x >= 0x1BAE) && (x <= 0x1BAF)) { // Lo   [2] SUNDANESE LETTER KHA..SUNDANESE LETTER SYA
    return "Sundanese";
  }
  if ((x >= 0x1BB0) && (x <= 0x1BB9)) { // Nd  [10] SUNDANESE DIGIT ZERO..SUNDANESE DIGIT NINE
    return "Sundanese";
  }
  if ((x >= 0x1BBA) && (x <= 0x1BBF)) { // Lo   [6] SUNDANESE AVAGRAHA..SUNDANESE LETTER FINAL M
    return "Sundanese";
  }
  if ((x >= 0x1CC0) && (x <= 0x1CC7)) { // Po   [8] SUNDANESE PUNCTUATION BINDU SURYA..SUNDANESE PUNCTUATION BINDU BA SATANGA
    return "Sundanese";
  }
  // Total code points: 72
  // ================================================
  if ((x >= 0x1C00) && (x <= 0x1C23)) { // Lo  [36] LEPCHA LETTER KA..LEPCHA LETTER A
    return "Lepcha";
  }
  if ((x >= 0x1C24) && (x <= 0x1C2B)) { // Mc   [8] LEPCHA SUBJOINED LETTER YA..LEPCHA VOWEL SIGN UU
    return "Lepcha";
  }
  if ((x >= 0x1C2C) && (x <= 0x1C33)) { // Mn   [8] LEPCHA VOWEL SIGN E..LEPCHA CONSONANT SIGN T
    return "Lepcha";
  }
  if ((x >= 0x1C34) && (x <= 0x1C35)) { // Mc   [2] LEPCHA CONSONANT SIGN NYIN-DO..LEPCHA CONSONANT SIGN KANG
    return "Lepcha";
  }
  if ((x >= 0x1C36) && (x <= 0x1C37)) { // Mn   [2] LEPCHA SIGN RAN..LEPCHA SIGN NUKTA
    return "Lepcha";
  }
  if ((x >= 0x1C3B) && (x <= 0x1C3F)) { // Po   [5] LEPCHA PUNCTUATION TA-ROL..LEPCHA PUNCTUATION TSHOOK
    return "Lepcha";
  }
  if ((x >= 0x1C40) && (x <= 0x1C49)) { // Nd  [10] LEPCHA DIGIT ZERO..LEPCHA DIGIT NINE
    return "Lepcha";
  }
  if ((x >= 0x1C4D) && (x <= 0x1C4F)) { // Lo   [3] LEPCHA LETTER TTA..LEPCHA LETTER DDA
    return "Lepcha";
  }
  // Total code points: 74
  // ================================================
  if ((x >= 0x1C50) && (x <= 0x1C59)) { // Nd  [10] OL CHIKI DIGIT ZERO..OL CHIKI DIGIT NINE
    return "Ol_Chiki";
  }
  if ((x >= 0x1C5A) && (x <= 0x1C77)) { // Lo  [30] OL CHIKI LETTER LA..OL CHIKI LETTER OH
    return "Ol_Chiki";
  }
  if ((x >= 0x1C78) && (x <= 0x1C7D)) { // Lm   [6] OL CHIKI MU TTUDDAG..OL CHIKI AHAD
    return "Ol_Chiki";
  }
  if ((x >= 0x1C7E) && (x <= 0x1C7F)) { // Po   [2] OL CHIKI PUNCTUATION MUCAAD..OL CHIKI PUNCTUATION DOUBLE MUCAAD
    return "Ol_Chiki";
  }
  // Total code points: 48
  // ================================================
  if ((x >= 0xA500) && (x <= 0xA60B)) { // Lo [268] VAI SYLLABLE EE..VAI SYLLABLE NG
    return "Vai";
  }
  if (x == 0xA60C ) {//  Lm       VAI SYLLABLE LENGTHENER
    return "Vai";
  }
  if ((x >= 0xA60D) && (x <= 0xA60F)) { // Po   [3] VAI COMMA..VAI QUESTION MARK
    return "Vai";
  }
  if ((x >= 0xA610) && (x <= 0xA61F)) { // Lo  [16] VAI SYLLABLE NDOLE FA..VAI SYMBOL JONG
    return "Vai";
  }
  if ((x >= 0xA620) && (x <= 0xA629)) { // Nd  [10] VAI DIGIT ZERO..VAI DIGIT NINE
    return "Vai";
  }
  if ((x >= 0xA62A) && (x <= 0xA62B)) { // Lo   [2] VAI SYLLABLE NDOLE MA..VAI SYLLABLE NDOLE DO
    return "Vai";
  }
  // Total code points: 300
  // ================================================
  if ((x >= 0xA880) && (x <= 0xA881)) { // Mc   [2] SAURASHTRA SIGN ANUSVARA..SAURASHTRA SIGN VISARGA
    return "Saurashtra";
  }
  if ((x >= 0xA882) && (x <= 0xA8B3)) { // Lo  [50] SAURASHTRA LETTER A..SAURASHTRA LETTER LLA
    return "Saurashtra";
  }
  if ((x >= 0xA8B4) && (x <= 0xA8C3)) { // Mc  [16] SAURASHTRA CONSONANT SIGN HAARU..SAURASHTRA VOWEL SIGN AU
    return "Saurashtra";
  }
  if (x == 0xA8C4 ) {//  Mn       SAURASHTRA SIGN VIRAMA
    return "Saurashtra";
  }
  if ((x >= 0xA8CE) && (x <= 0xA8CF)) { // Po   [2] SAURASHTRA DANDA..SAURASHTRA DOUBLE DANDA
    return "Saurashtra";
  }
  if ((x >= 0xA8D0) && (x <= 0xA8D9)) { // Nd  [10] SAURASHTRA DIGIT ZERO..SAURASHTRA DIGIT NINE
    return "Saurashtra";
  }
  // Total code points: 81
  // ================================================
  if ((x >= 0xA900) && (x <= 0xA909)) { // Nd  [10] KAYAH LI DIGIT ZERO..KAYAH LI DIGIT NINE
    return "Kayah_Li";
  }
  if ((x >= 0xA90A) && (x <= 0xA925)) { // Lo  [28] KAYAH LI LETTER KA..KAYAH LI LETTER OO
    return "Kayah_Li";
  }
  if ((x >= 0xA926) && (x <= 0xA92D)) { // Mn   [8] KAYAH LI VOWEL UE..KAYAH LI TONE CALYA PLOPHU
    return "Kayah_Li";
  }
  if ((x >= 0xA92E) && (x <= 0xA92F)) { // Po   [2] KAYAH LI SIGN CWI..KAYAH LI SIGN SHYA
    return "Kayah_Li";
  }
  // Total code points: 48
  // ================================================
  if ((x >= 0xA930) && (x <= 0xA946)) { // Lo  [23] REJANG LETTER KA..REJANG LETTER A
    return "Rejang";
  }
  if ((x >= 0xA947) && (x <= 0xA951)) { // Mn  [11] REJANG VOWEL SIGN I..REJANG CONSONANT SIGN R
    return "Rejang";
  }
  if ((x >= 0xA952) && (x <= 0xA953)) { // Mc   [2] REJANG CONSONANT SIGN H..REJANG VIRAMA
    return "Rejang";
  }
  if (x == 0xA95F ) {//  Po       REJANG SECTION MARK
    return "Rejang";
  }
  // Total code points: 37
  // ================================================
  if ((x >= 0x10280) && (x <= 0x1029C)) { // Lo  [29] LYCIAN LETTER A..LYCIAN LETTER X
    return "Lycian";
  }
  // Total code points: 29
  // ================================================
  if ((x >= 0x102A0) && (x <= 0x102D0)) { // Lo  [49] CARIAN LETTER A..CARIAN LETTER UUU3
    return "Carian";
  }
  // Total code points: 49
  // ================================================
  if ((x >= 0x10920) && (x <= 0x10939)) { // Lo  [26] LYDIAN LETTER A..LYDIAN LETTER C
    return "Lydian";
  }
  if (x == 0x1093F ) {//  Po       LYDIAN TRIANGULAR MARK
    return "Lydian";
  }
  // Total code points: 27
  // ================================================
  if ((x >= 0xAA00) && (x <= 0xAA28)) { // Lo  [41] CHAM LETTER A..CHAM LETTER HA
    return "Cham";
  }
  if ((x >= 0xAA29) && (x <= 0xAA2E)) { // Mn   [6] CHAM VOWEL SIGN AA..CHAM VOWEL SIGN OE
    return "Cham";
  }
  if ((x >= 0xAA2F) && (x <= 0xAA30)) { // Mc   [2] CHAM VOWEL SIGN O..CHAM VOWEL SIGN AI
    return "Cham";
  }
  if ((x >= 0xAA31) && (x <= 0xAA32)) { // Mn   [2] CHAM VOWEL SIGN AU..CHAM VOWEL SIGN UE
    return "Cham";
  }
  if ((x >= 0xAA33) && (x <= 0xAA34)) { // Mc   [2] CHAM CONSONANT SIGN YA..CHAM CONSONANT SIGN RA
    return "Cham";
  }
  if ((x >= 0xAA35) && (x <= 0xAA36)) { // Mn   [2] CHAM CONSONANT SIGN LA..CHAM CONSONANT SIGN WA
    return "Cham";
  }
  if ((x >= 0xAA40) && (x <= 0xAA42)) { // Lo   [3] CHAM LETTER FINAL K..CHAM LETTER FINAL NG
    return "Cham";
  }
  if (x == 0xAA43 ) {//  Mn       CHAM CONSONANT SIGN FINAL NG
    return "Cham";
  }
  if ((x >= 0xAA44) && (x <= 0xAA4B)) { // Lo   [8] CHAM LETTER FINAL CH..CHAM LETTER FINAL SS
    return "Cham";
  }
  if (x == 0xAA4C ) {//  Mn       CHAM CONSONANT SIGN FINAL M
    return "Cham";
  }
  if (x == 0xAA4D ) {//  Mc       CHAM CONSONANT SIGN FINAL H
    return "Cham";
  }
  if ((x >= 0xAA50) && (x <= 0xAA59)) { // Nd  [10] CHAM DIGIT ZERO..CHAM DIGIT NINE
    return "Cham";
  }
  if ((x >= 0xAA5C) && (x <= 0xAA5F)) { // Po   [4] CHAM PUNCTUATION SPIRAL..CHAM PUNCTUATION TRIPLE DANDA
    return "Cham";
  }
  // Total code points: 83
  // ================================================
  if ((x >= 0x1A20) && (x <= 0x1A54)) { // Lo  [53] TAI THAM LETTER HIGH KA..TAI THAM LETTER GREAT SA
    return "Tai_Tham";
  }
  if (x == 0x1A55 ) {//  Mc       TAI THAM CONSONANT SIGN MEDIAL RA
    return "Tai_Tham";
  }
  if (x == 0x1A56 ) {//  Mn       TAI THAM CONSONANT SIGN MEDIAL LA
    return "Tai_Tham";
  }
  if (x == 0x1A57 ) {//  Mc       TAI THAM CONSONANT SIGN LA TANG LAI
    return "Tai_Tham";
  }
  if ((x >= 0x1A58) && (x <= 0x1A5E)) { // Mn   [7] TAI THAM SIGN MAI KANG LAI..TAI THAM CONSONANT SIGN SA
    return "Tai_Tham";
  }
  if (x == 0x1A60 ) {//  Mn       TAI THAM SIGN SAKOT
    return "Tai_Tham";
  }
  if (x == 0x1A61 ) {//  Mc       TAI THAM VOWEL SIGN A
    return "Tai_Tham";
  }
  if (x == 0x1A62 ) {//  Mn       TAI THAM VOWEL SIGN MAI SAT
    return "Tai_Tham";
  }
  if ((x >= 0x1A63) && (x <= 0x1A64)) { // Mc   [2] TAI THAM VOWEL SIGN AA..TAI THAM VOWEL SIGN TALL AA
    return "Tai_Tham";
  }
  if ((x >= 0x1A65) && (x <= 0x1A6C)) { // Mn   [8] TAI THAM VOWEL SIGN I..TAI THAM VOWEL SIGN OA BELOW
    return "Tai_Tham";
  }
  if ((x >= 0x1A6D) && (x <= 0x1A72)) { // Mc   [6] TAI THAM VOWEL SIGN OY..TAI THAM VOWEL SIGN THAM AI
    return "Tai_Tham";
  }
  if ((x >= 0x1A73) && (x <= 0x1A7C)) { // Mn  [10] TAI THAM VOWEL SIGN OA ABOVE..TAI THAM SIGN KHUEN-LUE KARAN
    return "Tai_Tham";
  }
  if (x == 0x1A7F ) {//  Mn       TAI THAM COMBINING CRYPTOGRAMMIC DOT
    return "Tai_Tham";
  }
  if ((x >= 0x1A80) && (x <= 0x1A89)) { // Nd  [10] TAI THAM HORA DIGIT ZERO..TAI THAM HORA DIGIT NINE
    return "Tai_Tham";
  }
  if ((x >= 0x1A90) && (x <= 0x1A99)) { // Nd  [10] TAI THAM THAM DIGIT ZERO..TAI THAM THAM DIGIT NINE
    return "Tai_Tham";
  }
  if ((x >= 0x1AA0) && (x <= 0x1AA6)) { // Po   [7] TAI THAM SIGN WIANG..TAI THAM SIGN REVERSED ROTATED RANA
    return "Tai_Tham";
  }
  if (x == 0x1AA7 ) {//  Lm       TAI THAM SIGN MAI YAMOK
    return "Tai_Tham";
  }
  if ((x >= 0x1AA8) && (x <= 0x1AAD)) { // Po   [6] TAI THAM SIGN KAAN..TAI THAM SIGN CAANG
    return "Tai_Tham";
  }
  // Total code points: 127
  // ================================================
  if ((x >= 0xAA80) && (x <= 0xAAAF)) { // Lo  [48] TAI VIET LETTER LOW KO..TAI VIET LETTER HIGH O
    return "Tai_Viet";
  }
  if (x == 0xAAB0 ) {//  Mn       TAI VIET MAI KANG
    return "Tai_Viet";
  }
  if (x == 0xAAB1 ) {//  Lo       TAI VIET VOWEL AA
    return "Tai_Viet";
  }
  if ((x >= 0xAAB2) && (x <= 0xAAB4)) { // Mn   [3] TAI VIET VOWEL I..TAI VIET VOWEL U
    return "Tai_Viet";
  }
  if ((x >= 0xAAB5) && (x <= 0xAAB6)) { // Lo   [2] TAI VIET VOWEL E..TAI VIET VOWEL O
    return "Tai_Viet";
  }
  if ((x >= 0xAAB7) && (x <= 0xAAB8)) { // Mn   [2] TAI VIET MAI KHIT..TAI VIET VOWEL IA
    return "Tai_Viet";
  }
  if ((x >= 0xAAB9) && (x <= 0xAABD)) { // Lo   [5] TAI VIET VOWEL UEA..TAI VIET VOWEL AN
    return "Tai_Viet";
  }
  if ((x >= 0xAABE) && (x <= 0xAABF)) { // Mn   [2] TAI VIET VOWEL AM..TAI VIET TONE MAI EK
    return "Tai_Viet";
  }
  if (x == 0xAAC0 ) {//  Lo       TAI VIET TONE MAI NUENG
    return "Tai_Viet";
  }
  if (x == 0xAAC1 ) {//  Mn       TAI VIET TONE MAI THO
    return "Tai_Viet";
  }
  if (x == 0xAAC2 ) {//  Lo       TAI VIET TONE MAI SONG
    return "Tai_Viet";
  }
  if ((x >= 0xAADB) && (x <= 0xAADC)) { // Lo   [2] TAI VIET SYMBOL KON..TAI VIET SYMBOL NUENG
    return "Tai_Viet";
  }
  if (x == 0xAADD ) {//  Lm       TAI VIET SYMBOL SAM
    return "Tai_Viet";
  }
  if ((x >= 0xAADE) && (x <= 0xAADF)) { // Po   [2] TAI VIET SYMBOL HO HOI..TAI VIET SYMBOL KOI KOI
    return "Tai_Viet";
  }
  // Total code points: 72
  // ================================================
  if ((x >= 0x10B00) && (x <= 0x10B35)) { // Lo  [54] AVESTAN LETTER A..AVESTAN LETTER HE
    return "Avestan";
  }
  if ((x >= 0x10B39) && (x <= 0x10B3F)) { // Po   [7] AVESTAN ABBREVIATION MARK..LARGE ONE RING OVER TWO RINGS PUNCTUATION
    return "Avestan";
  }
  // Total code points: 61
  // ================================================
  if ((x >= 0x13000) && (x <= 0x1342E)) { // Lo [1071] EGYPTIAN HIEROGLYPH A001..EGYPTIAN HIEROGLYPH AA032
    return "Egyptian_Hieroglyphs";
  }
  // Total code points: 1071
  // ================================================
  if ((x >= 0x0800) && (x <= 0x0815)) { // Lo  [22] SAMARITAN LETTER ALAF..SAMARITAN LETTER TAAF
    return "Samaritan";
  }
  if ((x >= 0x0816) && (x <= 0x0819)) { // Mn   [4] SAMARITAN MARK IN..SAMARITAN MARK DAGESH
    return "Samaritan";
  }
  if (x == 0x081A ) {//  Lm       SAMARITAN MODIFIER LETTER EPENTHETIC YUT
    return "Samaritan";
  }
  if ((x >= 0x081B) && (x <= 0x0823)) { // Mn   [9] SAMARITAN MARK EPENTHETIC YUT..SAMARITAN VOWEL SIGN A
    return "Samaritan";
  }
  if (x == 0x0824 ) {//  Lm       SAMARITAN MODIFIER LETTER SHORT A
    return "Samaritan";
  }
  if ((x >= 0x0825) && (x <= 0x0827)) { // Mn   [3] SAMARITAN VOWEL SIGN SHORT A..SAMARITAN VOWEL SIGN U
    return "Samaritan";
  }
  if (x == 0x0828 ) {//  Lm       SAMARITAN MODIFIER LETTER I
    return "Samaritan";
  }
  if ((x >= 0x0829) && (x <= 0x082D)) { // Mn   [5] SAMARITAN VOWEL SIGN LONG I..SAMARITAN MARK NEQUDAA
    return "Samaritan";
  }
  if ((x >= 0x0830) && (x <= 0x083E)) { // Po  [15] SAMARITAN PUNCTUATION NEQUDAA..SAMARITAN PUNCTUATION ANNAAU
    return "Samaritan";
  }
  // Total code points: 61
  // ================================================
  if ((x >= 0xA4D0) && (x <= 0xA4F7)) { // Lo  [40] LISU LETTER BA..LISU LETTER OE
    return "Lisu";
  }
  if ((x >= 0xA4F8) && (x <= 0xA4FD)) { // Lm   [6] LISU LETTER TONE MYA TI..LISU LETTER TONE MYA JEU
    return "Lisu";
  }
  if ((x >= 0xA4FE) && (x <= 0xA4FF)) { // Po   [2] LISU PUNCTUATION COMMA..LISU PUNCTUATION FULL STOP
    return "Lisu";
  }
  // Total code points: 48
  // ================================================
  if ((x >= 0xA6A0) && (x <= 0xA6E5)) { // Lo  [70] BAMUM LETTER A..BAMUM LETTER KI
    return "Bamum";
  }
  if ((x >= 0xA6E6) && (x <= 0xA6EF)) { // Nl  [10] BAMUM LETTER MO..BAMUM LETTER KOGHOM
    return "Bamum";
  }
  if ((x >= 0xA6F0) && (x <= 0xA6F1)) { // Mn   [2] BAMUM COMBINING MARK KOQNDON..BAMUM COMBINING MARK TUKWENTIS
    return "Bamum";
  }
  if ((x >= 0xA6F2) && (x <= 0xA6F7)) { // Po   [6] BAMUM NJAEMLI..BAMUM QUESTION MARK
    return "Bamum";
  }
  if ((x >= 0x16800) && (x <= 0x16A38)) { // Lo [569] BAMUM LETTER PHASE-A NGKUE MFON..BAMUM LETTER PHASE-F VUEQ
    return "Bamum";
  }
  // Total code points: 657
  // ================================================
  if ((x >= 0xA980) && (x <= 0xA982)) { // Mn   [3] JAVANESE SIGN PANYANGGA..JAVANESE SIGN LAYAR
    return "Javanese";
  }
  if (x == 0xA983 ) {//  Mc       JAVANESE SIGN WIGNYAN
    return "Javanese";
  }
  if ((x >= 0xA984) && (x <= 0xA9B2)) { // Lo  [47] JAVANESE LETTER A..JAVANESE LETTER HA
    return "Javanese";
  }
  if (x == 0xA9B3 ) {//  Mn       JAVANESE SIGN CECAK TELU
    return "Javanese";
  }
  if ((x >= 0xA9B4) && (x <= 0xA9B5)) { // Mc   [2] JAVANESE VOWEL SIGN TARUNG..JAVANESE VOWEL SIGN TOLONG
    return "Javanese";
  }
  if ((x >= 0xA9B6) && (x <= 0xA9B9)) { // Mn   [4] JAVANESE VOWEL SIGN WULU..JAVANESE VOWEL SIGN SUKU MENDUT
    return "Javanese";
  }
  if ((x >= 0xA9BA) && (x <= 0xA9BB)) { // Mc   [2] JAVANESE VOWEL SIGN TALING..JAVANESE VOWEL SIGN DIRGA MURE
    return "Javanese";
  }
  if (x == 0xA9BC ) {//  Mn       JAVANESE VOWEL SIGN PEPET
    return "Javanese";
  }
  if ((x >= 0xA9BD) && (x <= 0xA9C0)) { // Mc   [4] JAVANESE CONSONANT SIGN KERET..JAVANESE PANGKON
    return "Javanese";
  }
  if ((x >= 0xA9C1) && (x <= 0xA9CD)) { // Po  [13] JAVANESE LEFT RERENGGAN..JAVANESE TURNED PADA PISELEH
    return "Javanese";
  }
  if (x == 0xA9CF ) {//  Lm       JAVANESE PANGRANGKEP
    return "Javanese";
  }
  if ((x >= 0xA9D0) && (x <= 0xA9D9)) { // Nd  [10] JAVANESE DIGIT ZERO..JAVANESE DIGIT NINE
    return "Javanese";
  }
  if ((x >= 0xA9DE) && (x <= 0xA9DF)) { // Po   [2] JAVANESE PADA TIRTA TUMETES..JAVANESE PADA ISEN-ISEN
    return "Javanese";
  }
  // Total code points: 91
  // ================================================
  if ((x >= 0xAAE0) && (x <= 0xAAEA)) { // Lo  [11] MEETEI MAYEK LETTER E..MEETEI MAYEK LETTER SSA
    return "Meetei_Mayek";
  }
  if (x == 0xAAEB ) {//  Mc       MEETEI MAYEK VOWEL SIGN II
    return "Meetei_Mayek";
  }
  if ((x >= 0xAAEC) && (x <= 0xAAED)) { // Mn   [2] MEETEI MAYEK VOWEL SIGN UU..MEETEI MAYEK VOWEL SIGN AAI
    return "Meetei_Mayek";
  }
  if ((x >= 0xAAEE) && (x <= 0xAAEF)) { // Mc   [2] MEETEI MAYEK VOWEL SIGN AU..MEETEI MAYEK VOWEL SIGN AAU
    return "Meetei_Mayek";
  }
  if ((x >= 0xAAF0) && (x <= 0xAAF1)) { // Po   [2] MEETEI MAYEK CHEIKHAN..MEETEI MAYEK AHANG KHUDAM
    return "Meetei_Mayek";
  }
  if (x == 0xAAF2 ) {//  Lo       MEETEI MAYEK ANJI
    return "Meetei_Mayek";
  }
  if ((x >= 0xAAF3) && (x <= 0xAAF4)) { // Lm   [2] MEETEI MAYEK SYLLABLE REPETITION MARK..MEETEI MAYEK WORD REPETITION MARK
    return "Meetei_Mayek";
  }
  if (x == 0xAAF5 ) {//  Mc       MEETEI MAYEK VOWEL SIGN VISARGA
    return "Meetei_Mayek";
  }
  if (x == 0xAAF6 ) {//  Mn       MEETEI MAYEK VIRAMA
    return "Meetei_Mayek";
  }
  if ((x >= 0xABC0) && (x <= 0xABE2)) { // Lo  [35] MEETEI MAYEK LETTER KOK..MEETEI MAYEK LETTER I LONSUM
    return "Meetei_Mayek";
  }
  if ((x >= 0xABE3) && (x <= 0xABE4)) { // Mc   [2] MEETEI MAYEK VOWEL SIGN ONAP..MEETEI MAYEK VOWEL SIGN INAP
    return "Meetei_Mayek";
  }
  if (x == 0xABE5 ) {//  Mn       MEETEI MAYEK VOWEL SIGN ANAP
    return "Meetei_Mayek";
  }
  if ((x >= 0xABE6) && (x <= 0xABE7)) { // Mc   [2] MEETEI MAYEK VOWEL SIGN YENAP..MEETEI MAYEK VOWEL SIGN SOUNAP
    return "Meetei_Mayek";
  }
  if (x == 0xABE8 ) {//  Mn       MEETEI MAYEK VOWEL SIGN UNAP
    return "Meetei_Mayek";
  }
  if ((x >= 0xABE9) && (x <= 0xABEA)) { // Mc   [2] MEETEI MAYEK VOWEL SIGN CHEINAP..MEETEI MAYEK VOWEL SIGN NUNG
    return "Meetei_Mayek";
  }
  if (x == 0xABEB ) {//  Po       MEETEI MAYEK CHEIKHEI
    return "Meetei_Mayek";
  }
  if (x == 0xABEC ) {//  Mc       MEETEI MAYEK LUM IYEK
    return "Meetei_Mayek";
  }
  if (x == 0xABED ) {//  Mn       MEETEI MAYEK APUN IYEK
    return "Meetei_Mayek";
  }
  if ((x >= 0xABF0) && (x <= 0xABF9)) { // Nd  [10] MEETEI MAYEK DIGIT ZERO..MEETEI MAYEK DIGIT NINE
    return "Meetei_Mayek";
  }
  // Total code points: 79
  // ================================================
  if ((x >= 0x10840) && (x <= 0x10855)) { // Lo  [22] IMPERIAL ARAMAIC LETTER ALEPH..IMPERIAL ARAMAIC LETTER TAW
    return "Imperial_Aramaic";
  }
  if (x == 0x10857 ) {//  Po       IMPERIAL ARAMAIC SECTION SIGN
    return "Imperial_Aramaic";
  }
  if ((x >= 0x10858) && (x <= 0x1085F)) { // No   [8] IMPERIAL ARAMAIC NUMBER ONE..IMPERIAL ARAMAIC NUMBER TEN THOUSAND
    return "Imperial_Aramaic";
  }
  // Total code points: 31
  // ================================================
  if ((x >= 0x10A60) && (x <= 0x10A7C)) { // Lo  [29] OLD SOUTH ARABIAN LETTER HE..OLD SOUTH ARABIAN LETTER THETH
    return "Old_South_Arabian";
  }
  if ((x >= 0x10A7D) && (x <= 0x10A7E)) { // No   [2] OLD SOUTH ARABIAN NUMBER ONE..OLD SOUTH ARABIAN NUMBER FIFTY
    return "Old_South_Arabian";
  }
  if (x == 0x10A7F ) {//  Po       OLD SOUTH ARABIAN NUMERIC INDICATOR
    return "Old_South_Arabian";
  }
  // Total code points: 32
  // ================================================
  if ((x >= 0x10B40) && (x <= 0x10B55)) { // Lo  [22] INSCRIPTIONAL PARTHIAN LETTER ALEPH..INSCRIPTIONAL PARTHIAN LETTER TAW
    return "Inscriptional_Parthian";
  }
  if ((x >= 0x10B58) && (x <= 0x10B5F)) { // No   [8] INSCRIPTIONAL PARTHIAN NUMBER ONE..INSCRIPTIONAL PARTHIAN NUMBER ONE THOUSAND
    return "Inscriptional_Parthian";
  }
  // Total code points: 30
  // ================================================
  if ((x >= 0x10B60) && (x <= 0x10B72)) { // Lo  [19] INSCRIPTIONAL PAHLAVI LETTER ALEPH..INSCRIPTIONAL PAHLAVI LETTER TAW
    return "Inscriptional_Pahlavi";
  }
  if ((x >= 0x10B78) && (x <= 0x10B7F)) { // No   [8] INSCRIPTIONAL PAHLAVI NUMBER ONE..INSCRIPTIONAL PAHLAVI NUMBER ONE THOUSAND
    return "Inscriptional_Pahlavi";
  }
  // Total code points: 27
  // ================================================
  if ((x >= 0x10C00) && (x <= 0x10C48)) { // Lo  [73] OLD TURKIC LETTER ORKHON A..OLD TURKIC LETTER ORKHON BASH
    return "Old_Turkic";
  }
  // Total code points: 73
  // ================================================
  if ((x >= 0x11080) && (x <= 0x11081)) { // Mn   [2] KAITHI SIGN CANDRABINDU..KAITHI SIGN ANUSVARA
    return "Kaithi";
  }
  if (x == 0x11082 ) {//  Mc       KAITHI SIGN VISARGA
    return "Kaithi";
  }
  if ((x >= 0x11083) && (x <= 0x110AF)) { // Lo  [45] KAITHI LETTER A..KAITHI LETTER HA
    return "Kaithi";
  }
  if ((x >= 0x110B0) && (x <= 0x110B2)) { // Mc   [3] KAITHI VOWEL SIGN AA..KAITHI VOWEL SIGN II
    return "Kaithi";
  }
  if ((x >= 0x110B3) && (x <= 0x110B6)) { // Mn   [4] KAITHI VOWEL SIGN U..KAITHI VOWEL SIGN AI
    return "Kaithi";
  }
  if ((x >= 0x110B7) && (x <= 0x110B8)) { // Mc   [2] KAITHI VOWEL SIGN O..KAITHI VOWEL SIGN AU
    return "Kaithi";
  }
  if ((x >= 0x110B9) && (x <= 0x110BA)) { // Mn   [2] KAITHI SIGN VIRAMA..KAITHI SIGN NUKTA
    return "Kaithi";
  }
  if ((x >= 0x110BB) && (x <= 0x110BC)) { // Po   [2] KAITHI ABBREVIATION SIGN..KAITHI ENUMERATION SIGN
    return "Kaithi";
  }
  if (x == 0x110BD ) {//  Cf       KAITHI NUMBER SIGN
    return "Kaithi";
  }
  if ((x >= 0x110BE) && (x <= 0x110C1)) { // Po   [4] KAITHI SECTION MARK..KAITHI DOUBLE DANDA
    return "Kaithi";
  }
  // Total code points: 66
  // ================================================
  if ((x >= 0x1BC0) && (x <= 0x1BE5)) { // Lo  [38] BATAK LETTER A..BATAK LETTER U
    return "Batak";
  }
  if (x == 0x1BE6 ) {//  Mn       BATAK SIGN TOMPI
    return "Batak";
  }
  if (x == 0x1BE7 ) {//  Mc       BATAK VOWEL SIGN E
    return "Batak";
  }
  if ((x >= 0x1BE8) && (x <= 0x1BE9)) { // Mn   [2] BATAK VOWEL SIGN PAKPAK E..BATAK VOWEL SIGN EE
    return "Batak";
  }
  if ((x >= 0x1BEA) && (x <= 0x1BEC)) { // Mc   [3] BATAK VOWEL SIGN I..BATAK VOWEL SIGN O
    return "Batak";
  }
  if (x == 0x1BED ) {//  Mn       BATAK VOWEL SIGN KARO O
    return "Batak";
  }
  if (x == 0x1BEE ) {//  Mc       BATAK VOWEL SIGN U
    return "Batak";
  }
  if ((x >= 0x1BEF) && (x <= 0x1BF1)) { // Mn   [3] BATAK VOWEL SIGN U FOR SIMALUNGUN SA..BATAK CONSONANT SIGN H
    return "Batak";
  }
  if ((x >= 0x1BF2) && (x <= 0x1BF3)) { // Mc   [2] BATAK PANGOLAT..BATAK PANONGONAN
    return "Batak";
  }
  if ((x >= 0x1BFC) && (x <= 0x1BFF)) { // Po   [4] BATAK SYMBOL BINDU NA METEK..BATAK SYMBOL BINDU PANGOLAT
    return "Batak";
  }
  // Total code points: 56
  // ================================================
  if (x == 0x11000 ) {//  Mc       BRAHMI SIGN CANDRABINDU
    return "Brahmi";
  }
  if (x == 0x11001 ) {//  Mn       BRAHMI SIGN ANUSVARA
    return "Brahmi";
  }
  if (x == 0x11002 ) {//  Mc       BRAHMI SIGN VISARGA
    return "Brahmi";
  }
  if ((x >= 0x11003) && (x <= 0x11037)) { // Lo  [53] BRAHMI SIGN JIHVAMULIYA..BRAHMI LETTER OLD TAMIL NNNA
    return "Brahmi";
  }
  if ((x >= 0x11038) && (x <= 0x11046)) { // Mn  [15] BRAHMI VOWEL SIGN AA..BRAHMI VIRAMA
    return "Brahmi";
  }
  if ((x >= 0x11047) && (x <= 0x1104D)) { // Po   [7] BRAHMI DANDA..BRAHMI PUNCTUATION LOTUS
    return "Brahmi";
  }
  if ((x >= 0x11052) && (x <= 0x11065)) { // No  [20] BRAHMI NUMBER ONE..BRAHMI NUMBER ONE THOUSAND
    return "Brahmi";
  }
  if ((x >= 0x11066) && (x <= 0x1106F)) { // Nd  [10] BRAHMI DIGIT ZERO..BRAHMI DIGIT NINE
    return "Brahmi";
  }
  // Total code points: 108
  // ================================================
  if ((x >= 0x0840) && (x <= 0x0858)) { // Lo  [25] MANDAIC LETTER HALQA..MANDAIC LETTER AIN
    return "Mandaic";
  }
  if ((x >= 0x0859) && (x <= 0x085B)) { // Mn   [3] MANDAIC AFFRICATION MARK..MANDAIC GEMINATION MARK
    return "Mandaic";
  }
  if (x == 0x085E ) {//  Po       MANDAIC PUNCTUATION
    return "Mandaic";
  }
  // Total code points: 29
  // ================================================
  if ((x >= 0x11100) && (x <= 0x11102)) { // Mn   [3] CHAKMA SIGN CANDRABINDU..CHAKMA SIGN VISARGA
    return "Chakma";
  }
  if ((x >= 0x11103) && (x <= 0x11126)) { // Lo  [36] CHAKMA LETTER AA..CHAKMA LETTER HAA
    return "Chakma";
  }
  if ((x >= 0x11127) && (x <= 0x1112B)) { // Mn   [5] CHAKMA VOWEL SIGN A..CHAKMA VOWEL SIGN UU
    return "Chakma";
  }
  if (x == 0x1112C ) {//  Mc       CHAKMA VOWEL SIGN E
    return "Chakma";
  }
  if ((x >= 0x1112D) && (x <= 0x11134)) { // Mn   [8] CHAKMA VOWEL SIGN AI..CHAKMA MAAYYAA
    return "Chakma";
  }
  if ((x >= 0x11136) && (x <= 0x1113F)) { // Nd  [10] CHAKMA DIGIT ZERO..CHAKMA DIGIT NINE
    return "Chakma";
  }
  if ((x >= 0x11140) && (x <= 0x11143)) { // Po   [4] CHAKMA SECTION MARK..CHAKMA QUESTION MARK
    return "Chakma";
  }
  // Total code points: 67
  // ================================================
  if ((x >= 0x109A0) && (x <= 0x109B7)) { // Lo  [24] MEROITIC CURSIVE LETTER A..MEROITIC CURSIVE LETTER DA
    return "Meroitic_Cursive";
  }
  if ((x >= 0x109BE) && (x <= 0x109BF)) { // Lo   [2] MEROITIC CURSIVE LOGOGRAM RMT..MEROITIC CURSIVE LOGOGRAM IMN
    return "Meroitic_Cursive";
  }
  // Total code points: 26
  // ================================================
  if ((x >= 0x10980) && (x <= 0x1099F)) { // Lo  [32] MEROITIC HIEROGLYPHIC LETTER A..MEROITIC HIEROGLYPHIC SYMBOL VIDJ-2
    return "Meroitic_Hieroglyphs";
  }
  // Total code points: 32
  // ================================================
  if ((x >= 0x16F00) && (x <= 0x16F44)) { // Lo  [69] MIAO LETTER PA..MIAO LETTER HHA
    return "Miao";
  }
  if (x == 0x16F50 ) {//  Lo       MIAO LETTER NASALIZATION
    return "Miao";
  }
  if ((x >= 0x16F51) && (x <= 0x16F7E)) { // Mc  [46] MIAO SIGN ASPIRATION..MIAO VOWEL SIGN NG
    return "Miao";
  }
  if ((x >= 0x16F8F) && (x <= 0x16F92)) { // Mn   [4] MIAO TONE RIGHT..MIAO TONE BELOW
    return "Miao";
  }
  if ((x >= 0x16F93) && (x <= 0x16F9F)) { // Lm  [13] MIAO LETTER TONE-2..MIAO LETTER REFORMED TONE-8
    return "Miao";
  }
  // Total code points: 133
  // ================================================
  if ((x >= 0x11180) && (x <= 0x11181)) { // Mn   [2] SHARADA SIGN CANDRABINDU..SHARADA SIGN ANUSVARA
    return "Sharada";
  }
  if (x == 0x11182 ) {//  Mc       SHARADA SIGN VISARGA
    return "Sharada";
  }
  if ((x >= 0x11183) && (x <= 0x111B2)) { // Lo  [48] SHARADA LETTER A..SHARADA LETTER HA
    return "Sharada";
  }
  if ((x >= 0x111B3) && (x <= 0x111B5)) { // Mc   [3] SHARADA VOWEL SIGN AA..SHARADA VOWEL SIGN II
    return "Sharada";
  }
  if ((x >= 0x111B6) && (x <= 0x111BE)) { // Mn   [9] SHARADA VOWEL SIGN U..SHARADA VOWEL SIGN O
    return "Sharada";
  }
  if ((x >= 0x111BF) && (x <= 0x111C0)) { // Mc   [2] SHARADA VOWEL SIGN AU..SHARADA SIGN VIRAMA
    return "Sharada";
  }
  if ((x >= 0x111C1) && (x <= 0x111C4)) { // Lo   [4] SHARADA SIGN AVAGRAHA..SHARADA OM
    return "Sharada";
  }
  if ((x >= 0x111C5) && (x <= 0x111C8)) { // Po   [4] SHARADA DANDA..SHARADA SEPARATOR
    return "Sharada";
  }
  if ((x >= 0x111D0) && (x <= 0x111D9)) { // Nd  [10] SHARADA DIGIT ZERO..SHARADA DIGIT NINE
    return "Sharada";
  }
  // Total code points: 83
  // ================================================
  if ((x >= 0x110D0) && (x <= 0x110E8)) { // Lo  [25] SORA SOMPENG LETTER SAH..SORA SOMPENG LETTER MAE
    return "Sora_Sompeng";
  }
  if ((x >= 0x110F0) && (x <= 0x110F9)) { // Nd  [10] SORA SOMPENG DIGIT ZERO..SORA SOMPENG DIGIT NINE
    return "Sora_Sompeng";
  }
  // Total code points: 35
  // ================================================
  if ((x >= 0x11680) && (x <= 0x116AA)) { // Lo  [43] TAKRI LETTER A..TAKRI LETTER RRA
    return "Takri";
  }
  if (x == 0x116AB ) {//  Mn       TAKRI SIGN ANUSVARA
    return "Takri";
  }
  if (x == 0x116AC ) {//  Mc       TAKRI SIGN VISARGA
    return "Takri";
  }
  if (x == 0x116AD ) {//  Mn       TAKRI VOWEL SIGN AA
    return "Takri";
  }
  if ((x >= 0x116AE) && (x <= 0x116AF)) { // Mc   [2] TAKRI VOWEL SIGN I..TAKRI VOWEL SIGN II
    return "Takri";
  }
  if ((x >= 0x116B0) && (x <= 0x116B5)) { // Mn   [6] TAKRI VOWEL SIGN U..TAKRI VOWEL SIGN AU
    return "Takri";
  }
  if (x == 0x116B6 ) {//  Mc       TAKRI SIGN VIRAMA
    return "Takri";
  }
  if (x == 0x116B7 ) {//  Mn       TAKRI SIGN NUKTA
    return "Takri";
  }
  if ((x >= 0x116C0) && (x <= 0x116C9)) { // Nd  [10] TAKRI DIGIT ZERO..TAKRI DIGIT NINE
    return "Takri";
  }
  // Total code points: 66
  // EOF
  return "Unknown";
}
UcdScripts::UcdScripts(const QString & target) {
  m_target = target;
  m_skipEntities = true;
  m_trim = true;
  m_skip.setPattern("Common|Inherited|Unknown");
  split();
}
/**
 * Returns the first specfic script encountered
 *
 * @param str target string
 *
 * @return the name of script or an empty string
 */
QString UcdScripts::getScript(const QString & str) {
  QString script;
  int sz = str.size();
  for(int i=0;i < sz;i++) {
    QChar c = str.at(i);
    script = UcdScripts::getScript(c.unicode());
    if ((script == "Common") || (script == "Inherited") || (script == "Unknown")) {
      ;
    }
    else {
      return script;
    }
  }
  return script;
}
/**
 * Returns true if each character matches the specific script (ignoring Common etc)
 *
 * @param str target string
 * *param script
 * @return true/false
 */
bool UcdScripts::isScript(const QString & str,const QString & script) {
  bool ok = true;
  int sz = str.size();
  QChar c;
  QString sc;
  QString t = script.toCaseFolded();
  for(int i=0;i < sz;i++) {
    c = str.at(i);
    sc = UcdScripts::getScript(c.unicode());
    if ((sc == "Common") || (sc == "Inherited") || (sc == "Unknown")) {
      ;
    }
    else if (sc.toCaseFolded() != t) {
        return false;
    }
  }
  return ok;
}
/**
 * Returns the first specfic script encountered
 *
 * @param str target string
 *
 * @return the name of script or an empty string
 */
QString UcdScripts::getScript(const QString & str,int * count) {
  QString script;
  QStringList scripts;
  int sc=0;
  int sz = str.size();
  for(int i=0;i < sz;i++) {
    QChar c = str.at(i);
    script = UcdScripts::getScript(c.unicode());
    if ((script == "Common") || (script == "Inherited") || (script == "Unknown")) {
      ;
    }
    else {
      if (! scripts.contains(script)) {
        scripts << script;
        sc++;
      }
    }
  }
  *count = sc;
  return scripts.join(",");
}
QString UcdScripts::getScriptAt(int i) {

  if (i < m_scripts.size() ) {
    return m_scripts[i].first;
  }
  else {
    return QString();
  }
}
QString UcdScripts::getStringAt(int i) {

  if (i < m_scripts.size() ) {
    return m_scripts[i].second;
  }
  else {
    return QString();
  }

}
void UcdScripts::split() {
  QString current;
  int offset = 0;

  QString saveString = m_target;
  if (m_skipEntities) {
    QRegExp rx("&\\w+;");
    m_target = m_target.replace(rx,"");
  }
  for(int i=0;i < m_target.size();i++) {
    QChar c = m_target.at(i);
    QString script = UcdScripts::getScript(c.unicode());
    //    qDebug("%d %d [%s] [%s]", i,c.unicode(), qPrintable(current),qPrintable(script));
    if (! m_skip.exactMatch(script) )
      if (script != current) {
        if (! current.isEmpty()) {
          QString chunk = m_target.mid(offset,i - offset);
          //          qDebug() << current << chunk;
          m_scripts.append(qMakePair(current,chunk.trimmed()));
          offset = i;
        }
        current = script;
      }
      else {
      }
    else {
      //      qDebug() << "skipping";
    }
  }
  //  qDebug() << "adding last chunk" << current << offset;
  QString chunk = m_target.mid(offset);
  //  qDebug() << "got chunk" << chunk;
  if (! chunk.isEmpty()) {
    m_scripts.append(qMakePair(current,chunk.trimmed()));
  }
  m_target = saveString;
}
