#pragma once
/*
  face_neonbrk.h - Zifferblatt "Neonbrecher"
  Neon-Spielfeld mit leuchtendem Rahmen. Oben, ueber dem Feld, steht die Uhrzeit als
  Anzeige mit Datum und Wetter. Im Feld leuchtende Bloecke in Cyan, Magenta, Gelb und
  Gruen (manche brauchen zwei Treffer), ein Leuchtball und ein Schlaeger. Ab und zu
  schwebt aus einem Block eine Kapsel nach unten; faengt der Schlaeger sie, wird er
  kurz breiter. Zur vollen Minute blitzt der Rahmen weiss.
  Uebergang: Rechteck-Blende von aussen nach innen.
*/
#include "faces_fx.h"

namespace f_arc {
using namespace fc;

enum : uint8_t { A_CYA = 1, A_MAG, A_YEL, A_GRN, A_CYA2, A_MAG2, A_YEL2, A_GRN2, A_PAD, A_PAD2, A_DIM, A_FRAME, A_FRAME2 };

static uint8_t hp[4][10];                       // 0 weg, 1 Treffer uebrig, 2 zwei Treffer
static Ball ball;
static float padX, padW, padOff;
static uint32_t lastT, flashUntil, widenUntil, nextRow, nextCap;
static uint8_t lastMm, rowsBuilt;
static int hits;
static float capX, capY;
static bool capOn, padSet;
static Sparks<28> sp;

static void enter(uint32_t now) {
  static const uint8_t C[4][3] = {{0, 230, 255}, {255, 40, 200}, {255, 230, 40}, {60, 255, 120}};
  for (int i = 0; i < 4; i++) {
    pal(A_CYA + i, C[i][0], C[i][1], C[i][2]);
    pal(A_CYA2 + i, C[i][0] / 3, C[i][1] / 3, C[i][2] / 3);
  }
  pal(A_PAD, 120, 240, 255); pal(A_PAD2, 255, 60, 210); pal(A_DIM, 130, 150, 200);
  pal(A_FRAME, 0, 200, 255); pal(A_FRAME2, 0, 70, 100);
  for (int r = 0; r < 4; r++) for (int c = 0; c < 10; c++) hp[r][c] = ((c + r * 3) % 7 == 0) ? 2 : 1;
  rowsBuilt = 4;
  ball = {80.0f, 100.0f, 52.0f, -66.0f};
  padX = 80; padW = 22; padOff = 0; padSet = false; hits = 0; capOn = false;
  lastT = now; flashUntil = widenUntil = 0; nextRow = 0; nextCap = now + 9000; lastMm = 255;
  sp.clear();
}

static void cellD(int x, int y, int s, int, int) {
  fb->fillRect(x, y, s, s, A_PAD);
}
static void cellG(int x, int y, int s, int, int) {
  fb->fillRect(x - 1, y - 1, s + 2, s + 2, A_FRAME2);
}

static int liveCount() {
  int n = 0;
  for (int r = 0; r < 4; r++) for (int c = 0; c < 10; c++) n += hp[r][c] ? 1 : 0;
  return n;
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) flashUntil = now + 450;
    lastMm = c.mm;
  }

  if (liveCount() == 0 && rowsBuilt >= 4) { rowsBuilt = 0; nextRow = now + 400; }
  if (rowsBuilt < 4 && now >= nextRow) {
    for (int cc = 0; cc < 10; cc++) hp[rowsBuilt][cc] = ((cc + rowsBuilt * 3) % 7 == 0) ? 2 : 1;
    rowsBuilt++; nextRow = now + 260;
  }

  // ---- Schlaeger-Breite ----
  const float wantW = now < widenUntil ? 34.0f : 22.0f;
  padW += (wantW - padW) * fcl(dt * 6.0f, 0.0f, 1.0f);

  // ---- Ball ----
  ballMove(ball, dt, 5.0f, 38.0f, FB_W - 6.0f, 119.0f);
  if (ball.y < 70) {
    const int col = ((int)ball.x - 4) / 15, row = ((int)ball.y - 40) / 7;
    if (col >= 0 && col < 10 && row >= 0 && row < 4 && hp[row][col]) {
      hp[row][col]--;
      sp.burst(4 + col * 15 + 7.0f, 40.0f + row * 7 + 2, hp[row][col] ? 3 : 8, 38, A_CYA + row);
      ball.vy = -ball.vy;
      hits++;
      if (!hp[row][col] && !capOn && now >= nextCap) { capOn = true; capX = 4 + col * 15 + 7.0f; capY = 40.0f + row * 7; nextCap = now + 18000; }
    }
  }
  if (ball.vy > 0 && ball.y > 70) {
    if (!padSet) { padSet = true; padOff = (hits % 9 == 8) ? 30.0f : (float)(rn(11) - 5); }
  } else padSet = false;
  const float want = ball.x + padOff;
  const float dx = want - padX, mv = 105.0f * dt;
  padX += fabsf(dx) < mv ? dx : (dx > 0 ? mv : -mv);
  padX = fcl(padX, padW * 0.5f + 5.0f, FB_W - 5.0f - padW * 0.5f);
  if (ball.vy > 0 && ball.y + 2 >= 108 && ball.y < 112 && fabsf(ball.x - padX) <= padW * 0.5f + 2) {
    ball.vy = -fabsf(ball.vy);
    ball.vx = (ball.x - padX) * 5.0f + (ball.vx > 0 ? 12.0f : -12.0f);
    ballSpeed(ball, 84.0f);
    ball.y = 106;
  }
  if (ball.y > 117) {
    sp.burst(ball.x, 116, 8, 35, A_PAD2);
    ball = {padX, 100.0f, (rn(2) ? 52.0f : -52.0f), -66.0f};
    hits++;
  }
  if (capOn) {                                              // Kapsel faellt
    capY += 24.0f * dt;
    if (capY >= 106 && capY < 112 && fabsf(capX - padX) < padW * 0.5f + 4) {
      capOn = false; widenUntil = now + 8000; sp.burst(capX, 108, 10, 40, A_YEL);
    } else if (capY > 117) capOn = false;
  }
  sp.step(dt, 25.0f);

  // ---- Zeichnen ----
  fb->fillScreen(0);
  const bool flash = now < flashUntil && ((now / 70) & 1);
  const uint8_t fcol = flash ? I_WHITE : A_FRAME;
  fb->drawRect(0, 0, FB_W, FB_H, A_FRAME2);
  fb->drawRect(1, 1, FB_W - 2, FB_H - 2, fcol);
  fb->drawFastHLine(2, 35, FB_W - 4, flash ? I_WHITE : A_PAD2);
  drawTime(39, 4, 3, c, cellG);
  drawTime(39, 4, 3, c, cellD);

  char b[40];
  if (c.timeOk) langWdDate(b, sizeof(b), c.wday, c.mday, c.mon);
  else          snprintf(b, sizeof(b), "-- --.--.");
  txt(5, 27, b, A_DIM);
  if (c.hasWx) tempTxt(FB_W - 6 - tempW(c.temp, 1), 27, c.temp, A_DIM);
  else         txtr(FB_W - 6, 27, "--", A_DIM);
  if (c.showSec && c.timeOk) fb->fillRect(39, 33, ((c.ss + 1) * 81) / 60, 1, A_CYA);

  for (int r = 0; r < 4; r++)
    for (int cc = 0; cc < 10; cc++) {
      if (!hp[r][cc]) continue;
      const int x = 4 + cc * 15, y = 40 + r * 7;
      if (hp[r][cc] == 2) { fb->fillRect(x, y, 14, 5, A_CYA2 + r); fb->drawRect(x, y, 14, 5, A_CYA + r); fb->drawPixel(x + 7, y + 2, I_WHITE); }
      else                { fb->fillRect(x, y, 14, 5, A_CYA + r); fb->drawFastHLine(x, y, 14, I_WHITE); }
    }
  sp.draw();
  if (capOn) {
    const uint8_t cc = A_CYA + ((now / 120) & 3);
    fb->fillRect((int)capX - 4, (int)capY, 8, 4, cc);
    fb->drawRect((int)capX - 4, (int)capY, 8, 4, I_WHITE);
  }
  fb->fillRect((int)ball.x - 2, (int)ball.y - 2, 5, 5, A_FRAME2);
  fb->fillRect((int)ball.x - 1, (int)ball.y - 1, 3, 3, I_WHITE);
  const int pw = (int)padW, px0 = (int)padX - pw / 2;
  fb->fillRect(px0, 109, pw, 3, A_PAD);
  fb->fillRect(px0, 109, 3, 3, A_PAD2);
  fb->fillRect(px0 + pw - 3, 109, 3, 3, A_PAD2);
}

static void cover(float p, bool, uint32_t) {
  const float q = 1.0f - p;
  const int hw = (int)(q * 82.0f), hh = (int)(q * 62.0f);
  uint8_t* bf = buf();
  const int top = 60 - hh, bot = 60 + hh, l = 80 - hw, r = 80 + hw;
  for (int y = 0; y < FB_H; y++) {
    uint8_t* row = bf + y * FB_W;
    if (y < top || y >= bot || hw <= 0) { memset(row, 0, FB_W); continue; }
    if (l > 0) memset(row, 0, imin(l, FB_W));
    if (r < FB_W) memset(row + imax(r, 0), 0, FB_W - imax(r, 0));
  }
  if (hw > 0 && hh > 0) fb->drawRect(l, top, 2 * hw, 2 * hh, A_PAD2);
}

}  // namespace f_arc
