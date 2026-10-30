#pragma once
/*
  face_dream.h - Zifferblatt "Traumsand"
  Nachthimmel mit funkelnden Sternen und Mondsichel. Ein kleiner, selbst erfundener Wicht mit
  Zipfelmuetze und weissem Bart schwebt auf einer Wolke und streut Traumsand: Die goldenen
  Koerner fallen im Bogen nach unten und verglimmen. Zur vollen Minute gibt es eine Handvoll
  Sand extra und eine Sternschnuppe. Unten waechst eine Sandlinie im Sekundentakt.
  Uebergang: Sand rieselt von oben ueber den Schirm.
*/
#include "faces_fx.h"

namespace f_dream {
using namespace fc;

enum : uint8_t { D_SKY1 = 1, D_SKY2, D_SKY3, D_ST1, D_ST2, D_MOON, D_CLOUD, D_CLOUD2, D_HAT, D_HAT2,
                 D_SKIN, D_BEARD, D_SAND1, D_SAND2, D_SAND3, D_DIG, D_SH, D_DIM, D_EYE, D_SACK };

struct Grain { float x, y, vx, vy; uint32_t t0; bool on; };
static Grain    gr[48];
static uint32_t lastT, lastSpawn, shootAt, burstUntil;
static uint8_t  lastMm;
static float    spawnAcc;

static void enter(uint32_t now) {
  pal(D_SKY1, 10, 12, 40); pal(D_SKY2, 20, 20, 62); pal(D_SKY3, 38, 30, 84);
  pal(D_ST1, 255, 244, 200); pal(D_ST2, 130, 130, 190);
  pal(D_MOON, 255, 236, 170);
  pal(D_CLOUD, 200, 206, 240); pal(D_CLOUD2, 140, 148, 200);
  pal(D_HAT, 120, 80, 200); pal(D_HAT2, 80, 50, 150);
  pal(D_SKIN, 255, 205, 170); pal(D_BEARD, 240, 240, 250);
  pal(D_SAND1, 255, 220, 110); pal(D_SAND2, 220, 160, 70); pal(D_SAND3, 130, 90, 60);
  pal(D_DIG, 255, 238, 190); pal(D_SH, 40, 28, 90); pal(D_DIM, 170, 170, 220);
  pal(D_EYE, 40, 30, 60); pal(D_SACK, 190, 130, 70);
  for (int i = 0; i < 48; i++) gr[i].on = false;
  lastT = now; lastSpawn = now; shootAt = now + 6000; burstUntil = 0; lastMm = 255; spawnAcc = 0;
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, D_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, D_DIG); }

static void spawn(uint32_t now, float x, float y, float sp) {
  for (int i = 0; i < 48; i++)
    if (!gr[i].on) {
      gr[i] = {x, y, (0.6f + rf()) * sp, -(0.2f + rf() * 0.9f) * sp * 0.6f, now, true};
      return;
    }
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) { burstUntil = now + 1500; shootAt = now + 300; }
    lastMm = c.mm;
  }

  fb->fillRect(0, 0, FB_W, 40, D_SKY1);
  fb->fillRect(0, 40, FB_W, 40, D_SKY2);
  fb->fillRect(0, 80, FB_W, 40, D_SKY3);

  // ---- Sterne ----
  for (int i = 0; i < 28; i++) {
    uint32_t h = (uint32_t)i * 2654435761u + 12345u;
    const int x = (int)((h >> 8) % 158) + 1, y = (int)((h >> 20) % 110) + 1;
    const uint32_t ph = (now / (350 + (h & 255))) + (uint32_t)i;
    const uint8_t m = ph % 4;
    if (m == 3) continue;
    fb->drawPixel(x, y, m == 0 ? D_ST1 : D_ST2);
    if (m == 0 && (i & 3) == 0) { fb->drawPixel(x - 1, y, D_ST2); fb->drawPixel(x + 1, y, D_ST2); fb->drawPixel(x, y - 1, D_ST2); fb->drawPixel(x, y + 1, D_ST2); }
  }

  // ---- Mondsichel ----
  fb->fillCircle(148, 14, 8, D_MOON);
  fb->fillCircle(152, 11, 7, D_SKY1);

  // ---- Sternschnuppe ----
  if ((int32_t)(now - shootAt) >= 0) {
    const float a = secs(now, shootAt);
    if (a > 1.0f) shootAt = now + 9000 + (uint32_t)rn(8000);
    else {
      const int x = 150 - (int)(a * 130.0f), y = 8 + (int)(a * 44.0f);
      for (int k = 0; k < 8; k++) fb->drawPixel(x + k * 2, y - k, k < 2 ? D_ST1 : (k < 5 ? D_ST2 : D_SKY3));
      fb->drawPixel(x, y, I_WHITE);
    }
  }

  // ---- Wolke mit Wicht ----
  const float bob = sinf((float)(now % 4000) / 4000.0f * 6.2832f) * 2.0f;
  const int by = 96 + (int)bob;
  fb->fillCircle(22, by + 6, 9, D_CLOUD2); fb->fillCircle(34, by + 3, 11, D_CLOUD2); fb->fillCircle(47, by + 6, 9, D_CLOUD2);
  fb->fillCircle(22, by + 4, 9, D_CLOUD);  fb->fillCircle(34, by + 1, 11, D_CLOUD);  fb->fillCircle(47, by + 4, 9, D_CLOUD);
  fb->fillRect(22, by + 4, 26, 7, D_CLOUD);
  // Koerper (Mantel)
  fb->fillRect(28, by - 14, 12, 14, D_HAT2);
  fb->fillRect(29, by - 14, 10, 14, D_HAT);
  // Saeckchen an der Seite
  fb->fillCircle(26, by - 6, 4, D_SACK);
  fb->drawFastHLine(24, by - 10, 4, D_SACK);
  // Kopf und Bart
  fb->fillCircle(34, by - 19, 5, D_SKIN);
  fb->fillTriangle(29, by - 18, 39, by - 18, 34, by - 7, D_BEARD);
  fb->drawPixel(32, by - 20, D_EYE); fb->drawPixel(36, by - 20, D_EYE);
  // Zipfelmuetze mit abgeknicktem Zipfel
  fb->fillTriangle(28, by - 22, 40, by - 22, 33, by - 36, D_HAT);
  fb->fillTriangle(33, by - 36, 37, by - 33, 40, by - 30, D_HAT2);
  fb->fillRect(28, by - 23, 13, 2, D_BEARD);
  // Arm streckt sich nach rechts
  const float sw = sinf((float)(now % 1600) / 1600.0f * 6.2832f);
  const int hx = 46, hy = by - 11 + (int)(sw * 2.0f);
  fb->drawLine(38, by - 11, hx, hy, D_HAT2);
  fb->drawLine(38, by - 12, hx, hy - 1, D_HAT);
  fb->fillCircle(hx + 1, hy, 2, D_SKIN);

  // ---- Sandkoerner ----
  const bool burst = now < burstUntil;
  spawnAcc += dt * (burst ? 48.0f : 14.0f);
  while (spawnAcc >= 1.0f) { spawnAcc -= 1.0f; spawn(now, (float)(hx + 3), (float)hy, burst ? 40.0f : 26.0f); }
  for (int i = 0; i < 48; i++) {
    Grain& g = gr[i];
    if (!g.on) continue;
    g.vy += 34.0f * dt; g.vx *= (1.0f - 0.8f * dt);
    g.x += g.vx * dt; g.y += g.vy * dt;
    const float age = secs(now, g.t0);
    if (age > 3.2f || g.y > FB_H - 2 || g.x > FB_W) { g.on = false; continue; }
    const uint8_t col = age < 1.2f ? D_SAND1 : (age < 2.3f ? D_SAND2 : D_SAND3);
    fb->drawPixel((int)g.x, (int)g.y, col);
    if (age < 1.5f) fb->drawPixel((int)g.x + 1, (int)g.y, D_SAND2);
  }

  // ---- Uhrzeit ----
  drawTime(26, 8, 4, c, cellS);
  drawTime(26, 8, 4, c, cellD);

  // ---- Datum und Wetter rechts ----
  if (c.timeOk) {
    char b[16];
    snprintf(b, sizeof(b), "%s %02u.%02u.", WD_SHORT[c.wday % 7], c.mday, c.mon);
    txtr(FB_W - 6, 48, b, D_DIM);
  }
  if (c.hasWx) {
    char b[16];
    tempTxt(FB_W - 6 - tempW(c.temp, 2), 66, c.temp, D_DIG, 2);
    snprintf(b, sizeof(b), "%d%%", c.hum);
    txtr(FB_W - 6, 86, b, D_DIM);
  }

  // ---- Sandlinie der Sekunden ----
  if (c.showSec && c.timeOk) {
    const int w = ((c.ss + 1) * FB_W) / 60;
    fb->fillRect(0, 117, w, 3, D_SAND2);
    fb->fillRect(0, 117, w, 1, D_SAND1);
  }
}

static void cover(float p, bool, uint32_t) {
  if (p <= 0.0f) return;
  uint8_t* bf = buf();
  if (p >= 0.97f) { memset(bf, D_SAND2, FB_W * FB_H); return; }
  for (int x = 0; x < FB_W; x++) {
    const float h = p * (FB_H + 24) - (float)((x * 7) % 24);
    int hh = (int)h;
    if (hh <= 0) continue;
    if (hh > FB_H) hh = FB_H;
    for (int y = 0; y < hh; y++) bf[y * FB_W + x] = (y > hh - 3) ? D_SAND1 : D_SAND2;
  }
}

}  // namespace f_dream
