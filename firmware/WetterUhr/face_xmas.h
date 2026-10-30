#pragma once
/*
  face_xmas.h - Zifferblatt "Weihnachten" mit Countdown
  Verschneite Winternacht: links ein geschmueckter Tannenbaum mit blinkenden Lichtern, Stern und
  Geschenken, es schneit. Rechts der Countdown bis Heiligabend (Tage und Stunden:Minuten:Sekunden
  bis zur Bescherung um 18 Uhr). Am Heiligabend ab 18 Uhr und an beiden Feiertagen steht dort
  "FROHE WEIHNACHTEN". Vier Adventskerzen zeigen, wie viele Adventssonntage schon waren.
  Zur vollen Minute laufen die Lichter am Baum als Lauflicht, der Stern funkelt.
  Uebergang: Schneegestoeber.
*/
#include "faces_fx.h"

namespace f_xmas {
using namespace fc;

enum : uint8_t { X_SKY1 = 1, X_SKY2, X_SKY3, X_ST1, X_ST2, X_SNOW, X_SNOW2, X_TREE1, X_TREE2, X_TREE3, X_TRUNK,
                 X_L1, X_L2, X_L3, X_L4, X_LOFF, X_STAR, X_STAR2, X_G1, X_G2, X_G3, X_RIB, X_RIB2, X_DIG, X_SH,
                 X_TXT, X_DIM, X_GOLD, X_CAND, X_CAND2, X_FL1, X_FL2, X_WR, X_WR2, X_GTXT, X_RED };

struct Flake { float x, y, v, ph; uint8_t big; };
static Flake    fl[40];
static uint32_t lastT, chaseUntil;
static uint8_t  lastMm;

static void enter(uint32_t now) {
  pal(X_SKY1, 6, 10, 34); pal(X_SKY2, 14, 24, 62); pal(X_SKY3, 30, 44, 96);
  pal(X_ST1, 255, 250, 220); pal(X_ST2, 120, 134, 190);
  pal(X_SNOW, 236, 242, 255); pal(X_SNOW2, 170, 190, 230);
  pal(X_TREE1, 10, 72, 40); pal(X_TREE2, 22, 120, 60); pal(X_TREE3, 60, 170, 90); pal(X_TRUNK, 110, 66, 34);
  pal(X_L1, 255, 60, 60); pal(X_L2, 255, 220, 70); pal(X_L3, 80, 170, 255); pal(X_L4, 255, 110, 220); pal(X_LOFF, 40, 60, 50);
  pal(X_STAR, 255, 226, 80); pal(X_STAR2, 255, 255, 200);
  pal(X_G1, 210, 40, 50); pal(X_G2, 50, 90, 210); pal(X_G3, 40, 150, 80); pal(X_RIB, 255, 210, 70); pal(X_RIB2, 245, 245, 245);
  pal(X_DIG, 255, 248, 232); pal(X_SH, 20, 10, 40); pal(X_TXT, 230, 236, 255); pal(X_DIM, 150, 166, 214);
  pal(X_GOLD, 255, 206, 90); pal(X_CAND, 200, 30, 40); pal(X_CAND2, 130, 16, 24); pal(X_FL1, 255, 240, 140); pal(X_FL2, 255, 150, 40);
  pal(X_WR, 20, 96, 50); pal(X_WR2, 12, 60, 32); pal(X_GTXT, 30, 46, 110); pal(X_RED, 255, 90, 90);
  for (int i = 0; i < 40; i++) fl[i] = {(float)rn(FB_W), (float)rn(FB_H), 5.0f + rf() * 9.0f, rf() * 6.28f, (uint8_t)(rn(6) == 0)};
  lastT = now; chaseUntil = 0; lastMm = 255;
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, X_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, X_DIG); }

// Baumschicht: dunkle Kontur, helle Flaeche, Schneespitzen an der Unterkante
static void tier(int cx, int top, int half, int bot) {
  fb->fillTriangle(cx, top, cx - half, bot, cx + half, bot, X_TREE1);
  fb->fillTriangle(cx + 1, top + 4, cx - half + 5, bot - 2, cx + half - 2, bot - 2, X_TREE2);
  fb->fillTriangle(cx + 3, top + 8, cx + 4, bot - 3, cx + half - 6, bot - 3, X_TREE3);
  for (int x = cx - half + 1; x < cx + half; x += 4) fb->drawFastHLine(x, bot, 2, X_SNOW);
}

static void gift(int x, int y, int w, int h, uint8_t col, uint8_t rib) {
  fb->fillRect(x, y, w, h, col);
  fb->drawFastVLine(x + w / 2, y, h, rib);
  fb->drawFastHLine(x, y + h / 2, w, rib);
  fb->drawPixel(x + w / 2 - 1, y - 1, rib); fb->drawPixel(x + w / 2 + 1, y - 1, rib);
  fb->drawPixel(x + w / 2 - 2, y - 2, rib); fb->drawPixel(x + w / 2 + 2, y - 2, rib);
}

static const int8_t LX[18] = {24, 29, 20, 31, 26, 15, 35, 22, 30, 11, 39, 19, 27, 34, 8, 42, 17, 31};
static const int8_t LY[18] = {26, 31, 37, 38, 44, 50, 52, 56, 59, 69, 70, 72, 76, 66, 80, 81, 63, 46};

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) chaseUntil = now + 4500;
    lastMm = c.mm;
  }
  const bool chase = (int32_t)(chaseUntil - now) > 0;

  // ---- Himmel und Sterne ----
  fb->fillRect(0, 0, FB_W, 40, X_SKY1);
  fb->fillRect(0, 40, FB_W, 36, X_SKY2);
  fb->fillRect(0, 76, FB_W, 24, X_SKY3);
  for (int i = 0; i < 30; i++) {
    const uint32_t h = (uint32_t)i * 2654435761u + 99u;
    const int x = (int)((h >> 8) % 158) + 1, y = (int)((h >> 20) % 74) + 1;
    const uint8_t m = ((now / (420 + (h & 255))) + (uint32_t)i) % 5;
    if (m == 4) continue;
    fb->drawPixel(x, y, m == 0 ? X_ST1 : X_ST2);
  }

  // ---- Schnee am Boden ----
  fb->fillRect(0, 98, FB_W, 22, X_SNOW);
  fb->fillCircle(70, 104, 12, X_SNOW); fb->fillCircle(130, 106, 16, X_SNOW); fb->fillCircle(4, 102, 10, X_SNOW);
  fb->drawFastHLine(56, 116, 30, X_SNOW2); fb->drawFastHLine(112, 118, 40, X_SNOW2); fb->drawFastHLine(0, 112, 20, X_SNOW2);

  // ---- Tannenbaum ----
  fb->fillRect(23, 84, 7, 14, X_TRUNK);
  tier(26, 47, 23, 86);
  tier(26, 30, 18, 63);
  tier(26, 18, 12, 41);
  for (int k = 0; k < 3; k++) {                                   // Girlanden
    const int y0 = 34 + k * 20;
    for (int x = 0; x < 20 + k * 8; x += 2) fb->drawPixel(14 - k * 4 + x, y0 + x / 4, X_GOLD);
  }
  for (int i = 0; i < 18; i++) {                                  // Lichter
    uint8_t col;
    if (chase) col = ((i + now / 90) % 4 == 0) ? X_L2 : X_LOFF;
    else {
      const bool on = ((now / 450 + (uint32_t)i * 3) % 7) != 0;
      col = on ? (uint8_t)(X_L1 + i % 4) : X_LOFF;
    }
    fb->drawPixel(LX[i], LY[i], col);
    if (col != X_LOFF) fb->drawPixel(LX[i], LY[i] + 1, col);
  }
  // Stern
  const bool twinkle = chase || ((now / 300) % 9) == 0;
  fb->fillRect(24, 12, 5, 1, X_STAR); fb->fillRect(25, 11, 3, 3, X_STAR); fb->drawPixel(26, 9, X_STAR);
  fb->drawPixel(26, 10, X_STAR); fb->drawPixel(25, 14, X_STAR); fb->drawPixel(27, 14, X_STAR);
  fb->drawPixel(26, 12, X_STAR2);
  if (twinkle) {
    fb->drawFastHLine(20, 12, 3, X_STAR2); fb->drawFastHLine(30, 12, 3, X_STAR2);
    fb->drawFastVLine(26, 5, 3, X_STAR2); fb->drawPixel(22, 8, X_STAR2); fb->drawPixel(30, 8, X_STAR2);
  }
  // Geschenke
  gift(4, 89, 11, 9, X_G1, X_RIB);
  gift(33, 91, 13, 7, X_G2, X_RIB2);
  gift(15, 93, 9, 5, X_G3, X_L1);

  // ---- Adventskranz mit vier Kerzen ----
  int lit = 0;
  if (c.timeOk) {
    const int doy = dayOfYear(c.year, c.mon, c.mday);
    const int adv4 = dayOfYear(c.year, 12, 24) - wdayOf(c.year, 12, 24);
    const int adv1 = adv4 - 21;
    if (doy >= adv1 && doy <= dayOfYear(c.year, 12, 26))
      for (int k = 0; k < 4; k++) if (adv1 + 7 * k <= doy) lit++;
  }
  fb->fillRoundRect(104, 93, 50, 6, 3, X_WR2);
  fb->fillRoundRect(105, 92, 48, 4, 2, X_WR);
  for (int x = 107; x < 152; x += 5) fb->drawPixel(x, 92, X_TREE3);
  for (int k = 0; k < 4; k++) {
    const int x = 109 + k * 12;
    fb->fillRect(x, 82, 5, 11, X_CAND);
    fb->drawFastVLine(x + 4, 82, 11, X_CAND2);
    fb->drawPixel(x + 2, 81, X_SH);
    if (k < lit) {
      const int f = (int)((now / 120 + (uint32_t)k * 5) % 3);
      fb->fillRect(x + 1, 77 + (f == 2 ? 1 : 0), 3, 4 - (f == 2 ? 1 : 0), X_FL2);
      fb->fillRect(x + 2, 76 + f % 2, 1, 4, X_FL1);
    }
  }
  fb->drawPixel(108, 94, X_L1); fb->drawPixel(127, 95, X_L1); fb->drawPixel(146, 94, X_L1);

  // ---- Schneefall ----
  for (int i = 0; i < 40; i++) {
    Flake& f = fl[i];
    f.y += f.v * dt; f.ph += dt * 1.3f;
    const float x = f.x + sinf(f.ph) * 3.0f;
    if (f.y > FB_H) { f.y = -2; f.x = (float)rn(FB_W); }
    if (f.big) fb->fillRect((int)x, (int)f.y, 2, 2, X_SNOW);
    else fb->drawPixel((int)x, (int)f.y, (f.y > 96) ? X_SNOW2 : X_SNOW);
  }

  // ---- Uhrzeit ----
  drawTime(49, 4, 4, c, cellS);
  drawTime(49, 4, 4, c, cellD);

  // ---- Countdown ----
  const int cx = 104;
  char b[24];
  if (c.timeOk && c.unixT > 0) {
    bool fest = false;
    int ty = c.year;
    if (c.mon == 12 && c.mday >= 24) {
      const long s24 = secsUntilLocal(c.unixT, c.year, 12, 24, 18);
      if (s24 <= 0 && c.mday <= 26) fest = true;
      else if (s24 <= 0) ty = c.year + 1;
    }
    if (fest) {
      const uint8_t a = ((now / 600) & 1) ? X_GOLD : X_RED;
      txtS(cx - txtw("FROHE", 3) / 2, 40, "FROHE", a, X_SH, 3);
      txtS(cx - txtw("WEIHNACHTEN") / 2, 66, "WEIHNACHTEN", X_TXT, X_SH);
    } else {
      long s = secsUntilLocal(c.unixT, ty, 12, 24, 18);
      if (s < 0) s = 0;
      const long days = s / 86400;
      const int hh = (int)((s % 86400) / 3600), mm = (int)((s % 3600) / 60), ss = (int)(s % 60);
      if (days >= 1) {
        txtS(cx - txtw("BIS HEILIGABEND") / 2, 37, "BIS HEILIGABEND", X_GOLD, X_SH);
        snprintf(b, sizeof(b), "%ld", days);
        const char* unit = days == 1 ? "TAG" : "TAGE";
        const int w = txtw(b, 3) + 6 + txtw(unit, 2), x0 = cx - w / 2;
        txtS(x0, 47, b, X_DIG, X_SH, 3);
        txtS(x0 + txtw(b, 3) + 6, 54, unit, X_TXT, X_SH, 2);
        snprintf(b, sizeof(b), "+ %02d:%02d:%02d", hh, mm, ss);
        txtS(cx - txtw(b) / 2, 71, b, X_DIM, X_SH);
      } else {
        txtS(cx - txtw("HEUTE IST") / 2, 37, "HEUTE IST", X_GOLD, X_SH);
        txtS(cx - txtw("HEILIGABEND") / 2, 46, "HEILIGABEND", X_RED, X_SH);
        snprintf(b, sizeof(b), "%02d:%02d:%02d", hh, mm, ss);
        txtS(cx - txtw(b, 2) / 2, 56, b, X_DIG, X_SH, 2);
        txtS(cx - txtw("BIS ZUR BESCHERUNG") / 2, 72, "BIS ZUR BESCHERUNG", X_DIM, X_SH);
      }
    }
    // Datum links unten, Temperatur rechts unten (auf dem Schnee)
    snprintf(b, sizeof(b), "%s %02u.%02u.", WD_SHORT[c.wday % 7], c.mday, c.mon);
    txt(52, 108, b, X_GTXT);
  } else {
    txtS(cx - txtw("WARTE AUF ZEIT") / 2, 50, "WARTE AUF ZEIT", X_DIM, X_SH);
  }
  if (c.hasWx) tempTxt(FB_W - 4 - tempW(c.temp, 1), 108, c.temp, X_GTXT);

  // Sekunden als kleine Schneeflocken-Leiste unter der Uhr
  if (c.showSec && c.timeOk) fb->drawFastHLine(49, 33, (c.ss + 1) * 108 / 60, X_DIM);
}

static void cover(float p, bool, uint32_t now) {
  if (p <= 0.0f) return;
  uint8_t* bf = buf();
  const int th = (int)(p * 17.0f);
  for (int y = 0; y < FB_H; y++)
    for (int x = 0; x < FB_W; x++) {
      const uint32_t h = (uint32_t)(x * 73856093u ^ y * 19349663u ^ (now / 60) * 83492791u);
      if ((int)(h % 16) < th) bf[y * FB_W + x] = (h & 64) ? X_SNOW : X_SKY2;
    }
}

}  // namespace f_xmas
