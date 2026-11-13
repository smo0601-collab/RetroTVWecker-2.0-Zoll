// Teil von RetroCYD - wird von RetroCYD.ino eingebunden
// ============================================================================
//  SPURWECHSEL  -  Rennspiel von oben: auf drei Spuren dem Verkehr ausweichen
// ============================================================================
//  Linke Bildhaelfte antippen = eine Spur nach links, rechte = nach rechts
//  (oder wischen / Steuerkreuz). Das Tempo steigt.
//  Tank: Kanister einsammeln, sonst bleibt das Auto stehen. Muenzen werden
//  gespart; damit kauft man im Startbildschirm neue Autos. Nach einer Weile
//  wird es Nacht - dann leuchten nur noch die Scheinwerfer den Weg aus.
//  Motorton (Lautsprecher-Anschluss des CYD) im Startbildschirm einschaltbar.
// ============================================================================
#pragma once

namespace Lanes {
  enum Phase : uint8_t { PH_PICK, PH_DRIVE, PH_CRASH, PH_EMPTY };
  Phase ph;
  Kit::Picker pick;
  char carLbl[32], sndLbl[24], note[40];

  // Autos 12 x 20 Pixel (doppelt gezeichnet = 24 x 40), X = Lackfarbe, x = dunkler Lack
  const char* const CAR_KOMPAKT[20] = {
    "...XXXXXX...",
    "..XXXXXXXX..",
    ".XyXXXXXXyX.",
    "kXXXXXXXXXXk",
    "kXXccccccXXk",
    "kXccccccccXk",
    ".XccccccccX.",
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    ".XwXXXXXXXX.",
    ".XwXXXXXXXX.",
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    ".XccccccccX.",
    "kXXccccccXXk",
    "kXXXXXXXXXXk",
    "kXXXXXXXXXXk",
    ".XXXXXXXXXX.",
    ".XrrXXXXrrX.",
    "..XXXXXXXX.."};
  const char* const CAR_SPORT[20] = {
    "....XXXX....",
    "...XXXXXX...",
    "..yXXwwXXy..",
    ".XXXXwwXXXX.",
    "kXXXXwwXXXXk",
    "kXXccwwccXXk",
    "kXccccccccXk",
    ".XccccccccX.",
    ".XXXXwwXXXX.",
    ".XXXXwwXXXX.",
    ".XXXXwwXXXX.",
    ".XXXXwwXXXX.",
    ".XXccwwccXX.",
    "kXXXXwwXXXXk",
    "kXXXXwwXXXXk",
    "kXXXXwwXXXXk",
    ".XXXXwwXXXX.",
    "kkkkkkkkkkkk",
    "kXrrXXXXrrXk",
    "............"};
  const char* const CAR_PICKUP[20] = {
    "..XXXXXXXX..",
    ".XXXXXXXXXX.",
    ".yXXXXXXXXy.",
    "kXXXXXXXXXXk",
    "kXXXXXXXXXXk",
    "kXccccccccXk",
    ".XccccccccX.",
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    ".XccccccccX.",
    ".XXXXXXXXXX.",
    ".XxxxxxxxxX.",
    "kXxeeeeeexXk",
    "kXxeeeeeexXk",
    ".XxeeeeeexX.",
    ".XxeeeeeexX.",
    "kXxeeeeeexXk",
    "kXxxxxxxxxXk",
    ".XrXXXXXXrX.",
    ".XXXXXXXXXX."};
  const char* const CAR_RENNER[20] = {
    ".....XX.....",
    ".....XX.....",
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    "kk..XXXX..kk",
    "kk..XXXX..kk",
    "kk..XwwX..kk",
    "....XwwX....",
    "...XXccXX...",
    "...XccccX...",
    "...XkkkkX...",
    "...XXXXXX...",
    "...XXwwXX...",
    "kk.XXwwXX.kk",
    "kk.XXXXXX.kk",
    "kk.XXXXXX.kk",
    "kk..XXXX..kk",
    "....XXXX....",
    ".XXXXXXXXXX.",
    ".XrXXXXXXrX."};
  const char* const CAR_LIMO[20] = {
    "...XXXXXX...",
    "..XXXXXXXX..",
    ".XXXXXXXXXX.",
    "kXXXXXXXXXXk",
    "kXXXXXXXXXXk",
    "kXccccccccXk",
    ".XccccccccX.",
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    ".XccccccccX.",
    "kXccccccccXk",
    "kXXXXXXXXXXk",
    "kXXXXXXXXXXk",
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    ".XrrXXXXrrX.",
    "..XXXXXXXX.."};
  const char* const CAR_LIEFERWAGEN[20] = {
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    "kXccccccccXk",
    "kXccccccccXk",
    ".XXXXXXXXXX.",
    ".XxxxxxxxxX.",
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    "kXXXXXXXXXXk",
    "kXXXXXXXXXXk",
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    "kXXXXXXXXXXk",
    "kXXXXXXXXXXk",
    ".XXXXXXXXXX.",
    ".XXXXXXXXXX.",
    ".XrXXXXXXrX.",
    ".XXXXXXXXXX."};
  const char* const CAR_LKW[20] = {
    "..XXXXXXXX..",
    ".XXXXXXXXXX.",
    "kXccccccccXk",
    "kXXXXXXXXXXk",
    ".XXXXXXXXXX.",
    "..kkkkkkkk..",
    ".wwwwwwwwww.",
    ".weeeeeeeew.",
    "kweeeeeeeewk",
    "kweeeeeeeewk",
    ".weeeeeeeew.",
    ".weeeeeeeew.",
    ".weeeeeeeew.",
    ".weeeeeeeew.",
    ".weeeeeeeew.",
    "kweeeeeeeewk",
    "kweeeeeeeewk",
    ".weeeeeeeew.",
    ".wrwwwwwwrw.",
    ".wwwwwwwwww."};

  struct Model { const char* name; const char* const* bmp; uint16_t price, col; float laneV; };
  const Model CARS[4] = {
    {"KOMPAKT", CAR_KOMPAKT, 0,   RGB(230, 40, 40),  7.5f},
    {"SPORT",   CAR_SPORT,   60,  RGB(255, 210, 0),  9.5f},
    {"PICKUP",  CAR_PICKUP,  150, RGB(40, 100, 255), 8.0f},
    {"RENNER",  CAR_RENNER,  300, RGB(240, 240, 240), 11.0f},
  };
  const char* const* TRAFFIC[3] = {CAR_LIMO, CAR_LIEFERWAGEN, CAR_LKW};
  const uint16_t TCOL[6] = {RGB(60, 180, 90), RGB(200, 120, 40), RGB(180, 60, 200), RGB(90, 160, 230), RGB(220, 220, 220), RGB(150, 150, 60)};

  // Fahrbahn
  const int LW = 44, CAR_W = 24, CAR_H = 40;
  int RX, PY;                       // linker Fahrbahnrand, Auto-Oberkante
  // Auto
  uint8_t model = 0, owned = 1;
  bool sound = false;
  uint16_t coinsBank = 0;
  int lane, targetLane;
  float px;
  float speed, fuel, dist, tv;      // tv = Tempo des Verkehrs
  uint16_t coinsRun;
  uint32_t t;                       // Spielschritte
  int endT;
  // Verkehr, Gegenstaende, Landschaft
  struct Car { float y; uint8_t lane, kind; uint16_t col; bool on; };
  Car cars[8];
  struct Item { float y; uint8_t lane, kind; bool on; };   // kind 0 = Muenze, 1 = Kanister
  Item items[4];
  struct Deco { float y; int16_t x; uint8_t kind; };
  Deco deco[12];
  float spawnAcc, gap;
  int lastFree = 1;

  int laneX(int l) { return RX + l * LW + (LW - CAR_W) / 2; }

  void loadGarage() {
    owned = prefs.getUChar("ln_own", 1) | 1;
    coinsBank = prefs.getUShort("ln_coin", 0);
    model = prefs.getUChar("ln_car", 0) & 3;
    if (!(owned & (1 << model))) model = 0;
    sound = prefs.getBool("ln_snd", false);
  }
  void saveGarage() {
    prefs.putUChar("ln_own", owned);
    prefs.putUShort("ln_coin", coinsBank);
    prefs.putUChar("ln_car", model);
    prefs.putBool("ln_snd", sound);
  }

  void labels() {
    const Model& m = CARS[model];
    if (owned & (1 << model)) snprintf(carLbl, sizeof(carLbl), "AUTO: %s", m.name);
    else snprintf(carLbl, sizeof(carLbl), "%s KAUFEN: %u M", m.name, m.price);
    snprintf(sndLbl, sizeof(sndLbl), "MOTORTON: %s", sound ? "AN" : "AUS");
    snprintf(note, sizeof(note), "Muenzen: %u", coinsBank);
  }

  void resetDeco() {
    for (int i = 0; i < 12; i++) {
      const bool left = i & 1;
      const int sideW = RX - 4;
      deco[i].x = left ? irand(2, max(3, sideW - 22)) : RX + 3 * LW + 6 + irand(0, max(1, sideW - 24));
      deco[i].y = irand(-40, SCR_H);
      deco[i].kind = irand(0, 2);
    }
  }

  void drive() {
    RX = (SCR_W - 3 * LW) / 2;
    PY = SCR_H - CAR_H - 10;
    lane = targetLane = 1;
    px = laneX(1);
    speed = 4.0f; fuel = 100; dist = 0; tv = 1.5f;
    coinsRun = 0; t = 0; endT = 0;
    memset(cars, 0, sizeof(cars));
    memset(items, 0, sizeof(items));
    spawnAcc = 0; gap = 140;
    lastFree = 1;
    resetDeco();
    ph = PH_DRIVE;
    Kit::dirty = true;
  }

  void start() {
    Kit::begin();
    tPauseW = 44; tPauseH = 36;
    loadGarage();
    if (gameRestart) { drive(); return; }
    pick.set("SPURWECHSEL");
    labels();
    pick.add("LOSFAHREN");
    pick.add(carLbl);
    pick.add(sndLbl);
    pick.note = note;
    ph = PH_PICK;
  }

  void addCar(int l) {
    for (auto& c : cars) if (!c.on) { c = {-(float)CAR_H - 4, (uint8_t)l, (uint8_t)irand(0, 2), TCOL[irand(0, 5)], true}; return; }
  }

  // Eine Reihe Verkehr: 1 oder 2 Autos, mindestens eine Spur bleibt frei. Alle Autos fahren gleich schnell,
  // die Abstaende zwischen den Reihen bleiben also erhalten -> immer schaffbar.
  void spawnRow() {
    const int freeL = irand(0, 2);
    const int a = (freeL + 1) % 3, b = (freeL + 2) % 3;
    const bool two = irand(0, 99) < 25 + (int)min(40.0f, speed * 3);
    if (two) { addCar(a); addCar(b); }
    else addCar(irand(0, 1) ? a : b);
    lastFree = freeL;
    // Muenze oder Kanister in der freien Spur
    const int r = irand(0, 99);
    const int kind = (fuel < 55 && r < 45) ? 1 : (r < 50 ? 0 : -1);
    if (kind >= 0) for (auto& it : items) if (!it.on) { it = {-(float)CAR_H + 6, (uint8_t)freeL, (uint8_t)kind, true}; break; }
  }

  void endRun(const char* why) {
    coinsBank += coinsRun;
    saveGarage();
    Kit::silence();
    gameOverText(why, (uint32_t)(dist / 8));
  }

  int nightLvl() {
    const uint32_t c = t % 2400;
    if (c < 1500) return 0;
    if (c < 1650 || c >= 2250) return 1;
    return 2;
  }

  void update() {
    if (ph == PH_PICK) {
      const int c = pick.update();
      if (c == 0) {
        if (owned & (1 << model)) { saveGarage(); drive(); }
        else if (coinsBank >= CARS[model].price) { coinsBank -= CARS[model].price; owned |= 1 << model; saveGarage(); drive(); }
        else { snprintf(note, sizeof(note), "Zu wenig Muenzen (%u von %u)", coinsBank, CARS[model].price); Kit::dirty = true; }
      } else if (c == 1) { model = (model + 1) % 4; labels(); if (owned & (1 << model)) saveGarage(); Kit::dirty = true; }
      else if (c == 2) { sound = !sound; labels(); saveGarage(); Kit::dirty = true; }
      return;
    }
    const Kit::Ev e = Kit::take();
    Kit::dirty = true;
    if (ph == PH_CRASH) { if (++endT > 40) endRun("UNFALL"); return; }
    if (ph == PH_EMPTY) {
      speed *= 0.95f;
      dist += speed;
      if (++endT > 50) endRun("TANK LEER");
      return;
    }
    t++;
    // Eingabe
    const int oldTarget = targetLane;
    if (E.left || (e.t == Kit::EV_TAP && e.x < SCR_W / 2) || (e.t == Kit::EV_SWIPE && e.dir == 3)) targetLane = max(0, targetLane - 1);
    if (E.right || (e.t == Kit::EV_TAP && e.x >= SCR_W / 2) || (e.t == Kit::EV_SWIPE && e.dir == 1)) targetLane = min(2, targetLane + 1);
    if (targetLane != oldTarget) Snd::fx(Snd::SLIDE, Snd::P_AUTO);
    const float tx = laneX(targetLane), lv = CARS[model].laneV;
    if (fabsf(tx - px) <= lv) { px = tx; lane = targetLane; } else px += tx > px ? lv : -lv;
    // Tempo, Tank
    speed = min(12.0f, 4.0f + t * 0.0035f);
    tv = 1.5f + speed * 0.12f;
    dist += speed;
    fuel -= 0.045f + speed * 0.006f;
    if (fuel <= 0) { fuel = 0; ph = PH_EMPTY; endT = 0; return; }
    if (fuel < 25) Led::set({255, 90, 0}, 300);           // LED: Tank fast leer -> orange blinken
    else Led::set({0, 0, 0});
    if (sound) Kit::tone(70 + (uint32_t)(speed * 22) + (lane != targetLane ? 25 : 0));
    // Verkehr
    gap = 60 + speed * 14;
    spawnAcc += speed - tv;
    if (spawnAcc >= gap) { spawnAcc = 0; spawnRow(); }
    const float rel = speed - tv;
    for (auto& c : cars) {
      if (!c.on) continue;
      c.y += rel;
      if (c.y > SCR_H + 4) { c.on = false; continue; }
      const float cx = RX + c.lane * LW + (LW - CAR_W) / 2;
      if (hitBox(px + 4, PY + 3, CAR_W - 8, CAR_H - 6, cx + 4, c.y + 3, CAR_W - 8, CAR_H - 6)) {
        ph = PH_CRASH; endT = 0; Kit::silence(); Snd::big(Snd::CRASH); rumble(500, 0xFF); return;
      }
    }
    for (auto& it : items) {
      if (!it.on) continue;
      it.y += speed;
      if (it.y > SCR_H) { it.on = false; continue; }
      const float ix = RX + it.lane * LW + LW / 2 - 8;
      if (hitBox(px + 2, PY, CAR_W - 4, CAR_H, ix, it.y, 16, 16)) {
        it.on = false;
        if (it.kind == 0) { coinsRun++; Snd::fx(Snd::COIN); rumble(40, 0x40); Led::flash({255, 200, 0}, 200); }
        else { fuel = min(100.0f, fuel + 32); Snd::fx(Snd::POWER); rumble(80, 0x60); Led::flash({0, 255, 0}, 350); }
      }
    }
    for (auto& d : deco) {
      d.y += speed;
      if (d.y > SCR_H + 4) { d.y = irand(-60, -20); d.kind = irand(0, 2); }
    }
  }

  // ---------------- Zeichnen ----------------
  uint16_t shade(uint16_t day, int n) {          // Tag -> Daemmerung -> Nacht
    if (!n) return day;
    const int r = (day >> 11) & 31, g = (day >> 5) & 63, b = day & 31;
    const int f = n == 1 ? 60 : 28;              // Prozent
    return (uint16_t)(((r * f / 100) << 11) | ((g * f / 100) << 5) | (b * f / 100 + (n == 2 ? 2 : 0)));
  }

  void drawCar(const char* const* bmp, int x, int y, uint16_t col) { Kit::pix(bmp, 12, 20, x, y, 2, col, shade(col, 2)); }

  void render() {
    if (ph == PH_PICK) {
      pick.render();
      drawCar(CARS[model].bmp, SCR_W - 44, SCR_H - 60, CARS[model].col);
      return;
    }
    const int n = nightLvl();
    // Landschaft
    cR(0, 0, SCR_W, SCR_H, shade(RGB(40, 140, 50), n));
    for (auto& d : deco) {
      const int y = (int)d.y;
      if (d.kind == 0) { cR(d.x + 8, y + 14, 5, 8, shade(RGB(100, 60, 20), n)); cC(d.x + 10, y + 9, 9, shade(RGB(20, 100, 35), n)); cC(d.x + 7, y + 6, 4, shade(RGB(50, 160, 60), n)); }
      else if (d.kind == 1) { cE(d.x + 10, y + 12, 10, 6, shade(RGB(30, 115, 40), n)); }
      else { cE(d.x + 8, y + 12, 7, 5, shade(RGB(130, 130, 120), n)); cE(d.x + 6, y + 10, 3, 2, shade(RGB(170, 170, 160), n)); }
    }
    // Fahrbahn
    cR(RX - 6, 0, 3 * LW + 12, SCR_H, shade(RGB(150, 150, 150), n));
    cR(RX - 3, 0, 3 * LW + 6, SCR_H, shade(RGB(70, 70, 82), n));
    if (n == 2) {                                   // Scheinwerferkegel
      const int cx = (int)px + CAR_W / 2, ty = PY - 150;
      const uint16_t L = RGB(95, 95, 80);
      cTri(cx - 10, PY + 4, cx + 10, PY + 4, cx - 48, ty, L);
      cTri(cx + 10, PY + 4, cx - 48, ty, cx + 48, ty, L);
    }
    const uint16_t lineC = shade(RGB(235, 235, 235), n);
    cR(RX - 1, 0, 2, SCR_H, lineC);
    cR(RX + 3 * LW - 1, 0, 2, SCR_H, lineC);
    const int off = (int)fmodf(dist, 40.0f);
    for (int l = 1; l < 3; l++)
      for (int y = -40 + off; y < SCR_H; y += 40) cR(RX + l * LW - 1, y, 3, 20, lineC);
    // Gegenstaende
    for (auto& it : items) {
      if (!it.on) continue;
      const int x = RX + it.lane * LW + LW / 2, y = (int)it.y + 8;
      if (it.kind == 0) { cC(x, y, 7, RGB(255, 200, 0)); cC(x, y, 4, RGB(255, 235, 110)); }
      else { cRR(x - 7, y - 8, 14, 16, 2, RGB(220, 30, 30)); cR(x - 3, y - 11, 6, 4, RGB(220, 30, 30)); cL(x - 4, y - 4, x + 4, y + 4, C_WHITE); cL(x + 4, y - 4, x - 4, y + 4, C_WHITE); }
    }
    // Verkehr
    for (auto& c : cars) {
      if (!c.on) continue;
      const int x = RX + c.lane * LW + (LW - CAR_W) / 2, y = (int)c.y;
      const bool lit = n < 2 || (y + CAR_H > PY - 150 && abs(x - (int)px) < 50 + (PY - y) / 3);
      if (lit) drawCar(TRAFFIC[c.kind], x, y, c.col);
      else { cRR(x + 2, y, CAR_W - 4, CAR_H, 5, RGB(25, 25, 35)); cR(x + 3, y + CAR_H - 5, 5, 3, C_RED); cR(x + CAR_W - 8, y + CAR_H - 5, 5, 3, C_RED); }
    }
    // eigenes Auto
    int shake = (ph == PH_CRASH && endT < 12) ? irand(-3, 3) : 0;
    if (!(ph == PH_CRASH && (endT / 4) % 2)) drawCar(CARS[model].bmp, (int)px + shake, PY, CARS[model].col);
    if (n == 2) { cR((int)px + 3, PY - 2, 5, 3, C_YELLOW); cR((int)px + CAR_W - 8, PY - 2, 5, 3, C_YELLOW); }
    // Anzeigen links und rechts
    char buf[16];
    const int sw = RX - 8, lx = 4, rx = RX + 3 * LW + 8;
    const float ts = sw >= 70 ? 2 : 1;
    cRR(lx - 2, 4, sw, 100, 6, RGB(15, 15, 30));
    cTxt("METER", lx + 4, 10, 1, C_GRAY, textdatum_t::top_left);
    snprintf(buf, sizeof(buf), "%lu", (unsigned long)(dist / 8));
    cTxt(buf, lx + 4, 22, ts, C_WHITE, textdatum_t::top_left);
    cTxt("KM/H", lx + 4, 44, 1, C_GRAY, textdatum_t::top_left);
    snprintf(buf, sizeof(buf), "%d", (int)(speed * 16));
    cTxt(buf, lx + 4, 56, ts, C_CYAN, textdatum_t::top_left);
    cTxt("MUENZEN", lx + 4, 78, 1, C_GRAY, textdatum_t::top_left);
    snprintf(buf, sizeof(buf), "%u", coinsRun);
    cTxt(buf, lx + 4, 90, 1, C_YELLOW, textdatum_t::top_left);
    // Tank
    const int gx = rx + max(0, (sw - 22) / 2), gy = 44, gh = SCR_H - gy - 26;
    cRR(gx - 4, gy - 4, 30, gh + 22, 6, RGB(15, 15, 30));
    cRO(gx, gy, 22, gh, C_GRAY);
    const int fh = (int)((gh - 4) * fuel / 100);
    cR(gx + 2, gy + 2 + (gh - 4 - fh), 18, fh, fuel < 25 ? C_RED : (fuel < 50 ? C_ORANGE : C_GREEN));
    cTxt("TANK", gx + 11, gy + gh + 9, 1, fuel < 25 && (t / 8) % 2 ? C_RED : C_GRAY);
    Kit::pauseIcon();
    if (ph == PH_EMPTY) Kit::panel("TANK LEER", nullptr, C_ORANGE, nullptr);
  }

  void loop() {
    if (Kit::pause()) return;
    Kit::step(update, render);
  }
}
