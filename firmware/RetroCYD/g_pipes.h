// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  ROHRPOST  -  Rohrstuecke drehen, bis alle Empfaenger an der Station haengen
// ============================================================================
//  Antippen dreht ein Feld um 90 Grad. Verbundene Rohre faerben sich ein.
//  Jedes Raetsel ist ein zufaelliger Spannbaum (immer loesbar), der danach
//  verdreht wird - unbegrenzt viele Raetsel. Spielarten 6 x 5 und 8 x 6 (fein,
//  am besten mit Stift). Punkte je Raetsel: Grundwert + Zeitbonus.
//  Controller: Steuerkreuz waehlt, A = rechts drehen, B = links drehen.
// ============================================================================
#pragma once

namespace Pipes {
  enum Phase : uint8_t { PH_PICK, PH_PLAY, PH_DONE };
  Phase ph;
  Kit::Picker pick;
  char lbl[2][28];

  const uint8_t U = 1, R = 2, D = 4, L = 8;
  uint8_t mode = 0, W = 6, H = 5;
  uint8_t pipe[48];          // aktuelle Ausrichtung (Bits U R D L)
  bool on[48];               // mit der Station verbunden
  uint8_t src;
  uint16_t moves, level, nOn;
  uint32_t score, lastGain;
  uint32_t t0, tSolved;
  uint8_t curX, curY, flashI = 255, flashT;
  int CS, BX, BY;
  Kit::Btn bNew;
  int lastMode = -1;

  const char* lvKey(int m) { return m ? "rp_lv1" : "rp_lv0"; }

  uint8_t rotCW(uint8_t b) { return (uint8_t)(((b << 1) | (b >> 3)) & 15); }

  void flood() {
    memset(on, 0, sizeof(on));
    uint8_t q[48];
    int qh = 0, qt = 0;
    q[qt++] = src;
    on[src] = true;
    while (qh < qt) {
      const int i = q[qh++], x = i % W, y = i / W;
      const uint8_t b = pipe[i];
      auto go = [&](uint8_t dir, int nx, int ny, uint8_t back) {
        if (!(b & dir) || nx < 0 || ny < 0 || nx >= W || ny >= H) return;
        const int j = ny * W + nx;
        if (on[j] || !(pipe[j] & back)) return;
        on[j] = true;
        q[qt++] = j;
      };
      go(U, x, y - 1, D); go(D, x, y + 1, U); go(L, x - 1, y, R); go(R, x + 1, y, L);
    }
    nOn = qt;
  }

  // zufaelliger Spannbaum (Prim): viele Abzweigungen, immer genau ein Weg zu jedem Feld
  void generate() {
    const int n = W * H;
    memset(pipe, 0, sizeof(pipe));
    bool in[48] = {false};
    struct Edge { uint8_t cell, dir; };
    Edge fr[48 * 4];
    int nf = 0;
    auto addEdges = [&](int i) {
      const int x = i % W, y = i / W;
      if (y > 0)     fr[nf++] = {(uint8_t)i, U};
      if (y < H - 1) fr[nf++] = {(uint8_t)i, D};
      if (x > 0)     fr[nf++] = {(uint8_t)i, L};
      if (x < W - 1) fr[nf++] = {(uint8_t)i, R};
    };
    src = (H / 2) * W + (W - 1) / 2;
    in[src] = true;
    addEdges(src);
    int cnt = 1;
    while (cnt < n && nf > 0) {
      const int k = irand(0, nf - 1);
      const Edge e = fr[k];
      fr[k] = fr[--nf];
      const int x = e.cell % W, y = e.cell / W;
      const int nx = x + (e.dir == R) - (e.dir == L), ny = y + (e.dir == D) - (e.dir == U);
      const int j = ny * W + nx;
      if (in[j]) continue;
      // Kreuzungen (4 Anschluesse) selten halten: sonst wird es zu einfach
      if (__builtin_popcount(pipe[e.cell]) >= 3 && nf > 0 && irand(0, 2)) { fr[nf++] = e; continue; }
      const uint8_t back = e.dir == U ? D : e.dir == D ? U : e.dir == L ? R : L;
      pipe[e.cell] |= e.dir;
      pipe[j] |= back;
      in[j] = true;
      cnt++;
      addEdges(j);
    }
    // verdrehen, bis es nicht mehr geloest ist
    do {
      for (int i = 0; i < n; i++) { const int r = irand(0, 3); for (int k = 0; k < r; k++) pipe[i] = rotCW(pipe[i]); }
      flood();
    } while (nOn == n);
  }

  void layout() {
    if (landscape()) {
      CS = min((SCR_W - 60) / W, (SCR_H - 8) / H);
      BX = 4; BY = (SCR_H - CS * H) / 2;
      const int px = BX + CS * W + 5;
      bNew = {(int16_t)px, (int16_t)(SCR_H - 30), (int16_t)(SCR_W - px - 3), 26};
    } else {
      CS = min((SCR_W - 8) / W, (SCR_H - 80) / H);
      BX = (SCR_W - CS * W) / 2; BY = 34;
      bNew = {(int16_t)(SCR_W - 80), (int16_t)(SCR_H - 32), 72, 26};
    }
  }

  void newPuzzle() {
    generate();
    moves = 0;
    t0 = millis();
    curX = src % W; curY = src / W;
    ph = PH_PLAY;
    Kit::dirty = true;
  }

  void startMode(int m) {
    mode = m;
    W = m ? 8 : 6;
    H = m ? 6 : 5;
    level = prefs.getUShort(lvKey(m), 1);
    score = 0;
    layout();
    newPuzzle();
  }

  void start() {
    Kit::begin();
    tPauseW = 40; tPauseH = 30;
    if (gameRestart && lastMode >= 0) { startMode(lastMode); return; }
    pick.set("ROHRPOST");
    snprintf(lbl[0], sizeof(lbl[0]), "6 x 5  (Nr. %u)", prefs.getUShort(lvKey(0), 1));
    snprintf(lbl[1], sizeof(lbl[1]), "8 x 6 FEIN (Nr. %u)", prefs.getUShort(lvKey(1), 1));
    pick.add(lbl[0]); pick.add(lbl[1]);
    pick.note = "Alle Empfaenger anschliessen";
    ph = PH_PICK;
  }

  uint32_t secs() { return ((ph == PH_DONE ? tSolved : millis()) - t0) / 1000; }

  void rotate(int i, bool cw) {
    if (cw) pipe[i] = rotCW(pipe[i]);
    else for (int k = 0; k < 3; k++) pipe[i] = rotCW(pipe[i]);
    moves++;
    flashI = i; flashT = 4;
    flood();
    Kit::dirty = true;
    if (nOn == W * H) {
      tSolved = millis();
      const uint32_t s = secs();
      const uint32_t base = mode ? 150 : 100, lim = mode ? 240 : 150;
      lastGain = base + (s < lim ? (lim - s) * 2 : 0);
      score += lastGain;
      level++;
      prefs.putUShort(lvKey(mode), level);
      Kit::hsSet(score);
      ph = PH_DONE;
      rumble(250, 0x90);
    }
  }

  uint32_t lastSec = 0;
  void update() {
    if (ph == PH_PICK) {
      const int c = pick.update();
      if (c >= 0) { lastMode = c; startMode(c); }
      return;
    }
    if (flashT) { flashT--; Kit::dirty = true; }
    if (ph == PH_PLAY && secs() != lastSec) { lastSec = secs(); Kit::dirty = true; }
    const Kit::Ev e = Kit::take();
    if (ph == PH_DONE) {
      if (e.t == Kit::EV_TAP || E.a || E.start) newPuzzle();
      return;
    }
    if (E.up && curY > 0) { curY--; Kit::dirty = true; }
    if (E.down && curY < H - 1) { curY++; Kit::dirty = true; }
    if (E.left && curX > 0) { curX--; Kit::dirty = true; }
    if (E.right && curX < W - 1) { curX++; Kit::dirty = true; }
    if (E.a) rotate(curY * W + curX, true);
    if (E.b) rotate(curY * W + curX, false);
    if (e.t != Kit::EV_TAP) return;
    if (Kit::hit(bNew, e.x, e.y)) { newPuzzle(); return; }
    const int x = (e.x - BX) / CS, y = (e.y - BY) / CS;
    if (e.x >= BX && e.y >= BY && x < W && y < H) { curX = x; curY = y; rotate(y * W + x, true); }
  }

  // Rohr aus Rechtecken: Arme zur Mitte, dunkler Rand
  void drawPipe(int i, int x, int y) {
    const uint8_t b = pipe[i];
    const bool lit = on[i];
    const bool done = ph == PH_DONE;
    const int c = CS / 2, w = max(6, CS / 4), h = w / 2;
    const uint16_t edge = RGB(15, 15, 25);
    const uint16_t col = !lit ? RGB(95, 95, 115) : (done ? RGB(255, 205, 40) : RGB(0, 200, 255));
    for (int pass = 0; pass < 2; pass++) {
      const int o = pass ? 0 : 2;                          // Rand: 2 Pixel breiter
      const uint16_t k = pass ? col : edge;
      if (b & U) cR(x + c - h - o, y, w + 2 * o, c + h + o, k);
      if (b & D) cR(x + c - h - o, y + c - h - o, w + 2 * o, CS - c + h + o, k);
      if (b & L) cR(x, y + c - h - o, c + h + o, w + 2 * o, k);
      if (b & R) cR(x + c - h - o, y + c - h - o, CS - c + h + o, w + 2 * o, k);
      if (pass == 0) cRR(x + c - h - 2, y + c - h - 2, w + 4, w + 4, 2, k);
      else cR(x + c - h, y + c - h, w, w, k);
    }
    if (i == src) {                                         // Station
      cRR(x + c - CS / 3, y + c - CS / 3, 2 * (CS / 3), 2 * (CS / 3), 4, RGB(200, 40, 40));
      cRR(x + c - CS / 3 + 3, y + c - CS / 3 + 3, 2 * (CS / 3) - 6, 2 * (CS / 3) - 6, 3, RGB(255, 90, 60));
      cC(x + c, y + c, max(2, CS / 10), C_YELLOW);
    } else if (__builtin_popcount(b) == 1) {               // Empfaenger mit Lampe
      const int s = CS * 2 / 5;
      cRR(x + c - s / 2, y + c - s / 2, s, s, 3, RGB(182, 182, 170));
      cC(x + c, y + c, max(2, s / 4), lit ? (done ? C_YELLOW : C_GREEN) : RGB(160, 30, 30));
    }
  }

  void render() {
    if (ph == PH_PICK) { pick.render(); return; }
    for (int yy = 0; yy < H; yy++)
      for (int xx = 0; xx < W; xx++) {
        const int i = yy * W + xx, x = BX + xx * CS, y = BY + yy * CS;
        cR(x, y, CS, CS, ((xx + yy) & 1) ? RGB(36, 36, 85) : RGB(0, 36, 85));
        if (flashT && flashI == i) cR(x, y, CS, CS, RGB(73, 73, 170));
        drawPipe(i, x, y);
        if (Kit::padMode && xx == curX && yy == curY) { cRO(x, y, CS, CS, C_WHITE); cRO(x + 1, y + 1, CS - 2, CS - 2, C_WHITE); }
      }
    char buf[24];
    const uint32_t s = secs();
    if (landscape()) {
      const int px = BX + CS * W + 5, pw = SCR_W - px - 3, cx = px + pw / 2;
      cTxt("RAETSEL", cx, 34, 1, C_GRAY);
      snprintf(buf, sizeof(buf), "%u", level);
      cTxt(buf, cx, 50, Kit::fit(buf, pw, 2), C_WHITE);
      cTxt("ZEIT", cx, 72, 1, C_GRAY);
      snprintf(buf, sizeof(buf), "%lu:%02lu", (unsigned long)(s / 60), (unsigned long)(s % 60));
      cTxt(buf, cx, 86, 1, C_WHITE);
      cTxt("ZUEGE", cx, 104, 1, C_GRAY);
      snprintf(buf, sizeof(buf), "%u", moves);
      cTxt(buf, cx, 118, 1, C_WHITE);
      cTxt("PUNKTE", cx, 136, 1, C_GRAY);
      snprintf(buf, sizeof(buf), "%lu", (unsigned long)score);
      cTxt(buf, cx, 150, 1, C_YELLOW);
      snprintf(buf, sizeof(buf), "%u/%u", nOn, W * H);
      cTxt(buf, cx, 172, 1, C_CYAN);
    } else {
      snprintf(buf, sizeof(buf), "RAETSEL %u   %lu:%02lu", level, (unsigned long)(s / 60), (unsigned long)(s % 60));
      cTxt(buf, 8, 10, 1.5, C_WHITE, textdatum_t::top_left);
      snprintf(buf, sizeof(buf), "Punkte %lu   %u/%u", (unsigned long)score, nOn, W * H);
      cTxt(buf, 8, SCR_H - 24, 1, C_YELLOW, textdatum_t::top_left);
    }
    Kit::btn(bNew, "NEU", Kit::PANEL, C_WHITE, false, 1.5);
    Kit::pauseIcon();
    if (ph == PH_DONE) {
      snprintf(buf, sizeof(buf), "+%lu Punkte (%lu s)", (unsigned long)lastGain, (unsigned long)secs());
      Kit::panel("ALLES ANGESCHLOSSEN", buf, C_YELLOW, "Antippen = naechstes Raetsel");
    }
  }

  uint32_t pausedAt = 0;
  void loop() {
    if (Kit::pause()) { if (!pausedAt) pausedAt = millis(); return; }
    if (pausedAt) { t0 += millis() - pausedAt; pausedAt = 0; }   // Pause zaehlt nicht zur Zeit
    Kit::step(update, render);
  }
}
