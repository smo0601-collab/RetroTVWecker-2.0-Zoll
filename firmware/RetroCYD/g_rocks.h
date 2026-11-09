// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  ASTRO ROCKS  -  Raumschiff zerschiesst Felsbrocken (Vektorgrafik)
// ============================================================================
//  links/rechts drehen, hoch = Schub, A = feuern (halten = Dauerfeuer),
//  B = Hypersprung (zufaelliger Ort)
// ============================================================================
#pragma once

namespace Rocks {
  struct Rock { float x, y, vx, vy, rot, vr; uint8_t size; bool on; uint8_t shape[9]; };
  struct Shot { float x, y, vx, vy; int life; };
  struct Part { float x, y, vx, vy; int life; uint16_t col; };

  const int MAXR = 28, MAXS = 6, MAXP = 24;
  const float RAD[3] = {22, 13, 7};
  GArr<Rock, MAXR> rocks;
  GArr<Shot, MAXS> shots;
  GArr<Part, MAXP> parts;

  float sx, sy, svx, svy, ang;
  bool alive, thrust;
  int lives, level, respawn, invul, fireCd, levelWait;
  uint32_t score, nextLife;

  void wrap(float& x, float& y, float m) {
    if (x < -m) x += SCR_W + 2 * m;
    if (x > SCR_W + m) x -= SCR_W + 2 * m;
    if (y < -m) y += SCR_H + 2 * m;
    if (y > SCR_H + m) y -= SCR_H + 2 * m;
  }

  void burst(float x, float y, int n, uint16_t col) {
    for (int k = 0; k < n; k++) {
      for (auto& p : parts) {
        if (p.life > 0) continue;
        float a = frand(0, 6.283f), s = frand(0.5f, 3.5f);
        p = {x, y, cosf(a) * s, sinf(a) * s, irand(10, 25), col};
        break;
      }
    }
  }

  void spawnRock(float x, float y, int size) {
    for (auto& r : rocks) {
      if (r.on) continue;
      float a = frand(0, 6.283f);
      float sp = frand(0.6f, 1.4f) * (1.0f + size * 0.45f) * (1.0f + level * 0.08f);
      r.x = x; r.y = y;
      r.vx = cosf(a) * sp; r.vy = sinf(a) * sp;
      r.rot = frand(0, 6.283f); r.vr = frand(-0.06f, 0.06f);
      r.size = size; r.on = true;
      for (int k = 0; k < 9; k++) r.shape[k] = irand(70, 115);
      return;
    }
  }

  void startLevel() {
    for (auto& r : rocks) r.on = false;
    int n = min(3 + level, 8);
    for (int i = 0; i < n; i++) {
      float x, y;
      do { x = frand(0, SCR_W); y = frand(0, SCR_H); } while (dist2(x, y, sx, sy) < 90 * 90);
      spawnRock(x, y, 0);
    }
  }

  void start() {
    rocks.alloc();
    shots.alloc();
    parts.alloc();
    sx = SCR_W / 2; sy = SCR_H / 2; svx = svy = 0; ang = -PI / 2;
    lives = 3; level = 0; score = 0; nextLife = 10000;
    alive = true; invul = 60; respawn = 0; fireCd = 0; levelWait = 0;
    for (auto& s : shots) s.life = 0;
    for (auto& p : parts) p.life = 0;
    startLevel();
  }

  void addScore(uint32_t v) {
    score += v;
    if (score >= nextLife) { lives++; nextLife += 10000; rumble(150, 0x60); }
  }

  void splitRock(Rock& r) {
    // Werte sichern: das erste Bruchstueck kann den frei gewordenen Platz von r belegen
    float x = r.x, y = r.y;
    int size = r.size;
    r.on = false;
    addScore(size == 0 ? 20 : size == 1 ? 50 : 100);
    burst(x, y, 6 + (2 - size) * 3, C_GRAY);
    rumble(40, 0x40);
    if (size < 2) {
      spawnRock(x, y, size + 1);
      spawnRock(x, y, size + 1);
    }
  }

  void killShip() {
    alive = false;
    burst(sx, sy, 20, C_ORANGE);
    burst(sx, sy, 10, C_YELLOW);
    rumble(500, 0xD0);
    lives--;
    respawn = 75;
  }

  void update() {
    if (alive) {
      if (cur.left)  ang -= 0.16f;
      if (cur.right) ang += 0.16f;
      thrust = cur.up;
      if (thrust) { svx += cosf(ang) * 0.24f; svy += sinf(ang) * 0.24f; }
      svx *= 0.988f; svy *= 0.988f;
      float sp = sqrtf(svx * svx + svy * svy);
      if (sp > 7) { svx *= 7 / sp; svy *= 7 / sp; }
      sx += svx; sy += svy;
      wrap(sx, sy, 6);
      if (fireCd > 0) fireCd--;
      if (E.a || (cur.a && fireCd == 0)) {
        for (auto& s : shots) {
          if (s.life > 0) continue;
          s = {sx + cosf(ang) * 10, sy + sinf(ang) * 10, cosf(ang) * 8 + svx, sinf(ang) * 8 + svy, 32};
          fireCd = 7;
          break;
        }
      }
      if (E.b) {   // Hypersprung
        burst(sx, sy, 8, C_CYAN);
        sx = frand(20, SCR_W - 20); sy = frand(30, SCR_H - 20); svx = svy = 0;
        invul = 15;
      }
      if (invul > 0) invul--;
    } else if (respawn > 0) {
      respawn--;
      if (respawn == 0) {
        if (lives <= 0) { gameOver(score); return; }
        bool clear = true;
        for (auto& r : rocks) if (r.on && dist2(r.x, r.y, SCR_W / 2, SCR_H / 2) < 70 * 70) clear = false;
        if (clear) {
          alive = true; sx = SCR_W / 2; sy = SCR_H / 2; svx = svy = 0; ang = -PI / 2; invul = 90;
        } else {
          respawn = 1;   // warten, bis die Mitte frei ist
        }
      }
    }

    for (auto& s : shots) {
      if (s.life <= 0) continue;
      s.x += s.vx; s.y += s.vy; s.life--;
      wrap(s.x, s.y, 0);
    }
    int count = 0;
    for (auto& r : rocks) {
      if (!r.on) continue;
      count++;
      r.x += r.vx; r.y += r.vy; r.rot += r.vr;
      wrap(r.x, r.y, RAD[r.size]);
      float rr = RAD[r.size];
      for (auto& s : shots) {
        if (s.life > 0 && dist2(s.x, s.y, r.x, r.y) < rr * rr) {
          s.life = 0;
          splitRock(r);
          break;
        }
      }
      if (r.on && alive && invul == 0 && dist2(sx, sy, r.x, r.y) < (rr + 5) * (rr + 5)) {
        splitRock(r);
        killShip();
      }
    }
    for (auto& p : parts) {
      if (p.life <= 0) continue;
      p.x += p.vx; p.y += p.vy; p.life--;
    }
    if (count == 0) {
      if (levelWait == 0) levelWait = 45;
      else if (--levelWait == 0) { level++; startLevel(); }
    }
  }

  void render() {
    // Hintergrundsterne (fest)
    for (int i = 0; i < 30; i++) cP((i * 97) % SCR_W, (i * 61 + 17) % SCR_H, C_GRID);

    for (auto& r : rocks) {
      if (!r.on) continue;
      float rr = RAD[r.size];
      if (!cVis((int)(r.y - rr - 2), (int)(2 * rr + 4))) continue;
      int px = 0, py = 0;
      for (int k = 0; k <= 9; k++) {
        int idx = k % 9;
        float a = r.rot + idx * 6.283f / 9;
        float d = rr * r.shape[idx] / 100.0f;
        int x = (int)(r.x + cosf(a) * d), y = (int)(r.y + sinf(a) * d);
        if (k > 0) cL(px, py, x, y, r.size == 0 ? C_GRAY : r.size == 1 ? RGB(170, 170, 190) : C_WHITE);
        px = x; py = y;
      }
    }
    for (auto& s : shots) if (s.life > 0) cR((int)s.x - 1, (int)s.y - 1, 2, 2, C_YELLOW);
    for (auto& p : parts) if (p.life > 0) cP((int)p.x, (int)p.y, p.life > 8 ? p.col : C_DIM);

    if (alive && (invul == 0 || (tick / 3) % 2)) {
      float c = cosf(ang), s = sinf(ang);
      int nx = sx + c * 10, ny = sy + s * 10;
      int lx = sx + cosf(ang + 2.5f) * 8, ly = sy + sinf(ang + 2.5f) * 8;
      int rx = sx + cosf(ang - 2.5f) * 8, ry = sy + sinf(ang - 2.5f) * 8;
      int bx = sx - c * 4, by = sy - s * 4;
      cL(nx, ny, lx, ly, C_CYAN);
      cL(nx, ny, rx, ry, C_CYAN);
      cL(lx, ly, bx, by, C_CYAN);
      cL(rx, ry, bx, by, C_CYAN);
      if (thrust && (tick % 2)) {
        int fx = sx - c * 13, fy = sy - s * 13;
        cL(lx, ly, fx, fy, C_ORANGE);
        cL(rx, ry, fx, fy, C_ORANGE);
      }
    }
    char lv[12];
    snprintf(lv, sizeof(lv), "WELLE %d", level + 1);
    cHud(score, lv);
    for (int i = 0; i < lives && i < 6; i++) {
      int x = 10 + i * 12, y = 22;
      cL(x, y - 5, x - 4, y + 4, C_CYAN);
      cL(x, y - 5, x + 4, y + 4, C_CYAN);
    }
  }

  void loop() {
    if (handlePause(fbNoRedraw)) return;
    fbStep(update, render);
  }
}
