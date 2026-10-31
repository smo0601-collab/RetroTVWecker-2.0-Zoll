#pragma once
/*
  face_aqua.h - Zifferblatt "Aquarium"
  Ein Korallenriff im Becken: Wasser mit weichem Farbverlauf, Lichtstrahlen und tanzenden Lichtmustern
  (Kaustik) auf dem Sand, Felsen im Hintergrund, Korallen, Seeanemone, wogender Seetang, Muscheln,
  Seestern und eine Schatztruhe, die ab und zu aufklappt und Blasen ausstoesst. Es schwimmen
  Clownfische, Doktorfische, ein Segelflosser, ein Kugelfisch und ein Schwarm kleiner Neonfische;
  eine Qualle pulsiert durchs Wasser, ein Krebs krabbelt ueber den Sand.
  Gefuettert wird automatisch zur vollen und halben Stunde, beim Einschalten des Designs und per
  Knopf im Browser. Beim Fuettern rieselt Futter von oben, die Fische schwimmen hin und fressen.
  Besuch: Alle paar Minuten (oder per Knopf im Browser) kommt abwechselnd
    - eine freundliche Seeschlange (eigene Figur), die sich durchs Becken schlaengelt; die Fische
      fliehen, der Kugelfisch blaest sich auf, der Krebs versteckt sich,
    - ein kleines Forschungs-U-Boot mit Scheinwerfer, Bullaugen und Propeller; es haelt an,
      sendet ein Sonar-"PING" und faehrt weiter.
  Uebergang: Eine Wasserlinie steigt von unten ueber den Schirm.
*/
#include "faces_fx.h"

namespace f_aqua {
using namespace fc;

enum : uint8_t { A_W1 = 1, A_W2, A_W3, A_W4, A_W5, A_RAY, A_CAU, A_SAND, A_SAND2, A_SAND3, A_SANDL, A_ROCKB, A_ROCKB2,
                 A_ROCK, A_ROCK2, A_KELP, A_KELP2, A_COR1, A_COR1B, A_COR2, A_COR2B, A_COR3, A_ANEM, A_ANEM2,
                 A_SHELL, A_STAR, A_CHEST, A_CHEST2, A_GOLD, A_BUB, A_BUBH,
                 A_CLOWN, A_WHITE, A_BLACK, A_TANG, A_TANG2, A_YEL, A_YEL2, A_ANG, A_ANG2, A_PUFF, A_PUFF2,
                 A_NEON, A_NEON2, A_JELLY, A_JELLY2, A_EYE, A_FOOD, A_CRAB, A_CRAB2,
                 A_SNAKE, A_SNAKE2, A_SNAKEB, A_SNEYE, A_SUB, A_SUB2, A_SUBW, A_BEAM, A_PROP,
                 A_DIG, A_SH, A_DIM };

static const int NF = 6, NFOOD = 24, NB = 12, NS = 8;
enum { K_CLOWN, K_TANG, K_YELLOW, K_ANGEL, K_PUFF };
struct Fish { float x, y, vx, vy, tx, ty; uint8_t kind; uint32_t retarget; };
struct Food { float x, y, vy; bool on; };
struct Bub  { float x, y, vy; uint8_t r; bool on; };
static Fish fish[NF];
static Food food[NFOOD];
static Bub  bub[NB];
static float schX[NS], schY[NS];
static int8_t SIN[256];
static uint32_t lastT, nextBub, chestAt;
static uint8_t  lastMm;
static float    crabX, crabV, jellyX, jellyY, puff;
static volatile bool feedReq = false;               // von der Weboberflaeche gesetzt
static volatile bool visitReq = false;              // Besuch sofort (Weboberflaeche)

// Besuch (Seeschlange / U-Boot)
enum { V_NONE, V_SNAKE, V_SUB };
static uint8_t  visit, nextKind;
static uint32_t visitT, visitAt;
static float    visDir;                              // +1 von links nach rechts, -1 umgekehrt
static const uint32_t SNAKE_MS = 15000, SUB_MS = 17000;

static void requestFeed() { feedReq = true; }
static void requestVisit() { visitReq = true; }

static inline int isin(int a) { return SIN[a & 255]; }        // -127..127

static void enter(uint32_t now) {
  pal(A_W1, 30, 120, 170); pal(A_W2, 18, 96, 150); pal(A_W3, 10, 74, 128); pal(A_W4, 6, 54, 104); pal(A_W5, 4, 38, 80);
  pal(A_RAY, 60, 150, 196); pal(A_CAU, 120, 200, 230);
  pal(A_SAND, 222, 196, 140); pal(A_SAND2, 196, 168, 112); pal(A_SAND3, 170, 142, 92); pal(A_SANDL, 250, 236, 190);
  pal(A_ROCKB, 14, 56, 96); pal(A_ROCKB2, 20, 66, 108); pal(A_ROCK, 104, 108, 128); pal(A_ROCK2, 76, 80, 100);
  pal(A_KELP, 60, 170, 80); pal(A_KELP2, 30, 120, 60);
  pal(A_COR1, 255, 110, 130); pal(A_COR1B, 200, 70, 100); pal(A_COR2, 255, 170, 60); pal(A_COR2B, 210, 120, 40);
  pal(A_COR3, 170, 110, 220); pal(A_ANEM, 255, 160, 200); pal(A_ANEM2, 220, 110, 160);
  pal(A_SHELL, 250, 220, 210); pal(A_STAR, 255, 120, 60);
  pal(A_CHEST, 140, 84, 40); pal(A_CHEST2, 96, 54, 24); pal(A_GOLD, 255, 214, 70);
  pal(A_BUB, 170, 225, 255); pal(A_BUBH, 240, 252, 255);
  pal(A_CLOWN, 255, 120, 30); pal(A_WHITE, 250, 250, 250); pal(A_BLACK, 20, 20, 30);
  pal(A_TANG, 40, 110, 240); pal(A_TANG2, 20, 50, 140); pal(A_YEL, 255, 220, 40); pal(A_YEL2, 220, 170, 20);
  pal(A_ANG, 220, 225, 235); pal(A_ANG2, 40, 40, 60); pal(A_PUFF, 220, 196, 130); pal(A_PUFF2, 150, 120, 70);
  pal(A_NEON, 60, 220, 255); pal(A_NEON2, 255, 60, 80);
  pal(A_JELLY, 230, 170, 240); pal(A_JELLY2, 170, 110, 200);
  pal(A_EYE, 10, 10, 20); pal(A_FOOD, 255, 190, 90); pal(A_CRAB, 230, 80, 60); pal(A_CRAB2, 170, 40, 30);
  pal(A_SNAKE, 70, 190, 120); pal(A_SNAKE2, 40, 130, 90); pal(A_SNAKEB, 210, 240, 150); pal(A_SNEYE, 255, 230, 60);
  pal(A_SUB, 255, 196, 40); pal(A_SUB2, 200, 140, 20); pal(A_SUBW, 170, 240, 255); pal(A_BEAM, 120, 200, 220); pal(A_PROP, 90, 90, 100);
  pal(A_DIG, 240, 252, 255); pal(A_SH, 4, 34, 70); pal(A_DIM, 150, 200, 235);
  for (int i = 0; i < 256; i++) SIN[i] = (int8_t)lroundf(sinf((float)i * 6.2831853f / 256.0f) * 127.0f);
  static const uint8_t KIND[NF] = {K_CLOWN, K_CLOWN, K_TANG, K_YELLOW, K_ANGEL, K_PUFF};
  for (int i = 0; i < NF; i++)
    fish[i] = {(float)(20 + rn(120)), (float)(48 + rn(40)), (rf() - 0.5f) * 14.0f, 0, 80, 70, KIND[i], now + rn(2000)};
  for (int i = 0; i < NS; i++) { schX[i] = 80; schY[i] = 60; }
  for (int i = 0; i < NFOOD; i++) food[i].on = false;
  for (int i = 0; i < NB; i++) bub[i].on = false;
  lastT = now; nextBub = now; lastMm = 255; crabX = 30; crabV = 5; jellyX = 120; jellyY = 70; puff = 0;
  chestAt = now + 9000;
  visit = V_NONE; visitAt = now + 45000 + (uint32_t)rn(45000); nextKind = (uint8_t)(rn(2) ? V_SNAKE : V_SUB);
  feedReq = true;                                    // beim Einschalten des Designs gibt es Futter
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, A_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, A_DIG); }

static void startFeed(uint32_t now) {
  (void)now;
  for (int k = 0; k < 14; k++)
    for (int i = 0; i < NFOOD; i++)
      if (!food[i].on) { food[i] = {(float)(40 + rn(80)), (float)(-rn(30)), 8.0f + rf() * 5.0f, true}; break; }
}

static void addBubble(float x, float y, uint8_t r) {
  for (int i = 0; i < NB; i++) if (!bub[i].on) { bub[i] = {x, y, 12.0f + rf() * 10.0f, r, true}; return; }
}

// ---------- Fische ----------
// Hilfen fuer gespiegelte Zeichnung: d = +1 schaut nach rechts
static void hl(int x, int y, int d, int a, int b, uint8_t c) {   // waagerechte Linie von a bis b (relativ, in Blickrichtung)
  const int x0 = x + d * a, x1 = x + d * b;
  fb->drawFastHLine(x0 < x1 ? x0 : x1, y, abs(x1 - x0) + 1, c);
}
static void vl(int x, int y, int d, int a, int y0, int h, uint8_t c) { fb->drawFastVLine(x + d * a, y + y0, h, c); }

static void drawFish(const Fish& f, uint32_t now) {
  const int d = f.vx >= 0 ? 1 : -1;
  const int x = (int)f.x, y = (int)f.y;
  const int wag = ((now / 140) & 1) ? 1 : 0;                       // Schwanzschlag
  switch (f.kind) {
    case K_CLOWN: {                                                  // Clownfisch: orange, drei weisse Baender
      for (int r = -3; r <= 3; r++) { const int w = r == -3 || r == 3 ? 3 : (abs(r) == 2 ? 5 : 6); hl(x, y + r, d, -w, w, A_CLOWN); }
      vl(x, y, d, 3, -2, 5, A_WHITE); vl(x, y, d, -1, -3, 7, A_WHITE); vl(x, y, d, -4, -2, 5, A_WHITE);
      vl(x, y, d, 2, -2, 5, A_BLACK); vl(x, y, d, -2, -3, 7, A_BLACK);
      fb->fillTriangle(x - d * 6, y, x - d * 10, y - 3 + wag, x - d * 10, y + 3 + wag, A_CLOWN);
      vl(x, y, d, -10, -3 + wag, 7, A_BLACK);
      fb->drawPixel(x + d * 5, y - 1, A_BLACK);
      break;
    }
    case K_TANG: {                                                   // Doktorfisch: blau, schwarzes Band, gelber Schwanz
      for (int r = -4; r <= 4; r++) { const int w = abs(r) == 4 ? 4 : (abs(r) == 3 ? 6 : 8); hl(x, y + r, d, -w + 1, w, A_TANG); }
      hl(x, y - 2, d, -5, 4, A_TANG2); hl(x, y - 1, d, -6, -1, A_TANG2); hl(x, y + 1, d, -4, 2, A_TANG2);
      fb->fillTriangle(x - d * 7, y, x - d * 12, y - 4 + wag, x - d * 12, y + 4 + wag, A_YEL);
      fb->drawPixel(x + d * 6, y - 2, A_BLACK);
      break;
    }
    case K_YELLOW: {                                                 // gelber Segeldoktor: hohe Scheibe
      for (int r = -6; r <= 6; r++) { const int w = abs(r) >= 5 ? 3 : (abs(r) >= 3 ? 5 : 6); hl(x, y + r, d, -w, w, A_YEL); }
      hl(x, y + 6, d, -2, 2, A_YEL2); hl(x, y - 6, d, -2, 2, A_YEL2);
      hl(x, y, d, 5, 8, A_YEL);                                      // Schnauze
      fb->fillTriangle(x - d * 5, y, x - d * 10, y - 4 + wag, x - d * 10, y + 4 + wag, A_YEL2);
      vl(x, y, d, -5, -1, 3, A_WHITE);
      fb->drawPixel(x + d * 4, y - 2, A_BLACK);
      break;
    }
    case K_ANGEL: {                                                  // Segelflosser: silbern, schwarze Streifen, lange Flossen
      for (int r = -4; r <= 4; r++) { const int w = abs(r) >= 3 ? 3 : 5; hl(x, y + r, d, -w, w, A_ANG); }
      fb->fillTriangle(x - d * 3, y - 4, x + d * 2, y - 4, x - d * 4, y - 13, A_ANG);   // Rueckenflosse
      fb->fillTriangle(x - d * 3, y + 4, x + d * 2, y + 4, x - d * 4, y + 13, A_ANG);   // Bauchflosse
      vl(x, y, d, 1, -6, 12, A_ANG2); vl(x, y, d, -3, -10, 20, A_ANG2);
      fb->fillTriangle(x - d * 5, y, x - d * 9, y - 3 + wag, x - d * 9, y + 3 + wag, A_ANG);
      fb->drawPixel(x + d * 3, y - 1, A_BLACK);
      break;
    }
    default: {                                                       // Kugelfisch, blaest sich bei Gefahr auf
      const int r = 4 + (int)(puff * 4.0f);
      fb->fillCircle(x, y, r, A_PUFF);
      for (int k = 0; k < 5; k++) fb->drawPixel(x - d * (r / 2) + (k % 3) * 2 * d - d, y - r / 2 + (k / 3) * 3, A_PUFF2);
      if (puff > 0.5f)                                               // Stacheln
        for (int a = 0; a < 256; a += 32) fb->drawPixel(x + isin(a + 64) * (r + 1) / 127, y + isin(a) * (r + 1) / 127, A_PUFF2);
      fb->fillTriangle(x - d * r, y, x - d * (r + 4), y - 2 + wag, x - d * (r + 4), y + 2 + wag, A_PUFF2);
      fb->drawPixel(x + d * (r - 2), y - 2, A_BLACK);
      fb->drawPixel(x + d * r, y, A_PUFF2);
    }
  }
}

// ---------- Besuch: Seeschlange ----------
static void drawSnake(float t, uint32_t now) {
  // Kopf faehrt quer; der Koerper folgt der gleichen Bahn zeitversetzt
  const float spd = 15.0f;
  const int N = 18;
  int hx = 0, hy = 0;
  for (int k = N - 1; k >= 0; k--) {
    const float tk = t - (float)k * 0.42f;
    const float px = visDir > 0 ? -30.0f + tk * spd : 190.0f - tk * spd;
    const int sy = 68 + isin((int)(tk * 60.0f)) * 9 / 127 + isin((int)(tk * 23.0f)) * 4 / 127;
    const int r = k == 0 ? 6 : 5 - k / 5;
    if (r < 1) continue;
    fb->fillCircle((int)px, sy, r, A_SNAKE2);
    fb->fillCircle((int)px, sy - 1, r - 1 > 0 ? r - 1 : 1, A_SNAKE);
    fb->drawFastHLine((int)px - r / 2, sy + r - 1, r, A_SNAKEB);               // heller Bauch
    if (k > 0 && (k & 1)) fb->fillTriangle((int)px - 2, sy - r + 1, (int)px + 2, sy - r + 1, (int)px - (int)visDir * 2, sy - r - 4, A_SNAKE2);   // Rueckenzacken
    if (k == 0) { hx = (int)px; hy = sy; }
  }
  // Kopf: Auge, Hoernchen, laechelnder Mund
  const int d = visDir > 0 ? 1 : -1;
  fb->fillCircle(hx + d * 4, hy + 1, 4, A_SNAKE);
  fb->fillCircle(hx + d * 1, hy - 3, 3, A_SNEYE);
  fb->fillRect(hx + d * 2 - (d < 0 ? 1 : 0), hy - 4, 2, 2, A_EYE);
  if (((now / 2500) % 5) == 0) fb->drawFastHLine(hx - 2, hy - 3, 6, A_SNAKE);   // blinzeln
  fb->drawLine(hx - d * 1, hy - 6, hx - d * 3, hy - 10, A_SNAKE2); fb->drawLine(hx + d * 2, hy - 6, hx + d * 2, hy - 10, A_SNAKE2);
  fb->drawLine(hx + d * 3, hy + 3, hx + d * 7, hy + 2, A_EYE);
  fb->drawPixel(hx + d * 6, hy, A_EYE);                                       // Nasenloch
}

// ---------- Besuch: U-Boot ----------
static float subX(float t) {
  // faehrt bis zur Mitte, haelt 4 s an (Sonar), faehrt weiter
  const float a = 6.0f, hold = 4.0f;
  float s;
  if (t < a) s = t * 15.0f;
  else if (t < a + hold) s = a * 15.0f;
  else s = a * 15.0f + (t - a - hold) * 15.0f;
  return visDir > 0 ? -40.0f + s : 200.0f - s;
}
static void drawSub(float t, uint32_t now) {
  const int d = visDir > 0 ? 1 : -1;
  const int x = (int)subX(t), y = 60 + isin((int)(t * 40.0f)) * 2 / 127;
  const bool hold = t >= 6.0f && t < 10.0f;
  // Scheinwerferkegel (gerastert)
  const int bx = x + d * 18;
  for (int k = 1; k < 34; k++) {
    const int h = k / 3 + 1, cx = bx + d * k;
    for (int yy = y + 1 - h + k / 4; yy <= y + 1 + h + k / 4; yy++)
      if (((cx + yy) & 1) == 0 && cx >= 0 && cx < FB_W && yy >= 0 && yy < FB_H) fb->drawPixel(cx, yy, A_BEAM);
  }
  // Rumpf
  fb->fillRoundRect(x - 17, y - 6, 34, 13, 6, A_SUB);
  fb->drawFastHLine(x - 13, y + 5, 26, A_SUB2);
  fb->fillRoundRect(x - 6, y - 12, 12, 7, 2, A_SUB);                           // Turm
  fb->drawFastVLine(x + d * 2, y - 17, 5, A_PROP);                             // Periskop
  fb->drawFastHLine(x + d * 2 - (d < 0 ? 2 : 0), y - 17, 3, A_PROP);
  for (int k = -1; k <= 1; k++) {                                              // Bullaugen
    fb->fillCircle(x + k * 8, y, 2, A_SUB2);
    fb->fillRect(x + k * 8 - 1, y - 1, 2, 2, A_SUBW);
  }
  // Propeller hinten
  const int px = x - d * 18;
  if ((now / 80) & 1) fb->drawFastVLine(px - d, y - 4, 9, A_PROP);
  else fb->drawFastHLine(px - d * 3 - (d < 0 ? 0 : 0), y, 4, A_PROP);
  fb->fillRect(px - (d > 0 ? 1 : 0), y - 1, 2, 3, A_SUB2);
  // Sonar beim Anhalten
  if (hold) {
    const float st = t - 6.0f;
    for (int k = 0; k < 2; k++) {
      const float rt = fmodf(st - k * 0.8f, 1.6f);
      if (rt > 0) fb->drawCircle(x, y, 6 + (int)(rt * 40.0f), A_SUBW);
    }
    if (st < 3.0f) { txt(x - 11, y - 28, "PING", ((now / 250) & 1) ? A_SUBW : A_DIG); }
  }
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm && (c.mm == 0 || c.mm == 30)) feedReq = true;
    lastMm = c.mm;
  }
  if (feedReq) { feedReq = false; startFeed(now); }

  // ---- Besuch planen ----
  if (visit == V_NONE && (visitReq || (int32_t)(now - visitAt) >= 0)) {
    visitReq = false;
    visit = nextKind; nextKind = (uint8_t)(visit == V_SNAKE ? V_SUB : V_SNAKE);
    visitT = now; visDir = rn(2) ? 1.0f : -1.0f;
  }
  const float vt = visit ? secs(now, visitT) : 0.0f;
  if (visit && vt * 1000.0f > (visit == V_SNAKE ? SNAKE_MS + 7000 : SUB_MS)) {
    visit = V_NONE; visitAt = now + 120000 + (uint32_t)rn(180000);
  }
  const bool danger = visit == V_SNAKE;
  puff += ((danger ? 1.0f : 0.0f) - puff) * fminf(1.0f, dt * 2.0f);

  // ---- Wasser: Farbverlauf (gerastert), Lichtstrahlen ----
  uint8_t* b = buf();
  const int tR = (int)(now / 40);
  for (int y = 0; y < 104; y++) {
    const int f = y * 4 * 16 / 104;                                  // 0..63
    const uint8_t c0 = (uint8_t)(A_W1 + f / 16), c1 = (uint8_t)(c0 < A_W5 ? c0 + 1 : c0);
    const int th = f & 15;
    uint8_t* row = b + y * FB_W;
    const uint8_t* bay = BAYER4 + (y & 3) * 4;
    for (int x = 0; x < FB_W; x++) row[x] = bay[x & 3] < th ? c1 : c0;
    for (int r = 0; r < 3; r++) {                                    // drei Lichtstrahlen, leicht wandernd (pro Zeile)
      const int rx = 30 + r * 50 + isin(tR / 3 + r * 80) / 16 - y * 2 / 5;
      const int w = 6 + r * 2 - y / 20;
      for (int x = imax(rx, 0); x < rx + w && x < FB_W; x++) if ((x + y) & 1) row[x] = A_RAY;
    }
  }
  // Kaustik nahe der Oberflaeche
  for (int y = 36; y < 46; y++)
    for (int x = 0; x < FB_W; x++) {
      const int v = isin(x * 5 + tR * 2) + isin(y * 11 - tR * 3) + isin((x + y) * 3 + tR);
      if (v > 230) b[y * FB_W + x] = A_RAY;
    }

  // ---- Felsen im Hintergrund ----
  fb->fillCircle(18, 104, 26, A_ROCKB); fb->fillCircle(56, 108, 18, A_ROCKB2); fb->fillCircle(118, 106, 22, A_ROCKB2);
  fb->fillCircle(150, 100, 20, A_ROCKB);

  // ---- Besuch Seeschlange (hinter Riff und Fischen) ----
  if (visit == V_SNAKE) drawSnake(vt, now);

  // ---- Sand mit Kaustik ----
  for (int y = 104; y < FB_H; y++)
    for (int x = 0; x < FB_W; x++) {
      const int v = isin(x * 6 + tR * 2) + isin(y * 14 - tR * 3) + isin((x - y) * 4 + tR);
      uint8_t col = ((x * 7 + y * 13) % 17 == 0) ? A_SAND3 : (((y - 104) * 2 + (isin(x * 9) >> 5)) % 7 == 0 ? A_SAND2 : A_SAND);
      if (v > 200) col = A_SANDL;
      b[y * FB_W + x] = col;
    }
  fb->drawFastHLine(0, 104, FB_W, A_SAND2);

  // ---- Riff: Felsen, Korallen, Anemone, Tang ----
  fb->fillCircle(132, 104, 9, A_ROCK); fb->fillCircle(143, 106, 7, A_ROCK2); fb->fillCircle(124, 107, 5, A_ROCK2);
  // Geweihkoralle (pink)
  for (int k = 0; k < 5; k++) {
    const int bx = 92 + k * 3, h = 10 + (k * 5) % 7;
    fb->drawLine(bx, 104, bx + (k - 2), 104 - h, A_COR1);
    fb->drawLine(bx + 1, 104, bx + (k - 2) + 1, 104 - h, A_COR1B);
    fb->drawLine(bx + (k - 2) / 2, 104 - h / 2, bx + (k - 2) / 2 + (k & 1 ? 3 : -3), 104 - h / 2 - 4, A_COR1);
  }
  // Hirnkoralle (orange)
  fb->fillCircle(62, 104, 7, A_COR2B); fb->fillCircle(62, 103, 6, A_COR2);
  for (int k = -4; k <= 4; k += 2) fb->drawLine(62 + k, 99, 62 + k + 1, 106, A_COR2B);
  // Faecherkoralle (violett) auf dem Felsen
  for (int k = -3; k <= 3; k++) fb->drawLine(134, 96, 134 + k * 3, 84 + abs(k), A_COR3);
  // Seeanemone (wogende Arme)
  for (int k = -5; k <= 5; k++) {
    const int sw = isin(tR * 3 + k * 20) * 3 / 127;
    fb->drawLine(40 + k, 103, 40 + k * 2 + sw, 94 + abs(k) / 2, (k & 1) ? A_ANEM : A_ANEM2);
  }
  fb->fillRect(36, 101, 9, 3, A_ANEM2);
  // Seetang links und rechts
  for (int p = 0; p < 3; p++) {
    const int bx = p == 0 ? 6 : (p == 1 ? 14 : 152), h = p == 1 ? 48 : 40;
    int px = bx;
    for (int s = 0; s < h; s += 3) {
      const int sx = bx + isin(tR * 2 + s * 6 + p * 70) * s / (127 * 7);
      fb->drawLine(px, 104 - s + 3, sx, 104 - s, A_KELP2);
      fb->drawLine(px + 1, 104 - s + 3, sx + 1, 104 - s, A_KELP);
      if ((s / 3) % 3 == 1) fb->fillTriangle(sx + 1, 104 - s, sx + 5, 102 - s, sx + 2, 106 - s, A_KELP);
      px = sx;
    }
  }
  // Muscheln, Seestern
  fb->fillCircle(76, 108, 2, A_SHELL); fb->drawPixel(76, 106, A_SAND3);
  fb->drawLine(110, 111, 116, 111, A_STAR); fb->drawLine(113, 108, 113, 114, A_STAR);
  fb->drawLine(111, 109, 115, 113, A_STAR); fb->drawLine(115, 109, 111, 113, A_STAR);

  // ---- Schatztruhe ----
  const float ct = secs(now, chestAt);
  if ((int32_t)(now - chestAt) >= 0 && ct > 5.0f) chestAt = now + 25000 + (uint32_t)rn(30000);
  const bool open = (int32_t)(now - chestAt) >= 0 && ct < 3.0f;
  fb->fillRect(20, 103, 14, 8, A_CHEST); fb->drawFastHLine(20, 106, 14, A_CHEST2); fb->drawRect(20, 103, 14, 8, A_CHEST2);
  fb->fillRect(26, 106, 2, 2, A_GOLD);
  if (open) {
    fb->fillRect(20, 96, 14, 3, A_CHEST); fb->drawRect(20, 96, 14, 3, A_CHEST2);
    fb->fillRect(22, 101, 10, 2, A_GOLD);
    if (((now / 160) & 3) == 0) addBubble(27.0f, 99.0f, 2);
  } else {
    fb->fillRoundRect(20, 99, 14, 5, 2, A_CHEST); fb->drawFastHLine(20, 103, 14, A_CHEST2);
  }

  // ---- Krebs (versteckt sich bei Gefahr hinter der Truhe) ----
  if (!danger) { crabX += crabV * dt; if (crabX < 44 || crabX > 110) crabV = -crabV; }
  else crabX += (36.0f - crabX) * fminf(1.0f, dt * 1.5f);
  {
    const int cx = (int)crabX;
    fb->fillRect(cx - 3, 108, 7, 3, A_CRAB); fb->drawFastHLine(cx - 2, 107, 5, A_CRAB);
    fb->drawPixel(cx - 5, 107, A_CRAB2); fb->drawPixel(cx + 5, 107, A_CRAB2);
    fb->drawPixel(cx - 4, 106, A_CRAB); fb->drawPixel(cx + 4, 106, A_CRAB);
    fb->drawPixel(cx - 1, 106, A_EYE); fb->drawPixel(cx + 1, 106, A_EYE);
    const int lg = (int)((now / 200) & 1);
    fb->drawPixel(cx - 3 + lg, 111, A_CRAB2); fb->drawPixel(cx + 3 - lg, 111, A_CRAB2);
  }

  // ---- Futter ----
  for (int i = 0; i < NFOOD; i++) {
    Food& f = food[i];
    if (!f.on) continue;
    f.y += f.vy * dt; f.x += (float)isin((int)(now / 6) + i * 20) / 127.0f * 3.0f * dt;
    if (f.y > 104) { f.on = false; continue; }
    if (f.y > 4) fb->fillRect((int)f.x, (int)f.y, 2, 2, A_FOOD);
  }

  // ---- Qualle ----
  {
    const int ph = (int)(now / 12) & 255;
    const float pulse = (float)isin(ph) / 127.0f;
    jellyY -= (pulse > 0.3f ? 6.0f : -2.0f) * dt;
    jellyX += (float)isin((int)(now / 50)) / 127.0f * 3.0f * dt;
    if (jellyY < 42) jellyY = 96;
    if (jellyY > 96) jellyY = 96;
    if (jellyX < 20) jellyX = 20;
    if (jellyX > 140) jellyX = 140;
    const int jx = (int)jellyX, jy = (int)jellyY, w = 6 + (pulse > 0 ? 0 : 1);
    for (int r = 0; r <= w; r++) {                                         // Schirm (obere Halbkugel)
      const int hw = (int)sqrtf((float)(w * w - r * r));
      fb->drawFastHLine(jx - hw, jy - r, 2 * hw + 1, r == w - 1 ? A_WHITE : A_JELLY);
    }
    fb->drawFastHLine(jx - w, jy, 2 * w + 1, A_JELLY2);
    for (int k = -2; k <= 2; k++)
      for (int s = 1; s < 10; s += 2) fb->drawPixel(jx + k * 2 + isin(ph * 2 + s * 20 + k * 30) * 1 / 127, jy + s, (s & 2) ? A_JELLY : A_JELLY2);
  }

  // ---- Fische ----
  const float monX = visit == V_SNAKE ? (visDir > 0 ? -30.0f + vt * 15.0f : 190.0f - vt * 15.0f) : -999.0f;
  for (int i = 0; i < NF; i++) {
    Fish& f = fish[i];
    float bd = 1e9f; int bi = -1;
    if (!danger)
      for (int k = 0; k < NFOOD; k++) {                              // naechstes Futter suchen
        if (!food[k].on || food[k].y < 12) continue;
        const float dx = food[k].x - f.x, dy = food[k].y - f.y, d2 = dx * dx + dy * dy;
        if (d2 < bd) { bd = d2; bi = k; }
      }
    float sp = 13.0f + (float)(f.kind * 2);
    if (f.kind == K_PUFF) sp = 8.0f;
    if (danger && fabsf(monX - f.x) < 70.0f) {                      // vor der Seeschlange fliehen
      f.tx = monX < f.x ? 152.0f : 8.0f; f.ty = f.kind == K_CLOWN ? 98.0f : 50.0f + (float)(i * 8);
      if (f.kind == K_CLOWN) f.tx = 40.0f;                          // Clownfische verstecken sich in der Anemone
      sp *= 2.4f;
      f.retarget = now + 1500;
    } else if (bi >= 0) {
      f.tx = food[bi].x; f.ty = food[bi].y; sp *= 1.9f;
      if (bd < 16.0f) food[bi].on = false;                           // gefressen
    } else if ((int32_t)(now - f.retarget) >= 0) {
      if (f.kind == K_CLOWN && rn(3) == 0) { f.tx = 40.0f + (float)(rn(9) - 4); f.ty = 94.0f; }   // zur Anemone
      else { f.tx = (float)(14 + rn(132)); f.ty = (float)(46 + rn(50)); }
      f.retarget = now + 2500 + (uint32_t)rn(3500);
    }
    const float dx = f.tx - f.x, dy = f.ty - f.y, d = sqrtf(dx * dx + dy * dy) + 0.01f;
    const float wx = dx / d * sp, wy = dy / d * sp * 0.6f;
    f.vx += (wx - f.vx) * 3.0f * dt; f.vy += (wy - f.vy) * 3.0f * dt;
    if (d < 2.0f) { f.vx *= 0.9f; }
    f.x += f.vx * dt; f.y += f.vy * dt;
    f.x = fcl(f.x, 10.0f, (float)FB_W - 10.0f);
    f.y = fcl(f.y, 44.0f, 99.0f);
    drawFish(f, now);
  }

  // ---- Neonfisch-Schwarm ----
  {
    const int t = (int)(now / 30);
    float lx = 80.0f + (float)isin(t) * 55.0f / 127.0f, ly = 66.0f + (float)isin(t * 2 + 40) * 12.0f / 127.0f;
    if (danger) { lx = monX < 80.0f ? 145.0f : 15.0f; ly = 48.0f; }
    const float vxl = (float)isin(t + 64);
    for (int i = 0; i < NS; i++) {
      const float ox = (float)((i % 4) * 5 - 8) * (vxl >= 0 ? -1.0f : 1.0f), oy = (float)((i / 4) * 4 - 2 + (i & 1) * 2);
      schX[i] += (lx + ox - schX[i]) * fminf(1.0f, dt * (danger ? 3.0f : 1.6f));
      schY[i] += (ly + oy - schY[i]) * fminf(1.0f, dt * (danger ? 3.0f : 1.6f));
      const int x = (int)schX[i], y = (int)schY[i], dd = vxl >= 0 ? 1 : -1;
      fb->drawFastHLine(dd > 0 ? x - 2 : x, y, 3, A_NEON);
      fb->drawFastHLine(dd > 0 ? x - 2 : x, y + 1, 3, A_NEON2);
      fb->drawPixel(x - dd * 3, y, A_NEON);
    }
  }

  // ---- U-Boot (vor den Fischen) ----
  if (visit == V_SUB) {
    drawSub(vt, now);
    if (((now / 120) % 3) == 0) addBubble(subX(vt) - visDir * 20.0f, 58.0f, 1);
  }

  // ---- Luftblasen ----
  if ((int32_t)(now - nextBub) >= 0) {
    nextBub = now + 500 + (uint32_t)rn(900);
    addBubble((float)(rn(2) ? 132 + rn(6) : 62), 98.0f, (uint8_t)(1 + rn(2)));
  }
  for (int i = 0; i < NB; i++) {
    if (!bub[i].on) continue;
    bub[i].y -= bub[i].vy * dt;
    if (bub[i].y < 38) { bub[i].on = false; continue; }
    const int bx = (int)bub[i].x + isin((int)(bub[i].y * 8.0f)) / 90, by = (int)bub[i].y;
    fb->drawCircle(bx, by, bub[i].r, A_BUB);
    fb->drawPixel(bx - 1, by - 1, A_BUBH);
  }

  // ---- Zeit ----
  drawTime(26, 4, 4, c, cellS);
  drawTime(26, 4, 4, c, cellD);
  if (c.timeOk) {
    char t[16];
    langWdDate(t, sizeof(t), c.wday, c.mday, c.mon);
    txt(4, 112, t, A_SH);
  }
  if (c.hasWx) tempTxt(FB_W - 4 - tempW(c.temp, 1), 112, c.temp, A_SH);
  if (c.showSec && c.timeOk) fb->fillRect(26, 34, ((c.ss + 1) * 108) / 60, 1, A_DIM);
}

static void cover(float p, bool, uint32_t now) {
  if (p <= 0.0f) return;
  uint8_t* bf = buf();
  if (p >= 0.97f) { memset(bf, A_W4, FB_W * FB_H); return; }
  const int top = FB_H - (int)(p * (FB_H + 6));
  for (int y = (top < 0 ? 0 : top); y < FB_H; y++) memset(bf + y * FB_W, y == top ? A_BUB : A_W4, FB_W);
  (void)now;
}

}  // namespace f_aqua
