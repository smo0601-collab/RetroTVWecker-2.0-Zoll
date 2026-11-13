// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  KIT fuer die Touch-Spiele (2048, Lichter aus, Paare finden, Rohrpost,
//  Vier gewinnt, Siebzehn und vier, Spurwechsel)
// ============================================================================
//  - Beruehrung als Ereignis: Antippen (Ort) oder Wischen (Richtung, ab 30 Pixel)
//  - Knoepfe, Auswahl-Bildschirm (Spielart), Hinweisfenster
//  - Bild wird nur neu gezeichnet, wenn sich etwas geaendert hat (Kit::dirty)
//  - Ton ueber den Lautsprecher-Anschluss des CYD (GPIO 26), geht von selbst aus
//  Mit Controller: Steuerkreuz bewegt einen Rahmen, A = antippen.
// ============================================================================
#pragma once

namespace Kit {
  enum EvType : uint8_t { EV_NONE, EV_TAP, EV_SWIPE };
  struct Ev { EvType t; int16_t x, y; int8_t dir; };     // dir: 0 hoch, 1 rechts, 2 runter, 3 links
  Ev ev{EV_NONE, 0, 0, -1};
  bool dirty = true;
  // Der Bildpuffer hat 256 Farben (je 3 Bit Rot/Gruen, 2 Bit Blau): sehr dunkle Toene werden schwarz.
  // Diese Farben bleiben sichtbar:
  const uint16_t PANEL = RGB(36, 36, 85), PANEL2 = RGB(73, 73, 170), NAVY = RGB(0, 0, 85);
  bool padMode = false;          // zuletzt mit Controller bedient -> Rahmen zeigen
  uint32_t lastT = 0, acc = 0;

  // ---------------- Beruehrung ----------------
  bool down = false, blocked = false;
  int16_t sx, sy, lx, ly, qx, qy;
  uint8_t ns = 0;
  uint32_t t0 = 0;

  bool inCorner(int x, int y) { return touchActive() && x > SCR_W - tPauseW && y < tPauseH; }
  void block() { blocked = true; down = false; ev.t = EV_NONE; }

  void poll() {
    int x, y;
    const bool t = touchXY(x, y);
    if (blocked) { if (!t) blocked = false; return; }
    if (t) {
      if (!down) { down = true; ns = 0; t0 = millis(); }
      // Erster Messwert nach dem Aufsetzen ist beim resistiven Touch oft ungenau -> zweiten als Start nehmen
      if (ns <= 1) { sx = x; sy = y; }
      lx = qx; ly = qy;            // letzter Wert vor dem Loslassen wird verworfen (ebenfalls ungenau)
      qx = x; qy = y;
      if (ns < 255) ns++;
      return;
    }
    if (!down) return;
    down = false;
    if (ns <= 2) { lx = qx; ly = qy; }
    if (inCorner(sx, sy)) return;                        // Ecke oben rechts = Menue (touchKeys)
    const int dx = lx - sx, dy = ly - sy;
    if (max(abs(dx), abs(dy)) >= 30) {
      ev = {EV_SWIPE, sx, sy, (int8_t)(abs(dx) > abs(dy) ? (dx > 0 ? 1 : 3) : (dy > 0 ? 2 : 0))};
    } else if (millis() - t0 < 1200) {
      ev = {EV_TAP, sx, sy, -1};
    }
    padMode = false;
    dirty = true;
  }

  Ev take() { Ev e = ev; ev.t = EV_NONE; return e; }
  bool anyPad() { return E.up || E.down || E.left || E.right || E.a || E.b; }

  // ---------------- Takt + Zeichnen ----------------
  void begin() {
    ev.t = EV_NONE; dirty = true; padMode = connectedPads() > 0;
    lastT = millis(); acc = 0; tick = 0;
    tPauseW = 52; tPauseH = 40;
    block();
  }
  void redraw() { dirty = true; lastT = millis(); acc = 0; }

  void step(void (*update)(), void (*render)()) {
    poll();
    const uint32_t now = millis();
    acc += now - lastT;
    lastT = now;
    if (acc > 200) acc = 200;
    while (acc >= TICK_MS) {
      acc -= TICK_MS;
      if (anyPad()) { if (!padMode) dirty = true; padMode = true; }
      update();
      tick++;
      clearEdges();
      if (state != ST_GAME || paused) return;
    }
    if (!dirty || !fbOk) return;
    dirty = false;
    for (int y0 = 0; y0 < SCR_H; y0 += fbBandH) {
      cvY0 = y0;
      canvas.fillScreen(C_BG);
      render();
      canvas.pushSprite(0, y0);
    }
  }

  // im Spiel: Pause-Menue / Halte-Menue; true = Spiel pausiert
  bool pause() {
    if (handlePause(redraw)) { block(); return true; }
    return false;
  }

  // ---------------- Highscore ohne Spielende ----------------
  void hsSet(uint32_t v) {
    if (v > highscore[curGame]) { highscore[curGame] = v; saveHighscores(); }
  }

  // ---------------- Zeichnen ----------------
  struct Btn { int16_t x, y, w, h; };
  bool hit(const Btn& b, int x, int y) { return b.w > 0 && x >= b.x && x < b.x + b.w && y >= b.y && y < b.y + b.h; }

  inline void cRRO(int x, int y, int w, int h, int r, uint16_t c) {
    if (cVis(y, h)) canvas.drawRoundRect(x, y - cvY0, w, h, r, c);
  }

  // groesste Schrift (3, 2, 1), mit der der Text in w Pixel passt
  float fit(const char* s, int w, float maxS = 3) {
    const int n = strlen(s);
    for (float t = maxS; t > 1; t -= 1) if (n * 6 * t <= w) return t;
    if (maxS >= 1.5f && n * 9 <= w) return 1.5f;
    return 1;
  }

  void btn(const Btn& b, const char* s, uint16_t bg, uint16_t fg, bool sel = false, float maxS = 2) {
    if (b.w <= 0) return;
    cRR(b.x, b.y, b.w, b.h, 6, bg);
    if (sel) { cRRO(b.x, b.y, b.w, b.h, 6, C_WHITE); cRRO(b.x + 1, b.y + 1, b.w - 2, b.h - 2, 5, C_WHITE); }
    const float t = fit(s, b.w - 8, maxS);
    cTxt(s, b.x + b.w / 2, b.y + b.h / 2 + 1, t, fg);
  }

  // Pause-Zeichen oben rechts (nur ohne Controller, dort liegt das Menue)
  void pauseIcon() {
    if (connectedPads() > 0) return;
    const int x = SCR_W - 20, y = max(4, tPauseH / 2 - 7);
    cR(x, y, 4, 13, C_GRAY);
    cR(x + 7, y, 4, 13, C_GRAY);
  }

  // Hinweisfenster in der Bildmitte
  void panel(const char* t1, const char* t2, uint16_t col, const char* t3 = "Antippen = weiter") {
    const int w = min(270, SCR_W - 24), h = 92, x = (SCR_W - w) / 2, y = (SCR_H - h) / 2;
    cRR(x, y, w, h, 10, PANEL);
    cRRO(x, y, w, h, 10, col);
    cTxt(t1, SCR_W / 2, y + 24, fit(t1, w - 16, 2), col);
    if (t2) cTxt(t2, SCR_W / 2, y + 52, fit(t2, w - 16, 2), C_WHITE);
    if (t3) cTxt(t3, SCR_W / 2, y + 76, 1, C_GRAY);
  }

  // Stern (gefuellt), r = Aussenradius
  void star(int cx, int cy, int r, uint16_t c) {
    int px[10], py[10];
    for (int i = 0; i < 10; i++) {
      const float a = -PI / 2 + i * PI / 5;
      const float rr = (i & 1) ? r * 0.45f : r;
      px[i] = cx + (int)lroundf(rr * cosf(a));
      py[i] = cy + (int)lroundf(rr * sinf(a));
    }
    for (int i = 0; i < 10; i++) cTri(cx, cy, px[i], py[i], px[(i + 1) % 10], py[(i + 1) % 10], c);
  }

  // Pixelbild aus Text: je Zeichen ein Pixel ('.' = durchsichtig), Farbe ueber Tabelle
  uint16_t palCol(char ch) {
    switch (ch) {
      case 'k': return RGB(25, 25, 35);   case 'w': return C_WHITE;          case 'r': return RGB(230, 40, 40);
      case 'R': return RGB(150, 20, 30);  case 'o': return RGB(255, 140, 0); case 'y': return RGB(255, 220, 0);
      case 'g': return RGB(40, 200, 70);  case 'G': return RGB(20, 120, 40); case 'b': return RGB(40, 100, 255);
      case 'B': return RGB(20, 50, 150);  case 'c': return RGB(0, 210, 230); case 'p': return RGB(255, 110, 190);
      case 'v': return RGB(160, 70, 230); case 'n': return RGB(140, 80, 30); case 'N': return RGB(90, 50, 20);
      case 'e': return RGB(150, 150, 165); case 'E': return RGB(80, 80, 95); case 's': return RGB(255, 200, 150);
      default:  return C_WHITE;
    }
  }
  // body = Ersatzfarbe fuer 'X' (z. B. Autolack), dark = fuer 'x'
  void pix(const char* const* rows, int w, int h, int x, int y, int s, uint16_t body = C_RED, uint16_t dark = C_DIM) {
    if (!cVis(y, h * s)) return;
    for (int r = 0; r < h; r++) {
      const char* row = rows[r];
      int c = 0;
      while (c < w) {
        const char ch = row[c];
        if (ch == '.') { c++; continue; }
        int e = c + 1;
        while (e < w && row[e] == ch) e++;
        const uint16_t col = ch == 'X' ? body : (ch == 'x' ? dark : palCol(ch));
        canvas.fillRect(x + c * s, y + r * s - cvY0, (e - c) * s, s, col);
        c = e;
      }
    }
  }

  // ---------------- Auswahl (Spielart) ----------------
  struct Picker {
    const char* title = "";
    const char* items[6];
    const char* note = nullptr;
    uint8_t n = 0, sel = 0;
    void set(const char* t) { title = t; n = 0; sel = 0; note = nullptr; }
    void add(const char* s) { if (n < 6) items[n++] = s; }
    Btn box(int i) const {
      const int top = landscape() ? 46 : 70;
      const int gap = 6;
      const int bh = min(40, (SCR_H - top - 22) / n - gap);
      const int w = min(250, SCR_W - 40);
      return {(int16_t)((SCR_W - w) / 2), (int16_t)(top + i * (bh + gap)), (int16_t)w, (int16_t)bh};
    }
    // -1 = noch nichts, sonst gewaehlter Punkt. B / View = zurueck zum Spielemenue.
    int update() {
      const Ev e = take();
      if (e.t == EV_TAP) for (int i = 0; i < n; i++) if (hit(box(i), e.x, e.y)) { sel = i; dirty = true; Snd::fx(Snd::SELECT); return i; }
      if (E.up)   { sel = (sel + n - 1) % n; dirty = true; Snd::fx(Snd::MOVE, Snd::P_AUTO); }
      if (E.down) { sel = (sel + 1) % n; dirty = true; Snd::fx(Snd::MOVE, Snd::P_AUTO); }
      if (E.a || E.start) { Snd::fx(Snd::SELECT); return sel; }
      if (E.b) { toMenu(); return -1; }
      return -1;
    }
    void render() const {
      drawStripesC(4);
      cTxt(title, SCR_W / 2, landscape() ? 30 : 46, fit(title, SCR_W - 70, 2), C_YELLOW);
      for (int i = 0; i < n; i++) btn(box(i), items[i], (padMode && i == sel) ? C_BLUE : PANEL, C_WHITE, padMode && i == sel);
      if (note) cTxt(note, SCR_W / 2, SCR_H - 10, 1, C_GRAY);
      pauseIcon();
    }
    static void drawStripesC(int y0) {
      const uint16_t st[] = {C_RED, C_ORANGE, C_YELLOW, C_GREEN, C_CYAN, C_PURPLE};
      for (int i = 0; i < 6; i++) cR(0, y0 + i * 2, SCR_W, 1, st[i]);
    }
  };

  // ---------------- Ton (Lautsprecher-Anschluss, GPIO 26) ----------------
  // Dauerton (z. B. Motor) ueber die Ton-Engine (fx_snd.h); Effekte haben Vorrang
  bool sndOn = false;
  uint32_t sndAt = 0;
  void tone(uint32_t f) {
    if (!Snd::on) return;
    sfx::motor((uint16_t)f);
    sndOn = true;
    sndAt = millis();
  }
  void silence() { if (sndOn) { sfx::motor(0); sndOn = false; } }
  // aus loop(): Ton aus, wenn ihn niemand mehr anfordert (Pause, Menue, Spielende)
  void soundService() { if (sndOn && (millis() - sndAt > 120 || state != ST_GAME || paused)) silence(); }
}
