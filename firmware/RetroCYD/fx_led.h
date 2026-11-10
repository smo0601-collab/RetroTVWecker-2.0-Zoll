// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  RGB-LED auf der Rueckseite des CYD (GPIO 4 rot, 16 gruen, 17 blau, LOW = an)
// ============================================================================
//  - Grundfarbe: im Menue die Farbe der Kategorie, in einigen Spielen z. B. die
//    Farbe des Spielers, der dran ist (Vier gewinnt), oder die Startampel.
//  - Blitze: jede Vibration (Treffer, Apfel, Absturz ...) blitzt kurz mit,
//    Spiele koennen eigene Farben blitzen lassen.
//  - Party: Regenbogen bei neuem Rekord oder geloestem Raetsel.
//  Ein/aus und Helligkeit: Web-Oberflaeche (Einstellungen).
// ============================================================================
#pragma once

namespace Led {
  const int PR = 4, PG = 16, PB = 17;
  const int CH_R = 2, CH_G = 3, CH_B = 4;        // LEDC-Kanaele (7 = Hintergrundlicht, 5 = Ton)
  struct Rgb { uint8_t r, g, b; };
  Rgb base{0, 0, 0}, fl{0, 0, 0};
  uint32_t flUntil = 0, flLen = 1, partyUntil = 0, blinkPer = 0;
  bool inited = false;

  void begin() {
    ledcSetup(CH_R, 5000, 8); ledcAttachPin(PR, CH_R);
    ledcSetup(CH_G, 5000, 8); ledcAttachPin(PG, CH_G);
    ledcSetup(CH_B, 5000, 8); ledcAttachPin(PB, CH_B);
    ledcWrite(CH_R, 255); ledcWrite(CH_G, 255); ledcWrite(CH_B, 255);   // 255 = aus (LOW = an)
    inited = true;
  }

  void out(Rgb c) {
    if (!inited) return;
    const uint32_t bri = cfg.ledOn ? cfg.ledBri : 0;
    auto d = [&](uint8_t v, uint32_t k) -> uint8_t {
      const uint32_t x = (uint32_t)v * v / 255 * bri / 100 * k / 100;
      return (uint8_t)(255 - (x > 255 ? 255 : x));
    };
    ledcWrite(CH_R, d(c.r, 100));
    ledcWrite(CH_G, d(c.g, 70));                  // gruen ist bei dieser LED viel heller
    ledcWrite(CH_B, d(c.b, 90));
  }

  Rgb c565(uint16_t c) { return {(uint8_t)(((c >> 11) & 31) * 255 / 31), (uint8_t)(((c >> 5) & 63) * 255 / 63), (uint8_t)((c & 31) * 255 / 31)}; }
  Rgb dim(Rgb c, int f) { return {(uint8_t)(c.r * f / 255), (uint8_t)(c.g * f / 255), (uint8_t)(c.b * f / 255)}; }

  // Grundfarbe (bleibt, bis etwas anderes gesetzt wird); per = Blinken in ms (0 = ruhig)
  void set(Rgb c, uint32_t per = 0) { base = c; blinkPer = per; }
  void set565(uint16_t c, int f = 255, uint32_t per = 0) { set(dim(c565(c), f), per); }
  void off() { base = {0, 0, 0}; blinkPer = 0; partyUntil = 0; flUntil = 0; out(base); }
  void flash(Rgb c, uint32_t ms) { fl = c; flLen = ms ? ms : 1; flUntil = millis() + flLen; }
  void party(uint32_t ms) { partyUntil = millis() + ms; }
  // aus der Vibration: kurzer Blitz, Staerke = Helligkeit, starke Stoesse rot
  void hit(uint16_t ms, uint8_t strength) {
    const uint32_t len = constrain((int)ms, 40, 350);
    flash(strength >= 0xC0 ? Rgb{255, 40, 0} : dim({255, 255, 255}, 80 + strength * 175 / 255), len);
  }

  Rgb hue(uint16_t h) {
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

  void tick() {
    if (!inited) return;
    static uint32_t last = 0;
    const uint32_t now = millis();
    if (now - last < 15) return;
    last = now;
    Rgb c = base;
    if (blinkPer && (now / blinkPer) % 2) c = {0, 0, 0};
    if (partyUntil && (int32_t)(partyUntil - now) > 0) c = hue((now * 3) % 1536);
    else partyUntil = 0;
    if (flUntil && (int32_t)(flUntil - now) > 0) {
      const uint32_t left = flUntil - now;          // ausklingen lassen
      const int t = (int)(left * 255 / flLen);
      c = {(uint8_t)(c.r + (fl.r - c.r) * t / 255), (uint8_t)(c.g + (fl.g - c.g) * t / 255), (uint8_t)(c.b + (fl.b - c.b) * t / 255)};
    } else flUntil = 0;
    out(c);
  }
}
