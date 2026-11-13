// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  LICHTER AUS  -  ein Tipp schaltet ein Feld und seine vier Nachbarn um,
//  am Ende muessen alle Lichter aus sein
// ============================================================================
//  Spielarten: 5 x 5, 6 x 6, drei Farben (jedes Feld wechselt aus -> blau ->
//  rot -> aus). Jedes Level entsteht durch Zufallszuege aus dem geloesten
//  Brett und ist damit immer loesbar. Sterne nach Zahl der Zuege; das Level
//  jeder Spielart wird gespeichert.
//  Controller: Steuerkreuz waehlt das Feld, A = umschalten, B = Level neu.
// ============================================================================
#pragma once

namespace Lights {
  enum Phase : uint8_t { PH_PICK, PH_PLAY, PH_DONE };
  Phase ph;
  Kit::Picker pick;
  char lbl[3][28];

  uint8_t mode = 0, N = 5, K = 2;      // K = Anzahl Zustaende je Feld
  uint8_t cell[36], startCell[36];
  uint16_t level, moves, par;
  uint8_t stars, curR, curC, flashI = 255, flashT = 0;
  int CS, BX, BY;
  Kit::Btn bReset;
  int lastMode = -1;

  struct Prog { uint16_t level, stars; };
  Prog prog[3];
  const char* key(int m) { static char k[8]; snprintf(k, sizeof(k), "lo_p%d", m); return k; }
  void loadProg() {
    for (int m = 0; m < 3; m++)
      if (prefs.getBytes(key(m), &prog[m], sizeof(Prog)) != sizeof(Prog) || prog[m].level == 0) prog[m] = {1, 0};
  }
  void saveProg() { prefs.putBytes(key(mode), &prog[mode], sizeof(Prog)); }
  uint32_t totalStars() { return (uint32_t)prog[0].stars + prog[1].stars + prog[2].stars; }

  void press(uint8_t* b, int r, int c) {
    const int8_t D[5][2] = {{0, 0}, {-1, 0}, {1, 0}, {0, -1}, {0, 1}};
    for (auto& d : D) {
      const int rr = r + d[0], cc = c + d[1];
      if (rr < 0 || cc < 0 || rr >= N || cc >= N) continue;
      uint8_t& v = b[rr * N + cc];
      v = (v + 1) % K;
    }
  }
  bool solved() { for (int i = 0; i < N * N; i++) if (cell[i]) return false; return true; }

  void genLevel() {
    do {
      memset(cell, 0, sizeof(cell));
      const int want = min(3 + (level - 1) / 2, N * N * 2 / 3);
      bool used[36] = {false};
      par = 0;
      for (int k = 0; k < want; k++) {
        int i;
        do i = irand(0, N * N - 1); while (used[i]);
        used[i] = true;
        // drei Farben: ein Feld 1- oder 2-mal druecken; Loesung = (K - Anzahl) mal druecken
        const int times = K == 3 ? irand(1, 2) : 1;
        for (int t = 0; t < times; t++) press(cell, i / N, i % N);
        par += (K - times) % K == 0 ? K : (K - times) % K;
      }
    } while (solved());
    memcpy(startCell, cell, sizeof(cell));
    moves = 0;
    flashT = 0;
    ph = PH_PLAY;
    Kit::dirty = true;
  }

  void layout() {
    if (landscape()) {
      CS = min((SCR_H - 10) / N, (SCR_W - 90) / N);
      BX = 5; BY = (SCR_H - CS * N) / 2;
      const int px = BX + CS * N + 6, pw = SCR_W - px - 4;
      bReset = {(int16_t)px, (int16_t)(SCR_H - 34), (int16_t)pw, 28};
    } else {
      CS = min((SCR_W - 10) / N, (SCR_H - 100) / N);
      BX = (SCR_W - CS * N) / 2; BY = 34;
      bReset = {(int16_t)(SCR_W - 96), (int16_t)(SCR_H - 34), 88, 28};
    }
  }

  void startMode(int m) {
    mode = m;
    N = m == 1 ? 6 : 5;
    K = m == 2 ? 3 : 2;
    level = prog[m].level;
    curR = curC = N / 2;
    layout();
    genLevel();
  }

  void start() {
    Kit::begin();
    tPauseW = 40; tPauseH = 30;
    loadProg();
    if (gameRestart && lastMode >= 0) { startMode(lastMode); return; }
    pick.set("LICHTER AUS");
    snprintf(lbl[0], sizeof(lbl[0]), "5 x 5   (LEVEL %u)", prog[0].level);
    snprintf(lbl[1], sizeof(lbl[1]), "6 x 6   (LEVEL %u)", prog[1].level);
    snprintf(lbl[2], sizeof(lbl[2]), "3 FARBEN (LEVEL %u)", prog[2].level);
    for (auto& l : lbl) pick.add(l);
    pick.note = "Alle Lichter ausschalten";
    ph = PH_PICK;
  }

  void tapCell(int r, int c) {
    press(cell, r, c);
    moves++;
    Snd::fx(Snd::TOGGLE, Snd::P_AUTO);
    flashI = r * N + c;
    flashT = 5;
    Kit::dirty = true;
    if (solved()) {
      stars = moves <= par ? 3 : (moves <= par + (par + 1) / 2 ? 2 : 1);
      prog[mode].stars += stars;
      prog[mode].level++;
      saveProg();
      Kit::hsSet(totalStars());
      ph = PH_DONE;
      Snd::fx(Snd::WIN, Snd::P_BIG);
      rumble(250, 0x90);
      Led::party(1500);
    }
  }

  void update() {
    if (ph == PH_PICK) {
      const int c = pick.update();
      if (c >= 0) { lastMode = c; startMode(c); }
      return;
    }
    if (flashT) { flashT--; Kit::dirty = true; }
    const Kit::Ev e = Kit::take();
    if (ph == PH_DONE) {
      if (e.t == Kit::EV_TAP || E.a || E.start) { level = prog[mode].level; genLevel(); }
      return;
    }
    if (E.up && curR > 0) { curR--; Kit::dirty = true; }
    if (E.down && curR < N - 1) { curR++; Kit::dirty = true; }
    if (E.left && curC > 0) { curC--; Kit::dirty = true; }
    if (E.right && curC < N - 1) { curC++; Kit::dirty = true; }
    if (E.a) { tapCell(curR, curC); return; }
    if (E.b) { memcpy(cell, startCell, sizeof(cell)); moves = 0; Kit::dirty = true; }
    if (e.t != Kit::EV_TAP) return;
    if (Kit::hit(bReset, e.x, e.y)) { memcpy(cell, startCell, sizeof(cell)); moves = 0; Kit::dirty = true; return; }
    const int c = (e.x - BX) / CS, r = (e.y - BY) / CS;
    if (e.x >= BX && e.y >= BY && r < N && c < N) { curR = r; curC = c; tapCell(r, c); }
  }

  uint16_t colOf(uint8_t v) {
    if (v == 0) return RGB(36, 36, 85);
    if (K == 3) return v == 1 ? RGB(0, 170, 255) : RGB(255, 60, 120);
    return RGB(255, 215, 40);
  }

  void render() {
    if (ph == PH_PICK) { pick.render(); return; }
    cRR(BX - 3, BY - 3, CS * N + 6, CS * N + 6, 8, Kit::NAVY);
    for (int r = 0; r < N; r++)
      for (int c = 0; c < N; c++) {
        const int i = r * N + c, x = BX + c * CS + 2, y = BY + r * CS + 2, s = CS - 4;
        const uint8_t v = cell[i];
        cRR(x, y, s, s, 6, colOf(v));
        if (v) {                                         // Lampe mit Glanzpunkt
          cRR(x + 4, y + 4, s - 8, s - 8, 5, K == 3 ? colOf(v) : RGB(255, 245, 150));
          cR(x + 7, y + 7, s / 5, 3, C_WHITE);
        } else {
          Kit::cRRO(x + 3, y + 3, s - 6, s - 6, 5, RGB(73, 73, 170));
        }
        if (flashT && flashI == i) Kit::cRRO(x - 1, y - 1, s + 2, s + 2, 7, C_WHITE);
        if (Kit::padMode && r == curR && c == curC) {
          Kit::cRRO(x - 2, y - 2, s + 4, s + 4, 7, C_CYAN);
          Kit::cRRO(x - 1, y - 1, s + 2, s + 2, 7, C_CYAN);
        }
      }
    char buf[24];
    int px, py, pw;
    if (landscape()) { px = BX + CS * N + 6; pw = SCR_W - px - 4; py = 8; }
    else { px = 8; pw = SCR_W - 16; py = BY + CS * N + 8; }
    const char* mn = mode == 0 ? "5 x 5" : (mode == 1 ? "6 x 6" : "3 FARBEN");
    if (landscape()) {
      cTxt(mn, px, py, 1, C_DIM, textdatum_t::top_left);
      cTxt("LEVEL", px, py + 22, 1, C_GRAY, textdatum_t::top_left);
      snprintf(buf, sizeof(buf), "%u", level);
      cTxt(buf, px, py + 34, 2, C_WHITE, textdatum_t::top_left);
      cTxt("ZUEGE", px, py + 62, 1, C_GRAY, textdatum_t::top_left);
      snprintf(buf, sizeof(buf), "%u", moves);
      cTxt(buf, px, py + 74, 2, moves <= par ? C_GREEN : C_ORANGE, textdatum_t::top_left);
      snprintf(buf, sizeof(buf), "Ziel %u", par);
      cTxt(buf, px, py + 96, 1, C_GRAY, textdatum_t::top_left);
      // Sterne fuer die aktuelle Zahl der Zuege
      const int st = moves <= par ? 3 : (moves <= par + (par + 1) / 2 ? 2 : 1);
      for (int k = 0; k < 3; k++) Kit::star(px + 10 + k * 22, py + 126, 9, k < st ? C_YELLOW : C_DIM);
      snprintf(buf, sizeof(buf), "%lu Sterne", (unsigned long)totalStars());
      cTxt(buf, px, py + 146, 1, C_YELLOW, textdatum_t::top_left);
    } else {
      snprintf(buf, sizeof(buf), "%s  LEVEL %u", mn, level);
      cTxt(buf, 8, 10, 1.5, C_WHITE, textdatum_t::top_left);
      snprintf(buf, sizeof(buf), "Zuege %u / Ziel %u", moves, par);
      cTxt(buf, px, py, 1.5, moves <= par ? C_GREEN : C_ORANGE, textdatum_t::top_left);
      snprintf(buf, sizeof(buf), "%lu Sterne", (unsigned long)totalStars());
      cTxt(buf, px, py + 20, 1, C_YELLOW, textdatum_t::top_left);
    }
    Kit::btn(bReset, "NOCHMAL", Kit::PANEL, C_WHITE, false, 1.5);
    Kit::pauseIcon();
    if (ph == PH_DONE) {
      snprintf(buf, sizeof(buf), "in %u Zuegen", moves);
      Kit::panel("GELOEST!", buf, C_YELLOW, "Antippen = naechstes Level");
      const int y = SCR_H / 2 + 46 + 16;
      cRR(SCR_W / 2 - 52, y - 15, 104, 30, 8, Kit::PANEL);
      Kit::cRRO(SCR_W / 2 - 52, y - 15, 104, 30, 8, C_YELLOW);
      for (int k = 0; k < 3; k++) Kit::star(SCR_W / 2 - 30 + k * 30, y, 12, k < stars ? C_YELLOW : C_DIM);
    }
  }

  void loop() {
    if (Kit::pause()) return;
    Kit::step(update, render);
  }
}
