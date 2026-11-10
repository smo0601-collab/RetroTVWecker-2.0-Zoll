// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  SIEBZEHN UND VIER  -  mit Karten moeglichst nah an 21, ohne zu ueberkaufen
// ============================================================================
//  Einsatz mit - und +, dann AUSTEILEN. Im Spiel: KARTE, HALTEN, VERDOPPELN,
//  bei zwei gleichen Karten TEILEN. Die Bank zieht bis 16 und haelt ab 17,
//  "Black Jack" (21 mit zwei Karten) zahlt 3:2. 4 Kartenspiele im Schlitten.
//  Der Chipstand und die Statistik werden gespeichert; der Trainer zeigt den
//  besten Zug nach der Grundstrategie.
//  Controller: links/rechts = Knopf waehlen, A = druecken.
// ============================================================================
#pragma once

namespace BJ {
  enum Phase : uint8_t { PH_BET, PH_DEAL, PH_PLAY, PH_DEALER, PH_RESULT };
  Phase ph;

  uint8_t shoe[208];
  int shoePos = 208;
  uint8_t dl[11], nd;                       // Bank
  uint8_t hd[2][11], nh[2];                 // bis zu 2 Haende (nach Teilen)
  int32_t bet[2];
  bool dbl[2], fin[2], splitAces;
  uint8_t nHands, act;
  bool hole;                                // zweite Bankkarte verdeckt
  int8_t outcome[2];                        // Ergebnis je Hand: 2 = BJ, 1 = gewonnen, 0 = gleich, -1 = verloren
  int32_t net;
  // Austeilen in Schritten mit Pause
  uint8_t dq[6], dqn, dqi;
  int waitT;
  int8_t animWho = -1;                      // 0 Bank, 1/2 Hand 1/2: letzte Karte gleitet herein
  int animT = 0;
  const int ANIM = 6;

  struct Save { uint32_t magic; int32_t chips; uint16_t hands, wins, losses, pushes, streak, best; uint8_t tip, betIdx; };
  Save sv;
  const int32_t BETS[7] = {10, 20, 50, 100, 200, 500, 1000};
  bool refilled = false;
  int sel = 0;                              // Controller: gewaehlter Knopf

  void load() {
    if (prefs.getBytes("bj_s", &sv, sizeof(sv)) != sizeof(sv) || sv.magic != 0x424A3137) {
      sv = {0x424A3137, 1000, 0, 0, 0, 0, 0, 0, 1, 1};
    }
    if (sv.betIdx > 6) sv.betIdx = 1;
  }
  void save() { prefs.putBytes("bj_s", &sv, sizeof(sv)); }

  // ---------------- Karten ----------------
  void shuffle() {
    for (int i = 0; i < 208; i++) shoe[i] = i % 52;
    for (int i = 207; i > 0; i--) { const int j = irand(0, i); const uint8_t t = shoe[i]; shoe[i] = shoe[j]; shoe[j] = t; }
    shoePos = 0;
  }
  uint8_t draw() { if (shoePos >= 208) shuffle(); return shoe[shoePos++]; }
  int rankOf(uint8_t c) { return c % 13; }        // 0 = Ass, 1..9 = 2..10, 10 B, 11 D, 12 K
  int cardVal(uint8_t c) { const int r = rankOf(c); return r == 0 ? 11 : (r >= 9 ? 10 : r + 1); }

  int total(const uint8_t* h, int n, bool* soft = nullptr) {
    int t = 0, aces = 0;
    for (int i = 0; i < n; i++) { t += cardVal(h[i]); if (rankOf(h[i]) == 0) aces++; }
    while (t > 21 && aces) { t -= 10; aces--; }
    if (soft) *soft = aces > 0;
    return t;
  }
  bool natural(const uint8_t* h, int n) { return n == 2 && total(h, n) == 21; }

  void give(int who) {                            // 0 = Bank, 1 = aktive Hand
    if (who == 0) dl[nd++] = draw();
    else hd[act][nh[act]++] = draw();
    animWho = who == 0 ? 0 : act + 1;
    animT = ANIM;
    Kit::dirty = true;
  }

  // ---------------- Grundstrategie (Trainer) ----------------
  bool canDouble() { return nh[act] == 2 && !splitAces && sv.chips >= bet[act]; }
  bool canSplit() {
    return nHands == 1 && nh[0] == 2 && cardVal(hd[0][0]) == cardVal(hd[0][1]) &&
           rankOf(hd[0][0]) == rankOf(hd[0][1]) && sv.chips >= bet[0];
  }
  // 'H' Karte, 'S' halten, 'D' verdoppeln, 'P' teilen
  char advice() {
    const uint8_t* h = hd[act];
    const int n = nh[act];
    const int up = cardVal(dl[0]);                 // 2..11
    bool soft;
    const int t = total(h, n, &soft);
    if (canSplit()) {
      const int v = cardVal(h[0]);
      if (v == 11 || v == 8) return 'P';
      if (v == 9 && up != 7 && up != 10 && up != 11) return 'P';
      if ((v == 7 || v == 3 || v == 2) && up <= 7) return 'P';
      if (v == 6 && up <= 6) return 'P';
      if (v == 4 && (up == 5 || up == 6)) return 'P';
    }
    char a;
    if (soft && n >= 2) {
      if (t >= 20) a = 'S';
      else if (t == 19) a = up == 6 ? 'd' : 'S';                    // 'd' = verdoppeln, sonst halten
      else if (t == 18) a = up <= 6 ? 'd' : (up <= 8 ? 'S' : 'H');
      else if (t == 17) a = (up >= 3 && up <= 6) ? 'D' : 'H';
      else if (t >= 15) a = (up >= 4 && up <= 6) ? 'D' : 'H';
      else a = (up == 5 || up == 6) ? 'D' : 'H';
    } else {
      if (t >= 17) a = 'S';
      else if (t >= 13) a = up <= 6 ? 'S' : 'H';
      else if (t == 12) a = (up >= 4 && up <= 6) ? 'S' : 'H';
      else if (t == 11) a = up == 11 ? 'H' : 'D';
      else if (t == 10) a = up <= 9 ? 'D' : 'H';
      else if (t == 9) a = (up >= 3 && up <= 6) ? 'D' : 'H';
      else a = 'H';
    }
    if (a == 'd') a = canDouble() ? 'D' : 'S';
    if (a == 'D' && !canDouble()) a = 'H';
    return a;
  }
  const char* adviceTxt(char a) { return a == 'H' ? "KARTE" : a == 'S' ? "HALTEN" : a == 'D' ? "VERDOPPELN" : "TEILEN"; }

  // ---------------- Ablauf ----------------
  void toBet() {
    ph = PH_BET;
    refilled = false;
    if (sv.chips < BETS[0]) { sv.chips = 1000; refilled = true; save(); }
    while (sv.betIdx > 0 && BETS[sv.betIdx] > sv.chips) sv.betIdx--;
    sel = 2;
    Kit::dirty = true;
  }

  void deal() {
    const int32_t b = BETS[sv.betIdx];
    if (b > sv.chips) return;
    if (shoePos > 160) shuffle();
    sv.chips -= b;
    nd = 0; nh[0] = nh[1] = 0; nHands = 1; act = 0;
    bet[0] = b; bet[1] = 0;
    dbl[0] = dbl[1] = fin[0] = fin[1] = false;
    splitAces = false;
    hole = true;
    outcome[0] = outcome[1] = 0;
    const uint8_t q[4] = {1, 0, 1, 0};
    memcpy(dq, q, 4); dqn = 4; dqi = 0;
    waitT = 2;
    ph = PH_DEAL;
    Kit::dirty = true;
  }

  void finishRound();

  void nextHand() {
    fin[act] = true;
    if (act + 1 < nHands) {
      act++;
      if (nh[act] < 2) { dq[0] = 1; dqn = 1; dqi = 0; waitT = 8; ph = PH_DEAL; }   // geteilte Hand bekommt ihre 2. Karte
      sel = 0;
      Kit::dirty = true;
      return;
    }
    // Bank spielt (nur wenn nicht alle Haende ueberkauft sind)
    hole = false;
    ph = PH_DEALER;
    waitT = 16;
    Kit::dirty = true;
  }

  void afterDeal() {
    if (nHands == 2) {                                // zweite Karte einer geteilten Hand ist da
      if (splitAces || total(hd[act], nh[act]) == 21) { nextHand(); return; }
      ph = PH_PLAY;
      sel = 0;
      return;
    }
    // Bank schaut bei Ass oder Zehn nach Black Jack
    const bool dBJ = natural(dl, 2);
    const bool pBJ = natural(hd[0], 2);
    if (dBJ || pBJ) { hole = false; finishRound(); return; }
    ph = PH_PLAY;
    sel = 0;
  }

  void act_(char a) {
    if (ph != PH_PLAY) return;
    if (a == 'H') {
      give(1);
      const int t = total(hd[act], nh[act]);
      if (t >= 21) { waitT = 10; fin[act] = true; }
    } else if (a == 'S') {
      nextHand();
    } else if (a == 'D' && canDouble()) {
      sv.chips -= bet[act];
      bet[act] *= 2;
      dbl[act] = true;
      give(1);
      waitT = 12; fin[act] = true;
    } else if (a == 'P' && canSplit()) {
      sv.chips -= bet[0];
      bet[1] = bet[0];
      hd[1][0] = hd[0][1];
      nh[0] = 1; nh[1] = 1;
      nHands = 2;
      splitAces = rankOf(hd[0][0]) == 0;
      dq[0] = 1; dqn = 1; dqi = 0; waitT = 6;            // Hand 1 bekommt ihre 2. Karte
      ph = PH_DEAL;
    }
    Kit::dirty = true;
  }

  void finishRound() {
    const int dt = total(dl, nd);
    const bool dBJ = natural(dl, nd);
    net = 0;
    for (int h = 0; h < nHands; h++) {
      const int pt = total(hd[h], nh[h]);
      const bool pBJ = nHands == 1 && natural(hd[h], nh[h]);
      int8_t o;
      if (pt > 21) o = -1;
      else if (pBJ && !dBJ) o = 2;
      else if (dBJ && !pBJ) o = -1;
      else if (dt > 21 || pt > dt) o = 1;
      else if (pt == dt) o = 0;
      else o = -1;
      outcome[h] = o;
      int32_t back = 0;
      if (o == 2) back = bet[h] + bet[h] * 3 / 2;
      else if (o == 1) back = 2 * bet[h];
      else if (o == 0) back = bet[h];
      sv.chips += back;
      net += back - bet[h];
      sv.hands++;
      if (o > 0) { sv.wins++; sv.streak++; if (sv.streak > sv.best) sv.best = sv.streak; }
      else if (o < 0) { sv.losses++; sv.streak = 0; }
      else sv.pushes++;
    }
    save();
    Kit::hsSet(sv.chips > 0 ? (uint32_t)sv.chips : 0);
    if (net > 0) rumble(200, 0x80);
    ph = PH_RESULT;
    sel = 0;
    Kit::dirty = true;
  }

  // ---------------- Knoepfe ----------------
  int BTN_Y() { return SCR_H - 52; }
  int nBtns() {
    if (ph == PH_BET) return 4;                       // -, +, AUSTEILEN, Trainer
    if (ph == PH_PLAY) return canSplit() ? 4 : 3;
    if (ph == PH_RESULT) return 2;
    return 0;
  }
  Kit::Btn btnRect(int i) {
    if (ph == PH_BET && i == 3) return {(int16_t)(SCR_W / 2 - 75), (int16_t)(BTN_Y() - 34), 150, 26};
    const int n = (ph == PH_BET) ? 3 : nBtns();
    const int gap = 5, w = (SCR_W - 10 - gap * (n - 1)) / n;
    return {(int16_t)(5 + i * (w + gap)), (int16_t)BTN_Y(), (int16_t)w, 50};
  }
  void press(int i) {
    if (ph == PH_BET) {
      if (i == 0 && sv.betIdx > 0) sv.betIdx--;
      if (i == 1 && sv.betIdx < 6 && BETS[sv.betIdx + 1] <= sv.chips) sv.betIdx++;
      if (i == 2) { save(); deal(); }
      if (i == 3) { sv.tip ^= 1; save(); }
    } else if (ph == PH_PLAY) {
      const char A[4] = {'H', 'S', 'D', 'P'};
      act_(A[i]);
    } else if (ph == PH_RESULT) {
      if (i == 0) { if (BETS[sv.betIdx] <= sv.chips) deal(); else toBet(); }
      else toBet();
    }
    Kit::dirty = true;
  }

  void start() {
    Kit::begin();
    tPauseW = 48; tPauseH = 22;
    load();
    shuffle();
    toBet();
  }

  void update() {
    if (animT) { animT--; Kit::dirty = true; }
    const Kit::Ev e = Kit::take();
    if (ph == PH_DEAL) {
      if (--waitT > 0) return;
      if (dqi < dqn) {
        const int who = dq[dqi++];
        give(who);
        waitT = 9;
        return;
      }
      afterDeal();
      return;
    }
    if (ph == PH_PLAY && fin[act] && waitT > 0) { if (--waitT == 0) nextHand(); return; }
    if (ph == PH_DEALER) {
      if (--waitT > 0) return;
      bool allBust = true;
      for (int h = 0; h < nHands; h++) if (total(hd[h], nh[h]) <= 21) allBust = false;
      if (!allBust && total(dl, nd) < 17) { give(0); waitT = 14; return; }
      finishRound();
      return;
    }
    const int n = nBtns();
    if (!n) return;
    if (E.left)  { sel = (sel + n - 1) % n; Kit::dirty = true; }
    if (E.right) { sel = (sel + 1) % n; Kit::dirty = true; }
    if (ph == PH_BET && E.up) press(1);
    if (ph == PH_BET && E.down) press(0);
    if (E.a) { press(sel); return; }
    if (e.t == Kit::EV_TAP)
      for (int i = 0; i < n; i++) if (Kit::hit(btnRect(i), e.x, e.y)) { press(i); return; }
  }

  // ---------------- Zeichnen ----------------
  const int CW = 50, CH = 70;
  const uint16_t RED = RGB(215, 25, 30), BLK = RGB(20, 20, 30);

  void suit(int cx, int cy, int s, int t, uint16_t c) {
    switch (t) {
      case 0:                                         // Herz
        cC(cx - s / 2, cy - s / 4, s / 2, c); cC(cx + s / 2, cy - s / 4, s / 2, c);
        cTri(cx - s, cy - s / 6, cx + s, cy - s / 6, cx, cy + s, c); break;
      case 1:                                         // Karo
        cTri(cx, cy - s, cx - (s * 3) / 4, cy, cx + (s * 3) / 4, cy, c);
        cTri(cx, cy + s, cx - (s * 3) / 4, cy, cx + (s * 3) / 4, cy, c); break;
      case 2:                                         // Pik
        cC(cx - s / 2, cy + s / 5, s / 2, c); cC(cx + s / 2, cy + s / 5, s / 2, c);
        cTri(cx - s, cy + s / 8, cx + s, cy + s / 8, cx, cy - s, c);
        cTri(cx, cy, cx - s / 2, cy + s, cx + s / 2, cy + s, c); break;
      default:                                        // Kreuz
        cC(cx, cy - s / 2, (s * 9) / 20, c); cC(cx - s / 2, cy + s / 6, (s * 9) / 20, c); cC(cx + s / 2, cy + s / 6, (s * 9) / 20, c);
        cTri(cx, cy, cx - s / 2, cy + s, cx + s / 2, cy + s, c); break;
    }
  }

  void card(int x, int y, uint8_t c, bool up) {
    if (!up) {
      cRR(x, y, CW, CH, 5, RGB(150, 20, 45));
      Kit::cRRO(x + 3, y + 3, CW - 6, CH - 6, 4, RGB(230, 150, 160));
      for (int k = 0; k < 3; k++) suit(x + CW / 2, y + 16 + k * 19, 5, 1, RGB(210, 90, 110));
      return;
    }
    cRR(x, y, CW, CH, 5, RGB(248, 246, 238));
    Kit::cRRO(x, y, CW, CH, 5, RGB(150, 150, 160));
    const int r = rankOf(c), s = c / 13;
    const uint16_t col = (s == 0 || s == 1) ? RED : BLK;
    static const char* RN[13] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "B", "D", "K"};
    cTxt(RN[r], x + 4, y + 4, 2, col, textdatum_t::top_left);
    suit(x + 10, y + 28, 5, s, col);
    if (r >= 10) {                                    // Bube, Dame, Koenig: grosser Buchstabe im Rahmen
      cRO(x + 20, y + 30, 26, 34, col);
      cTxt(RN[r], x + 33, y + 48, 3, col);
    } else {
      suit(x + 32, y + 48, 11, s, col);
    }
  }

  int handStep(int n, int areaW) { return n > 1 ? min(54, (areaW - CW) / (n - 1)) : 0; }

  void drawHand(const uint8_t* h, int n, int x0, int areaW, int y, int who, bool hideSecond) {
    const int st = handStep(n, areaW);
    for (int i = 0; i < n; i++) {
      int x = x0 + i * st, yy = y;
      if (i == n - 1 && animT && animWho == who) {      // neue Karte gleitet vom Schlitten herein
        const float t = 1.0f - (float)animT / ANIM;
        x = (int)(SCR_W - 56 + (x - (SCR_W - 56)) * t);
        yy = (int)(-CH + (y + CH) * t);
      }
      card(x, yy, h[i], !(hideSecond && i == 1));
    }
  }

  void render() {
    char buf[40];
    cR(0, 0, SCR_W, SCR_H, RGB(0, 100, 40));          // gruener Spieltisch
    // Kopfzeile
    cTxt("CHIPS", 6, 4, 1, C_GRAY, textdatum_t::top_left);
    snprintf(buf, sizeof(buf), "%ld", (long)sv.chips);
    cTxt(buf, 40, 2, 2, C_YELLOW, textdatum_t::top_left);
    if (ph != PH_BET) {
      int32_t b = bet[0] + (nHands == 2 ? bet[1] : 0);
      snprintf(buf, sizeof(buf), "EINSATZ %ld", (long)b);
      cTxt(buf, SCR_W - 54, 4, 1, C_WHITE, textdatum_t::top_right);
    }
    const int dY = 26, pY = landscape() ? 104 : 124;
    const int areaR = SCR_W - 62;

    if (ph == PH_BET) {
      cTxt("SIEBZEHN UND VIER", SCR_W / 2, 40, Kit::fit("SIEBZEHN UND VIER", SCR_W - 20, 2), C_YELLOW);
      cTxt("Bank zieht bis 16, haelt ab 17", SCR_W / 2, 60, 1, C_GRAY);
      cTxt("Black Jack zahlt 3:2", SCR_W / 2, 72, 1, C_GRAY);
      cTxt("EINSATZ", SCR_W / 2, 90, 1, C_GRAY);
      snprintf(buf, sizeof(buf), "%ld", (long)BETS[sv.betIdx]);
      cTxt(buf, SCR_W / 2, 110, 3, C_WHITE);
      snprintf(buf, sizeof(buf), "Runden %u  Gew. %u  Verl. %u  Gleich %u", sv.hands, sv.wins, sv.losses, sv.pushes);
      cTxt(buf, SCR_W / 2, 132, 1, C_CYAN);
      snprintf(buf, sizeof(buf), "Serie %u   beste Serie %u", sv.streak, sv.best);
      cTxt(buf, SCR_W / 2, 144, 1, C_CYAN);
      if (refilled) cTxt("Die Bank schenkt dir 1000 Chips", SCR_W / 2, BTN_Y() - 44, 1, C_GREEN);
      Kit::btn(btnRect(3), sv.tip ? "TRAINER: AN" : "TRAINER: AUS", sv.tip ? C_DGREEN : Kit::PANEL, C_WHITE, Kit::padMode && sel == 3, 1.5);
      Kit::btn(btnRect(0), "-", Kit::PANEL, C_WHITE, Kit::padMode && sel == 0, 3);
      Kit::btn(btnRect(1), "+", Kit::PANEL, C_WHITE, Kit::padMode && sel == 1, 3);
      Kit::btn(btnRect(2), "AUSTEILEN", C_DGREEN, C_WHITE, Kit::padMode && sel == 2, 2);
      Kit::pauseIcon();
      return;
    }
    // Tisch
    drawHand(dl, nd, 6, areaR - 6, dY, 0, hole);
    if (nd == 0) {
    } else if (!hole || nd < 2) {
      snprintf(buf, sizeof(buf), "%d", total(dl, nd));
      cTxt(buf, SCR_W - 30, dY + 40, 3, total(dl, nd) > 21 ? C_RED : C_WHITE);
    } else cTxt("?", SCR_W - 30, dY + 40, 3, C_GRAY);
    cTxt("BANK", SCR_W - 30, dY + 14, 1, C_GRAY);
    if (nHands == 1) {
      drawHand(hd[0], nh[0], 6, areaR - 6, pY, 1, false);
      const int t = total(hd[0], nh[0]);
      cTxt("DU", SCR_W - 30, pY + 14, 1, C_GRAY);
      snprintf(buf, sizeof(buf), "%d", t);
      cTxt(buf, SCR_W - 30, pY + 40, 3, t > 21 ? C_RED : (t == 21 ? C_GREEN : C_WHITE));
    } else {
      const int half = (areaR - 6) / 2;
      for (int h = 0; h < 2; h++) {
        const int x0 = 6 + h * (half + 4);
        if (ph == PH_PLAY && act == h) cRR(x0 - 3, pY - 3, half, CH + 6, 6, RGB(40, 70, 40));
        drawHand(hd[h], nh[h], x0, half - 6, pY, h + 1, false);
        const int t = total(hd[h], nh[h]);
        snprintf(buf, sizeof(buf), "H%d %d", h + 1, t);
        cTxt(buf, SCR_W - 30, pY + 18 + h * 24, 1.5, t > 21 ? C_RED : (act == h ? C_GREEN : C_WHITE));
      }
    }
    // Hinweiszeile
    const int hy = BTN_Y() - 10;
    if (ph == PH_PLAY && sv.tip && !fin[act]) {
      snprintf(buf, sizeof(buf), "TIPP: %s", adviceTxt(advice()));
      cTxt(buf, SCR_W / 2, hy, 1.5, C_CYAN);
    } else if (ph == PH_RESULT) {
      const int8_t o = outcome[0];
      const char* w;
      if (nHands == 2) w = net > 0 ? "GEWONNEN" : (net < 0 ? "VERLOREN" : "AUSGEGLICHEN");
      else if (o == 2) w = "BLACK JACK!";
      else if (o == 1) w = total(dl, nd) > 21 ? "BANK UEBERKAUFT" : "GEWONNEN";
      else if (o == 0) w = "UNENTSCHIEDEN";
      else w = total(hd[0], nh[0]) > 21 ? "UEBERKAUFT" : "VERLOREN";
      snprintf(buf, sizeof(buf), "%s  %+ld", w, (long)net);
      cTxt(buf, SCR_W / 2, hy, 1.5, net > 0 ? C_GREEN : (net < 0 ? C_RED : C_WHITE));
    }
    if (ph == PH_PLAY && !fin[act]) {
      Kit::btn(btnRect(0), "KARTE", C_BLUE, C_WHITE, Kit::padMode && sel == 0);
      Kit::btn(btnRect(1), "HALTEN", RGB(170, 40, 40), C_WHITE, Kit::padMode && sel == 1);
      const bool cd = canDouble();
      Kit::btn(btnRect(2), "VERDOPPELN", cd ? RGB(150, 100, 0) : Kit::PANEL, cd ? C_WHITE : C_DIM, Kit::padMode && sel == 2);
      if (canSplit()) Kit::btn(btnRect(3), "TEILEN", C_PURPLE, C_WHITE, Kit::padMode && sel == 3);
    } else if (ph == PH_RESULT) {
      Kit::btn(btnRect(0), "NOCHMAL", C_DGREEN, C_WHITE, Kit::padMode && sel == 0);
      Kit::btn(btnRect(1), "EINSATZ", Kit::PANEL, C_WHITE, Kit::padMode && sel == 1);
    }
    Kit::pauseIcon();
  }

  void loop() {
    if (Kit::pause()) return;
    Kit::step(update, render);
  }
}
