#pragma once
/*
  ticker.h - Laufschrift in Pixelform (ueber jedem Design)

  Der Text (UTF-8, mit Umlauten und Sonderzeichen) wird einmal in einen winzigen
  1-Bit-Puffer (8 Pixel hoch, nur waehrend des Durchlaufs) gerendert und dann Spalte fuer
  Spalte ueber den Bildschirm geschoben. Nichts blockiert, es wird nur ein schmales Band
  neu gezeichnet:
    - Themen-Designs: Band wird in den 160x120-Puffer gezeichnet (drawFb)
    - klassische Designs: Band wird zeilenweise direkt zum Display gesendet (drawClassic),
      danach zeichnet die Hauptdatei den Bildschirm einmal komplett neu

  Schrift: eingebaute 5x7-Pixelschrift (Codepage 437) mit Umlauten (Ä Ö Ü ä ö ü ß) und
  vielen Sonderzeichen (° € ± µ ² £ ¥ « » ½ ¼ é è à ç ñ ♥ → ← ↑ ↓ • ...). Unbekannte Zeichen werden "?".
*/
#include "faces_core.h"

namespace tk {

struct Cfg {
  uint8_t pos;        // 0 oben, 1 Mitte, 2 unten
  uint8_t big;        // 0 = 16 Bildschirmpixel hoch, 1 = 32
  uint8_t speed;      // 1..10
  bool    bg;         // Hintergrundband (klassische Designs: immer an)
  uint8_t r, g, b;    // Textfarbe
};

static Cfg cfg = {2, 0, 5, true, 255, 255, 255};
static GFXcanvas1* cv = nullptr;
static int   textW = 0;                  // Breite des gerenderten Textes in Schriftpixeln
static bool  active = false;             // laeuft gerade
static bool  manual = false;             // per Knopf gestartet (laeuft auch bei "aus" zu Ende)
static bool  justEnded = false;          // Durchlauf ist fertig (Hauptdatei wertet aus)
static float pos = 0;                    // Vorschub in Ansichtspixeln
static uint32_t lastT = 0;

static const uint8_t EURO[7] = {0x07, 0x08, 0x1C, 0x10, 0x1C, 0x08, 0x07};

// Unicode -> Codepage 437 (0x7F = eigenes Euro-Zeichen, 0 = unbekannt)
static uint8_t mapCp(uint32_t cp) {
  if (cp < 32) return ' ';
  if (cp < 127) return (uint8_t)cp;
  switch (cp) {
    case 0xC4: return 0x8E; case 0xD6: return 0x99; case 0xDC: return 0x9A;
    case 0xE4: return 0x84; case 0xF6: return 0x94; case 0xFC: return 0x81; case 0xDF: return 0xE1;
    case 0xB0: return 0xF8; case 0xB1: return 0xF1; case 0xB5: return 0xE6; case 0xB7: return 0xFA;
    case 0xB2: return 0xFD; case 0xA3: return 0x9C; case 0xA5: return 0x9D; case 0xA2: return 0x9B;
    case 0xA1: return 0xAD; case 0xBF: return 0xA8; case 0xAB: return 0xAE; case 0xBB: return 0xAF;
    case 0xBD: return 0xAB; case 0xBC: return 0xAC; case 0xAC: return 0xAA; case 0xA7: return 0x15;
    case 0xE9: return 0x82; case 0xE8: return 0x8A; case 0xE0: return 0x85; case 0xE2: return 0x83;
    case 0xEA: return 0x88; case 0xE7: return 0x87; case 0xF1: return 0xA4; case 0xD1: return 0xA5;
    case 0xE5: return 0x86; case 0xC5: return 0x8F; case 0xE6: return 0x91; case 0xC6: return 0x92;
    case 0xF4: return 0x93; case 0xF2: return 0x95; case 0xFB: return 0x96; case 0xF9: return 0x97;
    case 0xFF: return 0x98; case 0xE1: return 0xA0; case 0xED: return 0xA1; case 0xF3: return 0xA2;
    case 0xFA: return 0xA3; case 0xEF: return 0x8B; case 0xEE: return 0x8C; case 0xEC: return 0x8D;
    case 0xEB: return 0x89; case 0xC9: return 0x90;
    case 0x20AC: return 0x7F;                               // Euro
    case 0x2665: return 0x03; case 0x263A: return 0x01; case 0x266A: return 0x0D;
    case 0x2192: return 0x1A; case 0x2190: return 0x1B; case 0x2191: return 0x18; case 0x2193: return 0x19;
    case 0x2022: return 0x07; case 0x2013: case 0x2014: return '-';
    case 0x2018: case 0x2019: return '\'';
    case 0x201C: case 0x201D: case 0x201E: return '"';
    case 0x2026: return 0xFA;
    default: return '?';
  }
}

// UTF-8 -> Codepage-437-Bytes. Rueckgabe: Anzahl Zeichen
static int utf8ToCp(const char* s, uint8_t* out, int cap) {
  int n = 0;
  while (*s && n < cap) {
    const uint8_t c = (uint8_t)*s;
    uint32_t cp = c;
    int extra = 0;
    if (c >= 0xF0)      { cp = c & 0x07; extra = 3; }
    else if (c >= 0xE0) { cp = c & 0x0F; extra = 2; }
    else if (c >= 0xC0) { cp = c & 0x1F; extra = 1; }
    else if (c >= 0x80) { out[n++] = '?'; s++; continue; }
    s++;
    for (int i = 0; i < extra && ((uint8_t)*s & 0xC0) == 0x80; i++, s++) cp = (cp << 6) | ((uint8_t)*s & 0x3F);
    out[n++] = mapCp(cp);
  }
  return n;
}

static void stop() {
  if (cv) { delete cv; cv = nullptr; }
  active = false;
  manual = false;
}

// Text vorbereiten und Durchlauf starten. false: leer oder zu wenig Speicher
static bool start(const char* text, const Cfg& c, uint32_t now, bool byButton) {
  stop();
  cfg = c;
  uint8_t cp[170];
  const int n = utf8ToCp(text, cp, 160);
  if (n <= 0) return false;
  textW = n * 6;
  cv = new (std::nothrow) GFXcanvas1(textW, 8);
  if (!cv || !cv->getBuffer()) { if (cv) { delete cv; cv = nullptr; } return false; }
  cv->fillScreen(0);
  cv->cp437(true);
  for (int i = 0; i < n; i++) {
    if (cp[i] == 0x7F) {
      for (int y = 0; y < 7; y++)
        for (int x = 0; x < 5; x++)
          if (EURO[y] & (0x10 >> x)) cv->drawPixel(i * 6 + x, y, 1);
    } else cv->drawChar(i * 6, 0, cp[i], 1, 0, 1);
  }
  pos = 0; lastT = now; active = true; manual = byButton; justEnded = false;
  return true;
}

static inline bool colOn(int m, int y) {
  const uint8_t* b = cv->getBuffer();
  return b[y * ((textW + 7) >> 3) + (m >> 3)] & (0x80 >> (m & 7));
}

// Vorschub; true = Durchlauf ist zu Ende (Puffer wurde freigegeben)
static bool advance(uint32_t now, int viewW, int scale, float unitPx) {
  float dt = (float)(uint32_t)(now - lastT) * 0.001f;
  lastT = now;
  if (dt > 0.1f) dt = 0.1f;
  const float screenSpeed = 25.0f + cfg.speed * 17.0f;      // Bildschirmpixel pro Sekunde
  pos += screenSpeed / unitPx * dt;
  if (pos >= viewW + textW * scale + 2) {
    stop();
    justEnded = true;
    return true;
  }
  return false;
}

static int bandY(int viewH, int h) { return cfg.pos == 0 ? 0 : (cfg.pos == 1 ? (viewH - h) / 2 : viewH - h); }

// ---- Themen-Designs: Band in den 160x120-Puffer zeichnen ----
static void drawFb(uint32_t now) {
  if (!active || !cv) return;
  const int scale = cfg.big ? 2 : 1;
  if (advance(now, FB_W, scale, 2.0f)) return;
  const int h = 8 * scale + 2, y0 = bandY(FB_H, h);
  using namespace fc;
  if (cfg.bg) {
    fb->fillRect(0, y0, FB_W, h, 0);
    fb->drawFastHLine(0, y0, FB_W, I_TICK2);
    fb->drawFastHLine(0, y0 + h - 1, FB_W, I_TICK2);
  }
  const int base = FB_W - (int)pos;                           // x des ersten Zeichens
  int m0 = (0 - base) / scale; if (m0 < 0) m0 = 0;
  int m1 = (FB_W - base) / scale + 1; if (m1 > textW) m1 = textW;
  for (int m = m0; m < m1; m++) {
    const int x = base + m * scale;
    for (int fy = 0; fy < 8; fy++)
      if (colOn(m, fy)) {
        if (!cfg.bg) fb->fillRect(x + 1, y0 + 1 + fy * scale + 1, scale, scale, 0);
        fb->fillRect(x, y0 + 1 + fy * scale, scale, scale, I_TICK);
      }
  }
}

// ---- klassische Designs: Band zeilenweise zum Display senden ----
// fg/line: bereits helligkeitsskalierte RGB565-Farben
template <class TFT>
static void drawClassic(TFT& tft, uint32_t now, int W, int H, uint16_t fg, uint16_t line) {
  if (!active || !cv) return;
  const int scale = cfg.big ? 4 : 2;
  if (advance(now, W, scale, 1.0f)) return;
  const int h = 8 * scale + 2, y0 = bandY(H, h);
  static uint16_t row[320];
  const int ip = (int)pos, base = W - ip;
  tft.startWrite();
  tft.setAddrWindow(0, y0, W, h);
  for (int r = 0; r < h; r++) {
    if (r == 0 || r == h - 1) { for (int x = 0; x < W; x++) row[x] = line; }
    else {
      const int fy = (r - 1) / scale;
      for (int x = 0; x < W; x++) {
        const int m = (x - base) / scale;
        row[x] = (x >= base && m < textW && colOn(m, fy)) ? fg : 0;
      }
    }
    tft.writePixels(row, W);
  }
  tft.endWrite();
}

}  // namespace tk
