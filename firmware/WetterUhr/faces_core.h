#pragma once
/*
  faces_core.h - gemeinsame Bausteine fuer die Themen-Zifferblaetter

  Prinzip: Alle Themen-Zifferblaetter zeichnen in einen 160x120-Pixel-Puffer mit
  256 Palettenfarben (GFXcanvas8, ca. 19 KB). faces.h verdoppelt ihn auf 320x240
  und sendet nur Kacheln, die sich seit dem letzten Bild geaendert haben.
  Das ergibt echte Retro-Pixeloptik, kein Flackern und wenig SPI-Verkehr.

  Dieses Modul enthaelt: Zeichen-/Textfunktionen, Pixel-Ziffern, Sprites,
  Zufallszahlen und die Datenstruktur FaceCtx (aktuelle Zeit und Wetter).
*/
#include <Arduino.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include "config.h"
#include "icons.h"
#include "lang.h"

#define FB_W 160
#define FB_H 120

// Flugzeugdaten fuer das Flugradar-Zifferblatt (vom Netzwerk-Task der Hauptdatei geliefert)
#define MAX_PLANES 24
struct Plane {
  uint32_t id;           // Kennung (ICAO-Hexadresse), zum Wiedererkennen zwischen zwei Abrufen
  float   x, y;          // km vom Standort (x nach Osten, y nach Norden) zum Zeitpunkt fetchedMs
  float   vx, vy;        // Geschwindigkeit in km/s (Hochrechnung der Position zwischen zwei Abrufen)
  int32_t alt;           // Hoehe in Fuss
  int16_t gs;            // Geschwindigkeit ueber Grund in Knoten
  int16_t trk;           // Kurs 0..359, -1 = unbekannt
  int16_t vr;            // Steig-/Sinkrate in ft/min
  uint8_t ground;        // 1 = am Boden
  char    call[9];       // Rufzeichen
  char    type[5];       // Flugzeugtyp (z. B. A320)
};
struct PlaneList {
  uint8_t  n;            // Anzahl (nach Entfernung sortiert, naechstes zuerst)
  uint8_t  status;       // 0 noch nichts abgerufen, 1 ok, 2 letzter Abruf fehlgeschlagen
  float    radiusKm;
  uint32_t fetchedMs;    // millis() beim Abruf
  Plane    p[MAX_PLANES];
};

// Alles, was ein Zifferblatt zum Zeichnen braucht (wird pro Bild von der Hauptdatei gefuellt)
// Positionsdaten der Internationalen Raumstation (vom issTask geholt)
struct IssData {
  uint8_t  status;                 // 0 = noch keine Daten, 1 = ok, 2 = Abruf fehlgeschlagen
  float    lat, lon;               // letzte gemeldete Position (Grad)
  float    vlat, vlon;             // Geschwindigkeit in Grad pro Sekunde (zum Fortschreiben zwischen Abrufen)
  float    speedKmh;               // aus zwei Positionen berechnet
  float    altKm;                  // Bahnhoehe, falls geliefert (sonst 0)
  uint8_t  crew;                   // Personen auf der ISS (0 = unbekannt)
  uint32_t fetchedMs;              // millis() des letzten erfolgreichen Abrufs
};

struct FaceCtx {
  uint32_t now;                    // millis()
  bool     timeOk;                 // Uhrzeit gueltig?
  uint8_t  hh, mm, ss, wday;       // wday: 0 = Sonntag
  uint8_t  mday, mon;              // mon: 1..12
  uint16_t year;
  bool     showSec;
  bool     hasWx, stale;           // Wetterdaten vorhanden / veraltet
  int      temp, wcode, hum, wind; // Temperatur gerundet, WMO-Code, Feuchte %, Wind km/h
  bool     isDay;
  bool     wifi;
  bool     apMode;                 // Einrichtungs-Access-Point aktiv
  const char* info;                // z. B. IP-Adresse, kurz eingeblendet (oder nullptr)
  const PlaneList* planes;         // nur beim Flugradar gesetzt, sonst nullptr
  const IssData*   iss;            // nur beim ISS-Design gesetzt, sonst nullptr
  float    lat, lon;               // Standort (Grad)
  double   unixT;                  // UTC-Sekunden seit 1970 (0 = Zeit unbekannt)
  uint8_t  opts;                   // Zusatzoptionen: OPT_UFO, OPT_SLEIGH
};
#define OPT_UFO       0x01         // Flugradar: ab und zu ein UFO
#define OPT_SLEIGH    0x02         // ISS-Tracker: Weihnachtsschlitten statt ISS


namespace fc {

static GFXcanvas8* fb = nullptr;
static uint16_t    PAL[256];

// Reservierte Palettenplaetze (Index 0 ist immer Schwarz)
static const uint8_t I_WHITE = 253, I_RED = 254, I_ORANGE = 255;
static const uint8_t I_TICK = 252, I_TICK2 = 251;     // Laufschrift (ticker.h)

typedef void (*CellFn)(int x, int y, int s, int cx, int cy);

// ---------- kleine Helfer ----------
static inline int imin(int a, int b) { return a < b ? a : b; }
static inline int imax(int a, int b) { return a > b ? a : b; }
static inline int icl(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }
static inline float fcl(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }

static uint32_t rs = 2463534242u;
static inline uint32_t rnd() { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return rs; }
static inline int   rn(int n) { return n <= 0 ? 0 : (int)(rnd() % (uint32_t)n); }
static inline float rf() { return (float)(rnd() & 0xFFFF) / 65535.0f; }

static void pal(uint8_t i, uint8_t r, uint8_t g, uint8_t b) { PAL[i] = RGB565C(r, g, b); }
static void palReset() {
  memset(PAL, 0, sizeof(PAL));
  pal(I_WHITE, 240, 240, 240);
  pal(I_RED, 230, 50, 50);
  pal(I_ORANGE, 255, 140, 0);
}

// ---------- Text (eingebaute 5x7-Schrift, transparent) ----------
static void txt(int x, int y, const char* s, uint8_t col, int sz = 1) {
  for (; *s; ++s) { fb->drawChar(x, y, (unsigned char)*s, col, col, sz); x += 6 * sz; }
}
static int txtw(const char* s, int sz = 1) { int n = (int)strlen(s); return n ? n * 6 * sz - sz : 0; }
static void txtc(int cx, int y, const char* s, uint8_t col, int sz = 1) { txt(cx - txtw(s, sz) / 2, y, s, col, sz); }
static void txtr(int rx, int y, const char* s, uint8_t col, int sz = 1) { txt(rx - txtw(s, sz), y, s, col, sz); }

static void upper(char* dst, const char* src, size_t n) {
  size_t i = 0;
  for (; src[i] && i + 1 < n; i++) { char c = src[i]; dst[i] = (c >= 'a' && c <= 'z') ? (char)(c - 32) : c; }
  dst[i] = 0;
}

#define WD_FULL  (WDF_L[LANG_IDX])     // Wochentage in der eingestellten Sprache (lang.h)
#define WD_SHORT (WDS_L[LANG_IDX])

// Temperatur mit gezeichnetem Gradzeichen, Rueckgabe: Breite in Pixeln
static int tempW(int t, int sz) {
  char b[8];
  snprintf(b, sizeof(b), "%d", t);
  return txtw(b, sz) + 3 * sz + 2 + 5 * sz;
}
static int tempTxt(int x, int y, int t, uint8_t col, int sz = 1) {
  char b[8];
  snprintf(b, sizeof(b), "%d", t);
  int w = txtw(b, sz);
  txt(x, y, b, col, sz);
  const int rx = x + w + 1 + sz;
  for (int k = 0; k < 3; k++) {            // Ring 3x3 (mal sz)
    fb->fillRect(rx + k * sz, y, sz, sz, col);
    fb->fillRect(rx + k * sz, y + 2 * sz, sz, sz, col);
  }
  fb->fillRect(rx, y + sz, sz, sz, col);
  fb->fillRect(rx + 2 * sz, y + sz, sz, sz, col);
  txt(rx + 3 * sz + sz, y, "C", col, sz);
  return w + 3 * sz + 2 + 5 * sz;
}

// ---------- Pixel-Ziffern 5x7 ----------
static const uint8_t DIG[11][7] = {
  {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E},  // 0
  {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},  // 1
  {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F},  // 2
  {0x1E, 0x01, 0x01, 0x0E, 0x01, 0x01, 0x1E},  // 3
  {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02},  // 4
  {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E},  // 5
  {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E},  // 6
  {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},  // 7
  {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E},  // 8
  {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C},  // 9
  {0x00, 0x00, 0x00, 0x0E, 0x00, 0x00, 0x00},  // Strich (keine Zeit)
};

// Ein Zeichen aus Zellen der Groesse s zeichnen; fn() zeichnet jede einzelne Zelle
static void glyph(int x, int y, int d, int s, CellFn fn) {
  if (d < 0 || d > 10) d = 10;
  for (int cy = 0; cy < 7; cy++)
    for (int cx = 0; cx < 5; cx++)
      if (DIG[d][cy] & (0x10 >> cx)) fn(x + cx * s, y + cy * s, s, cx, cy);
}

// HH:MM aus Zellen. Gesamtbreite = 27 * s, Hoehe = 7 * s
static int timeW(int s) { return 27 * s; }
static void drawTime(int x, int y, int s, const FaceCtx& c, CellFn fn) {
  const int a = c.timeOk ? c.hh / 10 : 10, b = c.timeOk ? c.hh % 10 : 10;
  const int d = c.timeOk ? c.mm / 10 : 10, e = c.timeOk ? c.mm % 10 : 10;
  glyph(x,          y, a, s, fn);
  glyph(x + 6 * s,  y, b, s, fn);
  fn(x + 13 * s, y + 2 * s, s, 2, 2);        // Doppelpunkt
  fn(x + 13 * s, y + 4 * s, s, 2, 4);
  glyph(x + 16 * s, y, d, s, fn);
  glyph(x + 22 * s, y, e, s, fn);
}

// ---------- Sprites aus Textzeilen ----------
// '.' und ' ' sind transparent, alle anderen Zeichen werden ueber keys/cols in Palettenindizes uebersetzt.
// ori: 1 = horizontal spiegeln, 2 = vertikal spiegeln, 4 = Zeilen/Spalten tauschen (90 Grad)
static void blit(const char* const* rows, int w, int h, int x, int y,
                 const char* keys, const uint8_t* cols, int ori = 0) {
  for (int sy = 0; sy < h; sy++) {
    const char* r = rows[sy];
    for (int sx = 0; sx < w; sx++) {
      const char ch = r[sx];
      if (ch == '.' || ch == ' ' || ch == 0) continue;
      const char* k = strchr(keys, ch);
      if (!k) continue;
      int px = sx, py = sy;
      if (ori & 1) px = w - 1 - px;
      if (ori & 2) py = h - 1 - py;
      if (ori & 4) { int t = px; px = py; py = t; }
      fb->drawPixel(x + px, y + py, cols[k - keys]);
    }
  }
}

// ---------- gemeinsame Grafik-Helfer ----------
static const uint8_t BAYER4[16] = {0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5};

static inline uint8_t* buf() { return fb->getBuffer(); }

// Gemeinsamer Ausblend-Effekt: geordnetes Raster ueber den ganzen Puffer (p: 0 = nichts, 1 = alles)
static void coverDither(float p, uint8_t col = 0) {
  const int th = (int)(p * 16.0f + 0.5f);
  if (th <= 0) return;
  uint8_t* b = buf();
  if (th >= 16) { memset(b, col, FB_W * FB_H); return; }
  for (int y = 0; y < FB_H; y++)
    for (int x = 0; x < FB_W; x++)
      if (BAYER4[(y & 3) * 4 + (x & 3)] < th) b[y * FB_W + x] = col;
}

// ---------- Kalender-Helfer (fuer Feiertags-Zifferblaetter) ----------
// Tag im Jahr (1..366)
static int dayOfYear(int y, int m, int d) {
  static const int cum[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
  const bool leap = (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
  return cum[(m - 1) % 12] + d + ((leap && m > 2) ? 1 : 0);
}
static int daysInYear(int y) { return ((y % 4 == 0 && y % 100 != 0) || y % 400 == 0) ? 366 : 365; }
// Tage von (y,m,d) bis zum naechsten (tm,td); 0 = heute ist der Tag
static int daysUntil(int y, int m, int d, int tm, int td) {
  const int a = dayOfYear(y, m, d), b = dayOfYear(y, tm, td);
  return b >= a ? b - a : daysInYear(y) - a + dayOfYear(y + 1, tm, td);
}
// Wochentag (0 = Sonntag) nach Sakamoto
static int wdayOf(int y, int m, int d) {
  static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  if (m < 3) y -= 1;
  return (y + y / 4 - y / 100 + y / 400 + t[(m - 1) % 12] + d) % 7;
}
// Sekunden von "jetzt" (unixT) bis zum lokalen Zeitpunkt y-m-d h:00 (negativ = vorbei). Nutzt die eingestellte Zeitzone.
static long secsUntilLocal(double unixT, int y, int m, int d, int h) {
  struct tm t;
  memset(&t, 0, sizeof(t));
  t.tm_year = y - 1900; t.tm_mon = m - 1; t.tm_mday = d; t.tm_hour = h; t.tm_isdst = -1;
  return (long)((double)mktime(&t) - unixT);
}
// Ostersonntag (Gauss / Meeus), Ergebnis Monat und Tag
static void easterDate(int y, int& m, int& d) {
  const int a = y % 19, b = y / 100, c = y % 100, dd = b / 4, e = b % 4, f = (b + 8) / 25, g = (b - f + 1) / 3;
  const int h = (19 * a + b - dd - g + 15) % 30, i = c / 4, k = c % 4, l = (32 + 2 * e + 2 * i - h - k) % 7;
  const int mm = (a + 11 * h + 22 * l) / 451;
  m = (h + l - 7 * mm + 114) / 31; d = ((h + l - 7 * mm + 114) % 31) + 1;
}

// Zeitdifferenz in ms, die auch beim Ueberlauf von millis() stimmt
static inline float secs(uint32_t now, uint32_t since) { return (float)(uint32_t)(now - since) * 0.001f; }

}  // namespace fc
