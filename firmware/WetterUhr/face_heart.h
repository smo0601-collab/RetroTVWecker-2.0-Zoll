#pragma once
/*
  face_heart.h - Zifferblatt "Pulsierendes Herz"
  Ein Herz schlaegt mit rund 70 Schlaegen pro Minute (Doppelschlag "lub-dub"). Bei jedem
  Schlag laeuft ein Ring nach aussen, ab und zu steigt ein kleines Herz auf. Unten laeuft
  eine EKG-Kurve von rechts nach links, ihr Ausschlag faellt mit dem Herzschlag zusammen.
  Die Uhrzeit steht gross darueber, links Wochentag und Datum, rechts Temperatur und Feuchte.
  Zur vollen Minute blitzt das Herz weiss und schickt mehrere kleine Herzen los.
  Die Sekunden laufen als Balken ganz unten. Uebergang: Ein Herz waechst ueber den Schirm.
*/
#include "faces_fx.h"

namespace f_heart {
using namespace fc;

enum : uint8_t { P_BG = 1, P_H1, P_H2, P_GLINT, P_DIG, P_SH, P_DIM, P_RING1, P_RING2, P_RING3,
                 P_ECG1, P_ECG2, P_ECG3, P_MINI1, P_MINI2, P_MINI3, P_BAR };

static const uint32_t PERIOD = 857;                 // ms pro Herzschlag (ca. 70 pro Minute)
static const int CX = 80, CY = 54;                  // Herzmitte

struct Mini { float x, y, ph; uint32_t t0; bool on; };
struct Ring { uint32_t t0; bool on; };
static Mini     mini[6];
static Ring     ring[3];
static uint32_t t0, lastBeat, flashUntil, lastT;
static uint8_t  lastMm;

static void enter(uint32_t now) {
  pal(P_BG, 20, 6, 26);
  pal(P_H1, 235, 40, 80); pal(P_H2, 140, 14, 48); pal(P_GLINT, 255, 190, 205);
  pal(P_DIG, 255, 236, 242); pal(P_SH, 96, 10, 46); pal(P_DIM, 190, 120, 150);
  pal(P_RING1, 200, 40, 90); pal(P_RING2, 130, 28, 70); pal(P_RING3, 70, 16, 50);
  pal(P_ECG1, 120, 255, 160); pal(P_ECG2, 60, 160, 100); pal(P_ECG3, 30, 80, 60);
  pal(P_MINI1, 255, 120, 160); pal(P_MINI2, 200, 70, 120); pal(P_MINI3, 120, 40, 80);
  pal(P_BAR, 200, 60, 110);
  for (int i = 0; i < 6; i++) mini[i].on = false;
  for (int i = 0; i < 3; i++) ring[i].on = false;
  t0 = now; lastBeat = 0xFFFFFFFFu; flashUntil = 0; lastMm = 255; lastT = now;
}

// ---- Herz aus zwei Kreisen und einem Dreieck (Radius r der Rundungen) ----
static void heartFill(int cx, int cy, float r, uint8_t col) {
  const float dx = r * 0.9f, dy = r * 0.5f;
  const int ri = (int)(r + 0.5f);
  const int lx = cx - (int)dx, rx = cx + (int)dx, ly = cy - (int)dy;
  fb->fillCircle(lx, ly, ri, col);
  fb->fillCircle(rx, ly, ri, col);
  fb->fillTriangle(cx - (int)(r * 1.82f), ly + (int)(r * 0.4f), cx + (int)(r * 1.82f), ly + (int)(r * 0.4f),
                   cx, ly + (int)(r * 2.4f), col);
}

// ---- Herzschlag-Kurven ----
static float bump(float x, float c, float w) {
  float v = 1.0f - fabsf(x - c) / w;
  if (v <= 0.0f) return 0.0f;
  return v * v * (3.0f - 2.0f * v);
}
static float beatShape(float ph) {                 // 0..1: kraeftiger erster, leiserer zweiter Schlag
  const float a = bump(ph, 0.30f, 0.09f), b = 0.55f * bump(ph, 0.52f, 0.10f);
  return a > b ? a : b;
}
static float ecgY(float u) {                       // Auslenkung in Pixeln (negativ = nach oben)
  static const float T[13][2] = {{0, 0}, {0.10f, 0}, {0.14f, -3}, {0.18f, 0}, {0.22f, 0}, {0.25f, 3}, {0.29f, -22},
                                 {0.33f, 7}, {0.37f, 0}, {0.48f, 0}, {0.55f, -5}, {0.62f, 0}, {1.0f, 0}};
  for (int i = 0; i < 12; i++)
    if (u <= T[i + 1][0]) {
      const float d = T[i + 1][0] - T[i][0];
      return d <= 0 ? T[i][1] : T[i][1] + (T[i + 1][1] - T[i][1]) * (u - T[i][0]) / d;
    }
  return 0.0f;
}

static void miniHeart(int x, int y, uint8_t col) {
  static const uint8_t M[5] = {0x0A, 0x1F, 0x1F, 0x0E, 0x04};
  for (int r = 0; r < 5; r++)
    for (int c = 0; c < 5; c++)
      if (M[r] & (0x10 >> c)) fb->drawPixel(x + c, y + r, col);
}

static void spawnMini(uint32_t now, float x) {
  for (int i = 0; i < 6; i++)
    if (!mini[i].on) { mini[i] = {x, (float)(CY + 6), rf() * 6.28f, now, true}; return; }
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, P_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, P_DIG); }

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  frameDt(now, lastT);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) {
      flashUntil = now + 450;
      for (int i = 0; i < 5; i++) spawnMini(now, (float)(CX - 24 + i * 12));
    }
    lastMm = c.mm;
  }

  const uint32_t el = now - t0;
  const uint32_t idx = el / PERIOD;
  const float ph = (float)(el % PERIOD) / (float)PERIOD;
  if (ph >= 0.28f && idx != lastBeat) {                              // neuer Schlag
    lastBeat = idx;
    for (int i = 0; i < 3; i++)
      if (!ring[i].on) { ring[i].on = true; ring[i].t0 = now; break; }
    if (rn(3) == 0) spawnMini(now, CX + (float)(rn(25) - 12));
  }

  fb->fillScreen(P_BG);

  // ---- Ringe ----
  for (int i = 0; i < 3; i++) {
    if (!ring[i].on) continue;
    const float age = secs(now, ring[i].t0);
    if (age > 0.85f) { ring[i].on = false; continue; }
    const int r = 20 + (int)(age * 34.0f);
    fb->drawCircle(CX, CY + 2, r, age < 0.3f ? P_RING1 : (age < 0.55f ? P_RING2 : P_RING3));
  }

  // ---- aufsteigende Mini-Herzen ----
  for (int i = 0; i < 6; i++) {
    Mini& m = mini[i];
    if (!m.on) continue;
    const float age = secs(now, m.t0);
    if (age > 2.4f) { m.on = false; continue; }
    const int x = (int)(m.x + sinf(m.ph + age * 3.0f) * 5.0f), y = (int)(m.y - age * 26.0f);
    if (y < 0) { m.on = false; continue; }
    miniHeart(x, y, age < 1.0f ? P_MINI1 : (age < 1.8f ? P_MINI2 : P_MINI3));
  }

  // ---- EKG (unten, laeuft nach links; rechter Rand = jetzt) ----
  const int base = 98;
  const float perBeat = 51.4f;                                       // Pixel pro Herzschlag
  int py = base;
  for (int x = 0; x < FB_W; x++) {
    float u = ph - (float)(FB_W - 1 - x) / perBeat;
    u -= floorf(u);
    const int y = base + (int)(ecgY(u) + (ecgY(u) < 0 ? -0.5f : 0.5f));
    if (x > 0) fb->drawLine(x - 1, py, x, y, x < 55 ? P_ECG3 : (x < 110 ? P_ECG2 : P_ECG1));
    py = y;
  }
  fb->fillRect(FB_W - 3, py - 1, 3, 3, I_WHITE);

  // ---- Herz ----
  const bool flash = now < flashUntil && ((now / 70) & 1);
  const float r = 8.0f + 2.4f * beatShape(ph);
  heartFill(CX, CY, r + 1.5f, flash ? P_GLINT : P_H2);
  heartFill(CX, CY - 1, r - 0.3f, flash ? I_WHITE : P_H1);
  fb->fillRect(CX - (int)(r * 1.4f), CY - (int)(r * 1.1f), 3, 2, P_GLINT);
  fb->fillRect(CX - (int)(r * 1.4f), CY - (int)(r * 1.1f) + 2, 2, 2, P_GLINT);

  // ---- Uhrzeit ----
  drawTime(26, 6, 4, c, cellS);
  drawTime(26, 6, 4, c, cellD);

  // ---- Datum links, Wetter rechts ----
  if (c.timeOk) {
    char b[16];
    txt(6, 44, WD_SHORT[c.wday % 7], P_DIG, 2);
    langDate(b, sizeof(b), c.mday, c.mon);
    txt(6, 62, b, P_DIM);
  } else txt(6, 44, "--", P_DIG, 2);
  if (c.hasWx) {
    char b[16];
    tempTxt(FB_W - 6 - tempW(c.temp, 2), 44, c.temp, P_DIG, 2);
    snprintf(b, sizeof(b), "%d%%", c.hum);
    txtr(FB_W - 6, 62, b, P_DIM);
  } else txtr(FB_W - 6, 44, "--", P_DIG, 2);

  if (c.showSec && c.timeOk) fb->fillRect(10, 116, ((c.ss + 1) * 140) / 60, 2, P_BAR);
}

static void cover(float p, bool, uint32_t) {
  if (p <= 0.0f) return;
  if (p >= 0.94f) { memset(buf(), P_H2, FB_W * FB_H); return; }
  const float r = p / 0.94f * 78.0f;
  heartFill(CX, 62, r + 3.0f, P_H2);
  heartFill(CX, 62, r, P_H1);
}

}  // namespace f_heart
