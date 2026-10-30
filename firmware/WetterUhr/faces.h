#pragma once
/*
  faces.h - Themen-Zifferblaetter: Verwaltung, Bildausgabe und Uebergaenge

  Ablauf pro Bild:
    1. Das Zifferblatt zeichnet seine Szene komplett in den 160x120-Puffer (fb).
    2. Bei einem Wechsel wird ein zifferblatt-eigener Uebergangseffekt darueber gelegt.
    3. push() verdoppelt den Puffer auf 320x240 und sendet nur die 32x16-Pixel-Kacheln,
       deren Inhalt sich seit dem letzten Bild geaendert hat.

  Jedes Zifferblatt ist ein eigenes Modul (face_*.h) mit drei Funktionen:
    enter(now)               Zustand und Palette einrichten
    draw(ctx)                ein Bild zeichnen
    cover(p, in, now)        Uebergang: p = 0 nichts verdeckt ... 1 alles verdeckt;
                             in = false: Zifferblatt wird verlassen, true: wird betreten
  Neues Zifferblatt hinzufuegen: Modul schreiben, hier einbinden und in FACES[] eintragen,
  FACE_COUNT und CLOCK_STYLE_COUNT (config.h) erhoehen.
*/
#include <new>
#include "faces_core.h"
#include "face_adventure.h"
#include "face_blocks.h"
#include "face_maze.h"
#include "face_hop.h"
#include "face_neon.h"
#include "face_space.h"
#include "face_rain.h"
#include "face_works.h"
#include "face_bricks.h"
#include "face_frog.h"
#include "face_missile.h"
#include "face_neonbrk.h"
#include "face_heart.h"
#include "face_sine.h"
#include "face_badminton.h"
#include "face_testcard.h"
#include "face_dackel.h"
#include "face_radar.h"
#include "face_dream.h"
#include "face_aqua.h"
#include "face_voxel.h"
#include "face_family.h"
#include "face_iss.h"
#include "face_sunmoon.h"
#include "face_xmas.h"
#include "face_elf.h"
#include "face_halloween.h"
#include "face_easter.h"
#include "face_valentine.h"
#include "face_newyear.h"
#include "face_dumpling.h"
#include "ticker.h"

#define FACE_FIRST_STYLE 2       // clockStyle 0 = Flip-Karten, 1 = Digital + Balken, ab 2 = Themen-Zifferblaetter
#define FACE_COUNT       31
#define TESTCARD_STYLE   (FACE_FIRST_STYLE + 15)   // Testbild: Index 15 in FACES[] (Nachtmodus)
#define RADAR_STYLE      (FACE_FIRST_STYLE + 17)   // Flugradar: Index 17 in FACES[]
#define ISS_STYLE        (FACE_FIRST_STYLE + 22)   // ISS-Tracker: Index 22 in FACES[]
#define AQUA_STYLE       (FACE_FIRST_STYLE + 19)   // Aquarium: Index 19 in FACES[]
#define NEWYEAR_STYLE    (FACE_FIRST_STYLE + 29)   // Silvester: Index 29 in FACES[] (Silvester-Automatik)
// Neue Zifferblaetter immer am Ende von FACES[] anhaengen, damit gespeicherte Nummern gueltig bleiben.

namespace fc {

struct Face {
  const char* name;
  void (*enter)(uint32_t now);
  void (*draw)(const FaceCtx& c);
  void (*cover)(float p, bool in, uint32_t now);
  bool     scan;       // Scanlines (jede zweite Bildschirmzeile abgedunkelt)
  uint16_t frameMs;    // Zeit zwischen zwei Bildern
};

static const Face FACES[FACE_COUNT] = {
  {"Abenteuer",        f_adv::enter,   f_adv::draw,   f_adv::cover,   false, 50},
  {"Fallende Bloecke", f_blk::enter,   f_blk::draw,   f_blk::cover,   false, 50},
  {"Labyrinth",        f_maze::enter,  f_maze::draw,  f_maze::cover,  false, 50},
  {"Huepfwelt",        f_hop::enter,   f_hop::draw,   f_hop::cover,   false, 50},
  {"Neon-Raster",      f_neon::enter,  f_neon::draw,  f_neon::cover,  true,  50},
  {"Asteroiden",       f_space::enter, f_space::draw, f_space::cover, true,  50},
  {"Code-Regen",       f_rain::enter,  f_rain::draw,  f_rain::cover,  false, 55},
  {"Zahnrad-Werk",     f_fab::enter,   f_fab::draw,   f_fab::cover,   false, 50},
  {"Ziegelbrecher",    f_brk::enter,   f_brk::draw,   f_brk::cover,   false, 40},
  {"Froschfurt",       f_frog::enter,  f_frog::draw,  f_frog::cover,  false, 50},
  {"Raketenabwehr",    f_msl::enter,   f_msl::draw,   f_msl::cover,   false, 50},
  {"Neonbrecher",      f_arc::enter,   f_arc::draw,   f_arc::cover,   false, 40},
  {"Pulsierendes Herz", f_heart::enter, f_heart::draw, f_heart::cover, false, 40},
  {"Sinuskurve",       f_sine::enter,  f_sine::draw,  f_sine::cover,  false, 60},
  {"Badminton",        f_bad::enter,   f_bad::draw,   f_bad::cover,   false, 50},
  {"Testbild",         f_tv::enter,    f_tv::draw,    f_tv::cover,    false, 60},
  {"Dackelwelpe",      f_dack::enter,  f_dack::draw,  f_dack::cover,  false, 50},
  {"Flugradar",        f_radar::enter, f_radar::draw, f_radar::cover, false, 33},
  {"Traumsand",        f_dream::enter, f_dream::draw, f_dream::cover, false, 40},
  {"Aquarium",         f_aqua::enter,  f_aqua::draw,  f_aqua::cover,  false, 40},
  {"Blockwelt",        f_voxel::enter, f_voxel::draw, f_voxel::cover, false, 40},
  {"Familie",          f_family::enter, f_family::draw, f_family::cover, false, 50},
  {"ISS-Tracker",      f_iss::enter,   f_iss::draw,   f_iss::cover,   false, 50},
  {"Sonne und Mond",   f_sunmoon::enter, f_sunmoon::draw, f_sunmoon::cover, false, 100},
  {"Weihnachten",      f_xmas::enter,  f_xmas::draw,  f_xmas::cover,  false, 50},
  {"Weihnachtself",    f_elf::enter,   f_elf::draw,   f_elf::cover,   false, 50},
  {"Halloween",        f_hal::enter,   f_hal::draw,   f_hal::cover,   false, 50},
  {"Ostern",           f_egg::enter,   f_egg::draw,   f_egg::cover,   false, 50},
  {"Valentinstag",     f_val::enter,   f_val::draw,   f_val::cover,   false, 50},
  {"Silvester",        f_nye::enter,   f_nye::draw,   f_nye::cover,   false, 40},
  {"Squishy Dumpling", f_dump::enter,  f_dump::draw,  f_dump::cover,  false, 40},
};

static const uint32_t T_OUT = 650;     // Dauer des Verlassens
static const uint32_t T_IN  = 750;     // Dauer des Betretens

static int      cur       = -1;        // aktives Zifferblatt (0..FACE_COUNT-1)
static uint8_t  phase     = 0;         // 0 normal, 1 Ausblenden, 2 Einblenden
static uint32_t pStart    = 0, enterT = 0, lastFrame = 0;
static uint16_t LUT[256], LUT2[256];
static bool     scanlines = false;
static bool     forceAll  = true;
static uint32_t tileHash[(FB_W / 16) * (FB_H / 8)];

static void lutBuild(uint8_t pct) {
  PAL[I_TICK]  = RGB565C(tk::cfg.r, tk::cfg.g, tk::cfg.b);
  PAL[I_TICK2] = RGB565C(tk::cfg.r / 3, tk::cfg.g / 3, tk::cfg.b / 3);
  for (int i = 0; i < 256; i++) {
    LUT[i]  = scaleColor(PAL[i], pct);
    LUT2[i] = scaleColor(LUT[i], 58);
  }
  forceAll = true;
}

static bool ready() { return fb != nullptr && fb->getBuffer() != nullptr && cur >= 0; }
static bool isThemed(int style) { return style >= FACE_FIRST_STYLE && style < FACE_FIRST_STYLE + FACE_COUNT; }

// Zifferblatt aktivieren. false = Speicher reicht nicht (dann ein normales Design verwenden).
static bool begin(int idx, uint32_t now, bool intro, uint8_t pct) {
  if (idx < 0 || idx >= FACE_COUNT) return false;
  if (!fb) {
    fb = new (std::nothrow) GFXcanvas8(FB_W, FB_H);
    if (fb && !fb->getBuffer()) { delete fb; fb = nullptr; }
    if (!fb) { cur = -1; return false; }
  }
  cur = idx;
  rs ^= now * 2654435761u;                // Zufall mit der Startzeit mischen
  if (!rs) rs = 2463534242u;
  palReset();
  FACES[idx].enter(now);
  lutBuild(pct);
  enterT = now;
  lastFrame = 0;
  phase = intro ? 2 : 0;
  pStart = now;
  return true;
}

static void release() {
  if (fb) { delete fb; fb = nullptr; }
  cur = -1;
  phase = 0;
}

static void setBrightness(uint8_t pct) { if (cur >= 0) lutBuild(pct); }
static void invalidate() { forceAll = true; }
static void startOutro(uint32_t now) { phase = 1; pStart = now; }
static void cancelOutro(uint32_t now) { if (phase == 1) { phase = 2; pStart = now; } }
static bool outroDone(uint32_t now) { return phase == 1 && now - pStart >= T_OUT; }

// Puffer verdoppeln und geaenderte Kacheln senden (TFT = Adafruit_ST7789 oder Testattrappe)
template <class TFT>
static int push(TFT& tft) {
  static uint16_t out[32 * 16];
  const uint8_t* src = fb->getBuffer();
  int sent = 0;
  tft.startWrite();
  for (int ty = 0; ty < FB_H / 8; ty++) {
    for (int tx = 0; tx < FB_W / 16; tx++) {
      const uint8_t* p = src + (ty * 8) * FB_W + tx * 16;
      uint32_t h = 2166136261u;
      for (int r = 0; r < 8; r++) {
        for (int k = 0; k < 16; k += 4) {
          uint32_t v;
          memcpy(&v, p + r * FB_W + k, 4);
          h = (h ^ v) * 16777619u;
          h ^= h >> 15;
        }
      }
      const int idx = ty * (FB_W / 16) + tx;
      if (!forceAll && h == tileHash[idx]) continue;
      tileHash[idx] = h;
      for (int r = 0; r < 8; r++) {
        uint16_t* o0 = out + (2 * r) * 32;
        uint16_t* o1 = o0 + 32;
        for (int cx = 0; cx < 16; cx++) {
          const uint8_t i = p[r * FB_W + cx];
          const uint16_t a = LUT[i];
          const uint16_t b = scanlines ? LUT2[i] : a;
          o0[2 * cx] = o0[2 * cx + 1] = a;
          o1[2 * cx] = o1[2 * cx + 1] = b;
        }
      }
      tft.setAddrWindow(tx * 32, ty * 16, 32, 16);
      tft.writePixels(out, 32 * 16);
      sent++;
    }
  }
  tft.endWrite();
  forceAll = false;
  return sent;
}

// Ein Bild zeichnen (nach frameMs). Rueckgabe: true, wenn ein Bild gezeichnet wurde.
template <class TFT>
static bool frame(TFT& tft, const FaceCtx& c) {
  if (!ready()) return false;
  const Face& F = FACES[cur];
  if (!forceAll && c.now - lastFrame < F.frameMs) return false;
  lastFrame = c.now;
  scanlines = F.scan;

  F.draw(c);

  // Statuspunkt unten rechts: rot = kein WLAN, orange = Wetterdaten fehlen/veraltet
  if (!c.wifi)                  fb->fillRect(FB_W - 3, FB_H - 3, 2, 2, I_RED);
  else if (!c.hasWx || c.stale) fb->fillRect(FB_W - 3, FB_H - 3, 2, 2, I_ORANGE);

  // IP-Adresse beim Start kurz einblenden (im Einrichtungsmodus dauerhaft)
  if (c.info && c.info[0] && (c.apMode || c.now - enterT < 7000)) {
    fb->fillRect(0, FB_H - 9, FB_W, 9, 0);
    txtc(FB_W / 2, FB_H - 8, c.info, I_WHITE);
  }

  if (tk::active) tk::drawFb(c.now);                      // Laufschrift liegt ueber der Szene

  if (phase == 1) {
    float p = (float)(c.now - pStart) / (float)T_OUT;
    F.cover(fcl(p, 0.0f, 1.0f), false, c.now);
  } else if (phase == 2) {
    float p = 1.0f - (float)(c.now - pStart) / (float)T_IN;
    if (p <= 0.0f) phase = 0;
    else F.cover(p, true, c.now);
  }

  push(tft);
  return true;
}

}  // namespace fc
