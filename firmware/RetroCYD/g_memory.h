// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  PAARE FINDEN  -  verdeckte Karten paarweise aufdecken
// ============================================================================
//  24 Karten (6 x 4), 12 Paare. Spielarten: 1 Spieler (moeglichst wenige
//  Zuege), auf Zeit (90 Sekunden), 2 Spieler abwechselnd am selben Geraet
//  (wer ein Paar findet, ist noch einmal dran).
//  Symbolsaetze: BILDER, ab 3 gewonnenen Spielen FORMEN, ab 6 ZAHLEN.
//  Controller: Steuerkreuz waehlt die Karte, A = aufdecken.
// ============================================================================
#pragma once

namespace Memory {
  enum Phase : uint8_t { PH_PICK, PH_PLAY, PH_DONE };
  Phase ph;
  Kit::Picker pick;
  char setLbl[32], pickNote[40];

  // 12 eigene Pixelbilder (16 x 16, doppelt gezeichnet = 32 x 32), Farben: Kit::palCol
  const char* const SYM[12][16] = {
    {  // HERZ
      "................",
      "...rrr....rrr...",
      "..rrrrr..rrrrr..",
      ".rrwwrrrrrrrrrr.",
      ".rrwrrrrrrrrrrr.",
      ".rrrrrrrrrrrrrr.",
      ".rrrrrrrrrrrrrr.",
      "..rrrrrrrrrrrr..",
      "...rrrrrrrrrr...",
      "....rrrrrrrr....",
      ".....rrrrrr.....",
      "......rrrr......",
      ".......rr.......",
      "................",
      "................",
      "................"
    },
    {  // STERN
      ".......yy.......",
      ".......yy.......",
      "......yyyy......",
      "......yyyy......",
      "yyyyyyyyyyyyyyyy",
      ".yyyyyywyyyyyyy.",
      "..yyyyywyyyyyy..",
      "...yyyyyyyyyy...",
      "....yyyyyyyy....",
      "....yyyyyyyy....",
      "...yyyyyyyyyy...",
      "...yyyy..yyyy...",
      "..yyyy....yyyy..",
      "..yyy......yyy..",
      ".yy..........yy.",
      "................"
    },
    {  // SONNE
      ".......oo.......",
      "..o....oo....o..",
      "...o........o...",
      ".....yyyyyy.....",
      "....yyyyyyyy....",
      "...yyyyyyyyyy...",
      "oo.yyyyyyyyyy.oo",
      "oo.yyyyyyyyyy.oo",
      "...yyyyyyyyyy...",
      "...yyyyyyyyyy...",
      "....yyyyyyyy....",
      ".....yyyyyy.....",
      "...o........o...",
      "..o....oo....o..",
      ".......oo.......",
      "................"
    },
    {  // MOND
      ".....cccc.......",
      "...ccccc........",
      "..cccccc........",
      ".ccccccc........",
      ".cccccc.........",
      "cccccccc........",
      "ccccccc.........",
      "ccccccc.........",
      "cccccccc........",
      "cccccccc.......c",
      ".ccccccccc....cc",
      ".cccccccccccccc.",
      "..cccccccccccc..",
      "...cccccccccc...",
      ".....cccccc.....",
      "................"
    },
    {  // APFEL
      "........n.......",
      ".......n..gg....",
      ".......n.ggg....",
      "...rrrr.nrrr....",
      "..rrrrrrrrrrrr..",
      ".rrwrrrrrrrrrrr.",
      ".rwrrrrrrrrrrrr.",
      ".rrrrrrrrrrrrrr.",
      ".rrrrrrrrrrrrrr.",
      ".rrrrrrrrrrrrrr.",
      ".rrrrrrrrrrrrrr.",
      "..rrrrrrrrrrrr..",
      "..rrrrrrrrrrrr..",
      "...rrrr..rrrr...",
      "................",
      "................"
    },
    {  // FISCH
      "................",
      "................",
      "......bbbb......",
      "....bbbbbbbb....",
      "..bbbbbbbbbbbb.b",
      ".bbwkbbbbbbbbbbb",
      "bbbkkbbbcbbbbbbb",
      "bbbbbbbbbcbbbbb.",
      "bbbbbbbbbcbbbbb.",
      ".bbbbbbbcbbbbbbb",
      "..bbbbbbbbbbbb.b",
      "....bbbbbbbb....",
      "......bbbb......",
      "................",
      "................",
      "................"
    },
    {  // PILZ
      "................",
      ".....rrrrrr.....",
      "...rrrrwwrrrr...",
      "..rrwwrrrrrrrr..",
      ".rrrwwrrrrwwrrr.",
      ".rrrrrrrrrwwrrr.",
      "rrwwrrrrrrrrrrrr",
      "rrwwrrrrwwrrrrrr",
      "rrrrrrrrwwrrrrrr",
      ".....wwwwww.....",
      ".....wwwwww.....",
      ".....wwkwkw.....",
      ".....wwwwww.....",
      ".....wwwwww.....",
      "......wwww......",
      "................"
    },
    {  // SCHLUESSEL
      "................",
      "...yyyy.........",
      "..yyyyyy........",
      ".yy....yy.......",
      ".yy....yy.......",
      ".yy....yy.......",
      "..yyyyyy........",
      "...yyyyy........",
      "......yyy.......",
      ".......yyy......",
      "........yyy.....",
      ".........yyyyy..",
      "..........yyy...",
      "...........yyy..",
      "............yy..",
      "................"
    },
    {  // GLOCKE
      ".......oo.......",
      "......yyyy......",
      ".....yyyyyy.....",
      "....yywyyyyy....",
      "....ywyyyyyy....",
      "....ywyyyyyy....",
      "...yyyyyyyyyy...",
      "...yyyyyyyyyy...",
      "...yyyyyyyyyy...",
      "..yyyyyyyyyyyy..",
      ".yyyyyyyyyyyyyy.",
      ".oooooooooooooo.",
      ".......oo.......",
      "......oooo......",
      ".......oo.......",
      "................"
    },
    {  // BLUME
      "......pppp......",
      ".....pppppp.....",
      "..pp.pppppp.pp..",
      ".pppp.pppp.pppp.",
      ".pppppyyyypppp..",
      "..ppppyyyyppp...",
      "....pyyyyyyp....",
      "..ppppyyyypppp..",
      ".pppppyyyyppppp.",
      ".pppp.pppp.pppp.",
      "..pp.pppppp.pp..",
      ".....pppppp.....",
      ".......gg.......",
      "....gg.gg.......",
      ".....gggg.......",
      ".......gg......."
    },
    {  // KIRSCHEN
      "..........GG....",
      ".........G..G...",
      "........G...GG..",
      ".......G.....G..",
      "......G......G..",
      ".....G........G.",
      "....G.........G.",
      "...rrrr......rrr",
      "..rrrrrr...rrrrr",
      ".rrwrrrrr.rrwrrr",
      ".rwrrrrrr.rwrrrr",
      ".rrrrrrrr.rrrrrr",
      ".rrrrrrrr.rrrrr.",
      "..rrrrrr...rrr..",
      "...rrrr.........",
      "................"
    },
    {  // RAKETE
      ".......ww.......",
      "......wwww......",
      ".....wwwwww.....",
      ".....wwccww.....",
      ".....wcccww.....",
      ".....wwccww.....",
      ".....wwwwww.....",
      ".....wwwwww.....",
      "....rwwwwwwr....",
      "...rrwwwwwwrr...",
      "..rrrwwwwwwrrr..",
      "..rr.wwwwww.rr..",
      ".......oo.......",
      "......oyyo......",
      ".......oo.......",
      "................"
    },
  };
  const char* const SET_NAME[3] = {"BILDER", "FORMEN", "ZAHLEN"};

  uint8_t mode = 0, symSet = 0;          // mode: 0 = 1 Spieler, 1 = auf Zeit, 2 = 2 Spieler
  uint8_t val[24], st[24], owner[24];    // st: 0 verdeckt, 1 offen, 2 gefunden
  uint8_t flip[24], flipTo[24];          // Umdreh-Animation
  int8_t sel1, sel2;
  uint8_t player, found, pairs[2], waitT, curI;
  uint16_t moves, wins;
  int32_t timeLeft;
  uint32_t lastScore;
  bool record;
  int COLS, ROWS, CW, CH, X0, Y0;
  int lastMode = -1;
  const uint8_t FLIP = 6;
  const uint16_t P_COL[2] = {RGB(235, 60, 60), RGB(60, 130, 255)};   // Spieler rot / blau
  const int32_t TIME = 90 * 30;

  int unlocked() { return wins >= 6 ? 3 : (wins >= 3 ? 2 : 1); }

  void layout() {
    if (landscape()) { COLS = 6; ROWS = 4; } else { COLS = 4; ROWS = 6; }
    CW = min(landscape() ? 50 : 58, SCR_W / COLS);
    CH = min(56, (SCR_H - 24) / ROWS);
    X0 = (SCR_W - COLS * CW) / 2;
    Y0 = 24 + ((SCR_H - 24) - ROWS * CH) / 2;
  }

  void deal() {
    for (int i = 0; i < 24; i++) val[i] = i / 2;
    for (int i = 23; i > 0; i--) { const int j = irand(0, i); const uint8_t t = val[i]; val[i] = val[j]; val[j] = t; }
    memset(st, 0, sizeof(st));
    memset(flip, 0, sizeof(flip));
    memset(owner, 0, sizeof(owner));
    sel1 = sel2 = -1;
    player = 0; found = 0; pairs[0] = pairs[1] = 0; waitT = 0;
    moves = 0;
    timeLeft = TIME;
    curI = 0;
    Led::set(mode == 2 ? Led::c565(RGB(235, 60, 60)) : Led::Rgb{0, 0, 0});   // 2 Spieler: Farbe des Spielers
    ph = PH_PLAY;
    Kit::dirty = true;
  }

  void setLabel() {
    const int u = unlocked();
    snprintf(setLbl, sizeof(setLbl), "SYMBOLE: %s", SET_NAME[symSet]);
    if (u == 1) snprintf(pickNote, sizeof(pickNote), "%d/3 Saetze - FORMEN ab 3 Siegen (%u)", u, wins);
    else if (u == 2) snprintf(pickNote, sizeof(pickNote), "%d/3 Saetze - ZAHLEN ab 6 Siegen (%u)", u, wins);
    else snprintf(pickNote, sizeof(pickNote), "Alle 3 Symbolsaetze frei");
  }

  void start() {
    Kit::begin();
    tPauseW = 40; tPauseH = 22;
    layout();
    wins = prefs.getUShort("mem_win", 0);
    symSet = prefs.getUChar("mem_set", 0);
    if (symSet >= unlocked()) symSet = 0;
    if (gameRestart && lastMode >= 0) { mode = lastMode; deal(); return; }
    pick.set("PAARE FINDEN");
    pick.add("1 SPIELER");
    pick.add("AUF ZEIT (90 s)");
    pick.add("2 SPIELER");
    setLabel();
    pick.add(setLbl);
    pick.note = pickNote;
    ph = PH_PICK;
  }

  bool animating() { for (int i = 0; i < 24; i++) if (flip[i]) return true; return false; }

  void finish() {
    ph = PH_DONE;
    record = false;
    lastScore = 0;
    if (mode != 2 && found == 12) {
      lastScore = moves <= 12 ? 1500 : (uint32_t)max(100, 1500 - (moves - 12) * 50);
      if (mode == 1) lastScore += (timeLeft / 30) * 10;
      record = lastScore > highscore[curGame];
      Kit::hsSet(lastScore);
      wins++;
      prefs.putUShort("mem_win", wins);
    }
    rumble(300, 0x90);
    if (mode == 2) Led::set565(pairs[0] == pairs[1] ? C_WHITE : P_COL[pairs[1] > pairs[0]], 200, 300);
    else if (found == 12) { Led::set({0, 0, 0}); Led::party(2000); }
    else Led::flash({255, 0, 0}, 800);
    Kit::dirty = true;
  }

  void open(int i) {
    if (st[i] != 0 || flip[i] || sel2 >= 0) return;
    st[i] = 1; flipTo[i] = 1; flip[i] = FLIP;
    if (sel1 < 0) sel1 = i;
    else { sel2 = i; moves++; }
    Kit::dirty = true;
  }

  void update() {
    if (ph == PH_PICK) {
      const int c = pick.update();
      if (c == 3) {                                   // Symbolsatz wechseln
        symSet = (symSet + 1) % unlocked();
        prefs.putUChar("mem_set", symSet);
        setLabel();
        Kit::dirty = true;
      } else if (c >= 0) { mode = c; lastMode = c; deal(); }
      return;
    }
    const Kit::Ev e = Kit::take();
    if (ph == PH_DONE) {
      if (e.t == Kit::EV_TAP || E.a || E.start) deal();
      return;
    }
    for (int i = 0; i < 24; i++) if (flip[i]) { flip[i]--; Kit::dirty = true; }
    if (mode == 1) {
      if (timeLeft % 30 == 0) Kit::dirty = true;
      if (--timeLeft <= 0) { timeLeft = 0; finish(); return; }
    }
    // zweite Karte ist offen: vergleichen
    if (sel2 >= 0 && !animating()) {
      if (val[sel1] == val[sel2]) {
        st[sel1] = st[sel2] = 2;
        owner[sel1] = owner[sel2] = player;
        pairs[player]++;
        found++;
        sel1 = sel2 = -1;
        rumble(60, 0x60);
        Led::flash({0, 255, 60}, 400);                  // Paar gefunden: gruen
        Kit::dirty = true;
        if (found == 12) { finish(); return; }
      } else if (waitT == 0) {
        waitT = 30;
      } else if (--waitT == 0 || e.t == Kit::EV_TAP || E.a) {
        for (int k : {sel1, sel2}) { st[k] = 0; flipTo[k] = 0; flip[k] = FLIP; }
        sel1 = sel2 = -1;
        waitT = 0;
        if (mode == 2) { player ^= 1; Led::set565(P_COL[player], 140); }
        Kit::dirty = true;
        return;
      }
    }
    const int n = COLS * ROWS;
    if (E.left)  { curI = (curI + n - 1) % n; Kit::dirty = true; }
    if (E.right) { curI = (curI + 1) % n; Kit::dirty = true; }
    if (E.up)    { curI = (curI + n - COLS) % n; Kit::dirty = true; }
    if (E.down)  { curI = (curI + COLS) % n; Kit::dirty = true; }
    if (E.a) open(curI);
    if (e.t == Kit::EV_TAP) {
      const int c = (e.x - X0) / CW, r = (e.y - Y0) / CH;
      if (e.x >= X0 && e.y >= Y0 && c < COLS && r < ROWS) { curI = r * COLS + c; open(curI); }
    }
  }

  // ---------------- Zeichnen ----------------

  void symbol(uint8_t v, int cx, int cy) {
    if (symSet == 0) { Kit::pix(SYM[v], 16, 16, cx - 16, cy - 16, 2); return; }
    if (symSet == 2) {
      static const uint16_t NC[6] = {RGB(220, 40, 40), RGB(30, 120, 230), RGB(20, 160, 60), RGB(230, 120, 0), RGB(150, 60, 210), RGB(20, 150, 170)};
      char b[4];
      snprintf(b, sizeof(b), "%d", v + 1);
      cTxt(b, cx, cy + 1, 3, NC[v % 6]);
      return;
    }
    const uint16_t c = v < 6 ? RGB(30, 110, 230) : RGB(240, 110, 0);
    const int s = 13;
    switch (v % 6) {
      case 0: cC(cx, cy, s, c); break;
      case 1: cR(cx - s + 1, cy - s + 1, 2 * s - 2, 2 * s - 2, c); break;
      case 2: cTri(cx, cy - s, cx - s, cy + s - 2, cx + s, cy + s - 2, c); break;
      case 3: cTri(cx, cy - s, cx - s, cy, cx + s, cy, c); cTri(cx, cy + s, cx - s, cy, cx + s, cy, c); break;
      case 4: cC(cx, cy, s, c); cC(cx, cy, s - 6, RGB(235, 235, 245)); break;
      default: cR(cx - s, cy - 4, 2 * s, 8, c); cR(cx - 4, cy - s, 8, 2 * s, c); break;
    }
  }

  void card(int i) {
    const int c = i % COLS, r = i / COLS;
    const int cx = X0 + c * CW + CW / 2, cy = Y0 + r * CH + CH / 2;
    const int fw = CW - 6, fh = CH - 6;
    bool face = st[i] != 0;
    int w = fw;
    if (flip[i]) {                                     // schmaler werdendes, dann wieder breiteres Rechteck
      const float p = 1.0f - (float)flip[i] / FLIP;
      w = max(2, (int)(fw * fabsf(1.0f - 2.0f * p)));
      face = p < 0.5f ? !flipTo[i] : flipTo[i];
    }
    const int x = cx - w / 2, y = cy - fh / 2;
    if (!face) {
      cRR(x, y, w, fh, 5, RGB(40, 60, 160));
      if (w > 12) {
        Kit::cRRO(x + 3, y + 3, w - 6, fh - 6, 4, RGB(90, 120, 230));
        cTri(cx, cy - 8, cx - 6, cy, cx + 6, cy, RGB(90, 120, 230));
        cTri(cx, cy + 8, cx - 6, cy, cx + 6, cy, RGB(90, 120, 230));
      }
    } else {
      uint16_t bg = RGB(235, 235, 245);
      if (st[i] == 2 && !flip[i]) bg = mode == 2 ? (owner[i] ? RGB(190, 210, 255) : RGB(255, 200, 200)) : RGB(146, 219, 146);
      cRR(x, y, w, fh, 5, bg);
      if (w >= fw - 1) symbol(val[i], cx, cy);
      if (st[i] == 2 && !flip[i]) Kit::cRRO(x, y, w, fh, 5, mode == 2 ? P_COL[owner[i]] : C_GREEN);
    }
    if (Kit::padMode && i == curI && ph == PH_PLAY) {
      Kit::cRRO(cx - fw / 2 - 2, y - 2, fw + 4, fh + 4, 6, C_YELLOW);
      Kit::cRRO(cx - fw / 2 - 1, y - 1, fw + 2, fh + 2, 6, C_YELLOW);
    }
  }

  void render() {
    if (ph == PH_PICK) { pick.render(); return; }
    for (int i = 0; i < COLS * ROWS; i++) card(i);
    char buf[40];
    if (mode == 2) {
      for (int p = 0; p < 2; p++) {
        const int x = p ? SCR_W / 2 + 4 : 6;
        if (player == p && ph == PH_PLAY) cRR(x - 3, 3, SCR_W / 2 - 50, 17, 4, P_COL[p]);
        snprintf(buf, sizeof(buf), "%s %u", p ? "BLAU" : "ROT", pairs[p]);
        cTxt(buf, x + 4, 12, 1.5, (player == p && ph == PH_PLAY) ? C_WHITE : P_COL[p], textdatum_t::middle_left);
      }
    } else {
      snprintf(buf, sizeof(buf), "ZUEGE %u", moves);
      cTxt(buf, 6, 12, 1.5, C_WHITE, textdatum_t::middle_left);
      snprintf(buf, sizeof(buf), "PAARE %u/12", found);
      cTxt(buf, landscape() ? 118 : 100, 12, 1.5, C_CYAN, textdatum_t::middle_left);
      if (mode == 1) {
        const int s = (timeLeft + 29) / 30;
        snprintf(buf, sizeof(buf), "%d:%02d", s / 60, s % 60);
        cTxt(buf, SCR_W - 46, 12, 1.5, s <= 15 ? C_RED : C_YELLOW, textdatum_t::middle_right);
      }
    }
    Kit::pauseIcon();
    if (ph == PH_DONE) {
      if (mode == 2) {
        snprintf(buf, sizeof(buf), "%u : %u Paare", pairs[0], pairs[1]);
        Kit::panel(pairs[0] == pairs[1] ? "UNENTSCHIEDEN" : (pairs[0] > pairs[1] ? "ROT GEWINNT" : "BLAU GEWINNT"),
                   buf, pairs[0] == pairs[1] ? C_WHITE : P_COL[pairs[1] > pairs[0]], "Antippen = neues Spiel");
      } else if (found < 12) {
        snprintf(buf, sizeof(buf), "%u von 12 Paaren", found);
        Kit::panel("ZEIT UM", buf, C_RED, "Antippen = neues Spiel");
      } else {
        snprintf(buf, sizeof(buf), "%u Zuege - %lu Punkte", moves, (unsigned long)lastScore);
        Kit::panel(record ? "NEUER REKORD!" : "GESCHAFFT!", buf, C_YELLOW, "Antippen = neues Spiel");
      }
    }
  }

  void loop() {
    if (Kit::pause()) return;
    Kit::step(update, render);
  }
}
