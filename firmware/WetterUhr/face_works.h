#pragma once
/*
  face_works.h - Zifferblatt "Zahnrad-Werk"
  Dunkle Fabrikhalle mit vier Traegern und Leitern. Ein grosser Roboter links oben rollt
  Zahnraeder die Traeger entlang (Zickzack nach unten). Ein kleiner Monteur steigt
  Leitern hoch und runter und springt ueber Zahnraeder. Zur vollen Stunde blinkt BONUS
  mit Funken. Uhrzeit gross im oberen Bereich.
  Uebergang: Rolltor (schwarze Flaeche steigt von unten auf).
*/
#include "faces_fx.h"

namespace f_fab {
using namespace fc;

enum : uint8_t { W_GIR = 1, W_GIR2, W_LAD, W_GEAR, W_GEAR2, W_BODY, W_SKIN, W_ROB, W_ROB2, W_EYE, W_TXT, W_DIG, W_DIM };

static const int LY[4]  = {116, 96, 72, 48};      // Standhoehe der Ebenen (Unterkante der Fuesse)
static const int GX0[4] = {0, 0, 20, 0};
static const int GX1[4] = {159, 140, 159, 140};
static const int LX[3]  = {30, 130, 40};          // Leiter zwischen Ebene i und i+1

struct Gear { float x, y; int8_t lvl, dir; bool fall, on; float a; };
static Gear gear[4];
static float hx, hy, hvy, hdir, climbY;
static int8_t hlvl, htarget;
static uint8_t hstate;                            // 0 gehen, 1 klettern, 2 springen
static uint32_t lastT, nextGear, pauseUntil, bonusUntil;
static uint8_t lastHh;
static Sparks<20> sp;

static const uint8_t HCOL[4] = {W_GEAR, W_SKIN, W_BODY, W_GIR2};
static const char* const SPR_A[10] = {"..hhh..", ".hhhhh.", "..sss..", "..sss..", ".bbbbb.", "bbbbbbb", ".bbbbb.", ".bb.bb.", ".bb.bb.", ".kk.kk."};
static const char* const SPR_B[10] = {"..hhh..", ".hhhhh.", "..sss..", "..sss..", ".bbbbb.", "bbbbbbb", ".bbbbb.", "..bbb..", "..b.b..", "..k.k.."};

static void enter(uint32_t now) {
  pal(W_GIR, 200, 50, 40); pal(W_GIR2, 110, 24, 20); pal(W_LAD, 60, 120, 255);
  pal(W_GEAR, 255, 150, 30); pal(W_GEAR2, 170, 90, 10); pal(W_BODY, 60, 110, 230);
  pal(W_SKIN, 244, 204, 164); pal(W_ROB, 120, 134, 180); pal(W_ROB2, 60, 70, 110);
  pal(W_EYE, 255, 60, 60); pal(W_TXT, 255, 220, 60); pal(W_DIG, 255, 244, 210); pal(W_DIM, 130, 130, 160);
  for (int i = 0; i < 4; i++) gear[i].on = false;
  hx = 60; hlvl = 0; htarget = 3; hstate = 0; hdir = 1; hy = LY[0]; hvy = 0; climbY = 0;
  lastT = now; nextGear = now + 1500; pauseUntil = 0; bonusUntil = 0; lastHh = 255;
  sp.clear();
}

static void cellD(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, W_GIR2); }
static void cellM(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, W_DIG); }

static void girder(int lvl) {
  const int y = LY[lvl], x0 = GX0[lvl], w = GX1[lvl] - GX0[lvl] + 1;
  fb->fillRect(x0, y, w, 3, W_GIR);
  fb->drawFastHLine(x0, y + 2, w, W_GIR2);
  for (int x = x0 + 2; x < x0 + w; x += 8) fb->drawPixel(x, y + 1, W_TXT);
}

static void ladder(int i) {
  const int x = LX[i], top = LY[i + 1], bot = LY[i];
  fb->drawFastVLine(x, top, bot - top, W_LAD);
  fb->drawFastVLine(x + 5, top, bot - top, W_LAD);
  for (int y = top + 3; y < bot; y += 4) fb->drawFastHLine(x, y, 6, W_LAD);
}

static void drawGear(const Gear& g) {
  const int cx = (int)g.x, cy = (int)g.y - 4;
  fb->fillCircle(cx, cy, 3, W_GEAR);
  fb->drawPixel(cx, cy, 0);
  for (int k = 0; k < 4; k++) {
    const float a = g.a + k * 1.5708f;
    fb->drawPixel(cx + (int)lroundf(cosf(a) * 4.4f), cy + (int)lroundf(sinf(a) * 4.4f), W_GEAR2);
  }
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  if (c.timeOk) {
    if (lastHh != 255 && c.hh != lastHh) { bonusUntil = now + 2200; sp.burst(80, 40, 18, 45, W_TXT); }
    lastHh = c.hh;
  }

  // ---- Zahnraeder ----
  if (now >= nextGear) {
    nextGear = now + 3300 + rn(1500);
    for (int i = 0; i < 4; i++)
      if (!gear[i].on) { gear[i] = {26.0f, (float)LY[3], 3, 1, false, true, 0.0f}; break; }
  }
  for (int i = 0; i < 4; i++) {
    Gear& g = gear[i];
    if (!g.on) continue;
    g.a += g.dir * 5.0f * dt;
    if (g.fall) {
      g.y += 120.0f * dt;
      if (g.y >= LY[g.lvl]) { g.y = (float)LY[g.lvl]; g.fall = false; }
    } else {
      g.x += g.dir * 28.0f * dt;
      if (g.lvl > 0 && ((g.dir > 0 && g.x > GX1[g.lvl]) || (g.dir < 0 && g.x < GX0[g.lvl]))) {
        g.lvl--; g.dir = -g.dir; g.fall = true;                     // ueber die Kante: eine Ebene tiefer
      } else if (g.lvl == 0 && g.x < -6) g.on = false;
    }
  }

  // ---- Monteur ----
  const float feetY = (float)LY[hlvl];
  if (now >= pauseUntil) {
    if (hstate == 0) {
      const int li = htarget > hlvl ? hlvl : hlvl - 1;                // benutzte Leiter
      const float dest = LX[li] + 3.0f;
      const float d = dest - hx;
      if (fabsf(d) < 1.5f) { hstate = 1; climbY = feetY; hx = dest; }
      else {
        hdir = d > 0 ? 1.0f : -1.0f;
        hx += hdir * 22.0f * dt;
        for (int i = 0; i < 4; i++) {                                // Zahnrad voraus? springen
          const Gear& g = gear[i];
          if (g.on && !g.fall && g.lvl == hlvl && (g.x - hx) * hdir > 0 && (g.x - hx) * hdir < 16 && g.dir * hdir < 0) {
            hstate = 2; hvy = 125.0f; hy = feetY; break;
          }
        }
      }
    } else if (hstate == 1) {
      const float dir = htarget > hlvl ? -1.0f : 1.0f;                // y waechst nach unten
      climbY += dir * 18.0f * dt;
      const int nl = htarget > hlvl ? hlvl + 1 : hlvl - 1;
      if ((dir < 0 && climbY <= LY[nl]) || (dir > 0 && climbY >= LY[nl])) {
        hlvl = (int8_t)nl; hstate = 0;
        if (hlvl == htarget) {
          do { htarget = (int8_t)rn(4); } while (htarget == hlvl);
          pauseUntil = now + 500 + rn(700);
        }
      }
    } else {                                                          // Sprung
      hvy -= 300.0f * dt;
      hy -= hvy * dt;
      hx += hdir * 22.0f * dt;
      if (hy >= feetY) { hy = feetY; hstate = 0; }
    }
    hx = fcl(hx, (float)GX0[hlvl] + 4.0f, (float)GX1[hlvl] - 4.0f);
  }

  sp.step(dt, 40.0f);

  // ---- Zeichnen ----
  fb->fillScreen(0);
  for (int i = 0; i < 3; i++) ladder(i);
  for (int i = 0; i < 4; i++) girder(i);
  for (int i = 0; i < 4; i++) if (gear[i].on) drawGear(gear[i]);

  const int sx = (int)hx - 3;
  const bool alt = ((now / 160) & 1);
  if (hstate == 1) blit(alt ? SPR_A : SPR_B, 7, 10, sx, (int)climbY - 10, "hsbk", HCOL, alt ? 1 : 0);
  else {
    const int yy = (hstate == 2 ? (int)hy : LY[hlvl]) - 10;
    blit((hstate == 0 && alt) ? SPR_B : SPR_A, 7, 10, sx, yy, "hsbk", HCOL, hdir < 0 ? 1 : 0);
  }

  // Roboter auf dem obersten Traeger
  fb->fillRect(3, 30, 18, 17, W_ROB);
  fb->drawRect(3, 30, 18, 17, W_ROB2);
  fb->drawFastVLine(12, 25, 5, W_ROB2);
  fb->fillRect(11, 23, 3, 3, W_EYE);
  const bool blink = (now % 3200) < 140;
  if (blink) fb->drawFastHLine(8, 36, 8, W_ROB2);
  else { fb->fillCircle(12, 36, 3, W_EYE); fb->drawPixel(11, 35, I_WHITE); }
  for (int k = 0; k < 3; k++) fb->drawFastHLine(7, 41 + k * 2, 10, W_ROB2);

  sp.draw();
  drawTime(26, 4, 4, c, cellD);
  drawTime(26, 4, 4, c, cellM);
  if (c.showSec && c.timeOk) fb->fillRect(26, 34, ((c.ss + 1) * 108) / 60, 1, W_GIR);

  char b[40];
  if (now < bonusUntil) {
    if ((now / 160) & 1) txtS(80 - txtw("BONUS", 1) / 2, 37, "BONUS", W_TXT, W_GIR2);
  } else {
    if (c.timeOk) langWdDate(b, sizeof(b), c.wday, c.mday, c.mon);
    else          snprintf(b, sizeof(b), "-- --.--.");
    txt(28, 38, b, W_DIM);
    if (c.hasWx) tempTxt(FB_W - 28 - tempW(c.temp, 1), 38, c.temp, W_DIM);
    else         txtr(FB_W - 28, 38, "--", W_DIM);
  }
}

static void cover(float p, bool, uint32_t) {
  const int y0 = (int)((1.0f - p) * FB_H);
  if (y0 >= FB_H) return;
  memset(buf() + y0 * FB_W, 0, (size_t)(FB_H - y0) * FB_W);
  fb->drawFastHLine(0, y0, FB_W, W_GIR);
  if (y0 + 1 < FB_H) fb->drawFastHLine(0, y0 + 1, FB_W, W_GIR2);
}

}  // namespace f_fab
