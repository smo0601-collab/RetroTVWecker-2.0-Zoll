#pragma once
/*
  faces_fx.h - gemeinsame Effekt-Bausteine fuer die Arcade-Zifferblaetter
    Sparks<N>   Pixel-Partikel (Funken, Trümmer) mit fester Maximalzahl, ohne Speicherallokation
    Ball        Ballbewegung mit Abprall an Raendern und Rechtecken
    txtS        Arcade-Schrift: Text mit Schlagschatten
*/
#include "faces_core.h"

namespace fc {

struct Spark { float x, y, vx, vy, life; uint8_t col; };

template <int N>
struct Sparks {
  Spark s[N];
  void clear() { for (int i = 0; i < N; i++) s[i].life = 0; }
  void burst(float x, float y, int n, float spd, uint8_t col) {
    for (int i = 0; i < n; i++)
      for (int k = 0; k < N; k++)
        if (s[k].life <= 0) {
          const float a = rf() * 6.2832f, v = spd * (0.4f + rf());
          s[k] = {x, y, cosf(a) * v, sinf(a) * v, 0.35f + rf() * 0.45f, col};
          break;
        }
  }
  void step(float dt, float grav = 0.0f) {
    for (int k = 0; k < N; k++) {
      Spark& p = s[k];
      if (p.life <= 0) continue;
      p.life -= dt; p.vy += grav * dt; p.x += p.vx * dt; p.y += p.vy * dt;
      if (p.x < 0 || p.x >= FB_W || p.y < 0 || p.y >= FB_H) p.life = 0;
    }
  }
  void draw(uint8_t fade = 0) {
    for (int k = 0; k < N; k++) {
      const Spark& p = s[k];
      if (p.life <= 0) continue;
      fb->drawPixel((int)p.x, (int)p.y, (p.life < 0.15f && fade) ? fade : p.col);
    }
  }
};

struct Ball { float x, y, vx, vy; };

// Bewegt den Ball und spiegelt ihn an den Raendern (x0..x1, y0..y1). true = abgeprallt
static bool ballMove(Ball& b, float dt, float x0, float y0, float x1, float y1) {
  b.x += b.vx * dt; b.y += b.vy * dt;
  bool hit = false;
  if (b.x < x0)      { b.x = x0 + (x0 - b.x); b.vx = fabsf(b.vx);  hit = true; }
  else if (b.x > x1) { b.x = x1 - (b.x - x1); b.vx = -fabsf(b.vx); hit = true; }
  if (b.y < y0)      { b.y = y0 + (y0 - b.y); b.vy = fabsf(b.vy);  hit = true; }
  else if (b.y > y1) { b.y = y1 - (b.y - y1); b.vy = -fabsf(b.vy); hit = true; }
  return hit;
}

// Kreis (Radius r) gegen Rechteck: bei Ueberlappung entlang der flachsten Achse spiegeln
static bool ballRect(Ball& b, float r, float rx0, float ry0, float rx1, float ry1) {
  if (b.x + r < rx0 || b.x - r > rx1 || b.y + r < ry0 || b.y - r > ry1) return false;
  const float dl = b.x + r - rx0, dr = rx1 - (b.x - r), dt = b.y + r - ry0, db = ry1 - (b.y - r);
  float m = dl;
  if (dr < m) m = dr;
  if (dt < m) m = dt;
  if (db < m) m = db;
  if (m == dl)      { b.x = rx0 - r; b.vx = -fabsf(b.vx); }
  else if (m == dr) { b.x = rx1 + r; b.vx = fabsf(b.vx); }
  else if (m == dt) { b.y = ry0 - r; b.vy = -fabsf(b.vy); }
  else              { b.y = ry1 + r; b.vy = fabsf(b.vy); }
  return true;
}

// Geschwindigkeit auf sp normieren und fast waagerechte/senkrechte Bahnen vermeiden
static void ballSpeed(Ball& b, float sp) {
  float l = sqrtf(b.vx * b.vx + b.vy * b.vy);
  if (l < 0.001f) { b.vx = sp * 0.6f; b.vy = -sp * 0.8f; return; }
  b.vx *= sp / l; b.vy *= sp / l;
  const float mn = sp * 0.3f;
  if (fabsf(b.vy) < mn) b.vy = b.vy < 0 ? -mn : mn;
  if (fabsf(b.vx) < mn) b.vx = b.vx < 0 ? -mn : mn;
  l = sqrtf(b.vx * b.vx + b.vy * b.vy);
  b.vx *= sp / l; b.vy *= sp / l;
}

// Arcade-Schrift: Text mit Schlagschatten
static void txtS(int x, int y, const char* s, uint8_t col, uint8_t shadow, int sz = 1) {
  txt(x + sz, y + sz, s, shadow, sz);
  txt(x, y, s, col, sz);
}

// Zentrierter Schatten-Text, der bei langen Texten (andere Sprachen) kleiner wird, bis er in maxW passt;
// die Zeile bleibt dabei vertikal mittig an derselben Stelle.
static void txtSC(int cx, int y, const char* s, uint8_t col, uint8_t shadow, int sz = 1, int maxW = 156) {
  int z = sz;
  while (z > 1 && txtw(s, z) > maxW) z--;
  y += (sz - z) * 7 / 2;
  txtS(cx - txtw(s, z) / 2, y, s, col, shadow, z);
}

// Zeitdifferenz in Sekunden als dt (auf 0.1 s begrenzt)
static inline float frameDt(uint32_t now, uint32_t& last) {
  float dt = (float)(uint32_t)(now - last) * 0.001f;
  last = now;
  return dt > 0.1f ? 0.1f : dt;
}

}  // namespace fc
