#pragma once
/*
  face_elf.h - Zifferblatt "Weihnachtself"
  Ein lustiger Weihnachtself (eigene Figur) arbeitet in der Werkstatt: Ueber ein Foerderband kommen
  Geschenke, er greift sie und wirft sie in den grossen Sack. Ab und zu rutscht er dabei aus und sieht
  Sterne ("HOPPLA!"). Zur vollen Minute huepft er vor Freude und ruft etwas, z. B. wie viele Tage es
  noch bis Weihnachten sind. Draussen vor dem Fenster schneit es, auf der Tafel stehen Datum und
  Temperatur, die Uhrzeit haengt gross an der Wand.
  Uebergang: Ein Geschenkpapier-Vorhang mit Streifen faellt herab.
*/
#include "faces_fx.h"

namespace f_elf {
using namespace fc;

enum : uint8_t { E_WALL = 1, E_WALL2, E_WALL3, E_FLOOR, E_FLOOR2, E_WIN, E_FRAME, E_SNOW, E_BOARD, E_CHALK,
                 E_SHELF, E_BELT, E_BELT2, E_ROLL, E_SACK, E_SACK2, E_ROPE,
                 E_GRN, E_GRN2, E_RED, E_WHT, E_GOLD, E_SKIN, E_SKIN2, E_HAIR, E_EYE, E_BELTE,
                 E_P1, E_P2, E_P3, E_P4, E_RIB, E_DIG, E_SH, E_BUB, E_BTXT, E_STAR, E_TEDDY, E_BALL };

// Elf (12 x 22), schaut nach rechts. Arme werden extra gezeichnet.
static const char* const ELF[22] = {
  ".y..........",
  ".yg.........",
  "..gg........",
  "...gGg......",
  "...gggg.....",
  "..gggggggG..",
  ".rwrwrwrwr..",
  ".ehhsssss...",
  "eehssksks...",
  ".ehssssssss.",
  "..hsspsmmp..",
  "...ssssss...",
  "...gGggggg..",
  "..ggggggggg.",
  "..gggggggGg.",
  "..bbbbbBbbb.",
  "..ggggggggg.",
  "..gGg...gGg.",
  "...rw...rw..",
  "...wr...wr..",
  "...gggg.gggg",
  "..gg..ggg..g",
};
static const char    EK[] = "ygGrwshkpmbBe";
static uint8_t       EC[13];

enum { ST_IDLE, ST_GRAB, ST_TOSS, ST_SLIP, ST_JUMP };
struct Gift { float x; uint8_t col; bool on; };
static Gift     gifts[6];
static uint32_t lastT, stT, nextGift, bubbleUntil;
static uint8_t  st, lastMm, msgIdx, tossCol;
static int      packed;
static char     bubble[24];

static void enter(uint32_t now) {
  pal(E_WALL, 112, 64, 36); pal(E_WALL2, 92, 50, 28); pal(E_WALL3, 132, 80, 44);
  pal(E_FLOOR, 168, 116, 66); pal(E_FLOOR2, 140, 92, 50);
  pal(E_WIN, 18, 28, 74); pal(E_FRAME, 70, 38, 20); pal(E_SNOW, 236, 242, 255);
  pal(E_BOARD, 34, 52, 40); pal(E_CHALK, 226, 232, 214); pal(E_SHELF, 80, 44, 22);
  pal(E_BELT, 50, 52, 60); pal(E_BELT2, 90, 94, 104); pal(E_ROLL, 150, 156, 168);
  pal(E_SACK, 150, 104, 60); pal(E_SACK2, 112, 74, 40); pal(E_ROPE, 220, 190, 120);
  pal(E_GRN, 40, 170, 70); pal(E_GRN2, 22, 110, 46); pal(E_RED, 220, 40, 50); pal(E_WHT, 245, 245, 245);
  pal(E_GOLD, 255, 210, 60); pal(E_SKIN, 255, 206, 168); pal(E_SKIN2, 230, 160, 130); pal(E_HAIR, 220, 110, 40);
  pal(E_EYE, 20, 20, 30); pal(E_BELTE, 60, 36, 20);
  pal(E_P1, 220, 50, 60); pal(E_P2, 60, 110, 230); pal(E_P3, 240, 200, 60); pal(E_P4, 170, 70, 210); pal(E_RIB, 250, 250, 250);
  pal(E_DIG, 255, 240, 200); pal(E_SH, 50, 24, 10); pal(E_BUB, 250, 250, 245); pal(E_BTXT, 30, 30, 40);
  pal(E_STAR, 255, 230, 80); pal(E_TEDDY, 170, 110, 60); pal(E_BALL, 230, 60, 60);
  const uint8_t c[13] = {E_GOLD, E_GRN, E_GRN2, E_RED, E_WHT, E_SKIN, E_HAIR, E_EYE, E_SKIN2, E_RED, E_BELTE, E_GOLD, E_SKIN};
  memcpy(EC, c, sizeof(EC));
  for (int i = 0; i < 6; i++) gifts[i].on = false;
  gifts[0] = {80.0f, E_P1, true}; gifts[1] = {120.0f, E_P2, true};
  lastT = now; st = ST_IDLE; stT = now; nextGift = now + 3000; bubbleUntil = 0; lastMm = 255; msgIdx = 0; packed = 0;
  bubble[0] = 0;
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, E_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, E_DIG); }

static void giftBox(int x, int y, uint8_t col) {               // 8 x 7, (x,y) = links oben
  fb->fillRect(x, y, 8, 7, col);
  fb->drawFastVLine(x + 3, y, 7, E_RIB);
  fb->drawFastHLine(x, y + 3, 8, E_RIB);
  fb->drawPixel(x + 2, y - 1, E_RIB); fb->drawPixel(x + 4, y - 1, E_RIB);
}

// Sprite in doppelter Groesse (2x2 Pixel je Bildpunkt). ori wie blit().
static void blit2(const char* const* rows, int w, int h, int x, int y, const char* keys, const uint8_t* cols, int ori = 0) {
  for (int sy = 0; sy < h; sy++)
    for (int sx = 0; sx < w; sx++) {
      const char ch = rows[sy][sx];
      if (ch == '.' || ch == ' ' || ch == 0) continue;
      const char* k = strchr(keys, ch);
      if (!k) continue;
      int px = sx, py = sy;
      if (ori & 1) px = w - 1 - px;
      if (ori & 2) py = h - 1 - py;
      if (ori & 4) { int t = px; px = py; py = t; }
      fb->fillRect(x + 2 * px, y + 2 * py, 2, 2, cols[k - keys]);
    }
}

static void say(const char* s, uint32_t now, uint32_t ms) { strlcpy(bubble, s, sizeof(bubble)); bubbleUntil = now + ms; }

// Arm von der Schulter (sx,sy) in Richtung (dx,dy), Laenge 11, 3 Pixel dick
static void arm(int sx, int sy, float dx, float dy) {
  const int ex = sx + (int)lroundf(dx * 11.0f), ey = sy + (int)lroundf(dy * 11.0f);
  for (int k = 0; k < 3; k++) fb->drawLine(sx, sy + k, ex, ey + k, k == 2 ? E_GRN2 : E_GRN);
  fb->fillRect(ex - 1, ey, 4, 4, E_SKIN);
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  const float t = secs(now, stT);

  // Tage bis Heiligabend (fuer die Sprechblase)
  int xmasDays = -1;
  if (c.timeOk) xmasDays = daysUntil(c.year, c.mon, c.mday, 12, 24);

  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm && st != ST_SLIP) {
      st = ST_JUMP; stT = now;
      char b[24];
      const bool fest = c.mon == 12 && c.mday >= 24 && c.mday <= 26;
      switch (msgIdx++ % 4) {
        case 0: say("HO HO HO!", now, 2600); break;
        case 1:
          if (fest) say(TR(D_MERRY_XMAS), now, 2600);
          else if (xmasDays >= 0) { snprintf(b, sizeof(b), TR(D_ELF_DAYS), xmasDays); say(b, now, 2600); }
          break;
        case 2: snprintf(b, sizeof(b), TR(D_ELF_GIFTS), packed); say(b, now, 2600); break;
        default: say(TR(D_ELF_YAY), now, 2600); break;
      }
    }
    lastMm = c.mm;
  }

  // ---- Wand, Boden ----
  fb->fillScreen(E_WALL);
  for (int x = 0; x < FB_W; x += 12) { fb->drawFastVLine(x, 0, 96, E_WALL2); fb->drawFastVLine(x + 1, 0, 96, E_WALL3); }
  fb->fillRect(0, 96, FB_W, 24, E_FLOOR);
  for (int y = 100; y < FB_H; y += 6) fb->drawFastHLine(0, y, FB_W, E_FLOOR2);
  fb->drawFastHLine(0, 96, FB_W, E_SHELF);

  // ---- Fenster mit Schneefall ----
  fb->fillRect(4, 8, 38, 30, E_FRAME);
  fb->fillRect(6, 10, 34, 26, E_WIN);
  for (int i = 0; i < 16; i++) {
    const uint32_t h = (uint32_t)i * 2246822519u + 7u;
    const int x = 6 + (int)((h >> 7) % 34), y = 10 + (int)(((now / (70 + (h & 31))) + (h >> 16)) % 26);
    fb->drawPixel(x, y, E_SNOW);
  }
  fb->fillRect(6, 32, 34, 4, E_SNOW);
  fb->drawFastVLine(22, 10, 26, E_FRAME); fb->drawFastHLine(6, 22, 34, E_FRAME);
  fb->fillRect(2, 38, 42, 2, E_SHELF);

  // ---- Tafel (Datum, Temperatur) und Regal ----
  fb->fillRect(102, 38, 54, 22, E_SHELF);
  fb->fillRect(104, 40, 50, 18, E_BOARD);
  char b[24];
  if (c.timeOk) { langWdDate(b, sizeof(b), c.wday, c.mday, c.mon); txtc(129, 41, b, E_CHALK); }
  if (c.hasWx) tempTxt(129 - tempW(c.temp, 1) / 2, 50, c.temp, E_CHALK);
  fb->fillRect(100, 72, 58, 2, E_SHELF);
  fb->fillCircle(108, 68, 3, E_TEDDY); fb->fillCircle(108, 63, 2, E_TEDDY);           // Teddy
  fb->drawPixel(106, 61, E_TEDDY); fb->drawPixel(110, 61, E_TEDDY);
  fb->fillCircle(120, 69, 3, E_BALL); fb->drawPixel(119, 68, E_WHT);                 // Ball
  fb->fillRect(128, 66, 10, 5, E_P2); fb->fillRect(134, 63, 4, 3, E_P2);             // Lok
  fb->fillCircle(130, 71, 1, E_EYE); fb->fillCircle(136, 71, 1, E_EYE);
  fb->fillRect(141, 66, 7, 5, E_P1); fb->fillCircle(143, 71, 1, E_EYE); fb->fillCircle(146, 71, 1, E_EYE);
  fb->fillRect(151, 64, 4, 8, E_P3); fb->drawPixel(152, 63, E_RED);                    // Nussknacker
  // Lichterkette oben
  for (int x = 2; x < FB_W; x += 8) {
    const int y = 1 + (int)(fabsf(sinf((float)x * 0.2f)) * 2.0f);
    fb->drawPixel(x, y, ((x / 8 + now / 500) % 3 == 0) ? E_GOLD : ((x / 8) % 2 ? E_P1 : E_GRN));
  }

  // ---- Foerderband ----
  const int BY = 78, ENDX = 64;
  fb->fillRect(62, BY, 98, 3, E_BELT2);
  fb->fillRect(62, BY + 3, 98, 6, E_BELT);
  for (int x = 70 - (int)((now / 80) % 10); x < FB_W; x += 10) fb->drawFastVLine(x, BY, 3, E_BELT);
  for (int x = 66; x < FB_W; x += 12) fb->fillCircle(x, BY + 6, 2, E_ROLL);
  fb->fillRect(70, BY + 9, 3, 9, E_BELT); fb->fillRect(150, BY + 9, 3, 9, E_BELT);

  // neue Geschenke von rechts
  if ((int32_t)(now - nextGift) >= 0) {
    for (int i = 0; i < 6; i++) if (!gifts[i].on) { gifts[i] = {162.0f, (uint8_t)(E_P1 + rn(4)), true}; break; }
    nextGift = now + 3500 + (uint32_t)rn(3000);
  }
  // Bewegung mit Stau am Ende
  for (int pass = 0; pass < 6; pass++) {
    int best = -1;
    for (int i = 0; i < 6; i++) {                                // jeweils das vorderste noch nicht bewegte suchen
      if (!gifts[i].on) continue;
      int ahead = 0;
      for (int j = 0; j < 6; j++) if (gifts[j].on && gifts[j].x < gifts[i].x) ahead++;
      if (ahead == pass) { best = i; break; }
    }
    if (best < 0) break;
    float lim = (float)ENDX;
    for (int j = 0; j < 6; j++) if (gifts[j].on && gifts[j].x < gifts[best].x) lim = fmaxf(lim, gifts[j].x + 10.0f);
    gifts[best].x = fmaxf(lim, gifts[best].x - 12.0f * dt);
  }
  int front = -1;
  for (int i = 0; i < 6; i++) if (gifts[i].on && gifts[i].x <= ENDX + 0.5f) front = i;

  // ---- Zustandsmaschine des Elfen ----
  if (st == ST_IDLE && front >= 0) { st = ST_GRAB; stT = now; }
  else if (st == ST_GRAB && t > 0.5f) {
    if (front >= 0) { tossCol = gifts[front].col; gifts[front].on = false; st = ST_TOSS; stT = now; }
    else st = ST_IDLE;
  } else if (st == ST_TOSS && t > 1.1f) {
    packed++;
    if (rn(4) == 0) { st = ST_SLIP; stT = now; say(TR(D_ELF_OOPS), now, 2200); }
    else st = ST_IDLE;
  } else if (st == ST_SLIP && t > 2.4f) { st = ST_IDLE; stT = now; }
  else if (st == ST_JUMP && t > 2.6f) { st = ST_IDLE; stT = now; }

  // Geschenke auf dem Band
  for (int i = 0; i < 6; i++) if (gifts[i].on) giftBox((int)gifts[i].x, BY - 7, gifts[i].col);

  // ---- Sack ----
  fb->fillRoundRect(1, 70, 30, 27, 9, E_SACK2);
  fb->fillRoundRect(2, 70, 27, 26, 9, E_SACK);
  fb->fillTriangle(6, 72, 26, 72, 16, 63, E_SACK);
  fb->drawFastHLine(8, 66, 17, E_ROPE); fb->drawFastHLine(8, 67, 17, E_ROPE);
  giftBox(7, 58, E_P3); giftBox(15, 57, E_P2);
  snprintf(b, sizeof(b), "%d", packed);
  txtc(16, 82, b, E_SACK2);

  // ---- Elf ----
  int ex = 34, ey = 52;                                          // Sprite links oben (2x, Fuesse bei y 96)
  if (st == ST_JUMP) ey -= (int)(fabsf(sinf(t * 7.0f)) * 9.0f);
  if (st == ST_SLIP) {
    // liegt auf dem Ruecken (Sprite gedreht), Sterne kreisen
    blit2(ELF, 12, 22, 22, 72, EK, EC, 4 | 2);
    for (int k = 0; k < 3; k++) {
      const float a = t * 5.0f + k * 2.09f;
      const int sx = 50 + (int)(cosf(a) * 9.0f), sy = 72 + (int)(sinf(a) * 3.0f);
      fb->drawPixel(sx, sy, E_STAR); fb->drawPixel(sx - 1, sy, E_STAR); fb->drawPixel(sx + 1, sy, E_STAR);
      fb->drawPixel(sx, sy - 1, E_STAR); fb->drawPixel(sx, sy + 1, E_STAR);
    }
  } else {
    blit2(ELF, 12, 22, ex, ey, EK, EC);
    const int sx = ex + 6, sy = ey + 26;                         // Schultern (hinten, vorne = sx + 10)
    switch (st) {
      case ST_GRAB: arm(sx + 10, sy, 1.0f, 0.1f); arm(sx, sy, 0.9f, 0.4f); break;
      case ST_TOSS: if (t < 0.3f) { arm(sx + 10, sy, -0.3f, -1.0f); arm(sx, sy, -0.6f, -0.8f); }
                    else { arm(sx + 10, sy, 0.3f, 1.0f); arm(sx, sy, -0.2f, 1.0f); } break;
      case ST_JUMP: arm(sx + 10, sy, 0.7f, -0.8f); arm(sx, sy, -0.7f, -0.8f); break;
      default: {
        const bool wave = ((now / 4000) % 3) == 0;
        arm(sx + 10, sy, wave ? 0.6f + 0.4f * sinf(t * 9.0f) : 0.2f, wave ? -0.8f : 1.0f);
        arm(sx, sy, -0.2f, 1.0f);
      }
    }
    if (((now / 2600) % 7) == 0 && st == ST_IDLE) { fb->fillRect(ex + 10, ey + 16, 2, 2, E_SKIN); fb->fillRect(ex + 14, ey + 16, 2, 2, E_SKIN); }   // blinzeln
  }
  // Geschenk im Flug
  if (st == ST_TOSS) {
    const float a = fcl(t / 0.8f, 0.0f, 1.0f);
    const int gx = (int)(ENDX - 6 + (12.0f - (ENDX - 6)) * a), gy = (int)(64.0f - sinf(a * 3.1416f) * 22.0f);
    if (a < 1.0f) giftBox(gx, gy, tossCol);
  }

  // ---- Uhrzeit ----
  drawTime(49, 6, 4, c, cellS);
  drawTime(49, 6, 4, c, cellD);
  if (c.showSec && c.timeOk) fb->drawFastHLine(49, 36, (c.ss + 1) * 108 / 60, E_DIG);

  // ---- Sprechblase ----
  if (bubble[0] && (int32_t)(bubbleUntil - now) > 0) {
    const int w = txtw(bubble) + 8, bx = icl(st == ST_SLIP ? 30 : 46, 2, FB_W - w - 2), by = st == ST_SLIP ? 54 : 40;
    fb->fillRoundRect(bx, by, w, 12, 3, E_BUB);
    const int tx = st == ST_SLIP ? 36 : 52;
    fb->fillTriangle(tx, by + 11, tx + 5, by + 11, tx + 1, by + 15, E_BUB);
    txt(bx + 4, by + 3, bubble, E_BTXT);
  }
}

static void cover(float p, bool, uint32_t) {
  if (p <= 0.0f) return;
  const int h = (int)(p * (FB_H + 8));
  for (int y = 0; y < h && y < FB_H; y++)
    for (int x = 0; x < FB_W; x++) {
      const int d = (x + y) % 16;
      fb->drawPixel(x, y, d < 8 ? E_P1 : (d < 10 ? E_GOLD : E_RED));
    }
  if (h < FB_H) fb->drawFastHLine(0, h, FB_W, E_GOLD);
}

}  // namespace f_elf
