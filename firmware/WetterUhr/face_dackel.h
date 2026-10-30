#pragma once
/*
  face_dackel.h - Zifferblatt "Dackelwelpe"
  Ein schwarz-brauner Dackelwelpe (Kurzhaar, Black & Tan) jagt auf der Wiese einem Ball nach,
  stupst ihn mit der Nase an, setzt sich zwischendurch und wedelt, schnueffelt am Boden und
  bellt zur vollen Minute "WUFF!". Szene, Ball und Verhalten kommen aus pet.h.
*/
#include "pet.h"

namespace f_dack {
using namespace pet;

enum : uint8_t { D_K = P_BREED, D_K2, D_TAN, D_TAN2, D_EYE, D_NOSE };

static void enter(uint32_t now) {
  pet::enter(now);
  pal(D_K, 34, 28, 30); pal(D_K2, 66, 58, 62);
  pal(D_TAN, 196, 112, 40); pal(D_TAN2, 150, 78, 24);
  pal(D_EYE, 40, 20, 10); pal(D_NOSE, 12, 10, 12);
}

static void head(const M& m, int hx, int hy, bool open, bool bark) {
  m.circ(hx, hy, 5, D_K);                                           // Schaedel
  m.rect(hx + 2, hy - 1, 8, 5, D_TAN);                              // Schnauze
  m.rect(hx + 8, hy - 1, 2, 2, D_NOSE);                             // Nase
  m.rect(hx + 3, hy + 3, 6, 1, D_TAN2);                             // Unterkiefer
  if (open || bark) {
    m.rect(hx + 4, hy + 3, 5, 2, D_NOSE);
    m.rect(hx + 5, hy + 3, 3, 3, P_TONGUE);
  }
  m.rect(hx + 1, hy - 3, 2, 2, D_EYE);                              // Auge
  m.px(hx + 1, hy - 3, I_WHITE);
  m.rect(hx - 1, hy - 5, 3, 1, D_TAN);                              // brauner Augenpunkt
  m.rect(hx - 6, hy - 3, 5, 9, D_K);                                // Schlappohr
  m.circ(hx - 4, hy + 6, 2, D_K);
  m.rect(hx - 5, hy - 2, 1, 6, D_K2);
}

static void dogDraw(int x, int y, int d, uint8_t mode, float walk, float tail, bool moving, bool bark) {
  const M m{x, y, d};
  const int sw = moving ? (int)lroundf(sinf(walk) * 3.0f) : 0;
  const int wag = (int)lroundf(sinf(tail) * 3.0f);

  if (mode == M_SIT) {
    m.thick(-17, -3, -22 + wag, -2 + (wag > 0 ? 0 : 1), D_K);                 // Rute auf dem Boden
    m.tri(-13, -10, 6, -20, 8, -12, D_K);                                      // aufgerichteter Rumpf
    m.tri(-13, -10, 8, -12, -8, -2, D_K);
    m.circ(-10, -5, 5, D_K);                                                   // Hinterteil am Boden
    m.rect(-12, -1, 9, 1, D_K);
    m.rect(-9, -2, 5, 2, D_TAN2);                                              // Hinterpfote
    m.rect(5, -13, 3, 13, D_K);                                                // Vorderbeine
    m.rect(8, -13, 3, 13, D_K);
    m.rect(5, -1, 6, 1, D_TAN);
    m.rect(7, -14, 4, 5, D_TAN2);                                              // Brust
    head(m, 10, -25, true, bark);
    return;
  }
  // Beine
  m.rect(-14 + sw, -4, 3, 4, D_K); m.rect(-14 + sw, -1, 3, 1, D_TAN);
  m.rect(-9 - sw, -4, 3, 4, D_K);  m.rect(-9 - sw, -1, 3, 1, D_TAN);
  m.rect(3 - sw, -4, 3, 4, D_K);   m.rect(3 - sw, -1, 3, 1, D_TAN);
  m.rect(8 + sw, -4, 3, 4, D_K);   m.rect(8 + sw, -1, 3, 1, D_TAN);
  // Rumpf, lang und tief
  m.rect(-15, -11, 25, 8, D_K);
  m.circ(-15, -7, 4, D_K);
  m.circ(10, -7, 4, D_K);
  m.rect(-12, -11, 20, 1, D_K2);                                               // Glanz auf dem Ruecken
  m.rect(5, -5, 6, 3, D_TAN2);                                                 // Brust
  m.rect(-8, -4, 12, 1, D_TAN2);                                               // Bauchkante
  // Rute
  m.thick(-18, -9, -23, -12 + wag, D_K);
  // Hals und Kopf
  if (mode == M_SNIFF) {
    m.thick(8, -9, 13, -4, D_K);
    head(m, 16, -3, false, false);
  } else {
    m.circ(9, -9, 4, D_K);
    head(m, 14, -14 - (moving && sw > 0 ? 1 : 0), moving, bark);
  }
}

static void draw(const FaceCtx& c) { pet::draw(c, dogDraw); }
static void cover(float p, bool in, uint32_t now) { pet::cover(p, in, now); }

}  // namespace f_dack
