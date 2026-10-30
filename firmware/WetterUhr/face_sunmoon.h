#pragma once
/*
  face_sunmoon.h - Zifferblatt "Sonne und Mond"
  Zeigt den echten Stand von Sonne und Mond fuer deinen Standort (Breite/Laenge aus den Einstellungen),
  berechnet aus Datum und Uhrzeit (astro.h, keine Internetdaten noetig):
    - Diagramm: Hoehe von Sonne (gelb) und Mond (silber) ueber den ganzen Tag, Nachtbereich dunkel,
      Daemmerung abgestuft, senkrechte Linie = jetzt
    - Auf- und Untergang, aktuelle Hoehe, Mondphase als Scheibe mit Prozentwert
  Uebergang: Eine Sonnenscheibe waechst von der Mitte.
*/
#include "faces_fx.h"
#include "astro.h"

namespace f_sunmoon {
using namespace fc;

enum : uint8_t { M_BG = 1, M_DAY, M_TW1, M_TW2, M_NIGHT, M_HOR, M_SUN, M_SUND, M_MOON, M_MOOND, M_NOW, M_DIG, M_SH, M_DIM,
                 M_DISC, M_LIT, M_GRID };

static const int NS = 97;                       // Stuetzstellen: 0:00 .. 24:00 im 15-Minuten-Raster
static float    sunA[NS], moonA[NS], tmpS[NS], tmpM[NS];
static int      fillIdx;                        // naechste zu berechnende Stuetzstelle (>= NS: fertig)
static double   fillMid, shownMid;               // lokale Mitternacht (UTC-Sekunden) der laufenden / angezeigten Berechnung
static bool     haveShown;
static char     sRise[8], sSet[8], mRise[8], mSet[8];
static float    curSunAlt, curMoonAlt, curSunAz, curMoonAz, curElong, curIllum;
static uint8_t  lastSs;
static uint32_t lastT;

static void enter(uint32_t now) {
  pal(M_BG, 10, 14, 34); pal(M_DAY, 70, 130, 200); pal(M_TW1, 42, 62, 120); pal(M_TW2, 24, 34, 82); pal(M_NIGHT, 12, 16, 44);
  pal(M_HOR, 200, 200, 220); pal(M_SUN, 255, 214, 70); pal(M_SUND, 120, 96, 40); pal(M_MOON, 214, 224, 240); pal(M_MOOND, 84, 92, 110);
  pal(M_NOW, 255, 120, 120); pal(M_DIG, 255, 248, 230); pal(M_SH, 4, 6, 20); pal(M_DIM, 150, 160, 190);
  pal(M_DISC, 44, 50, 70); pal(M_LIT, 236, 236, 214); pal(M_GRID, 52, 62, 100);
  fillIdx = NS; fillMid = 0; shownMid = 0; haveShown = false; lastSs = 255; lastT = now;
  strcpy(sRise, "--:--"); strcpy(sSet, "--:--"); strcpy(mRise, "--:--"); strcpy(mSet, "--:--");
  curSunAlt = curMoonAlt = 0; curElong = 0; curIllum = 0;
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, M_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, M_DIG); }

static void fmtTime(char* out, double t, double mid) {      // t: UTC-Sekunden -> "HH:MM" relativ zur lokalen Mitternacht
  int m = (int)((t - mid) / 60.0 + 0.5);
  if (m < 0) m = 0; if (m > 1439) m = 1439;
  snprintf(out, 8, "%02d:%02d", m / 60, m % 60);
}

// Auf-/Untergang aus den Stuetzstellen suchen (lineare Interpolation)
static void findEvents(const float* a, double mid, float h0, char* rise, char* set) {
  strcpy(rise, "--:--"); strcpy(set, "--:--");
  bool gotR = false, gotS = false;
  for (int i = 0; i + 1 < NS; i++) {
    const float x0 = a[i] - h0, x1 = a[i + 1] - h0;
    if (x0 < 0 && x1 >= 0 && !gotR) { gotR = true; fmtTime(rise, mid + (i + x0 / (x0 - x1)) * 900.0, mid); }
    if (x0 >= 0 && x1 < 0 && !gotS) { gotS = true; fmtTime(set, mid + (i + x0 / (x0 - x1)) * 900.0, mid); }
  }
}

static void step(const FaceCtx& c) {                         // Berechnung scheibchenweise: 3 Stuetzstellen je Bild
  if (!c.timeOk || c.unixT <= 0) return;
  const double mid = c.unixT - (double)(c.hh * 3600 + c.mm * 60 + c.ss);
  if (fillIdx >= NS && (!haveShown || fabs(mid - shownMid) > 600.0) && fabs(mid - fillMid) > 600.0) { fillIdx = 0; fillMid = mid; }
  for (int k = 0; k < 3 && fillIdx < NS; k++, fillIdx++) {
    float az;
    const double t = fillMid + fillIdx * 900.0;
    astro::sunAltAz(t, c.lat, c.lon, tmpS[fillIdx], az);
    astro::moonAltAz(t, c.lat, c.lon, tmpM[fillIdx], az);
    if (fillIdx == NS - 1) {
      memcpy(sunA, tmpS, sizeof(sunA)); memcpy(moonA, tmpM, sizeof(moonA));
      shownMid = fillMid; haveShown = true;
      findEvents(sunA, shownMid, -0.833f, sRise, sSet);
      findEvents(moonA, shownMid, -0.7f, mRise, mSet);
    }
  }
}

static void moonDisc(int cx, int cy, int R, float elong, bool south) {
  const float ce = cosf(astro::rad(elong));
  for (int y = -R; y <= R; y++) {
    const int w = (int)(sqrtf((float)(R * R - y * y)) + 0.5f);
    fb->drawFastHLine(cx - w, cy + y, 2 * w + 1, M_DISC);
    int x0, x1;
    if (elong <= 180.0f) { x0 = (int)(w * ce); x1 = w; }
    else { x0 = -w; x1 = (int)(-w * ce); }
    if (x1 < x0) continue;
    if (south) { const int t = -x0; x0 = -x1; x1 = t; }
    fb->drawFastHLine(cx + x0, cy + y, x1 - x0 + 1, M_LIT);
  }
  fb->drawCircle(cx, cy, R, M_MOOND);
}

static const char* phaseName(float e) {
  if (e < 12 || e >= 348) return "NEUMOND";
  if (e < 78) return "ZUNEHMEND";
  if (e < 102) return "1. VIERTEL";
  if (e < 168) return "ZUNEHMEND";
  if (e < 192) return "VOLLMOND";
  if (e < 258) return "ABNEHMEND";
  if (e < 282) return "3. VIERTEL";
  return "ABNEHMEND";
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  frameDt(now, lastT);
  step(c);
  if (c.timeOk && c.unixT > 0 && c.ss != lastSs) {           // einmal je Sekunde den aktuellen Stand rechnen
    lastSs = c.ss;
    astro::sunAltAz(c.unixT, c.lat, c.lon, curSunAlt, curSunAz);
    astro::moonAltAz(c.unixT, c.lat, c.lon, curMoonAlt, curMoonAz);
    astro::moonPhase(c.unixT, curElong, curIllum);
  }

  fb->fillScreen(M_BG);

  // ---- Diagramm ----
  const int X0 = 6, X1 = 154, Y0 = 27, Y1 = 68;               // Zeichenflaeche
  const float aMax = 66.0f, aMin = -20.0f;
  const float ppd = (float)(Y1 - Y0) / (aMax - aMin);
  auto yOf = [&](float a) { int y = Y0 + (int)((aMax - a) * ppd + 0.5f); return y < Y0 ? Y0 : (y > Y1 ? Y1 : y); };
  const int yh = yOf(0), y6 = yOf(-6), y12 = yOf(-12);
  fb->fillRect(X0, Y0, X1 - X0 + 1, yh - Y0 + 1, M_DAY);                          // Tag
  fb->fillRect(X0, yh, X1 - X0 + 1, y6 - yh + 1, M_TW1);                          // buergerliche Daemmerung
  fb->fillRect(X0, y6, X1 - X0 + 1, y12 - y6 + 1, M_TW2);                         // nautische
  fb->fillRect(X0, y12, X1 - X0 + 1, Y1 - y12 + 1, M_NIGHT);                      // Nacht
  for (int h = 0; h <= 24; h += 6) {
    const int x = X0 + (X1 - X0) * h / 24;
    fb->drawFastVLine(x, Y0, Y1 - Y0 + 1, M_GRID);
    char b[4]; snprintf(b, sizeof(b), "%d", h % 24 == 0 && h ? 24 : h);
    txtc(x < 20 ? x + 3 : (x > 140 ? x - 4 : x), Y1 + 3, b, M_DIM);
  }
  fb->drawFastHLine(X0, yh, X1 - X0 + 1, M_HOR);
  if (haveShown) {
    int px = 0, pys = 0, pym = 0;
    for (int i = 0; i < NS; i++) {
      const int x = X0 + (X1 - X0) * i / (NS - 1), ys = yOf(sunA[i]), ym = yOf(moonA[i]);
      if (i) {
        fb->drawLine(px, pym, x, ym, moonA[i] > 0 ? M_MOON : M_MOOND);
        fb->drawLine(px, pys, x, ys, sunA[i] > -0.8f ? M_SUN : M_SUND);
        fb->drawLine(px, pys + 1, x, ys + 1, sunA[i] > -0.8f ? M_SUN : M_SUND);
      }
      px = x; pys = ys; pym = ym;
    }
  }
  if (c.timeOk) {                                              // jetzt
    const int mins = c.hh * 60 + c.mm;
    const int xn = X0 + (X1 - X0) * mins / 1440;
    fb->drawFastVLine(xn, Y0, Y1 - Y0 + 1, M_NOW);
    fb->fillCircle(xn, yOf(curSunAlt), 2, M_SUN); fb->drawCircle(xn, yOf(curSunAlt), 3, M_SH);
    fb->fillCircle(xn, yOf(curMoonAlt), 2, M_MOON); fb->drawCircle(xn, yOf(curMoonAlt), 3, M_SH);
  }

  // ---- Uhrzeit und Datum ----
  drawTime(4, 3, 3, c, cellS);
  drawTime(4, 3, 3, c, cellD);
  if (c.timeOk) {
    char b[24];
    snprintf(b, sizeof(b), "%s %02u.%02u.", WD_SHORT[c.wday % 7], c.mday, c.mon);
    txtr(FB_W - 4, 4, b, M_DIG);
  }
  if (c.hasWx) tempTxt(FB_W - 4 - tempW(c.temp, 1), 15, c.temp, M_DIM);

  // ---- Werte ----
  char b[24];
  txt(3, 81, "SONNE", M_SUN);
  snprintf(b, sizeof(b), "AUF %s", sRise); txt(3, 90, b, M_DIG);
  snprintf(b, sizeof(b), "UNT %s", sSet);  txt(3, 98, b, M_DIG);
  snprintf(b, sizeof(b), "ALT %d", (int)lroundf(curSunAlt)); txt(3, 106, b, M_DIM);
  txt(62, 81, "MOND", M_MOON);
  snprintf(b, sizeof(b), "AUF %s", mRise); txt(62, 90, b, M_DIG);
  snprintf(b, sizeof(b), "UNT %s", mSet);  txt(62, 98, b, M_DIG);
  snprintf(b, sizeof(b), "ALT %d", (int)lroundf(curMoonAlt)); txt(62, 106, b, M_DIM);
  moonDisc(137, 99, 9, curElong, c.lat < 0);
  snprintf(b, sizeof(b), "%d%%", (int)lroundf(curIllum * 100.0f));
  txtc(137, 111, b, M_DIG);
  txtr(FB_W - 2, 81, phaseName(curElong), M_DIM);
}

static void cover(float p, bool, uint32_t) {
  if (p <= 0.0f) return;
  uint8_t* bf = buf();
  if (p >= 0.97f) { memset(bf, M_SUN, FB_W * FB_H); return; }
  const int r = (int)(p / 0.97f * 105.0f);
  const int cx = 80, cy = 60;
  for (int y = 0; y < FB_H; y++) {
    const int dy = y - cy;
    if (dy * dy > r * r) continue;
    const int w = (int)sqrtf((float)(r * r - dy * dy));
    const int x0 = cx - w < 0 ? 0 : cx - w, x1 = cx + w >= FB_W ? FB_W - 1 : cx + w;
    memset(bf + y * FB_W + x0, M_SUN, x1 - x0 + 1);
  }
}

}  // namespace f_sunmoon
