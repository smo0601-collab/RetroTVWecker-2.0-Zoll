#pragma once
/*
  Farben (zur Laufzeit abdunkelbar), Wettersymbole aus Grafikprimitiven und
  Zuordnung WMO-Wettercode -> Symbol/Text (deutsch, nur ASCII, da die
  eingebauten GFX-Schriften keine Umlaute enthalten).
*/
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <math.h>
#include "config.h"

constexpr uint16_t RGB565C(uint8_t r, uint8_t g, uint8_t b) {
  return TFT_SWAP_RB
    ? (uint16_t)(((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3))
    : (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}

// ----- Basisfarben (volle Helligkeit) -----
constexpr uint16_t C_BG       = 0x0000;
constexpr uint16_t B_WHITE    = RGB565C(240, 240, 240);
constexpr uint16_t B_GREY     = RGB565C(130, 135, 145);
constexpr uint16_t B_DIM      = RGB565C(28, 32, 40);
constexpr uint16_t B_LINE     = RGB565C(60, 66, 78);
constexpr uint16_t B_CYAN     = RGB565C(0, 200, 255);
constexpr uint16_t B_YELLOW   = RGB565C(255, 200, 0);
constexpr uint16_t B_ORANGE   = RGB565C(255, 140, 0);
constexpr uint16_t B_RED      = RGB565C(230, 50, 50);
constexpr uint16_t B_GREEN    = RGB565C(60, 210, 110);
constexpr uint16_t B_BLUE     = RGB565C(70, 140, 255);
constexpr uint16_t B_CLOUD    = RGB565C(190, 198, 210);
constexpr uint16_t B_DCLOUD   = RGB565C(120, 128, 140);

// ----- Aktive Farben (werden von applyTheme() skaliert) -----
static uint16_t C_WHITE = B_WHITE, C_GREY = B_GREY, C_DIM = B_DIM, C_LINE = B_LINE,
                C_CYAN = B_CYAN, C_YELLOW = B_YELLOW, C_ORANGE = B_ORANGE, C_RED = B_RED,
                C_GREEN = B_GREEN, C_BLUE = B_BLUE, C_CLOUD = B_CLOUD, C_DARKCLOUD = B_DCLOUD;

// ----- Akzentfarben (im Browser waehlbar) -----
#define ACCENT_COUNT 9
static const uint8_t ACCENT_RGB[ACCENT_COUNT][3] = {
  {0, 200, 255}, {60, 210, 110}, {255, 140, 0}, {255, 200, 0}, {230, 50, 50},
  {255, 80, 170}, {70, 140, 255}, {240, 240, 240}, {170, 100, 255}
};
static const char* const ACCENT_NAME[ACCENT_COUNT] = {
  "Cyan", "Gruen", "Orange", "Gelb", "Rot", "Pink", "Blau", "Weiss", "Violett"
};
static uint8_t  gAccentIdx = 0;
static uint16_t C_ACCENT   = B_CYAN;

static uint16_t scaleColor(uint16_t c, uint8_t pct) {
  uint32_t r = (c >> 11) & 0x1F, g = (c >> 5) & 0x3F, b = c & 0x1F;
  r = r * pct / 100; g = g * pct / 100; b = b * pct / 100;
  return (uint16_t)((r << 11) | (g << 5) | b);
}

// pct = 100 -> volle Farben; kleiner -> alle Farben gleichmaessig abgedunkelt
static void applyTheme(uint8_t pct) {
  C_WHITE = scaleColor(B_WHITE, pct);   C_GREY = scaleColor(B_GREY, pct);
  C_DIM = scaleColor(B_DIM, pct);       C_LINE = scaleColor(B_LINE, pct);
  C_CYAN = scaleColor(B_CYAN, pct);     C_YELLOW = scaleColor(B_YELLOW, pct);
  C_ORANGE = scaleColor(B_ORANGE, pct); C_RED = scaleColor(B_RED, pct);
  C_GREEN = scaleColor(B_GREEN, pct);   C_BLUE = scaleColor(B_BLUE, pct);
  C_CLOUD = scaleColor(B_CLOUD, pct);   C_DARKCLOUD = scaleColor(B_DCLOUD, pct);
  const uint8_t* a = ACCENT_RGB[gAccentIdx < ACCENT_COUNT ? gAccentIdx : 0];
  C_ACCENT = scaleColor(RGB565C(a[0], a[1], a[2]), pct);
}

enum IconType : uint8_t {
  ICON_SUN, ICON_MOON, ICON_PARTLY, ICON_PARTLY_NIGHT,
  ICON_CLOUD, ICON_FOG, ICON_RAIN, ICON_SNOW, ICON_STORM
};

static inline int I(float v) { return (int)lroundf(v); }

// ----- WMO-Code (Open-Meteo weather_code) -> Symbol -----
static IconType iconForCode(int code, bool isDay) {
  if (code == 0 || code == 1) return isDay ? ICON_SUN : ICON_MOON;
  if (code == 2)              return isDay ? ICON_PARTLY : ICON_PARTLY_NIGHT;
  if (code == 3)              return ICON_CLOUD;
  if (code == 45 || code == 48) return ICON_FOG;
  if ((code >= 51 && code <= 57) || (code >= 61 && code <= 67) ||
      (code >= 80 && code <= 82)) return ICON_RAIN;
  if ((code >= 71 && code <= 77) || code == 85 || code == 86) return ICON_SNOW;
  if (code >= 95 && code <= 99) return ICON_STORM;
  return ICON_CLOUD;
}

static const char* textForCode(int code) {
  switch (code) {
    case 0:  return "Klar";
    case 1:  return "Meist klar";
    case 2:  return "Wolkig";
    case 3:  return "Bedeckt";
    case 45: case 48: return "Nebel";
    case 51: case 53: case 55: return "Nieselregen";
    case 56: case 57: return "Eisniesel";
    case 61: return "Lt. Regen";
    case 63: return "Regen";
    case 65: return "Starkregen";
    case 66: case 67: return "Eisregen";
    case 71: return "Lt. Schnee";
    case 73: return "Schnee";
    case 75: return "Starkschnee";
    case 77: return "Griesel";
    case 80: return "Lt. Schauer";
    case 81: return "Schauer";
    case 82: return "St. Schauer";
    case 85: case 86: return "Schneefall";
    case 95: return "Gewitter";
    case 96: case 99: return "Gewit.+Hagel";
    default: return "--";
  }
}

// ----- Einzelne Symbolteile -----
static void iconCloud(Adafruit_GFX& g, int cx, int cy, float s, uint16_t col) {
  g.fillCircle(cx - I(0.50f * s), cy + I(0.05f * s), I(0.34f * s), col);
  g.fillCircle(cx - I(0.05f * s), cy - I(0.18f * s), I(0.48f * s), col);
  g.fillCircle(cx + I(0.50f * s), cy + I(0.08f * s), I(0.32f * s), col);
  g.fillRoundRect(cx - I(0.80f * s), cy + I(0.02f * s), I(1.60f * s), I(0.38f * s), I(0.19f * s), col);
}

static void iconSun(Adafruit_GFX& g, int cx, int cy, float s, uint16_t col) {
  g.fillCircle(cx, cy, I(0.36f * s), col);
  for (int i = 0; i < 8; i++) {
    float a = i * PI / 4.0f;
    float c = cosf(a), sn = sinf(a);
    int x0 = cx + I(c * 0.54f * s), y0 = cy + I(sn * 0.54f * s);
    int x1 = cx + I(c * 0.82f * s), y1 = cy + I(sn * 0.82f * s);
    g.drawLine(x0, y0, x1, y1, col);
    if (s >= 18) {
      g.drawLine(x0 + 1, y0, x1 + 1, y1, col);
      g.drawLine(x0, y0 + 1, x1, y1 + 1, col);
    }
  }
}

static void iconMoon(Adafruit_GFX& g, int cx, int cy, float s, uint16_t col) {
  g.fillCircle(cx, cy, I(0.45f * s), col);
  g.fillCircle(cx + I(0.22f * s), cy - I(0.15f * s), I(0.40f * s), C_BG);
}

// ----- Gesamtsymbol, (cx,cy) = Mitte, s = grobe halbe Breite in Pixeln -----
static void drawIcon(Adafruit_GFX& g, int cx, int cy, float s, IconType t) {
  switch (t) {
    case ICON_SUN:
      iconSun(g, cx, cy, s, C_YELLOW);
      break;
    case ICON_MOON:
      iconMoon(g, cx, cy, s, C_YELLOW);
      break;
    case ICON_PARTLY:
      iconSun(g, cx - I(0.28f * s), cy - I(0.28f * s), s * 0.62f, C_YELLOW);
      iconCloud(g, cx + I(0.12f * s), cy + I(0.18f * s), s * 0.8f, C_CLOUD);
      break;
    case ICON_PARTLY_NIGHT:
      iconMoon(g, cx - I(0.25f * s), cy - I(0.25f * s), s * 0.62f, C_YELLOW);
      iconCloud(g, cx + I(0.12f * s), cy + I(0.18f * s), s * 0.8f, C_CLOUD);
      break;
    case ICON_CLOUD:
      iconCloud(g, cx, cy, s, C_CLOUD);
      break;
    case ICON_FOG:
      for (int i = 0; i < 4; i++) {
        float w = (i % 2) ? 1.2f * s : 1.5f * s;
        int h = max(2, I(0.10f * s));
        g.fillRoundRect(cx - I(w / 2), cy - I(0.45f * s) + I(i * 0.30f * s), I(w), h, h / 2, C_GREY);
      }
      break;
    case ICON_RAIN:
      iconCloud(g, cx, cy - I(0.25f * s), s * 0.9f, C_CLOUD);
      for (int k = -1; k <= 1; k++) {
        int x = cx + I(k * 0.38f * s);
        int y0 = cy + I(0.28f * s);
        int y1 = y0 + I(0.36f * s);
        g.drawLine(x, y0, x - I(0.12f * s), y1, C_BLUE);
        if (s >= 18) g.drawLine(x + 1, y0, x - I(0.12f * s) + 1, y1, C_BLUE);
      }
      break;
    case ICON_SNOW:
      iconCloud(g, cx, cy - I(0.25f * s), s * 0.9f, C_CLOUD);
      for (int k = -1; k <= 1; k++) {
        int x = cx + I(k * 0.38f * s);
        int y = cy + I((k == 0 ? 0.62f : 0.45f) * s);
        g.fillCircle(x, y, max(1, I(0.08f * s)), C_WHITE);
      }
      break;
    case ICON_STORM:
      iconCloud(g, cx, cy - I(0.25f * s), s * 0.9f, C_DARKCLOUD);
      g.fillTriangle(cx + I(0.12f * s), cy + I(0.10f * s),
                     cx - I(0.16f * s), cy + I(0.48f * s),
                     cx + I(0.06f * s), cy + I(0.48f * s), C_YELLOW);
      g.fillTriangle(cx + I(0.06f * s), cy + I(0.48f * s),
                     cx - I(0.08f * s), cy + I(0.88f * s),
                     cx + I(0.22f * s), cy + I(0.36f * s), C_YELLOW);
      break;
  }
}
