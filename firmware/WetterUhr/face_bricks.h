#pragma once
/*
  face_bricks.h - Zifferblatt "Ziegelbrecher"
  Schwarzer Hintergrund, fuenf Reihen bunter Steine oben (rot, orange, gelb, gruen, blau).
  Ein Ball prallt an Raendern, Steinen und der (unsichtbaren) Uhrzeit-Zone ab, ein Schlaeger
  folgt ihm mit kleiner Verzoegerung und verfehlt ihn gelegentlich. Getroffene Steine
  zerfallen in Funken; sind alle weg, baut sich das Feld Reihe fuer Reihe neu auf.
  Die Uhrzeit steht gross in der Mitte, Ziffern in den Farben der Reihen.
  Zur vollen Minute blinken alle Steine weiss. Uebergang: Steinvorhang faellt von oben.
*/
#include "faces_fx.h"

namespace f_brk {
using namespace fc;

enum : uint8_t { B_R1 = 1, B_R2, B_R3, B_R4, B_R5, B_H1, B_H2, B_H3, B_H4, B_H5, B_PAD, B_DIM, B_PAD2 };

static uint8_t alive[5][10];
static Ball ball;
static float padX, padOff;
static uint32_t lastT, flashUntil, nextRow, missAt;
static uint8_t lastMm, rowsBuilt;
static int hits;
static Sparks<28> sp;

static void enter(uint32_t now) {
  static const uint8_t C[5][3] = {{230, 40, 40}, {250, 140, 30}, {240, 220, 40}, {50, 200, 70}, {60, 110, 240}};
  for (int i = 0; i < 5; i++) {
    pal(B_R1 + i, C[i][0], C[i][1], C[i][2]);
    pal(B_H1 + i, imin(255, C[i][0] + 80), imin(255, C[i][1] + 80), imin(255, C[i][2] + 80));
  }
  pal(B_PAD, 200, 210, 230); pal(B_DIM, 140, 140, 170); pal(B_PAD2, 230, 60, 60);
  memset(alive, 1, sizeof(alive));
  rowsBuilt = 5;
  ball = {80.0f, 100.0f, 50.0f, -62.0f};
  padX = 80; padOff = 0; hits = 0;
  lastT = now; flashUntil = 0; nextRow = 0; missAt = 0; lastMm = 255;
  sp.clear();
}

static void cellD(int x, int y, int s, int, int cy) {
  static const uint8_t ROW[7] = {B_R1, B_R2, B_R3, B_R4, B_R5, B_R5, B_R5};
  fb->fillRect(x, y, s, s, ROW[cy]);
  fb->drawFastHLine(x, y, s, B_H1 + (ROW[cy] - B_R1));
}

static int liveCount() {
  int n = 0;
  for (int r = 0; r < 5; r++) for (int c = 0; c < 10; c++) n += alive[r][c];
  return n;
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) flashUntil = now + 500;
    lastMm = c.mm;
  }

  // ---- Neuaufbau: leeres Feld fuellt sich Reihe fuer Reihe ----
  if (liveCount() == 0 && rowsBuilt >= 5) { rowsBuilt = 0; nextRow = now + 400; }
  if (rowsBuilt < 5 && now >= nextRow) { memset(alive[rowsBuilt], 1, 10); rowsBuilt++; nextRow = now + 260; }

  // ---- Ball ----
  ballMove(ball, dt, 2.0f, 2.0f, FB_W - 3.0f, 119.0f);
  ballRect(ball, 2.0f, 22.0f, 42.0f, 138.0f, 78.0f);                   // Uhrzeit-Zone (unsichtbar)
  if (ball.y < 40) {                                                    // Steine
    const int col = (int)ball.x / 16, row = ((int)ball.y - 4) / 7;
    if (col >= 0 && col < 10 && row >= 0 && row < 5 && alive[row][col]) {
      alive[row][col] = 0;
      sp.burst(col * 16 + 8.0f, 4.0f + row * 7 + 2, 7, 40, B_H1 + row);
      ball.vy = -ball.vy;
      hits++;
    }
  }
  // Schlaeger folgt dem Ball; alle paar Treffer absichtlich daneben
  if (ball.vy > 0 && ball.y > 60) {
    if (!missAt) { missAt = 1; padOff = (hits % 7 == 6) ? 26.0f : (float)(rn(13) - 6); }
  } else missAt = 0;
  const float want = ball.x + padOff;
  const float dx = want - padX, mv = 110.0f * dt;
  padX += fabsf(dx) < mv ? dx : (dx > 0 ? mv : -mv);
  padX = fcl(padX, 12.0f, FB_W - 12.0f);
  if (ball.vy > 0 && ball.y + 2 >= 109 && ball.y < 113 && fabsf(ball.x - padX) <= 14.0f) {
    ball.vy = -fabsf(ball.vy);
    ball.vx = (ball.x - padX) * 5.0f + (ball.vx > 0 ? 12.0f : -12.0f);
    ballSpeed(ball, 78.0f);
    ball.y = 107;
  }
  if (ball.y > 117) {                                                   // verfehlt: neuer Ball
    sp.burst(ball.x, 116, 8, 35, B_PAD2);
    ball = {padX, 100.0f, (rn(2) ? 48.0f : -48.0f), -62.0f};
    hits++;
  }
  sp.step(dt, 30.0f);

  // ---- Zeichnen ----
  fb->fillScreen(0);
  const bool flash = now < flashUntil && ((now / 80) & 1);
  for (int r = 0; r < 5; r++)
    for (int cc = 0; cc < 10; cc++) {
      if (!alive[r][cc]) continue;
      const int x = cc * 16 + 1, y = 4 + r * 7;
      fb->fillRect(x, y, 14, 5, flash ? I_WHITE : B_R1 + r);
      if (!flash) fb->drawFastHLine(x, y, 14, B_H1 + r);
    }
  sp.draw();
  drawTime(26, 46, 4, c, cellD);
  if (c.showSec && c.timeOk) fb->fillRect(26, 77, ((c.ss + 1) * 108) / 60, 1, B_DIM);

  char b[40];
  if (c.timeOk) langWdDate(b, sizeof(b), c.wday, c.mday, c.mon);
  else          snprintf(b, sizeof(b), "-- --.--.");
  txt(4, 84, b, B_DIM);
  if (c.hasWx) tempTxt(FB_W - 4 - tempW(c.temp, 1), 84, c.temp, B_DIM);
  else         txtr(FB_W - 4, 84, "--", B_DIM);

  fb->fillRect((int)ball.x - 1, (int)ball.y - 1, 3, 3, I_WHITE);
  fb->fillRect((int)padX - 12, 110, 24, 3, B_PAD);
  fb->fillRect((int)padX - 12, 110, 3, 3, B_PAD2);
  fb->fillRect((int)padX + 9, 110, 3, 3, B_PAD2);
}

static void cover(float p, bool, uint32_t) {
  for (int r = 0; r < 20; r++)
    for (int cc = 0; cc < 10; cc++) {
      const float th = (r + ((cc * 7) % 5) * 0.5f) / 24.0f;            // 0..1: obere Zeilen zuerst
      if (p > th) {
        const int x = cc * 16 + 1, y = r * 6;
        fb->fillRect(x - 1, y, 16, 6, 0);
        fb->fillRect(x, y, 14, 5, B_R1 + (r % 5));
        fb->drawFastHLine(x, y, 14, B_H1 + (r % 5));
      }
    }
}

}  // namespace f_brk
