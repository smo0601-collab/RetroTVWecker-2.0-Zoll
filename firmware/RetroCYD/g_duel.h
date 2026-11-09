// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  DOJO DUEL  -  Kampfsport-Duell: 1 Spieler gegen den Computer (immer
//                staerkere Gegner) oder 2 Spieler mit zwei Controllern
// ============================================================================
//  links/rechts laufen (vom Gegner weg = blocken), hoch = springen,
//  runter = ducken, A = Schlag, B = Tritt, X = Energiekugel
//  Zwei Gewinnrunden entscheiden den Kampf.
// ============================================================================
#pragma once

namespace Duel {
  enum { IDLE, WALK, CROUCH, JUMP, PUNCH, KICK, SPECIAL, BLOCK, HIT, KO, WIN };
  struct Fighter {
    float x, y, vx, vy;
    int face, hp, st, stT, specCd, wins;
    bool landed, cpu;
    uint16_t col, belt;
    int aiT, aiMove;
  };
  struct Ball { float x, y, vx; int owner; bool on; };

  Fighter F[2];
  Ball balls[2];
  int round, roundT, endT, introT, level, floorY;
  bool twoP;
  uint32_t score;
  char banner[24];
  int bannerT;

  const uint16_t CPU_COLS[5] = {C_RED, C_ORANGE, C_PURPLE, C_GREEN, C_PINK};

  void resetRound() {
    floorY = SCR_H - 22;
    for (int i = 0; i < 2; i++) {
      Fighter& f = F[i];
      f.x = i == 0 ? SCR_W * 0.25f : SCR_W * 0.75f;
      f.y = floorY; f.vx = f.vy = 0;
      f.face = i == 0 ? 1 : -1;
      f.hp = 100; f.st = IDLE; f.stT = 0; f.specCd = 0; f.landed = false; f.aiT = 0; f.aiMove = 0;
    }
    for (auto& b : balls) b.on = false;
    roundT = 60 * 30;
    endT = 0;
    introT = 50;
    snprintf(banner, sizeof(banner), "RUNDE %d", round);
    bannerT = 50;
  }

  void newMatch() {
    round = 1;
    F[0].wins = F[1].wins = 0;
    F[0].col = C_CYAN; F[0].belt = C_BLUE; F[0].cpu = false;
    F[1].col = twoP ? C_WHITE : CPU_COLS[level % 5];
    F[1].belt = twoP ? C_RED : C_DIM;
    F[1].cpu = !twoP;
    resetRound();
  }

  void start() {
    twoP = numPlayers >= 2;
    level = 0; score = 0;
    newMatch();
  }

  const Keys& keysFor(int i) { return twoP ? pk[i] : cur; }
  const Keys& edgesFor(int i) { return twoP ? PE[i] : E; }

  bool busy(const Fighter& f) {
    return f.st == PUNCH || f.st == KICK || f.st == SPECIAL || f.st == HIT || f.st == KO || f.st == WIN;
  }

  void act(Fighter& f, int st) {
    f.st = st; f.stT = 0; f.landed = false;
  }

  // Eingaben als Bits: 1 links 2 rechts 4 hoch 8 runter 16 A 32 B 64 X
  void control(int i, int in) {
    Fighter& f = F[i];
    if (busy(f) || f.st == JUMP) return;
    bool back = (f.face > 0 && (in & 1)) || (f.face < 0 && (in & 2));
    if (in & 16) { act(f, PUNCH); return; }
    if (in & 32) { act(f, KICK); return; }
    if ((in & 64) && f.specCd == 0) { act(f, SPECIAL); f.specCd = 90; return; }
    if (in & 4) { act(f, JUMP); f.vy = -7.5f; f.vx = (in & 1) ? -2.6f : (in & 2) ? 2.6f : 0; return; }
    if (in & 8) { f.st = CROUCH; f.vx = 0; return; }
    if (in & 3) {
      f.vx = (in & 1) ? -2.2f : 2.2f;
      if (back) f.vx *= 0.7f;
      f.st = back ? BLOCK : WALK;
      if (back && fabsf(F[1 - i].x - f.x) > 70 && !balls[1 - i].on) f.st = WALK;
      return;
    }
    f.vx = 0; f.st = IDLE;
  }

  int cpuInput(int i) {
    Fighter& f = F[i];
    Fighter& o = F[1 - i];
    if (--f.aiT > 0) return f.aiMove;
    float d = fabsf(o.x - f.x);
    int toward = o.x > f.x ? 2 : 1, away = toward == 2 ? 1 : 2;
    int skill = min(level, 8);
    f.aiT = max(3, 12 - skill) + irand(0, 6);
    int r = irand(0, 99);
    bool threat = o.st == PUNCH || o.st == KICK || balls[1 - i].on;
    int m;
    if (threat && r < 25 + skill * 7) m = (balls[1 - i].on && r % 2) ? 8 : away;
    else if (d > 90) m = (r < 20 + skill * 3 && f.specCd == 0) ? 64 : (r < 30 ? 4 | toward : toward);
    else if (d > 34) m = r < 50 ? toward : (r < 75 ? 32 : 4 | toward);
    else m = r < 45 ? 16 : r < 75 ? 32 : r < 88 ? away : 8;
    f.aiMove = m;
    return m;
  }

  int playerInput(int i) {
    const Keys& k = keysFor(i);
    const Keys& e = edgesFor(i);
    return (k.left ? 1 : 0) | (k.right ? 2 : 0) | (e.up ? 4 : 0) | (k.down ? 8 : 0) |
           (e.a ? 16 : 0) | (e.b ? 32 : 0) | (e.x ? 64 : 0);
  }

  void damage(int target, int dmg, float push) {
    Fighter& t = F[target];
    Fighter& a = F[1 - target];
    bool blocking = t.st == BLOCK || (t.st == CROUCH && dmg <= 6);
    if (blocking) {
      t.hp -= 1; t.x += push * a.face * 0.5f;
      rumbleFor(twoP ? target : 0, 40, 0x40);
    } else {
      t.hp -= dmg;
      act(t, HIT);
      t.vx = push * a.face;
      if (target == 1 && !twoP) score += dmg * 10;
      rumbleFor(twoP ? target : 0, 120, 0x90);
    }
    if (t.hp <= 0) { t.hp = 0; act(t, KO); t.vy = -4; t.vx = a.face * 3; }
  }

  void updateFighter(int i) {
    Fighter& f = F[i];
    Fighter& o = F[1 - i];
    f.stT++;
    if (f.specCd > 0) f.specCd--;
    if (!busy(f) && f.st != JUMP) f.face = o.x > f.x ? 1 : -1;

    float d = fabsf(o.x - f.x);
    switch (f.st) {
      case PUNCH:
        f.vx = 0;
        if (!f.landed && f.stT >= 4 && f.stT <= 7 && d < 34 && fabsf(o.y - f.y) < 40) { f.landed = true; damage(1 - i, 6, 3); }
        if (f.stT >= 12) f.st = IDLE;
        break;
      case KICK:
        f.vx = 0;
        if (!f.landed && f.stT >= 7 && f.stT <= 11 && d < 44 && fabsf(o.y - f.y) < 40) { f.landed = true; damage(1 - i, 9, 5); }
        if (f.stT >= 18) f.st = IDLE;
        break;
      case SPECIAL:
        f.vx = 0;
        if (f.stT == 8 && !balls[i].on) balls[i] = {f.x + f.face * 20, f.y - 36, f.face * 5.0f, i, true};
        if (f.stT >= 20) f.st = IDLE;
        break;
      case HIT:
        f.vx *= 0.8f;
        if (f.stT >= 14) f.st = IDLE;
        break;
      case JUMP:
        if (!f.landed && f.vy > -2 && d < 40 && fabsf(o.y - (f.y + 20)) < 50 && (keysFor(i).a || keysFor(i).b || (f.cpu && irand(0, 6) == 0))) {
          f.landed = true; damage(1 - i, 8, 4);
        }
        break;
      case KO:
        f.vx *= 0.95f;
        break;
    }
    // Physik
    if (f.st == JUMP || f.st == KO || f.y < floorY) {
      f.vy += 0.45f;
      f.y += f.vy;
      if (f.y >= floorY) { f.y = floorY; f.vy = 0; if (f.st == JUMP) f.st = IDLE; }
    }
    f.x += f.vx;
    f.x = constrain(f.x, 16.0f, (float)SCR_W - 16);
    // nicht ineinander laufen
    if (fabsf(F[0].x - F[1].x) < 22 && F[0].y > floorY - 30 && F[1].y > floorY - 30) {
      float mid = (F[0].x + F[1].x) / 2, s = F[0].x < F[1].x ? -1 : 1;
      F[0].x = mid + s * 11; F[1].x = mid - s * 11;
    }
  }

  void endRound(int winner) {
    endT = 90;
    if (winner >= 0) {
      F[winner].wins++;
      if (F[winner].st != KO) act(F[winner], WIN);
      if (twoP) snprintf(banner, sizeof(banner), "SPIELER %d!", winner + 1);
      else snprintf(banner, sizeof(banner), "%s", winner == 0 ? "DU GEWINNST!" : "VERLOREN!");
      if (winner == 0 && !twoP) score += 500 + roundT / 3;
    } else {
      snprintf(banner, sizeof(banner), "UNENTSCHIEDEN");
    }
    bannerT = 90;
  }

  void update() {
    if (bannerT > 0) bannerT--;
    if (endT > 0) {
      for (int i = 0; i < 2; i++) updateFighter(i);
      if (--endT == 0) {
        if (F[0].wins >= 2 || F[1].wins >= 2) {
          if (twoP) { gameOverText(F[0].wins >= 2 ? "SPIELER 1 SIEGT" : "SPIELER 2 SIEGT", 0); return; }
          if (F[1].wins >= 2) { gameOver(score); return; }
          score += 2000; level++;
          newMatch();
          snprintf(banner, sizeof(banner), "GEGNER %d", level + 1);
          bannerT = 60;
        } else {
          round++;
          resetRound();
        }
      }
      return;
    }
    if (introT > 0) { introT--; return; }
    if (roundT > 0) roundT--;

    for (int i = 0; i < 2; i++) control(i, F[i].cpu ? cpuInput(i) : playerInput(i));
    for (int i = 0; i < 2; i++) updateFighter(i);

    for (auto& b : balls) {
      if (!b.on) continue;
      b.x += b.vx;
      if (b.x < -10 || b.x > SCR_W + 10) { b.on = false; continue; }
      Ball& o = balls[1 - b.owner];
      if (o.on && fabsf(o.x - b.x) < 10) { b.on = false; o.on = false; continue; }
      Fighter& t = F[1 - b.owner];
      bool ducked = t.st == CROUCH;
      if (!ducked && t.st != KO && fabsf(t.x - b.x) < 14 && fabsf((t.y - 36) - b.y) < 20) {
        b.on = false;
        damage(1 - b.owner, 12, 4);
      }
    }
    if (F[0].hp <= 0 || F[1].hp <= 0) endRound(F[0].hp <= 0 ? (F[1].hp <= 0 ? -1 : 1) : 0);
    else if (roundT == 0) endRound(F[0].hp == F[1].hp ? -1 : F[0].hp > F[1].hp ? 0 : 1);
  }

  void limb(int x0, int y0, int x1, int y1, uint16_t c) {
    cL(x0, y0, x1, y1, c);
    cL(x0 + 1, y0, x1 + 1, y1, c);
    cL(x0, y0 + 1, x1, y1 + 1, c);
  }

  void drawFighter(const Fighter& f) {
    int x = (int)f.x, y = (int)f.y, s = f.face;
    uint16_t c = f.col, skin = RGB(240, 190, 150);
    if (f.st == HIT && (f.stT / 2) % 2) c = C_WHITE;
    if (f.st == KO && f.y >= floorY) {   // liegt am Boden
      limb(x - 20, y - 3, x + 14, y - 3, c);
      cC(x + 20 * s, y - 5, 5, skin);
      return;
    }
    int hipY = y - 24, shY = y - 46, headY = y - 54;
    if (f.st == CROUCH) { hipY = y - 14; shY = y - 32; headY = y - 40; }
    if (f.st == JUMP) { hipY = y - 26; }
    int lean = f.st == HIT ? -s * 5 : 0;
    // Beine
    if (f.st == KICK && f.stT >= 5 && f.stT <= 13) {
      limb(x, hipY, x - s * 8, y, c);
      limb(x, hipY, x + s * 32, hipY - 6, c);
      cR(x + s * 32 - 2, hipY - 8, 5, 4, skin);
    } else if (f.st == JUMP) {
      limb(x, hipY, x + s * 8, hipY + 10, c); limb(x + s * 8, hipY + 10, x, hipY + 18, c);
      limb(x, hipY, x - s * 6, hipY + 12, c); limb(x - s * 6, hipY + 12, x - s * 10, hipY + 18, c);
    } else {
      int step = (f.st == WALK) ? (int)(sinf(f.x * 0.25f) * 6) : 0;
      int spread = f.st == CROUCH ? 12 : 8;
      limb(x, hipY, x - spread + step, y, c);
      limb(x, hipY, x + spread - step, y, c);
    }
    // Koerper und Guertel
    limb(x, hipY, x + lean, shY, c);
    cR(x - 7 + lean, shY, 14, hipY - shY, c);
    cR(x - 7 + lean / 2, hipY - 4, 14, 3, f.belt);
    // Kopf mit Stirnband
    cC(x + lean, headY, 7, skin);
    cR(x - 7 + lean, headY - 4, 14, 2, f.belt);
    cP(x + lean + s * 3, headY - 1, C_BG);
    // Arme
    int ax = x + lean, ay = shY + 3;
    if (f.st == PUNCH && f.stT >= 3 && f.stT <= 9) {
      limb(ax, ay, ax + s * 28, ay - 2, c);
      cC(ax + s * 29, ay - 2, 3, skin);
      limb(ax, ay, ax + s * 6, ay + 10, c);
    } else if (f.st == SPECIAL) {
      int ext = f.stT >= 6 ? 22 : 8;
      limb(ax, ay, ax + s * ext, ay - 1, c); limb(ax, ay + 3, ax + s * ext, ay + 4, c);
      cC(ax + s * (ext + 2), ay + 1, 4, skin);
    } else if (f.st == BLOCK) {
      limb(ax, ay, ax + s * 10, ay - 10, c); limb(ax, ay, ax + s * 12, ay - 4, c);
      cC(ax + s * 11, ay - 10, 3, skin);
    } else if (f.st == WIN) {
      limb(ax, ay, ax + 4, ay - 20, c); cC(ax + 4, ay - 22, 3, skin);
      limb(ax, ay, ax - s * 8, ay + 10, c);
    } else {
      limb(ax, ay, ax + s * 9, ay + 6, c); limb(ax + s * 9, ay + 6, ax + s * 12, ay - 4, c);
      cC(ax + s * 12, ay - 5, 3, skin);
      limb(ax, ay, ax + s * 3, ay + 9, c); limb(ax + s * 3, ay + 9, ax + s * 8, ay + 2, c);
    }
  }

  void bar(int x, int w, int hp, bool rightAlign, uint16_t col) {
    cRO(x, 18, w, 8, C_WHITE);
    int fill = (w - 2) * hp / 100;
    cR(rightAlign ? x + w - 1 - fill : x + 1, 19, fill, 6, hp > 30 ? col : C_RED);
  }

  void render() {
    // Dojo-Hintergrund
    cR(0, floorY - 90, SCR_W, 6, RGB(90, 50, 30));
    for (int x = 10; x < SCR_W; x += 46) { cR(x, floorY - 84, 30, 50, RGB(230, 210, 160)); cRO(x, floorY - 84, 30, 50, RGB(90, 50, 30)); cV(x + 15, floorY - 84, 50, RGB(90, 50, 30)); }
    cR(0, floorY, SCR_W, SCR_H - floorY, RGB(150, 100, 50));
    for (int x = 0; x < SCR_W; x += 40) cV(x, floorY, SCR_H - floorY, RGB(120, 75, 35));

    for (auto& b : balls) {
      if (!b.on) continue;
      cC((int)b.x, (int)b.y, 7, b.owner == 0 ? C_CYAN : C_ORANGE);
      cC((int)b.x, (int)b.y, 3, C_WHITE);
      cC((int)(b.x - b.vx * 2), (int)b.y, 4, b.owner == 0 ? C_BLUE : C_RED);
    }
    drawFighter(F[1]);
    drawFighter(F[0]);

    int bw = SCR_W / 2 - 34;
    bar(6, bw, F[0].hp, false, C_YELLOW);
    bar(SCR_W - 6 - bw, bw, F[1].hp, true, C_YELLOW);
    char t[12];
    snprintf(t, sizeof(t), "%d", (roundT + 29) / 30);
    cTxt(t, SCR_W / 2, 22, 2, C_WHITE);
    for (int i = 0; i < F[0].wins; i++) cC(10 + i * 10, 32, 3, C_YELLOW);
    for (int i = 0; i < F[1].wins; i++) cC(SCR_W - 10 - i * 10, 32, 3, C_YELLOW);
    if (twoP) cTxt("1P  vs  2P", SCR_W / 2, 4, 1, C_CYAN, textdatum_t::top_center);
    else cHud(score);
    if (bannerT > 0) cTxt(banner, SCR_W / 2, SCR_H / 2 - 40, 2, C_YELLOW);
    else if (introT > 0) cTxt("KAMPF!", SCR_W / 2, SCR_H / 2 - 40, 2, C_RED);
  }

  void loop() {
    if (handlePause(fbNoRedraw)) return;
    fbStep(update, render);
  }
}
