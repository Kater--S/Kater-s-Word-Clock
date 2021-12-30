
#include "config.h"

#if CLOCK
#include <ezTime.h>
//#include <WiFi.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>
Timezone myTZ;
#endif

#include <Adafruit_NeoPixel.h>

#ifndef LOG_TELNET
#define LOG_TELNET  0
#endif

#ifndef LAYOUT_SPACESTATUS
#define LAYOUT_SPACESTATUS 0
#endif

#if LOG_TELNET
#include <TelnetStream.h>
#define LogTarget TelnetStream
#else
#define LogTarget Serial
#endif

#if CLOCK && (SPACESTATUS || MARQUEE)
#include <PubSubClient.h>
WiFiClient wifiClient;
PubSubClient client(wifiClient);
int spacestatus = -1;   // -1 = unknown, 0 = closed, 1 = open
#endif

#if MARQUEE
const int MAX_MSG_LEN = 1024;
#include "font.h"

#ifndef MARQUEE_INTRO
#define MARQUEE_INTRO 0
#endif

#endif

/*

  -- GERMAN VERSION --

   Encoding the time:

   58:    Es ist gleich %hh Uhr
   59:    Es ist gleich %hh Uhr
   00:    Es ist        %hh Uhr
   01:    Es war gerade %hh Uhr
   02:    Es war gerade %hh Uhr

   03:    Es ist gleich fünf nach %hh
   04:    Es ist gleich fünf nach %hh
   05:    Es ist        fünf nach %hh
   06:    Es war gerade fünf nach %hh
   07:    Es war gerade fünf nach %hh

   08-12  Es ... ...... zehn nach %hh
   13-17: Es ... ...... viertel nach %hh
   18-23: Es ... ...... zwanzig nach %hh
   24-27: Es ... ...... fünf vor halb %hh+1
   28-33: Es ... ...... halb %hh+1
   34-37: Es ... ...... fünf nach halb %hh+1
   38-43: Es ... ...... zwanzig vor %hh+1
   44-47: Es ... ...... viertel vor %hh+1
   48-53: Es ... ...... zehn vor %hh+1
   54-57: Es ... ...... fünf vor %hh+1
   58-59  Es ist gleich %hh+1 Uhr

   Short version:

   00-04: %hh Uhr
   05-09: fünf nach %hh
   10-14: zehn nach %hh
   15-19: viertel nach %hh
   20-24: zwanzig nach %hh
   25-29: fünf for halb %hh+1
   30-34: halb %hh+1
   35-39: fünf nach halb %hh+1
   40-44: zwanzig vor %hh+1
   45-49: viertel vor %hh+1
   50-54: zehn vor %hh+1
   55-59: fünf vor %hh+1

   Matrix:
  x=  0123456789abc
  y=0 ESEISTLWARMPY
    1 GERADEGLEICHZ
    2 PUNKTETWAKALT
    3 ZWANZIGZEHNEU
    4 FÜNFEMINUTENV
    5 VIERTELPNACHT
    6 VORTHALBGELFE
    7 ZWÖLFÜNFZWEIG
    8 ZEHNEUNACHTAN
    9 UNDREINSFVIER
    a SECHSIEBENASS
    b QUHREZEITWFÜR
    c HÜTTEHAUFZUDB
  or c SKAFFEESSENDB

   x
   0123456789abc
   ES IST WAR
   GERADEgleich
   PUNKTetwa
   ZWANZIGzehn
   FÜNF MINUTEN
   VIERTEL NACH
   VOR HALB ELF
   ZWÖLFünfZWEI
   ZEHNeunACHT
   UNDreINS VIER
   SECHSieben
    UHR ZEIT FÜR
   HÜTTE AUFzu
  or  KAFFEEssen

   Coding:
   x=4, y=7, len=3 -> 0x040703;
*/

const int MAX_X = 13; // number of columns
const int MAX_Y = 13; // number of rows
const char* matrix[] = {  // only used for debugging output
  "ESEISTLWARMPY",
  "GERADEGLEICHZ",
  "PUNKTETWAKALT",
  "ZWANZIGZEHNEU",
  "FUNFEMINUTENV",
  "VIERTELPNACHT",
  "VORTHALBGELFE",
  "ZWOLFUNFZWEIG",
  "ZEHNEUNACHTAN",
  "UNDREINSFVIER",
  "SECHSIEBENASS",
  "QUHREZEITWFUR",
#if LAYOUT_SPACESTATUS
  "HUTTEHAUFZUDB"
#else
  "SKAFFEESSENDB"
#endif
};

const long C_ES       = 0x000002;
const long C_IST      = 0x030003;
const long C_WAR      = 0x070003;
const long C_WARM     = 0x070004;
const long C_MEZ1     = 0x0a0001;
const long C_MEZ2     = 0x080101;
const long C_MEZ3     = 0x0c0101;
const long C_GERADE   = 0x000106;
const long C_DE       = 0x040102;
const long C_GLEICH   = 0x060106;
const long C_PUNKT    = 0x000205;
const long C_ETWA     = 0x050204;
const long C_WZ1      = 0x070201;
const long C_WZ2      = 0x070301;
const long C_KALT     = 0x090204;
const long C_ZWANZIG  = 0x000307;
const long C_WAN      = 0x010303;
const long C_ZEHN     = 0x070304;
const long C_NEU      = 0x0a0303;
const long C_FUENF    = 0x000404;
const long C_MINUTEN  = 0x050407;
const long C_VIERTEL  = 0x000507;
const long C_NACH     = 0x080504;
const long C_VOR      = 0x000603;
const long C_HALB     = 0x040604;
const long C_11       = 0x090603;
const long C_12       = 0x000705;
const long C_5        = 0x040704;
const long C_2        = 0x080704;
const long C_10       = 0x000804;
const long C_9        = 0x030804;
const long C_8        = 0x070804;
const long C_UND      = 0x000903;
const long C_3        = 0x020904;
const long C_1        = 0x040904; // EINS
const long C_1x       = 0x040903; // EIN
const long C_4        = 0x090904;
const long C_6        = 0x000a05;
const long C_7        = 0x040a06;
const long C_NASS     = 0x090a04;
const long C_UHR      = 0x010b03;
const long C_ZEIT     = 0x050b04;
const long C_FUER     = 0x0a0b04;
#if LAYOUT_SPACESTATUS
const long C_HUE      = 0x000c02;
const long C_HUETTE   = 0x000c05;
const long C_AUF      = 0x060c03;
const long C_ZU       = 0x090c02;
#else
const long C_HUE      = 0x020b01;
const long C_HUETTE   = 0x020b01;
const long C_AUF      = 0x020c01;
const long C_ZU       = 0x0a0c01;
const long C_SK       = 0x000c02;
const long C_KAFFEE   = 0x010c06;
const long C_ESSEN    = 0x060c05;
#endif
const long C_DB       = 0x0b0c02;

const int hours[] = { C_12, C_1, C_2, C_3,  C_4,  C_5,  C_6,  C_7,  C_8,  C_9,  C_10,  C_11,  C_12 };

long ledcodes[15] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int ledcodes_idx = 0;
String txt;

#if TESTPANEL

/*
   temporary matrix panel: 20x15 LEDs, wired differently

     />>>>>>>>>>>>>>>>>>>>
     \<<<<<0123456789abc<< \
     />>>>>0123456789abc>> /
     \<<<<<0123456789abc<< \
     />>>>>0123456789abc>> /
     \<<<<<0123456789abc<< \
     />>>>>0123456789abc>> /
     \<<<<<0123456789abc<< \
     />>>>>0123456789abc>> /
     \<<<<<0123456789abc<< \
     />>>>>0123456789abc>> /
     \<<<<<0123456789abc<< \
     />>>>>0123456789abc>> /
     \<<<<<0123456789abc<< \
  in-->>>>>>>>>>>>>>>>>>>> /
      01234567890123456789
*/

const int panel_x = 20;
const int panel_y = 15;
const int offset_x = 5;
const int offset_y = 1;

#else
// final matrix panel
const int panel_x = 13;
const int panel_y = 13;
const int offset_x = 0;
const int offset_y = 0;
#endif

const int day_brightness = C_DAY_BRIGHTNESS;
const int night_brightness = C_NIGHT_BRIGHTNESS;
const int day_h = C_DAY_H;
const int day_m = C_DAY_M;
const int night_h = C_NIGHT_H;
const int night_m = C_NIGHT_M;

char hostname[50];

int brightness = day_brightness;

// display modes
const int DM_INIT = 0;
const int DM_CLOCK = 1;
const int DM_MARQUEE = 2;
const int DM_MARQUEE_INTRO = 3;

int displaymode = DM_INIT;


// FUNCTIONS

///////////////////////////////////////////////////////
// pixels & colors

int panel2strip(int x, int y)
{
#if TESTPANEL
  // for 20x15 panel on grey eurobox lid
  return y * panel_x + ( (y & 1) ? (panel_x - x - 1) : x);
#else
  // for 13x13 panel in RIBBA frame
  return (panel_y - y - 1) * panel_x + ( (y & 1) ? (panel_x - x - 1) : x);
#endif
}

Adafruit_NeoPixel strip = Adafruit_NeoPixel(300, LED_PIN, NEO_GRB + NEO_KHZ800);

void clearLEDs()
{
  for (int i = 0; i < 300; i++) strip.setPixelColor(i, 0);
  //strip.show();
}

#if 1
// moving colors (HSV)

const int pat_x = 2 * MAX_X;
const int pat_y = 2 * MAX_Y;
int pat_offset_x = 0;
int pat_offset_y = 0;
const int num_peaks = 3;
int peak_coord_x[num_peaks], peak_coord_y[num_peaks], peak_height[num_peaks], peak_reach[num_peaks];

void change_colorscheme(int lvl = 0)
{
  static int cycle = 0;
  if (lvl == 0) {
    // move color map one step
    //LogTarget.println("move color map");
    int dir = random(2);
    pat_offset_x = (pat_offset_x + (dir != 2 ? 1 : 0)) % pat_x;
    pat_offset_y = (pat_offset_y + (dir != 0 ? 1 : 0)) % pat_y;
    cycle = (cycle + 1) % 1000;
    if (cycle == 0) change_colorscheme(1);
  } else {
    // calculate new color map
    LogTarget.println("generate new color map");
    for (int peak = 0; peak < num_peaks; peak++) {
      peak_coord_x[peak] = random(pat_x);
      peak_coord_y[peak] = random(pat_y);
      peak_height[peak] = 50 + random(100);
      peak_reach[peak] = 5 + random(2 * pat_y);
      LogTarget.println(String("peak ") + peak
                        + " = pos (" + peak_coord_x[peak] + ", " + peak_coord_y[peak] + "),"
                        + " h " + peak_height[peak] + ", r " + peak_reach[peak]);
    }
  }
}

uint32_t Wheel(byte WheelPos, int subbrightness = 100) {
  int full = 255;
  int third = 255 / 3;
  int secondthird = full - third;
  int bright = brightness * subbrightness / 100;

  if (WheelPos < third) {
    return strip.Color(WheelPos * 3 * bright / 255, (full - WheelPos * 3)  * bright / 255, 0);
  }
  else if (WheelPos < secondthird) {
    WheelPos -= third;
    return strip.Color((full - WheelPos * 3) * bright / 255, 0, WheelPos * 3 * bright / 255);
  }
  else {
    WheelPos -= secondthird;
    return strip.Color(0, WheelPos * 3 * bright / 255, (full - WheelPos * 3) * bright / 255);
  }
}

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))

uint32_t xy2col(int i, int j, int subbrightness = 100)
{
  int col = 0;
  for (int peak = 0; peak < num_peaks; peak++) {
    // calculate distance in cyclic grid
    //simple rectangular grid (Manhattan) distance
    int a, b;
    a = ((i % MAX_X) + pat_offset_x) % pat_x;
    b = peak_coord_x[peak];
    int delta_x = min(min(
                        abs(a - b),
                        abs(a - b + pat_x)),
                      abs(b - a + pat_x));
    a = ((j % MAX_Y) + pat_offset_y) % pat_y;
    b = peak_coord_y[peak];
    int delta_y = min(min(
                        abs(a - b),
                        abs(a - b + pat_y)),
                      abs(b - a + pat_y));
    int delta = delta_x + delta_y;  // simple grid distance
    //col += max(peak_reach[peak] - delta, 0) * 255 / peak_height[peak];
    col += (int)max(0.0f, (1 - (float)delta / peak_reach[peak]) * peak_height[peak]);
  }
  col = col % 256;
  return Wheel(col, subbrightness);
}

#else

// simple color gradients

int colorscheme = 0;
void change_colorscheme(int lvl = 0)
{
  colorscheme++;
}
uint32_t xy2col(int i, int j, subbrightness = 100)
{
  int bright = brightness * subbrightness / 100;
  switch (colorscheme % 3) {
    case 0:
      return strip.Color(i * bright / MAX_X, bright / 2, j * bright / MAX_X);
    case 1:
      return strip.Color(bright / 2, i * bright / MAX_X, j * bright / MAX_X);
    case 2:
      return strip.Color(i * bright / MAX_X, j * bright / MAX_X, bright / 2);
  }
}
#endif

uint32_t dim_color(uint32_t col, int bitshift)
{
  // MSB is White (unused)
  uint8_t r = (col >> 16) & 0xff;
  uint8_t g = (col >>  8) & 0xff;
  uint8_t b = (col      ) & 0xff;
  return ((r >> bitshift) << 16) | ((g >> bitshift) << 8) | (b >> bitshift);
}

void set_matrix(bool show = true)
{
  int pr = 0;
  LogTarget.println("set_matrix");
  for (int i = 0; i < MAX_X; i += 1) {
    if (pr) LogTarget.print(String(" row ") + i + ": ");
    for (int j = 0; j < MAX_Y; j += 1) {
      if (pr) LogTarget.print(String("") + j + " ");
      strip.setPixelColor(panel2strip(i + offset_x, j + offset_y),
                          xy2col(i, j, 20));
      //                    strip.Color(i * brightness / MAX_X, brightness, j * brightness / MAX_X));
      if (show) strip.show();
    }
    if (pr) LogTarget.println();
  }
  if (show) {
    strip.show();
    delay(100);
  }
}

void set_LED(int i, int j, int subbrightness = 100, uint32_t color = 0)
{
  strip.setPixelColor(panel2strip(i + offset_x, (MAX_Y - j - 1) + offset_y), color ? color : xy2col(i, j, subbrightness));
  //strip.show();
}

///////////////////////////////////////////////////////
// time

bool is_day(int h, int m)
{
  bool ret = false;
  const int day_hm = (day_h % 24) * 60 + day_m;
  const int night_hm = (night_h % 24) * 60 + night_m;
  int now_hm = (h % 24) * 60 + m;
  if (day_hm < night_hm) {
    return (day_hm < now_hm) && (now_hm < night_hm);
  } else if (day_hm > night_hm) {
    return !((night_hm < now_hm) && (now_hm < day_hm));
  } else { // day_hm == night_hm
    return true;  // no night dimming at all
  }
  return ret;
}

void encode_time(int h, int m, int s = 0, bool short_format = false)
{
  int hh = h % 12;
  hh += (hh ? 0 : 12);
  int hnext = (hh + 1) % 12;
  hnext += (hnext ? 0 : 12);
  int mm = m;

  ledcodes_idx = 0;
  if (short_format) {
    txt = String("");
  } else {
    ledcodes[ledcodes_idx++] = C_ES;
    txt = String("Es ");
    switch ((mm + 2) % 5)
    {
      case 0:
      case 1:
        txt = txt + "ist gleich ";
        ledcodes[ledcodes_idx++] = C_IST;
        ledcodes[ledcodes_idx++] = C_GLEICH;
        break;
      case 2:
        txt = txt + "ist ";
        ledcodes[ledcodes_idx++] = C_IST;
        if (s == 0) {
          txt = txt + "Punkt ";
          ledcodes[ledcodes_idx++] = C_PUNKT;
        }
        break;
      case 3:
      case 4:
        txt = txt + "war gerade ";
        ledcodes[ledcodes_idx++] = C_WAR;
        ledcodes[ledcodes_idx++] = C_GERADE;
        break;
    }
  }
  int hrs = (mm > 30) ? hnext : hh;
  int mmpart = ((mm + (short_format ? 0 : 2)) % 60) / 5;
  LogTarget.println((String)"hrs = " + hrs + ", mm = " + mm + ", mmpart = " + mmpart);
  switch (mmpart)
  {
    case 0:
      txt = txt + hrs + " Uhr";
      ledcodes[ledcodes_idx++] = (hrs == 1) ?
                                 C_1x :             // "EIN UHR" instead of "EINS UHR"
                                 hours[hrs];
      ledcodes[ledcodes_idx++] = C_UHR;
      break;
    case 1:
      txt = txt + "fünf " + (!short_format ? "minuten " : "") + "nach " + hh;
      ledcodes[ledcodes_idx++] = C_FUENF;
      if (!short_format) ledcodes[ledcodes_idx++] = C_MINUTEN;
      ledcodes[ledcodes_idx++] = C_NACH;
      ledcodes[ledcodes_idx++] = hours[hh];
      break;
    case 2:
      txt = txt + "zehn " + (!short_format ? "minuten " : "") + "nach " + hh;
      ledcodes[ledcodes_idx++] = C_ZEHN;
      if (!short_format) ledcodes[ledcodes_idx++] = C_MINUTEN;
      ledcodes[ledcodes_idx++] = C_NACH;
      ledcodes[ledcodes_idx++] = hours[hh];
      break;
    case 3:
      txt = txt + "viertel nach " + hh;
      ledcodes[ledcodes_idx++] = C_VIERTEL;
      ledcodes[ledcodes_idx++] = C_NACH;
      ledcodes[ledcodes_idx++] = hours[hh];
      break;
    case 4:
      txt = txt + "zwanzig " + (!short_format ? "minuten " : "") + "nach " + hh;
      ledcodes[ledcodes_idx++] = C_ZWANZIG;
      if (!short_format) ledcodes[ledcodes_idx++] = C_MINUTEN;
      ledcodes[ledcodes_idx++] = C_NACH;
      ledcodes[ledcodes_idx++] = hours[hh];
      break;
    case 5:
      txt = txt + "fünf " + (!short_format ? "minuten " : "") + "vor halb " + hnext;
      ledcodes[ledcodes_idx++] = C_FUENF;
      if (!short_format) ledcodes[ledcodes_idx++] = C_MINUTEN;
      ledcodes[ledcodes_idx++] = C_VOR;
      ledcodes[ledcodes_idx++] = C_HALB;
      ledcodes[ledcodes_idx++] = hours[hnext];
      break;
    case 6:
      txt = txt + "halb " + hnext;
      ledcodes[ledcodes_idx++] = C_HALB;
      ledcodes[ledcodes_idx++] = hours[hnext];
      break;
    case 7:
      txt = txt + "fünf " + (!short_format ? "minuten " : "") + "nach halb " + hnext;
      ledcodes[ledcodes_idx++] = C_FUENF;
      if (!short_format) ledcodes[ledcodes_idx++] = C_MINUTEN;
      ledcodes[ledcodes_idx++] = C_NACH;
      ledcodes[ledcodes_idx++] = C_HALB;
      ledcodes[ledcodes_idx++] = hours[hnext];
      break;
    case 8:
      txt = txt + "zwanzig " + (!short_format ? "minuten " : "") + "vor " + hnext;
      ledcodes[ledcodes_idx++] = C_ZWANZIG;
      if (!short_format) ledcodes[ledcodes_idx++] = C_MINUTEN;
      ledcodes[ledcodes_idx++] = C_VOR;
      ledcodes[ledcodes_idx++] = hours[hnext];
      break;
    case 9:
      txt = txt + "viertel vor " + hnext;
      ledcodes[ledcodes_idx++] = C_VIERTEL;
      ledcodes[ledcodes_idx++] = C_VOR;
      ledcodes[ledcodes_idx++] = hours[hnext];
      break;
    case 10:
      txt = txt + "zehn " + (!short_format ? "minuten " : "") + "vor " + hnext;
      ledcodes[ledcodes_idx++] = C_ZEHN;
      if (!short_format) ledcodes[ledcodes_idx++] = C_MINUTEN;
      ledcodes[ledcodes_idx++] = C_VOR;
      ledcodes[ledcodes_idx++] = hours[hnext];
      break;
    case 11:
      txt = txt + "fünf " + (!short_format ? "minuten " : "") + "vor " + hnext;
      ledcodes[ledcodes_idx++] = C_FUENF;
      if (!short_format) ledcodes[ledcodes_idx++] = C_MINUTEN;
      ledcodes[ledcodes_idx++] = C_VOR;
      ledcodes[ledcodes_idx++] = hours[hnext];
      break;
  }
#if CLOCK && SPACESTATUS
  if (spacestatus != -1) {
    ledcodes[ledcodes_idx++] = C_HUETTE;
    if (spacestatus == 1) {
      ledcodes[ledcodes_idx++] = C_AUF;
    } else {
      ledcodes[ledcodes_idx++] = C_ZU;
    }
  }
#endif
}

void show_ledcodes(int subbrightness = 100)
{
  /*LogTarget.println("-> ");
    for(int i = 0; i < ledcodes_idx; i++) {
    LogTarget.print(String(ledcodes[i], HEX) + " ");
    }*/
  LogTarget.println("+-------------+");
  for (int y = 0; y < MAX_Y; y++) {
    //LogTarget.println(String("Reihe ") + y + ": ");
    String row = String("             "); // length = MAX_X
    for (int i = 0; i < ledcodes_idx; i++) {
      long code = ledcodes[i];
      int ypos = (int)(code >> 8) & 0xff;
      //LogTarget.print(String("teste Wort ") + String(code, HEX) + ": ypos " + ypos + " ");
      if (ypos == y) {
        //LogTarget.print(" - passt. Zeichen:");
        int xpos = (int)(code >> 16) & 0xff;
        int len = (int)(code >> 0) & 0xff;
        for (int j = 0; j < len; j++) {
          char ch = matrix[y][xpos + j];
          row.setCharAt(xpos + j, ch);
          set_LED(xpos + j, ypos, subbrightness);
          //LogTarget.print(ch);
        }
        /*LogTarget.println();
          } else {
          LogTarget.println(" - passt nicht.");*/
      }
    }
    LogTarget.println(String("|") + row + "|");
  }
  LogTarget.println("+-------------+");
  /*
    for (int i = 0; i < MAX_X; i++)
      for (int j = 0; j < MAX_Y; j++)
        setLED(i, j);
  */
  strip.show();
  change_colorscheme(); // move color map a bit
}

int h0 = 0, m0 = 0;

void show_time(int h, int m, int s = 0, bool use_short = false)
{
  if (h != h0 || m != m0)   // minute has changed since last call -> generate new color scheme
    change_colorscheme(1);
  h0 = h; m0 = m;
  clearLEDs();
  encode_time(h, m, s, use_short);
  LogTarget.println(String() + (h < 10 ? "0" : "") + h + ":" + (m < 10 ? "0" : "") + m + ":" + (s < 10 ? "0" : "") + s + " -> " + txt + "  (" + ledcodes_idx + " Wörter)");
  show_ledcodes();
}

///////////////////////////////////////////////////////
// marquee

#if MARQUEE

uint8_t scrollbuffer[MAX_MSG_LEN];
uint16_t textIndex = 0;
uint8_t colIndex = 0;
uint16_t scrollWhitespace = 0;
uint64_t marqueeDelayTimestamp = 0;
bool marquee_done = true;
uint8_t marquee_columns[MAX_X];
uint16_t scrollDelay = 35;                        // Initial scroll delay
uint32_t marquee_col = 0;

void nextChar()
{
  if (scrollbuffer[++textIndex] == '\0')
  {
    textIndex = 0;
    scrollWhitespace = MAX_X; // start over with empty display
    marquee_done = true;
  }
}

void nextCol(uint8_t glyph_width)
{
  if (++colIndex == glyph_width)
  {
    scrollWhitespace = 1;   // inter-glyph space in pixels
    colIndex = 0;
    nextChar();
  }
}

void set_matrix_dots(int subbrightness = 100) {
  for (int column = 0; column < MAX_X; column++) {
    uint8_t colpattern = marquee_columns[column];
    for (int bit = 0; bit < 8; bit++) {
      if (colpattern & (1 << bit)) {
        set_LED(column, 3 + bit, subbrightness, marquee_col);
      }
    }
  }
}

void writeCol(int subbrightness = 100)
{
  if (scrollWhitespace > 0) {
    scrollWhitespace--;
    for (int i = 0; i < MAX_X - 1; i++) marquee_columns[i] = marquee_columns[i + 1]; // scroll left
    marquee_columns[MAX_X - 1] = 0; // add empty column
    set_matrix_dots(subbrightness);
    return;
  }
  uint8_t asc = scrollbuffer[textIndex] - 32;
  uint16_t idx = pgm_read_word(&(font_index[asc]));
  uint8_t glyph_width = pgm_read_byte(&(font[idx]));
  uint8_t colpattern = pgm_read_byte(&(font[colIndex + idx + 1]));
  LogTarget.println((String)"setColumn(" + String(colpattern, BIN) + ")");
  for (int i = 0; i < MAX_X - 1; i++) marquee_columns[i] = marquee_columns[i + 1]; // scroll left
  marquee_columns[MAX_X - 1] = colpattern; // add new pattern
  set_matrix_dots(subbrightness);
  nextCol(glyph_width);
}

void marquee()
{
  if (millis() < 1)
    marqueeDelayTimestamp = 0;
  if (millis() < marqueeDelayTimestamp)
    return;
  marqueeDelayTimestamp = millis() + scrollDelay;
  LogTarget.println((String)"scroll left");
  writeCol();
}


void encode_marquee()
{
  //set_matrix(false);  // colored background, for now
  //set_matrix();         // black background, for now
  marquee();
}

void show_marquee()
{
  LogTarget.println((String)"show_marquee(), textIndex = " + textIndex);
  clearLEDs();
  encode_marquee();
  if (marquee_done)
    displaymode = DM_CLOCK;
  strip.show();
  change_colorscheme(); // move color map a bit
}

#if MARQUEE_INTRO
void show_intro()
{
  // CAREFUL! 
  // Since the animation lights up ALL the LEDs in the matrix, 
  // we must not exceed the maximum current!
  // Therefore we dim the non-default color for the animation by a factor of 16, 
  // whereas the marquee text is shown with "only" a quarter the color values.
  // If default (mapped) color is used, we specify a subbrightness of 40 %.
  LogTarget.println("show_intro");
  uint32_t color = dim_color(marquee_col, 3);
  clearLEDs();
  strip.show();
  for (int j = 0; j < MAX_Y; j += 1) {
    for (int i = 0; i < MAX_X; i += 1) {
      strip.setPixelColor(panel2strip(i + offset_x, MAX_Y - 1 - j + offset_y), marquee_col ? color : xy2col(i, j, 40));
    }
    strip.show();
    delay(100);
  }
  delay(100);
  for (int j = 0; j < MAX_Y / 2; j += 1) {
    delay(150);
    for (int i = 0; i < MAX_X; i += 1) {
      strip.setPixelColor(panel2strip(i + offset_x, MAX_Y / 2 + j + offset_y), 0);
    }
    for (int i = 0; i < MAX_X; i += 1) {
      strip.setPixelColor(panel2strip(i + offset_x, MAX_Y / 2 - j + offset_y), 0);
    }
    strip.show();
  }

  displaymode = DM_MARQUEE;
}
#endif
#endif


int h = 0, m = 0, s = 0, s0 = 0;
void action()
{
#if CLOCK
  // regular operation, not test

  //LogTarget.println((String)"displaymode is " + (displaymode == DM_CLOCK ? "CLOCK" : (displaymode == DM_MARQUEE ? "MARQUEE" : "UNKNOWN")));
  switch (displaymode) {
    case DM_CLOCK: {
        // regular Word Clock display mode
        s = myTZ.second();
        if (s == s0) return;
        s0 = s;
        m = myTZ.minute();
        h = myTZ.hour();
        bool daytime = is_day(h, m);
        brightness = daytime ? day_brightness : night_brightness;
        bool use_short = C_SHORT ? (C_SHORT == 1 ? true : !daytime) : false;
        show_time(h, m, s, use_short);
        break;
      }

#if MARQUEE
    case DM_MARQUEE:
      // marquee display mode
      show_marquee();
      break;
#if MARQUEE_INTRO
    case DM_MARQUEE_INTRO:
      show_intro();
      break;
#endif
#endif

    default:
      LogTarget.println((String)"unknown display mode " + displaymode);
      displaymode = DM_CLOCK;
      break;
  }

#else
  // test mode
#if 0
  showtime(h, m, s);
  s++;
  if (s == 60) {
    s = 0;
    m++;
    if (m == 60) {
      m = 0;
      h = (h + 1) % 24;
    }
  }
#else
  set_matrix();
  change_colorscheme();
#endif
#endif
}

void setup()
{
  Serial.begin(115200);
  delay(200);
  LogTarget.print("\n\n\n\n\n");
  delay(200);
  while (!Serial) {
    ;  // wait for LogTarget port to connect. Needed for native USB port only
  }
  LogTarget.println("WordClock V1.5");

#if CLOCK
  // Use Katers-WordClock-[MAC] as Name for (a) WifiManager AP, (b) OTA hostname, (c) MQTT client name
  uint8_t mac[6];
  WiFi.macAddress(mac);
  sprintf(hostname, "Katers-WordClock-%02x%02x%02x", mac[3], mac[4], mac[5]);
  LogTarget.println(String("hostname = ") + hostname);

  WiFiManager wifiManager;
#endif

  strip.begin();
  strip.show();

  randomSeed(analogRead(0) + millis() + 2);
  // This is pretty deterministic, so the color scheme will usually be the same.
  // Change to another seed value for a different color scheme after your fancy.
  change_colorscheme(1);
  set_matrix();

  ledcodes_idx = 0;
#if SPACESTATUS
  ledcodes[ledcodes_idx++] = C_HUE;
#endif
  show_ledcodes(250);
  delay(1000);

#if CLOCK
  ledcodes_idx = 0;
  ledcodes[ledcodes_idx++] = C_WAN;
  show_ledcodes(250);
  delay(1000);

  wifiManager.autoConnect(hostname);

  //// OTA FUNCTIONS

  ArduinoOTA.setHostname(hostname);

  // No authentication by default
  //ArduinoOTA.setPassword("WC");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    LogTarget.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    LogTarget.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    LogTarget.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    LogTarget.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) LogTarget.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) LogTarget.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) LogTarget.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) LogTarget.println("Receive Failed");
    else if (error == OTA_END_ERROR) LogTarget.println("End Failed");
  });
  ArduinoOTA.begin();
  //
  //
  //// END OF OTA FUNCTIONS

  ledcodes_idx = 0;
  ledcodes[ledcodes_idx++] = C_ZEIT;
  show_ledcodes(250);

  setDebug(DEBUG);
  waitForSync();

  LogTarget.println();
  LogTarget.println("UTC:             " + UTC.dateTime());

  if (myTZ.setLocation(F("de"))) {
    ledcodes_idx = 0;
    ledcodes[ledcodes_idx++] = C_DE;
    show_ledcodes(250);
    LogTarget.print(F("Germany:         "));
    LogTarget.println(myTZ.dateTime());
  } else {
    ledcodes_idx = 0;
    ledcodes[ledcodes_idx++] = C_WZ1;
    ledcodes[ledcodes_idx++] = C_WZ2;
    show_ledcodes(250);
  }
  delay(5000);

  // See if local time can be obtained (does not work in countries that span multiple timezones)
  LogTarget.print(F("Local (GeoIP):   "));
  if (myTZ.setLocation()) {
    LogTarget.println(myTZ.dateTime());
  } else {
    LogTarget.println(String("Error: ") + errorString());
  }

  randomSeed(analogRead(0) + millis()); // This will be definitely non-deterministic as it includes network timing.
#endif

  change_colorscheme(1);

#if CLOCK && (SPACESTATUS || MARQUEE)
  client.setServer(C_MQTTSERVER, 1883);
  client.setCallback(callback);
#endif

  displaymode = DM_CLOCK;
}

#if CLOCK && (SPACESTATUS || MARQUEE)
void reconnect() {
  const long delta = 10  * 1000;
  static long lastcalled = 0;
  long thiscall = millis() + delta;
  if (thiscall - lastcalled < delta) return;
  lastcalled = thiscall;
  // just try once
  LogTarget.println(String("Attempting MQTT connection to ") + C_MQTTSERVER + "...");
  // Attempt to connect
  if (client.connect(hostname, C_MQTTUSER, C_MQTTPASSWORD)) {
    LogTarget.println("connected");
    // ... and resubscribe
#if SPACESTATUS
    LogTarget.println(client.subscribe(C_MQTTTOPIC_SPACE));
#endif
#if MARQUEE
    LogTarget.println(client.subscribe(C_MQTTTOPIC_MSG));
#endif
  } else {
    LogTarget.print("failed, rc=");
    LogTarget.print(client.state());
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  LogTarget.print((String)"Message arrived [" + topic + "] ");
  for (int i = 0; i < length; i++) {
    LogTarget.print((char)payload[i]);
  }
  LogTarget.println();

  bool topic_ok = false;
  
#if SPACESTATUS
  if (!strcmp(topic, C_MQTTTOPIC_SPACE)) {
    topic_ok = true;
    if (!strncmp((char *)payload, "1", length)) {
      spacestatus = 1;
    } else {
      spacestatus = 0;
    }
  }
#endif

#if MARQUEE
  if (!strcmp(topic, C_MQTTTOPIC_MSG)) {
    topic_ok = true;
    if (marquee_done) {
      marquee_col = encode_message(scrollbuffer, payload, length);
#if MARQUEE_INTRO
      displaymode = DM_MARQUEE_INTRO;
#else
      displaymode = DM_MARQUEE;
#endif
      marquee_done = false;
      for (int i = 0; i < MAX_X; i++) marquee_columns[i] = 0;
      LogTarget.println((String)"Got new marquee text: " + (char*)scrollbuffer);
    } else {
      LogTarget.println("Still busy displaying a marquee text, ignoring this message");
    }
  }
#endif

  if (!topic_ok) {
    LogTarget.println((String)"Unknown topic: " + topic);
  }
}
#endif

void loop()
{
  action();

#if CLOCK
  events();
  delay(100);
  ArduinoOTA.handle();
#if SPACESTATUS || MARQUEE
  if (!client.connected()) {
    spacestatus = -1;
    reconnect();
  }
  client.loop();
#endif
#else
  // simulation only
  delay(10);
#endif

}
