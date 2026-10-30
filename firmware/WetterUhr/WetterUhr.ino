/*
  Wetteruhr - ESP32-C3 SuperMini + GMT020-02-7P (ST7789, 240x320)

  EIN Bildschirm: Uhr (oben) und Wetter (unten).

  Bedienung nur per Browser (kein Touchsensor): Helligkeit, Display ein/aus.

  Einstellungen per Browser: http://<IP-Adresse> oder http://wetteruhr.local
  (WLAN, Zeitzone, Standort, Helligkeit, ...). Ohne WLAN-Daten startet der
  Access-Point "Wetteruhr-Setup" (Seite: http://192.168.4.1).

  Dimmung:
    - mit Backlight-Umbau (PIN_TFT_BL >= 0): echte PWM-Dimmung, Standby = Backlight aus
    - ohne Umbau: Software-Dimmung (Farben abdunkeln), Backlight bleibt an

  Bibliotheken: Adafruit GFX, Adafruit ST7735 and ST7789, ArduinoJson (v7)
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <ArduinoJson.h>
#include <esp_idf_version.h>
#include <esp_sntp.h>
#include <Update.h>
#include <esp_ota_ops.h>
#include <ArduinoOTA.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>

#include "config.h"
#include "icons.h"
#include "settings.h"
#include "types.h"
#include "faces.h"        // Themen-Zifferblaetter (Design 2..31)
#include "buzzer.h"       // aktiver Piezo-Summer, Wecker

#if DEBUG_SERIAL
#define LOGF(...) Serial.printf(__VA_ARGS__)
#else
#define LOGF(...) do {} while (0)
#endif

// =====================================================================
// Globale Objekte und Zustand
// =====================================================================
Adafruit_ST7789 tft(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);

static bool     standby       = false;
static bool     wakePending   = false;   // Sleep-Out laeuft (120 ms Wartezeit)
static uint32_t wakeAt        = 0;
static bool     showAfterDraw = false;   // Display erst nach dem Zeichnen einschalten
static bool     needFull      = true;
static uint8_t  gStyle        = 0;       // gerade angezeigtes Design (bei Auto-Wechsel != gSet.clockStyle)
static uint32_t gStyleSince   = 0;
static uint32_t bootMs        = 0;

static bool     apActive      = false;   // Einrichtungs-Access-Point laeuft
static bool     setupMode     = false;   // keine WLAN-Daten gespeichert
static uint32_t lastConnectedMs = 0;
static volatile bool gRefreshNow = false;   // Wetter sofort neu abrufen
static bool     gOtaActive    = false;   // Firmware-Update laeuft: Anzeige pausiert

#if PIN_TFT_BL >= 0
static const bool HW_DIM = true;
#else
static const bool HW_DIM = false;
#endif

// =====================================================================
// Wetterdaten (vom Hintergrund-Task geschrieben, mit Mutex geschuetzt)
// =====================================================================
static Weather           gWeather;
static SemaphoreHandle_t gMutex = nullptr;
static SemaphoreHandle_t gNetMutex = nullptr;     // immer nur ein HTTPS-Abruf gleichzeitig (spart RAM)

static volatile uint32_t gDiagSec[4] = {0,0,0,0}, gDiagSecLast[4] = {0,0,0,0};   // wifi, web, render, delay(2)
static volatile uint32_t gDiagGapMax = 0, gDiagFrameMax = 0, gDiagGapLast = 0, gDiagFrameLast = 0;

static bool getWeather(Weather& out) {
  static Weather cache;                                   // bei belegtem Mutex: letzten Stand nehmen, nie warten
  if (xSemaphoreTake(gMutex, pdMS_TO_TICKS(2)) == pdTRUE) { cache = gWeather; xSemaphoreGive(gMutex); }
  out = cache;
  return out.valid;
}

// =====================================================================
// Hilfsfunktionen
// =====================================================================
static bool timeValid() { return time(nullptr) > 1700000000L; }

static void applyTimezone() {
  setenv("TZ", gSet.tz, 1);
  tzset();
}

static bool gTimeManual = false;   // Uhrzeit wurde zuletzt im Browser gesetzt

// Lokale Zeit (in der eingestellten Zeitzone) als Systemzeit setzen. false = ungueltige Eingabe.
static bool setLocalTime(int y, int mo, int d, int h, int mi, int s) {
  if (y < 2024 || y > 2099 || mo < 1 || mo > 12 || d < 1 || d > 31 ||
      h < 0 || h > 23 || mi < 0 || mi > 59 || s < 0 || s > 59) return false;
  applyTimezone();
  struct tm t;
  memset(&t, 0, sizeof(t));
  t.tm_year = y - 1900; t.tm_mon = mo - 1; t.tm_mday = d;
  t.tm_hour = h; t.tm_min = mi; t.tm_sec = s;
  t.tm_isdst = -1;                 // Sommerzeit aus der Zeitzone bestimmen
  const time_t e = mktime(&t);
  if (e < 1700000000L) return false;
  struct timeval tv = { e, 0 };
  if (settimeofday(&tv, nullptr) != 0) return false;
  gTimeManual = true;
  return true;
}

static void ntpStop() {
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 1, 0)
  if (esp_sntp_enabled()) esp_sntp_stop();
#else
  if (sntp_enabled()) sntp_stop();
#endif
}

// "veraltet" = 3 Aktualisierungsintervalle ohne neue Daten (Standard 45 min)
static uint32_t staleMs() { return (uint32_t)gSet.updMin * 60000UL * 3UL; }

static const char* WEEKDAY[7]  = {"Sonntag", "Montag", "Dienstag", "Mittwoch",
                                  "Donnerstag", "Freitag", "Samstag"};
static const char* WEEKDAY2[7] = {"So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};

// 0 = Sonntag; Eingabe "YYYY-MM-DD"
static int dayOfWeekFromIso(const char* s) {
  int y = 0, m = 0, d = 0;
  if (sscanf(s, "%d-%d-%d", &y, &m, &d) != 3 || m < 1 || m > 12) return -1;
  static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  if (m < 3) y -= 1;
  return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

// ---- Cache gegen unnoetiges Neuzeichnen (weniger Flackern) ----
enum { K_H1, K_M1, K_SEC, K_DATE, K_STAT, K_WX, K_ATIME, K_BAR, K_COUNT };
static char cacheS[K_COUNT][48];

static void resetCaches() {
  for (int i = 0; i < K_COUNT; i++) { cacheS[i][0] = 1; cacheS[i][1] = 0; }
}
static bool upd(int k, const char* now) {
  if (strcmp(cacheS[k], now) == 0) return false;
  strlcpy(cacheS[k], now, sizeof(cacheS[k]));
  return true;
}

// ---- Text zeichnen: Bereich loeschen, ausrichten, ausgeben ----
static void drawText(const GFXfont* font, uint8_t size, const char* txt,
                     int16_t x, int16_t baseline, uint16_t col, Align al,
                     int16_t clrX, int16_t clrY, int16_t clrW, int16_t clrH) {
  tft.fillRect(clrX, clrY, clrW, clrH, C_BG);
  tft.setFont(font);
  tft.setTextSize(size);
  tft.setTextColor(col);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(txt, 0, baseline, &x1, &y1, &w, &h);
  int16_t cx;
  if (al == AL_CENTER)     cx = x - (int16_t)w / 2 - x1;
  else if (al == AL_RIGHT) cx = x - (int16_t)w - x1;
  else                     cx = x - x1;
  tft.setCursor(cx, baseline);
  tft.print(txt);
}

// ---- Temperatur mit Gradzeichen (die GFX-Schriften haben kein Gradzeichen) ----
static void drawTemp(int16_t xAnchor, int16_t baseline, int t, const GFXfont* font,
                     uint8_t size, uint16_t col, Align al,
                     int16_t clrX, int16_t clrY, int16_t clrW, int16_t clrH) {
  tft.fillRect(clrX, clrY, clrW, clrH, C_BG);
  char num[8];
  snprintf(num, sizeof(num), "%d", t);
  tft.setFont(font);
  tft.setTextSize(size);
  tft.setTextColor(col);
  int16_t x1, y1, x1c, y1c;
  uint16_t wn, hn, wc, hc;
  tft.getTextBounds(num, 0, baseline, &x1, &y1, &wn, &hn);
  tft.getTextBounds("C", 0, baseline, &x1c, &y1c, &wc, &hc);
  int16_t r = max(2, (int)hn / 9);
  int16_t total = (int16_t)wn + 2 * r + 5 + (int16_t)wc;
  int16_t xs = xAnchor;
  if (al == AL_CENTER)     xs = xAnchor - total / 2;
  else if (al == AL_RIGHT) xs = xAnchor - total;
  tft.setCursor(xs - x1, baseline);
  tft.print(num);
  int16_t cxm = xs + wn + 2 + r;
  int16_t cym = y1 + r + 1;
  tft.drawCircle(cxm, cym, r, col);
  if (r > 2) tft.drawCircle(cxm, cym, r - 1, col);
  tft.setCursor(xs + wn + 2 * r + 5 - x1c, baseline);
  tft.print("C");
}

static void drawWifiIcon(int x, int y, bool connected) {
  uint16_t col = connected ? C_GREEN : C_RED;
  for (int i = 0; i < 3; i++) {
    int h = 4 + i * 4;
    tft.fillRect(x + i * 5, y + 12 - h, 3, h, col);
  }
}

// =====================================================================
// Helligkeit: Backlight-PWM (nach Umbau) oder Software-Dimmung
// =====================================================================
// ---- Zeitplan-Dimmung ----
static bool gDimActive = false;   // true = gerade im Dimm-Zeitfenster
static bool gNightTvBlocked = false;   // Testbild liess sich nicht starten (Speicher): bis Fensterende nicht erneut versuchen

// Nachtmodus: im Dimm-Zeitfenster das Testbild statt des gewaehlten Designs zeigen
static bool nightTv() { return gDimActive && gSet.dimTv && !gNightTvBlocked; }

// Silvester-Automatik: 31.12. ab 23:30 bis 1.1. 00:30 das Silvester-Zifferblatt (Feuerwerk)
static bool gNyeBlocked = false;   // Zifferblatt liess sich nicht starten (Speicher)
static bool nyeWindow() {
  static uint32_t at = 0;
  static bool v = false;
  const uint32_t now = millis();
  if (at && now - at < 1000) return v;
  at = now ? now : 1;
  v = false;
  if (gSet.nyeAuto && timeValid()) {
    time_t n = time(nullptr);
    struct tm t;
    localtime_r(&n, &t);
    const int m = t.tm_hour * 60 + t.tm_min;
    v = (t.tm_mon == 11 && t.tm_mday == 31 && m >= 23 * 60 + 30) || (t.tm_mon == 0 && t.tm_mday == 1 && m < 30);
  }
  return v;
}
static bool nyeNow() { return nyeWindow() && !gNyeBlocked; }

// Design, das gerade Vorrang hat (Silvester vor Nachtmodus), -1 = keines
static int overrideStyle() {
  if (nyeNow()) return NEWYEAR_STYLE;
  if (nightTv()) return TESTCARD_STYLE;
  return -1;
}

// Weihnachtsschlitten statt ISS?
static bool sleighNow() {
  if (gSet.sleigh == 1) return true;
  if (gSet.sleigh != 2 || !timeValid()) return false;
  time_t n = time(nullptr);
  struct tm t;
  localtime_r(&n, &t);
  return t.tm_mon == 11 && t.tm_mday <= 26;
}

// Helligkeit, die aktuell gelten soll (Zeitfenster hat Vorrang vor dem Schieberegler)
static uint8_t effectiveBright() {
  return gDimActive ? gSet.dimLevel : gSet.bright;
}

static void blWrite(uint8_t duty) {
#if PIN_TFT_BL >= 0
  #if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcWrite(PIN_TFT_BL, duty);
  #else
    ledcWrite(BL_PWM_CHANNEL, duty);
  #endif
#else
  (void)duty;
#endif
}

static void blInit() {
#if PIN_TFT_BL >= 0
  #if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttach(PIN_TFT_BL, BL_PWM_FREQ, 8);
  #else
    ledcSetup(BL_PWM_CHANNEL, BL_PWM_FREQ, 8);
    ledcAttachPin(PIN_TFT_BL, BL_PWM_CHANNEL);
  #endif
  blWrite(BL_ACTIVE_HIGH ? 0 : 255);   // bis zum ersten Zeichnen aus
#endif
}

// Gamma-Anpassung (ca. 2), damit die Stufen gleichmaessig wirken
static uint8_t dutyFor(uint8_t pct) {
  uint32_t d = (uint32_t)pct * pct * 255UL / 10000UL;
  if (d < 2) d = 2;
  return BL_ACTIVE_HIGH ? (uint8_t)d : (uint8_t)(255 - d);
}

static void blApply() {
  if (HW_DIM && !standby) blWrite(dutyFor(effectiveBright()));
}

static void blOff() {
  if (HW_DIM) blWrite(BL_ACTIVE_HIGH ? 0 : 255);
}

static void themeApply() {
  gAccentIdx = gSet.accent;
  applyTheme(HW_DIM ? 100 : effectiveBright());
}

static char     gDbgSrc = '-';           // Diagnose: wer hat das Design zuletzt gewechselt (W = Browser, T = Touch, A = Automatik)
static int      gDbgLook = -1;           // Diagnose: zuletzt im Browser gewaehlte Nummer
static uint32_t gAutoHold = 0;           // nach manueller Wahl: Automatik-Wechsel bis dahin pausiert
static uint32_t touchBlockUntil = 0;     // Touch-Eingaben bis zu diesem Zeitpunkt ignorieren
static uint32_t gTouchCount = 0, gTouchLastMs = 0;   // Diagnose: erkannte Beruehrungen
static bool     setDirty = false;
static uint32_t setDirtyAt = 0;

static void setBrightness(uint8_t pct, bool markDirty) {
  if (pct < 5) pct = 5;
  if (pct > 100) pct = 100;
  gSet.bright = pct;
  if (HW_DIM) {
    blApply();
  } else {
    themeApply();
    needFull = true;     // Farben neu zeichnen
  }
  if (markDirty) { setDirty = true; setDirtyAt = millis(); }  // Flash erst nach 3 s Ruhe beschreiben
}

// =====================================================================
// Display: Init, Standby, Aufwecken
// =====================================================================
// Prueft einmal pro Sekunde (oder sofort mit force), ob das Dimm-Zeitfenster gilt
static bool dimWindowNow() {
  if (!gSet.dimOn || !timeValid()) return false;
  time_t n = time(nullptr);
  struct tm t;
  localtime_r(&n, &t);
  const int m = t.tm_hour * 60 + t.tm_min;
  const int a = gSet.dimStart, b = gSet.dimEnd;
  if (a == b) return false;
  if (a < b) return (m >= a && m < b);
  return (m >= a || m < b);   // Fenster ueber Mitternacht
}

static void dimTick(uint32_t now, bool force) {
  static uint32_t last = 0;
  if (!force && now - last < 1000) return;
  last = now;
  const bool want = dimWindowNow();
  if (want == gDimActive) return;
  gDimActive = want;
  if (!want) gNightTvBlocked = false;
  LOGF("Dimm-Zeitfenster: %s\n", want ? "an" : "aus");
  if (HW_DIM) {
    blApply();
  } else {
    themeApply();
    needFull = true;
  }
}

// Design-Wechsel nach Zeitplan: nur wenn aktiv und mindestens zwei Designs gewaehlt sind
static int autoCount() {
  int n = 0;
  for (int i = 0; i < CLOCK_STYLE_COUNT; i++) if (gSet.autoMask & (1ULL << i)) n++;
  return n;
}
// Naechstes Design nach "from": aus der Wechsel-Liste (wenn mindestens zwei gewaehlt), sonst aus allen
static int nextStyle(int from) {
  const bool list = autoCount() >= 2;
  for (int k = 1; k <= CLOCK_STYLE_COUNT; k++) {
    const int i = (from + k) % CLOCK_STYLE_COUNT;
    if (!list || (gSet.autoMask & (1ULL << i))) return i;
  }
  return from;
}
static bool autoActive() { return gSet.autoOn && autoCount() >= 2; }
static bool inList(int s) { return s >= 0 && s < CLOCK_STYLE_COUNT && (gSet.autoMask & (1ULL << s)); }
// Design zum Start bzw. nach Nachtmodus/Speichern: bei aktivem Wechsel immer eines aus der Liste
static uint8_t listStyle(int from) {
  if (!autoActive()) return gSet.clockStyle;
  return (uint8_t)(inList(from) ? from : nextStyle(from));
}
static void styleManual() {            // Auswahl im Browser: sofort uebernehmen, Timer neu starten
  gAutoHold = millis() + 120000UL;       // gewaehltes Design mindestens 2 Minuten halten
  touchBlockUntil = millis() + 2000;     // Neuaufbau des Bildes erzeugt Stoerimpulse: Touch kurz sperren
  gStyle = gSet.clockStyle;
  gStyleSince = millis();
  needFull = true;
}
static void styleTick(uint32_t now) {
  static bool wasNight = false;
  if (!nyeWindow()) gNyeBlocked = false;
  const int ov = overrideStyle();
  if (ov >= 0) {                         // Silvester bzw. Nachtmodus haben Vorrang vor Auswahl und Automatik
    if (gStyle != ov) { gStyle = (uint8_t)ov; needFull = true; gDbgSrc = 'N'; }
    gStyleSince = now;
    wasNight = true;
    return;
  }
  if (wasNight) {                        // Vorrang vorbei: zurueck zum gewaehlten Design bzw. zur Liste
    wasNight = false;
    const uint8_t s = listStyle(gSet.clockStyle);
    if (gStyle != s) { gStyle = s; needFull = true; }
    gStyleSince = now;
  }
  const bool act = gSet.autoOn && autoCount() >= 2;
  if (!act) {
    if (gStyle != gSet.clockStyle) { gStyle = gSet.clockStyle; needFull = true; }
    gStyleSince = now;
    return;
  }
  if (standby || wakePending) { gStyleSince = now; return; }
  if ((int32_t)(now - gAutoHold) < 0) { gStyleSince = now; return; }
  if (!inList(gStyle)) {                 // z. B. Wechsel gerade eingeschaltet: sofort in die Liste
    gStyle = (uint8_t)nextStyle(gStyle); gStyleSince = now; needFull = true; gDbgSrc = 'A';
    return;
  }
  if (now - gStyleSince < gSet.autoSec * 1000UL) return;
  gStyleSince = now;
  int next = -1;
  if (gSet.autoRnd) {
    int k = (int)(esp_random() % (uint32_t)(autoCount() - ((gSet.autoMask >> gStyle) & 1 ? 1 : 0)));
    for (int i = 0; i < CLOCK_STYLE_COUNT; i++) {
      if (!(gSet.autoMask & (1ULL << i)) || i == gStyle) continue;
      if (k-- == 0) { next = i; break; }
    }
  } else {
    for (int k = 1; k <= CLOCK_STYLE_COUNT; k++) {
      const int i = (gStyle + k) % CLOCK_STYLE_COUNT;
      if (gSet.autoMask & (1ULL << i)) { next = i; break; }
    }
  }
  if (next >= 0 && next != gStyle) { gStyle = (uint8_t)next; needFull = true; gDbgSrc = 'A'; }
}

// ---------------------------------------------------------------------
// Laufschrift (ticker.h): Steuerung. Gezeichnet wird in themedTick() bzw. tickerClassic().
// ---------------------------------------------------------------------
static bool     gTickNow  = false;    // einmal sofort abspielen (Knopf / Speichern)
static uint32_t gTickNext = 0;        // naechster Start in der Schleife

static void tickRgb(uint8_t idx, uint8_t& r, uint8_t& g, uint8_t& b) {
  if (idx < ACCENT_COUNT) { r = ACCENT_RGB[idx][0]; g = ACCENT_RGB[idx][1]; b = ACCENT_RGB[idx][2]; }
  else { r = 240; g = 240; b = 240; }
}
static void tickerRestart() {          // nach geaenderten Einstellungen: neu beginnen
  if (tk::active) { tk::stop(); needFull = true; }
  gTickNext = 0;
  gTickNow = gSet.tickOn && !gSet.tickLoop;     // "einmalig": beim Speichern einmal abspielen
}
static void tickerTick(uint32_t now) {
  if (standby || wakePending) { if (tk::active) tk::stop(); return; }
  if (tk::justEnded) {
    tk::justEnded = false;
    gTickNext = now + (uint32_t)gSet.tickPause * 1000UL;
    if (fc::cur < 0) needFull = true;           // klassisches Design: Bildschirm neu aufbauen
  }
  if (tk::active) {
    if (!gSet.tickOn && !tk::manual) { tk::stop(); if (fc::cur < 0) needFull = true; }
    return;
  }
  if (!gSet.tick[0]) { gTickNow = false; return; }
  bool go = false, byButton = false;
  if (gTickNow) { gTickNow = false; go = true; byButton = true; }
  else if (gSet.tickOn && gSet.tickLoop && (int32_t)(now - gTickNext) >= 0) go = true;
  if (!go) return;
  tk::Cfg c;
  c.pos = gSet.tickPos; c.big = gSet.tickBig; c.speed = gSet.tickSpeed;
  c.bg = gSet.tickBg || fc::cur < 0;            // klassisch: Band immer mit Hintergrund
  tickRgb(gSet.tickColor, c.r, c.g, c.b);
  if (tk::start(gSet.tick, c, now, byButton) && fc::cur >= 0) fc::setBrightness(HW_DIM ? 100 : effectiveBright());
  else if (!tk::active) gTickNext = now + 5000;
}
static void tickerClassic(uint32_t now) {
  static uint32_t last = 0;
  if (!tk::active || now - last < 30) return;
  last = now;
  const uint8_t pct = HW_DIM ? 100 : effectiveBright();
  tk::drawClassic(tft, now, tft.width(), tft.height(),
                  scaleColor(RGB565C(tk::cfg.r, tk::cfg.g, tk::cfg.b), pct),
                  scaleColor(RGB565C(tk::cfg.r / 3, tk::cfg.g / 3, tk::cfg.b / 3), pct));
}

static void displayInit() {
  // SPI-Pins explizit setzen (entspricht beim C3 den Standardpins 4/5/6/7).
  // CS steuert die Adafruit-Bibliothek per GPIO, daher hier -1.
  SPI.begin(PIN_TFT_SCK, -1, PIN_TFT_MOSI, -1);
  tft.init(240, 320);
  tft.setRotation(gSet.rotation);
#if TFT_FORCE_INVERT >= 0
  tft.invertDisplay(TFT_FORCE_INVERT == 1);
#endif
  tft.fillScreen(C_BG);
}

static void enterStandby() {
  LOGF("Standby\n");
  standby = true;
  wakePending = false;
  tft.fillScreen(C_BG);
  tft.enableDisplay(false);
  tft.enableSleep(true);
  blOff();                       // nur mit Umbau: Backlight wirklich aus
}

static void startWake() {
  LOGF("Aufwachen\n");
  standby = false;
  needFull = true;
  tft.enableSleep(false);
  wakePending = true;
  wakeAt = millis() + 130;       // ST7789: nach Sleep-Out ca. 120 ms warten (ohne delay())
}

// ---------------------------------------------------------------------
// Summer: Wecker und Stundensignal (Muster und Ausgabe in buzzer.h)
// ---------------------------------------------------------------------
static void alarmStart(uint32_t now) {
  gAlarmRinging = true;
  gAlarmSince = now;
  bz::play(BZ_PAT(bz::P_ALARM), true);
  if (standby) startWake();               // Display zum Wecken einschalten
  LOGF("Wecker klingelt\n");
}

static void soundTick(uint32_t now) {
  bz::tick(now);
  if (gAlarmRinging && now - gAlarmSince >= ALARM_MAX_MS) alarmStop();
  static uint32_t last = 0;
  static int lastMin = -1;
  if (now - last < 500) return;
  last = now;
  if (!timeValid()) return;
  time_t n = time(nullptr);
  struct tm t;
  localtime_r(&n, &t);
  const int m = t.tm_hour * 60 + t.tm_min;
  if (m == lastMin) return;               // jede Minute nur einmal pruefen
  const bool first = (lastMin < 0);
  lastMin = m;
  if (first && t.tm_sec > 5) return;      // nach Start/Neustart nicht nachtraeglich ausloesen
  const int wd = (t.tm_wday + 6) % 7;     // 0 = Montag
  if (gSet.alOn && m == gSet.alTime && (gSet.alDays & (1 << wd))) { alarmStart(now); return; }
  if (gSet.bzHour && t.tm_min == 0 && !gAlarmRinging && !(gSet.bzQuiet && gDimActive))
    bz::play(BZ_PAT(bz::P_HOUR));
}

// ---------------------------------------------------------------------
// Touch-Taster (TTP223): kurz = naechstes Design (aus der Wechsel-Liste), doppelt = Laufschrift, lang = Standby
// ---------------------------------------------------------------------
#if PIN_TOUCH >= 0
static void touchTick(uint32_t now) {
  static bool down = false, longDone = false, pending = false;
  static uint32_t downAt = 0, tapAt = 0, hiSince = 0, loSince = 0;
  const bool raw = (digitalRead(PIN_TOUCH) == (TOUCH_ACTIVE_HIGH ? HIGH : LOW));
  if ((int32_t)(now - touchBlockUntil) < 0) {          // Sperre (z. B. kurz nach Design-Wahl im Browser): Stoerimpulse ignorieren
    hiSince = 0; loSince = now; down = false; pending = false;
    return;
  }
  if (raw) {                                           // Beruehrung zaehlt erst, wenn sie TOUCH_MIN_MS durchgehend anliegt
    loSince = 0;
    if (!hiSince) hiSince = now;
    else if (!down && now - hiSince >= TOUCH_MIN_MS) {
      down = true; downAt = now; longDone = false;
      gTouchCount++; gTouchLastMs = now;
      LOGF("Touch: Beruehrung erkannt (%lu)\n", (unsigned long)gTouchCount);
      if (gAlarmRinging) { alarmStop(); longDone = true; return; }   // klingelnder Wecker: nur abstellen
      if (gSet.bzTouch && !bz::busy()) bz::play(BZ_PAT(bz::P_CLICK));
      if (standby) { startWake(); longDone = true; }   // Beruehrung weckt, ohne weitere Aktion
    }
  } else {
    hiSince = 0;
    if (!loSince) loSince = now;
    else if (down && now - loSince >= 30) {            // losgelassen (entprellt)
      down = false;
      if (!longDone) {                                 // kurzer Tipp
        if (pending && now - tapAt <= TOUCH_DOUBLE_MS) {
          pending = false;
          if (gSet.tick[0] && !standby) gTickNow = true;   // zweiter Tipp: Laufschrift
        } else { pending = true; tapAt = now; }
      }
    }
  }
  if (down && !longDone && now - downAt >= TOUCH_LONG_MS) {   // lange Beruehrung: Standby
    longDone = true; pending = false;
    if (!standby) enterStandby();
  }
  if (pending && now - tapAt > TOUCH_DOUBLE_MS) {       // einzelner Tipp: naechstes Design
    pending = false;
    if (!standby && !wakePending) {
      gDbgSrc = 'T';
      touchBlockUntil = now + TOUCH_LOCK_MS;            // Neuaufbau des Bildes erzeugt Stoerimpulse: Touch kurz sperren
      if (overrideStyle() >= 0) {                       // Nachtmodus/Silvester: Bild bleibt, Auswahl gilt fuer danach
        gSet.clockStyle = (uint8_t)nextStyle(gSet.clockStyle);
        setDirty = true; setDirtyAt = now;
      } else if (gSet.autoOn && autoCount() >= 2) {     // Wechsel aktiv: in der Liste weiterblaettern,
        gStyle = (uint8_t)nextStyle(gStyle);            // danach laeuft der Wechsel normal weiter (volles Intervall)
        gStyleSince = now;
        needFull = true;
      } else {                                          // kein Wechsel: neues Design bleibt (wird gespeichert)
        gSet.clockStyle = (uint8_t)nextStyle(gStyle);
        gStyle = gSet.clockStyle;
        gStyleSince = now;
        needFull = true;
        setDirty = true; setDirtyAt = now;
      }
    }
  }
}
#endif


static void finishWake() {
  wakePending = false;
  showAfterDraw = true;          // Display erst einschalten, wenn der Inhalt gezeichnet ist
  needFull = true;
}

// =====================================================================
// Firmware-Update ueber WLAN (Browser-Upload und Arduino IDE)
// =====================================================================
// Freie OTA-Partition vorhanden? (fehlt z. B. bei "Huge APP (No OTA)")
static const esp_partition_t* otaPartition() { return esp_ota_get_next_update_partition(nullptr); }
static size_t otaMaxSize() { const esp_partition_t* p = otaPartition(); return p ? p->size : 0; }

// Fortschritt auf dem Display (pct 0..100). msg != nullptr: Text darunter neu schreiben
static void otaScreen(int pct, const char* msg) {
  static int last = -1;
  if (standby || wakePending) return;
  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;
  if (pct == 0 && last != 0) {
    tft.fillScreen(C_BG);
    drawText(&FreeSansBold12pt7b, 1, "Firmware-Update", tft.width() / 2, 80, C_WHITE, AL_CENTER, 0, 56, tft.width(), 32);
    tft.drawRect(28, 108, 264, 24, C_GREY);
  }
  if (pct != last) {
    tft.fillRect(30, 110, 260 * pct / 100, 20, C_ACCENT);
    char b[8];
    snprintf(b, sizeof(b), "%d %%", pct);
    drawText(&FreeSans9pt7b, 1, b, tft.width() / 2, 154, C_WHITE, AL_CENTER, 0, 138, tft.width(), 22);
    last = pct;
  }
  if (msg) drawText(&FreeSans9pt7b, 1, msg, tft.width() / 2, 186, C_GREY, AL_CENTER, 0, 168, tft.width(), 26);
}

// Nach einem abgebrochenen Update: Anzeige wieder aufnehmen
static void otaResume() {
  gOtaActive = false;
  resetCaches();
  needFull = true;
  tft.fillScreen(C_BG);
}

#if ARDUINO_OTA
static bool gArduinoOtaOn = false;
static void arduinoOtaBegin() {
  ArduinoOTA.setHostname(gSet.host);
  if (gSet.webPw[0]) ArduinoOTA.setPassword(gSet.webPw);
  ArduinoOTA.setMdnsEnabled(false);                 // mDNS macht webLoop() (MDNS.enableArduino)
  ArduinoOTA.setRebootOnSuccess(true);
  ArduinoOTA.onStart([]() { gOtaActive = true; otaScreen(0, "Arduino IDE, bitte warten ..."); });
  ArduinoOTA.onProgress([](unsigned int done, unsigned int total) { if (total) otaScreen((int)((uint64_t)done * 100 / total), nullptr); });
  ArduinoOTA.onEnd([]() { otaScreen(100, "Fertig - Neustart"); });
  ArduinoOTA.onError([](ota_error_t e) { LOGF("OTA-Fehler %u\n", (unsigned)e); otaResume(); });
  ArduinoOTA.begin();
  gArduinoOtaOn = true;
  LOGF("Arduino-OTA bereit (Port 3232)\n");
}
#endif

// =====================================================================
// WLAN, Access-Point und NTP
// =====================================================================
static uint32_t lastWifiTry = 0;
static bool     ntpStarted  = false;

static void startAp() {
  WiFi.mode(WIFI_AP_STA);                 // AP + Station (Station noetig fuer WLAN-Suche)
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  apActive = true;
  LOGF("Access-Point %s: %s\n", AP_SSID, WiFi.softAPIP().toString().c_str());
}

static void stopAp() {
  WiFi.softAPdisconnect(true);
  apActive = false;
  LOGF("Access-Point aus\n");
}

static void wifiBegin() {
  if (gSet.ssid[0] == 0) {                // keine WLAN-Daten -> Einrichtung
    setupMode = true;
    startAp();
    return;
  }
  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  WiFi.setHostname(gSet.host);
  WiFi.setAutoReconnect(true);
  WiFi.setSleep(false);          // kein Modem-Sleep: vermeidet periodische Aussetzer/Latenzspitzen
#if WIFI_REDUCE_TX_POWER
  WiFi.setTxPower(WIFI_POWER_8_5dBm);
#endif
  WiFi.begin(gSet.ssid, gSet.pass);
  lastWifiTry = millis();
}

static void wifiLoop(uint32_t now) {
  if (setupMode) return;                  // im Einrichtungsmodus nichts erzwingen

  if (WiFi.status() == WL_CONNECTED) {
    lastConnectedMs = now;
    if (!ntpStarted) {
      if (gSet.ntpOn) {
        configTzTime(gSet.tz, gSet.ntp, "time.nist.gov");   // SNTP laeuft danach selbststaendig weiter
        gTimeManual = false;
      }
      ntpStarted = true;
#if ARDUINO_OTA
      if (!gArduinoOtaOn) arduinoOtaBegin();
#endif
      LOGF("WLAN ok: %s\n", WiFi.localIP().toString().c_str());
    }
    if (apActive) stopAp();               // Fallback-AP beenden, sobald das WLAN wieder da ist
    return;
  }

  if (!apActive && now - lastConnectedMs > AP_FALLBACK_MS) startAp();   // 60 s ohne Verbindung

  if (now - lastWifiTry >= WIFI_RETRY_MS) {
    lastWifiTry = now;
    LOGF("WLAN-Neuverbindung\n");
    WiFi.disconnect();
    WiFi.begin(gSet.ssid, gSet.pass);
  }
}

// =====================================================================
// Wetterabruf (eigener FreeRTOS-Task -> Hauptschleife blockiert nie)
// =====================================================================
static bool fetchWeather(Weather& out) {
  char url[460];
  snprintf(url, sizeof(url),
    "https://api.open-meteo.com/v1/forecast?latitude=%.4f&longitude=%.4f"
    "&current=temperature_2m,relative_humidity_2m,apparent_temperature,is_day,weather_code,wind_speed_10m"
    "&daily=weather_code,temperature_2m_max,temperature_2m_min"
    "&timezone=auto&wind_speed_unit=kmh&forecast_days=3",
    (double)gSet.lat, (double)gSet.lon);

  WiFiClientSecure client;
  client.setInsecure();   // Zertifikat wird NICHT geprueft (siehe README)
  HTTPClient http;
  http.setConnectTimeout(WEATHER_HTTP_TIMEOUT_MS);
  http.setTimeout(WEATHER_HTTP_TIMEOUT_MS);
  if (!http.begin(client, url)) { LOGF("Wetter: begin() fehlgeschlagen\n"); return false; }

  int code = http.GET();
  if (code != HTTP_CODE_OK) {
    LOGF("Wetter: HTTP %d\n", code);
    http.end();
    return false;
  }
  String body = http.getString();
  http.end();

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, body);
  if (err) { LOGF("Wetter: JSON-Fehler %s\n", err.c_str()); return false; }

  JsonObject cur = doc["current"];
  if (cur.isNull() || cur["temperature_2m"].isNull()) { LOGF("Wetter: keine current-Daten\n"); return false; }

  out.temp  = cur["temperature_2m"].as<float>();
  out.feels = cur["apparent_temperature"] | out.temp;
  out.hum   = cur["relative_humidity_2m"] | 0;
  out.wind  = cur["wind_speed_10m"] | 0.0f;
  out.code  = cur["weather_code"] | 0;
  out.isDay = (cur["is_day"] | 1) != 0;

  out.nDays = 0;
  JsonObject d = doc["daily"];
  if (!d.isNull()) {
    JsonArray dc = d["weather_code"], dmx = d["temperature_2m_max"],
              dmn = d["temperature_2m_min"], dt = d["time"];
    int n = min((int)dc.size(), min((int)dmx.size(), min((int)dmn.size(), (int)dt.size())));
    if (n > 3) n = 3;
    for (int i = 0; i < n; i++) {
      out.dCode[i] = dc[i] | 0;
      out.dMax[i]  = dmx[i] | 0.0f;
      out.dMin[i]  = dmn[i] | 0.0f;
      const char* s = dt[i] | "";
      strlcpy(out.dDate[i], s, sizeof(out.dDate[i]));
    }
    out.nDays = n;
  }
  return true;
}

static void weatherTask(void*) {
  uint32_t lastAttempt = 0;
  bool haveAttempt = false, lastOk = false;
  for (;;) {
    uint32_t now = millis();
    if (WiFi.status() == WL_CONNECTED) {
      uint32_t interval = lastOk ? (uint32_t)gSet.updMin * 60000UL : WEATHER_RETRY_MS;
      if (gRefreshNow || !haveAttempt || now - lastAttempt >= interval) {
        gRefreshNow = false;
        haveAttempt = true;
        lastAttempt = now;
        Weather nw;
        xSemaphoreTake(gNetMutex, portMAX_DELAY);
        const bool wxOk = fetchWeather(nw);
        xSemaphoreGive(gNetMutex);
        if (wxOk) {
          nw.valid = true;
          nw.fetchedMs = millis();
          xSemaphoreTake(gMutex, portMAX_DELAY);
          nw.version = gWeather.version + 1;
          gWeather = nw;
          xSemaphoreGive(gMutex);
          lastOk = true;
          LOGF("Wetter aktualisiert: %.1f C\n", nw.temp);
        } else {
          lastOk = false;   // alte Daten bleiben erhalten und werden als veraltet markiert
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

// =====================================================================
// Flugradar-Daten (nur aktiv, solange das Design "Flugradar" angezeigt wird)
// Quellen (offene ADS-B-Dienste, werden der Reihe nach probiert):
//   adsb.lol, airplanes.live, adsb.fi - nur fuer private Nutzung
// =====================================================================
static PlaneList         gPlanes;                 // vom radarTask geschrieben (mit gMutex geschuetzt)
static uint16_t          gPlanesVer = 0;
static volatile uint32_t gRadarWantedAt = 0;      // zuletzt angefordert (millis, 0 = nie)
static uint8_t           gRadarHost = 0;

#define RADAR_POLL_MS 15000UL
static const char* const RADAR_URL[3] = {
  "https://api.adsb.lol/v2/point/%.4f/%.4f/%d",
  "https://api.airplanes.live/v2/point/%.4f/%.4f/%d",
  "https://opendata.adsb.fi/api/v2/lat/%.4f/lon/%.4f/dist/%d"
};

static void planeInsert(PlaneList& L, const Plane& p) {
  const float d2 = p.x * p.x + p.y * p.y;
  int pos = L.n;
  while (pos > 0 && L.p[pos - 1].x * L.p[pos - 1].x + L.p[pos - 1].y * L.p[pos - 1].y > d2) pos--;
  if (pos >= MAX_PLANES) return;
  const int last = L.n < MAX_PLANES ? L.n : MAX_PLANES - 1;
  for (int i = last; i > pos; i--) L.p[i] = L.p[i - 1];
  L.p[pos] = p;
  if (L.n < MAX_PLANES) L.n++;
}

static WiFiClientSecure* gRadarClient = nullptr;   // dauerhaft (Keep-Alive), nur waehrend das Radar laeuft

// Liest einen HTTP-"chunked"-Rumpf als gewoehnlichen Stream.
class ChunkStream : public Stream {
 public:
  explicit ChunkStream(WiFiClient& c) : c_(c) {}
  int available() override { return done_ ? 0 : 1; }
  int read() override {
    if (done_) return -1;
    if (remain_ == 0) {
      String l = c_.readStringUntil('\n');
      if (l.length() == 0) { done_ = true; return -1; }
      remain_ = (int)strtol(l.c_str(), nullptr, 16);
      if (remain_ <= 0) { done_ = true; c_.readStringUntil('\n'); return -1; }
    }
    uint32_t t0 = millis();
    while (!c_.available()) {
      if (!c_.connected() || millis() - t0 > 8000) { done_ = true; return -1; }
      delay(1);
    }
    const int b = c_.read();
    if (--remain_ == 0) c_.readStringUntil('\n');   // CRLF hinter dem Block
    return b;
  }
  int peek() override { return -1; }                  // wird vom JSON-Leser nicht benoetigt
  size_t write(uint8_t) override { return 0; }
  void drain() { while (!done_) { if (read() < 0) break; } }
 private:
  WiFiClient& c_;
  int remain_ = 0;
  bool done_ = false;
};

static bool fetchPlanesFrom(const char* fmt, PlaneList& out) {
  const float km = (float)gSet.radarKm;
  int nm = (int)ceilf(km / 1.852f);
  if (nm < 1) nm = 1;
  char url[128];
  snprintf(url, sizeof(url), fmt, (double)gSet.lat, (double)gSet.lon, nm);

  // Verbindung bleibt zwischen den Abrufen offen (Keep-Alive): der teure TLS-Handshake
  // (rund eine halbe Sekunde Rechenzeit auf dem C3) faellt dann nur beim ersten Abruf an.
  if (!gRadarClient) { gRadarClient = new WiFiClientSecure(); gRadarClient->setInsecure(); }
  static HTTPClient http;
  static const char* lastFmt = nullptr;
  if (lastFmt != fmt) { http.end(); gRadarClient->stop(); lastFmt = fmt; }
  http.setReuse(true);
  http.setConnectTimeout(8000);
  http.setTimeout(8000);
  http.setUserAgent("WetterUhr/1.0");
  if (!http.begin(*gRadarClient, url)) return false;
  const int code = http.GET();
  if (code != HTTP_CODE_OK) { LOGF("Radar: HTTP %d\n", code); http.end(); gRadarClient->stop(); return false; }

  JsonDocument filter;                               // nur die noetigen Felder behalten (spart RAM)
  static const char* const KEYS[] = {"flight", "hex", "lat", "lon", "alt_baro", "alt_geom", "gs", "track", "t",
                                     "baro_rate", "geom_rate", "seen_pos"};
  for (const char* k : KEYS) { filter["ac"][0][k] = true; filter["aircraft"][0][k] = true; }
  JsonDocument doc;
  DeserializationError err;
  if (http.getSize() < 0) {                          // "chunked": Bloecke selbst entpacken
    ChunkStream cs(*gRadarClient);
    err = deserializeJson(doc, cs, DeserializationOption::Filter(filter));
    cs.drain();                                      // Rest lesen, damit die Verbindung wiederverwendbar bleibt
  } else {
    err = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));
  }
  http.end();                                        // Verbindung bleibt wegen setReuse(true) offen
  if (err) gRadarClient->stop();
  if (err) { LOGF("Radar: JSON %s\n", err.c_str()); return false; }
  JsonArray arr = doc["ac"].as<JsonArray>();
  if (arr.isNull()) arr = doc["aircraft"].as<JsonArray>();
  if (arr.isNull()) return false;

  out.n = 0;
  const double lat0 = gSet.lat, lon0 = gSet.lon;
  const double kx = 111.32 * cos(lat0 * 0.0174533);
  const float maxD2 = km * km * 1.1f;
  for (JsonObject a : arr) {
    if (!a["lat"].is<double>() || !a["lon"].is<double>()) continue;
    Plane p;
    memset(&p, 0, sizeof(p));
    p.x = (float)((a["lon"].as<double>() - lon0) * kx);
    p.y = (float)((a["lat"].as<double>() - lat0) * 111.0);
    if (p.x * p.x + p.y * p.y > maxD2) continue;
    JsonVariant ab = a["alt_baro"];
    if (ab.is<const char*>()) p.ground = 1;                              // "ground"
    else p.alt = ab.is<int>() ? ab.as<int>() : (int)(a["alt_geom"] | 0);
    p.gs  = (int16_t)(a["gs"] | 0.0f);
    p.trk = a["track"].is<float>() ? (int16_t)(((int)lroundf(a["track"].as<float>()) % 360 + 360) % 360) : (int16_t)-1;
    p.vr  = (int16_t)(a["baro_rate"] | (int)(a["geom_rate"] | 0));
    const char* fl = a["flight"] | "";
    while (*fl == ' ') fl++;
    strlcpy(p.call, fl, sizeof(p.call));
    for (int i = (int)strlen(p.call) - 1; i >= 0 && p.call[i] == ' '; i--) p.call[i] = 0;
    if (!p.call[0]) { const char* hx = a["hex"] | ""; strlcpy(p.call, hx, sizeof(p.call)); for (char* q = p.call; *q; q++) *q = toupper((unsigned char)*q); }
    strlcpy(p.type, a["t"] | "", sizeof(p.type));
    p.id = (uint32_t)strtoul(a["hex"] | "0", nullptr, 16);
    if (p.trk >= 0 && p.gs > 0) {
      const float v = p.gs * 1.852f / 3600.0f, r = p.trk * 0.0174533f;
      p.vx = v * sinf(r); p.vy = v * cosf(r);
      const float sp = a["seen_pos"] | 0.0f;                             // Alter der Positionsmeldung
      if (sp > 0.0f && sp < 20.0f) { p.x += p.vx * sp; p.y += p.vy * sp; }
    }
    planeInsert(out, p);
  }
  return true;
}

static void radarTask(void*) {
  static PlaneList nl;                                // statisch: schont den Task-Stack
  uint32_t lastTry = 0;
  bool have = false;
  for (;;) {
    const uint32_t now = millis();
    const uint32_t wa = gRadarWantedAt;
    const bool wanted = wa != 0 && now - wa < 4000;
    static bool wasWanted = false;
    if (wanted && !wasWanted) have = false;           // gerade eingeschaltet: sofort abrufen
    wasWanted = wanted;
    if (wanted && WiFi.status() == WL_CONNECTED && (!have || now - lastTry >= RADAR_POLL_MS)) {
      lastTry = now; have = true;
      bool ok = false;
      xSemaphoreTake(gNetMutex, portMAX_DELAY);
      for (int k = 0; k < 3 && !ok; k++) {
        const uint8_t h = (gRadarHost + k) % 3;
        ok = fetchPlanesFrom(RADAR_URL[h], nl);
        if (ok) gRadarHost = h;
      }
      xSemaphoreGive(gNetMutex);
      xSemaphoreTake(gMutex, portMAX_DELAY);
      if (ok) { nl.status = 1; nl.radiusKm = (float)gSet.radarKm; nl.fetchedMs = millis(); gPlanes = nl; }
      else    { gPlanes.status = 2; }                  // alte Flugzeuge bleiben sichtbar (werden als veraltet markiert)
      gPlanesVer++;
      xSemaphoreGive(gMutex);
      LOGF("Radar: %s, %d Flugzeuge\n", ok ? "ok" : "Fehler", ok ? (int)nl.n : -1);
    }
    if (!wanted && gRadarClient && gRadarClient->connected() && now - wa > 20000UL)
      gRadarClient->stop();                         // Radar nicht mehr gebraucht: TLS-Speicher freigeben
    if (!wanted && have && now - wa > 60000UL) {      // lange nicht gebraucht: Daten verwerfen
      have = false;
      xSemaphoreTake(gMutex, portMAX_DELAY);
      memset(&gPlanes, 0, sizeof(gPlanes));
      gPlanesVer++;
      xSemaphoreGive(gMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// =====================================================================
// ISS-Position (Design "ISS-Tracker"): einfacher HTTP-Abruf ohne TLS (kein Rechenzeit-Stau),
// nur solange das Design angezeigt wird
// =====================================================================
#define ISS_POLL_MS 5000UL
static IssData           gIss;                    // vom issTask geschrieben (mit gMutex geschuetzt)
static uint16_t          gIssVer = 0;
static volatile uint32_t gIssWantedAt = 0;

static float issDistKm(float la1, float lo1, float la2, float lo2) {
  const float p1 = la1 * 0.0174533f, p2 = la2 * 0.0174533f, dl = (lo2 - lo1) * 0.0174533f;
  const float s1 = sinf((p2 - p1) / 2), s2 = sinf(dl / 2);
  float a = s1 * s1 + cosf(p1) * cosf(p2) * s2 * s2;
  if (a > 1) a = 1;
  return 12742.0f * asinf(sqrtf(a));
}
static float jsonNum(JsonVariantConst v) { return v.is<const char*>() ? (float)atof(v.as<const char*>()) : v.as<float>(); }

static bool fetchIss(IssData& d) {
  static uint32_t prevTs = 0;
  WiFiClient client;
  HTTPClient http;
  http.setConnectTimeout(5000);
  http.setTimeout(5000);
  if (!http.begin(client, "http://api.open-notify.org/iss-now.json")) return false;
  if (http.GET() != HTTP_CODE_OK) { http.end(); return false; }
  JsonDocument doc;
  const DeserializationError err = deserializeJson(doc, http.getStream());
  http.end();
  if (err) return false;
  if (!doc["iss_position"]["latitude"].is<const char*>() && !doc["iss_position"]["latitude"].is<float>()) return false;
  const float la = jsonNum(doc["iss_position"]["latitude"]), lo = jsonNum(doc["iss_position"]["longitude"]);
  const uint32_t ts = doc["timestamp"] | 0;
  if (d.status == 1 && ts > prevTs && ts - prevTs <= 60) {
    const float dt = (float)(ts - prevTs);
    float dlon = lo - d.lon;
    if (dlon > 180) dlon -= 360;
    if (dlon < -180) dlon += 360;
    d.vlat = (la - d.lat) / dt;
    d.vlon = dlon / dt;
    d.speedKmh = issDistKm(d.lat, d.lon, la, lo) / dt * 3600.0f;
  } else if (d.status != 1 || ts - prevTs > 60) { d.vlat = d.vlon = 0; }
  d.lat = la; d.lon = lo;
  prevTs = ts;
  return true;
}

static uint8_t fetchCrew() {                      // Personen auf der ISS (open-notify astros.json), 0 = unbekannt
  WiFiClient client;
  HTTPClient http;
  http.setConnectTimeout(5000);
  http.setTimeout(5000);
  if (!http.begin(client, "http://api.open-notify.org/astros.json")) return 0;
  if (http.GET() != HTTP_CODE_OK) { http.end(); return 0; }
  JsonDocument filter;
  filter["people"][0]["craft"] = true;
  JsonDocument doc;
  const DeserializationError err = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));
  http.end();
  if (err) return 0;
  int n = 0;
  for (JsonObject p : doc["people"].as<JsonArray>()) if (strcmp(p["craft"] | "", "ISS") == 0) n++;
  return (uint8_t)n;
}

static void issTask(void*) {
  static IssData nd;
  uint32_t lastTry = 0, lastCrew = 0;
  bool have = false, wasWanted = false;
  for (;;) {
    const uint32_t now = millis();
    const uint32_t wa = gIssWantedAt;
    const bool wanted = wa != 0 && now - wa < 4000;
    if (wanted && !wasWanted) have = false;
    wasWanted = wanted;
    if (wanted && WiFi.status() == WL_CONNECTED && (!have || now - lastTry >= ISS_POLL_MS)) {
      lastTry = now; have = true;
      xSemaphoreTake(gMutex, portMAX_DELAY); nd = gIss; xSemaphoreGive(gMutex);
      const bool ok = fetchIss(nd);
      if (ok && (lastCrew == 0 || now - lastCrew > 1800000UL)) { lastCrew = now ? now : 1; const uint8_t c = fetchCrew(); if (c) nd.crew = c; }
      xSemaphoreTake(gMutex, portMAX_DELAY);
      if (ok) { nd.status = 1; nd.fetchedMs = millis(); gIss = nd; }
      else    { gIss.status = 2; }
      gIssVer++;
      xSemaphoreGive(gMutex);
      LOGF("ISS: %s\n", ok ? "ok" : "Fehler");
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

// =====================================================================
// Browser-Einstellungsseite (braucht die Definitionen oben)
// =====================================================================
#include "webui.h"

// =====================================================================
// Zeichnen: Ein-Bildschirm-Layout (320 x 240, Querformat)
//   y   0..26   Statusleiste (WLAN, IP/Hinweis, Wetter-Status)
//   y  28..136  Uhr - klassische Designs (ab 2 folgen die Themen-Zifferblaetter, faces.h):
//                 0 Flip-Karten         (zwei Karten: Stunden / Minuten)
//                 1 Digital + Balken    (grosse Zeit, Sekundenbalken)
//   y 140..204  Wetter aktuell (Symbol, Temperatur, Zustand, Details)
//   y 208..238  Vorhersage (3 Tage)
// =====================================================================
static const int CLOCK_Y = 28;

static void drawStatic() {
  tft.drawFastHLine(10, 138, tft.width() - 20, C_LINE);
}

static void updateStatus(bool timeOk, const Weather& w, bool hv, bool stale) {
  (void)timeOk;
  const int W = tft.width();
  const bool wf = (WiFi.status() == WL_CONNECTED);
  char b[48];
  snprintf(b, sizeof(b), "%d%d%d%d|%lu", wf, apActive, hv, stale, wf ? (unsigned long)(uint32_t)WiFi.localIP() : 0UL);
  if (!upd(K_STAT, b)) return;

  tft.fillRect(0, 0, W, 27, C_BG);
  drawWifiIcon(8, 5, wf);

  tft.setFont(&FreeSans9pt7b);
  tft.setTextSize(1);
  char left[40];
  uint16_t lcol = C_GREY;
  if (apActive) {
    snprintf(left, sizeof(left), "Setup: %s", WiFi.softAPIP().toString().c_str());
    lcol = C_ORANGE;
  } else if (wf) {
    snprintf(left, sizeof(left), "%s", WiFi.localIP().toString().c_str());
  } else {
    snprintf(left, sizeof(left), "Kein WLAN");
    lcol = C_RED;
  }
  tft.setTextColor(lcol);
  tft.setCursor(30, 19);
  tft.print(left);

  const char* rt = nullptr;
  uint16_t rcol = C_ORANGE;
  if (!hv)        { rt = "kein Wetter"; rcol = C_RED; }
  else if (stale) { rt = "Wetter veraltet"; }
  if (rt) {
    int16_t x1, y1; uint16_t tw, th;
    tft.getTextBounds(rt, 0, 19, &x1, &y1, &tw, &th);
    tft.setTextColor(rcol);
    tft.setCursor(W - 8 - (int16_t)tw - x1, 19);
    tft.print(rt);
  }
  (void)w;
}

static void drawSeconds(bool ok, const struct tm& t, int sx, int baseline) {
  if (!gSet.seconds) return;
  char b[8];
  if (ok) snprintf(b, sizeof(b), "%02d", t.tm_sec); else snprintf(b, sizeof(b), "--");
  if (upd(K_SEC, b)) drawText(&FreeSansBold12pt7b, 1, b, sx, baseline, C_ACCENT, AL_LEFT, sx - 2, baseline - 22, 40, 28);
}

static void drawDateLine(bool ok, const struct tm& t) {
  char b[48];
  if (ok) snprintf(b, sizeof(b), "%s, %02d.%02d.%04d", WEEKDAY[t.tm_wday], t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
  else    snprintf(b, sizeof(b), "--.--.----");
  if (upd(K_DATE, b)) drawText(&FreeSans12pt7b, 1, b, tft.width() / 2, 130, C_WHITE, AL_CENTER, 0, 110, tft.width(), 26);
}

// Design 0: zwei Karten (Stunden / Minuten)
static void drawCard(int x, int y, int w, int h, const char* txt, uint16_t col) {
  tft.fillRoundRect(x, y, w, h, 8, C_DIM);
  tft.setFont(&FreeSansBold18pt7b);
  tft.setTextSize(2);
  tft.setTextColor(col);
  int16_t x1, y1;
  uint16_t tw, th;
  tft.getTextBounds(txt, 0, 0, &x1, &y1, &tw, &th);
  tft.setCursor(x + (w - (int)tw) / 2 - x1, y + (h - (int)th) / 2 - y1);
  tft.print(txt);
  tft.drawFastHLine(x, y + h / 2, w, C_BG);       // Klappkante
  tft.drawFastHLine(x, y + h / 2 + 1, w, C_BG);
}

static void updateFlip(bool ok, const struct tm& t) {
  const int cw = 120, ch = 74, x0 = 35, y0 = CLOCK_Y + 2;
  char b[8];
  if (ok) snprintf(b, sizeof(b), "%02d", t.tm_hour); else snprintf(b, sizeof(b), "--");
  if (upd(K_H1, b)) drawCard(x0, y0, cw, ch, b, C_WHITE);
  if (ok) snprintf(b, sizeof(b), "%02d", t.tm_min); else snprintf(b, sizeof(b), "--");
  if (upd(K_M1, b)) drawCard(x0 + cw + 10, y0, cw, ch, b, C_ACCENT);
  drawSeconds(ok, t, 288, y0 + ch - 4);
  drawDateLine(ok, t);
}

// Design 1: grosse Zeit mit Sekundenbalken
static void updateBarClock(bool ok, const struct tm& t) {
  char b[16];
  if (ok) snprintf(b, sizeof(b), "%02d:%02d", t.tm_hour, t.tm_min); else snprintf(b, sizeof(b), "--:--");
  if (upd(K_ATIME, b)) drawText(&FreeSansBold24pt7b, 2, b, tft.width() / 2, 94, C_WHITE, AL_CENTER, 0, 28, tft.width(), 70);
  const int sec = (ok && gSet.seconds) ? t.tm_sec : -1;
  snprintf(b, sizeof(b), "%d", sec);
  if (upd(K_BAR, b)) {
    tft.fillRoundRect(20, 101, 280, 6, 3, C_DIM);
    if (sec >= 0) {
      int w = (sec + 1) * 280 / 60;
      if (w < 6) w = 6;
      tft.fillRoundRect(20, 101, w, 6, 3, C_ACCENT);
    }
  }
  drawDateLine(ok, t);
}

static void updateClock(bool ok) {
  time_t n = time(nullptr);
  struct tm t;
  localtime_r(&n, &t);
  if (gStyle == 1) updateBarClock(ok, t);
  else             updateFlip(ok, t);
}

static int textW(const GFXfont* f, const char* txt) {
  tft.setFont(f);
  tft.setTextSize(1);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds(txt, 0, 0, &x1, &y1, &w, &h);
  return (int)w;
}

static void drawWeatherBlock(const Weather& w, bool hv, bool stale) {
  const int W = tft.width();
  tft.fillRect(0, 140, W, 100, C_BG);

  if (!hv) {
    drawText(&FreeSans12pt7b, 1, "Keine Wetterdaten", W / 2, 180, C_ORANGE, AL_CENTER, 0, 160, W, 28);
    return;
  }

  const uint16_t tcol = stale ? C_ORANGE : C_WHITE;
  char b[24];

  // Symbol, Temperatur, Zustand
  drawIcon(tft, 40, 172, 26, iconForCode(w.code, w.isDay));
  drawTemp(80, 180, (int)lroundf(w.temp), &FreeSansBold18pt7b, 1, tcol, AL_LEFT, 74, 148, 108, 40);
  drawText(&FreeSans9pt7b, 1, textForCode(w.code), 80, 200, C_ACCENT, AL_LEFT, 74, 188, 110, 16);

  // Details rechts: Gefuehlt / Feuchte / Wind
  const int lx = 188, rx = W - 8;
  drawText(&FreeSans9pt7b, 1, "Gefuehlt", lx, 162, C_GREY, AL_LEFT, lx - 2, 148, 56, 18);
  drawTemp(rx, 162, (int)lroundf(w.feels), &FreeSansBold9pt7b, 1, tcol, AL_RIGHT, rx - 52, 148, 54, 18);
  drawText(&FreeSans9pt7b, 1, "Feuchte", lx, 181, C_GREY, AL_LEFT, lx - 2, 167, 56, 18);
  snprintf(b, sizeof(b), "%d %%", w.hum);
  drawText(&FreeSansBold9pt7b, 1, b, rx, 181, tcol, AL_RIGHT, rx - 52, 167, 54, 18);
  drawText(&FreeSans9pt7b, 1, "Wind", lx, 200, C_GREY, AL_LEFT, lx - 2, 186, 56, 18);
  snprintf(b, sizeof(b), "%d km/h", (int)lroundf(w.wind));
  drawText(&FreeSansBold9pt7b, 1, b, rx, 200, tcol, AL_RIGHT, rx - 66, 186, 68, 18);

  // Vorhersage (heute + 2 Tage); bei zu langem Text ohne Wochentag
  for (int i = 0; i < w.nDays && i < 3; i++) {
    const int bx = 6 + i * 105;
    tft.drawRoundRect(bx, 208, 100, 30, 6, C_LINE);
    drawIcon(tft, bx + 17, 223, 10, iconForCode(w.dCode[i], true));
    int dow = dayOfWeekFromIso(w.dDate[i]);
    const int mx = (int)lroundf(w.dMax[i]), mn = (int)lroundf(w.dMin[i]);
    snprintf(b, sizeof(b), "%s %d/%d", dow >= 0 ? WEEKDAY2[dow] : "--", mx, mn);
    if (textW(&FreeSans9pt7b, b) > 62) snprintf(b, sizeof(b), "%d/%d", mx, mn);
    drawText(&FreeSans9pt7b, 1, b, bx + 34, 228, C_WHITE, AL_LEFT, bx + 32, 211, 66, 24);
  }
}

static void updateWeatherBlock(const Weather& w, bool hv, bool stale) {
  char b[48];
  snprintf(b, sizeof(b), "%lu|%d|%d", (unsigned long)w.version, hv, stale);
  if (upd(K_WX, b)) drawWeatherBlock(w, hv, stale);
}

// =====================================================================
// Darstellung (ca. 10x pro Sekunde, zeichnet nur Geaendertes)
// =====================================================================
// ---------------------------------------------------------------------
// Themen-Zifferblaetter (Design 2..31): eigener Zeichenpfad mit 160x120-Puffer.
// Rueckgabe true = diese Funktion hat den Bildschirm bedient (klassischer Pfad ueberspringen).
// ---------------------------------------------------------------------
static bool themedTick(uint32_t now) {
  static bool outro = false;
  static uint32_t ipAt = 0;
  static char ipBuf[24] = "";
  const int  want  = gStyle;
  const bool wantT = fc::isThemed(want);
  const int  wIdx  = want - FACE_FIRST_STYLE;
  const uint8_t pct = HW_DIM ? 100 : effectiveBright();

  if (fc::cur >= 0) {
    if (!wantT || wIdx != fc::cur) {            // anderes Design gewaehlt: erst Ausblenden
      if (!outro) { fc::startOutro(now); outro = true; }
      if (fc::outroDone(now)) {
        fc::release();
        outro = false;
        if (!wantT) { needFull = true; return false; }
      }
    } else if (outro) {
      fc::cancelOutro(now);
      outro = false;
    }
  }
  if (fc::cur < 0) {
    if (!wantT) return false;
    if (!fc::begin(wIdx, now, true, pct)) {     // Speicher reicht nicht: normales Design verwenden
      LOGF("Zifferblatt: kein Speicher\n");
      if (nyeNow()) gNyeBlocked = true;                 // Vorrang-Design: gewaehltes Design behalten
      else if (nightTv()) gNightTvBlocked = true;
      else gSet.clockStyle = 0;
      gStyle = 0;
      needFull = true;
      return false;
    }
    needFull = false;
  }
  if (needFull) {                               // Helligkeit/Aufwachen/Drehung: alles neu senden
    fc::setBrightness(pct);
    fc::invalidate();
    needFull = false;
  }

  FaceCtx c;
  memset(&c, 0, sizeof(c));
  c.now = now;
  c.timeOk = timeValid();
  if (c.timeOk) {
    time_t n = time(nullptr);
    struct tm t;
    localtime_r(&n, &t);
    c.hh = t.tm_hour; c.mm = t.tm_min; c.ss = t.tm_sec; c.wday = t.tm_wday;
    c.mday = t.tm_mday; c.mon = t.tm_mon + 1; c.year = t.tm_year + 1900;
  }
  c.showSec = gSet.seconds;
  Weather w;
  const bool hv = getWeather(w);
  c.hasWx = hv;
  c.stale = hv && (now - w.fetchedMs > staleMs());
  if (hv) {
    c.temp = (int)lroundf(w.temp); c.wcode = w.code; c.hum = w.hum;
    c.wind = (int)lroundf(w.wind); c.isDay = w.isDay;
  }
  c.wifi = WiFi.status() == WL_CONNECTED;
  c.apMode = apActive;
  if (now - ipAt > 1000 || ipBuf[0] == 0) {
    ipAt = now;
    if (apActive)    snprintf(ipBuf, sizeof(ipBuf), "Setup: %s", WiFi.softAPIP().toString().c_str());
    else if (c.wifi) snprintf(ipBuf, sizeof(ipBuf), "%s", WiFi.localIP().toString().c_str());
    else             ipBuf[0] = 0;
  }
  c.info = ipBuf[0] ? ipBuf : nullptr;

  c.lat = gSet.lat; c.lon = gSet.lon;
  c.unixT = c.timeOk ? (double)time(nullptr) : 0.0;
  c.opts = (gSet.ufoOn ? OPT_UFO : 0) | (sleighNow() ? OPT_SLEIGH : 0);
  if (want == ISS_STYLE && !(c.opts & OPT_SLEIGH)) {   // ISS-Tracker: Daten vom issTask abholen (nicht beim Schlitten)
    static IssData issView;
    static uint16_t issVer = 0xFFFF;
    gIssWantedAt = now ? now : 1;
    if (xSemaphoreTake(gMutex, pdMS_TO_TICKS(2)) == pdTRUE) {
      if (gIssVer != issVer) { issView = gIss; issVer = gIssVer; }
      xSemaphoreGive(gMutex);
    }
    c.iss = &issView;
  }
  if (want == RADAR_STYLE) {                    // Flugradar: Daten vom radarTask abholen
    static PlaneList view;
    static uint16_t viewVer = 0xFFFF;
    gRadarWantedAt = now ? now : 1;
    if (xSemaphoreTake(gMutex, pdMS_TO_TICKS(2)) == pdTRUE) {
      if (gPlanesVer != viewVer) { view = gPlanes; viewVer = gPlanesVer; }
      xSemaphoreGive(gMutex);
    }
    c.planes = &view;
  }

  const uint32_t t0 = micros();
  const bool drew = fc::frame(tft, c);
  if (drew) {
    const uint32_t dt = micros() - t0;
    if (dt > gDiagFrameMax) gDiagFrameMax = dt;
  }
  if (showAfterDraw) {
    tft.enableDisplay(true);
    blApply();
    showAfterDraw = false;
  }
  return true;
}

static void renderTick(uint32_t now) {
  if (themedTick(now)) return;
  static uint32_t last = 0;
  if (!needFull && now - last < 100) { tickerClassic(now); return; }
  last = now;

  if (needFull) {
    tft.fillScreen(C_BG);
    resetCaches();
    needFull = false;
    drawStatic();
  }

  const bool ok = timeValid();
  Weather w;
  const bool hv = getWeather(w);
  const bool stale = hv && (millis() - w.fetchedMs > staleMs());

  updateStatus(ok, w, hv, stale);
  updateClock(ok);
  updateWeatherBlock(w, hv, stale);
  tickerClassic(now);

  if (showAfterDraw) {
    tft.enableDisplay(true);
    blApply();
    showAfterDraw = false;
  }
}

// =====================================================================
// Setup / Loop
// =====================================================================
void setup() {
#if PIN_TOUCH >= 0
  pinMode(PIN_TOUCH, TOUCH_ACTIVE_HIGH ? INPUT_PULLDOWN : INPUT_PULLUP);
#endif
#if DEBUG_SERIAL
  Serial.begin(115200);
#if defined(ARDUINO_USB_CDC_ON_BOOT) && ARDUINO_USB_CDC_ON_BOOT
  Serial.setTxTimeoutMs(0);      // USB-CDC ohne Leser darf die Schleife nie blockieren
#endif
#endif
  bz::init();                    // Summer-Pin sofort auf "aus"
  settingsLoad();
  gStyle = listStyle(gSet.clockStyle);   // aktiver Wechsel: gleich mit einem Design aus der Liste starten
  gStyleSince = millis();
  applyTimezone();
  themeApply();
  blInit();
  bootMs = millis();
  lastConnectedMs = bootMs;

  displayInit();
  blApply();
  drawText(&FreeSans12pt7b, 1, "Starte ...", tft.width() / 2, tft.height() / 2, C_GREY, AL_CENTER,
           0, tft.height() / 2 - 30, tft.width(), 45);

  gMutex = xSemaphoreCreateMutex();
  wifiBegin();
  webBegin();
  gNetMutex = xSemaphoreCreateMutex();
  xTaskCreate(weatherTask, "weather", 10240, nullptr, 1, nullptr);
  xTaskCreate(radarTask, "radar", 12288, nullptr, 0, nullptr);   // Prioritaet 0: der Netzabruf bremst die Anzeige nie
  xTaskCreate(issTask, "iss", 8192, nullptr, 0, nullptr);

  resetCaches();
  needFull = true;
}

void loop() {
  const uint32_t now = millis();
  {
    static uint32_t prev = 0, win = 0;
    if (prev && now - prev > gDiagGapMax) gDiagGapMax = now - prev;
    prev = now;
    if (now - win >= 10000) {            // Zehn-Sekunden-Fenster auswerten
      win = now;
      gDiagGapLast = gDiagGapMax; gDiagFrameLast = gDiagFrameMax;
      LOGF("Diag: max Schleifenluecke %lu ms, max Bildzeit %lu us\n", (unsigned long)gDiagGapMax, (unsigned long)gDiagFrameMax);
      for (int k = 0; k < 4; k++) { gDiagSecLast[k] = gDiagSec[k]; gDiagSec[k] = 0; }
      LOGF("Diag: wlan %lu web %lu zeichnen %lu delay %lu us\n", (unsigned long)gDiagSecLast[0], (unsigned long)gDiagSecLast[1], (unsigned long)gDiagSecLast[2], (unsigned long)gDiagSecLast[3]);
      gDiagGapMax = 0; gDiagFrameMax = 0;
    }
  }
  uint32_t m0 = micros();
  auto lap = [&](int k) { const uint32_t t = micros(); const uint32_t d = t - m0; m0 = t; if (d > gDiagSec[k]) gDiagSec[k] = d; };
  wifiLoop(now);
  lap(0);
  webLoop(now);
#if ARDUINO_OTA
  if (gArduinoOtaOn) ArduinoOTA.handle();
#endif
  lap(1);
  dimTick(now, false);
  styleTick(now);
  soundTick(now);
  tickerTick(now);
#if PIN_TOUCH >= 0
  touchTick(now);
#endif
  if (wakePending && (int32_t)(now - wakeAt) >= 0) finishWake();
  m0 = micros();
  if (!standby && !wakePending && !gOtaActive) renderTick(now);
  lap(2);
  if (setDirty && now - setDirtyAt > 3000) {   // Helligkeit erst nach Ruhephase speichern
    settingsSave();
    setDirty = false;
  }
  m0 = micros();
  delay(2);   // gibt nur CPU-Zeit an andere Tasks ab; keine Wartelogik
  lap(3);
}
