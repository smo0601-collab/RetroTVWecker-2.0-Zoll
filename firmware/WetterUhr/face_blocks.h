#pragma once
/*
  face_blocks.h - Zifferblatt "Fallende Bloecke"
  Ein kleiner Roboter spielt rechts im Schacht (10x20 Felder) mit fallenden Steinen,
  links stehen Uhrzeit, Sekunden-Zellen, naechster Stein, Temperatur und Zeilenzaehler.
  Der Spieler macht absichtlich ab und zu Fehler, der Turm waechst und wird
  gelegentlich abgeraeumt. Jede Minute blinkt der Schachtrand.
  Uebergang: bunte Steine stapeln sich von unten auf.
*/
#include "faces_core.h"

namespace f_blk {
using namespace fc;

enum : uint8_t { K_WELL = 15, K_PANEL, K_DIM, K_BRIGHT, K_GRID, K_EDGE };

static const uint8_t BASE[7][3] = {{0, 220, 230}, {240, 220, 0}, {170, 60, 220}, {60, 210, 60},
                                   {230, 50, 50}, {60, 90, 240}, {250, 150, 30}};
static const int8_t BASEC[7][4][2] = {
  {{0, 1}, {1, 1}, {2, 1}, {3, 1}},   // I
  {{1, 0}, {2, 0}, {1, 1}, {2, 1}},   // O
  {{1, 0}, {0, 1}, {1, 1}, {2, 1}},   // T
  {{1, 0}, {2, 0}, {0, 1}, {1, 1}},   // S
  {{0, 0}, {1, 0}, {1, 1}, {2, 1}},   // Z
  {{0, 0}, {0, 1}, {1, 1}, {2, 1}},   // J
  {{2, 0}, {0, 1}, {1, 1}, {2, 1}},   // L
};
static const uint8_t ROWC[7] = {1, 1, 4, 4, 2, 7, 5};

static int8_t  SH[7][4][4][2];
static uint8_t BD[20][10], TMP[20][10];
static uint8_t ptype, prot, ntype, tr, st;
static int8_t  px, py, tx;
static uint32_t tStep, tMove, tState, pulseT, lastT;
static uint16_t flashRows;
static uint32_t lines;
static uint8_t lastMm, wipeLeft;

static bool fits(int t, int r, int x, int y) {
  for (int k = 0; k < 4; k++) {
    const int cx = x + SH[t][r][k][0], cy = y + SH[t][r][k][1];
    if (cx < 0 || cx >= 10 || cy < 0 || cy >= 20 || BD[cy][cx]) return false;
  }
  return true;
}

static float evalPos(int t, int r, int x, int y) {
  memcpy(TMP, BD, sizeof(BD));
  for (int k = 0; k < 4; k++) TMP[y + SH[t][r][k][1]][x + SH[t][r][k][0]] = 1;
  int full = 0, agg = 0, bump = 0, holes = 0, h[10];
  for (int yy = 0; yy < 20; yy++) {
    bool all = true;
    for (int xx = 0; xx < 10; xx++) if (!TMP[yy][xx]) { all = false; break; }
    if (all) full++;
  }
  for (int xx = 0; xx < 10; xx++) {
    h[xx] = 0;
    bool seen = false;
    for (int yy = 0; yy < 20; yy++) {
      if (TMP[yy][xx]) { if (!seen) { h[xx] = 20 - yy; seen = true; } }
      else if (seen) holes++;
    }
    agg += h[xx];
  }
  for (int xx = 0; xx < 9; xx++) bump += abs(h[xx] - h[xx + 1]);
  return -0.51f * agg + 0.76f * full - 0.36f * holes - 0.18f * bump;
}

static void choose() {
  float best = -1e9f;
  const bool mistake = rn(100) < 16;
  tr = 0; tx = px;
  for (int r = 0; r < 4; r++)
    for (int x = -3; x <= 9; x++) {
      if (!fits(ptype, r, x, 0)) continue;
      int y = 0;
      while (fits(ptype, r, x, y + 1)) y++;
      float s = evalPos(ptype, r, x, y);
      if (mistake) s += rf() * 30.0f;
      if (s > best) { best = s; tr = r; tx = x; }
    }
}

static void spawnPiece(uint32_t now) {
  ptype = ntype; ntype = rn(7);
  prot = 0; px = 3; py = 0; st = 0;
  tStep = tMove = now;
  if (!fits(ptype, prot, px, py)) { st = 2; wipeLeft = 20; tState = now; return; }
  choose();
}

static void enter(uint32_t now) {
  for (int t = 0; t < 7; t++) {
    for (int k = 0; k < 4; k++) { SH[t][0][k][0] = BASEC[t][k][0]; SH[t][0][k][1] = BASEC[t][k][1]; }
    for (int r = 1; r < 4; r++)
      for (int k = 0; k < 4; k++) {
        SH[t][r][k][0] = 3 - SH[t][r - 1][k][1];
        SH[t][r][k][1] = SH[t][r - 1][k][0];
      }
    pal(1 + t, BASE[t][0], BASE[t][1], BASE[t][2]);
    pal(8 + t, imin(255, BASE[t][0] + 90), imin(255, BASE[t][1] + 90), imin(255, BASE[t][2] + 90));
  }
  pal(K_WELL, 12, 12, 24); pal(K_PANEL, 18, 20, 38); pal(K_DIM, 120, 130, 170);
  pal(K_BRIGHT, 230, 230, 255); pal(K_GRID, 24, 24, 42); pal(K_EDGE, 90, 100, 160);
  memset(BD, 0, sizeof(BD));
  lines = 0; flashRows = 0; pulseT = 0; lastMm = 255; lastT = now;
  ntype = rn(7);
  spawnPiece(now);
}

static void lockPiece(uint32_t now) {
  for (int k = 0; k < 4; k++) BD[py + SH[ptype][prot][k][1]][px + SH[ptype][prot][k][0]] = ptype + 1;
  flashRows = 0;
  for (int y = 0; y < 20; y++) {
    bool all = true;
    for (int x = 0; x < 10; x++) if (!BD[y][x]) { all = false; break; }
    if (all) flashRows |= (uint16_t)(1u << y);
  }
  if (flashRows) { st = 1; tState = now; }
  else spawnPiece(now);
}

static void removeRows() {
  int dst = 19, n = 0;
  memcpy(TMP, BD, sizeof(BD));
  memset(BD, 0, sizeof(BD));
  for (int y = 19; y >= 0; y--) {
    if (flashRows & (1u << y)) { n++; continue; }
    memcpy(BD[dst--], TMP[y], 10);
  }
  lines += n;
  flashRows = 0;
}

static void blockAt(int x, int y, uint8_t col) {
  fb->fillRect(x, y, 6, 6, col);
  fb->drawFastHLine(x, y, 6, col + 7);
  fb->drawFastVLine(x, y, 6, col + 7);
}

static void cellT(int x, int y, int s, int, int cy) {
  const uint8_t c = ROWC[cy];
  fb->fillRect(x, y, s, s, c);
  fb->drawFastHLine(x, y, s, c + 7);
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) pulseT = now;
    lastMm = c.mm;
  }

  // ---- Spiellogik ----
  if (st == 0) {
    if (now - tMove >= 70) {
      tMove = now;
      if (prot != tr) {
        const int nr = (prot + 1) & 3;
        if (fits(ptype, nr, px, py)) prot = nr; else tr = prot;
      } else if (px < tx && fits(ptype, prot, px + 1, py)) px++;
      else if (px > tx && fits(ptype, prot, px - 1, py)) px--;
    }
    const bool aligned = prot == tr && px == tx;
    if (now - tStep >= (aligned ? 28u : 340u)) {
      tStep = now;
      if (fits(ptype, prot, px, py + 1)) py++; else lockPiece(now);
    }
  } else if (st == 1) {
    if (now - tState >= 320) { removeRows(); spawnPiece(now); }
  } else if (st == 2) {
    if (now - tState >= 45) {
      tState = now;
      memmove(BD[1], BD[0], 190);
      memset(BD[0], 0, 10);
      if (--wipeLeft == 0) { memset(BD, 0, sizeof(BD)); spawnPiece(now); }
    }
  }

  // ---- Zeichnen ----
  fb->fillScreen(K_PANEL);
  fb->fillRect(100, 0, 60, 120, K_WELL);
  for (int i = 1; i < 10; i++) fb->drawFastVLine(100 + i * 6, 0, 120, K_GRID);
  for (int i = 1; i < 20; i++) fb->drawFastHLine(100, i * 6, 60, K_GRID);
  const bool pulse = (uint32_t)(now - pulseT) < 600 && ((now / 90) & 1);
  fb->drawFastVLine(99, 0, 120, pulse ? I_WHITE : K_EDGE);

  for (int y = 0; y < 20; y++)
    for (int x = 0; x < 10; x++) {
      if (!BD[y][x]) continue;
      if (st == 1 && (flashRows & (1u << y))) {
        fb->fillRect(100 + x * 6, y * 6, 6, 6, ((now / 70) & 1) ? I_WHITE : BD[y][x]);
      } else blockAt(100 + x * 6, y * 6, BD[y][x]);
    }

  if (st == 0) {
    int gy = py;
    while (fits(ptype, prot, px, gy + 1)) gy++;
    if (gy != py)
      for (int k = 0; k < 4; k++)
        fb->drawRect(100 + (px + SH[ptype][prot][k][0]) * 6, (gy + SH[ptype][prot][k][1]) * 6, 6, 6, K_EDGE);
    for (int k = 0; k < 4; k++)
      blockAt(100 + (px + SH[ptype][prot][k][0]) * 6, (py + SH[ptype][prot][k][1]) * 6, ptype + 1);
  }

  // ---- linke Tafel ----
  drawTime(8, 6, 3, c, cellT);
  const int lit = c.timeOk ? c.ss / 6 : 0;
  for (int i = 0; i < 10; i++) {
    uint8_t col = K_GRID;
    if (c.showSec && c.timeOk) {
      if (i < lit) col = ROWC[i % 7];
      else if (i == lit && ((now / 250) & 1)) col = K_DIM;
    } else if (!c.showSec) col = K_GRID;
    fb->fillRect(8 + i * 8, 32, 7, 4, col);
  }

  txt(8, 44, "NEXT", K_DIM);
  fb->drawRect(8, 53, 25, 25, K_EDGE);
  for (int k = 0; k < 4; k++)
    fb->fillRect(10 + SH[ntype][0][k][0] * 5, 55 + SH[ntype][0][k][1] * 5, 5, 5, ntype + 1);

  txt(44, 44, "TEMP", K_DIM);
  if (c.hasWx) tempTxt(44, 54, c.temp, K_BRIGHT, 2);
  else         txt(44, 54, "--", K_BRIGHT, 2);

  txt(44, 76, "LINES", K_DIM);
  char b[16];
  snprintf(b, sizeof(b), "%lu", (unsigned long)lines);
  txt(44, 86, b, K_BRIGHT, 2);

  if (c.timeOk) snprintf(b, sizeof(b), "%s %02u.%02u.", WD_SHORT[c.wday % 7], c.mday, c.mon);
  else          snprintf(b, sizeof(b), "-- --.--.");
  txt(8, 108, b, K_DIM);
}

static void cover(float p, bool, uint32_t) {
  for (int cy = 0; cy < 20; cy++)
    for (int cx = 0; cx < 27; cx++) {
      const uint32_t h = (uint32_t)(cx * 2654435761u) >> 29;                  // 0..7
      const float j = (float)(h % 6);
      if ((float)(19 - cy) < p * 26.0f - j) {
        const uint32_t k = ((uint32_t)(cx * 73856093u) ^ (uint32_t)(cy * 19349663u)) >> 7;
        blockAt(cx * 6, cy * 6, 1 + (uint8_t)(k % 7));
      }
    }
}

}  // namespace f_blk
