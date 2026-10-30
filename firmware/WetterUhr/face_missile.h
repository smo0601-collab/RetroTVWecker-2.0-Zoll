#pragma once
/*
  face_missile.h - Zifferblatt "Raketenabwehr"
  Nachthimmel mit Sternen, unten eine Pixelstadt mit drei Abwehrstellungen. Feindliche
  Raketen ziehen rote Spuren diagonal nach unten, die Stellungen schiessen blaue
  Abfangspuren auf die berechnete Treffstelle; dort blueht eine kleine Explosion auf.
  Selten kommt eine Rakete durch und beschaedigt ein Gebaeude kurz.
  Alle Flugbahnen laufen HINTER der Kommandozentrale (Uhr-Feld mit Rahmen), die Uhrzeit
  bleibt also immer frei. Zur vollen Minute laeuft ein Radarring ueber den Himmel.
  Uebergang: Radar-Kreis waechst von unten mittig.
*/
#include "faces_fx.h"

namespace f_msl {
using namespace fc;

enum : uint8_t { M_STAR = 1, M_RED, M_RED2, M_BLUE, M_BLUE2, M_ORA, M_YEL, M_BLD, M_WIN, M_GND, M_DIM, M_TXT };

struct En { float sx, sy, ex, ey, dur, t, launch, px, py; uint8_t tur; bool on, doomed, fired; };
struct Ic { float tx, ty, px, py, dur, t; int8_t target; bool on; };
struct Ex { float x, y, t, maxr; bool on; };

static En en[4];
static Ic ic[4];
static Ex ex[6];
static uint8_t bh[16], bw[16], bx[16];
static uint32_t dmgUntil[16];
static uint8_t starX[30], starY[30];
static const int TUR[3] = {26, 80, 134};
static uint32_t lastT, nextSpawn, radarT;
static uint8_t lastMm;

static void addEx(float x, float y, float r) {
  for (int i = 0; i < 6; i++) if (!ex[i].on) { ex[i] = {x, y, 0, r, true}; return; }
}

static void enter(uint32_t now) {
  pal(M_STAR, 120, 130, 170); pal(M_RED, 255, 70, 50); pal(M_RED2, 120, 30, 26);
  pal(M_BLUE, 70, 130, 255); pal(M_BLUE2, 160, 205, 255); pal(M_ORA, 255, 150, 30);
  pal(M_YEL, 255, 230, 120); pal(M_BLD, 40, 62, 130); pal(M_WIN, 255, 220, 90);
  pal(M_GND, 90, 44, 24); pal(M_DIM, 110, 140, 220); pal(M_TXT, 255, 190, 60);
  int x = 0, i = 0;
  while (x < FB_W && i < 16) {
    bw[i] = 7 + rn(6); bx[i] = (uint8_t)x; bh[i] = 8 + rn(15);
    x += bw[i] + 1; i++;
  }
  for (; i < 16; i++) { bx[i] = 0; bw[i] = 0; bh[i] = 0; }
  for (int k = 0; k < 30; k++) { starX[k] = rn(FB_W); starY[k] = rn(90); }
  for (int k = 0; k < 4; k++) { en[k].on = false; ic[k].on = false; }
  for (int k = 0; k < 6; k++) ex[k].on = false;
  for (int k = 0; k < 16; k++) dmgUntil[k] = 0;
  lastT = now; nextSpawn = now + 800; radarT = 0; lastMm = 255;
}

static void cellD(int x, int y, int s, int, int) {
  fb->fillRect(x, y, s, s, M_BLUE2);
  fb->drawPixel(x, y, I_WHITE);
}

static void spawnEnemy(uint32_t now) {
  for (int k = 0; k < 4; k++) {
    if (en[k].on) continue;
    En& e = en[k];
    e.sx = (float)rn(FB_W); e.sy = 0;
    const int b = rn(16);
    e.ex = (float)(bx[b] + bw[b] / 2); e.ey = 112.0f - bh[b];
    if (bw[b] == 0) { e.ex = (float)(10 + rn(140)); e.ey = 100; }
    const float dx = e.ex - e.sx, dy = e.ey - e.sy;
    const float len = sqrtf(dx * dx + dy * dy);
    e.dur = len / (18.0f + rn(10));
    e.t = 0; e.on = true; e.fired = false;
    e.doomed = rn(100) < 10;
    // Abfangpunkt bei 78 % der Strecke, naechste Stellung schiesst
    const float f = 0.78f;
    e.px = e.sx + dx * f; e.py = e.sy + dy * f;
    int best = 0; float bd = 1e9f;
    for (int t = 0; t < 3; t++) { const float d = fabsf(TUR[t] - e.px); if (d < bd) { bd = d; best = t; } }
    e.tur = (uint8_t)best;
    const float tx = e.px - TUR[best], ty = e.py - 108.0f;
    const float ti = sqrtf(tx * tx + ty * ty) / 85.0f;
    e.launch = f * e.dur - ti;
    if (e.launch < 0) e.launch = 0;
    (void)now;
    return;
  }
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) radarT = now ? now : 1;
    lastMm = c.mm;
  }
  if (radarT && now - radarT > 1700) radarT = 0;

  if (now >= nextSpawn) { nextSpawn = now + 1700 + rn(1500); spawnEnemy(now); }

  // ---- Raketen und Abfangjaeger ----
  for (int k = 0; k < 4; k++) {
    En& e = en[k];
    if (!e.on) continue;
    e.t += dt;
    if (!e.doomed && !e.fired && e.t >= e.launch) {
      e.fired = true;
      for (int j = 0; j < 4; j++)
        if (!ic[j].on) {
          const float tx = e.px - TUR[e.tur], ty = e.py - 108.0f;
          ic[j] = {(float)TUR[e.tur], 108.0f, e.px, e.py, sqrtf(tx * tx + ty * ty) / 85.0f + 0.01f, 0, (int8_t)k, true};
          break;
        }
    }
    if (e.t >= e.dur) {                                     // eingeschlagen
      e.on = false;
      addEx(e.ex, e.ey, 8);
      for (int b = 0; b < 16; b++) if (bw[b] && fabsf(e.ex - (bx[b] + bw[b] / 2)) < bw[b]) dmgUntil[b] = now + 12000;
    }
  }
  for (int j = 0; j < 4; j++) {
    Ic& i = ic[j];
    if (!i.on) continue;
    i.t += dt;
    if (i.t >= i.dur) {
      i.on = false;
      addEx(i.px, i.py, 7);
      if (i.target >= 0 && en[i.target].on) en[i.target].on = false;
    }
  }
  for (int k = 0; k < 6; k++) {
    if (!ex[k].on) continue;
    ex[k].t += dt;
    if (ex[k].t >= 0.9f) ex[k].on = false;
  }

  // ---- Zeichnen ----
  fb->fillScreen(0);
  for (int k = 0; k < 30; k++)
    if (((now / (300 + k * 17)) + k) % 6) fb->drawPixel(starX[k], starY[k], M_STAR);

  if (radarT) {                                             // Radarring ab der mittleren Stellung
    const float r = (float)(now - radarT) / 1700.0f * 125.0f;
    fb->drawCircle(80, 108, (int)r, M_BLUE);
    if (r > 6) fb->drawCircle(80, 108, (int)r - 4, M_DIM);
  }
  for (int k = 0; k < 4; k++) {
    const En& e = en[k];
    if (!e.on) continue;
    const float f = e.t / e.dur;
    const float hx = e.sx + (e.ex - e.sx) * f, hy = e.sy + (e.ey - e.sy) * f;
    fb->drawLine((int)e.sx, (int)e.sy, (int)hx, (int)hy, M_RED2);
    fb->fillRect((int)hx - 1, (int)hy - 1, 2, 2, M_RED);
  }
  for (int j = 0; j < 4; j++) {
    const Ic& i = ic[j];
    if (!i.on) continue;
    const float f = i.t / i.dur;
    const float hx = i.tx + (i.px - i.tx) * f, hy = i.ty + (i.py - i.ty) * f;
    fb->drawLine((int)i.tx, (int)i.ty, (int)hx, (int)hy, M_BLUE);
    fb->drawPixel((int)hx, (int)hy, I_WHITE);
  }
  for (int k = 0; k < 6; k++) {
    const Ex& x = ex[k];
    if (!x.on) continue;
    const int r = (int)(x.maxr * sinf(x.t / 0.9f * 3.1416f));
    if (r < 1) continue;
    fb->fillCircle((int)x.x, (int)x.y, r, M_ORA);
    if (r > 2) fb->fillCircle((int)x.x, (int)x.y, r * 6 / 10, M_YEL);
    fb->drawCircle((int)x.x, (int)x.y, r + 1, M_RED);
  }

  // Stadt und Stellungen
  fb->fillRect(0, 112, FB_W, 8, M_GND);
  for (int b = 0; b < 16; b++) {
    if (!bw[b]) continue;
    int h = bh[b];
    if (now < dmgUntil[b]) h -= 4;
    if (h < 3) h = 3;
    fb->fillRect(bx[b], 112 - h, bw[b], h, M_BLD);
    for (int wy = 112 - h + 2; wy < 110; wy += 3)
      for (int wx = bx[b] + 1; wx < bx[b] + bw[b] - 1; wx += 2)
        if ((wx * 7 + wy * 3 + b) % 5 < 3) fb->drawPixel(wx, wy, M_WIN);
  }
  for (int t = 0; t < 3; t++) {
    fb->fillTriangle(TUR[t] - 7, 112, TUR[t] + 7, 112, TUR[t], 105, M_BLUE);
    fb->drawPixel(TUR[t], 104, M_BLUE2);
  }

  // Kommandozentrale (verdeckt alle Flugbahnen darunter)
  fb->fillRect(20, 22, 120, 52, 0);
  fb->drawRect(20, 22, 120, 52, M_BLUE);
  fb->drawRect(22, 24, 116, 48, M_BLD);
  fb->drawFastHLine(20, 22, 10, M_ORA); fb->drawFastVLine(20, 22, 10, M_ORA);
  fb->drawFastHLine(130, 73, 10, M_ORA); fb->drawFastVLine(139, 64, 10, M_ORA);
  drawTime(26, 38, 4, c, cellD);
  char b[32];
  if (c.timeOk) snprintf(b, sizeof(b), "LAGE %s %02u.%02u.", WD_SHORT[c.wday % 7], c.mday, c.mon);
  else          snprintf(b, sizeof(b), "LAGE --");
  txt(26, 27, b, M_DIM);
  if (c.hasWx) tempTxt(135 - tempW(c.temp, 1), 27, c.temp, M_TXT);
  if (c.showSec && c.timeOk) fb->fillRect(26, 68, ((c.ss + 1) * 108) / 60, 1, M_ORA);
}

static void cover(float p, bool, uint32_t) {
  const int r = (int)(p * 140.0f);
  if (r <= 0) return;
  fb->fillCircle(80, 112, r, 0);
  if (p < 1.0f) fb->drawCircle(80, 112, r, M_BLUE2);
}

}  // namespace f_msl
