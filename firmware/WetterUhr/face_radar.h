#pragma once
/*
  face_radar.h - Zifferblatt "Flugradar"
  Echte Flugzeuge in der Naehe, wie auf einem Radarschirm: Der Standort (Breiten-/Laengengrad
  aus den Einstellungen) liegt in der Mitte, Norden ist oben. Die Flugzeuge kommen aus offenen
  ADS-B-Daten (adsb.lol, airplanes.live, adsb.fi) und werden alle paar Sekunden abgerufen;
  dazwischen wird ihre Position anhand von Kurs und Geschwindigkeit weitergerechnet, damit sie
  fluessig fliegen. Jedes Flugzeug ist ein Pfeil in Kursrichtung mit kurzer Spur, die Farbe zeigt die
  Hoehe (Boden grau, niedrig cyan, dann gruen, gelb, orange). Ein Strahl kreist ueber dem Schirm.
  Rechts: Uhrzeit, Datum, Temperatur und eine Infotafel zum jeweils markierten Flugzeug
  (Rufzeichen, Typ, Hoehe, Geschwindigkeit, Entfernung); alle fuenf Sekunden kommt das naechste dran.
  UFO (abschaltbar): Ab und zu taucht fuer ca. 20 Sekunden ein "unbekanntes Objekt" auf. Es schwebt,
  springt ruckartig an neue Stellen, blinkt in Magenta und Gruen; die Infotafel zeigt TR(D_UFO_EX) mit
  unmoeglichen Werten. Nur ein Spass, keine echten Daten.
  Uebergang: Ein Radarring waechst aus der Mitte.
*/
#include "faces_fx.h"

namespace f_radar {
using namespace fc;

enum : uint8_t { R_SCOPE = 1, R_RING, R_RING2, R_SW1, R_SW2, R_SW3, R_SW4, R_GND, R_A1, R_A2, R_A3, R_A4,
                 R_T1, R_T2, R_T3, R_T4, R_PANEL, R_BORDER, R_TXT, R_DIM, R_DIG, R_SH, R_AMB, R_ERR, R_UFO, R_UFO2 };

static const int CX = 58, CY = 61, RAD = 55;

static uint32_t lastT, curFetch;
// Gleitende Positionskorrektur: Beim neuen Abruf springt ein Flugzeug nicht, sondern gleitet in 2,5 s zur neuen Position
static uint32_t shId[MAX_PLANES];
static float    shX[MAX_PLANES], shY[MAX_PLANES], offX[MAX_PLANES], offY[MAX_PLANES];
static int      shN;

// UFO
static uint32_t ufoAt, ufoT0, ufoMoveT;
static float    ufoHold;
static bool     ufoOn;
static float    ufoX, ufoY, ufoFX, ufoFY, ufoTX, ufoTY;    // Position in Pixeln relativ zur Mitte, Start/Ziel einer Bewegung
static bool     kick = false;                               // vom Browser gesetzt: UFO sofort zeigen
static const uint32_t UFO_MS = 20000;

static void ufoTarget(uint32_t now) {
  const float a = rf() * 6.2832f, r = (0.25f + rf() * 0.6f) * RAD;
  ufoFX = ufoX; ufoFY = ufoY; ufoTX = sinf(a) * r; ufoTY = -cosf(a) * r; ufoMoveT = now;
  ufoHold = 0.8f + rf() * 1.4f;
}

static void enter(uint32_t now) {
  pal(R_SCOPE, 2, 20, 12); pal(R_RING, 12, 66, 38); pal(R_RING2, 26, 120, 66);
  pal(R_SW1, 110, 255, 160); pal(R_SW2, 30, 110, 60); pal(R_SW3, 18, 72, 40); pal(R_SW4, 10, 44, 26);
  pal(R_GND, 120, 130, 128); pal(R_A1, 90, 230, 255); pal(R_A2, 120, 255, 120); pal(R_A3, 255, 232, 70); pal(R_A4, 255, 140, 60);
  pal(R_T1, 40, 110, 128); pal(R_T2, 56, 128, 56); pal(R_T3, 128, 116, 36); pal(R_T4, 128, 70, 30);
  pal(R_PANEL, 5, 14, 11); pal(R_BORDER, 26, 110, 62); pal(R_TXT, 200, 255, 214); pal(R_DIM, 104, 170, 128);
  pal(R_DIG, 240, 255, 236); pal(R_SH, 0, 8, 4); pal(R_AMB, 255, 190, 60); pal(R_ERR, 255, 100, 80);
  pal(R_UFO, 255, 80, 230); pal(R_UFO2, 120, 255, 140);
  lastT = now; curFetch = 0; shN = 0;
  ufoOn = false; ufoAt = now + 25000 + (uint32_t)rn(35000);
  for (int i = 0; i < MAX_PLANES; i++) { offX[i] = offY[i] = 0.0f; }
}

static uint8_t altCol(const Plane& p, bool trail) {
  if (p.ground) return R_GND;
  uint8_t i = p.alt < 3000 ? 0 : (p.alt < 10000 ? 1 : (p.alt < 25000 ? 2 : 3));
  return (uint8_t)((trail ? R_T1 : R_A1) + i);
}

// Strahl mit weichem Schweif: Dreiecksfaecher statt einzelner Linien (keine Luecken, keine "Treppen")
static void sweepFan(float a) {
  const int segs = 12;
  const float span = 0.95f;
  for (int k = segs - 1; k >= 0; k--) {
    const uint8_t col = k < 3 ? R_SW2 : (k < 6 ? R_SW3 : (k < 9 ? R_SW4 : R_SCOPE));
    if (col == R_SCOPE) continue;
    const float a0 = a - span * (float)(k + 1) / segs, a1 = a - span * (float)k / segs;
    fb->fillTriangle(CX, CY, CX + (int)lroundf(sinf(a0) * (RAD - 1)), CY - (int)lroundf(cosf(a0) * (RAD - 1)),
                     CX + (int)lroundf(sinf(a1) * (RAD - 1)), CY - (int)lroundf(cosf(a1) * (RAD - 1)), col);
  }
  fb->drawLine(CX, CY, CX + (int)lroundf(sinf(a) * (RAD - 1)), CY - (int)lroundf(cosf(a) * (RAD - 1)), R_SW1);
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, R_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, R_DIG); }

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  frameDt(now, lastT);
  const PlaneList* L = c.planes;
  const bool have = L && L->status != 0;
  const float radiusKm = (L && L->radiusKm >= 1.0f) ? L->radiusKm : 25.0f;
  const float scale = (float)RAD / radiusKm;
  const float age = (have && L->fetchedMs) ? secs(now, L->fetchedMs) : 0.0f;
  const float dtS = age > 45.0f ? 45.0f : age;
  const bool stale = have && (L->status == 2 || age > 40.0f);

  fb->fillScreen(0);

  // ---- Schirm ----
  fb->fillCircle(CX, CY, RAD, R_SCOPE);
  sweepFan((float)(now % 5000) / 5000.0f * 6.2832f);
  fb->drawCircle(CX, CY, RAD / 3, R_RING);
  fb->drawCircle(CX, CY, (RAD * 2) / 3, R_RING);
  fb->drawCircle(CX, CY, RAD, R_RING2);
  fb->drawFastHLine(CX - RAD, CY, 2 * RAD + 1, R_RING);
  fb->drawFastVLine(CX, CY - RAD, 2 * RAD + 1, R_RING);
  for (int a = 0; a < 360; a += 30) {                                     // Gradstriche am Rand
    const float r = a * 0.0174533f, sn = sinf(r), cs = cosf(r);
    fb->drawLine(CX + (int)lroundf(sn * (RAD - 3)), CY - (int)lroundf(cs * (RAD - 3)),
                 CX + (int)lroundf(sn * RAD), CY - (int)lroundf(cs * RAD), R_RING2);
  }
  txt(CX - 2, CY - RAD + 5, "N", R_DIM);

  // ---- Flugzeuge ----
  int nAir = 0, airIdx[MAX_PLANES];
  float px[MAX_PLANES], py[MAX_PLANES], dist[MAX_PLANES];
  const int n = have ? L->n : 0;
  if (have && L->fetchedMs != curFetch) {                                  // neuer Abruf: Versatz zur zuletzt gezeigten Position merken
    curFetch = L->fetchedMs;
    for (int i = 0; i < n; i++) {
      offX[i] = offY[i] = 0.0f;
      const Plane& p = L->p[i];
      for (int j = 0; j < shN; j++)
        if (shId[j] == p.id && p.id) {
          offX[i] = shX[j] - (p.x + p.vx * dtS); offY[i] = shY[j] - (p.y + p.vy * dtS);
          if (fabsf(offX[i]) > 8.0f || fabsf(offY[i]) > 8.0f) offX[i] = offY[i] = 0.0f;
          break;
        }
    }
  }
  const float fade = fcl(1.0f - age / 2.5f, 0.0f, 1.0f);
  for (int i = 0; i < n; i++) {
    const Plane& p = L->p[i];
    const float x = p.x + p.vx * dtS + offX[i] * fade, y = p.y + p.vy * dtS + offY[i] * fade;
    dist[i] = sqrtf(x * x + y * y);
    px[i] = CX + x * scale; py[i] = CY - y * scale;
    shId[i] = p.id; shX[i] = x; shY[i] = y;
    if (!p.ground) airIdx[nAir++] = i;
  }
  shN = n;
  const int sel = nAir > 0 ? airIdx[(now / 5000) % (uint32_t)nAir] : -1;
  for (int pass = 0; pass < 2; pass++)                                     // erst Spuren/Boden, dann Flugzeuge
    for (int i = 0; i < n; i++) {
      const Plane& p = L->p[i];
      if (dist[i] > radiusKm * 1.04f) continue;
      const int x = (int)lroundf(px[i]), y = (int)lroundf(py[i]);
      const bool moving = p.gs > 40 && p.trk >= 0;
      const float rd = p.trk * 0.0174533f;
      const float dx = moving ? sinf(rd) : 0.0f, dy = moving ? -cosf(rd) : 0.0f;
      if (pass == 0) {
        if (moving) {
          fb->drawPixel((int)lroundf(px[i] - dx * 4.0f), (int)lroundf(py[i] - dy * 4.0f), altCol(p, true));
          fb->drawPixel((int)lroundf(px[i] - dx * 7.0f), (int)lroundf(py[i] - dy * 7.0f), altCol(p, true));
          fb->drawPixel((int)lroundf(px[i] - dx * 10.0f), (int)lroundf(py[i] - dy * 10.0f), altCol(p, true));
        }
        continue;
      }
      const uint8_t col = altCol(p, false);
      if (moving) {                                                        // Pfeil in Kursrichtung
        fb->fillTriangle((int)lroundf(px[i] + dx * 4.0f), (int)lroundf(py[i] + dy * 4.0f),
                         (int)lroundf(px[i] - dx * 2.5f - dy * 2.6f), (int)lroundf(py[i] - dy * 2.5f + dx * 2.6f),
                         (int)lroundf(px[i] - dx * 2.5f + dy * 2.6f), (int)lroundf(py[i] - dy * 2.5f - dx * 2.6f), col);
      } else fb->fillRect(x - 1, y - 1, 3, 3, col);
      if (i == sel) {                                                      // Markierung und Rufzeichen
        fb->drawCircle(x, y, 7, I_WHITE);
        const int tw = txtw(p.call);
        const int tx = (x + 9 + tw < CX + RAD + 2) ? x + 9 : x - 9 - tw;
        txt(tx + 1, y - 3, p.call, R_SH);
        txt(tx, y - 4, p.call, R_AMB);
      } else if (n <= 4 && p.call[0]) {
        const int tw = txtw(p.call);
        const int tx = (x + 6 + tw < CX + RAD + 2) ? x + 6 : x - 6 - tw;
        txt(tx, y - 3, p.call, R_DIM);
      }
    }

  // ---- UFO ----
  if (!(c.opts & OPT_UFO) && !kick) { ufoOn = false; }
  else {
    if (kick || (!ufoOn && (int32_t)(now - ufoAt) >= 0)) {
      kick = false; ufoOn = true; ufoT0 = now;
      const float a = rf() * 6.2832f;
      ufoX = sinf(a) * RAD; ufoY = -cosf(a) * RAD;            // kommt vom Rand
      ufoTarget(now);
    }
    if (ufoOn && now - ufoT0 > UFO_MS) { ufoOn = false; ufoAt = now + 90000 + (uint32_t)rn(210000); }
  }
  if (ufoOn) {
    const float mt = secs(now, ufoMoveT);
    if (mt < 0.35f) {                                                      // ruckartiger Sprung
      const float e = mt / 0.35f, k = e * e * (3.0f - 2.0f * e);
      ufoX = ufoFX + (ufoTX - ufoFX) * k; ufoY = ufoFY + (ufoTY - ufoFY) * k;
    } else if (mt > 0.35f + ufoHold && now - ufoT0 < UFO_MS - 1500) ufoTarget(now);
    const float age = secs(now, ufoT0);
    const float wob = sinf(age * 9.0f) * 1.2f;
    int ux = CX + (int)lroundf(ufoX + wob), uy = CY + (int)lroundf(ufoY);
    if (now - ufoT0 > UFO_MS - 1500) {                                     // verschwindet nach oben aus dem Bild
      const float e = (float)(now - ufoT0 - (UFO_MS - 1500)) / 1500.0f;
      uy -= (int)(e * e * 80.0f);
    }
    const uint8_t a = ((now / 150) & 1) ? R_UFO : R_UFO2;
    fb->drawCircle(ux, uy, 6 + (int)((now / 120) % 3), a);
    fb->fillRect(ux - 4, uy, 9, 2, R_UFO);
    fb->fillRect(ux - 2, uy - 2, 5, 2, R_UFO2);
    fb->drawPixel(ux - 3, uy + 1, ((now / 100) & 1) ? I_WHITE : R_AMB);
    fb->drawPixel(ux, uy + 1, ((now / 100) & 1) ? R_AMB : I_WHITE);
    fb->drawPixel(ux + 3, uy + 1, ((now / 100) & 1) ? I_WHITE : R_AMB);
    const int tx = (ux + 10 + 18 < CX + RAD + 2) ? ux + 10 : ux - 10 - 23;
    txt(tx + 1, uy - 3, TR(D_UFO_Q), R_SH);
    txt(tx, uy - 4, TR(D_UFO_Q), a);
  }

  // Standort in der Mitte
  fb->drawFastHLine(CX - 2, CY, 5, I_WHITE);
  fb->drawFastVLine(CX, CY - 2, 5, I_WHITE);

  // Hinweise im Schirm
  char b[32];
  if (!have)            txtc(CX, CY + 14, c.wifi ? TR(D_LOADING) : TR(D_NO_WIFI), R_AMB);
  else if (L->status == 2 && L->n == 0 && L->fetchedMs == 0) txtc(CX, CY + 14, TR(D_NO_DATA), R_ERR);
  else if (n == 0)      txtc(CX, CY + 14, TR(D_NO_PLANES), R_DIM);
  if (stale && have && L->fetchedMs) txtc(CX, CY - 14, TR(D_OUTDATED), R_ERR);
  snprintf(b, sizeof(b), "%dKM", (int)lroundf(radiusKm));
  txt(2, 2, b, R_DIM);
  snprintf(b, sizeof(b), TR(D_N_PLANES), nAir);
  txt(2, 111, b, R_DIM);
  if (ufoOn) txt(2 + txtw(b) + 6, 111, TR(D_PLUS_UFO), ((now / 300) & 1) ? R_UFO : R_UFO2);

  // ---- Seitenleiste ----
  fb->fillRect(117, 0, 43, FB_H, R_PANEL);
  fb->drawFastVLine(116, 0, FB_H, R_BORDER);
  const int a = c.timeOk ? c.hh / 10 : 10, bb = c.timeOk ? c.hh % 10 : 10;
  const int d = c.timeOk ? c.mm / 10 : 10, e = c.timeOk ? c.mm % 10 : 10;
  glyph(122, 3, a, 3, cellS); glyph(122, 3, a, 3, cellD);
  glyph(140, 3, bb, 3, cellS); glyph(140, 3, bb, 3, cellD);
  glyph(122, 27, d, 3, cellS); glyph(122, 27, d, 3, cellD);
  glyph(140, 27, e, 3, cellS); glyph(140, 27, e, 3, cellD);
  if (c.showSec && c.timeOk) fb->fillRect(119, 50, ((c.ss + 1) * 39) / 60, 1, R_SW2);
  if (c.timeOk) langDate(b, sizeof(b), c.mday, c.mon); else snprintf(b, sizeof(b), "--.--.");
  txtc(138, 54, b, R_DIM);
  if (c.hasWx) tempTxt(138 - tempW(c.temp, 1) / 2, 63, c.temp, R_TXT);
  fb->drawFastHLine(119, 73, 39, R_BORDER);

  if (ufoOn) {
    const bool bl = (now / 300) & 1;
    txtc(138, 76, TR(D_UFO_EX), bl ? R_UFO : R_UFO2);
    txtc(138, 85, TR(D_UNKNOWN), R_TXT);
    txtc(138, 94, "FL???", R_A3);
    txtc(138, 103, "9999KT", R_TXT);
    snprintf(b, sizeof(b), "%.1fKM", (double)(sqrtf(ufoX * ufoX + ufoY * ufoY) / scale));
    txtc(138, 112, b, R_DIM);
  } else if (sel >= 0) {
    const Plane& p = L->p[sel];
    char cs[9]; strlcpy(cs, p.call, 8);
    txtc(138, 76, cs, R_AMB);
    txtc(138, 85, p.type[0] ? p.type : "----", R_TXT);
    if (p.alt >= 6000) snprintf(b, sizeof(b), "FL%03d", (int)(p.alt / 100)); else snprintf(b, sizeof(b), "%dFT", (int)(p.alt / 100) * 100);
    if (p.ground) snprintf(b, sizeof(b), "%s", TR(D_GROUND));
    const int w = txtw(b);
    txt(138 - w / 2 - (p.vr > 300 || p.vr < -300 ? 3 : 0), 94, b, R_A3);
    if (p.vr > 300)       txt(138 + w / 2 - 2, 94, "\x18", R_A2);
    else if (p.vr < -300) txt(138 + w / 2 - 2, 94, "\x19", R_A4);
    snprintf(b, sizeof(b), "%dKT", (int)p.gs);
    txtc(138, 103, b, R_TXT);
    snprintf(b, sizeof(b), "%.1fKM", (double)dist[sel]);
    txtc(138, 112, b, R_DIM);
  } else {
    if (!have) { txtc(138, 85, TR(D_WAIT1), R_DIM); txtc(138, 94, TR(D_WAIT2), R_DIM); txtc(138, 103, TR(D_WAIT3), R_DIM); }
    else       { txtc(138, 85, TR(D_NOPL1), R_DIM); txtc(138, 94, TR(D_NOPL2), R_DIM); txtc(138, 103, TR(D_NOPL3), R_DIM); }
  }
}

static void cover(float p, bool, uint32_t) {
  if (p <= 0.0f) return;
  const int r = (int)(p * 105.0f);
  fb->fillCircle(CX, CY, r, R_SCOPE);
  fb->drawCircle(CX, CY, r, R_SW1);
  if (r > 3) fb->drawCircle(CX, CY, r - 3, R_SW3);
  if (p >= 0.97f) memset(buf(), R_SCOPE, FB_W * FB_H);
}

}  // namespace f_radar
