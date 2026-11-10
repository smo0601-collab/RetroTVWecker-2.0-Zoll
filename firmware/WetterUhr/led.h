#pragma once
/*
  led.h - RGB-LED auf der Rueckseite des CYD (GPIO 4 rot, 16 gruen, 17 blau, LOW = an)

  Modus (Browser: Helligkeit > RGB-LED):
    0 aus, 1 feste Farbe, 2 Wetter-Farbe, 3 Temperatur-Farbe, 4 Farbwechsel, 5 Atmen, 6 Sekundenpuls
  Wann: immer / nur nachts (Nachtlicht) / nachts aus. "Nachts" = Dimm-Zeitfenster, ohne Zeitplan nach Sonnenstand.
  Hinweise blinken zusaetzlich (einzeln abschaltbar): Wecker rot, Update blau, kein WLAN orange,
  Gewitter violett, Frost eisblau. Im Standby bleibt die LED aus (ausser Wecker und Update).
  Wird aus WetterUhr.ino vor webui.h eingebunden (nur CYD; beim ESP32-C3 leer).
*/

#if PIN_LED_R >= 0
namespace led {

enum Mode : uint8_t { M_OFF = 0, M_FIX, M_WX, M_TEMP, M_RAINBOW, M_BREATHE, M_PULSE, M_COUNT };
enum Hint : uint8_t { H_ALARM = 1, H_WIFI = 2, H_UPDATE = 4, H_WARN = 8 };

struct Rgb { uint8_t r, g, b; };

static Rgb   cur{0, 0, 0};
static bool  inited = false;
// Vorschau aus dem Browser (gilt einige Sekunden, ohne zu speichern)
static uint32_t prevUntil = 0;
static uint8_t  prevMode = 0, prevBri = 0;
static uint32_t prevCol = 0;
// Wetter fuer die Farben (einmal pro Sekunde kopiert)
static bool  wxOk = false;
static int   wxCode = 0, wxCodeToday = 0;
static float wxTemp = 0, wxMinToday = 99;
static bool  wxDay = true;
static uint32_t wxAt = 0;

static void begin() {
  ledcAttach(PIN_LED_R, 5000, 8);
  ledcAttach(PIN_LED_G, 5000, 8);
  ledcAttach(PIN_LED_B, 5000, 8);
  ledcWrite(PIN_LED_R, 255); ledcWrite(PIN_LED_G, 255); ledcWrite(PIN_LED_B, 255);   // LOW = an -> 255 = aus
  inited = true;
}

// Gamma ~2 und Helligkeit (Prozent); die LED ist sehr hell, gruen besonders -> etwas zuruecknehmen
static void out(Rgb c, uint8_t bri) {
  if (!inited) return;
  auto d = [&](uint8_t v, uint16_t k) -> uint8_t {
    const uint32_t x = (uint32_t)v * v / 255 * bri / 100 * k / 100;
    return (uint8_t)(255 - (x > 255 ? 255 : x));
  };
  ledcWrite(PIN_LED_R, d(c.r, 100));
  ledcWrite(PIN_LED_G, d(c.g, 70));
  ledcWrite(PIN_LED_B, d(c.b, 90));
  cur = c;
}

static Rgb fromHex(uint32_t h) { return {(uint8_t)(h >> 16), (uint8_t)(h >> 8), (uint8_t)h}; }
static Rgb scale(Rgb c, int f) { return {(uint8_t)(c.r * f / 255), (uint8_t)(c.g * f / 255), (uint8_t)(c.b * f / 255)}; }
static Rgb mix(Rgb a, Rgb b, int t) {   // t 0..255
  return {(uint8_t)(a.r + (b.r - a.r) * t / 255), (uint8_t)(a.g + (b.g - a.g) * t / 255), (uint8_t)(a.b + (b.b - a.b) * t / 255)};
}
static Rgb hue(uint16_t h) {            // 0..1535
  const uint8_t f = h & 255;
  switch ((h >> 8) % 6) {
    case 0: return {255, f, 0};
    case 1: return {(uint8_t)(255 - f), 255, 0};
    case 2: return {0, 255, f};
    case 3: return {0, (uint8_t)(255 - f), 255};
    case 4: return {f, 0, 255};
    default: return {255, 0, (uint8_t)(255 - f)};
  }
}
// weiche Welle 0..255, Periode p ms
static int wave(uint32_t now, uint32_t p) {
  const float s = sinf((now % p) * 6.2831853f / p);
  return (int)(127.5f + 127.5f * s);
}

static Rgb tempColor(float t) {
  static const struct { float t; Rgb c; } K[] = {
    {-10, {60, 60, 255}}, {0, {0, 200, 255}}, {10, {0, 255, 80}}, {20, {255, 220, 0}}, {30, {255, 40, 0}}};
  if (t <= K[0].t) return K[0].c;
  for (int i = 1; i < 5; i++)
    if (t <= K[i].t) return mix(K[i - 1].c, K[i].c, (int)((t - K[i - 1].t) * 255 / (K[i].t - K[i - 1].t)));
  return K[4].c;
}

static Rgb wxColor(uint32_t now) {
  if (!wxOk) return {40, 40, 40};
  const int c = wxCode;
  if (c >= 95) {                                    // Gewitter: violett mit kurzen Blitzen
    if ((now / 50) % 97 == 0 || (now / 50) % 131 == 0) return {255, 255, 255};
    return {150, 0, 255};
  }
  if ((c >= 71 && c <= 77) || c == 85 || c == 86) return mix({180, 230, 255}, {255, 255, 255}, wave(now, 4000));   // Schnee
  if ((c >= 51 && c <= 67) || (c >= 80 && c <= 82)) return mix({0, 40, 200}, {0, 110, 255}, wave(now, 3000));     // Regen
  if (c == 45 || c == 48) return {120, 120, 130};                                                                 // Nebel
  if (c == 2 || c == 3) return wxDay ? Rgb{170, 170, 150} : Rgb{60, 60, 110};                                     // bewoelkt
  return wxDay ? Rgb{255, 190, 0} : Rgb{20, 30, 160};                                                             // klar
}

static bool nightNow() {
  if (gSet.dimOn) return gDimActive;
  return wxOk && !wxDay;
}

static void tick(uint32_t now) {
  if (!inited) return;
  static uint32_t last = 0;
  if (now - last < 20) return;                     // 50 x pro Sekunde reicht
  last = now;
  if (now - wxAt > 1000 || !wxAt) {
    wxAt = now;
    Weather w;
    wxOk = getWeather(w);
    if (wxOk) {
      wxCode = w.code; wxTemp = w.temp; wxDay = w.isDay;
      wxCodeToday = w.nDays > 0 ? w.dCode[0] : w.code;
      wxMinToday = w.nDays > 0 ? w.dMin[0] : w.temp;
    }
  }
  const uint8_t hints = gSet.ledHint;
  // 1. Hinweise mit Vorrang
  if (gAlarmRinging && (hints & H_ALARM)) { out((now / 400) % 2 ? Rgb{255, 0, 0} : Rgb{0, 0, 0}, 100); return; }
  if (gOtaActive && (hints & H_UPDATE)) { out((now / 150) % 2 ? Rgb{0, 60, 255} : Rgb{0, 0, 0}, 80); return; }
  // 2. Grundfarbe
  const bool preview = prevUntil && (int32_t)(prevUntil - now) > 0;
  if (!preview) prevUntil = 0;
  const uint8_t mode = preview ? prevMode : gSet.ledMode;
  const uint8_t bri = preview ? prevBri : gSet.ledBri;
  const Rgb fix = fromHex(preview ? prevCol : gSet.ledCol);
  bool show = mode != M_OFF && !standby;
  if (!preview && show) {
    if (gSet.ledTime == 1 && !nightNow()) show = false;   // nur nachts (Nachtlicht)
    if (gSet.ledTime == 2 && nightNow()) show = false;    // nachts aus
  }
  Rgb c{0, 0, 0};
  if (show) {
    switch (mode) {
      case M_FIX:     c = fix; break;
      case M_WX:      c = wxColor(now); break;
      case M_TEMP:    c = wxOk ? tempColor(wxTemp) : Rgb{40, 40, 40}; break;
      case M_RAINBOW: c = hue((now / 40) % 1536); break;                 // ein Umlauf in etwa 60 s
      case M_BREATHE: c = scale(fix, 20 + wave(now, 5000) * 235 / 255); break;
      case M_PULSE: {
        static time_t lastS = 0;
        static uint32_t at = 0;
        const time_t s = time(nullptr);
        if (s != lastS) { lastS = s; at = now; }
        const uint32_t d = now - at;
        c = d < 120 ? fix : (d < 300 ? scale(fix, 255 - (int)(d - 120) * 255 / 180) : Rgb{0, 0, 0});
        break;
      }
      default: break;
    }
  }
  uint8_t b = bri;
  // 3. Hinweise, die kurz darueber blinken (auch wenn die LED sonst aus ist, nicht im Standby)
  if (!standby) {
    const uint32_t p = now % 8000;
    const bool dbl = p < 120 || (p > 260 && p < 380);       // Doppelblitz alle 8 s
    if ((hints & H_WARN) && wxOk && dbl) {
      if (wxCode >= 95 || wxCodeToday >= 95) { c = {170, 0, 255}; b = max<uint8_t>(b, 60); }
      else if (wxTemp <= 1.0f || wxMinToday <= 0.0f) { c = {120, 220, 255}; b = max<uint8_t>(b, 60); }
    }
    if ((hints & H_WIFI) && !setupMode && WiFi.status() != WL_CONNECTED && (now % 3000) < 150) { c = {255, 90, 0}; b = max<uint8_t>(b, 60); }
  }
  out(c, b);
}

// Vorschau aus dem Browser
static void preview(uint8_t m, uint32_t col, uint8_t b) {
  prevMode = m; prevCol = col; prevBri = b;
  prevUntil = millis() + 8000;
  if (!prevUntil) prevUntil = 1;
}

}  // namespace led
#endif
