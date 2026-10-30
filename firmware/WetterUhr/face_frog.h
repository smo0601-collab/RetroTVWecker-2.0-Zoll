#pragma once
/*
  face_frog.h - Zifferblatt "Froschfurt"
  Oben ein Ufer mit der Uhrzeit, darunter Wasser mit Baumstaemmen und Schildkroeten, eine
  Grasinsel, drei Fahrspuren mit Autos und unten das Startufer. Ein kleiner Frosch huepft
  Spur fuer Spur nach oben: auf der Strasse wartet er auf Luecken, im Wasser nimmt er
  Baumstaemme mit. Gelegentlich gibt es einen Fehlsprung (Platsch oder Zusammenstoss).
  Zur vollen Minute funkelt es am Frosch. Uebergang: gegenlaeufige Streifen schliessen sich.
*/
#include "faces_fx.h"

namespace f_frog {
using namespace fc;

enum : uint8_t { F_GRASS = 1, F_GRASS2, F_ROAD, F_LINE, F_WATER, F_WATER2, F_LOG, F_LOG2, F_TURT, F_FROG, F_FROG2,
                 F_RED, F_YEL, F_CYA, F_WHT, F_ORA, F_PUR, F_DIM, F_PANEL };

// Spuren von oben nach unten: 0..2 Wasser, 3 Insel, 4..6 Strasse. Sprungstufen: 0 Start .. 8 Ziel
static const int STEP_Y[9] = {107, 99, 89, 79, 70, 60, 50, 40, 33};
static const float SPD[7] = {14, -20, 10, 0, 30, -24, 38};   // Wasser/Insel/Strasse, px/s

static float off[7];
static uint32_t lastT, hopStart, waitUntil, deadUntil, celebUntil;
static int step;            // aktuelle Stufe
static float fx;            // Frosch-x
static float fyDraw;
static bool hopping;
static int hopFrom;
static uint8_t state;       // 0 normal, 1 tot (Auto), 2 tot (Wasser)
static uint8_t lastMm;
static Sparks<20> sp;

static void enter(uint32_t now) {
  pal(F_GRASS, 30, 110, 40); pal(F_GRASS2, 20, 70, 30); pal(F_ROAD, 50, 50, 62); pal(F_LINE, 200, 200, 70);
  pal(F_WATER, 20, 60, 170); pal(F_WATER2, 40, 100, 210); pal(F_LOG, 140, 86, 34); pal(F_LOG2, 84, 52, 20);
  pal(F_TURT, 40, 160, 90); pal(F_FROG, 100, 235, 70); pal(F_FROG2, 40, 140, 30);
  pal(F_RED, 230, 50, 50); pal(F_YEL, 240, 210, 40); pal(F_CYA, 40, 210, 230); pal(F_WHT, 230, 230, 240);
  pal(F_ORA, 250, 140, 30); pal(F_PUR, 170, 70, 210); pal(F_DIM, 200, 220, 200); pal(F_PANEL, 10, 44, 22);
  for (int i = 0; i < 7; i++) off[i] = (float)(i * 37 % 190);
  step = 0; fx = 80; hopping = false; state = 0; waitUntil = 0; deadUntil = 0; celebUntil = 0;
  lastT = now; lastMm = 255;
  sp.clear();
}

static float itemX(int lane, int i, float t) {          // Position von Objekt i der Spur (Wasser: P=190, Strasse: P=200)
  const float P = lane < 3 ? 190.0f : 200.0f, gap = lane < 3 ? 95.0f : 67.0f;
  float x = fmodf(off[lane] + SPD[lane] * t + i * gap, P);
  if (x < 0) x += P;
  return x - 30.0f;
}
static int itemCount(int lane) { return lane < 3 ? 2 : 3; }
static float itemLen(int lane) { return lane == 1 ? 24.0f : (lane < 3 ? 32.0f : 14.0f); }
static int laneOfStep(int s) {                           // Stufe -> Spur (-1 = Ufer/Insel)
  if (s >= 5 && s <= 7) return 7 - s;                    // 7 = Wasser oben (Spur 0) .. 5 = Wasser unten (Spur 2)
  if (s >= 1 && s <= 3) return 7 - s;                    // 3 -> 4(Strasse oben) .. 1 -> 6
  return -1;
}

// Ist die Spur fuer x im Zeitraum t0..t1 (ab jetzt, s) frei von Autos?
static bool roadClear(int lane, float x, float t0, float t1, float now0) {
  for (float t = t0; t <= t1; t += 0.15f)
    for (int i = 0; i < itemCount(lane); i++) {
      const float cx = itemX(lane, i, now0 + t) + itemLen(lane) * 0.5f;
      if (fabsf(cx - x) < 11.0f) return false;
    }
  return true;
}
// Liegt an x zum Zeitpunkt t ein Stamm/eine Schildkroete? Rueckgabe Index oder -1
static int onItem(int lane, float x, float t) {
  for (int i = 0; i < itemCount(lane); i++) {
    const float x0 = itemX(lane, i, t), x1 = x0 + itemLen(lane);
    if (x > x0 + 3 && x < x1 - 3) return i;
  }
  return -1;
}

static int  ride = -1;                                   // Stamm, auf dem der Frosch sitzt

static void respawn(uint32_t now) {
  step = 0; fx = (float)(40 + rn(80)); hopping = false; state = 0; ride = -1;
  waitUntil = now + 600;
}

static void drawFrog(int x, int y, bool stretch) {
  fb->fillRect(x - 3, y - 3, 7, 6, F_FROG);
  fb->drawFastHLine(x - 2, y + 1, 5, F_FROG2);
  fb->drawPixel(x - 3, y - 4, I_WHITE); fb->drawPixel(x + 3, y - 4, I_WHITE);
  fb->drawPixel(x - 3, y - 3, F_FROG2); fb->drawPixel(x + 3, y - 3, F_FROG2);
  if (stretch) { fb->drawFastVLine(x - 4, y + 2, 3, F_FROG); fb->drawFastVLine(x + 4, y + 2, 3, F_FROG); }
  else         { fb->drawPixel(x - 4, y + 2, F_FROG); fb->drawPixel(x + 4, y + 2, F_FROG); }
}

static void drawCar(int x, int y, uint8_t col, int dir) {
  fb->fillRect(x, y + 1, 14, 6, col);
  fb->fillRect(x + (dir > 0 ? 8 : 3), y, 4, 2, col);
  fb->drawPixel(x + (dir > 0 ? 9 : 4), y + 1, F_WATER2);
  fb->drawPixel(x + (dir > 0 ? 13 : 0), y + 3, F_YEL);
  fb->drawPixel(x + 3, y + 7, 0); fb->drawPixel(x + 10, y + 7, 0);
  fb->fillRect(x + 2, y + 7, 3, 1, F_ROAD); fb->fillRect(x + 9, y + 7, 3, 1, F_ROAD);
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  for (int i = 0; i < 7; i++) off[i] = fmodf(off[i] + SPD[i] * dt + 190.0f * 200.0f, 190.0f * 200.0f);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) sp.burst(fx, fyDraw, 12, 40, F_YEL);
    lastMm = c.mm;
  }

  // ---- Frosch-Logik ----
  if (state != 0) {
    if (now >= deadUntil) respawn(now);
  } else if (celebUntil && now >= celebUntil) { celebUntil = 0; respawn(now); }
  else if (!celebUntil) {
    if (hopping) {
      if (now - hopStart >= 280) {                       // gelandet
        hopping = false; step = hopFrom + 1;
        const int lane = laneOfStep(step);
        ride = -1;
        if (step == 8) { celebUntil = now + 1200; sp.burst(fx, 33, 16, 45, F_YEL); }
        else if (lane >= 0 && lane < 3) {
          ride = onItem(lane, fx, 0);
          if (ride < 0) { state = 2; deadUntil = now + 900; sp.burst(fx, STEP_Y[step], 12, 35, F_WATER2); }
        } else if (lane >= 4) {
          for (int i = 0; i < itemCount(lane); i++) {
            const float cx = itemX(lane, i, 0) + 7;
            if (fabsf(cx - fx) < 9.0f) { state = 1; deadUntil = now + 900; sp.burst(fx, STEP_Y[step], 10, 40, F_RED); }
          }
        }
        waitUntil = now + 180;
      }
    } else {
      // auf Stamm mitfahren
      const int lane = laneOfStep(step);
      if (lane >= 0 && lane < 3 && ride >= 0) {
        fx += SPD[lane] * dt;
        if (fx < 6 || fx > FB_W - 6) { state = 2; deadUntil = now + 900; sp.burst(fx, STEP_Y[step], 12, 35, F_WATER2); }
      } else if (lane >= 4) {
        for (int i = 0; i < itemCount(lane); i++) {      // wartend ueberfahren?
          const float cx = itemX(lane, i, 0) + 7;
          if (fabsf(cx - fx) < 8.0f) { state = 1; deadUntil = now + 900; sp.burst(fx, STEP_Y[step], 10, 40, F_RED); }
        }
      }
      if (state == 0 && now >= waitUntil) {
        const int next = laneOfStep(step + 1);
        bool go = false;
        if (next < 0) go = true;                         // Insel / Ufer
        else if (next >= 4) {
          const int after = laneOfStep(step + 2);
          go = (roadClear(next, fx, 0.2f, 0.9f, 0.0f) && (after < 4 || roadClear(after, fx, 0.5f, 1.4f, 0.0f))) || rn(100) < 3;
        }
        else {                                           // Wasser: Landung auf Stamm?
          const int it = onItem(next, fx + 0.0f, 0.28f);
          go = it >= 0 || rn(100) < 3;
        }
        const int own = laneOfStep(step);
        if (!go && own >= 4 && !roadClear(own, fx, 0.0f, 0.6f, 0.0f)) {      // Auto naht von hinten: weiterspringen
          go = (next >= 4) ? roadClear(next, fx, 0.15f, 0.4f, 0.0f) : (next < 0 || onItem(next, fx, 0.28f) >= 0);
        }
        if (go) { hopping = true; hopStart = now; hopFrom = step; }
        else {                                           // seitlich nachruecken
          waitUntil = now + (laneOfStep(step) >= 4 ? 120 + rn(160) : 350 + rn(500));
          if (laneOfStep(step) < 0 && rn(3) == 0) fx = fcl(fx + (rn(2) ? 14.0f : -14.0f), 14.0f, FB_W - 14.0f);
        }
      }
    }
  }
  sp.step(dt, 20.0f);

  // ---- Frosch-Zeichenposition ----
  float yy = (float)STEP_Y[step];
  if (hopping) {
    const float k = fcl((float)(uint32_t)(now - hopStart) / 280.0f, 0.0f, 1.0f);
    yy = STEP_Y[hopFrom] + (STEP_Y[hopFrom + 1] - STEP_Y[hopFrom]) * k - 5.0f * sinf(k * 3.1416f);
  }
  fyDraw = yy;

  // ---- Zeichnen ----
  fb->fillRect(0, 0, FB_W, 36, F_GRASS);
  fb->fillRect(0, 36, FB_W, 30, F_WATER);
  fb->fillRect(0, 66, FB_W, 8, F_GRASS);
  fb->fillRect(0, 74, FB_W, 30, F_ROAD);
  fb->fillRect(0, 104, FB_W, 16, F_GRASS);
  for (int x = ((int)(now / 120)) % 8; x < FB_W; x += 8) { fb->drawPixel(x, 41, F_WATER2); fb->drawPixel(x + 4, 51, F_WATER2); fb->drawPixel(x, 61, F_WATER2); }
  for (int x = 0; x < FB_W; x += 12) { fb->drawFastHLine(x, 83, 6, F_LINE); fb->drawFastHLine(x, 93, 6, F_LINE); }
  fb->drawFastHLine(0, 36, FB_W, F_GRASS2); fb->drawFastHLine(0, 73, FB_W, F_GRASS2); fb->drawFastHLine(0, 104, FB_W, F_GRASS2);

  for (int lane = 0; lane < 3; lane++) {                 // Wasser
    const int y = 38 + lane * 10;
    for (int i = 0; i < 2; i++) {
      const int x = (int)itemX(lane, i, 0);
      if (lane == 1) for (int k = 0; k < 3; k++) { fb->fillCircle(x + 4 + k * 8, y + 4, 4, F_TURT); fb->drawPixel(x + 4 + k * 8, y + 3, F_FROG2); }
      else { fb->fillRect(x, y + 1, 32, 7, F_LOG); fb->drawFastHLine(x, y + 7, 32, F_LOG2); fb->drawPixel(x + 4, y + 3, F_LOG2); fb->drawPixel(x + 17, y + 4, F_LOG2); }
    }
  }
  static const uint8_t CC[6] = {F_RED, F_YEL, F_CYA, F_WHT, F_ORA, F_PUR};
  for (int lane = 4; lane < 7; lane++) {                 // Strasse
    const int y = 75 + (lane - 4) * 10;
    for (int i = 0; i < 3; i++) drawCar((int)itemX(lane, i, 0), y, CC[((lane - 4) * 3 + i) % 6], SPD[lane] > 0 ? 1 : -1);
  }

  sp.draw();
  if (state == 0) drawFrog((int)fx, (int)yy, hopping);
  else if (state == 1) { fb->drawLine((int)fx - 3, (int)yy - 3, (int)fx + 3, (int)yy + 3, F_RED); fb->drawLine((int)fx + 3, (int)yy - 3, (int)fx - 3, (int)yy + 3, F_RED); }
  else { fb->drawCircle((int)fx, (int)yy, 3 + (int)((now / 100) % 4), F_WATER2); }

  // Uhr auf dem oberen Ufer
  fb->fillRect(22, 1, 116, 33, F_PANEL);
  fb->drawRect(22, 1, 116, 33, F_GRASS2);
  drawTime(26, 3, 4, c, [](int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, I_WHITE); });
  if (c.showSec && c.timeOk) fb->fillRect(26, 32, ((c.ss + 1) * 108) / 60, 1, F_FROG);

  char b[40];
  if (c.timeOk) snprintf(b, sizeof(b), "%s %02u.%02u.", WD_SHORT[c.wday % 7], c.mday, c.mon);
  else          snprintf(b, sizeof(b), "-- --.--.");
  txt(4, 112, b, F_DIM);
  if (c.hasWx) tempTxt(FB_W - 12 - tempW(c.temp, 1), 112, c.temp, F_DIM);
  else         txtr(FB_W - 12, 112, "--", F_DIM);
}

static void cover(float p, bool, uint32_t) {
  uint8_t* bf = buf();
  for (int band = 0; band < 15; band++) {
    const int y0 = band * 8;
    const float th = (float)band / 30.0f;
    int w = (int)(fcl((p - th) / 0.55f, 0.0f, 1.0f) * FB_W);
    if (w <= 0) continue;
    for (int y = y0; y < y0 + 8 && y < FB_H; y++) {
      if (band & 1) memset(bf + y * FB_W, 0, w);
      else          memset(bf + y * FB_W + (FB_W - w), 0, w);
    }
    if (w < FB_W) fb->drawFastVLine((band & 1) ? w : FB_W - w - 1, y0, 8, F_FROG);
  }
}

}  // namespace f_frog
