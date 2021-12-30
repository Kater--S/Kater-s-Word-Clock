
#include <Arduino.h>

#include "config.h"

static const uint8_t font[] PROGMEM = {
  2, 0b00000000, 0b00000000,                                     /* 032 =   */
  2, 0b01101111, 0b01101111,                                     /* 033 = ! */
  3, 0b00000011, 0b00000000, 0b00000011,                         /* 034 = " */
  5, 0b00010100, 0b01111111, 0b00010100, 0b01111111, 0b00010100, /* 035 = # */
  5, 0b00100100, 0b00101010, 0b01111111, 0b00101010, 0b00010010, /* 036 = $ */
  5, 0b00100011, 0b00010011, 0b00001000, 0b01100100, 0b01100010, /* 037 = % */
  5, 0b00110100, 0b01001010, 0b01001010, 0b00110100, 0b01010000, /* 038 = & */
  1, 0b00000011,                                                 /* 039 = ' */
  3, 0b00011100, 0b00100010, 0b01000001,                         /* 040 = ( */
  3, 0b01000001, 0b00100010, 0b00011100,                         /* 041 = ) */
  3, 0b00000101, 0b00000010, 0b00000101,                         /* 042 = * */
  5, 0b00001000, 0b00001000, 0b00111110, 0b00001000, 0b00001000, /* 043 = + */
  2, 0b11100000, 0b01100000,                                     /* 044 = , */
  3, 0b00001000, 0b00001000, 0b00001000,                         /* 045 = - */
  2, 0b01100000, 0b01100000,                                     /* 046 = . */
  5, 0b01000000, 0b00110000, 0b00001000, 0b00000110, 0b00000001, /* 047 = / */
  5, 0b00111110, 0b01010001, 0b01001001, 0b01000101, 0b00111110, /* 048 = 0 */
  5, 0b00000000, 0b01000010, 0b01111111, 0b01000000, 0b00000000, /* 049 = 1 */
  5, 0b01000010, 0b01100001, 0b01010001, 0b01001001, 0b01000110, /* 050 = 2 */
  5, 0b00100001, 0b01000001, 0b01000101, 0b01001011, 0b00110001, /* 051 = 3 */
  5, 0b00011000, 0b00010100, 0b00010010, 0b01111111, 0b00010000, /* 052 = 4 */
  5, 0b00100111, 0b01000101, 0b01000101, 0b01000101, 0b00111001, /* 053 = 5 */
  5, 0b00111100, 0b01001010, 0b01001001, 0b01001001, 0b00110000, /* 054 = 6 */
  5, 0b00000001, 0b00000001, 0b01111001, 0b00000101, 0b00000011, /* 055 = 7 */
  5, 0b00110110, 0b01001001, 0b01001001, 0b01001001, 0b00110110, /* 056 = 8 */
  5, 0b00000110, 0b01001001, 0b01001001, 0b00101001, 0b00011110, /* 057 = 9 */
  2, 0b00110110, 0b00110110,                                     /* 058 = : */
  2, 0b01110110, 0b00110110,                                     /* 059 = ; */
  4, 0b00001000, 0b00010100, 0b00100010, 0b01000001,             /* 060 = < */
  5, 0b00010100, 0b00010100, 0b00010100, 0b00010100, 0b00010100, /* 061 = = */
  4, 0b01000001, 0b00100010, 0b00010100, 0b00001000,             /* 062 = > */
  5, 0b00000010, 0b00000001, 0b01010001, 0b00001001, 0b00000110, /* 063 = ? */
  5, 0b00111110, 0b01000001, 0b01011101, 0b01010101, 0b01011110, /* 064 = @ */
  5, 0b01111110, 0b00001001, 0b00001001, 0b00001001, 0b01111110, /* 065 = A */
  5, 0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b00110110, /* 066 = B */
  5, 0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00100010, /* 067 = C */
  5, 0b01111111, 0b01000001, 0b01000001, 0b01000001, 0b00111110, /* 068 = D */
  5, 0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b01000001, /* 069 = E */
  5, 0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000001, /* 070 = F */
  5, 0b00111110, 0b01000001, 0b01001001, 0b01001001, 0b01111010, /* 071 = G */
  5, 0b01111111, 0b00001000, 0b00001000, 0b00001000, 0b01111111, /* 072 = H */
  3, 0b01000001, 0b01111111, 0b01000001,                         /* 073 = I */
  5, 0b00100000, 0b01000001, 0b01000001, 0b00111111, 0b00000001, /* 074 = J */
  5, 0b01111111, 0b00001000, 0b00010100, 0b00100010, 0b01000001, /* 075 = K */
  5, 0b01111111, 0b01000000, 0b01000000, 0b01000000, 0b01000000, /* 076 = L */
  5, 0b01111111, 0b00000010, 0b00000100, 0b00000010, 0b01111111, /* 077 = M */
  5, 0b01111111, 0b00000100, 0b00001000, 0b00010000, 0b01111111, /* 078 = N */
  5, 0b00111110, 0b01000001, 0b01000001, 0b01000001, 0b00111110, /* 079 = O */
  5, 0b01111111, 0b00001001, 0b00001001, 0b00001001, 0b00000110, /* 080 = P */
  5, 0b00111110, 0b01000001, 0b01010001, 0b00100001, 0b01011110, /* 081 = Q */
  5, 0b01111111, 0b00001001, 0b00011001, 0b00101001, 0b01000110, /* 082 = R */
  5, 0b00100110, 0b01001001, 0b01001001, 0b01001001, 0b00110010, /* 083 = S */
  5, 0b00000001, 0b00000001, 0b01111111, 0b00000001, 0b00000001, /* 084 = T */
  5, 0b00111111, 0b01000000, 0b01000000, 0b01000000, 0b00111111, /* 085 = U */
  5, 0b00011111, 0b00100000, 0b01000000, 0b00100000, 0b00011111, /* 086 = V */
  5, 0b00111111, 0b01000000, 0b00110000, 0b01000000, 0b00111111, /* 087 = W */
  5, 0b01100011, 0b00010100, 0b00001000, 0b00010100, 0b01100011, /* 088 = X */
  5, 0b00000011, 0b00000100, 0b01111000, 0b00000100, 0b00000011, /* 089 = Y */
  5, 0b01100001, 0b01010001, 0b01001001, 0b01000101, 0b01000011, /* 090 = Z */
  3, 0b01111111, 0b01000001, 0b01000001,                         /* 091 = [ */
  5, 0b00000001, 0b00000110, 0b00001000, 0b00110000, 0b01000000, /* 092 = \ */
  3, 0b01000001, 0b01000001, 0b01111111,                         /* 093 = ] */
  5, 0b00000100, 0b00000010, 0b00000001, 0b00000010, 0b00000100, /* 094 = ^ */
  5, 0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000, /* 095 = _ */
  1, 0b00000011,                                                 /* 096 = ' */
  5, 0b00100000, 0b01010100, 0b01010100, 0b01010100, 0b01111000, /* 097 = a */
  5, 0b01111111, 0b00101000, 0b01000100, 0b01000100, 0b00111000, /* 098 = b */
  5, 0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00101000, /* 099 = c */
  5, 0b00111000, 0b01000100, 0b01000100, 0b00101000, 0b01111111, /* 100 = d */
  5, 0b00111000, 0b01010100, 0b01010100, 0b01010100, 0b00011000, /* 101 = e */
  4, 0b00000100, 0b01111110, 0b00000101, 0b00000001,             /* 102 = f */
  5, 0b00011000, 0b10100100, 0b10100100, 0b10100100, 0b01111100, /* 103 = g */
  5, 0b01111111, 0b00000100, 0b00000100, 0b00000100, 0b01111000, /* 104 = h */
  3, 0b01000100, 0b01111101, 0b01000000,                         /* 105 = i */
  4, 0b01000000, 0b10000000, 0b10000100, 0b01111101,             /* 106 = j */
  5, 0b01111111, 0b00010000, 0b00010000, 0b00101000, 0b01000100, /* 107 = k */
  3, 0b01000001, 0b01111111, 0b01000000,                         /* 108 = l */
  5, 0b01111100, 0b00000100, 0b01111100, 0b00000100, 0b01111000, /* 109 = m */
  5, 0b01111100, 0b00001000, 0b00000100, 0b00000100, 0b01111000, /* 110 = n */
  5, 0b00111000, 0b01000100, 0b01000100, 0b01000100, 0b00111000, /* 111 = o */
  5, 0b11111100, 0b00100100, 0b00100100, 0b00100100, 0b00011000, /* 112 = p */
  5, 0b00011000, 0b00100100, 0b00100100, 0b00100100, 0b11111100, /* 113 = q */
  4, 0b01111100, 0b00001000, 0b00000100, 0b00001000,             /* 114 = r */
  5, 0b01001000, 0b01010100, 0b01010100, 0b01010100, 0b00100000, /* 115 = s */
  4, 0b00000100, 0b00111110, 0b01000100, 0b01000000,             /* 116 = t */
  5, 0b00111100, 0b01000000, 0b01000000, 0b00100000, 0b01111100, /* 117 = u */
  5, 0b00011100, 0b00100000, 0b01000000, 0b00100000, 0b00011100, /* 118 = v */
  5, 0b00111100, 0b01000000, 0b00110000, 0b01000000, 0b00111100, /* 119 = w */
  5, 0b01000100, 0b00101000, 0b00010000, 0b00101000, 0b01000100, /* 120 = x */
  5, 0b00011100, 0b10100000, 0b10100000, 0b10100000, 0b01111100, /* 121 = y */
  5, 0b01000100, 0b01100100, 0b01010100, 0b01001100, 0b01000100, /* 122 = z */
  3, 0b00001000, 0b00110110, 0b01000001,                         /* 123 = { */
  1, 0b01111111,                                                 /* 124 = | */
  3, 0b01000001, 0b00110110, 0b00001000,                         /* 125 = } */
  5, 0b00011000, 0b00000100, 0b00001000, 0b00010000, 0b00001100, /* 126 = ~ */

  5, 0b01010101, 0b10101010, 0b01010101, 0b10101010, 0b01010101, /* 127 = checkerboard pattern */
  5, 0b00011100, 0b00110110, 0b01010101, 0b01010101, 0b01000001, /* 128 =  */
  7, 0b00111111, 0b01000010, 0b10000100, 0b10000100, 0b10000100, 0b01000010, 0b00111111,   /* 129 = cat emoji U+1F431*/
  0,                                                             /* 130 =  */
  0,                                                             /* 131 =  */
  0,                                                             /* 132 =  */
  5, 0b01000000, 0b00000000, 0b01000000, 0b00000000, 0b01000000, /* 133 =  */
  3, 0b00000100, 0b01111110, 0b00000100,                         /* 134 =  */
  3, 0b00100100, 0b01111110, 0b00100100,                         /* 135 =  */
  0,                                                             /* 136 =  */
  5, 0b00100011, 0b00010011, 0b01101000, 0b00000100, 0b01100010, /* 137 = ‰ */
  5, 0b01001000, 0b10010101, 0b10010110, 0b10010101, 0b01100000, /* 138 = Š */
  0,                                                             /* 139 =  */
  0,                                                             /* 140 =  */
  0,                                                             /* 141 =  */
  5, 0b01111000, 0b10000101, 0b10000110, 0b10000101, 0b01001000, /* 142 = Č */
  5, 0b11000100, 0b10100101, 0b10010110, 0b10001101, 0b10000100, /* 143 = Ž */
  0,                                                             /* 144 =  */
  0,                                                             /* 145 =  */
  0,                                                             /* 146 =  */
  0,                                                             /* 147 =  */
  0,                                                             /* 148 =  */
  0,                                                             /* 149 =  */
  4, 0b00001000, 0b00001000, 0b00001000, 0b00001000,             /* 150 =  */
  5, 0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000, /* 151 =  */
  0,                                                             /* 152 =  */
  0,                                                             /* 153 =  */
  5, 0b01001000, 0b01010101, 0b01010110, 0b01010101, 0b00100000, /* 154 = š */
  0,                                                             /* 155 =  */
  0,                                                             /* 156 =  */
  5, 0b00111000, 0b01000101, 0b01000110, 0b01000101, 0b00101000, /* 157 = č */
  5, 0b01000100, 0b01100101, 0b01010110, 0b01001101, 0b01000100, /* 158 = ž */
  0,                                                             /* 159 =  */
  4, 0b00000000, 0b00000000, 0b00000000, 0b00000000,             /* 160 =   = non-breaking (wide) space */
  0,                                                             /* 161 = ¡ */
  0,                                                             /* 162 = ¢ */
  0,                                                             /* 163 = £ */
  0,                                                             /* 164 = ¤ */
  0,                                                             /* 165 = ¥ */
  0,                                                             /* 166 = ¦ */
  5, 0b00001010, 0b01010101, 0b01010101, 0b01010101, 0b00101000, /* 167 = § */
  0,                                                             /* 168 = ¨ */
  0,                                                             /* 169 = © */
  0,                                                             /* 170 = ª */
  0,                                                             /* 171 = « */
  0,                                                             /* 172 = ¬ */
  0,                                                             /* 173 = ­ */
  0,                                                             /* 174 = ® */
  0,                                                             /* 175 = ¯ */
  4, 0b00000110, 0b00001001, 0b00001001, 0b00000110,             /* 176 = ° */
  5, 0b01000100, 0b01000100, 0b01011111, 0b01000100, 0b01000100, /* 177 = ± */
  3, 0b00011101, 0b00010101, 0b00010111,                         /* 178 = ² */
  3, 0b00010101, 0b00010101, 0b00011111,                         /* 179 = ³ */
  0,                                                             /* 180 = ´ */
  5, 0b11111100, 0b00100000, 0b01000000, 0b01000000, 0b00111100, /* 181 = µ */
  0,                                                             /* 182 = ¶ */
  5, 0b00001000, 0b00011100, 0b00111110, 0b00011100, 0b00001000, /* 183 = · or • */
  0,                                                             /* 184 = ¸ */
  3, 0b00000000, 0b00011111, 0b00000000,                         /* 185 = ¹ */
  3, 0b00011111, 0b00010001, 0b00011111,                         /* 186 = º */
  0,                                                             /* 187 = » */
  0,                                                             /* 188 = ¼ */
  0,                                                             /* 189 = ½ */
  0,                                                             /* 190 = ¾ */
  0,                                                             /* 191 = ¿ */
  0,                                                             /* 192 = À */
  0,                                                             /* 193 = Á */
  0,                                                             /* 194 = Â */
  0,                                                             /* 195 = Ã */
  5, 0b11111000, 0b00010101, 0b00010100, 0b00010101, 0b11111000, /* 196 = Ä */
  0,                                                             /* 197 = Å */
  0,                                                             /* 198 = Æ */
  0,                                                             /* 199 = Ç */
  0,                                                             /* 200 = È */
  0,                                                             /* 201 = É */
  0,                                                             /* 202 = Ê */
  0,                                                             /* 203 = Ë */
  0,                                                             /* 204 = Ì */
  0,                                                             /* 205 = Í */
  0,                                                             /* 206 = Î */
  0,                                                             /* 207 = Ï */
  0,                                                             /* 208 = Ð */
  0,                                                             /* 209 = Ñ */
  0,                                                             /* 210 = Ò */
  0,                                                             /* 211 = Ó */
  0,                                                             /* 212 = Ô */
  0,                                                             /* 213 = Õ */
  5, 0b01111000, 0b10000101, 0b10000100, 0b10000101, 0b01111000, /* 214 = Ö */
  0,                                                             /* 215 = × */
  0,                                                             /* 216 = Ø */
  0,                                                             /* 217 = Ù */
  0,                                                             /* 218 = Ú */
  0,                                                             /* 219 = Û */
  5, 0b01111100, 0b10000001, 0b10000000, 0b10000001, 0b11111100, /* 220 = Ü */
  0,                                                             /* 221 = Ý */
  0,                                                             /* 222 = Þ */
  5, 0b11111110, 0b00000001, 0b01000101, 0b01001010, 0b00110000, /* 223 = ß */
  0,                                                             /* 224 = à */
  0,                                                             /* 225 = á */
  0,                                                             /* 226 = â */
  0,                                                             /* 227 = ã */
  5, 0b00100000, 0b01010101, 0b01010100, 0b01010101, 0b01111000, /* 228 = ä */
  0,                                                             /* 229 = å */
  0,                                                             /* 230 = æ */
  0,                                                             /* 231 = ç */
  0,                                                             /* 232 = è */
  0,                                                             /* 233 = é */
  0,                                                             /* 234 = ê */
  0,                                                             /* 235 = ë */
  0,                                                             /* 236 = ì */
  0,                                                             /* 237 = í */
  0,                                                             /* 238 = î */
  0,                                                             /* 239 = ï */
  0,                                                             /* 240 = ð */
  0,                                                             /* 241 = ñ */
  0,                                                             /* 242 = ò */
  0,                                                             /* 243 = ó */
  0,                                                             /* 244 = ô */
  0,                                                             /* 245 = õ */
  5, 0b00111000, 0b01000101, 0b01000100, 0b01000101, 0b00111000, /* 246 = ö */
  0,                                                             /* 247 = ÷ */
  0,                                                             /* 248 = ø */
  0,                                                             /* 249 = ù */
  0,                                                             /* 250 = ú */
  0,                                                             /* 251 = û */
  5, 0b00111100, 0b01000001, 0b01000000, 0b00100001, 0b01111100, /* 252 = ü */
  0,                                                             /* 253 = ý */
  0,                                                             /* 254 = þ */
  0,                                                             /* 255 = ÿ */
};

uint16_t* font_index = 0; // will be calculated at start by calculate_font_index()

void calculate_font_index()
{
  LogTarget.println((String)"calculate_font_index()");
  uint16 fontsize = sizeof(font);
  LogTarget.println((String)"Font uses " + fontsize + " bytes.");
  uint8_t* fontptr = (uint8_t*)font;
  int num_chars = 0;
  // 1st sweep: count chars
  while (fontptr < font + fontsize) {
    int char_width = pgm_read_byte(fontptr); //*fontptr;
    fontptr += char_width + 1;  // add character X size
    num_chars++;
  }
  // allocate index table
  uint16_t* index = (uint16_t*)malloc(num_chars * sizeof(uint16_t));
  // 2nd sweep: calculate index values
  fontptr = (uint8_t*)font;
  for (int i = 0; i < num_chars; i++) {
    index[i] = fontptr - font;
    int char_width = pgm_read_byte(fontptr); //*fontptr;
    //LogTarget.println((String)"idx " + i + " = " + (char)(i+32) + ": " + char_width + " -> " + index[i]);
    fontptr += char_width + 1;  // add character X size
  }
  font_index = index;
  LogTarget.println((String)"font_index ready.");
}

uint32_t encode_message(uint8_t* text, byte* payload, int length)
{
  const int pr = 0; // debug prints
  uint32_t marquee_col = 0;

  if (!font_index) {
    LogTarget.println((String)"Need to calculate font_index first!");
    calculate_font_index();
  }

  text[0] = ' ';
  for (int i = 0 ; i < MAX_MSG_LEN; i++) {
    text[i] = 0;
  }

  length = (length > MAX_MSG_LEN) ? (MAX_MSG_LEN - 1) : length;

  // convert UTF-8 into local code points (indices into font_index[] table)

  char tmp[16];     // only for debug printing
  int i = 0, j = 0;
  while (i < length) {
    uint8_t b = payload[i++];
    if (pr) {
      sprintf(tmp, "0x%.2X = '%c' -> ", b, b);
      LogTarget.print(tmp);
    }
    if (i == 1 && b == '!') {   // parameter escape
      LogTarget.println("parameter escape");
      if ((i+6) < length && payload[i] == '#') {
        // color code (6x hex digit)
        i++;
        marquee_col = strtol( (const char*)&(payload[i]), NULL, 16);
        i += 6;
      }
    } else if ((b & 0b10000000) == 0) {                    // 7-bit ASCII
      if (pr) LogTarget.println("ASCII");
      text[j++] = b;

      /* extending the character set:
           1 - find a suitable position in the font table ( = code point) in file font.h
           2 - put the glyph width and its pixel pattern into the font table
           3 - find out the Unicode (UTF-8) representation of the glyph
               e.g. by setting "const bool pr = 1;" in line 343 above
           4 - supplement the decoder in the following lines
             4.1 - two-byte sequence: find out if it starts with 0xC2, C3, C4, …
                   and check for the second byte
             4.2 - three-byte sequence: find out the prefix (0xE2)
                   and check for the second and third bytes
             4.3 - put the code point index into text[j++]
           5 - don't forget the else parts when introducing a new prefix
      */

    } else if ((b & 0b11100000) == 0b11000000) {    // UTF-8 2-byte sequence: starts with 0b110xxxxx
      if (pr) LogTarget.println("UTF-8 (2)");
      if (b == 0xC3) {                              // UTF-8 1st byte
        text[j++] = payload[i++] + 64;              // map 2nd byte to Latin-1 (simplified)

      } else if (b == 0xC2) {
        if (i < length && payload[i] == 0x86) {           //  = dagger sign
          text[j++] = 134;
        } else if (i < length && payload[i] == 0x87) {    //  = double dagger sign
          text[j++] = 135;
        } else if (i < length && payload[i] == 0x89) {    //  = per mille sign
          text[j++] = 137;
        } else if (i < length && payload[i] == 0xA0) {    // no-break space (wide space)
          text[j++] = 129;
        } else if (i < length && payload[i] == 0xA7) {    // § = section sign
          text[j++] = 167;
        } else if (i < length && payload[i] == 0xB0) {    // ° = degrees sign
          text[j++] = 176;
        } else if (i < length && payload[i] == 0xB1) {    // ± = plus/minus sign
          text[j++] = 177;
        } else if (i < length && payload[i] == 0xB2) {    // ² = superscript 2
          text[j++] = 178;
        } else if (i < length && payload[i] == 0xB3) {    // ³ = superscript 3
          text[j++] = 179;
        } else if (i < length && payload[i] == 0xB5) {    // µ = mu
          text[j++] = 181;
        } else if (i < length && payload[i] == 0xB9) {    // ¹ = superscript 1
          text[j++] = 185;
        } else if (i < length && payload[i] == 0xBA) {    // º = masculine numeral indicator ("numero")
          text[j++] = 186;
        } else {
          // unknown
          text[j++] = 127;                         // add checkerboard pattern
        }
        i += 1;
      } else if (b == 0xC4) {
        if (i < length && payload[i] == 0x8C) {           // Č = C with caron
          text[j++] = 142;
        } else if (i < length && payload[i] == 0x8D) {    // č = c with caron
          text[j++] = 157;
        } else {
          // unknown
          text[j++] = 127;                         // add checkerboard pattern
        }
      } else if (b == 0xC5) {
        if (i < length && payload[i] == 0xA0) {           // Š = S with caron
          text[j++] = 138;
        } else if (i < length && payload[i] == 0xA1) {    // š = s with caron
          text[j++] = 154;
        } else if (i < length && payload[i] == 0xBD) {    // Ž = Z with caron
          text[j++] = 143;
        } else if (i < length && payload[i] == 0xBE) {    // ž = z with caron
          text[j++] = 158;
        } else {
          // unknown
          text[j++] = 127;                         // add checkerboard pattern
        }
      } else {
        // unknown
        text[j++] = 127;                           // add checkerboard pattern
        i += 1;
      }

    } else if ((b & 0b11110000) == 0b11100000) {   // UTF-8 3-byte sequence: starts with 0b1110xxxx
      if (pr) LogTarget.println("UTF-8 (3)");
      if (i + 1 < length && b == 0xE2) {
        if (payload[i] == 0x82 && payload[i + 1] == 0xAC) {
          text[j++] = 128;                               // € = euro sign
        } else if (payload[i] == 0x80 && payload[i + 1] == 0x93) {
          text[j++] = 150;                               // – = en dash
        } else if (payload[i] == 0x80 && payload[i + 1] == 0x94) {
          text[j++] = 151;                               // — = em dash
        } else if (payload[i] == 0x80 && payload[i + 1] == 0xA2) {
          text[j++] = 183;                               // • = bullet
        } else if (payload[i] == 0x80 && payload[i + 1] == 0xA6) {
          text[j++] = 133;                               // … = ellipsis
        } else if (payload[i] == 0x80 && payload[i + 1] == 0xB0) {
          text[j++] = 137;                               // ‰ = per mille
        } else {
          // unknown
          text[j++] = 127;                         // add checkerboard pattern
        }
        i += 2;
      } else {
        // unknown, skip remaining 2 bytes
        i += 2;
        text[j++] = 127;                           // add checkerboard pattern
        text[j++] = 127;                           // add checkerboard pattern
      }

    } else if ((b & 0b11111000) == 0b11110000) {   // UTF-8 4-byte sequence_ starts with 0b11110xxx
      if (pr) LogTarget.println("UTF-8 (4)");
      if (i + 2 < length && b == 0xF0) {
        if (payload[i] == 0x9F && payload[i + 1] == 0x90 && payload[i + 2] == 0xB1) {
          text[j++] = 129;                               // cat emoji
        } else {
          // unknown
          text[j++] = 127;                         // add checkerboard pattern
        }
        i += 3;
      } else {
        // unknown, skip remaining 3 bytes
        i += 3;
        text[j++] = 127;                             // add checkerboard pattern
        text[j++] = 127;                             // add checkerboard pattern
        text[j++] = 127;                             // add checkerboard pattern
      }

    } else {                                        // unsupported (illegal) UTF-8 sequence
      if (pr) LogTarget.print("UTF-8 (n) ");
      while ((payload[i] & 0b10000000) && i < length) {
        if (pr) LogTarget.print("+");
        i++;                                        // skip non-ASCII characters
        text[j++] = 127;                           // add checkerboard pattern
      }
      if (pr) LogTarget.println();
    }
  }

  // fill undefined characters with placeholder glyph
  // j == text length
  for (int i = 0; i < j; i++) {
    uint8_t asc = text[i] - 32;
    uint16_t idx = pgm_read_word(&(font_index[asc]));
    uint8_t w = pgm_read_byte(&(font[idx]));
    if (w == 0) {
      // character is NOT defined, replace with 0x7f = checkerboard pattern
      text[i] = 0x7F;
    }
  }

  // add trailer with space glyphs
  int space_left = MAX_MSG_LEN - j;
  const int MAX_TRAILER_LEN = 5;
  int trailer_len = min(MAX_TRAILER_LEN, MAX_MSG_LEN - (j + 1));
  for (int i = 0; i < trailer_len; i++) {
    text[j + i] = 32; // space
  }
  text[j + trailer_len] = 0;

  return marquee_col;
}
