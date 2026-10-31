#pragma once
/*
  face_blocks.h - Zifferblatt "Blockwelt"
  Eine Welt aus Wuerfeln scrollt vorbei: Gras, Erde, Stein, Baeume aus Bloecken, Wolken. Eine
  kleine, selbst erfundene Blockfigur wandert, springt ueber Huegel und winkt ab und zu.
  Die Tageszeit folgt der echten Uhr (Sonne am Tag, Mond und Sterne nachts).
  Die Uhrzeit steht oben in Blockziffern, unten laeuft eine Leiste mit den Sekunden.
  Uebergang: Ein Vorhang aus Bloecken faellt von oben.
*/
#include "faces_fx.h"

namespace f_voxel {
using namespace fc;

enum : uint8_t { B_SKYD = 1, B_SKYN, B_SKYE, B_GRASS, B_GRASS2, B_DIRT, B_DIRT2, B_STONE, B_STONE2, B_WOOD, B_LEAF,
                 B_LEAF2, B_CLOUD, B_SUN, B_MOON, B_SK, B_SHIRT, B_PANTS, B_HAIR, B_DIG, B_SH, B_STAR };

static uint32_t lastT, t0;
static float    wx;                                   // Weltposition (Bloecke)

static int heightAt(int bx) {                          // Terrainhoehe in Bloecken, deterministisch
  const uint32_t h = (uint32_t)bx * 73856093u ^ 0x5bd1e995u;
  const int base = 3 + (int)((sinf((float)bx * 0.23f) + 1.0f) * 1.2f);
  return base + (((h >> 7) & 7) == 0 ? 1 : 0);
}
static bool treeAt(int bx) { return (((uint32_t)bx * 2654435761u) >> 11) % 7 == 0; }

static void enter(uint32_t now) {
  pal(B_SKYD, 110, 190, 255); pal(B_SKYN, 14, 18, 52); pal(B_SKYE, 240, 140, 90);
  pal(B_GRASS, 90, 190, 70); pal(B_GRASS2, 66, 150, 52); pal(B_DIRT, 134, 96, 60); pal(B_DIRT2, 104, 72, 44);
  pal(B_STONE, 130, 130, 140); pal(B_STONE2, 100, 100, 112); pal(B_WOOD, 110, 76, 44); pal(B_LEAF, 54, 140, 54);
  pal(B_LEAF2, 36, 104, 40); pal(B_CLOUD, 245, 248, 255); pal(B_SUN, 255, 232, 110); pal(B_MOON, 240, 240, 220);
  pal(B_SK, 232, 190, 150); pal(B_SHIRT, 40, 170, 190); pal(B_PANTS, 60, 60, 150); pal(B_HAIR, 90, 56, 30);
  pal(B_DIG, 255, 255, 255); pal(B_SH, 20, 30, 50); pal(B_STAR, 255, 250, 210);
  lastT = now; t0 = now; wx = 40.0f;
}

static void cellS(int x, int y, int s, int, int) { fb->fillRect(x + 1, y + 1, s, s, B_SH); }
static void cellD(int x, int y, int s, int, int) { fb->fillRect(x, y, s, s, B_DIG); }

static void draw(const FaceCtx& c) {
  const uint32_t now = c.now;
  const float dt = frameDt(now, lastT);
  wx += 2.4f * dt;                                      // Bloecke pro Sekunde
  const int S = 8;                                      // Blockgroesse in Pixeln
  const float hour = c.timeOk ? (float)c.hh + (float)c.mm / 60.0f : 12.0f;
  const bool night = hour < 6.0f || hour >= 20.0f;
  const bool eve = (hour >= 18.0f && hour < 20.0f) || (hour >= 6.0f && hour < 7.5f);
  fb->fillScreen(night ? B_SKYN : (eve ? B_SKYE : B_SKYD));

  // Himmelskoerper
  const float f = night ? ((hour >= 20.0f ? hour - 20.0f : hour + 4.0f) / 10.0f) : ((hour - 6.0f) / 14.0f);
  const int bx = 10 + (int)(f * 140.0f), by = 56 - (int)(sinf(f * 3.1416f) * 40.0f);
  if (night) {
    for (int i = 0; i < 18; i++) {
      const uint32_t h = (uint32_t)i * 2654435761u + 99u;
      if (((now / (400 + (h & 255))) + (uint32_t)i) % 3) fb->drawPixel((int)((h >> 8) % 158) + 1, (int)((h >> 20) % 56) + 1, B_STAR);
    }
    fb->fillRect(bx - 4, by - 4, 8, 8, B_MOON); fb->fillRect(bx - 1, by - 2, 2, 2, B_SKYN);
  } else fb->fillRect(bx - 5, by - 5, 10, 10, B_SUN);

  // Wolken (Bloecke) driften langsam
  for (int k = 0; k < 3; k++) {
    const int cx = (int)(fmodf((float)k * 70.0f - wx * 2.5f, 220.0f) + 220.0f) % 220 - 30, cy = 20 + k * 11;
    if (!night) { fb->fillRect(cx, cy, 20, 5, B_CLOUD); fb->fillRect(cx + 4, cy - 4, 11, 4, B_CLOUD); }
  }

  // Terrain
  const int first = (int)wx;
  const float frac = wx - (float)first;
  const int groundY = 112;
  for (int i = -1; i < 22; i++) {
    const int bxw = first + i;
    const int x = (int)((float)i * S - frac * S);
    const int hgt = heightAt(bxw);
    for (int j = 0; j < hgt; j++) {
      const int y = groundY - (j + 1) * S + S;
      const bool top = (j == hgt - 1);
      uint8_t col = top ? ((bxw & 1) ? B_GRASS : B_GRASS2) : (j >= hgt - 3 ? ((bxw + j) & 1 ? B_DIRT : B_DIRT2) : ((bxw + j) & 1 ? B_STONE : B_STONE2));
      fb->fillRect(x, y, S, S, col);
      if (top) fb->fillRect(x, y, S, 2, B_GRASS);
    }
    if (treeAt(bxw) && hgt < 6) {                        // Baum aus Bloecken
      const int ty = groundY - hgt * S + S;
      for (int j = 1; j <= 3; j++) fb->fillRect(x + 2, ty - j * S, 4, S, B_WOOD);
      fb->fillRect(x - 6, ty - 5 * S, 20, S, B_LEAF); fb->fillRect(x - 2, ty - 6 * S, 12, S, B_LEAF2);
      fb->fillRect(x - 2, ty - 4 * S, 12, S, B_LEAF2);
    }
  }

  // Figur: bleibt in der Mitte, Hoehe folgt dem Boden
  const int px = 80;
  const int gh = heightAt((int)(wx + 80.0f / S));
  const int py = groundY - gh * S + S - 1;
  const int st = (int)((now / 180) & 1);
  const bool wave = ((now / 1000) % 9) == 0;
  fb->fillRect(px - 3, py - 8 + 0, 3, 8 - st, B_PANTS); fb->fillRect(px + 1, py - 8, 3, 7 + st, B_PANTS);
  fb->fillRect(px - 4, py - 16, 9, 8, B_SHIRT);
  fb->fillRect(px - 7, py - 16 + (wave ? -4 : 1), 3, 7, B_SK);                   // Arm (winkt)
  fb->fillRect(px + 5, py - 15, 3, 7, B_SK);
  fb->fillRect(px - 4, py - 24, 9, 8, B_SK);
  fb->fillRect(px - 4, py - 25, 9, 3, B_HAIR);
  fb->fillRect(px - 2, py - 21, 2, 2, B_SH); fb->fillRect(px + 2, py - 21, 2, 2, B_SH);

  drawTime(26, 4, 4, c, cellS);
  drawTime(26, 4, 4, c, cellD);
  if (c.showSec && c.timeOk) for (int i = 0; i < 12; i++) fb->fillRect(8 + i * 12, 116, 10, 3, (i <= c.ss / 5) ? B_GRASS : B_STONE2);
  if (c.timeOk) {
    char b[16];
    langDate(b, sizeof(b), c.mday, c.mon);
    txt(3, 34, b, B_DIG);
  }
  if (c.hasWx) tempTxt(FB_W - 3 - tempW(c.temp, 1), 34, c.temp, B_DIG);
  (void)t0;
}

static void cover(float p, bool, uint32_t) {
  if (p <= 0.0f) return;
  uint8_t* bf = buf();
  if (p >= 0.97f) { memset(bf, B_DIRT, FB_W * FB_H); return; }
  for (int i = 0; i < 20; i++) {
    const float th = (float)((i * 7) % 20) / 20.0f * 0.5f;
    if (p < th) continue;
    const int h = (int)(fminf(1.0f, (p - th) / 0.45f) * (float)FB_H);
    for (int y = 0; y < h; y++) memset(bf + y * FB_W + i * 8, ((y / 8 + i) & 1) ? B_DIRT : B_DIRT2, 8);
  }
}

}  // namespace f_voxel
