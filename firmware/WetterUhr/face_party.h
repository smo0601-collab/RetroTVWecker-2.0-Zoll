#pragma once
/*
  face_party.h - Einblendung am Feier- oder besonderen Tag (Kalender, cal.h)
  Fuer jeden Tag eine kleine, lustige Szene (160x120, animiert), oben rechts die Uhrzeit, unten der Name
  des Tages. Wird alle paar Minuten fuer 20 Sekunden statt des Designs gezeigt (Einstellung im Browser:
  Kalender), die Laufschrift laeuft mit einem passenden Spruch darueber.
  Szenen: Feuerwerk (Neujahr/Silvester), Sternsinger, Blumenstrauss, Fisch, Osterhase, Maibaum,
  Bollerwagen, Pfingstochse, Blumenteppich, Kraeuterstrauss, Luftballons, Fahne mit Brandenburger Tor,
  Luthers Thesentuer, Kuerbisse, Grablichter, Regenschirm, Weihnachtsbaum, Herzen, Schere und Krawatte,
  Konfetti-Clown, Erntekorb, Laternenumzug, Nikolausstiefel, Adventskranz, fliegender Schulranzen,
  Schule mit Papierfliegern.
*/
#include "faces_fx.h"

namespace f_party {
using namespace fc;

static uint8_t id = 0;                    // welcher Tag (cal::CalId), vor dem Einblenden setzen

enum : uint8_t {
  P_SKY = 1, P_SKY2, P_NIGHT, P_NIGHT2, P_WHITE, P_BLACK, P_RED, P_DRED, P_GREEN, P_DGREEN, P_BLUE, P_DBLUE,
  P_YEL, P_GOLD, P_ORANGE, P_BROWN, P_DBROWN, P_SKIN, P_PINK, P_PURPLE, P_GRAY, P_DGRAY, P_LGRAY, P_CYAN,
  P_GRASS, P_GRASS2, P_SAND, P_CREAM, P_MAGENTA, P_BAND, P_BANDT, P_WATER, P_WATER2, P_LIGHT, P_FLAME,
  P_STONE, P_ROOF, P_SNOW, P_LILAC
};

static uint32_t t0, lastT;
static Sparks<60> sp;
static float     fx[12], fy[12], fv[12];
static uint8_t   fc_[12];

static void enter(uint32_t now) {
  pal(P_SKY, 110, 180, 255); pal(P_SKY2, 170, 215, 255); pal(P_NIGHT, 10, 14, 48); pal(P_NIGHT2, 30, 30, 80);
  pal(P_WHITE, 250, 250, 250); pal(P_BLACK, 15, 15, 20); pal(P_RED, 230, 40, 40); pal(P_DRED, 140, 20, 30);
  pal(P_GREEN, 60, 190, 70); pal(P_DGREEN, 20, 110, 40); pal(P_BLUE, 50, 110, 240); pal(P_DBLUE, 20, 50, 140);
  pal(P_YEL, 255, 225, 40); pal(P_GOLD, 230, 170, 20); pal(P_ORANGE, 255, 130, 20); pal(P_BROWN, 150, 90, 40);
  pal(P_DBROWN, 90, 50, 25); pal(P_SKIN, 250, 200, 160); pal(P_PINK, 255, 120, 180); pal(P_PURPLE, 150, 60, 200);
  pal(P_GRAY, 140, 140, 150); pal(P_DGRAY, 70, 70, 80); pal(P_LGRAY, 200, 200, 210); pal(P_CYAN, 60, 220, 230);
  pal(P_GRASS, 70, 170, 60); pal(P_GRASS2, 40, 130, 45); pal(P_SAND, 240, 210, 150); pal(P_CREAM, 255, 240, 210);
  pal(P_MAGENTA, 240, 50, 200); pal(P_BAND, 20, 20, 40); pal(P_BANDT, 255, 240, 150); pal(P_WATER, 40, 120, 200);
  pal(P_WATER2, 90, 170, 235); pal(P_LIGHT, 255, 250, 200); pal(P_FLAME, 255, 180, 40); pal(P_STONE, 120, 110, 100);
  pal(P_ROOF, 180, 60, 50); pal(P_SNOW, 235, 240, 255); pal(P_LILAC, 190, 150, 255);
  t0 = now; lastT = now;
  sp.clear();
  for (int i = 0; i < 12; i++) { fx[i] = (float)rn(FB_W); fy[i] = (float)rn(FB_H); fv[i] = 10 + rf() * 20; fc_[i] = (uint8_t)rn(6); }
}

// ---------- kleine Bausteine ----------
static void sky(uint8_t a, uint8_t b, int h = 100) { fb->fillRect(0, 0, FB_W, h / 2, a); fb->fillRect(0, h / 2, FB_W, h - h / 2, b); }
static void ground(int y, uint8_t a, uint8_t b) {
  fb->fillRect(0, y, FB_W, FB_H - y, a);
  for (int x = 0; x < FB_W; x += 4) fb->drawPixel(x + (y & 2), y + 1, b);
}
static void heart(int x, int y, int s, uint8_t c) {   // x,y = Mitte oben
  fb->fillCircle(x - s / 2, y, s / 2 + 1, c);
  fb->fillCircle(x + s / 2, y, s / 2 + 1, c);
  fb->fillTriangle(x - s - 1, y + 1, x + s + 1, y + 1, x, y + s + s / 2, c);
}
static void star5(int cx, int cy, int r, uint8_t c) {
  int px[10], py[10];
  for (int i = 0; i < 10; i++) {
    const float a = -1.5708f + i * 0.6283f, rr = (i & 1) ? r * 0.45f : r;
    px[i] = cx + (int)lroundf(rr * cosf(a)); py[i] = cy + (int)lroundf(rr * sinf(a));
  }
  for (int i = 0; i < 10; i++) fb->fillTriangle(cx, cy, px[i], py[i], px[(i + 1) % 10], py[(i + 1) % 10], c);
}
static void flower(int x, int y, uint8_t petal, int s = 3) {
  fb->drawFastVLine(x, y, 14, P_DGREEN);
  fb->fillCircle(x - s, y, s, petal); fb->fillCircle(x + s, y, s, petal);
  fb->fillCircle(x, y - s, s, petal); fb->fillCircle(x, y + s, s, petal);
  fb->fillCircle(x, y, s - 1, P_YEL);
}
static void person(int x, int y, uint8_t shirt, float ph, bool arm = false) {   // y = Fuesse
  const int leg = (int)(sinf(ph) * 3);
  fb->drawLine(x, y - 8, x - leg, y, P_DBLUE); fb->drawLine(x, y - 8, x + leg, y, P_DBLUE);
  fb->fillRect(x - 3, y - 17, 7, 10, shirt);
  fb->fillCircle(x, y - 21, 4, P_SKIN);
  if (arm) fb->drawLine(x + 3, y - 15, x + 9, y - 12, P_SKIN);
}
static void rocket(Sparks<60>& s, float t, float& x, float& y, float& v, uint8_t& c, float dt) {
  if (y > 18 + ((int)x * 7) % 34 && v > 0) { y -= v * dt * 4; fb->drawFastVLine((int)x, (int)y, 4, P_LIGHT); if (rn(3) == 0) fb->drawPixel((int)x + rn(3) - 1, (int)y + 5, P_FLAME); }
  else if (v > 0) {
    static const uint8_t C[6] = {P_RED, P_YEL, P_CYAN, P_MAGENTA, P_GREEN, P_WHITE};
    s.burst(x, y, 30, 60, C[c % 6]); v = 0;
  }
  if (v <= 0 && rn(14) == 0) { x = (float)(15 + rn(130)); y = 100; v = 14 + rf() * 10; c = (uint8_t)rn(6); }
  (void)t;
}

// ---------- Szenen ----------
static void sFireworks(float t, float dt, bool eve) {
  sky(P_NIGHT, P_NIGHT2);
  for (int i = 0; i < 24; i++) fb->drawPixel((i * 53) % FB_W, (i * 29) % 60, (i + (int)(t * 2)) % 5 ? P_LGRAY : P_NIGHT);
  for (int i = 0; i < 4; i++) rocket(sp, t, fx[i], fy[i], fv[i], fc_[i], dt);
  sp.step(dt, 18); sp.draw();
  // Stadt-Silhouette
  static const int8_t H[16] = {18, 26, 14, 30, 22, 12, 34, 20, 16, 28, 24, 14, 30, 18, 22, 26};
  for (int i = 0; i < 16; i++) {
    fb->fillRect(i * 10, 100 - H[i], 10, H[i], P_BLACK);
    for (int k = 0; k < H[i] - 6; k += 6) if (((i * 7 + k) % 3) == 0) fb->fillRect(i * 10 + 3, 100 - H[i] + 3 + k, 2, 2, P_YEL);
  }
  fb->fillRect(0, 100, FB_W, 20, P_BLACK);
  // Sektglaeser stossen an
  const int d = (int)(fabsf(sinf(t * 1.5f)) * 8);
  for (int k = 0; k < 2; k++) {
    const int x = 68 + (k ? 18 - d : d);
    fb->fillTriangle(x - 5, 70, x + 5, 70, x, 84, P_LGRAY);
    fb->fillTriangle(x - 4, 72, x + 4, 72, x, 82, P_GOLD);
    fb->drawFastVLine(x, 84, 10, P_LGRAY); fb->drawFastHLine(x - 4, 94, 9, P_LGRAY);
  }
  if (d < 2) for (int i = 0; i < 3; i++) fb->drawPixel(78 + rn(9) - 4, 64 - rn(6), P_YEL);
  txtc(80, 18, eve ? "GUTEN RUTSCH!" : "PROSIT NEUJAHR!", P_YEL, 1);
}

static void sKings(float t) {
  sky(P_NIGHT, P_NIGHT2);
  star5(80 + (int)(sinf(t) * 2), 16, 7, P_YEL);
  for (int i = 0; i < 6; i++) fb->drawLine(80, 24, 70 + i * 4, 40, P_GOLD);
  ground(96, P_SAND, P_GOLD);
  static const uint8_t robe[3] = {P_RED, P_PURPLE, P_DGREEN};
  for (int k = 0; k < 3; k++) {
    float x = fmodf(t * 12 + k * 30, 200) - 30;
    const int xi = (int)x, y = 96;
    fb->fillTriangle(xi - 7, y, xi + 7, y, xi, y - 22, robe[k]);
    fb->fillCircle(xi, y - 25, 5, k == 1 ? P_BROWN : P_SKIN);
    fb->fillRect(xi - 5, y - 33, 11, 4, P_GOLD);
    for (int z = 0; z < 3; z++) fb->fillTriangle(xi - 5 + z * 4, y - 33, xi - 3 + z * 4, y - 33, xi - 4 + z * 4, y - 37, P_GOLD);
    fb->fillRect(xi + 5, y - 14, 4, 4, P_GOLD);
  }
  txtc(80, 44, "C+M+B", P_GOLD, 2);
}

static void sFlowers(float t, bool mama) {
  sky(P_SKY2, P_SKY);
  ground(92, P_GRASS, P_GRASS2);
  // Vase mit Strauss
  fb->fillRoundRect(68, 70, 24, 26, 6, P_BLUE);
  static const uint8_t pc[7] = {P_RED, P_PINK, P_YEL, P_PURPLE, P_ORANGE, P_MAGENTA, P_WHITE};
  for (int i = 0; i < 7; i++) {
    const float a = -1.2f + i * 0.4f + sinf(t * 1.2f + i) * 0.05f;
    const int x = 80 + (int)(sinf(a) * 24), y = 66 - (int)(cosf(a) * 26);
    fb->drawLine(80, 72, x, y, P_DGREEN);
    flower(x, y, pc[i], 4);
  }
  // Herzen steigen auf
  for (int i = 0; i < 5; i++) {
    const float y = fmodf(110 - t * 14 - i * 22, 120);
    heart(20 + i * 30 + (int)(sinf(t + i) * 4), (int)y, 3, i & 1 ? P_PINK : P_RED);
  }
  if (mama) txtc(80, 16, "DANKE MAMA!", P_RED, 2);
}

static void sFish(float t) {
  sky(P_SKY2, P_SKY, 60);
  fb->fillRect(0, 60, FB_W, 60, P_WATER);
  for (int y = 64; y < 120; y += 8) for (int x = ((int)(t * 20) + y) % 16; x < FB_W; x += 16) fb->drawFastHLine(x, y, 6, P_WATER2);
  // springender Fisch
  const float p = fmodf(t, 3.0f) / 3.0f;
  const int x = 30 + (int)(p * 100), y = 70 - (int)(sinf(p * 3.1416f) * 40);
  const float ang = cosf(p * 3.1416f);
  fb->fillCircle(x, y, 7, P_ORANGE);
  fb->fillTriangle(x - (int)(8 * 1), y - (int)(ang * 3), x - 14, y - 6 - (int)(ang * 4), x - 14, y + 6 - (int)(ang * 4), P_ORANGE);
  fb->fillCircle(x + 3, y - 2, 1, P_BLACK);
  if (p > 0.85f || p < 0.1f) sp.burst((float)x, 62, 2, 20, P_WATER2);
  sp.step(1.0f / 20, 30); sp.draw();
  // Teller
  fb->fillRoundRect(100, 96, 50, 10, 5, P_WHITE);
  fb->fillCircle(125, 96, 4, P_YEL);
}

static void sEaster(float t) {
  sky(P_SKY2, P_SKY);
  ground(88, P_GRASS, P_GRASS2);
  static const uint8_t ec[5] = {P_RED, P_YEL, P_CYAN, P_PINK, P_PURPLE};
  for (int i = 0; i < 5; i++) {
    const int x = 14 + i * 30, y = 100 + (i & 1) * 6;
    fb->fillCircle(x, y, 6, ec[i]); fb->fillCircle(x, y - 3, 5, ec[i]);
    fb->drawFastHLine(x - 5, y - 1, 11, P_WHITE);
  }
  // huepfender Hase
  const float p = fmodf(t * 0.6f, 1.0f);
  const int hx = 10 + (int)(fmodf(t * 30, 180)) - 20, hy = 84 - (int)(fabsf(sinf(t * 6)) * 14);
  (void)p;
  fb->fillCircle(hx, hy, 9, P_LGRAY);
  fb->fillCircle(hx + 9, hy - 8, 6, P_LGRAY);
  fb->fillRoundRect(hx + 6, hy - 26, 3, 14, 1, P_LGRAY); fb->fillRoundRect(hx + 11, hy - 25, 3, 13, 1, P_LGRAY);
  fb->drawFastVLine(hx + 7, hy - 23, 9, P_PINK);
  fb->fillCircle(hx - 9, hy - 2, 3, P_WHITE);
  fb->drawPixel(hx + 11, hy - 9, P_BLACK); fb->drawPixel(hx + 15, hy - 7, P_PINK);
  // Korb
  fb->fillRoundRect(hx - 2, hy + 2, 12, 8, 2, P_BROWN);
  fb->fillCircle(hx + 4, hy + 1, 2, P_YEL);
  txtc(62, 4, "FROHE OSTERN!", P_DBLUE, 1);
}

static void sMaypole(float t) {
  sky(P_SKY2, P_SKY);
  ground(96, P_GRASS, P_GRASS2);
  fb->fillRect(78, 10, 5, 88, P_WHITE);
  for (int y = 14; y < 96; y += 8) fb->fillRect(78, y, 5, 4, P_BLUE);
  fb->fillCircle(80, 12, 9, P_DGREEN);
  fb->fillCircle(80, 12, 5, P_GREEN);
  static const uint8_t rc[6] = {P_RED, P_YEL, P_BLUE, P_GREEN, P_MAGENTA, P_ORANGE};
  for (int i = 0; i < 6; i++) {
    const float a = t * 1.4f + i * 1.047f;
    const int ex = 80 + (int)(cosf(a) * 46), ey = 92 + (int)(sinf(a) * 6);
    fb->drawLine(80, 20, ex, ey - 16, rc[i]);
    person(ex, ey, rc[i], t * 8 + i);
  }
  txtc(80, 100, "TANZ IN DEN MAI", P_DGREEN, 1);
}

static void sCart(float t) {
  sky(P_SKY2, P_SKY);
  fb->fillCircle(140, 14, 8, P_YEL);
  ground(92, P_GRASS, P_GRASS2);
  fb->fillRect(0, 92, FB_W, 6, P_SAND);
  const int x = (int)fmodf(t * 22, 220) - 50;
  person(x + 52, 96, P_BLUE, t * 9, true);
  fb->fillTriangle(x + 47, 72, x + 57, 72, x + 52, 66, P_YEL);          // Strohhut
  fb->drawLine(x + 36, 88, x + 61, 84, P_DBROWN);                         // Deichsel
  fb->fillRect(x, 74, 36, 14, P_RED);
  fb->drawRect(x, 74, 36, 14, P_DRED);
  for (int k = 0; k < 2; k++) {                                           // Kisten
    fb->fillRect(x + 3 + k * 16, 62, 14, 12, P_GOLD);
    for (int b = 0; b < 3; b++) fb->fillRect(x + 5 + k * 16 + b * 4, 58, 2, 5, P_DBROWN);
  }
  for (int k = 0; k < 2; k++) {
    const int wx = x + 7 + k * 22;
    fb->fillCircle(wx, 90, 5, P_BLACK);
    const float a = -t * 8;
    fb->drawLine(wx, 90, wx + (int)(cosf(a) * 4), 90 + (int)(sinf(a) * 4), P_LGRAY);
  }
  txtc(62, 4, "PROST VATERTAG!", P_DBLUE, 1);
}

static void sOx(float t) {
  sky(P_SKY2, P_SKY);
  ground(90, P_GRASS, P_GRASS2);
  const int x = 60, y = 70;
  const int nod = (int)(sinf(t * 2) * 2);
  fb->fillRoundRect(x - 26, y - 14, 46, 24, 8, P_BROWN);
  for (int k = 0; k < 4; k++) fb->fillRect(x - 22 + k * 12, y + 8, 5, 14, P_DBROWN);
  fb->fillRoundRect(x + 14, y - 22 + nod, 20, 18, 6, P_BROWN);
  fb->fillRoundRect(x + 26, y - 12 + nod, 12, 9, 4, P_PINK);
  fb->drawPixel(x + 30, y - 8 + nod, P_BLACK); fb->drawPixel(x + 34, y - 8 + nod, P_BLACK);
  fb->fillCircle(x + 22, y - 16 + nod, 1, P_BLACK);
  fb->drawLine(x + 16, y - 22 + nod, x + 10, y - 30 + nod, P_CREAM); fb->drawLine(x + 30, y - 22 + nod, x + 36, y - 30 + nod, P_CREAM);
  static const uint8_t wc[5] = {P_RED, P_YEL, P_MAGENTA, P_WHITE, P_ORANGE};
  for (int k = 0; k < 5; k++) fb->fillCircle(x + 14 + k * 5, y - 24 + nod - (k == 2 ? 2 : 0), 3, wc[k]);   // Blumenkranz
  fb->drawLine(x - 26, y - 8, x - 32, y + 4 + (int)(sinf(t * 3) * 3), P_DBROWN);
  for (int i = 0; i < 6; i++) flower(10 + i * 26 + (i & 1) * 6, 98, wc[i % 5], 2);
  txtc(62, 4, "FROHE PFINGSTEN!", P_DBLUE, 1);
}

static void sCarpet(float t) {
  sky(P_SKY2, P_SKY, 50);
  // Kirche
  fb->fillRect(110, 18, 36, 40, P_LGRAY);
  fb->fillTriangle(106, 20, 150, 20, 128, 4, P_ROOF);
  fb->fillRect(96, 2, 12, 56, P_LGRAY);
  fb->fillTriangle(94, 4, 110, 4, 102, -10, P_ROOF);
  fb->fillRoundRect(122, 40, 12, 18, 6, P_DBROWN);
  fb->fillRect(0, 58, FB_W, 62, P_STONE);
  // Blumenteppich (bewegt sich langsam ins Bild)
  static const uint8_t cc[5] = {P_RED, P_YEL, P_PURPLE, P_WHITE, P_ORANGE};
  const int off = (int)(t * 6) % 16;
  for (int r = 0; r < 4; r++)
    for (int c = -1; c < 11; c++) {
      const int x = c * 16 + off - 4 + (r & 1) * 8, y = 64 + r * 14;
      fb->fillCircle(x, y, 5, cc[(r + c + 10) % 5]);
      fb->fillCircle(x, y, 2, P_YEL);
    }
}

static void sHerbs(float t) {
  sky(P_SKY2, P_SKY);
  fb->fillCircle(24, 18, 10, P_YEL);
  ground(94, P_GRASS, P_GRASS2);
  for (int i = 0; i < 9; i++) {
    const float a = -0.9f + i * 0.22f + sinf(t + i) * 0.04f;
    const int x = 80 + (int)(sinf(a) * 34), y = 74 - (int)(cosf(a) * 40);
    fb->drawLine(80, 90, x, y, P_DGREEN);
    static const uint8_t hc[4] = {P_YEL, P_LILAC, P_WHITE, P_GREEN};
    fb->fillCircle(x, y, 3, hc[i % 4]);
    fb->fillCircle(x - 2, y + 4, 2, P_GREEN);
  }
  fb->fillRect(72, 88, 16, 4, P_RED);
  fb->fillRect(74, 92, 12, 10, P_DGREEN);
}

static void sBalloons(float t) {
  sky(P_SKY2, P_SKY);
  ground(100, P_GRASS, P_GRASS2);
  static const uint8_t bc[8] = {P_RED, P_YEL, P_BLUE, P_GREEN, P_MAGENTA, P_ORANGE, P_CYAN, P_PURPLE};
  for (int i = 0; i < 8; i++) {
    const float y = fmodf(130 - t * (12 + i * 2) - i * 19, 150) - 20;
    const int x = 12 + i * 19 + (int)(sinf(t * 1.5f + i) * 5);
    fb->fillCircle(x, (int)y, 7, bc[i]);
    fb->fillCircle(x - 2, (int)y - 3, 2, P_WHITE);
    fb->drawLine(x, (int)y + 7, x + (int)(sinf(t * 3 + i) * 3), (int)y + 22, P_DGRAY);
  }
  for (int k = 0; k < 4; k++) person(24 + k * 36, 106, bc[k + 2], t * 5 + k, true);
}

static void sUnity(float t) {
  sky(P_SKY2, P_SKY);
  ground(98, P_GRASS, P_GRASS2);
  // Brandenburger Tor
  const int gx = 92;
  fb->fillRect(gx, 52, 62, 8, P_SAND);
  fb->fillRect(gx + 4, 46, 54, 6, P_SAND);
  for (int k = 0; k < 6; k++) fb->fillRect(gx + 2 + k * 11, 60, 5, 38, P_SAND);
  fb->fillRect(gx + 24, 38, 14, 8, P_GOLD);                                // Quadriga
  // wehende Fahne
  fb->drawFastVLine(18, 18, 80, P_DGRAY);
  static const uint8_t fcol[3] = {P_BLACK, P_RED, P_GOLD};
  for (int x = 0; x < 50; x++) {
    const int w = (int)(sinf(t * 5 - x * 0.18f) * 3 * x / 50.0f);
    for (int s = 0; s < 3; s++) fb->drawFastVLine(19 + x, 20 + s * 10 + w, 10, fcol[s]);
  }
  txtc(80, 14, "EINIGKEIT RECHT FREIHEIT", P_DBLUE, 1);
}

static void sThesis(float t) {
  fb->fillRect(0, 0, FB_W, 120, P_STONE);
  for (int y = 0; y < 100; y += 10) for (int x = (y / 10 & 1) * 10; x < FB_W; x += 20) fb->drawRect(x, y, 20, 10, P_DGRAY);
  fb->fillRect(40, 14, 80, 92, P_DBROWN);                                  // Tuer
  fb->fillTriangle(36, 16, 124, 16, 80, 0, P_DGRAY);
  for (int k = 0; k < 4; k++) fb->drawFastVLine(48 + k * 20, 16, 90, P_BROWN);
  // Zettel: immer mehr Thesen
  const int sheets = 1 + ((int)(t / 1.2f)) % 6;
  for (int s = 0; s < sheets; s++) {
    const int x = 50 + (s % 3) * 20, y = 24 + (s / 3) * 34;
    fb->fillRect(x, y, 16, 22, P_CREAM);
    for (int l = 0; l < 5; l++) fb->drawFastHLine(x + 2, y + 4 + l * 4, 12, P_GRAY);
  }
  // Hammer schlaegt
  const float ph = fmodf(t * 2.5f, 1.0f), a = ph < 0.5f ? ph * 2 : 2 - ph * 2;
  const int hx = 132, hy = 50;
  const int ex = hx - (int)(sinf(a * 1.3f) * 18), ey = hy - (int)(cosf(a * 1.3f) * 18);
  fb->drawLine(hx + 10, hy + 18, ex, ey, P_BROWN); fb->drawLine(hx + 11, hy + 18, ex + 1, ey, P_BROWN);
  fb->fillRect(ex - 6, ey - 3, 12, 7, P_GRAY);
  if (a < 0.08f) sp.burst((float)ex - 6, (float)ey, 3, 20, P_YEL);
  sp.step(1.0f / 20, 20); sp.draw();
  txtc(80, 99, "95 THESEN - 1517", P_CREAM, 1);
}

static void sPumpkins(float t) {
  sky(P_NIGHT, P_NIGHT2);
  fb->fillCircle(130, 20, 12, P_LIGHT); fb->fillCircle(125, 17, 12, P_NIGHT);
  ground(96, P_DGRAY, P_BLACK);
  for (int k = 0; k < 3; k++) {
    const int x = 30 + k * 45, y = 90 - (k == 1 ? 6 : 0), r = k == 1 ? 16 : 12;
    fb->fillCircle(x, y, r, P_ORANGE);
    fb->drawFastVLine(x, y - r + 2, 2 * r - 4, P_GOLD);
    fb->fillRect(x - 1, y - r - 4, 3, 5, P_DGREEN);
    const uint8_t glow = (int)(t * 6 + k) % 3 ? P_YEL : P_FLAME;
    fb->fillTriangle(x - r / 2 - 2, y - 3, x - r / 2 + 4, y - 3, x - r / 2 + 1, y - 9, glow);
    fb->fillTriangle(x + r / 2 - 4, y - 3, x + r / 2 + 2, y - 3, x + r / 2 - 1, y - 9, glow);
    for (int z = -r / 2; z < r / 2; z += 4) fb->fillTriangle(x + z, y + 3, x + z + 4, y + 3, x + z + 2, y + 8, glow);
  }
  for (int i = 0; i < 3; i++) {                                           // Fledermaeuse
    const int bx = (int)fmodf(t * (20 + i * 7) + i * 60, 190) - 15, by = 20 + i * 12 + (int)(sinf(t * 3 + i) * 5);
    const int w = (int)(t * 10 + i) % 2 ? 3 : -2;
    fb->fillCircle(bx, by, 2, P_BLACK);
    fb->drawLine(bx, by, bx - 7, by - w, P_BLACK); fb->drawLine(bx, by, bx + 7, by - w, P_BLACK);
  }
  // Gespenst
  const int gx = 80 + (int)(sinf(t * 0.8f) * 50), gy = 40 + (int)(sinf(t * 2) * 4);
  fb->fillCircle(gx, gy, 9, P_SNOW); fb->fillRect(gx - 9, gy, 19, 12, P_SNOW);
  for (int k = 0; k < 3; k++) fb->fillCircle(gx - 6 + k * 6, gy + 12, 3, P_NIGHT2);
  fb->fillCircle(gx - 3, gy - 2, 1, P_BLACK); fb->fillCircle(gx + 3, gy - 2, 1, P_BLACK);
  txtc(80, 99, "BUUUH!", P_ORANGE, 1);
}

static void sCandles(float t) {
  sky(P_NIGHT2, P_DGRAY);
  ground(92, P_DGREEN, P_GRASS2);
  for (int k = 0; k < 6; k++) {
    const int x = 16 + k * 26, y = 96 + (k & 1) * 6;
    fb->fillRect(x - 5, y - 12, 11, 14, P_RED);
    fb->fillRect(x - 3, y - 10, 7, 4, P_ORANGE);
    const int fl = (int)(sinf(t * 9 + k * 2) * 1.5f);
    fb->fillCircle(x + fl / 2, y - 16, 2, P_FLAME);
    fb->drawPixel(x + fl, y - 19, P_YEL);
  }
  for (int i = 0; i < 20; i++) fb->drawPixel((i * 37) % FB_W, (i * 17) % 50, P_LGRAY);
}

static void sUmbrella(float t) {
  sky(P_GRAY, P_LGRAY);
  ground(96, P_GRASS2, P_DGREEN);
  fb->fillCircle(50, 18, 14, P_DGRAY); fb->fillCircle(70, 14, 16, P_DGRAY); fb->fillCircle(92, 20, 13, P_DGRAY);
  for (int i = 0; i < 30; i++) {
    const int x = (i * 23) % FB_W, y = (int)fmodf(i * 13 + t * 70, 90) + 24;
    fb->drawLine(x, y, x - 1, y + 4, P_WATER2);
  }
  person(80, 100, P_PURPLE, 0);
  fb->drawFastVLine(84, 64, 22, P_BLACK);
  fb->fillCircle(84, 64, 16, P_RED); fb->fillRect(68, 64, 33, 17, P_LGRAY);
  // Gedankenblase
  fb->fillCircle(104, 50, 2, P_WHITE); fb->fillCircle(110, 42, 3, P_WHITE); fb->fillCircle(124, 32, 10, P_WHITE);
  txtc(124, 29, "?", P_DBLUE, 1);
}

static void sTree(float t, bool eve) {
  sky(P_NIGHT, P_NIGHT2);
  for (int i = 0; i < 26; i++) {
    const int x = (i * 41 + (int)(t * 4) * (i & 1)) % FB_W, y = (int)fmodf(i * 11 + t * (8 + i % 5), 100);
    fb->drawPixel(x, y, P_SNOW);
  }
  ground(98, P_SNOW, P_LGRAY);
  for (int k = 0; k < 4; k++) fb->fillTriangle(80 - 14 - k * 8, 30 + k * 16, 80 + 14 + k * 8, 30 + k * 16, 80, 12 + k * 14, P_DGREEN);
  fb->fillRect(76, 92, 8, 8, P_DBROWN);
  star5(80, 12, 6, P_YEL);
  static const uint8_t lc[4] = {P_RED, P_YEL, P_CYAN, P_MAGENTA};
  for (int i = 0; i < 16; i++) {
    const int row = i / 4, x = 80 - (8 + row * 8) + (i % 4) * (5 + row * 4), y = 34 + row * 16;
    fb->fillCircle(x, y, 1, ((int)(t * 3) + i) % 3 ? lc[i % 4] : P_DGREEN);
  }
  static const uint8_t gc[3] = {P_RED, P_BLUE, P_GOLD};
  for (int k = 0; k < 3; k++) {
    const int x = 40 + k * 34 + (k == 1 ? 12 : 0), y = 90;
    fb->fillRect(x, y, 16, 12, gc[k]);
    fb->fillRect(x + 7, y, 2, 12, P_YEL); fb->fillRect(x, y + 5, 16, 2, P_YEL);
  }
  txtc(58, 3, eve ? "BESCHERUNG!" : "FROHE WEIHNACHTEN", P_YEL, 1);
}

static void sHearts(float t) {
  sky(P_PINK, P_MAGENTA);
  for (int i = 0; i < 10; i++) {
    const float y = fmodf(130 - t * (10 + i * 2) - i * 13, 140) - 10;
    heart(10 + i * 15, (int)y, 3 + (i % 3), i & 1 ? P_RED : P_WHITE);
  }
  heart(80, 46, 18 + (int)(sinf(t * 6) * 2), P_RED);
  // Pfeil
  const int ax = (int)fmodf(t * 70, 260) - 50;
  fb->drawLine(ax, 70, ax + 30, 52, P_GOLD);
  fb->fillTriangle(ax + 30, 52, ax + 24, 52, ax + 28, 58, P_GOLD);
  fb->drawLine(ax, 70, ax - 4, 66, P_WHITE); fb->drawLine(ax, 70, ax + 1, 75, P_WHITE);
}

static void sScissors(float t) {
  sky(P_SKY2, P_SKY);
  // Mann mit Krawatte
  fb->fillRect(50, 40, 60, 80, P_DBLUE);
  fb->fillCircle(80, 28, 14, P_SKIN);
  fb->fillCircle(74, 26, 2, P_BLACK); fb->fillCircle(86, 26, 2, P_BLACK);
  const float ph = fmodf(t, 4.0f);
  const bool cut = ph > 2.0f;
  fb->fillRect(73, 34, 14, 3, P_DRED);                                     // erschrockener Mund
  fb->fillTriangle(70, 42, 90, 42, 80, 48, P_WHITE);
  fb->fillTriangle(76, 46, 84, 46, 80, 52, P_RED);                          // Knoten
  if (!cut) fb->fillTriangle(75, 52, 85, 52, 80, 100, P_RED);
  else {
    fb->fillTriangle(75, 52, 85, 52, 80, 70, P_RED);                        // Stummel
    const int fall = (int)((ph - 2.0f) * 60);
    fb->fillTriangle(77, 72 + fall, 83, 72 + fall, 80, 100 + fall, P_RED);
  }
  // Schere
  const int sx = 112 - (int)(fminf(ph, 2.0f) * 12), sy = 68;
  const float open = cut ? 0.1f : fabsf(sinf(t * 8)) * 0.5f;
  fb->fillCircle(sx + 22, sy - 6, 4, P_ORANGE); fb->fillCircle(sx + 22, sy + 6, 4, P_ORANGE);
  fb->fillCircle(sx + 22, sy - 6, 2, P_SKY); fb->fillCircle(sx + 22, sy + 6, 2, P_SKY);
  fb->drawLine(sx + 20, sy - 5, sx - 8, sy - (int)(open * 12), P_LGRAY); fb->drawLine(sx + 20, sy - 4, sx - 8, sy + 1 - (int)(open * 12), P_LGRAY);
  fb->drawLine(sx + 20, sy + 5, sx - 8, sy + (int)(open * 12), P_LGRAY); fb->drawLine(sx + 20, sy + 4, sx - 8, sy - 1 + (int)(open * 12), P_LGRAY);
  txtc(80, 6, "ALAAF!", P_RED, 2);
}

static void sClown(float t) {
  sky(P_PURPLE, P_BLUE);
  static const uint8_t cc[6] = {P_RED, P_YEL, P_GREEN, P_CYAN, P_MAGENTA, P_WHITE};
  for (int i = 0; i < 60; i++) {
    const int x = (i * 29 + (int)(sinf(t + i) * 4)) % FB_W, y = (int)fmodf(i * 17 + t * (20 + i % 7), 120);
    fb->fillRect(x, y, 2, 2, cc[i % 6]);
  }
  fb->fillCircle(80, 62, 26, P_WHITE);
  fb->fillCircle(80, 66, 6, P_RED);                                         // Nase
  fb->fillCircle(70, 54, 4, P_BLUE); fb->fillCircle(90, 54, 4, P_BLUE);
  fb->fillCircle(70, 54, 2, P_BLACK); fb->fillCircle(90, 54, 2, P_BLACK);
  for (int x = -12; x <= 12; x++) { fb->drawPixel(80 + x, 80 - (x * x) / 24, P_RED); fb->drawPixel(80 + x, 81 - (x * x) / 24, P_RED); }   // Lachen
  // Narrenkappe mit Schellen
  fb->fillTriangle(56, 44, 80, 40, 48, 14 + (int)(sinf(t * 4) * 4), P_RED);
  fb->fillTriangle(80, 40, 104, 44, 112, 14 - (int)(sinf(t * 4) * 4), P_YEL);
  fb->fillTriangle(66, 40, 94, 40, 80, 8, P_GREEN);
  fb->fillCircle(48, 14 + (int)(sinf(t * 4) * 4), 3, P_GOLD); fb->fillCircle(112, 14 - (int)(sinf(t * 4) * 4), 3, P_GOLD); fb->fillCircle(80, 8, 3, P_GOLD);
  fb->fillRoundRect(2, 88, 156, 19, 4, P_BAND);
  txtc(80, 90, "HELAU! ALAAF!", P_YEL, 2);
}

static void sHarvest(float t) {
  sky(P_SKY2, P_ORANGE);
  ground(90, P_GOLD, P_BROWN);
  for (int i = 0; i < 14; i++) {                                           // Weizen im Wind
    const int x = 6 + i * 11, sway = (int)(sinf(t * 2 + i * 0.5f) * 3);
    fb->drawLine(x, 92, x + sway, 60, P_GOLD);
    for (int k = 0; k < 4; k++) fb->fillRect(x + sway - 1, 60 + k * 3, 3, 2, P_YEL);
  }
  fb->fillRoundRect(48, 86, 64, 22, 6, P_BROWN);
  for (int k = 0; k < 6; k++) fb->drawFastVLine(52 + k * 10, 88, 18, P_DBROWN);
  fb->fillCircle(66, 84, 11, P_ORANGE); fb->fillRect(65, 71, 3, 4, P_DGREEN);
  fb->fillCircle(88, 84, 6, P_RED); fb->fillCircle(98, 86, 6, P_GREEN); fb->fillCircle(80, 80, 5, P_RED);
  txtc(62, 4, "DANKE FUER DIE ERNTE", P_DBROWN, 1);
}

static void sLanterns(float t) {
  sky(P_NIGHT, P_NIGHT2);
  fb->fillCircle(130, 18, 9, P_LIGHT);
  ground(98, P_BLACK, P_DGRAY);
  static const uint8_t lc[5] = {P_RED, P_YEL, P_ORANGE, P_CYAN, P_MAGENTA};
  for (int k = 0; k < 5; k++) {
    const int x = (int)fmodf(t * 15 + k * 36, 200) - 20;
    const int y = 100;
    fb->fillRect(x - 3, y - 18, 7, 12, P_BLACK);                            // Kind (Silhouette)
    fb->fillCircle(x, y - 22, 4, P_BLACK);
    fb->drawLine(x - 1, y - 6, x - 3, y, P_BLACK); fb->drawLine(x + 1, y - 6, x + 3, y, P_BLACK);
    fb->drawLine(x + 3, y - 16, x + 10, y - 26, P_DBROWN);
    const int ly = y - 24 + (int)(sinf(t * 4 + k) * 2);
    fb->fillCircle(x + 10, ly, 5, lc[k]);
    fb->fillCircle(x + 10, ly, 2, P_LIGHT);
  }
  // Martinsgans
  const int gx = 18, gy = 84;
  fb->fillCircle(gx, gy, 7, P_WHITE); fb->fillCircle(gx + 8, gy - 9, 3, P_WHITE);
  fb->drawLine(gx + 4, gy - 4, gx + 7, gy - 8, P_WHITE); fb->drawLine(gx + 5, gy - 4, gx + 8, gy - 8, P_WHITE);
  fb->fillTriangle(gx + 10, gy - 10, gx + 14, gy - 9, gx + 10, gy - 8, P_ORANGE);
  txtc(60, 4, "LATERNE, LATERNE!", P_YEL, 1);
}

static void sBoot(float t) {
  sky(P_NIGHT2, P_DBLUE);
  for (int i = 0; i < 30; i++) {
    const int x = (i * 31) % FB_W, y = (int)fmodf(i * 9 + t * (10 + i % 4), 120);
    fb->drawPixel(x, y, P_SNOW);
  }
  fb->fillRect(0, 96, FB_W, 24, P_BROWN);
  // Stiefel
  fb->fillRect(64, 56, 24, 40, P_RED);
  fb->fillRoundRect(64, 82, 44, 16, 6, P_RED);
  fb->fillRect(62, 52, 28, 8, P_WHITE);
  // Inhalt: Mandarine, Lebkuchen, Zuckerstange
  fb->fillCircle(70, 48, 6, P_ORANGE);
  fb->fillRect(78, 38, 3, 16, P_WHITE);
  for (int k = 0; k < 4; k++) fb->fillRect(78, 38 + k * 4, 3, 2, P_RED);
  fb->fillRoundRect(80, 30, 4, 4, 2, P_WHITE);
  fb->fillCircle(86, 46, 6, P_BROWN);
  // Nikolausmuetze fliegt vorbei
  const int hx = (int)fmodf(t * 30, 220) - 30, hy = 20 + (int)(sinf(t * 2) * 6);
  fb->fillTriangle(hx, hy + 10, hx + 22, hy + 10, hx + 16, hy - 6, P_RED);
  fb->fillRect(hx - 2, hy + 10, 26, 4, P_WHITE);
  fb->fillCircle(hx + 16, hy - 6, 3, P_WHITE);
  txtc(80, 99, "HO HO HO!", P_WHITE, 1);
}

static void sWreath(float t, int n) {
  sky(P_NIGHT2, P_DBROWN);
  fb->fillRect(0, 90, FB_W, 30, P_BROWN);
  fb->fillRoundRect(26, 78, 108, 22, 11, P_DGREEN);
  for (int k = 0; k < 20; k++) fb->fillCircle(32 + k * 5, 82 + (k & 1) * 6, 3, k % 3 ? P_GREEN : P_DGREEN);
  for (int k = 0; k < 5; k++) fb->fillCircle(36 + k * 22, 90, 2, P_RED);
  for (int c = 0; c < 4; c++) {
    const int x = 42 + c * 25, y = 50 + (c & 1) * 6;
    fb->fillRect(x - 5, y, 10, 32 - (c & 1) * 6, P_RED);
    if (c < n) {
      const int fl = (int)(sinf(t * 10 + c * 1.7f) * 1.5f);
      fb->fillCircle(x + fl / 2, y - 6, 3, P_FLAME);
      fb->fillCircle(x + fl / 2, y - 7, 1, P_LIGHT);
      fb->drawPixel(x + fl, y - 10, P_YEL);
    }
    fb->drawFastVLine(x, y - 3, 3, P_BLACK);
  }
  char b[16];
  snprintf(b, sizeof(b), "%d. ADVENT", n);
  txtc(80, 16, b, P_YEL, 2);
}

static void sSchoolBag(float t) {
  sky(P_SKY2, P_SKY);
  fb->fillCircle(136, 16, 10, P_YEL);
  for (int k = 0; k < 8; k++) fb->drawLine(136, 16, 136 + (int)(cosf(k * 0.785f + t) * 18), 16 + (int)(sinf(k * 0.785f + t) * 18), P_YEL);
  fb->fillRect(0, 92, FB_W, 28, P_SAND);
  fb->fillRect(0, 86, FB_W, 6, P_WATER);
  // Sonnenschirm
  fb->drawFastVLine(30, 60, 36, P_DBROWN);
  fb->fillTriangle(8, 62, 52, 62, 30, 48, P_RED);
  fb->fillTriangle(19, 62, 41, 62, 30, 48, P_WHITE);
  // Schulranzen fliegt davon
  const float p = fmodf(t * 0.5f, 1.0f);
  const int bx = 70 + (int)(p * 80), by = 90 - (int)(p * 90 + sinf(p * 9) * 6);
  fb->fillRoundRect(bx - 9, by - 11, 18, 22, 4, P_BLUE);
  fb->fillRect(bx - 9, by - 4, 18, 3, P_YEL);
  fb->drawRect(bx - 5, by + 2, 10, 6, P_YEL);
  sp.burst((float)bx, (float)by + 11, 2, 18, P_ORANGE);
  sp.step(1.0f / 20, 10); sp.draw();
  txtc(62, 18, "FERIEN!", P_DBLUE, 2);
}

static void sSchool(float t) {
  sky(P_SKY2, P_SKY);
  ground(98, P_GRASS, P_GRASS2);
  fb->fillRect(14, 40, 90, 58, P_ROOF);
  fb->fillTriangle(8, 42, 110, 42, 59, 18, P_DRED);
  for (int r = 0; r < 2; r++) for (int c = 0; c < 4; c++) fb->fillRect(22 + c * 20, 50 + r * 18, 12, 10, P_SKY2);
  fb->fillRect(52, 80, 14, 18, P_DBROWN);
  // Glocke laeutet
  const int sw = (int)(sinf(t * 10) * 3);
  fb->fillTriangle(54 + sw, 30, 64 + sw, 30, 59 + sw, 20, P_GOLD);
  fb->fillCircle(59 + sw * 2, 31, 1, P_DBROWN);
  // Papierflieger
  for (int k = 0; k < 3; k++) {
    const float p = fmodf(t * 0.35f + k * 0.33f, 1.0f);
    const int x = 60 + (int)(p * 110), y = 50 - (int)(sinf(p * 3.14f) * 30) + k * 10;
    fb->fillTriangle(x, y, x - 12, y - 4, x - 12, y + 3, P_WHITE);
    fb->drawLine(x, y, x - 12, y + 1, P_LGRAY);
  }
  txtc(128, 100, "SCHULE AUS!", P_DBLUE, 1);
}

// ---------- Zeichnen ----------
static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  const float t = secs(now, t0);
  switch (id) {
    case 0:  sFireworks(t, dt, false); break;                     // Neujahr
    case 1:  sKings(t); break;                                    // Heilige Drei Koenige
    case 2:  sFlowers(t, false); break;                           // Frauentag
    case 3:  sFish(t); break;                                     // Karfreitag
    case 4: case 5: sEaster(t); break;                            // Ostern
    case 6:  sMaypole(t); break;                                  // 1. Mai
    case 7: case 24: sCart(t); break;                             // Himmelfahrt / Vatertag
    case 8: case 9: sOx(t); break;                                // Pfingsten
    case 10: sCarpet(t); break;                                   // Fronleichnam
    case 11: sHerbs(t); break;                                    // Mariae Himmelfahrt
    case 12: sBalloons(t); break;                                 // Weltkindertag
    case 13: sUnity(t); break;                                    // Tag der Deutschen Einheit
    case 14: sThesis(t); break;                                   // Reformationstag
    case 15: sCandles(t); break;                                  // Allerheiligen
    case 16: sUmbrella(t); break;                                 // Buss- und Bettag
    case 17: case 18: sTree(t, false); break;                     // Weihnachten
    case 19: sHearts(t); break;                                   // Valentinstag
    case 20: sScissors(t); break;                                 // Weiberfastnacht
    case 21: case 22: sClown(t); break;                           // Rosenmontag / Faschingsdienstag
    case 23: sFlowers(t, true); break;                            // Muttertag
    case 25: sHarvest(t); break;                                  // Erntedank
    case 26: sPumpkins(t); break;                                 // Halloween
    case 27: sLanterns(t); break;                                 // Sankt Martin
    case 28: sBoot(t); break;                                     // Nikolaus
    case 29: case 30: case 31: case 32: sWreath(t, id - 28); break;   // Advent
    case 33: sTree(t, true); break;                               // Heiligabend
    case 34: sFireworks(t, dt, true); break;                      // Silvester
    case 35: sSchoolBag(t); break;                                // Ferienbeginn
    default: sSchool(t); break;                                   // Letzter Schultag
  }
  // Uhrzeit oben rechts, Name des Tages unten
  if (c.timeOk) {
    char b[8];
    snprintf(b, sizeof(b), "%02d:%02d", c.hh, c.mm);
    fb->fillRoundRect(FB_W - 34, 1, 33, 11, 3, P_BAND);
    txt(FB_W - 32, 3, b, P_BANDT, 1);
  }
  char nm[40];
  cal::ascii(nm, cal::name(id), sizeof(nm));
  for (char* p = nm; *p; p++) if (*p >= 'a' && *p <= 'z') *p -= 32;
  fb->fillRect(0, FB_H - 11, FB_W, 11, P_BAND);
  txtc(FB_W / 2, FB_H - 9, nm, P_BANDT, 1);
}

// Uebergang: Konfetti-Vorhang von oben
static void cover(float p, bool in, uint32_t now) {
  (void)in; (void)now;
  const int h = (int)(p * FB_H);
  fb->fillRect(0, 0, FB_W, h, P_BAND);
  static const uint8_t cc[5] = {P_RED, P_YEL, P_CYAN, P_MAGENTA, P_GREEN};
  for (int i = 0; i < 40; i++) {
    const int x = (i * 37) % FB_W, y = h - (i * 13) % 20;
    if (y >= 0 && y < FB_H) fb->fillRect(x, y, 3, 2, cc[i % 5]);
  }
}

}  // namespace f_party
