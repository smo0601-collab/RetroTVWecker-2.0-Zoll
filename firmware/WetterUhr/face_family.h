#pragma once
/*
  face_family.h - Zifferblatt "Familie"
  Fuenf gezeichnete Figuren stehen auf einer Wiese: Vater (blond, mit Bart), Mutter (braune Haare)
  und die drei Toechter mit 20, 18 und 10 Jahren in drei verschiedenen Blondtoenen. Alle atmen,
  blinzeln und winken abwechselnd; die Juengste huepft ab und zu. Zur vollen Minute steigen Herzen auf.
  Uhrzeit oben auf einem Schild, Datum und Temperatur im Gras.
  Uebergang: Ein Vorhang aus Blumen waechst von unten.
*/
#include "faces_fx.h"

namespace f_family {
using namespace fc;

enum : uint8_t { Y_SKY1 = 1, Y_SKY2, Y_SKY3, Y_CLOUD, Y_GRASS, Y_GRASS2, Y_HILL, Y_SUN, Y_SKIN, Y_SKIN2, Y_EYE, Y_MOUTH, Y_CHEEK,
                 Y_HV, Y_HV2, Y_HM, Y_HM2, Y_H20, Y_H202, Y_H18, Y_H182, Y_H10, Y_H102,
                 Y_BLUE, Y_BLUE2, Y_RED, Y_RED2, Y_TEAL, Y_TEAL2, Y_YEL, Y_YEL2, Y_PINK, Y_PINK2,
                 Y_BEARD, Y_BEARD2, Y_JEANS, Y_JEANS2, Y_SHOE, Y_DIG, Y_SH, Y_HEART, Y_TREE, Y_TREE2, Y_TRUNK,
                 Y_FLOWER, Y_WALL, Y_WALL2, Y_ROOF, Y_ROOF2, Y_DOOR, Y_WIN };

static uint32_t lastT, heartAt;
static uint8_t  lastMm;

static void enter(uint32_t now) {
  pal(Y_SKY1, 112, 184, 244); pal(Y_SKY2, 150, 206, 250); pal(Y_SKY3, 196, 232, 252); pal(Y_CLOUD, 255, 255, 255);
  pal(Y_GRASS, 104, 178, 82); pal(Y_GRASS2, 84, 152, 66); pal(Y_HILL, 130, 196, 112); pal(Y_SUN, 255, 232, 120);
  pal(Y_SKIN, 255, 214, 178); pal(Y_SKIN2, 234, 176, 140); pal(Y_EYE, 44, 34, 40); pal(Y_MOUTH, 196, 84, 90); pal(Y_CHEEK, 250, 170, 160);
  pal(Y_HV, 224, 186, 100);  pal(Y_HV2, 244, 216, 140);          // Vater: dunkelblond
  pal(Y_HM, 104, 64, 38);    pal(Y_HM2, 138, 92, 58);            // Mutter: braun
  pal(Y_H20, 226, 182, 100); pal(Y_H202, 246, 214, 140);         // Tochter 20: honigblond
  pal(Y_H18, 244, 226, 156); pal(Y_H182, 255, 246, 200);         // Tochter 18: hellblond
  pal(Y_H10, 200, 158, 88);  pal(Y_H102, 224, 190, 124);         // Tochter 10: dunkelblond
  pal(Y_BLUE, 60, 106, 196); pal(Y_BLUE2, 42, 76, 150); pal(Y_RED, 206, 64, 76); pal(Y_RED2, 156, 42, 58);
  pal(Y_TEAL, 48, 176, 176); pal(Y_TEAL2, 30, 130, 136); pal(Y_YEL, 246, 206, 66); pal(Y_YEL2, 206, 162, 40);
  pal(Y_PINK, 246, 136, 176); pal(Y_PINK2, 206, 96, 140);
  pal(Y_BEARD, 168, 170, 178); pal(Y_BEARD2, 214, 216, 224);     // grauer Bart
  pal(Y_JEANS, 66, 88, 140); pal(Y_JEANS2, 48, 66, 112); pal(Y_SHOE, 70, 54, 48);
  pal(Y_DIG, 255, 252, 244); pal(Y_SH, 50, 70, 110); pal(Y_HEART, 250, 70, 110);
  pal(Y_TREE, 66, 150, 78); pal(Y_TREE2, 90, 176, 96); pal(Y_TRUNK, 112, 82, 52); pal(Y_FLOWER, 255, 170, 200);
  pal(Y_WALL, 240, 230, 210); pal(Y_WALL2, 212, 200, 176); pal(Y_ROOF, 196, 84, 72); pal(Y_ROOF2, 160, 62, 56);
  pal(Y_DOOR, 120, 82, 56); pal(Y_WIN, 170, 214, 240);
  lastT = now; heartAt = 0; lastMm = 255;
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, Y_SKY3); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, Y_SH); }

static void limb(int x0, int y0, int x1, int y1, uint8_t col) {
  fb->drawLine(x0, y0, x1, y1, col);
  fb->drawLine(x0 + 1, y0, x1 + 1, y1, col);
}

// Figur: hair/top = Palettenindex der Hauptfarbe (+1 = Licht bzw. Schatten)
// style: 0 kurz, 1 Bob/schulterlang, 2 lang offen, 3 Pferdeschwanz, 4 Zoepfe
static void person(int cx, int foot, int h, int r, uint8_t hair, uint8_t top, uint8_t pants, int style, bool dress,
                   bool beard, uint32_t now, int ph, bool wave, int hop) {
  const int bob = ((now / 800 + (uint32_t)ph) & 1) ? 0 : 1;                      // Atmen
  foot -= hop;
  const int hy = foot - h + r + bob;                                             // Kopfmitte
  const int sy = hy + r + 2;                                                     // Schulterlinie
  const int hip = foot - h * 38 / 100;                                           // Huefte
  const int sw = r + 3 + (beard ? 1 : 0), hw = sw - 2;                           // Schulter-/Taillenbreite
  const uint8_t top2 = top + 1, hair2 = hair + 1;

  // Haare hinten
  if (style == 1 || style == 2) {
    fb->fillCircle(cx, hy, r + 1, hair);
    fb->fillRect(cx - r - 1, hy, 2 * r + 3, (style == 2 ? r * 2 + 3 : r + 3), hair);
    fb->fillCircle(cx - r, hy + (style == 2 ? r * 2 + 3 : r + 3), 1, hair);
    fb->fillCircle(cx + r, hy + (style == 2 ? r * 2 + 3 : r + 3), 1, hair);
  } else if (style == 3) {
    fb->fillCircle(cx + r + 1, hy + 2, 2, hair);
    limb(cx + r, hy + 3, cx + r + 2, hy + r + 6, hair);
    fb->fillCircle(cx + r + 3, hy + r + 7, 1, hair2);
  } else if (style == 4) {
    for (int k = -1; k <= 1; k += 2) {
      fb->fillCircle(cx + k * (r + 2), hy + 3, 2, hair);
      fb->fillCircle(cx + k * (r + 3), hy + r + 3, 2, hair);
      fb->drawPixel(cx + k * (r + 2), hy + 2, Y_PINK);
    }
  }

  // Beine bzw. Kleid
  if (dress) {
    const int knee = foot - h * 17 / 100;
    limb(cx - 2, knee, cx - 2, foot - 2, Y_SKIN2); limb(cx + 1, knee, cx + 1, foot - 2, Y_SKIN2);
    fb->fillTriangle(cx - hw, hip - 3, cx + hw, hip - 3, cx - hw - 6, knee, top);
    fb->fillTriangle(cx + hw, hip - 3, cx + hw + 6, knee, cx - hw - 6, knee, top);
    fb->drawLine(cx - hw - 6, knee, cx + hw + 6, knee, top2);
    fb->drawLine(cx + 1, hip - 2, cx + hw + 4, knee - 1, top2);
  } else {
    fb->fillTriangle(cx - hw, hip - 2, cx, hip - 2, cx - hw / 2 - 1, foot - 3, pants);
    fb->fillTriangle(cx + hw, hip - 2, cx, hip - 2, cx + hw / 2 + 1, foot - 3, pants);
    fb->fillRect(cx - hw / 2 - 2, foot - 4, 3, 2, pants); fb->fillRect(cx + hw / 2, foot - 4, 3, 2, pants);
    fb->drawFastVLine(cx, hip, 4, Y_JEANS2);
  }
  fb->fillRect(cx - hw / 2 - 3, foot - 2, 5, 2, Y_SHOE); fb->fillRect(cx + hw / 2 - 1, foot - 2, 5, 2, Y_SHOE);

  // Oberkoerper
  fb->fillTriangle(cx - sw, sy, cx + sw, sy, cx - hw, hip, top);
  fb->fillTriangle(cx + sw, sy, cx + hw, hip, cx - hw, hip, top);
  fb->fillCircle(cx - sw + 1, sy + 1, 2, top); fb->fillCircle(cx + sw - 1, sy + 1, 2, top);
  fb->drawLine(cx + sw - 3, sy + 2, cx + hw - 1, hip - 1, top2);                  // Schatten rechts
  if (!dress) fb->drawFastHLine(cx - hw, hip - 1, 2 * hw + 1, Y_JEANS2);          // Guertel

  // Arme
  {
    const int el = cx - sw - 2, er = cx + sw + 2, ey = sy + (hip - sy) / 2;
    limb(cx - sw, sy + 1, el, ey, top); limb(el, ey, el + 1, hip + 1, Y_SKIN);
    fb->fillCircle(el + 1, hip + 2, 1, Y_SKIN);
    if (wave) {
      const int w = (int)((now / 160) & 1) * 2;
      limb(cx + sw, sy + 1, er + 2, ey - 2, top);
      limb(er + 2, ey - 2, er + 3 + w, sy - 7, Y_SKIN);
      fb->fillCircle(er + 3 + w, sy - 8, 1, Y_SKIN);
    } else {
      limb(cx + sw, sy + 1, er, ey, top); limb(er, ey, er - 1, hip + 1, Y_SKIN);
      fb->fillCircle(er, hip + 2, 1, Y_SKIN);
    }
  }

  // Hals und Kopf
  fb->fillRect(cx - 1, hy + r - 1, 3, 4, Y_SKIN2);
  fb->fillCircle(cx, hy, r, Y_SKIN);
  fb->drawPixel(cx - r, hy + 1, Y_SKIN2); fb->drawPixel(cx + r, hy + 1, Y_SKIN2);   // Ohren

  // Haare vorn: Haube mit rundem Haaransatz
  fb->fillCircle(cx, hy - 1, r, hair);
  fb->fillCircle(cx, hy + 2, r - 1, Y_SKIN);
  if (style != 0) fb->fillTriangle(cx - r, hy + 1, cx, hy - r, cx + r - 2, hy - 2, hair);   // Seitenscheitel
  fb->drawLine(cx - r / 2, hy - r + 1, cx + r / 2 - 1, hy - r + 1, hair2);

  // Gesicht
  const int ex = r / 2 + 1;
  const bool blink = ((now / 100 + (uint32_t)ph * 13) % 40) == 0;
  if (blink) { fb->drawFastHLine(cx - ex - 1, hy + 2, 2, Y_EYE); fb->drawFastHLine(cx + ex, hy + 2, 2, Y_EYE); }
  else { fb->drawFastVLine(cx - ex, hy + 1, 2, Y_EYE); fb->drawFastVLine(cx + ex, hy + 1, 2, Y_EYE); }
  if (beard) {                                                                      // grauer Vollbart
    fb->fillTriangle(cx - r, hy + 3, cx + r, hy + 3, cx, hy + r + 5, Y_BEARD);
    fb->fillCircle(cx - r / 2, hy + r - 1, r / 2 + 1, Y_BEARD);
    fb->fillCircle(cx + r / 2, hy + r - 1, r / 2 + 1, Y_BEARD);
    fb->fillRect(cx - 3, hy + 4, 7, 1, Y_BEARD2);                                   // Schnurrbart
    fb->drawFastHLine(cx - 1, hy + 6, 3, Y_MOUTH);
    fb->drawLine(cx - r + 1, hy + 4, cx - 2, hy + r + 3, Y_BEARD2);
  } else {
    fb->drawPixel(cx - ex - 1, hy + 3, Y_CHEEK); fb->drawPixel(cx + ex + 1, hy + 3, Y_CHEEK);
    fb->drawPixel(cx - 2, hy + r - 2, Y_MOUTH); fb->drawFastHLine(cx - 1, hy + r - 1, 3, Y_MOUTH); fb->drawPixel(cx + 2, hy + r - 2, Y_MOUTH);
  }
}

static void heart(int x, int y, uint8_t col) {
  static const uint8_t M[5] = {0x0A, 0x1F, 0x1F, 0x0E, 0x04};
  for (int r = 0; r < 5; r++) for (int c = 0; c < 5; c++) if (M[r] & (0x10 >> c)) fb->drawPixel(x + c, y + r, col);
}

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  frameDt(now, lastT);
  if (c.timeOk) {
    if (lastMm != 255 && c.mm != lastMm) heartAt = now;
    lastMm = c.mm;
  }

  fb->fillRect(0, 0, FB_W, 34, Y_SKY1);
  fb->fillRect(0, 34, FB_W, 30, Y_SKY2);
  fb->fillRect(0, 64, FB_W, 40, Y_SKY3);
  fb->fillCircle(146, 14, 8, Y_SUN); fb->drawCircle(146, 14, 10, Y_SKY2);
  for (int k = 0; k < 2; k++) {                                                       // Wolken
    const int cx = (int)(fmodf((float)(now % 90000) / 90000.0f * 220.0f + (float)k * 110.0f, 220.0f)) - 30;
    const int cy = 36 + k * 8;
    fb->fillCircle(cx, cy, 5, Y_CLOUD); fb->fillCircle(cx + 7, cy - 2, 6, Y_CLOUD); fb->fillCircle(cx + 14, cy, 5, Y_CLOUD);
    fb->fillRect(cx, cy, 15, 5, Y_CLOUD);
  }
  fb->fillCircle(40, 128, 50, Y_HILL); fb->fillCircle(130, 130, 54, Y_HILL);          // Huegel
  fb->fillRect(0, 96, FB_W, 24, Y_GRASS);
  for (int x = 0; x < FB_W; x += 6) fb->drawPixel(x + (x * 7) % 5, 98 + (x * 3) % 20, Y_GRASS2);
  for (int x = 4; x < FB_W; x += 17) { fb->drawPixel(x, 112 + (x % 5), Y_FLOWER); fb->drawPixel(x + 1, 112 + (x % 5), Y_YEL); }
  // Haus links
  fb->fillRect(2, 62, 28, 36, Y_WALL); fb->fillRect(22, 62, 8, 36, Y_WALL2);
  fb->fillTriangle(-2, 62, 34, 62, 16, 44, Y_ROOF); fb->fillTriangle(16, 44, 34, 62, 24, 62, Y_ROOF2);
  fb->fillRect(22, 46, 4, 8, Y_ROOF2);
  fb->fillRect(11, 80, 8, 18, Y_DOOR); fb->drawPixel(17, 90, Y_SUN);
  fb->fillRect(5, 68, 7, 7, Y_WIN); fb->drawFastVLine(8, 68, 7, Y_WALL); fb->drawFastHLine(5, 71, 7, Y_WALL);
  fb->fillRect(20, 68, 7, 7, Y_WIN); fb->drawFastVLine(23, 68, 7, Y_WALL); fb->drawFastHLine(20, 71, 7, Y_WALL);
  // Baum rechts
  fb->fillRect(150, 72, 4, 26, Y_TRUNK);
  fb->fillCircle(152, 62, 11, Y_TREE); fb->fillCircle(145, 70, 8, Y_TREE); fb->fillCircle(158, 70, 7, Y_TREE);
  fb->fillCircle(149, 58, 4, Y_TREE2); fb->fillCircle(156, 66, 3, Y_TREE2);

  // Wer winkt? alle 3 s ein anderer; Juengste huepft alle 7 s
  const int waver = (int)((now / 3000) % 5);
  const int hopPhase = (int)(now % 7000);
  const int hop = hopPhase < 400 ? (int)(sinf((float)hopPhase / 400.0f * 3.1416f) * 6.0f) : 0;
  const int foot = 108;
  person(42, foot, 64, 7, Y_HV,  Y_BLUE, Y_JEANS, 0, false, true,  now, 0, waver == 0, 0);   // Vater (blond, grauer Bart)
  person(66, foot, 58, 6, Y_HM,  Y_RED,  Y_JEANS, 1, true,  false, now, 1, waver == 1, 0);   // Mutter (braun)
  person(88, foot, 56, 6, Y_H20, Y_TEAL, Y_JEANS, 2, false, false, now, 2, waver == 2, 0);   // Tochter 20 (honigblond)
  person(109, foot, 54, 6, Y_H18, Y_YEL, Y_JEANS, 3, false, false, now, 3, waver == 3, 0);   // Tochter 18 (hellblond)
  person(129, foot, 40, 6, Y_H10, Y_PINK, Y_JEANS, 4, true, false, now, 4, waver == 4, hop); // Tochter 10 (dunkelblond)

  // Herzen zur vollen Minute
  if (heartAt && now - heartAt < 2500) {
    for (int i = 0; i < 5; i++) {
      const float a = secs(now, heartAt) - (float)i * 0.15f;
      if (a < 0) continue;
      heart(34 + i * 22 + (int)(sinf(a * 4.0f + (float)i) * 3.0f), 56 - (int)(a * 16.0f), Y_HEART);
    }
  }

  // Schild mit Zeit
  fb->fillRoundRect(36, 2, 88, 24, 4, Y_TRUNK); fb->fillRoundRect(37, 3, 86, 22, 3, Y_WALL);
  drawTime(40, 4, 3, c, cellD);
  fb->fillRect(62, 26, 3, 5, Y_TRUNK); fb->fillRect(95, 26, 3, 5, Y_TRUNK);

  if (c.timeOk) {
    char b[16];
    snprintf(b, sizeof(b), "%s %02u.%02u.", WD_SHORT[c.wday % 7], c.mday, c.mon);
    txt(4, 111, b, Y_DIG);
  }
  if (c.hasWx) tempTxt(FB_W - 4 - tempW(c.temp, 1), 111, c.temp, Y_DIG);
  if (c.showSec && c.timeOk) for (int i = 0; i <= c.ss; i += 2) fb->drawPixel(40 + i * 80 / 60, 119, Y_FLOWER);
}

static void cover(float p, bool, uint32_t) {
  if (p <= 0.0f) return;
  uint8_t* bf = buf();
  if (p >= 0.97f) { memset(bf, Y_GRASS, FB_W * FB_H); return; }
  for (int x = 0; x < FB_W; x++) {
    const float th = (float)((x * 5) % 13) / 13.0f * 0.4f;
    if (p < th) continue;
    const int h = (int)(fminf(1.0f, (p - th) / 0.55f) * (float)(FB_H + 4));
    for (int y = FB_H - 1; y >= FB_H - h && y >= 0; y--) bf[y * FB_W + x] = (y == FB_H - h && (x & 3) == 0) ? Y_FLOWER : Y_GRASS;
  }
}

}  // namespace f_family
