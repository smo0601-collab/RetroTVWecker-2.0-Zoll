#pragma once
/*
  faces_hires.h - feinere Ausgabe der Themen-Designs auf 320x240

  1. Kanten glaetten (Scale2x/EPX): Beim Verdoppeln des 160x120-Puffers wird jeder Pixel nicht einfach zu
     einem 2x2-Block, sondern Treppenstufen an schraegen Kanten werden geglaettet. Schachbrett-Muster und
     gerade Kanten bleiben unveraendert. Kostet kaum Rechenzeit, keinen Speicher.
  2. Feine Schrift (nur mit genug Speicher, auf dem CYD): Texte der Designs werden nicht in den kleinen
     Puffer gezeichnet, sondern in voller Aufloesung und geglaettet darueber gelegt (eigene 4-Bit-Ebene,
     38 KB). Gleiche Schrift und Position wie bisher, nur ohne Treppen.

  Beide lassen sich im Browser abschalten (Anzeige > Darstellung).
*/
#include "font5x7.h"

namespace fc {

static bool hiSmooth = false;            // Kanten glaetten
static bool hiTextOn = false;            // feine Schrift gewuenscht
static uint8_t* hiLayer = nullptr;       // 320x240, 4 Bit je Pixel: 0 = kein Text, 1..15 = Farbe aus hiCol
static uint8_t  hiCol[16];               // Palettenindex je Textfarbe
static int      hiNCol = 0;
static bool     hiUsed = false;          // Ebene enthaelt in diesem Bild etwas
static bool     hiWasUsed = false;       // ... im letzten Bild

static const int HW = FB_W * 2, HH = FB_H * 2;

static bool hiAlloc() {
  if (hiLayer) return true;
  hiLayer = (uint8_t*)malloc(HW * HH / 2);
  if (hiLayer) memset(hiLayer, 0, HW * HH / 2);
  return hiLayer != nullptr;
}
static void hiFree() { if (hiLayer) { free(hiLayer); hiLayer = nullptr; } hiUsed = hiWasUsed = false; }

static inline uint8_t hiGet(int x, int y) {
  const uint8_t v = hiLayer[(y * HW + x) >> 1];
  return (x & 1) ? (v >> 4) : (v & 15);
}
static inline void hiSet(int x, int y, uint8_t c) {
  if ((unsigned)x >= (unsigned)HW || (unsigned)y >= (unsigned)HH) return;
  uint8_t& v = hiLayer[(y * HW + x) >> 1];
  v = (x & 1) ? (uint8_t)((v & 0x0F) | (c << 4)) : (uint8_t)((v & 0xF0) | c);
}

// --- Glaetten kleiner Schwarz-Weiss-Bilder (Zeichen) ---
static void epx2(const uint8_t* s, int w, int h, uint8_t* d) {
  for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++) {
      const uint8_t E = s[y * w + x];
      const uint8_t B = y > 0 ? s[(y - 1) * w + x] : 0, H = y < h - 1 ? s[(y + 1) * w + x] : 0;
      const uint8_t D = x > 0 ? s[y * w + x - 1] : 0, F = x < w - 1 ? s[y * w + x + 1] : 0;
      uint8_t* o = d + (2 * y) * (2 * w) + 2 * x;
      o[0]         = (D == B && B != F && D != H) ? D : E;
      o[1]         = (B == F && B != D && F != H) ? F : E;
      o[2 * w]     = (D == H && D != B && H != F) ? D : E;
      o[2 * w + 1] = (H == F && D != H && B != F) ? F : E;
    }
}
static void epx3(const uint8_t* s, int w, int h, uint8_t* d) {
  auto P = [&](int x, int y) -> uint8_t { return (x < 0 || y < 0 || x >= w || y >= h) ? 0 : s[y * w + x]; };
  const int W3 = 3 * w;
  for (int y = 0; y < h; y++)
    for (int x = 0; x < w; x++) {
      const uint8_t A = P(x - 1, y - 1), B = P(x, y - 1), C = P(x + 1, y - 1);
      const uint8_t D = P(x - 1, y), E = P(x, y), F = P(x + 1, y);
      const uint8_t G = P(x - 1, y + 1), H = P(x, y + 1), I = P(x + 1, y + 1);
      uint8_t* o = d + (3 * y) * W3 + 3 * x;
      if (B != H && D != F) {
        o[0]          = D == B ? D : E;
        o[1]          = (D == B && E != C) || (B == F && E != A) ? B : E;
        o[2]          = B == F ? F : E;
        o[W3]         = (D == B && E != G) || (D == H && E != A) ? D : E;
        o[W3 + 1]     = E;
        o[W3 + 2]     = (B == F && E != I) || (H == F && E != C) ? F : E;
        o[2 * W3]     = D == H ? D : E;
        o[2 * W3 + 1] = (D == H && E != I) || (H == F && E != G) ? H : E;
        o[2 * W3 + 2] = H == F ? F : E;
      } else {
        for (int k = 0; k < 3; k++) o[k] = o[W3 + k] = o[2 * W3 + k] = E;
      }
    }
}

// Ein Zeichen geglaettet in die Ebene setzen. x/y in Bildschirm-Pixeln, f = Vergroesserung (2 * Textgroesse)
static void hiGlyph(int x, int y, unsigned char ch, int f, uint8_t col) {
  static uint8_t a[40], b[160];
  if (ch >= 176) ch++;                                   // wie Adafruit GFX ohne cp437()
  int w = 5, h = 8;
  for (int cx = 0; cx < 5; cx++) {
    const uint8_t bits = FC_FONT5X7[ch * 5 + cx];
    for (int cy = 0; cy < 8; cy++) a[cy * 5 + cx] = (bits >> cy) & 1;
  }
  // Einmal glaetten (doppelte Aufloesung), dann ganzzahlig vergroessern: feinere Stufen, Pixel-Stil bleibt.
  // (Mehrfaches Glaetten wuerde die kleinen Zeichen zu rund und "zerlaufen" aussehen lassen.)
  epx2(a, w, h, b);
  w *= 2; h *= 2;
  const uint8_t* s = b;
  const int rest = f / 2 < 1 ? 1 : f / 2;
  for (int yy = 0; yy < h * rest; yy++)
    for (int xx = 0; xx < w * rest; xx++)
      if (s[(yy / rest) * w + xx / rest]) hiSet(x + xx, y + yy, col);
}

// Gemerkte Texte (faces_core.h, txt) in die Ebene zeichnen
static void hiRasterize() {
  hiWasUsed = hiUsed;
  if (hiUsed) memset(hiLayer, 0, HW * HH / 2);
  hiUsed = false;
  hiNCol = 0;
  for (int i = 0; i < txtN; i++) {
    const TxtItem& t = txtItems[i];
    int ci = 0;
    for (int k = 0; k < hiNCol; k++) if (hiCol[k] == t.col) { ci = k + 1; break; }
    if (!ci) {
      if (hiNCol >= 15) continue;
      hiCol[hiNCol++] = t.col;
      ci = hiNCol;
    }
    const int f = 2 * t.sz;
    int x = 2 * t.x;
    for (int k = 0; k < t.len; k++, x += 6 * f) {
      hiGlyph(x, 2 * t.y, (unsigned char)txtBuf[t.off + k], f, (uint8_t)ci);
    }
    hiUsed = true;
  }
  txtN = 0; txtUsed = 0;
}

// Prueft, ob eine Kachel (16x8 im Puffer = 32x16 auf dem Bildschirm) Text enthaelt; Pruefsumme dazu
static uint32_t hiTileHash(int tx, int ty) {
  if (!hiLayer || (!hiUsed && !hiWasUsed)) return 0;
  uint32_t h = 0;
  for (int r = 0; r < 16; r++) {
    const uint8_t* p = hiLayer + ((ty * 16 + r) * HW + tx * 32) / 2;
    for (int k = 0; k < 16; k += 4) { uint32_t v; memcpy(&v, p + k, 4); h = (h ^ v) * 16777619u; h ^= h >> 13; }
  }
  for (int k = 0; k < hiNCol; k++) h = (h ^ hiCol[k]) * 16777619u;
  return h;
}

// Eine Kachel 32x16 fuer den Bildschirm erzeugen (verdoppeln, ggf. glaetten, feine Schrift darueber)
static void hiTile(const uint8_t* src, int tx, int ty, uint16_t* out, const uint16_t* L1, const uint16_t* L2, bool scan) {
  for (int r = 0; r < 8; r++) {
    const int y = ty * 8 + r;
    const uint8_t* row = src + y * FB_W;
    const uint8_t* up = y > 0 ? row - FB_W : row;
    const uint8_t* dn = y < FB_H - 1 ? row + FB_W : row;
    uint16_t* o0 = out + (2 * r) * 32;
    uint16_t* o1 = o0 + 32;
    for (int cx = 0; cx < 16; cx++) {
      const int x = tx * 16 + cx;
      const uint8_t E = row[x];
      uint8_t e0 = E, e1 = E, e2 = E, e3 = E;
      if (hiSmooth) {
        const uint8_t B = up[x], H = dn[x];
        const uint8_t D = x > 0 ? row[x - 1] : E, F = x < FB_W - 1 ? row[x + 1] : E;
        if (B != H && D != F) {
          if (D == B) e0 = D;
          if (B == F) e1 = F;
          if (D == H) e2 = D;
          if (H == F) e3 = F;
        }
      }
      o0[2 * cx] = L1[e0]; o0[2 * cx + 1] = L1[e1];
      o1[2 * cx] = scan ? L2[e2] : L1[e2]; o1[2 * cx + 1] = scan ? L2[e3] : L1[e3];
    }
  }
  if (hiLayer && hiUsed) {
    for (int r = 0; r < 16; r++) {
      const int y = ty * 16 + r;
      for (int c = 0; c < 32; c++) {
        const uint8_t v = hiGet(tx * 32 + c, y);
        if (v) out[r * 32 + c] = (scan && (r & 1)) ? L2[hiCol[v - 1]] : L1[hiCol[v - 1]];
      }
    }
  }
}

}  // namespace fc
