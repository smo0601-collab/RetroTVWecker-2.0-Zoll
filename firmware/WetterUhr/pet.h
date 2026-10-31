#pragma once
/*
  pet.h - Wiesenszene fuer das Hunde-Zifferblatt (Dackel)

  Eine Wiese mit Himmel (Tag und Nacht), Wolken, Blumen und einem Ball. Der Welpe jagt den
  Ball, stupst ihn mit der Nase an, setzt sich nach ein paar Staenden hin oder schnueffelt.
  Zur vollen Minute bellt er (TR(D_WOOF)). Die Rasse zeichnet nur den Hund selbst (DogFn);
  Verhalten, Ball und Hintergrund sind hier gemeinsam. Uhrzeit oben gross, darunter Datum
  und Temperatur.
*/
#include "faces_fx.h"

namespace pet {
using namespace fc;

enum : uint8_t { P_SKY1 = 1, P_SKY2, P_NSKY1, P_NSKY2, P_CLOUD, P_CLOUD2, P_SUN, P_MOON, P_STAR,
                 P_GR1, P_GR2, P_GR3, P_NGR1, P_NGR2, P_NGR3, P_FL1, P_FL2, P_FL3, P_BALL1, P_BALL2,
                 P_DIG, P_SH, P_TXT, P_SHAD, P_BUB, P_BUBTX, P_TONGUE, P_BREED = 40 };   // ab 40 frei fuer die Rasse

enum : uint8_t { M_RUN = 0, M_SIT = 1, M_SNIFF = 2 };

static const int FEET = 108;                      // y der Pfoten
static const int BALLY = 105;                     // y des Ballmittelpunkts auf dem Boden
static const int MOUTH = 18;                      // Abstand Hundemitte - Nase

struct Dog { float x; int8_t dir; uint8_t mode; uint32_t until; float walk, tail; bool moving; };
static Dog      dog;
static Ball     ball;
static bool     ballRest;
static int      kicks, kicksMax;
static uint32_t lastT, barkUntil;
static uint8_t  lastMm;
static float    cloudX[3];
static Sparks<20> sp;

typedef void (*DogFn)(int x, int y, int d, uint8_t mode, float walk, float tail, bool moving, bool bark);

// Spiegelhilfe: Zeichnen relativ zu (x, y) mit Blickrichtung d (+1 rechts, -1 links)
struct M {
  int x, y, d;
  void rect(int a, int b, int w, int h, uint8_t col) const { fb->fillRect(d > 0 ? x + a : x - a - w + 1, y + b, w, h, col); }
  void circ(int a, int b, int r, uint8_t col) const { fb->fillCircle(x + d * a, y + b, r, col); }
  void ring(int a, int b, int r, uint8_t col) const { fb->drawCircle(x + d * a, y + b, r, col); }
  void line(int a0, int b0, int a1, int b1, uint8_t col) const { fb->drawLine(x + d * a0, y + b0, x + d * a1, y + b1, col); }
  void thick(int a0, int b0, int a1, int b1, uint8_t col) const { line(a0, b0, a1, b1, col); line(a0, b0 + 1, a1, b1 + 1, col); line(a0 + 1, b0, a1 + 1, b1, col); }
  void tri(int a0, int b0, int a1, int b1, int a2, int b2, uint8_t col) const {
    fb->fillTriangle(x + d * a0, y + b0, x + d * a1, y + b1, x + d * a2, y + b2, col);
  }
  void px(int a, int b, uint8_t col) const { fb->drawPixel(x + d * a, y + b, col); }
};

static void enter(uint32_t now) {
  pal(P_SKY1, 110, 182, 255); pal(P_SKY2, 170, 214, 255);
  pal(P_NSKY1, 14, 20, 52); pal(P_NSKY2, 30, 38, 82);
  pal(P_CLOUD, 252, 252, 255); pal(P_CLOUD2, 205, 225, 250);
  pal(P_SUN, 255, 226, 80); pal(P_MOON, 238, 238, 214); pal(P_STAR, 255, 255, 220);
  pal(P_GR1, 86, 176, 70); pal(P_GR2, 74, 160, 60); pal(P_GR3, 52, 128, 46);
  pal(P_NGR1, 24, 64, 46); pal(P_NGR2, 20, 54, 40); pal(P_NGR3, 14, 40, 32);
  pal(P_FL1, 255, 240, 90); pal(P_FL2, 255, 120, 160); pal(P_FL3, 250, 250, 255);
  pal(P_BALL1, 232, 60, 60); pal(P_BALL2, 255, 170, 160);
  pal(P_DIG, 255, 255, 255); pal(P_SH, 22, 52, 110); pal(P_TXT, 255, 255, 255); pal(P_SHAD, 40, 100, 40);
  pal(P_BUB, 255, 255, 255); pal(P_BUBTX, 30, 30, 40); pal(P_TONGUE, 255, 120, 140);
  dog = {50.0f, 1, M_RUN, 0, 0.0f, 0.0f, false};
  ball = {96.0f, 40.0f, -20.0f, 0.0f};
  ballRest = false; kicks = 0; kicksMax = 3 + rn(3);
  lastT = now; barkUntil = 0; lastMm = 255;
  cloudX[0] = 20; cloudX[1] = 78; cloudX[2] = 132;
  sp.clear();
}

static void step(const FaceCtx& c, float dt) {
  const uint32_t now = c.now;
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) {
      barkUntil = now + 1500;
      sp.burst(dog.x + dog.dir * 12.0f, (float)(FEET - 24), 8, 40, P_FL1);
    }
    lastMm = c.mm;
  }

  // ---- Ball ----
  ball.vy += 260.0f * dt;
  ball.x += ball.vx * dt; ball.y += ball.vy * dt;
  if (ball.x < 26.0f)  { ball.x = 26.0f;  ball.vx = fabsf(ball.vx); }
  if (ball.x > 134.0f) { ball.x = 134.0f; ball.vx = -fabsf(ball.vx); }
  if (ball.y >= (float)BALLY) {
    ball.y = (float)BALLY;
    if (fabsf(ball.vy) > 45.0f) { ball.vy = -ball.vy * 0.55f; ball.vx *= 0.85f; }
    else ball.vy = 0.0f;
    if (ball.vy == 0.0f) { ball.vx -= ball.vx * 1.6f * dt; if (fabsf(ball.vx) < 5.0f) ball.vx = 0.0f; }
  }
  ballRest = (ball.y >= (float)BALLY && ball.vy == 0.0f && ball.vx == 0.0f);

  // ---- Hund ----
  dog.moving = false;
  if (dog.mode == M_RUN) {
    if (fabsf(ball.x - dog.x) > 8.0f) dog.dir = ball.x >= dog.x ? 1 : -1;
    const float tgt = ball.x - dog.dir * (float)MOUTH, dx = tgt - dog.x;
    const float spd = ballRest ? 44.0f : 64.0f, mv = spd * dt;
    if (fabsf(dx) > 1.0f) { dog.x += fabsf(dx) <= mv ? dx : (dx > 0 ? mv : -mv); dog.moving = true; dog.walk += spd * dt * 0.42f; }
    dog.x = fcl(dog.x, 6.0f, 154.0f);
    const float mouth = dog.x + dog.dir * (float)MOUTH;
    if (fabsf(ball.x - mouth) < 7.0f && ball.y > (float)(BALLY - 9)) {
      if (kicks >= kicksMax) {
        dog.mode = rn(3) ? M_SIT : M_SNIFF;
        dog.until = now + 2600 + (uint32_t)rn(1800);
        kicks = 0; kicksMax = 3 + rn(3);
      } else {
        ball.vx = dog.dir * (70.0f + (float)rn(55));
        ball.vy = -(60.0f + (float)rn(40));
        kicks++;
      }
    }
  } else if ((int32_t)(now - dog.until) >= 0) dog.mode = M_RUN;
  dog.tail += dt * (dog.mode == M_SIT ? 15.0f : 10.0f);
  if (dog.tail > 6.2832f * 50.0f) dog.tail -= 6.2832f * 50.0f;
  if (dog.walk > 6.2832f * 50.0f) dog.walk -= 6.2832f * 50.0f;

  for (int i = 0; i < 3; i++) { cloudX[i] += (4.0f + i * 2.0f) * dt; if (cloudX[i] > FB_W + 8) cloudX[i] = -26.0f; }
  sp.step(dt, 30.0f);
}

static void cloud(int x, int y, uint8_t a, uint8_t b) {
  fb->fillCircle(x, y, 4, a); fb->fillCircle(x + 6, y - 3, 5, a); fb->fillCircle(x + 12, y, 4, a);
  fb->fillRect(x, y, 13, 4, a); fb->drawFastHLine(x - 2, y + 4, 18, b);
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, P_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, P_DIG); }

static void draw(const FaceCtx& c, DogFn dogFn) {
  const uint32_t now = c.now;
  step(c, frameDt(now, lastT));
  const bool day = c.hasWx ? c.isDay : (c.timeOk ? (c.hh >= 7 && c.hh < 20) : true);

  // ---- Himmel und Wiese ----
  fb->fillRect(0, 0, FB_W, 40, day ? P_SKY1 : P_NSKY1);
  fb->fillRect(0, 40, FB_W, 24, day ? P_SKY2 : P_NSKY2);
  fb->fillRect(0, 64, FB_W, FB_H - 64, day ? P_GR1 : P_NGR1);
  for (int y = 72; y < FB_H; y += 12) fb->fillRect(0, y, FB_W, 6, day ? P_GR2 : P_NGR2);
  fb->fillRect(0, 60, FB_W, 4, day ? P_GR3 : P_NGR3);                      // Hecke
  for (int x = 2; x < FB_W; x += 7) fb->drawPixel(x, 59 + (x * 5) % 3, day ? P_GR3 : P_NGR3);
  if (day) {
    fb->fillCircle(146, 14, 6, P_SUN);
    for (int i = 0; i < 3; i++) cloud((int)cloudX[i], 46 + i * 4 - (i == 1 ? 2 : 0), P_CLOUD, P_CLOUD2);
  } else {
    fb->fillCircle(146, 14, 6, P_MOON); fb->fillCircle(149, 12, 5, P_NSKY1);
    static const uint8_t SX[8] = {8, 22, 60, 100, 120, 152, 40, 84}, SY[8] = {10, 40, 44, 38, 46, 36, 50, 54};
    for (int i = 0; i < 8; i++) if (((now / 400) + i * 3) % 5) fb->drawPixel(SX[i], SY[i], P_STAR);
  }
  // Blumen und Grasbueschel (fest)
  for (int i = 0; i < 14; i++) {
    const int x = 6 + (i * 37) % 150, y = 68 + (i * 53) % 46;
    fb->drawFastVLine(x, y + 1, 3, day ? P_GR3 : P_NGR3);
    fb->drawPixel(x, y, i % 3 == 0 ? P_FL1 : (i % 3 == 1 ? P_FL2 : P_FL3));
    if (i % 2) { fb->drawPixel(x - 1, y + 1, day ? P_GR3 : P_NGR3); fb->drawPixel(x + 1, y + 1, day ? P_GR3 : P_NGR3); }
  }

  // ---- Schatten, Ball, Hund ----
  const int bx = (int)ball.x, by = (int)ball.y;
  fb->drawFastHLine(bx - 3, BALLY + 4, 7 - (BALLY - by) / 12, P_SHAD);
  fb->drawFastHLine((int)dog.x - 15, FEET + 1, 31, P_SHAD);
  fb->drawFastHLine((int)dog.x - 12, FEET + 2, 25, P_SHAD);
  const bool bark = now < barkUntil;
  dogFn((int)dog.x, FEET, dog.dir, dog.mode, dog.walk, dog.tail, dog.moving, bark);
  fb->fillCircle(bx, by, 3, P_BALL1);
  fb->drawPixel(bx - 1, by - 1, P_BALL2);
  if (bark) {
    const int hx = (int)dog.x + dog.dir * 8 - 15, hy = FEET - 44;
    const int bxp = icl(hx, 1, FB_W - 32);
    fb->fillRect(bxp, hy, 30, 11, P_BUB);
    fb->fillTriangle(bxp + 10, hy + 11, bxp + 18, hy + 11, bxp + 14 + dog.dir * 4, hy + 16, P_BUB);
    txt(bxp + 2, hy + 2, TR(D_WOOF), P_BUBTX);
  }
  sp.draw();

  // ---- Zeit und Infos ----
  drawTime(26, 6, 4, c, cellS);
  drawTime(26, 6, 4, c, cellD);
  char b[24];
  if (c.timeOk) langWdDate(b, sizeof(b), c.wday, c.mday, c.mon);
  else          snprintf(b, sizeof(b), "-- --.--.");
  txtS(4, 37, b, P_TXT, P_SH);
  if (c.hasWx) {
    const int w = tempW(c.temp, 1);
    tempTxt(FB_W - 4 - w + 1, 38, c.temp, P_SH);
    tempTxt(FB_W - 4 - w, 37, c.temp, P_TXT);
  } else txtr(FB_W - 4, 37, "--", P_TXT);
  if (c.showSec && c.timeOk) fb->fillRect(0, FB_H - 2, ((c.ss + 1) * FB_W) / 60, 2, day ? P_FL1 : P_STAR);
}

// Uebergang: Pfotenabdruecke-Raster bzw. Gras waechst von unten
static void cover(float p, bool, uint32_t) {
  if (p <= 0.0f) return;
  uint8_t* bf = buf();
  const int h = (int)(p * (FB_H + 10));
  for (int x = 0; x < FB_W; x++) {
    const int hh = icl(h - ((x * 7) % 11), 0, FB_H);
    if (hh <= 0) continue;
    for (int y = FB_H - hh; y < FB_H; y++) bf[y * FB_W + x] = (y == FB_H - hh) ? P_GR1 : P_GR3;
  }
}

}  // namespace pet
