#pragma once
/*
  face_hop.h - Zifferblatt "Huepfwelt"
  Blauer Himmel, Wolken, Gebuesch und ein Ziegelboden. Die vier Ziffern sitzen in Bonus-
  Bloecken. Ein runder Held rennt hin und her, sammelt schwebende Muenzen und springt bei
  jedem Zeitwechsel unter den passenden Block: der Block ruckt, die Ziffer wechselt, eine
  Muenze springt heraus. Oben ein Anzeigenband mit Datum, Temperatur, Wind und Feuchte.
  Uebergang: Blendenkreis.
*/
#include "faces_core.h"

namespace f_hop {
using namespace fc;

enum : uint8_t { H_SKY = 1, H_CLOUD, H_CLOUD2, H_BRICK, H_BRICK2, H_BRICK3, H_BLK, H_BLK2, H_BLK3, H_BLK4, H_DIG,
                 H_BODY, H_BODY2, H_FOOT, H_COIN, H_COIN2, H_BUSH, H_BUSH2, H_SHAD, H_POP };

static const int BX[4] = {14, 38, 100, 124};
static const int BY = 22, BS = 22, GROUND = 96;
static const int COINX[5] = {24, 52, 80, 108, 136};

static float hx, hy, hvy, hdir;
static uint8_t shown[4], mission;                // mission: 0 = keine, sonst Blockindex + 1
static float bumpT[4];
static uint32_t coinBack[5], lastT, hopAt;
static float cloudX[3];
struct Pop { float x, y, vy, life; };
static Pop pop[4];
static uint8_t jumpKind;                         // 0 normal, 1 Mission

static void enter(uint32_t now) {
  pal(H_SKY, 92, 148, 252); pal(H_CLOUD, 248, 248, 252); pal(H_CLOUD2, 200, 220, 255);
  pal(H_BRICK, 190, 80, 20); pal(H_BRICK2, 110, 44, 0); pal(H_BRICK3, 240, 150, 60);
  pal(H_BLK, 240, 190, 60); pal(H_BLK2, 150, 96, 10); pal(H_BLK3, 120, 84, 40); pal(H_BLK4, 255, 232, 150);
  pal(H_DIG, 90, 45, 0);
  pal(H_BODY, 60, 200, 140); pal(H_BODY2, 170, 255, 210); pal(H_FOOT, 255, 150, 30);
  pal(H_COIN, 255, 212, 30); pal(H_COIN2, 190, 130, 0);
  pal(H_BUSH, 40, 170, 60); pal(H_BUSH2, 20, 110, 40);
  pal(H_SHAD, 36, 60, 150); pal(H_POP, 255, 255, 170);
  hx = 70; hy = 0; hvy = 0; hdir = 1; mission = 0; jumpKind = 0; hopAt = now + 3000;
  for (int i = 0; i < 4; i++) { shown[i] = 255; bumpT[i] = 9; pop[i].life = 0; }
  for (int i = 0; i < 5; i++) coinBack[i] = 0;
  cloudX[0] = 20; cloudX[1] = 80; cloudX[2] = 135;
  lastT = now;
}

static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, H_DIG); }

static void cloud(int x, int y) {
  fb->fillCircle(x, y, 5, H_CLOUD);
  fb->fillCircle(x + 7, y - 3, 6, H_CLOUD);
  fb->fillCircle(x + 14, y, 5, H_CLOUD);
  fb->fillRect(x, y, 15, 6, H_CLOUD);
  fb->drawFastHLine(x - 3, y + 5, 22, H_CLOUD2);
}

static void bush(int x, int w) {
  fb->fillRect(x, GROUND - 5, w, 5, H_BUSH);
  fb->fillCircle(x + w / 4, GROUND - 5, 5, H_BUSH);
  fb->fillCircle(x + w * 3 / 4, GROUND - 6, 6, H_BUSH);
  fb->drawPixel(x + 4, GROUND - 4, H_BUSH2); fb->drawPixel(x + w - 5, GROUND - 6, H_BUSH2);
}

static void block(int i, int off, int digit) {
  const int x = BX[i], y = BY - off;
  const bool used = bumpT[i] < 0.3f;
  fb->fillRect(x, y, BS, BS, used ? H_BLK3 : H_BLK);
  fb->drawRect(x, y, BS, BS, H_BLK2);
  fb->drawFastHLine(x + 1, y + 1, BS - 2, H_BLK4);
  fb->drawFastVLine(x + 1, y + 1, BS - 2, H_BLK4);
  fb->drawPixel(x + 2, y + 2, H_BLK2); fb->drawPixel(x + BS - 3, y + 2, H_BLK2);
  fb->drawPixel(x + 2, y + BS - 3, H_BLK2); fb->drawPixel(x + BS - 3, y + BS - 3, H_BLK2);
  glyph(x + 6, y + 4, digit, 2, cellD);
}

static void spawnPop(float x, float y) {
  for (int k = 0; k < 4; k++)
    if (pop[k].life <= 0) { pop[k] = {x, y, -110.0f, 0.7f}; return; }
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  float dt = (float)(uint32_t)(now - lastT) * 0.001f;
  if (dt > 0.1f) dt = 0.1f;
  lastT = now;

  const uint8_t want[4] = {(uint8_t)(c.timeOk ? c.hh / 10 : 10), (uint8_t)(c.timeOk ? c.hh % 10 : 10),
                           (uint8_t)(c.timeOk ? c.mm / 10 : 10), (uint8_t)(c.timeOk ? c.mm % 10 : 10)};
  for (int i = 0; i < 4; i++) if (shown[i] == 255 || (!c.timeOk && shown[i] != 10)) shown[i] = want[i];

  // ---- Held ----
  if (!mission) {                                       // naechsten wartenden Block suchen
    int bestI = -1; float bestD = 1e9f;
    for (int i = 0; i < 4; i++)
      if (shown[i] != want[i]) {
        const float d = fabsf(hx - (BX[i] + BS / 2));
        if (d < bestD) { bestD = d; bestI = i; }
      }
    if (bestI >= 0) { mission = (uint8_t)(bestI + 1); jumpKind = 1; }
  }
  const bool ground = hy <= 0.0f && hvy >= 0.0f;
  if (ground) {
    hy = 0; hvy = 0;
    if (mission) {
      const float dest = (float)(BX[mission - 1] + BS / 2);
      const float d = dest - hx;
      if (fabsf(d) < 2.0f) { hvy = 180.0f; hy = 0.1f; }
      else { hdir = d > 0 ? 1.0f : -1.0f; hx += hdir * 52.0f * dt; if (fabsf(dest - hx) < 1.0f) hx = dest; }
    } else {
      hx += hdir * 22.0f * dt;
      if (hx > 148) { hx = 148; hdir = -1; }
      if (hx < 12)  { hx = 12;  hdir = 1; }
      if (now > hopAt) { hvy = 120.0f; hy = 0.1f; hopAt = now + 2500 + rn(3500); if (rn(3) == 0) hdir = -hdir; }
    }
  } else {
    hvy -= 320.0f * dt;
    hy += hvy * dt;
    if (hy < 0) { hy = 0; hvy = 0; }
    // Block getroffen? Kopf = GROUND - 14 - hy
    if (mission && hvy > 0 && (GROUND - 14 - hy) <= BY + BS + 1) {
      const int i = mission - 1;
      bumpT[i] = 0; shown[i] = want[i];
      spawnPop((float)(BX[i] + BS / 2 - 2), (float)BY - 2);
      hvy = -20.0f; mission = 0;
    }
  }
  for (int i = 0; i < 4; i++) if (bumpT[i] < 9) bumpT[i] += dt;

  // ---- Hintergrund ----
  fb->fillScreen(H_SKY);
  for (int i = 0; i < 3; i++) {
    cloudX[i] += (3.0f + i * 1.5f) * dt;
    if (cloudX[i] > FB_W + 10) cloudX[i] = -26;
  }
  cloud((int)cloudX[0], 58); cloud((int)cloudX[1], 66); cloud((int)cloudX[2], 54);
  bush(6, 30); bush(70, 26); bush(122, 34);
  fb->fillRect(0, GROUND, FB_W, FB_H - GROUND, H_BRICK);
  for (int r = 0; r < 3; r++) {
    const int y = GROUND + r * 8;
    fb->drawFastHLine(0, y, FB_W, r == 0 ? H_BRICK3 : H_BRICK2);
    for (int x = (r & 1) * 8; x < FB_W; x += 16) fb->drawFastVLine(x, y, 8, H_BRICK2);
  }

  // ---- Bloecke ----
  for (int i = 0; i < 4; i++) {
    int off = 0;
    if (bumpT[i] < 0.25f) off = (int)(7.0f * sinf(bumpT[i] / 0.25f * 3.1416f));
    block(i, off, shown[i]);
  }
  fb->fillRect(78, 28, 4, 4, H_COIN); fb->fillRect(78, 36, 4, 4, H_COIN);          // Doppelpunkt
  fb->drawRect(78, 28, 4, 4, H_COIN2); fb->drawRect(78, 36, 4, 4, H_COIN2);

  // ---- Muenzen ----
  const int hcy = GROUND - 7 - (int)hy;
  for (int i = 0; i < 5; i++) {
    if (now < coinBack[i]) continue;
    const int cy = 70;
    if (abs((int)hx - COINX[i]) < 9 && hcy - 7 < cy + 5 && hcy + 7 > cy - 5) { coinBack[i] = now + 9000; spawnPop((float)COINX[i], (float)cy); continue; }
    const int w = 1 + (int)(fabsf(cosf(now * 0.006f + i)) * 3.0f);
    fb->fillRect(COINX[i] - w, cy - 4, 2 * w + 1, 9, H_COIN);
    fb->drawRect(COINX[i] - w, cy - 4, 2 * w + 1, 9, H_COIN2);
  }
  for (int k = 0; k < 4; k++) {
    Pop& p = pop[k];
    if (p.life <= 0) continue;
    p.life -= dt; p.vy += 330.0f * dt; p.y += p.vy * dt;
    if (p.life <= 0 || p.y > FB_H) { p.life = 0; continue; }
    const int w = 1 + (int)(fabsf(cosf(now * 0.02f)) * 2.0f);
    fb->fillRect((int)p.x - w + 2, (int)p.y, 2 * w, 6, H_COIN);
    if (p.life < 0.25f) fb->drawPixel((int)p.x + 2, (int)p.y - 3, H_POP);
  }

  // ---- Held zeichnen ----
  {
    const int cx = (int)hx;
    const int step = ground ? (int)((now / 110) & 1) : 0;
    fb->fillCircle(cx, hcy, 6, H_BODY);
    fb->fillCircle(cx - 2, hcy - 2, 2, H_BODY2);
    const int d = hdir < 0 ? -1 : 1;
    fb->fillRect(cx + d * 2 - 1, hcy - 3, 3, 4, I_WHITE);
    fb->drawPixel(cx + d * 3, hcy - 2, H_DIG); fb->drawPixel(cx + d * 3, hcy - 1, H_DIG);
    fb->drawFastHLine(cx + d * 2, hcy + 2, 3, H_DIG);
    const int fy = hcy + 6;
    fb->fillRect(cx - 5 + (ground ? step * 2 : 0), fy - 1, 4, 3, H_FOOT);
    fb->fillRect(cx + 1 - (ground ? step * 2 : 0), fy - 1, 4, 3, H_FOOT);
  }

  // ---- Anzeigenband ----
  char b[20];
  const int cx[4] = {4, 44, 88, 126};
  if (c.timeOk) snprintf(b, sizeof(b), "%s %u", WD_SHORT[c.wday % 7], c.mday); else snprintf(b, sizeof(b), "-- --");
  txt(cx[0] + 1, 4, b, H_SHAD); txt(cx[0], 3, b, I_WHITE);
  if (c.hasWx) {
    tempTxt(cx[1] + 1, 4, c.temp, H_SHAD); tempTxt(cx[1], 3, c.temp, I_WHITE);
    snprintf(b, sizeof(b), "W%d", c.wind);
    txt(cx[2] + 1, 4, b, H_SHAD); txt(cx[2], 3, b, I_WHITE);
    snprintf(b, sizeof(b), "F%d%%", c.hum);
    txt(cx[3] + 1, 4, b, H_SHAD); txt(cx[3], 3, b, I_WHITE);
  } else {
    txt(cx[1], 3, "--", I_WHITE);
  }
  if (c.showSec && c.timeOk) {
    snprintf(b, sizeof(b), "%02u", c.ss);
    txtc(80, 49, b, H_SHAD);
  }
}

static void cover(float p, bool, uint32_t) {
  const float R = (1.0f - p) * 101.0f;
  const float R2 = R * R;
  uint8_t* bf = buf();
  for (int y = 0; y < FB_H; y++) {
    const float dy = (float)(y - 60);
    uint8_t* row = bf + y * FB_W;
    if (dy * dy >= R2) { memset(row, 0, FB_W); continue; }
    const int half = (int)sqrtf(R2 - dy * dy);
    const int l = 80 - half, r = 80 + half;
    if (l > 0) memset(row, 0, imin(l, FB_W));
    if (r < FB_W) memset(row + imax(r, 0), 0, FB_W - imax(r, 0));
  }
}

}  // namespace f_hop
