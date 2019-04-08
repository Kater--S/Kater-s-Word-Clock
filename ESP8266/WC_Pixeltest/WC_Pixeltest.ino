
#define CLOCK



#ifdef CLOCK
#include <ezTime.h>
//#include <WiFi.h>
#include <ESP8266WiFi.h>

Timezone myTZ;
#endif

#include <Adafruit_NeoPixel.h>

/*

   Zeit-Umschreibung:

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

   Matrix:
   x
   0123456789abc
  y ESEISTLWARMPY
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
  c SKAFFEESSENDB

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
    KAFFEEssen

   Coding:
   x=4, y=7, len=3 -> 0x040703;
*/

const int MAX_X = 13; // number of columns
const int MAX_Y = 13; // number of rows
const char* matrix[] = {
  "ESEISTLWARMPY",
  "GERADEGLEICHZ",
  "PUNKTETWAKALT",
  "ZWANZIGZEHNEU",
  "FÜNFEMINUTENV",
  "VIERTELPNACHT",
  "VORTHALBGELFE",
  "ZWÖLFÜNFZWEIG",
  "ZEHNEUNACHTAN",
  "UNDREINSFVIER",
  "SECHSIEBENASS",
  "QUHREZEITWFÜR",
  "SKAFFEESSENDB"
};

const long C_ES       = 0x000002;
const long C_IST      = 0x030003;
const long C_WAR      = 0x070003;
const long C_WARM     = 0x070004;
const long C_GERADE   = 0x000106;
const long C_GLEICH   = 0x060106;
const long C_PUNKT    = 0x000205;
const long C_ETWA     = 0x050204;
const long C_KALT     = 0x090204;
const long C_ZWANZIG  = 0x000307;
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
const long C_1        = 0x040904;
const long C_4        = 0x090904;
const long C_6        = 0x000a05;
const long C_7        = 0x040a06;
const long C_NASS     = 0x090a04;
const long C_UHR      = 0x010b03;
const long C_ZEIT     = 0x050b04;
const long C_FUER     = 0x0a0b04;
const long C_SK       = 0x000c02;
const long C_KAFFEE   = 0x010c06;
const long C_ESSEN    = 0x060c05;
const long C_DB       = 0x0b0c02;

const int hours[] = { C_12, C_1, C_2, C_3,  C_4,  C_5,  C_6,  C_7,  C_8,  C_9,  C_10,  C_11,  C_12 };

long ledcodes[15] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int ledcodes_idx = 0;
String txt;

/*
   Behelfsmatrix: 20x15-Panel

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

int panel2strip(int x, int y)
{
  return y * panel_x + ( (y & 1) ? (panel_x - x - 1) : x);
}

Adafruit_NeoPixel strip = Adafruit_NeoPixel(300, D2, NEO_GRB + NEO_KHZ800);

void set_matrix()
{
  for (int i = 0; i < MAX_X; i += 1)
    for (int j = 0; j < MAX_Y; j += 1)
      strip.setPixelColor(panel2strip(i + offset_x, j + offset_y), strip.Color(i * 100 / MAX_X, 100, j * 100 / MAX_X));
  strip.show();
  delay(100);
}


void encode_time(int h, int m, int s = 0)
{
  int hh = h % 12;
  hh += (hh ? 0 : 12);
  int hnext = (hh + 1) % 12;
  hnext += (hnext ? 0 : 12);
  int mm = m;

  ledcodes_idx = 0;
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
  switch (((mm + 2) % 60) / 5)
  {
    case 0:
      txt = txt + (mm > 30 ? hnext : hh) + " Uhr";
      ledcodes[ledcodes_idx++] = hours[(mm > 30 ? hnext : hh)];
      ledcodes[ledcodes_idx++] = C_UHR;
      break;
    case 1:
      txt = txt + "fünf nach " + hh;
      ledcodes[ledcodes_idx++] = C_FUENF;
      ledcodes[ledcodes_idx++] = C_NACH;
      ledcodes[ledcodes_idx++] = hours[hh];
      break;
    case 2:
      txt = txt + "zehn nach " + hh;
      ledcodes[ledcodes_idx++] = C_ZEHN;
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
      txt = txt + "zwanzig nach " + hh;
      ledcodes[ledcodes_idx++] = C_ZWANZIG;
      ledcodes[ledcodes_idx++] = C_NACH;
      ledcodes[ledcodes_idx++] = hours[hh];
      break;
    case 5:
      txt = txt + "fünf vor halb " + hnext;
      ledcodes[ledcodes_idx++] = C_FUENF;
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
      txt = txt + "fünf nach halb " + hnext;
      ledcodes[ledcodes_idx++] = C_FUENF;
      ledcodes[ledcodes_idx++] = C_NACH;
      ledcodes[ledcodes_idx++] = C_HALB;
      ledcodes[ledcodes_idx++] = hours[hnext];
      break;
    case 8:
      txt = txt + "zwanzig vor " + hnext;
      ledcodes[ledcodes_idx++] = C_ZWANZIG;
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
      txt = txt + "zehn vor " + hnext;
      ledcodes[ledcodes_idx++] = C_ZEHN;
      ledcodes[ledcodes_idx++] = C_VOR;
      ledcodes[ledcodes_idx++] = hours[hnext];
      break;
    case 11:
      txt = txt + "fünf vor " + hnext;
      ledcodes[ledcodes_idx++] = C_FUENF;
      ledcodes[ledcodes_idx++] = C_VOR;
      ledcodes[ledcodes_idx++] = hours[hnext];
      break;
  }
}

void clearLEDs()
{
  for (int i = 0; i < 300; i++) strip.setPixelColor(i, 0);
  //strip.show();
}

int colorscheme = 0;
uint32_t xy2col(int i, int j)
{
  switch (colorscheme % 3) {
    case 0:
      return strip.Color(i * 255 / MAX_X, 255, j * 255 / MAX_X);
    case 1:
      return strip.Color(255, i * 255 / MAX_X, j * 255 / MAX_X);
    case 2:
      return strip.Color(i * 255 / MAX_X, j * 255 / MAX_X, 255);
  }
}

void setLED(int i, int j)
{
  strip.setPixelColor(panel2strip(i + offset_x, (MAX_Y - j - 1) + offset_y), xy2col(i, j));
  //strip.show();
}

void showtime(int h, int m, int s = 0)
{
  clearLEDs();
  encode_time(h, m, s);
  Serial.println(String() + h + ":" + m + ":" + s + " -> " + txt + "  (" + ledcodes_idx + " Wörter)");
  /*Serial.println("-> ");
    for(int i = 0; i < ledcodes_idx; i++) {
    Serial.print(String(ledcodes[i], HEX) + " ");
    }*/
  Serial.println("+-------------+");
  for (int y = 0; y < MAX_Y; y++) {
    //Serial.println(String("Reihe ") + y + ": ");
    String row = String("             "); // length = MAX_X
    for (int i = 0; i < ledcodes_idx; i++) {
      long code = ledcodes[i];
      int ypos = (int)(code >> 8) & 0xff;
      //Serial.print(String("teste Wort ") + String(code, HEX) + ": ypos " + ypos + " ");
      if (ypos == y) {
        //Serial.print(" - passt. Zeichen:");
        int xpos = (int)(code >> 16) & 0xff;
        int len = (int)(code >> 0) & 0xff;
        for (int j = 0; j < len; j++) {
          char ch = matrix[y][xpos + j];
          row.setCharAt(xpos + j, ch);
          setLED(xpos + j, ypos);
          //Serial.print(ch);
        }
        /*Serial.println();
          } else {
          Serial.println(" - passt nicht.");*/
      }
    }
    Serial.println(String("|") + row + "|");
  }
  Serial.println("+-------------+");
/*
  for (int i = 0; i < MAX_X; i++)
    for (int j = 0; j < MAX_Y; j++)
      setLED(i, j);
*/
  strip.show();
  colorscheme++;
}

/*
  uint32_t Wheel(byte WheelPos) {
  int full = 255;
  int third = 255 / 3;
  int secondthird = full - third;

  if (WheelPos < third) {
    return strip.Color(WheelPos * 3, full - WheelPos * 3, 0);
  }
  else if (WheelPos < secondthird) {
    WheelPos -= third;
    return strip.Color(full - WheelPos * 3, 0, WheelPos * 3);
  }
  else {
    WheelPos -= secondthir;
    return strip.Color(0, WheelPos * 3, full - WheelPos * 3);
  }
  }
*/

int h = 0, m = 0;

void action()
{
#ifdef CLOCK
  showtime(myTZ.hour(), myTZ.minute(), myTZ.second());
#else
  showtime(h, m);
  m++;
  if (m == 60) {
    m = 0;
    h = (h + 1) % 24;
  }
#endif
}

void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for Serial port to connect. Needed for native USB port only
  }
  Serial.println("Uhrzeit-Test für WordClock");

  strip.begin();
  strip.show();
  /**
    delay(1000);
    for (int i=1; i<300; i += 1) {
    strip.setPixelColor(i, strip.Color(100,0,0));
    strip.setPixelColor(i-1, strip.Color(0,100,0));
    strip.show();
    delay(0);
    }
    set_matrix();
  **/

#ifdef CLOCK
  WiFi.begin("Freifunk", "");
  setDebug(DEBUG);

  waitForSync();

  Serial.println();
  Serial.println("UTC:             " + UTC.dateTime());

  myTZ.setLocation(F("de"));
  Serial.print(F("Germany:         "));
  Serial.println(myTZ.dateTime());


  // See if local time can be obtained (does not work in countries that span multiple timezones)
  Serial.print(F("Local (GeoIP):   "));
  if (myTZ.setLocation()) {
    Serial.println(myTZ.dateTime());
  } else {
    Serial.println(String("Error: ") + errorString());
  }
#endif
}

void loop()
{
  action();
#ifdef CLOCK
  events();
#endif
  delay(1000);
}
