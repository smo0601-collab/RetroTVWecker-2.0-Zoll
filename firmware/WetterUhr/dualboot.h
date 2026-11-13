#pragma once
/*
  dualboot.h - nur CYD: zweites Programm RetroCYD (Spiele) neben der Uhr

  - Beim Einschalten (Strom an / Reset-Taste) und wenn RetroCYD installiert ist: Auswahl "Uhr" oder "Spiele"
    per Touchscreen. Ohne Auswahl startet nach 6 s die Uhr.
  - Langes Halten auf dem Bildschirm: Menue mit "Spiele", "Standby", "Zurueck".
  - Touch kalibrieren: 3 Kreuze antippen (beim ersten Start mit Spielen automatisch, sonst im Browser).
  Wird aus WetterUhr.ino nach den Display-Funktionen eingebunden.
*/
#include "cyd_shared.h"

namespace dual {

static cyd::App other = cyd::APP_NONE;          // was im zweiten Programmbereich steckt
static bool games() { return other == cyd::APP_GAMES; }
static void detect() { other = cyd::otherApp(); LOGF("Zweiter Programmbereich: %d\n", (int)other); }

enum Ui : uint8_t { UI_NONE = 0, UI_MENU };
static Ui ui = UI_NONE;
static bool calReq = false;                      // Kalibrierung vom Browser angefordert
static bool gamesReq = false;                    // Spiele vom Browser angefordert
static bool relayReq = false;                    // Uhr-Update ueber RetroCYD angefordert

static const uint16_t BTN_BG = 0x2124, BTN_HI = 0x041F, BTN_OFF = 0x18E3, BTN_FR = 0x8410;

struct Btn { int16_t x, y, w, h; const char* label; bool on; };

static void drawBtn(const Btn& b, bool hl) {
  tft.fillRoundRect(b.x, b.y, b.w, b.h, 10, !b.on ? BTN_OFF : (hl ? BTN_HI : BTN_BG));
  tft.drawRoundRect(b.x, b.y, b.w, b.h, 10, hl ? C_WHITE : BTN_FR);
  tft.setFont(&FreeSansBold12pt7b);
  tft.setTextSize(1);
  int16_t bx, by; uint16_t bw, bh;
  tft.getTextBounds(b.label, 0, 0, &bx, &by, &bw, &bh);
  tft.setTextColor(b.on ? C_WHITE : C_GREY);
  tft.setCursor(b.x + (b.w - (int)bw) / 2 - bx, b.y + (b.h - (int)bh) / 2 - by);
  tft.print(b.label);
  tft.setFont(nullptr);
}

static void title(const char* t, int y) {
  drawText(&FreeSans12pt7b, 1, t, tft.width() / 2, y, C_WHITE, AL_CENTER, 0, y - 22, tft.width(), 28);
}
static void hint(const char* t, int y) {
  drawText(&FreeSans9pt7b, 1, t, tft.width() / 2, y, C_GREY, AL_CENTER, 0, y - 16, tft.width(), 22);
}

static int hit(const Btn* b, int n, int x, int y) {
  for (int i = 0; i < n; i++)
    if (b[i].on && x >= b[i].x && x < b[i].x + b[i].w && y >= b[i].y && y < b[i].y + b[i].h) return i;
  return -1;
}

// Spiele starten: Start-Partition auf RetroCYD setzen und neu starten. RetroCYD stellt beim Start selbst
// wieder auf die Uhr zurueck, damit nach Stromausfall oder "Zur Uhr" die Uhr kommt.
static void startGames() {
  if (!games() || !cyd::bootOther()) return;
  tft.fillScreen(C_BG);
  title(TR(K_GAMES_START), 128);
  delay(300);
  ESP.restart();
}

// ---------------------------------------------------------------------
// Kalibrierung (blockierend, ca. 3 x tippen). Rueckgabe: true = gespeichert
// ---------------------------------------------------------------------
static bool waitTap(int& a, int& b, uint32_t timeoutMs) {
  const uint32_t t0 = millis();
  while (cyd::touchRaw(a, b) >= XPT_Z_MIN) { if (millis() - t0 > timeoutMs) return false; delay(10); }   // erst loslassen
  uint32_t since = 0;
  while (millis() - t0 < timeoutMs) {
    int ra, rb;
    if (cyd::touchRaw(ra, rb) >= XPT_Z_MIN) {
      if (!since) since = millis();
      if (millis() - since > 120) {                  // ruhig gedrueckt: Mittelwert ueber 150 ms
        long sa = 0, sb = 0; int n = 0;
        const uint32_t t1 = millis();
        while (millis() - t1 < 150) { if (cyd::touchRaw(ra, rb) >= XPT_Z_MIN) { sa += ra; sb += rb; n++; } delay(5); }
        if (n >= 3) {
          a = sa / n; b = sb / n;
          if (gSet.bzTouch) bz::click();
          while (cyd::touchRaw(ra, rb) >= XPT_Z_MIN && millis() - t0 < timeoutMs) delay(10);
          return true;
        }
        since = 0;
      }
    } else since = 0;
    bz::tick(millis());
    delay(10);
  }
  return false;
}

static bool calibrate() {
  static const int16_t PX[3] = {20, 300, 20}, PY[3] = {20, 20, 220};
  int A[3], B[3];
  for (int i = 0; i < 3; i++) {
    tft.fillScreen(C_BG);
    title(TR(K_CAL_TITLE), 110);
    hint(TR(K_CAL_TAP), 140);
    const int x = PX[i], y = PY[i];
    tft.drawFastHLine(x - 12, y, 25, C_WHITE);
    tft.drawFastVLine(x, y - 12, 25, C_WHITE);
    tft.drawCircle(x, y, 6, 0xF800);
    if (!waitTap(A[i], B[i], 20000)) { tft.fillScreen(C_BG); needFull = true; return false; }
  }
  const bool ok = cyd::calFrom(A, B, gSet.rotation);
  tft.fillScreen(C_BG);
  title(TR(ok ? K_CAL_OK : K_CAL_FAIL), 128);
  if (ok) cyd::calSave();
  delay(900);
  tft.fillScreen(C_BG);
  needFull = true;
  return ok;
}

// ---------------------------------------------------------------------
// Auswahl beim Einschalten: Uhr oder Spiele (blockierend, hoechstens 6 s)
// ---------------------------------------------------------------------
static void bootChooser() {
  const esp_reset_reason_t r = esp_reset_reason();
  if (!games() || (r != ESP_RST_POWERON && r != ESP_RST_EXT)) return;   // nach Neustart aus Software direkt zur Uhr
  if (!cyd::cal.ok) calibrate();                    // erstes Mal: Touch einrichten, damit die Knoepfe stimmen
  Btn b[2] = {{12, 62, 142, 120, TR(K_CLOCK), true}, {166, 62, 142, 120, TR(K_GAMES), true}};
  tft.fillScreen(C_BG);
  title(TR(K_CHOOSE), 40);
  drawBtn(b[0], false); drawBtn(b[1], false);
  const uint32_t t0 = millis();
  int lastSec = -1, sel = -1, down = -1;
  while (true) {
    const uint32_t el = millis() - t0;
    const int sec = 6 - (int)(el / 1000);
    if (down < 0 && sec <= 0) break;
    if (sec != lastSec && sec > 0) {
      lastSec = sec;
      char t[48]; snprintf(t, sizeof(t), TR(K_CLOCK_IN), sec);
      hint(t, 218);
    }
    int x, y;
    if (xpt::point(x, y, gSet.rotation)) {
      const int h = hit(b, 2, x, y);
      if (h != down) { if (down >= 0) drawBtn(b[down], false); if (h >= 0) drawBtn(b[h], true); down = h; }
      if (h >= 0) sel = h;
    } else if (down >= 0 || sel >= 0) {             // losgelassen: Auswahl gilt
      if (sel >= 0) break;
    }
    delay(15);
  }
  if (sel == 1) startGames();
  tft.fillScreen(C_BG);
}

// ---------------------------------------------------------------------
// Menue nach langem Halten (nicht blockierend, laeuft in loop())
// ---------------------------------------------------------------------
static Btn mb[3];
static int mDown = -1;
static uint32_t mSince = 0;
static bool mWaitRelease = true;

static void menuOpen() {
  ui = UI_MENU;
  mDown = -1; mSince = millis(); mWaitRelease = true;
  mb[0] = {20, 46, 280, 54, TR(K_GAMES), games()};
  mb[1] = {20, 108, 280, 54, TR(K_STANDBY), true};
  mb[2] = {20, 170, 280, 54, TR(K_BACK), true};
  tft.fillScreen(C_BG);
  title(TR(K_MENU), 30);
  for (auto& b : mb) drawBtn(b, false);
  if (!games()) hint(TR(K_NO_GAMES), 238);
}
static void menuClose() { ui = UI_NONE; tft.fillScreen(C_BG); needFull = true; }

static void menuTick(uint32_t now) {
  int x, y;
  const bool t = xpt::point(x, y, gSet.rotation);
  if (mWaitRelease) { if (!t) mWaitRelease = false; return; }   // Finger vom langen Halten erst loslassen
  if (t) {
    mSince = now;
    const int h = hit(mb, 3, x, y);
    if (h != mDown) { if (mDown >= 0) drawBtn(mb[mDown], false); if (h >= 0) drawBtn(mb[h], true); mDown = h; }
    return;
  }
  if (mDown >= 0) {                                  // losgelassen ueber einem Knopf
    const int sel = mDown; mDown = -1;
    if (gSet.bzTouch && !bz::busy()) bz::click();
    if (sel == 0) { startGames(); menuClose(); }
    else if (sel == 1) { ui = UI_NONE; enterStandby(); }
    else menuClose();
    return;
  }
  if (now - mSince > 10000) menuClose();             // 10 s nichts getan
}

// in loop(): true = Dual-Oberflaeche aktiv (Uhr-Anzeige und normale Touch-Gesten pausieren)
static bool tick(uint32_t now) {
  if (gamesReq) { gamesReq = false; startGames(); }
  if (relayReq) {                                    // Uhr-Update: RetroCYD im Update-Modus starten
    relayReq = false;
    delay(300);                                      // Antwort an den Browser noch rausschicken
    if (games() && cyd::bootOther()) {
      tft.fillScreen(C_BG);
      title(TR(K_FW_UPDATE), 110);
      hint("RetroCYD ...", 140);
      delay(200);
      ESP.restart();
    }
  }
  if (calReq) { calReq = false; ui = UI_NONE; calibrate(); return true; }
  if (ui == UI_MENU) { menuTick(now); return ui != UI_NONE; }
  return false;
}

}  // namespace dual
