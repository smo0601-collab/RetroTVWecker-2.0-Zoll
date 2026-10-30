#pragma once
/*
  face_adventure.h - Zifferblatt "Abenteuer"
  Nachthimmel mit Mond, Huegeln und Kiefern. Die Uhrzeit steht in goldenen Zellen auf einer
  Steintafel. Unten wandert ein kleiner Reisender mit Stab, Gluehwuermchen schweben,
  Herzen zeigen das Wetter. Jede Minute spruehen Funken aus der Tafel.
  Uebergang: Raster-Aufloesung.
*/
#include "faces_core.h"

namespace f_adv {
using namespace fc;

enum : uint8_t { A_SKY1 = 1, A_SKY2, A_SKY3, A_STAR, A_HILL1, A_HILL2, A_PINE1, A_PINE2, A_STONE, A_EDGE, A_DARK,
                 A_GOLD, A_GOLD2, A_FLY, A_HEART, A_SKIN, A_TUNIC, A_STAFF, A_HAT, A_GRASS, A_GRASS2, A_SPARK, A_MOON };

static const char* const SPR_A[12] = {
  "..hhhhh..", ".hhhhhhh.", "..sssss.p", "..sesss.p", "...sss..p", "..ttttt.p",
  ".tttttt.p", "s.tttt..p", "..tttt..p", "..t..t..p", "..b..b..p", ".bb..bb.p"};
static const char* const SPR_B[12] = {
  "..hhhhh..", ".hhhhhhh.", "..sssss.p", "..sesss.p", "...sss..p", "..ttttt.p",
  ".tttttt.p", "s.tttt..p", "..tttt..p", "...tt...p", "...bb...p", "..bbbb..p"};
static const char* const SPR_HEART[6] = {".hh.hh.", "hhhhhhh", "hhhhhhh", ".hhhhh.", "..hhh..", "...h..."};

static uint8_t hillA[FB_W], hillB[FB_W];
static uint8_t starX[24], starY[24];
static const uint8_t pineX[7] = {10, 33, 58, 96, 121, 146, 78};
struct Fly { float bx, by, ph, sp; };
static Fly fly[7];
struct Par { float x, y, vx, vy, life; };
static Par par[16];
static float wx, wdir;
static uint32_t lastT, pauseUntil, burstT;
static uint8_t lastMm, lastHh;

static void enter(uint32_t now) {
  pal(A_SKY1, 8, 10, 36);   pal(A_SKY2, 22, 26, 70);  pal(A_SKY3, 58, 46, 100);
  pal(A_STAR, 210, 210, 240); pal(A_HILL1, 30, 40, 84); pal(A_HILL2, 22, 62, 66);
  pal(A_PINE1, 10, 46, 40); pal(A_PINE2, 22, 80, 56);
  pal(A_STONE, 74, 76, 96); pal(A_EDGE, 120, 124, 150); pal(A_DARK, 26, 28, 44);
  pal(A_GOLD, 255, 204, 64); pal(A_GOLD2, 160, 104, 24); pal(A_FLY, 200, 255, 120);
  pal(A_HEART, 232, 60, 72); pal(A_SKIN, 244, 204, 164); pal(A_TUNIC, 64, 176, 84);
  pal(A_STAFF, 150, 100, 52); pal(A_HAT, 30, 120, 56); pal(A_GRASS, 34, 84, 44);
  pal(A_GRASS2, 56, 120, 60); pal(A_SPARK, 255, 244, 170); pal(A_MOON, 238, 238, 214);
  for (int x = 0; x < FB_W; x++) {
    hillA[x] = (uint8_t)(66 + 7 * sinf(x * 0.045f) + 4 * sinf(x * 0.13f + 1.0f));
    hillB[x] = (uint8_t)(84 + 5 * sinf(x * 0.07f + 2.0f) + 3 * sinf(x * 0.19f));
  }
  for (int i = 0; i < 24; i++) { starX[i] = rn(FB_W); starY[i] = 2 + rn(44); }
  for (int i = 0; i < 7; i++) fly[i] = {(float)(10 + rn(140)), (float)(62 + rn(34)), rf() * 6.0f, 0.6f + rf() * 0.8f};
  for (int i = 0; i < 16; i++) par[i].life = 0;
  wx = 30; wdir = 1; pauseUntil = 0; burstT = 0; lastT = now;
  lastMm = 255; lastHh = 255;
}

static void cellG(int x, int y, int s, int, int) {
  fb->fillRect(x, y, s, s, A_GOLD2);
  fb->fillRect(x, y, s - 1, s - 1, A_GOLD);
}

static void spawn(int n, float x, float y, float spd) {
  for (int i = 0; i < n; i++)
    for (int k = 0; k < 16; k++)
      if (par[k].life <= 0) {
        const float a = rf() * 6.2832f, v = spd * (0.4f + rf());
        par[k] = {x, y, cosf(a) * v, sinf(a) * v - 12.0f, 0.7f + rf() * 0.6f};
        break;
      }
}

static void pine(int x, int y) {
  fb->fillRect(x - 1, y - 3, 3, 5, A_STAFF);
  fb->fillTriangle(x, y - 22, x - 7, y - 10, x + 7, y - 10, A_PINE1);
  fb->fillTriangle(x, y - 16, x - 9, y - 3, x + 9, y - 3, A_PINE1);
  fb->fillTriangle(x, y - 22, x - 3, y - 12, x, y - 12, A_PINE2);
  fb->fillTriangle(x, y - 16, x - 5, y - 6, x, y - 6, A_PINE2);
}

static void draw(const FaceCtx& c) {
  float dt = (float)(uint32_t)(c.now - lastT) * 0.001f;
  if (dt > 0.1f) dt = 0.1f;
  lastT = c.now;

  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) { burstT = c.now; spawn(14, 80, 32, 40); }
    if (lastHh != 255 && c.hh != lastHh) spawn(16, 80, 32, 64);
    lastMm = c.mm; lastHh = c.hh;
  }

  // Himmel in drei Baendern, Uebergaenge gerastert
  fb->fillRect(0, 0, FB_W, 34, A_SKY1);
  fb->fillRect(0, 34, FB_W, 26, A_SKY2);
  fb->fillRect(0, 60, FB_W, 60, A_SKY3);
  for (int x = 0; x < FB_W; x++) {
    if (x & 1) { fb->drawPixel(x, 33, A_SKY2); fb->drawPixel(x, 59, A_SKY3); }
    else       { fb->drawPixel(x, 34, A_SKY1); fb->drawPixel(x, 60, A_SKY2); }
  }
  for (int i = 0; i < 24; i++) {
    const int tw = (int)(((c.now / (180 + i * 23)) + i) % 5);
    if (tw == 0) continue;
    fb->drawPixel(starX[i], starY[i], A_STAR);
    if (tw == 2) {
      fb->drawPixel(starX[i] - 1, starY[i], A_STAR); fb->drawPixel(starX[i] + 1, starY[i], A_STAR);
      fb->drawPixel(starX[i], starY[i] - 1, A_STAR); fb->drawPixel(starX[i], starY[i] + 1, A_STAR);
    }
  }
  fb->fillCircle(138, 14, 6, A_MOON);
  fb->fillCircle(141, 12, 5, A_SKY1);

  for (int x = 0; x < FB_W; x++) {
    fb->drawFastVLine(x, hillA[x], FB_H - hillA[x], A_HILL1);
    fb->drawFastVLine(x, hillB[x], FB_H - hillB[x], A_HILL2);
  }
  for (int i = 0; i < 7; i++) pine(pineX[i], hillB[pineX[i]] + 3);

  fb->fillRect(0, 100, FB_W, 20, A_GRASS);
  fb->drawFastHLine(0, 100, FB_W, A_GRASS2);
  for (int x = 2; x < FB_W; x += 5) fb->drawPixel(x, 103 + ((x * 7) % 5), A_GRASS2);

  // Steintafel mit Uhrzeit
  const bool flash = (uint32_t)(c.now - burstT) < 600 && ((c.now / 80) & 1);
  fb->fillRect(18, 12, 124, 40, flash ? A_GOLD : A_EDGE);
  fb->fillRect(20, 14, 120, 36, A_STONE);
  fb->fillRect(22, 16, 116, 32, A_DARK);
  fb->drawPixel(21, 15, A_GOLD2); fb->drawPixel(138, 15, A_GOLD2);
  fb->drawPixel(21, 48, A_GOLD2); fb->drawPixel(138, 48, A_GOLD2);
  drawTime(26, 18, 4, c, cellG);
  if (c.showSec && c.timeOk) fb->fillRect(24, 46, ((c.ss + 1) * 112) / 60, 1, A_GOLD2);

  // Datum und Temperatur auf der Wiese
  char b[24];
  if (c.timeOk) snprintf(b, sizeof(b), "%s %02u.%02u.", WD_SHORT[c.wday % 7], c.mday, c.mon);
  else          snprintf(b, sizeof(b), "-- --.--.");
  txt(4, 109, b, A_GOLD);
  if (c.hasWx) tempTxt(FB_W - 8 - tempW(c.temp, 1), 109, c.temp, A_GOLD);
  else         txt(FB_W - 20, 109, "--", A_GOLD);

  // Herzen: Wetter (klar = 3, wolkig = 2, sonst 1; ohne Daten leer)
  const int full = !c.hasWx ? 0 : (c.wcode <= 1 ? 3 : (c.wcode <= 3 ? 2 : 1));
  for (int i = 0; i < 3; i++) {
    const uint8_t col = i < full ? A_HEART : A_DARK;
    const uint8_t k[1] = {col};
    blit(SPR_HEART, 7, 6, 20 + i * 9, 3, "h", k);
  }

  // Gluehwuermchen
  for (int i = 0; i < 7; i++) {
    Fly& f = fly[i];
    f.ph += dt * f.sp;
    const int x = (int)(f.bx + sinf(f.ph) * 9.0f), y = (int)(f.by + cosf(f.ph * 0.7f) * 5.0f);
    if (((c.now / (150 + i * 20)) % 7) < 5) {
      fb->drawPixel(x, y, A_FLY);
      if (((c.now / 200 + i) & 3) == 0) { fb->drawPixel(x + 1, y, A_GOLD2); fb->drawPixel(x - 1, y, A_GOLD2); }
    }
  }

  // Reisender
  const bool paused = c.now < pauseUntil;
  if (!paused) {
    wx += wdir * 14.0f * dt;
    if (wx > 148) { wx = 148; wdir = -1; }
    if (wx < 12)  { wx = 12;  wdir = 1; }
    const int r = rn(1000);
    if (r < 4) pauseUntil = c.now + 800 + rn(1500);
    else if (r < 7) wdir = -wdir;
  }
  const uint8_t wcol[6] = {A_HAT, A_SKIN, A_SKY1, A_TUNIC, A_STAFF, A_STAFF};
  const bool step = !paused && ((c.now / 170) & 1);
  blit(step ? SPR_B : SPR_A, 9, 12, (int)wx - 4, 94, "hsetbp", wcol, wdir < 0 ? 1 : 0);

  // Funken
  for (int k = 0; k < 16; k++) {
    Par& p = par[k];
    if (p.life <= 0) continue;
    p.life -= dt; p.vy += 70.0f * dt;
    p.x += p.vx * dt; p.y += p.vy * dt;
    if (p.x < 0 || p.x >= FB_W || p.y < 0 || p.y >= FB_H) { p.life = 0; continue; }
    fb->drawPixel((int)p.x, (int)p.y, p.life > 0.4f ? A_SPARK : A_GOLD);
    if (p.life > 0.6f) fb->drawPixel((int)p.x + 1, (int)p.y, A_GOLD2);
  }
}

static void cover(float p, bool, uint32_t) { coverDither(p, 0); }

}  // namespace f_adv
