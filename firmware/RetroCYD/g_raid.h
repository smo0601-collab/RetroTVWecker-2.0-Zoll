// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  JUNGLE RAID  -  Ein Soldat kaempft sich durch den Dschungel nach vorne
// ============================================================================
//  Steuerkreuz/Stick: laufen (8 Richtungen, Blickrichtung = Schussrichtung),
//  A = schiessen, B = Granate werfen (fliegt ueber Hindernisse).
//  Baeume, Felsen und Sandsaecke halten Kugeln auf.
// ============================================================================
#pragma once

namespace Raid {
  const uint16_t SOLD[11] = {0x0038, 0x007C, 0x007C, 0x0038, 0x00FE, 0x017D, 0x017D, 0x007C, 0x006C, 0x006C, 0x00EE};

  enum { TREE, ROCK, SANDBAG };
  struct Obst { float x, y; uint8_t type; bool on; };
  struct Foe { float x, y, vx, vy; int t, shootT, aimT; bool on, guard; };
  struct Bul { float x, y, vx, vy; int life; bool on; };
  struct Nade { float x0, y0, x1, y1; int t; bool on; };
  struct Boom { float x, y; int t; };
  struct Box { float x, y; bool on; };

  const int MAXO = 48, MAXF = 12, MAXB = 8, MAXEB = 12;
  GArr<Obst, MAXO> obst;
  GArr<Foe, MAXF> foes;
  GArr<Bul, MAXB> bul;
  GArr<Bul, MAXEB> ebul;
  Nade nades[3];
  Boom booms[6];
  Box boxes[3];

  float px, py, camY, genY, bestY;
  float aimX, aimY;
  int lives, grenades, invT, deadT, fireCd, spawnT, walk;
  uint32_t score;

  inline int sy(float wy) { return (int)(wy - camY); }

  bool blockedAt(float x, float y, float r) {
    for (auto& o : obst) {
      if (!o.on) continue;
      if (o.type == SANDBAG) {
        if (x + r > o.x - 17 && x - r < o.x + 17 && y + r > o.y - 5 && y - r < o.y + 5) return true;
      } else {
        float rr = (o.type == TREE ? 10 : 8) + r;
        if (dist2(x, y, o.x, o.y) < rr * rr) return true;
      }
    }
    return false;
  }

  void addObst(float x, float y, uint8_t type) {
    for (auto& o : obst) if (!o.on) { o = {x, y, type, true}; return; }
  }

  // Neue Hindernisreihen oberhalb des sichtbaren Bereichs erzeugen
  void generate() {
    while (genY > camY - 60) {
      genY -= 36;
      int n = irand(0, 2);
      for (int k = 0; k < n; k++) {
        float x = frand(14, SCR_W - 14);
        if (fabsf(x - px) < 20 && fabsf(genY - py) < 40) continue;
        int r = irand(0, 9);
        addObst(x, genY + frand(-8, 8), r < 5 ? TREE : r < 8 ? ROCK : SANDBAG);
      }
    }
    for (auto& o : obst) if (o.on && sy(o.y) > SCR_H + 30) o.on = false;
  }

  void spawnFoe() {
    for (auto& f : foes) {
      if (f.on) continue;
      bool side = irand(0, 3) == 0;
      f.x = side ? (irand(0, 1) ? -8 : SCR_W + 8) : frand(16, SCR_W - 16);
      f.y = side ? camY + frand(20, SCR_H * 0.4f) : camY - 12;
      f.vx = side ? (f.x < 0 ? 1.0f : -1.0f) : frand(-0.5f, 0.5f);
      f.vy = side ? 0.2f : frand(0.6f, 1.1f);
      f.t = 0; f.shootT = irand(40, 100); f.aimT = 0; f.on = true; f.guard = false;
      // manche Gegner verschanzen sich hinter Sandsaecken
      for (auto& o : obst) {
        if (o.on && o.type == SANDBAG && sy(o.y) > -20 && sy(o.y) < SCR_H * 0.4f && irand(0, 2) == 0) {
          bool taken = false;
          for (auto& g : foes) if (g.on && g.guard && fabsf(g.x - o.x) < 4 && fabsf(g.y - (o.y - 14)) < 4) taken = true;
          if (taken) continue;
          f.x = o.x; f.y = o.y - 14; f.vx = f.vy = 0; f.guard = true;
          break;
        }
      }
      if (blockedAt(f.x, f.y, 5)) { f.on = false; continue; }
      return;
    }
  }

  void start() {
    obst.alloc();
    foes.alloc();
    bul.alloc();
    ebul.alloc();
    camY = 0; px = SCR_W / 2; py = SCR_H - 30; genY = SCR_H; bestY = py;
    aimX = 0; aimY = -1;
    lives = 3; grenades = 5; invT = 60; deadT = 0; fireCd = 0; spawnT = 60; score = 0; walk = 0;
    for (auto& o : obst) o.on = false;
    for (auto& f : foes) f.on = false;
    for (auto& b : bul) b.on = false;
    for (auto& b : ebul) b.on = false;
    for (auto& n : nades) n.on = false;
    for (auto& b : booms) b.t = 0;
    for (auto& b : boxes) b.on = false;
    genY = camY + SCR_H;
    while (genY > camY - 60) {           // Startbereich locker bestuecken
      genY -= 36;
      if (genY < py - 50 && irand(0, 1)) addObst(frand(14, SCR_W - 14), genY, irand(0, 2) ? TREE : ROCK);
    }
  }

  void killPlayer() {
    if (invT > 0 || deadT > 0) return;
    lives--; deadT = 60;
    rumble(500, 0xC0);
    for (auto& b : booms) if (b.t <= 0) { b = {px, py, 14}; break; }
  }

  void killFoe(Foe& f) {
    f.on = false;
    score += f.guard ? 150 : 100;
    rumble(40, 0x50);
    if (irand(0, 9) == 0)
      for (auto& b : boxes) if (!b.on) { b = {f.x, f.y, true}; break; }
  }

  void explode(float x, float y) {
    for (auto& b : booms) if (b.t <= 0) { b = {x, y, 14}; break; }
    rumble(200, 0x90);
    for (auto& f : foes) if (f.on && dist2(f.x, f.y, x, y) < 28 * 28) killFoe(f);
    for (auto& o : obst) if (o.on && o.type == SANDBAG && dist2(o.x, o.y, x, y) < 26 * 26) o.on = false;
  }

  void update() {
    for (auto& b : booms) if (b.t > 0) b.t--;
    if (deadT > 0) {
      if (--deadT == 0) {
        if (lives <= 0) { gameOver(score); return; }
        invT = 90;
        for (auto& b : ebul) b.on = false;
        // freie Stelle suchen
        for (int k = 0; k < 20 && blockedAt(px, py, 7); k++) px = frand(20, SCR_W - 20);
      }
      return;
    }
    if (invT > 0) invT--;

    // Laufen
    float mx = 0, my = 0;
    if (cur.left) mx = -1;
    if (cur.right) mx = 1;
    if (cur.up) my = -1;
    if (cur.down) my = 1;
    if (mx == 0 && abs(cur.ax) > 300) mx = cur.ax > 0 ? 1 : -1;
    if (my == 0 && abs(cur.ay) > 300) my = cur.ay > 0 ? 1 : -1;
    if (mx || my) {
      float l = sqrtf(mx * mx + my * my);
      aimX = mx / l; aimY = my / l;
      float sp = 1.8f;
      float nx = px + aimX * sp, ny = py + aimY * sp;
      if (!blockedAt(nx, py, 6)) px = nx;
      if (!blockedAt(px, ny, 6)) py = ny;
      px = constrain(px, 8.0f, (float)SCR_W - 8);
      py = min(py, camY + SCR_H - 10);
      walk++;
    }
    // Kamera folgt nach oben
    float lim = camY + SCR_H * 0.45f;
    if (py < lim) camY -= lim - py;
    if (py < bestY - 32) { bestY = py; score += 10; }
    generate();

    // Schiessen und Granaten
    if (fireCd > 0) fireCd--;
    if ((E.a || cur.a) && fireCd == 0) {
      for (auto& b : bul) if (!b.on) { b = {px + aimX * 8, py + aimY * 8, aimX * 6, aimY * 6, 40, true}; fireCd = 7; break; }
    }
    if (E.b && grenades > 0) {
      for (auto& n : nades) {
        if (n.on) continue;
        float ax = aimX, ay = aimY;
        if (ay > 0.5f) { ax = 0; ay = -1; }   // nicht nach hinten werfen
        n = {px, py, px + ax * 80, py + ay * 80, 0, true};
        grenades--;
        break;
      }
    }
    for (auto& b : bul) {
      if (!b.on) continue;
      b.x += b.vx; b.y += b.vy;
      if (--b.life <= 0 || blockedAt(b.x, b.y, 1)) { b.on = false; continue; }
      for (auto& f : foes) if (f.on && dist2(f.x, f.y, b.x, b.y) < 8 * 8) { b.on = false; killFoe(f); break; }
    }
    for (auto& n : nades) {
      if (!n.on) continue;
      if (++n.t >= 24) { n.on = false; explode(n.x1, n.y1); }
    }

    // Gegner
    if (--spawnT <= 0) {
      spawnFoe();
      int lvl = min((int)(-camY / 600), 6);
      spawnT = max(30, 90 - lvl * 9) + irand(0, 40);
    }
    for (auto& f : foes) {
      if (!f.on) continue;
      f.t++;
      if (f.aimT > 0) {
        if (--f.aimT == 0) {
          for (auto& b : ebul) {
            if (b.on) continue;
            float a = atan2f(py - f.y, px - f.x) + frand(-0.15f, 0.15f);
            b = {f.x, f.y, cosf(a) * 3.0f, sinf(a) * 3.0f, 90, true};
            break;
          }
        }
      } else if (!f.guard) {
        if (f.t % 40 == 0) {
          float dx = px - f.x;
          f.vx = constrain(dx / 120.0f, -0.9f, 0.9f) + frand(-0.4f, 0.4f);
          f.vy = (f.y < py - 60) ? frand(0.4f, 1.0f) : frand(-0.3f, 0.3f);
        }
        float nx = f.x + f.vx, ny = f.y + f.vy;
        if (!blockedAt(nx, f.y, 5)) f.x = nx; else f.vx = -f.vx;
        if (!blockedAt(f.x, ny, 5)) f.y = ny; else f.vy = 0;
      }
      if (--f.shootT <= 0 && sy(f.y) > 0 && sy(f.y) < SCR_H) {
        f.aimT = 15;
        f.shootT = irand(60, 130) - min((int)(-camY / 600), 6) * 6;
      }
      if (sy(f.y) > SCR_H + 20 || f.x < -30 || f.x > SCR_W + 30) { f.on = false; continue; }
      if (dist2(f.x, f.y, px, py) < 10 * 10) killPlayer();
    }
    for (auto& b : ebul) {
      if (!b.on) continue;
      b.x += b.vx; b.y += b.vy;
      if (--b.life <= 0 || blockedAt(b.x, b.y, 1)) { b.on = false; continue; }
      if (dist2(b.x, b.y, px, py) < 6 * 6) { b.on = false; killPlayer(); }
    }
    for (auto& b : boxes) {
      if (!b.on) continue;
      if (sy(b.y) > SCR_H + 10) b.on = false;
      else if (dist2(b.x, b.y, px, py) < 12 * 12) { b.on = false; grenades = min(grenades + 3, 9); score += 50; rumble(80, 0x50); }
    }
  }

  void render() {
    canvas.fillScreen(RGB(50, 100, 45));
    // Gras-Tupfer abhaengig von der Weltposition
    int gy0 = ((int)camY / 24) * 24;
    for (int gy = gy0 - 24; gy < camY + SCR_H + 24; gy += 24)
      for (int gx = 0; gx < SCR_W; gx += 24) {
        uint32_t h = (uint32_t)(gx * 73856093u) ^ (uint32_t)(gy * 19349663u);
        int ox = h % 20, oy = (h >> 8) % 20;
        cR(gx + ox, sy(gy + oy), 3, 2, RGB(70, 130, 60));
        if ((h >> 16) % 4 == 0) cR(gx + oy, sy(gy + ox), 5, 3, RGB(110, 90, 50));
      }
    for (auto& b : boxes) if (b.on) { cR((int)b.x - 6, sy(b.y) - 5, 12, 10, RGB(120, 100, 40)); cTxt("G", (int)b.x, sy(b.y), 1, C_YELLOW); }
    for (auto& o : obst) {
      if (!o.on) continue;
      int x = (int)o.x, y = sy(o.y);
      if (o.type == SANDBAG) {
        for (int k = 0; k < 4; k++) cRR(x - 17 + k * 9, y - 5, 9, 10, 3, RGB(190, 170, 110));
        for (int k = 0; k < 3; k++) cRR(x - 13 + k * 9, y - 9, 9, 6, 2, RGB(170, 150, 95));
      } else if (o.type == ROCK) {
        cC(x, y, 8, C_GRAY);
        cC(x - 2, y - 2, 4, RGB(170, 170, 180));
      }
    }
    for (auto& f : foes) {
      if (!f.on) continue;
      int x = (int)f.x, y = sy(f.y);
      cSpr(SOLD, 9, 11, x - 4, y - 6, f.aimT > 0 ? C_ORANGE : RGB(170, 140, 90));
      float a = atan2f(py - f.y, px - f.x);
      cL(x, y, x + (int)(cosf(a) * 8), y + (int)(sinf(a) * 8), C_BG);
    }
    for (auto& n : nades) {
      if (!n.on) continue;
      float t = n.t / 24.0f;
      int x = (int)(n.x0 + (n.x1 - n.x0) * t), y = sy(n.y0 + (n.y1 - n.y0) * t) - (int)(sinf(t * PI) * 20);
      cC(x, y, 3, RGB(40, 60, 30));
    }
    // Spieler
    if (deadT == 0 && (invT == 0 || (tick / 3) % 2)) {
      int x = (int)px, y = sy(py);
      cSpr(SOLD, 9, 11, x - 4, y - 6, RGB(60, 200, 220));
      cR(x - 3, y - 6, 7, 2, C_RED);          // Stirnband
      cL(x, y, x + (int)(aimX * 10), y + (int)(aimY * 10), C_BG);
      if ((walk / 4) % 2) cP(x - 2, y + 5, C_BG);
    }
    for (auto& b : bul) if (b.on) cR((int)b.x - 1, sy(b.y) - 1, 2, 2, C_YELLOW);
    for (auto& b : ebul) if (b.on) cR((int)b.x - 1, sy(b.y) - 1, 3, 3, C_PINK);
    // Baumkronen ueber allem
    for (auto& o : obst) {
      if (!o.on || o.type != TREE) continue;
      int x = (int)o.x, y = sy(o.y);
      cC(x, y, 11, RGB(25, 75, 30));
      cC(x - 3, y - 3, 6, RGB(40, 110, 45));
    }
    for (auto& b : booms) {
      if (b.t <= 0) continue;
      int r = (14 - b.t) * 2 + 4;
      cC((int)b.x, sy(b.y), r, b.t > 7 ? C_YELLOW : C_ORANGE);
      cCO((int)b.x, sy(b.y), r + 3, C_RED);
    }
    cHud(score);
    for (int i = 0; i < grenades; i++) cC(10 + i * 9, 22, 3, RGB(40, 60, 30));
    for (int i = 0; i < lives - 1 && i < 4; i++) cSpr(SOLD, 9, 11, SCR_W - 12 - i * 12, 16, RGB(60, 200, 220));
    if (tick < 60) cTxt("VORWAERTS!", SCR_W / 2, SCR_H / 2, 2, C_YELLOW);
  }

  void loop() {
    if (handlePause(fbNoRedraw)) return;
    fbStep(update, render);
  }
}
