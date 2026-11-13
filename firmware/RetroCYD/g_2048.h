// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  2048  -  gleiche Zahlenkacheln verschmelzen beim Schieben, Ziel ist die 2048
// ============================================================================
//  Touch: wischen (ab 30 Pixel) oder Pfeilfelder rechts. "ZURUECK" nimmt den
//  letzten Zug zurueck, "NEU" (zweimal tippen) beginnt neu.
//  Controller: Steuerkreuz schiebt, B = Zug zurueck.
//  Spielarten 4 x 4 und 5 x 5; das laufende Spiel wird im Flash gesichert und
//  kann spaeter fortgesetzt werden.
// ============================================================================
#pragma once

namespace G2048 {
  enum Phase : uint8_t { PH_PICK, PH_PLAY, PH_WON };
  Phase ph;
  Kit::Picker pick;
  char lbl[2][28];
  int8_t pickMode[3];

  uint8_t N = 4;
  uint8_t g[25], pg[25];             // Exponent: 0 = leer, 1 = 2, 2 = 4, ...
  uint32_t score, pscore;
  bool canUndo, keepGoing;
  int lastMode = -1;

  struct Mv { int8_t fr, fc, tr, tc; uint8_t v; };
  Mv mv[25];
  uint8_t nmv, anim, popT;
  uint8_t pop[25];                   // 1 = verschmolzen, 2 = neu
  const uint8_t ANIM = 4, POP = 4;

  int TS, BX, BY;
  Kit::Btn bUndo, bNew, bDir[4];
  uint32_t newArm = 0;               // "NEU" einmal getippt
  bool saveDirty = false;
  uint32_t saveAt = 0;

  struct Save { uint8_t magic, n, g[25]; uint32_t score; };
  const char* key(int n) { return n == 5 ? "s2048_5" : "s2048_4"; }

  bool loadSave(int n, Save& s) {
    if (prefs.getBytes(key(n), &s, sizeof(s)) != sizeof(s)) return false;
    return s.magic == 0x48 && s.n == n;
  }
  void writeSave() {
    Save s{0x48, N, {0}, score};
    memcpy(s.g, g, sizeof(g));
    prefs.putBytes(key(N), &s, sizeof(s));
    Kit::hsSet(score);
    saveDirty = false;
    saveAt = millis();
  }
  void clearSave() { prefs.remove(key(N)); saveDirty = false; }

  int idx(int r, int c) { return r * N + c; }
  // Feld Nr. k einer Reihe, gezaehlt von der Kante, zu der geschoben wird
  void cellAt(int dir, int line, int k, int& r, int& c) {
    switch (dir) {
      case 0: r = k;         c = line;      break;
      case 2: r = N - 1 - k; c = line;      break;
      case 3: r = line;      c = k;         break;
      default: r = line;     c = N - 1 - k; break;
    }
  }

  void spawn() {
    int empty[25], ne = 0;
    for (int i = 0; i < N * N; i++) if (!g[i]) empty[ne++] = i;
    if (!ne) return;
    const int i = empty[irand(0, ne - 1)];
    g[i] = irand(0, 9) == 0 ? 2 : 1;
    pop[i] = 2;
  }

  bool movesLeft() {
    for (int r = 0; r < N; r++)
      for (int c = 0; c < N; c++) {
        const uint8_t v = g[idx(r, c)];
        if (!v) return true;
        if (c + 1 < N && g[idx(r, c + 1)] == v) return true;
        if (r + 1 < N && g[idx(r + 1, c)] == v) return true;
      }
    return false;
  }

  bool doMove(int dir) {
    uint8_t ng[25] = {0};
    uint8_t np[25] = {0};
    uint32_t gain = 0;
    bool moved = false;
    uint8_t top = 0;                       // groesste neue Zahl (fuer die Tonhoehe)
    nmv = 0;
    for (int line = 0; line < N; line++) {
      int out = 0;
      uint8_t lastV = 0;
      bool lastMerged = true;
      for (int k = 0; k < N; k++) {
        int r, c;
        cellAt(dir, line, k, r, c);
        const uint8_t v = g[idx(r, c)];
        if (!v) continue;
        int tr, tc;
        if (out > 0 && v == lastV && !lastMerged) {
          cellAt(dir, line, out - 1, tr, tc);
          ng[idx(tr, tc)] = v + 1;
          np[idx(tr, tc)] = 1;
          gain += 1u << (v + 1);
          if (v + 1 > top) top = v + 1;
          lastMerged = true;
          moved = true;
        } else {
          cellAt(dir, line, out, tr, tc);
          ng[idx(tr, tc)] = v;
          if (tr != r || tc != c) moved = true;
          lastV = v;
          lastMerged = false;
          out++;
        }
        mv[nmv++] = {(int8_t)r, (int8_t)c, (int8_t)tr, (int8_t)tc, v};
      }
    }
    if (!moved) return false;
    memcpy(pg, g, sizeof(g));
    pscore = score;
    canUndo = true;
    memcpy(g, ng, sizeof(g));
    memcpy(pop, np, sizeof(pop));
    score += gain;
    spawn();
    anim = ANIM;
    popT = 0;
    saveDirty = true;
    if (top) Snd::pitch(300 + top * 90, 450 + top * 120, 90, sfx::DC);   // verschmolzen: je groesser, desto hoeher
    else Snd::fx(Snd::SLIDE, Snd::P_AUTO);
    if (gain >= 64) rumble(40, 0x50);
    return true;
  }

  void layout() {
    if (landscape()) {
      TS = N == 4 ? 56 : 45;
      BX = 6;
      BY = (SCR_H - TS * N) / 2;
      const int px = BX + TS * N + 6, pw = SCR_W - px - 4;
      bUndo = {(int16_t)px, 122, (int16_t)pw, 26};
      bNew  = {(int16_t)px, 152, (int16_t)pw, 22};
      const int ds = min(22, pw / 3), dx = px + (pw - 3 * ds) / 2, dy = SCR_H - 3 * ds - 3;
      bDir[0] = {(int16_t)(dx + ds), (int16_t)dy, (int16_t)ds, (int16_t)ds};
      bDir[1] = {(int16_t)(dx + 2 * ds), (int16_t)(dy + ds), (int16_t)ds, (int16_t)ds};
      bDir[2] = {(int16_t)(dx + ds), (int16_t)(dy + 2 * ds), (int16_t)ds, (int16_t)ds};
      bDir[3] = {(int16_t)dx, (int16_t)(dy + ds), (int16_t)ds, (int16_t)ds};
    } else {
      TS = min((SCR_W - 12) / N, (SCR_H - 112) / N);
      BX = (SCR_W - TS * N) / 2;
      BY = 30;
      const int y0 = BY + TS * N + 6, ds = min(22, (SCR_H - y0 - 2) / 3);
      const int dx = SCR_W - 3 * ds - 8, dy = y0 + max(0, (SCR_H - y0 - 3 * ds) / 2);
      bDir[0] = {(int16_t)(dx + ds), (int16_t)dy, (int16_t)ds, (int16_t)ds};
      bDir[1] = {(int16_t)(dx + 2 * ds), (int16_t)(dy + ds), (int16_t)ds, (int16_t)ds};
      bDir[2] = {(int16_t)(dx + ds), (int16_t)(dy + 2 * ds), (int16_t)ds, (int16_t)ds};
      bDir[3] = {(int16_t)dx, (int16_t)(dy + ds), (int16_t)ds, (int16_t)ds};
      bUndo = {8, (int16_t)(SCR_H - 30), 72, 26};
      bNew  = {86, (int16_t)(SCR_H - 30), 60, 26};
    }
  }

  void newGame(int n, bool resume) {
    N = n;
    layout();
    memset(g, 0, sizeof(g));
    memset(pop, 0, sizeof(pop));
    score = 0;
    canUndo = false;
    keepGoing = false;
    Save s;
    if (resume && loadSave(n, s)) {
      memcpy(g, s.g, sizeof(g));
      score = s.score;
      for (int i = 0; i < N * N; i++) if (g[i] >= 11) keepGoing = true;
    } else {
      spawn(); spawn();
      writeSave();
    }
    anim = 0;
    popT = 0;
    ph = PH_PLAY;
    Kit::dirty = true;
  }

  void buildPicker() {
    pick.set("2048");
    int k = 0;
    for (int n = 4; n <= 5; n++) {
      Save s;
      if (loadSave(n, s)) {
        snprintf(lbl[n - 4], sizeof(lbl[0]), "WEITER %dx%d (%lu)", n, n, (unsigned long)s.score);
        pick.add(lbl[n - 4]);
        pickMode[k++] = n + 10;      // fortsetzen
      }
    }
    pick.add("NEU 4 x 4"); pickMode[k++] = 4;
    pick.add("NEU 5 x 5"); pickMode[k++] = 5;
    pick.note = "Wischen = schieben   B = Zug zurueck";
  }

  void start() {
    Kit::begin();
    tPauseW = 40; tPauseH = 30;
    if (gameRestart && lastMode > 0) { newGame(lastMode, false); return; }
    buildPicker();
    ph = PH_PICK;
  }

  void tryMove(int dir) {
    if (anim || ph != PH_PLAY) return;
    newArm = 0;
    if (!doMove(dir)) return;
    Kit::dirty = true;
  }

  void afterAnim() {
    for (int i = 0; i < N * N; i++)
      if (g[i] >= 11 && !keepGoing) { keepGoing = true; ph = PH_WON; writeSave(); Snd::big(Snd::RECORD); rumble(300, 0xA0); Led::party(3000); return; }
    if (!movesLeft()) {
      clearSave();
      lastMode = N;
      gameOverText("KEIN ZUG MEHR", score);
    }
  }

  void update() {
    if (ph == PH_PICK) {
      const int c = pick.update();
      if (c >= 0) {
        const int m = pickMode[c];
        lastMode = m % 10;
        newGame(m % 10, m > 10);
      }
      return;
    }
    if (saveDirty && millis() - saveAt > 3000) writeSave();
    if (newArm && millis() - newArm > 2500) { newArm = 0; Kit::dirty = true; }
    if (anim) {
      anim--;
      Kit::dirty = true;
      if (!anim) { popT = POP; afterAnim(); }
      return;
    }
    if (popT) { popT--; Kit::dirty = true; }

    const Kit::Ev e = Kit::take();
    if (ph == PH_WON) {
      if (e.t == Kit::EV_TAP || E.a || E.start) { ph = PH_PLAY; Kit::dirty = true; }
      return;
    }
    if (E.up) tryMove(0);
    else if (E.right) tryMove(1);
    else if (E.down) tryMove(2);
    else if (E.left) tryMove(3);
    if (E.b && canUndo) { memcpy(g, pg, sizeof(g)); score = pscore; canUndo = false; memset(pop, 0, sizeof(pop)); saveDirty = true; Kit::dirty = true; }
    if (e.t == Kit::EV_SWIPE) { tryMove(e.dir); return; }
    if (e.t != Kit::EV_TAP) return;
    for (int d = 0; d < 4; d++) if (Kit::hit(bDir[d], e.x, e.y)) { tryMove(d); return; }
    if (Kit::hit(bUndo, e.x, e.y) && canUndo) {
      memcpy(g, pg, sizeof(g)); score = pscore; canUndo = false;
      memset(pop, 0, sizeof(pop)); saveDirty = true; Kit::dirty = true;
      return;
    }
    if (Kit::hit(bNew, e.x, e.y)) {
      if (newArm) { newArm = 0; clearSave(); newGame(N, false); }
      else newArm = millis();
      Kit::dirty = true;
    }
  }

  // ---------------- Zeichnen ----------------
  uint16_t tileCol(uint8_t e) {
    static const uint16_t C[] = {
      RGB(36, 36, 85),   RGB(109, 109, 170), RGB(40, 120, 200), RGB(20, 170, 205), RGB(30, 190, 120),
      RGB(120, 200, 40), RGB(220, 200, 20), RGB(250, 150, 20), RGB(245, 90, 30),  RGB(230, 40, 70),
      RGB(210, 40, 150), RGB(255, 215, 0),  RGB(160, 60, 230), RGB(100, 60, 255), RGB(255, 255, 255)};
    return C[e > 14 ? 14 : e];
  }

  void tile(int cx, int cy, uint8_t e, float sc) {
    const int s = (int)((TS - 6) * sc);
    if (s < 4) return;
    const uint16_t col = tileCol(e);
    cRR(cx - s / 2, cy - s / 2, s, s, max(3, s / 8), col);
    if (e >= 11) Kit::cRRO(cx - s / 2, cy - s / 2, s, s, max(3, s / 8), C_WHITE);
    char buf[8];
    snprintf(buf, sizeof(buf), "%lu", 1ul << e);
    const float t = Kit::fit(buf, s - 4, 3);
    const uint16_t fg = (e == 11 || e == 6 || e == 14) ? RGB(40, 30, 10) : C_WHITE;
    cTxt(buf, cx, cy + 1, t, fg);
  }

  void cellCenter(int r, int c, int& x, int& y) { x = BX + c * TS + TS / 2; y = BY + r * TS + TS / 2; }

  void render() {
    if (ph == PH_PICK) { pick.render(); return; }
    // Brett
    cRR(BX - 3, BY - 3, TS * N + 6, TS * N + 6, 8, Kit::NAVY);
    for (int r = 0; r < N; r++)
      for (int c = 0; c < N; c++) {
        int x, y;
        cellCenter(r, c, x, y);
        cRR(x - (TS - 6) / 2, y - (TS - 6) / 2, TS - 6, TS - 6, 5, tileCol(0));
      }
    if (anim) {                                        // Kacheln gleiten
      const float t = 1.0f - (float)anim / ANIM;
      for (int i = 0; i < nmv; i++) {
        int x0, y0, x1, y1;
        cellCenter(mv[i].fr, mv[i].fc, x0, y0);
        cellCenter(mv[i].tr, mv[i].tc, x1, y1);
        tile(x0 + (int)((x1 - x0) * t), y0 + (int)((y1 - y0) * t), mv[i].v, 1.0f);
      }
    } else {
      for (int r = 0; r < N; r++)
        for (int c = 0; c < N; c++) {
          const int i = idx(r, c);
          if (!g[i]) continue;
          int x, y;
          cellCenter(r, c, x, y);
          float sc = 1.0f;
          if (popT && pop[i] == 1) sc = 1.0f + 0.10f * sinf(PI * popT / POP);
          if (popT && pop[i] == 2) sc = 1.0f - 0.6f * popT / POP;
          tile(x, y, g[i], sc);
        }
    }
    // Seitenleiste
    char buf[16];
    if (landscape()) {
      const int px = BX + TS * N + 6, pw = SCR_W - px - 4, cx = px + pw / 2;
      cTxt("PUNKTE", px, 36, 1, C_GRAY, textdatum_t::top_left);
      snprintf(buf, sizeof(buf), "%lu", (unsigned long)score);
      cTxt(buf, cx, 54, Kit::fit(buf, pw, 2), C_WHITE);
      cTxt("BESTWERT", px, 76, 1, C_GRAY, textdatum_t::top_left);
      snprintf(buf, sizeof(buf), "%lu", (unsigned long)max(score, highscore[curGame]));
      cTxt(buf, cx, 94, Kit::fit(buf, pw, 2), C_YELLOW);
      cTxt(N == 4 ? "4 x 4" : "5 x 5", px, 6, 1, C_DIM, textdatum_t::top_left);
    } else {
      cTxt("PUNKTE", 8, 4, 1, C_GRAY, textdatum_t::top_left);
      snprintf(buf, sizeof(buf), "%lu", (unsigned long)score);
      cTxt(buf, 8, 14, 1.5, C_WHITE, textdatum_t::top_left);
      cTxt("BEST", 110, 4, 1, C_GRAY, textdatum_t::top_left);
      snprintf(buf, sizeof(buf), "%lu", (unsigned long)max(score, highscore[curGame]));
      cTxt(buf, 110, 14, 1.5, C_YELLOW, textdatum_t::top_left);
    }
    Kit::btn(bUndo, "ZURUECK", canUndo ? C_BLUE : Kit::PANEL, canUndo ? C_WHITE : C_DIM, false, 1);
    Kit::btn(bNew, newArm ? "SICHER?" : "NEU", newArm ? C_RED : Kit::PANEL, C_WHITE, false, 1);
    for (int d = 0; d < 4; d++) {
      const Kit::Btn& b = bDir[d];
      cRR(b.x + 1, b.y + 1, b.w - 2, b.h - 2, 4, Kit::PANEL);
      const int cx = b.x + b.w / 2, cy = b.y + b.h / 2, a = b.w / 4;
      if (d == 0) cTri(cx, cy - a, cx - a, cy + a / 2, cx + a, cy + a / 2, C_CYAN);
      if (d == 2) cTri(cx, cy + a, cx - a, cy - a / 2, cx + a, cy - a / 2, C_CYAN);
      if (d == 3) cTri(cx - a, cy, cx + a / 2, cy - a, cx + a / 2, cy + a, C_CYAN);
      if (d == 1) cTri(cx + a, cy, cx - a / 2, cy - a, cx - a / 2, cy + a, C_CYAN);
    }
    Kit::pauseIcon();
    if (ph == PH_WON) Kit::panel("2048 GESCHAFFT!", "Weiter zur 4096?", C_YELLOW, "Antippen = weiterspielen");
  }

  void loop() {
    if (Kit::pause()) { if (saveDirty) writeSave(); return; }
    Kit::step(update, render);
  }
}
