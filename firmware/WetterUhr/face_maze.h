#pragma once
/*
  face_maze.h - Zifferblatt "Labyrinth"
  Eine Maus sammelt Kruemel in einem Ringgang und springt ueber drei Katzen. Kaese in den
  Ecken macht die Katzen kurz zahm (blau) - dann werden sie gefangen. Jede Minute faellt ein
  Apfel in den Gang, zur vollen Stunde leuchten die Waende bunt. Die Uhr steht frei in der Mitte.
  Uebergang: Maus-Bisse (wachsende Kreise).
*/
#include "faces_core.h"

namespace f_maze {
using namespace fc;

enum : uint8_t { M_WALL = 1, M_WALL2, M_WALL3, M_WALL4, M_CRUMB, M_CHEESE, M_HOLE, M_MOUSE, M_PINK,
                 M_CAT1, M_CAT2, M_CAT3, M_SCARED, M_SCARED2, M_EYE, M_APPLE, M_LEAF, M_GOLD, M_DIM, M_DIG, M_DIGS };

static const int X0 = 14, Y0 = 20, X1 = 146, Y1 = 108, PW = X1 - X0, PH = Y1 - Y0, PL = 2 * (PW + PH);
static const float CORNER[4] = {0, (float)PW, (float)(PW + PH), (float)(2 * PW + PH)};

static float mt, ct[3], cv[3];
static uint32_t lastT, scaredUntil, jumpStart, hourFlash, appleUntil, popUntil, cheeseGone[4], catBack[3];
static float appleT;
static uint32_t score;
static uint8_t lastMm, lastHh, lastCrumb;


static void pathPos(float t, float& x, float& y, int& dx, int& dy) {
  while (t < 0) t += PL;
  while (t >= PL) t -= PL;
  if (t < PW)                 { x = X0 + t;               y = Y0;                 dx = 1;  dy = 0; }
  else if (t < PW + PH)       { x = X1;                   y = Y0 + (t - PW);      dx = 0;  dy = 1; }
  else if (t < 2 * PW + PH)   { x = X1 - (t - PW - PH);   y = Y1;                 dx = -1; dy = 0; }
  else                        { x = X0;                   y = Y1 - (t - 2 * PW - PH); dx = 0; dy = -1; }
}
static float wrapd(float a, float b) {          // a - b in -PL/2 .. PL/2
  float d = fmodf(a - b + PL * 1.5f, (float)PL);
  return d - PL * 0.5f;
}

static void enter(uint32_t now) {
  pal(M_WALL, 40, 70, 220); pal(M_WALL2, 0, 220, 240); pal(M_WALL3, 255, 80, 200); pal(M_WALL4, 255, 220, 40);
  pal(M_CRUMB, 200, 170, 120); pal(M_CHEESE, 255, 210, 40); pal(M_HOLE, 190, 140, 10);
  pal(M_MOUSE, 170, 170, 184); pal(M_PINK, 240, 150, 170);
  pal(M_CAT1, 240, 130, 40); pal(M_CAT2, 130, 140, 170); pal(M_CAT3, 150, 70, 200);
  pal(M_SCARED, 60, 80, 255); pal(M_SCARED2, 230, 230, 255); pal(M_EYE, 255, 255, 100);
  pal(M_APPLE, 230, 40, 40); pal(M_LEAF, 60, 200, 60);
  pal(M_GOLD, 255, 210, 80); pal(M_DIM, 120, 120, 150); pal(M_DIG, 255, 235, 120); pal(M_DIGS, 190, 120, 30);
  mt = 20;
  ct[0] = mt + 80;  cv[0] = 27;
  ct[1] = mt + 190; cv[1] = 29;
  ct[2] = mt + 300; cv[2] = 31;
  for (int i = 0; i < 3; i++) { while (ct[i] >= PL) ct[i] -= PL; catBack[i] = 0; }
  for (int i = 0; i < 4; i++) cheeseGone[i] = 0;
  scaredUntil = jumpStart = hourFlash = appleUntil = popUntil = 0;
  score = 0; lastMm = lastHh = 255; lastCrumb = 0; lastT = now;
}

static void cellD(int x, int y, int s, int, int) {
  fb->fillRect(x, y, s, s, M_DIGS);
  fb->fillRect(x, y, s - 1, s - 1, M_DIG);
}

static void mouse(float t, bool air, float lift, uint32_t now) {
  float x, y; int dx, dy;
  pathPos(t, x, y, dx, dy);
  const int px = (int)x, py = (int)y - (air ? (int)lift : 0);
  const int nx = -dy, ny = dx;                         // Querrichtung
  const int tw = (int)(sinf(now * 0.012f) * 2.0f);
  fb->drawLine(px - dx * 3, py - dy * 3, px - dx * 8 + nx * tw, py - dy * 8 + ny * tw, M_PINK);
  fb->fillCircle(px, py, 3, M_MOUSE);
  const int hx = px + dx * 3, hy = py + dy * 3;
  fb->fillCircle(hx, hy, 2, M_MOUSE);
  fb->fillCircle(hx - dx + nx * 2, hy - dy + ny * 2, 1, M_PINK);
  fb->fillCircle(hx - dx - nx * 2, hy - dy - ny * 2, 1, M_PINK);
  fb->drawPixel(hx + dx * 2, hy + dy * 2, M_DIM);       // Nase
  fb->drawPixel(hx + dx + nx, hy + dy + ny, 0);         // Auge
}

static void cat(float t, uint8_t col, bool scared, uint32_t now) {
  float x, y; int dx, dy;
  pathPos(t, x, y, dx, dy);
  const int cx = (int)x, cy = (int)y;
  uint8_t body = col, eye = M_EYE;
  if (scared) { body = (now < scaredUntil - 1200 || ((now / 120) & 1)) ? M_SCARED : M_SCARED2; eye = I_WHITE; }
  const int tw = (int)(sinf(now * 0.01f + t) * 2.0f);
  fb->drawLine(cx - dx * 4, cy - dy * 4, cx - dx * 8 - dy * tw, cy - dy * 8 + dx * tw, body);
  fb->fillCircle(cx, cy, 4, body);
  fb->fillTriangle(cx - 4, cy - 2, cx - 3, cy - 6, cx - 1, cy - 3, body);
  fb->fillTriangle(cx + 4, cy - 2, cx + 3, cy - 6, cx + 1, cy - 3, body);
  fb->drawPixel(cx - 2, cy - 1, eye); fb->drawPixel(cx + 2, cy - 1, eye);
  fb->drawPixel(cx, cy + 1, M_PINK);
  if (scared) { fb->drawPixel(cx - 2, cy, M_SCARED2); fb->drawPixel(cx + 2, cy, M_SCARED2); }
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  float dt = (float)(uint32_t)(now - lastT) * 0.001f;
  if (dt > 0.1f) dt = 0.1f;
  lastT = now;

  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) { appleT = mt + 90; appleUntil = now + 20000; }
    if (lastHh != 255 && c.hh != lastHh) hourFlash = now + 1400;
    lastMm = c.mm; lastHh = c.hh;
  }

  // ---- Bewegung ----
  const bool scared = now < scaredUntil;
  mt += 34.0f * dt;
  if (mt >= PL) mt -= PL;
  const uint8_t crumb = (uint8_t)((int)(mt / 8.0f));
  if (crumb != lastCrumb) { score += 10; lastCrumb = crumb; }

  for (int i = 0; i < 3; i++) {
    if (now < catBack[i]) continue;
    ct[i] += (scared ? 10.0f : cv[i]) * dt;
    if (ct[i] >= PL) ct[i] -= PL;
    const float d = wrapd(ct[i], mt);                   // > 0: Katze liegt vor der Maus
    if (scared && d > -4 && d < 6) {                    // gefangen
      score += 200;
      catBack[i] = now + 1600;
      ct[i] = fmodf(mt + 150.0f + rn(120), (float)PL);
      popUntil = now + 900;
    }
    if (!scared && d > 0 && d < 15 && now - jumpStart > 700) jumpStart = now;
  }
  for (int i = 0; i < 4; i++) {
    if (cheeseGone[i]) { if (now - cheeseGone[i] > 9000) cheeseGone[i] = 0; continue; }
    if (fabsf(wrapd(mt, CORNER[i])) < 4.0f) { cheeseGone[i] = now ? now : 1; scaredUntil = now + 5000; score += 50; }
  }
  if (appleUntil && now < appleUntil) {
    if (fabsf(wrapd(mt, appleT)) < 4.0f) { appleUntil = 0; score += 100; popUntil = now + 900; }
  } else appleUntil = 0;

  // ---- Zeichnen ----
  fb->fillScreen(0);
  uint8_t wc = M_WALL;
  if (now < hourFlash) wc = M_WALL + ((now / 90) & 3);
  fb->drawRoundRect(X0 - 7, Y0 - 7, PW + 14, PH + 14, 7, wc);
  fb->drawRoundRect(X0 - 6, Y0 - 6, PW + 12, PH + 12, 6, wc);
  fb->drawRoundRect(X0 + 6, Y0 + 6, PW - 12, PH - 12, 3, wc);
  fb->drawRoundRect(X0 + 7, Y0 + 7, PW - 14, PH - 14, 2, wc);

  for (int k = 0; k < 55; k++) {
    if (k * 8 < (int)mt) continue;
    float x, y; int dx, dy;
    pathPos((float)(k * 8), x, y, dx, dy);
    fb->fillRect((int)x, (int)y, 2, 2, M_CRUMB);
  }
  for (int i = 0; i < 4; i++) {
    if (cheeseGone[i]) continue;
    float x, y; int dx, dy;
    pathPos(CORNER[i], x, y, dx, dy);
    const int cx = (int)x, cy = (int)y;
    fb->fillTriangle(cx - 4, cy + 3, cx + 4, cy + 3, cx + 4, cy - 3, M_CHEESE);
    fb->drawPixel(cx + 2, cy + 1, M_HOLE); fb->drawPixel(cx, cy + 2, M_HOLE); fb->drawPixel(cx + 3, cy - 1, M_HOLE);
  }
  if (appleUntil) {
    float x, y; int dx, dy;
    pathPos(appleT, x, y, dx, dy);
    fb->fillCircle((int)x, (int)y, 3, M_APPLE);
    fb->drawPixel((int)x + 1, (int)y - 4, M_LEAF); fb->drawPixel((int)x + 2, (int)y - 4, M_LEAF);
  }

  const uint8_t cc[3] = {M_CAT1, M_CAT2, M_CAT3};
  for (int i = 0; i < 3; i++) if (now >= catBack[i]) cat(ct[i], cc[i], scared, now);

  const float jt = secs(now, jumpStart);
  const bool air = jt < 0.45f;
  mouse(mt, air, air ? 9.0f * sinf(jt / 0.45f * 3.1416f) : 0, now);

  // ---- Mitte: Uhr ----
  drawTime(39, 48, 3, c, cellD);
  if (c.showSec && c.timeOk) fb->fillRect(39, 74, ((c.ss + 1) * 81) / 60, 1, M_DIGS);
  char b[24];
  if (c.hasWx) snprintf(b, sizeof(b), "%dKMH %d%%", c.wind, c.hum); else snprintf(b, sizeof(b), "--");
  txtc(80, 82, b, M_DIM);

  // ---- Kopfzeile ----
  if (c.timeOk) langWdDate(b, sizeof(b), c.wday, c.mday, c.mon);
  else          snprintf(b, sizeof(b), "-- --.--.");
  txt(4, 3, b, M_GOLD);
  snprintf(b, sizeof(b), "%05lu", (unsigned long)(score % 100000));
  txtc(80, 3, b, M_DIM);
  if (c.hasWx) tempTxt(FB_W - 4 - tempW(c.temp, 1), 3, c.temp, M_GOLD);
  else         txtr(FB_W - 4, 3, "--", M_GOLD);

  if (now < popUntil) {
    float x, y; int dx, dy;
    pathPos(mt, x, y, dx, dy);
    const int rise = (int)((popUntil - now) < 900 ? (900 - (popUntil - now)) / 60 : 0);
    txtc((int)x, (int)y - 12 - rise, "+", M_EYE);
  }
}

static void cover(float p, bool, uint32_t) {
  // 8 x 6 Kreise im Abstand von 20 Pixeln wachsen zu einem geschlossenen Bild zusammen
  for (int gy = 0; gy < 6; gy++)
    for (int gx = 0; gx < 8; gx++) {
      const float delay = (float)(((gx * 7 + gy * 13) % 9)) * 0.045f;
      float r = (p - delay) * 15.0f / 0.64f;
      if (r <= 0) continue;
      if (r > 15.0f) r = 15.0f;
      fb->fillCircle(gx * 20 + 10, gy * 20 + 10, (int)r, 0);
    }
}

}  // namespace f_maze
