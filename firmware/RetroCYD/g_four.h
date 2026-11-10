// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  VIER GEWINNT  -  Steine fallen in sieben Spalten, vier in einer Reihe gewinnen
// ============================================================================
//  Spalte antippen (jede Spalte reicht ueber die volle Bildhoehe).
//  Gegner: CPU mit Suchtiefe 4, 6 oder 8 Zuegen (Alpha-Beta-Suche), oder
//  2 Spieler abwechselnd am Geraet. Dazu "Fuenf gewinnt" auf 9 x 7.
//  Serienwertung (Siege : Niederlagen : Remis); Highscore = laengste Siegesserie.
//  Controller: links/rechts = Spalte, A = einwerfen.
// ============================================================================
#pragma once

namespace Four {
  enum Phase : uint8_t { PH_PICK, PH_TURN, PH_DROP, PH_THINK, PH_WIN, PH_OVER };
  Phase ph;
  Kit::Picker pick;

  uint8_t mode = 0, C = 7, R = 6, WIN = 4, depthMax = 4;
  bool twoP = false;
  uint8_t bd[9 * 8];                 // [c * 8 + r], r = 0 unten; 0 leer, 1 rot, 2 gelb
  uint8_t hgt[9];
  uint8_t turn, starter, winner, moveCount, curC, lastC;
  uint8_t winCells[9][2], nWin;
  uint16_t sWin, sLoss, sDraw, streak, bestStreak, round;
  int lastMode = -1;
  // Fallender Stein
  uint8_t dropC, dropP, dropR;
  float dropY, dropV;
  int thinkT, overT;
  // Darstellung
  int CWp, RHp, X0, Y0, RAD;

  // ---------------- Spiellogik ----------------
  inline uint8_t& at(int c, int r) { return bd[c * 8 + r]; }
  inline uint8_t get(int c, int r) { return (c < 0 || r < 0 || c >= C || r >= R) ? 0 : bd[c * 8 + r]; }

  int lineLen(int c, int r, int p, int dc, int dr) {
    int n = 1;
    for (int k = 1; get(c + k * dc, r + k * dr) == p; k++) n++;
    for (int k = 1; get(c - k * dc, r - k * dr) == p; k++) n++;
    return n;
  }
  bool winAt(int c, int r, int p) {
    return lineLen(c, r, p, 1, 0) >= WIN || lineLen(c, r, p, 0, 1) >= WIN ||
           lineLen(c, r, p, 1, 1) >= WIN || lineLen(c, r, p, 1, -1) >= WIN;
  }
  void markWin(int c, int r, int p) {
    const int8_t D[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    nWin = 0;
    for (auto& d : D) {
      if (lineLen(c, r, p, d[0], d[1]) < WIN) continue;
      int k = 0;
      while (get(c - (k + 1) * d[0], r - (k + 1) * d[1]) == p) k++;
      for (int j = -k; get(c + j * d[0], r + j * d[1]) == p && nWin < 9; j++) {
        winCells[nWin][0] = c + j * d[0];
        winCells[nWin][1] = r + j * d[1];
        nWin++;
      }
      return;
    }
  }

  // ---------------- CPU ----------------
  const int INF = 1000000, WINS = 100000;
  uint8_t order[9];
  uint32_t nodes, deadline;
  bool timeout;

  int evalFor(int p) {
    static const int W4[5] = {0, 1, 8, 60, 0}, W5[6] = {0, 1, 4, 20, 100, 0};
    const int* Wt = WIN == 4 ? W4 : W5;
    const int8_t D[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
    int s = 0;
    for (int c = 0; c < C; c++)
      for (int r = 0; r < R; r++)
        for (auto& d : D) {
          const int ec = c + (WIN - 1) * d[0], er = r + (WIN - 1) * d[1];
          if (ec >= C || er < 0 || er >= R) continue;
          int a = 0, b = 0;
          for (int k = 0; k < WIN; k++) {
            const uint8_t v = bd[(c + k * d[0]) * 8 + r + k * d[1]];
            if (v == p) a++; else if (v) b++;
          }
          if (!b) s += Wt[a]; else if (!a) s -= Wt[b];
        }
    for (int r = 0; r < hgt[C / 2]; r++) s += at(C / 2, r) == p ? 3 : -3;
    return s;
  }

  int negamax(int depth, int alpha, int beta, int p) {
    if ((++nodes & 1023) == 0 && millis() > deadline) timeout = true;
    if (timeout) return 0;
    for (int k = 0; k < C; k++) {                  // sofortiger Sieg?
      const int c = order[k];
      if (hgt[c] >= R) continue;
      const int r = hgt[c];
      at(c, r) = p;
      const bool w = winAt(c, r, p);
      at(c, r) = 0;
      if (w) return WINS + depth;
    }
    if (depth == 0) return evalFor(p);
    int best = -INF;
    bool any = false;
    for (int k = 0; k < C; k++) {
      const int c = order[k];
      if (hgt[c] >= R) continue;
      any = true;
      at(c, hgt[c]++) = p;
      const int v = -negamax(depth - 1, -beta, -alpha, 3 - p);
      at(c, --hgt[c]) = 0;
      if (v > best) best = v;
      if (v > alpha) alpha = v;
      if (alpha >= beta) break;
    }
    return any ? best : 0;
  }

  int cpuMove(int p) {
    for (int k = 0; k < C; k++) order[k] = C / 2 + ((k & 1) ? (k + 1) / 2 : -(k + 1) / 2);   // Mitte zuerst
    int bestC = -1;
    deadline = millis() + 2500;
    timeout = false;
    nodes = 0;
    for (int d = 2; d <= depthMax; d += 2) {        // iterativ tiefer, bis die Zeit abläuft
      int alpha = -INF, bc = -1, bv = -INF;
      for (int k = 0; k < C; k++) {
        const int c = order[k];
        if (hgt[c] >= R) continue;
        const int r = hgt[c];
        at(c, r) = p;
        int v;
        if (winAt(c, r, p)) v = WINS + 100;
        else { hgt[c]++; v = -negamax(d - 1, -INF, -alpha, 3 - p); hgt[c]--; }
        at(c, r) = 0;
        if (depthMax == 4 && v < WINS / 2 && v > -WINS / 2) v += irand(-25, 25);   // LEICHT: etwas Zufall
        if (timeout) break;
        if (v > bv) { bv = v; bc = c; }
        if (v > alpha) alpha = v;
      }
      if (timeout) break;
      bestC = bc;
      // beste Spalte zuerst weitersuchen
      for (int k = 0; k < C; k++) if (order[k] == bc) { for (int j = k; j > 0; j--) order[j] = order[j - 1]; order[0] = bc; break; }
      if (bv >= WINS) break;
    }
    if (bestC < 0) for (int k = 0; k < C; k++) if (hgt[order[k]] < R) { bestC = order[k]; break; }
    return bestC;
  }

  // ---------------- Ablauf ----------------
  bool cpuTurn() { return !twoP && turn == 2; }

  void layout() {
    CWp = min(45, SCR_W / C);
    RHp = min(CWp, (SCR_H - 28) / R);
    X0 = (SCR_W - C * CWp) / 2;
    Y0 = SCR_H - R * RHp - 1;
    RAD = min(CWp, RHp) / 2 - 3;
  }
  int cellX(int c) { return X0 + c * CWp + CWp / 2; }
  int cellY(int r) { return Y0 + (R - 1 - r) * RHp + RHp / 2; }

  void newRound() {
    memset(bd, 0, sizeof(bd));
    memset(hgt, 0, sizeof(hgt));
    moveCount = 0; nWin = 0; winner = 0;
    starter = (round & 1) ? 2 : 1;
    turn = starter;
    curC = C / 2;
    ph = cpuTurn() ? PH_THINK : PH_TURN;
    thinkT = 8;
    Kit::dirty = true;
  }

  void startMode(int m) {
    mode = m;
    twoP = m == 3;
    C = m == 4 ? 9 : 7;
    R = m == 4 ? 7 : 6;
    WIN = m == 4 ? 5 : 4;
    depthMax = m == 0 ? 4 : (m == 1 ? 6 : (m == 2 ? 8 : (m == 4 ? 4 : 0)));
    sWin = sLoss = sDraw = 0;
    streak = 0;
    round = 0;
    layout();
    newRound();
  }

  void start() {
    Kit::begin();
    tPauseW = 48; tPauseH = 24;
    if (gameRestart && lastMode >= 0) { startMode(lastMode); return; }
    pick.set("VIER GEWINNT");
    pick.add("CPU LEICHT (Tiefe 4)");
    pick.add("CPU MITTEL (Tiefe 6)");
    pick.add("CPU STARK (Tiefe 8)");
    pick.add("2 SPIELER");
    pick.add("FUENF GEWINNT 9 x 7");
    pick.note = "Spalte antippen = Stein einwerfen";
    ph = PH_PICK;
  }

  void drop(int c) {
    if (c < 0 || c >= C || hgt[c] >= R) return;
    dropC = c; dropP = turn; dropR = hgt[c];
    dropY = Y0 - RHp / 2; dropV = 0;
    lastC = c;
    ph = PH_DROP;
    Kit::dirty = true;
  }

  void landed() {
    at(dropC, hgt[dropC]++) = dropP;
    moveCount++;
    rumble(30, 0x40);
    if (winAt(dropC, dropR, dropP)) {
      winner = dropP;
      markWin(dropC, dropR, dropP);
      if (!twoP) {
        if (winner == 1) { sWin++; streak++; bestStreak = max(bestStreak, streak); Kit::hsSet(streak); }
        else { sLoss++; streak = 0; }
      } else (winner == 1 ? sWin : sLoss)++;
      ph = PH_WIN; overT = 0;
      rumble(300, 0xA0);
      return;
    }
    if (moveCount >= C * R) { winner = 0; sDraw++; ph = PH_WIN; overT = 0; return; }
    turn = 3 - turn;
    if (cpuTurn()) { ph = PH_THINK; thinkT = 8; } else ph = PH_TURN;
  }

  void update() {
    if (ph == PH_PICK) {
      const int c = pick.update();
      if (c >= 0) { lastMode = c; startMode(c); }
      return;
    }
    const Kit::Ev e = Kit::take();
    switch (ph) {
      case PH_TURN:
        if (E.left && curC > 0) { curC--; Kit::dirty = true; }
        if (E.right && curC < C - 1) { curC++; Kit::dirty = true; }
        if (E.a || E.down) drop(curC);
        else if (e.t == Kit::EV_TAP) {
          const int c = (e.x - X0) / CWp;
          if (e.x >= X0 && c < C) { curC = c; drop(c); }
        }
        break;
      case PH_DROP: {
        dropV += 1.8f;
        dropY += dropV;
        Kit::dirty = true;
        if (dropY >= cellY(dropR)) landed();
        break;
      }
      case PH_THINK:
        Kit::dirty = true;
        if (--thinkT <= 0) {
          const int c = cpuMove(2);
          curC = c;
          drop(c);
          fbResume(); Kit::lastT = millis(); Kit::acc = 0;   // Rechenzeit nicht nachholen
        }
        break;
      case PH_WIN:
        overT++;
        if ((overT & 7) == 0) Kit::dirty = true;
        if (overT > 40) { ph = PH_OVER; Kit::dirty = true; }
        break;
      case PH_OVER:
        if (e.t == Kit::EV_TAP || E.a || E.start) { round++; newRound(); }
        break;
      default: break;
    }
  }

  // ---------------- Zeichnen ----------------
  const uint16_t COLS[3] = {C_BG, RGB(235, 45, 45), RGB(255, 210, 0)};

  void disc(int x, int y, int p, bool hi) {
    cC(x, y, RAD, COLS[p]);
    cC(x, y, RAD - 4, p == 1 ? RGB(200, 30, 30) : RGB(225, 180, 0));
    if (hi) { cCO(x, y, RAD, C_WHITE); cCO(x, y, RAD - 1, C_WHITE); }
  }

  void render() {
    if (ph == PH_PICK) { pick.render(); return; }
    // Kopfzeile
    char buf[32];
    const char* msg;
    uint16_t mc = COLS[turn];
    if (ph == PH_WIN || ph == PH_OVER) { msg = winner ? "VIER IN EINER REIHE!" : "UNENTSCHIEDEN"; mc = winner ? COLS[winner] : C_WHITE; }
    else if (ph == PH_THINK || (ph == PH_DROP && cpuTurn())) msg = "CPU DENKT...";
    else if (twoP) msg = turn == 1 ? "ROT IST DRAN" : "GELB IST DRAN";
    else msg = "DU BIST DRAN";
    if (WIN == 5 && ph != PH_WIN && ph != PH_OVER) cTxt("5", 6, 12, 1, C_DIM, textdatum_t::middle_left);
    cTxt(msg, 16, 12, 1.5, mc, textdatum_t::middle_left);
    snprintf(buf, sizeof(buf), "%u:%u:%u", sWin, sLoss, sDraw);
    cTxt(buf, SCR_W - 52, 12, 1, C_GRAY, textdatum_t::middle_right);
    // Brett
    const uint16_t BOARD = RGB(30, 70, 200), HOLE = RGB(10, 12, 30);
    cRR(X0 - 1, Y0 - 3, C * CWp + 2, R * RHp + 4, 6, BOARD);
    if (ph == PH_TURN && (Kit::padMode || twoP))      // gewaehlte Spalte
      cR(X0 + curC * CWp + 2, Y0 - 3, CWp - 4, 3, COLS[turn]);
    for (int c = 0; c < C; c++)
      for (int r = 0; r < R; r++) {
        const int x = cellX(c), y = cellY(r);
        const uint8_t v = at(c, r);
        bool inWin = false;
        for (int k = 0; k < nWin; k++) if (winCells[k][0] == c && winCells[k][1] == r) inWin = true;
        if (!v) { cC(x, y, RAD, HOLE); continue; }
        if (inWin && ph == PH_WIN && ((overT >> 3) & 1)) { cC(x, y, RAD, C_WHITE); continue; }
        disc(x, y, v, inWin && ph == PH_OVER);
      }
    if (ph == PH_DROP) disc(cellX(dropC), (int)dropY, dropP, false);
    if (ph == PH_TURN && Kit::padMode) cTri(cellX(curC), Y0 - 6, cellX(curC) - 6, Y0 - 14, cellX(curC) + 6, Y0 - 14, COLS[turn]);
    Kit::pauseIcon();
    if (ph == PH_OVER) {
      const char* t;
      if (!winner) t = "UNENTSCHIEDEN";
      else if (twoP) t = winner == 1 ? "ROT GEWINNT" : "GELB GEWINNT";
      else t = winner == 1 ? "DU GEWINNST!" : "CPU GEWINNT";
      if (twoP) snprintf(buf, sizeof(buf), "Rot %u : %u Gelb  (%u remis)", sWin, sLoss, sDraw);
      else snprintf(buf, sizeof(buf), "Serie %u:%u:%u  Folge %u", sWin, sLoss, sDraw, streak);
      Kit::panel(t, buf, winner ? COLS[winner] : C_WHITE, "Antippen = naechste Runde");
    }
  }

  void loop() {
    if (Kit::pause()) return;
    Kit::step(update, render);
  }
}
