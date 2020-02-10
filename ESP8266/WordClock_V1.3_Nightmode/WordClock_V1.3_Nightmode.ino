// TODO
// Config (PIN / MQTT / defines) auslagern
// Code aufräumen / auf platformio switchen

#include "config.h"

#ifdef CLOCK
#include <ezTime.h>
//#include <WiFi.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <WiFiManager.h>  
#include <Adafruit_NeoPixel.h>
Timezone myTZ;
#endif

#include "colorconv.h"

#ifdef SPACEAPI
#include <PubSubClient.h>
WiFiClient wifiClient;
PubSubClient client(wifiClient);
boolean spacestatus = false;
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
const char* matrix[] = {  // only used for debugging output
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
  "HÜTTEHAUFZUDB"
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
const long C_HUE      = 0x000c02;
const long C_HUETTE   = 0x000c05;
const long C_AUF      = 0x060c03;
const long C_ZU       = 0x090c02;
const long C_DB       = 0x0b0c02;

const int hours[] = { C_12, C_1, C_2, C_3,  C_4,  C_5,  C_6,  C_7,  C_8,  C_9,  C_10,  C_11,  C_12 };

long ledcodes[15] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int ledcodes_idx = 0;
String txt;

#ifdef TESTPANEL

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

const int day_brightness = 100;
const int night_brightness = 5;
const int day_h = 8;
const int day_m = 0;
const int night_h = 22;
const int night_m = 00;

int brightness = day_brightness;

int panel2strip(int x, int y)
{
  // for 20x15 panel on grey eurobox lid
  //return y * panel_x + ( (y & 1) ? (panel_x - x - 1) : x);
  // for 13x13 panel in RIBBA frame
  return (panel_y - y - 1) * panel_x + ( (y & 1) ? (panel_x - x - 1) : x);
}

Adafruit_NeoPixel strip = Adafruit_NeoPixel(300, D4, NEO_GRB + NEO_KHZ800);

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
  int hrs = (mm > 30) ? hnext : hh;
  switch (((mm + 2) % 60) / 5)
  {
    case 0:
      txt = txt + hrs + " Uhr";
      ledcodes[ledcodes_idx++] = (hrs == 1) ?
                                 C_1x :             // "EIN UHR" instead of "EINS UHR"
                                 hours[hrs];
      ledcodes[ledcodes_idx++] = C_UHR;
      break;
    case 1:
      txt = txt + "fünf nach " + hh;
      ledcodes[ledcodes_idx++] = C_FUENF;
      ledcodes[ledcodes_idx++] = C_MINUTEN;
      ledcodes[ledcodes_idx++] = C_NACH;
      ledcodes[ledcodes_idx++] = hours[hh];
      break;
    case 2:
      txt = txt + "zehn nach " + hh;
      ledcodes[ledcodes_idx++] = C_ZEHN;
      ledcodes[ledcodes_idx++] = C_MINUTEN;
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
      ledcodes[ledcodes_idx++] = C_MINUTEN;
      ledcodes[ledcodes_idx++] = C_NACH;
      ledcodes[ledcodes_idx++] = hours[hh];
      break;
    case 5:
      txt = txt + "fünf vor halb " + hnext;
      ledcodes[ledcodes_idx++] = C_FUENF;
      ledcodes[ledcodes_idx++] = C_MINUTEN;
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
      ledcodes[ledcodes_idx++] = C_MINUTEN;
      ledcodes[ledcodes_idx++] = C_NACH;
      ledcodes[ledcodes_idx++] = C_HALB;
      ledcodes[ledcodes_idx++] = hours[hnext];
      break;
    case 8:
      txt = txt + "zwanzig vor " + hnext;
      ledcodes[ledcodes_idx++] = C_ZWANZIG;
      ledcodes[ledcodes_idx++] = C_MINUTEN;
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
      ledcodes[ledcodes_idx++] = C_MINUTEN;
      ledcodes[ledcodes_idx++] = C_VOR;
      ledcodes[ledcodes_idx++] = hours[hnext];
      break;
    case 11:
      txt = txt + "fünf vor " + hnext;
      ledcodes[ledcodes_idx++] = C_FUENF;
      ledcodes[ledcodes_idx++] = C_MINUTEN;
      ledcodes[ledcodes_idx++] = C_VOR;
      ledcodes[ledcodes_idx++] = hours[hnext];
      break;
  }
  #ifdef SPACEAPI
  ledcodes[ledcodes_idx++] = C_HUETTE;
  if(spacestatus==true) {
    ledcodes[ledcodes_idx++] = C_AUF;
  } else {
    ledcodes[ledcodes_idx++] = C_ZU;
  }
  #endif
}

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
    //Serial.println("move color map");
    int dir = random(2);
    pat_offset_x = (pat_offset_x + (dir != 2 ? 1 : 0)) % pat_x;
    pat_offset_y = (pat_offset_y + (dir != 0 ? 1 : 0)) % pat_y;
    cycle = (cycle + 1) % 1000;
    if (cycle == 0) change_colorscheme(1);
  } else {
    // calculate new color map
    Serial.println("generate new color map");
    for (int peak = 0; peak < num_peaks; peak++) {
      peak_coord_x[peak] = random(pat_x);
      peak_coord_y[peak] = random(pat_y);
      peak_height[peak] = 50 + random(100);
      peak_reach[peak] = 5 + random(2 * pat_y);
      Serial.println(String("peak ") + peak + " = pos (" + peak_coord_x[peak] + ", " + peak_coord_y[peak] + "), h " + peak_height[peak] + ", r " + peak_reach[peak]);
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
    //simple rectangular grid distance
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

void set_matrix()
{
  Serial.println("set_matrix");
  for (int i = 0; i < MAX_X; i += 1) {
    Serial.print(String(" row ") + i + ": ");
    //int j = i; {
    for (int j = 0; j < MAX_Y; j += 1) {
      Serial.print(String("") + j + " ");
      //strip.setPixelColor(panel2strip(i + offset_x, j + offset_y), strip.Color(i * brightness / MAX_X, brightness, j * brightness / MAX_X));
      strip.setPixelColor(panel2strip(i + offset_x, j + offset_y), xy2col(i, j, 20));
      strip.show();
      /*delay(10);
        strip.setPixelColor(panel2strip(i + offset_x, j + offset_y), 0);
        strip.show();
        delay(10);*/
    }
    Serial.println();
  }
  strip.show();
  delay(100);
}



void set_LED(int i, int j, int subbrightness = 100)
{
  strip.setPixelColor(panel2strip(i + offset_x, (MAX_Y - j - 1) + offset_y), xy2col(i, j, subbrightness));
  //strip.show();
}

void show_text(int subbrightness = 100)
{
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
          set_LED(xpos + j, ypos, subbrightness);
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
  change_colorscheme();
}

int h0 = 0, m0 = 0;

void show_time(int h, int m, int s = 0)
{
  if (h != h0 || m != m0)
    change_colorscheme(1);
  h0 = h; m0 = m;
  clearLEDs();
  encode_time(h, m, s);
  Serial.println(String() + h + ":" + m + ":" + s + " -> " + txt + "  (" + ledcodes_idx + " Wörter)");
  show_text();
}

int h = 0, m = 0, s = 0, s0 = 0;
void action()
{
#ifdef CLOCK
  s = myTZ.second();
  if (s == s0) return;
  s0 = s;
  m = myTZ.minute();
  h = myTZ.hour();
  brightness = is_day(h, m) ? day_brightness : night_brightness;
  show_time(h, m, s);
#else
#if 1
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

#ifdef SPACEAPI
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient", C_MQTTUSER, C_MQTTPASSWORD)) {
      Serial.println("connected");
      // ... and resubscribe
      client.subscribe(C_MQTTTOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
#endif

void setup()
{
  Serial.begin(115200);
  while (!Serial) {
    ;  // wait for Serial port to connect. Needed for native USB port only
  }
  Serial.println("WordClock V1.3");
  
  WiFiManager wifiManager;

  strip.begin();
  strip.show();

  randomSeed(analogRead(0) + millis() + 2); // This is pretty deterministic, so the color scheme will always be the same.
  // Change to another seed value for a different color scheme after your fancy.
  change_colorscheme(1);
  set_matrix();

  ledcodes_idx = 0;
  ledcodes[ledcodes_idx++] = C_DB;
  ledcodes[ledcodes_idx++] = C_HUE;
  show_text(250);
  delay(1000);

#ifdef CLOCK
  ledcodes_idx = 0;
  ledcodes[ledcodes_idx++] = C_WAN;
  show_text(250);
  delay(1000);

   wifiManager.autoConnect("Katers-WordClock");
  /*while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
    }*/
  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("WordClock");

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
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();

  ledcodes_idx = 0;
  ledcodes[ledcodes_idx++] = C_ZEIT;
  show_text(250);

  setDebug(DEBUG);
  waitForSync();

  Serial.println();
  Serial.println("UTC:             " + UTC.dateTime());

  if (myTZ.setLocation(F("de"))) {
    ledcodes_idx = 0;
    ledcodes[ledcodes_idx++] = C_DE;
    show_text(250);
    Serial.print(F("Germany:         "));
    Serial.println(myTZ.dateTime());
  } else {
    ledcodes_idx = 0;
    ledcodes[ledcodes_idx++] = C_WZ1;
    ledcodes[ledcodes_idx++] = C_WZ2;
    show_text(250);
  }
  delay(5000);

  // See if local time can be obtained (does not work in countries that span multiple timezones)
  Serial.print(F("Local (GeoIP):   "));
  if (myTZ.setLocation()) {
    Serial.println(myTZ.dateTime());
  } else {
    Serial.println(String("Error: ") + errorString());
  }

  randomSeed(analogRead(0) + millis()); // This will be definitely non-deterministic as it includes network timing.
#endif

  change_colorscheme(1);

#ifdef SPACEAPI
  client.setServer(C_MQTTSERVER, 1883);
  client.setCallback(callback);
#endif

}

#ifdef SPACEAPI
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  if(!strncmp((char *)payload, "1", length)) {
    spacestatus = true;
  } else {
    spacestatus = false;
  }
  Serial.println();
}
#endif

void loop()
{
  action();
#ifdef CLOCK
  events();
  delay(100);
#else
  delay(10);
#endif

  ArduinoOTA.handle();
#ifdef SPACEAPI
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
#endif

}
