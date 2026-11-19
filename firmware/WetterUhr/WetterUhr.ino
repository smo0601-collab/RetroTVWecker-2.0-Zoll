/*
  Wetteruhr / RetroTV-Wecker - zwei Boards aus einem Quellcode (Auswahl automatisch, siehe config.h):
    - CYD 2,8 Zoll (ESP32-2432S028R): ESP32, ILI9341 320x240, Touchscreen XPT2046, Lautsprecher, RGB-LED
      Arduino IDE: Board "ESP32 Dev Module"
    - ESP32-C3 SuperMini + GMT020-02-7P (ST7789 320x240) + TTP223 + aktiver Summer
      Arduino IDE: Board "ESP32C3 Dev Module", USB CDC On Boot: Enabled
  Partitionsschema bei beiden: Minimal SPIFFS (1.9MB APP with OTA)

  Einstellungen per Browser: http://<IP-Adresse> oder http://wetteruhr.local
  (WLAN, Zeitzone, Standort, Helligkeit, ...). Ohne WLAN-Daten startet der
  Access-Point "Wetteruhr-Setup" (Seite: http://192.168.4.1).

  Dimmung:
    - mit Backlight-PWM (PIN_TFT_BL >= 0, beim CYD immer): echte PWM-Dimmung, Standby = Backlight aus
    - ohne (C3-Modul ohne Umbau): Software-Dimmung (Farben abdunkeln), Backlight bleibt an

  Bibliotheken: Adafruit GFX, Adafruit ST7735 and ST7789, Adafruit ILI9341, ArduinoJson (v7)
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <esp_heap_caps.h>
#include <SPI.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <ArduinoJson.h>
#include <esp_idf_version.h>
#include <esp_sntp.h>
#include <esp_netif.h>
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
#include "board.h"        // Display, Touch und LED je nach Board (CYD oder ESP32-C3)
#include "icons.h"
#include "settings.h"
#include "types.h"
#include "cal.h"          // Kalender: Feiertage, besondere Tage, Schulferien
#include "buzzer.h"       // Summer (ESP32-C3) bzw. Lautsprecher mit Klaengen (CYD), Wecker
#include "faces.h"        // Themen-Zifferblaetter (Design 2..31)

// ---- Protokoll: Serial (wenn DEBUG_SERIAL) und Ringpuffer im RTC-Speicher ----
// Der RTC-Speicher uebersteht Neustart und Absturz: unter http://<uhr>/log steht auch, was vor dem letzten
// Neustart passiert ist (zwei Puffer: vorige und aktuelle Sitzung).
#define RLOG_SZ 2000
RTC_NOINIT_ATTR static char rtcLog[2][RLOG_SZ];
RTC_NOINIT_ATTR static uint32_t rtcLogPos[2], rtcLogWrap[2], rtcLogCur, rtcLogMagic;
static portMUX_TYPE gLogMux = portMUX_INITIALIZER_UNLOCKED;
// Zeilen fuer das dauerhafte Protokoll im Flash (plog.h) warten hier, bis die Hauptschleife sie schreibt. Liegt im
// RTC-Speicher: nach Absturz oder Neustart werden sie beim naechsten Start noch gespeichert.
#define RPEND_SZ 1536
RTC_NOINIT_ATTR static char rtcPend[RPEND_SZ];
RTC_NOINIT_ATTR static uint32_t rtcPendLen, rtcPendLost;
static void rlogBoot() {
  if (rtcLogMagic != 0x4C4F4731u || rtcLogCur > 1 || rtcLogPos[0] >= RLOG_SZ || rtcLogPos[1] >= RLOG_SZ || rtcPendLen > RPEND_SZ) {
    memset(rtcLog, 0, sizeof(rtcLog)); rtcLogPos[0] = rtcLogPos[1] = 0; rtcLogWrap[0] = rtcLogWrap[1] = 0; rtcLogCur = 0;
    rtcPendLen = 0; rtcPendLost = 0;
  } else rtcLogCur ^= 1;                               // vorige Sitzung bleibt im anderen Puffer stehen
  rtcLogPos[rtcLogCur] = 0; rtcLogWrap[rtcLogCur] = 0;
  rtcLogMagic = 0x4C4F4731u;
}
// persist: Zeile auch ins dauerhafte Protokoll (Flash). Gleiche Meldungen kurz hintereinander werden dort gezaehlt
// statt jedes Mal geschrieben (sonst fuellen z. B. Radar-Fehler alle 15 s den Speicher).
static volatile bool gTimeSet = false;     // Uhrzeit per NTP gesetzt (vorher: Zeit seit dem Start ins Protokoll)
static void rlogf(uint8_t persist, const char* fmt, ...) {   // persist: 0 nur RTC, 1 Flash (gleiche zaehlen), 2 Flash immer
  char b[320];                                         // die Zustandszeile hat rund 230 Zeichen
  const uint32_t s = millis() / 1000;
  int n = snprintf(b, sizeof(b), "%lu:%02lu ", (unsigned long)(s / 60), (unsigned long)(s % 60));
  va_list ap; va_start(ap, fmt);
  const int m = vsnprintf(b + n, sizeof(b) - n, fmt, ap);
  va_end(ap);
  if (m >= (int)sizeof(b) - n) b[sizeof(b) - 2] = '\n';  // zu lang: abschneiden, Zeilenende aber behalten
  if (persist) {
    char q[380];
    const char* msg = b + n;
    uint32_t h = 2166136261u;                          // Kennung der Meldung (ohne Zahlen: "HTTP -1" == "HTTP 403")
    for (const char* p = msg; *p; p++) if (!(*p >= '0' && *p <= '9')) h = (h ^ (uint8_t)*p) * 16777619u;
    static uint32_t lastH = 0, lastAt = 0, rep = 0;
    const uint32_t ms = millis();
    int qn = 0;
    if (persist == 1 && h == lastH && ms - lastAt < 15UL * 60000UL) { rep++; lastAt = ms; }
    else {
      if (rep) qn += snprintf(q, sizeof(q), "    (vorige Meldung %lu-mal wiederholt)\n", (unsigned long)rep);
      rep = 0; lastH = (persist == 1) ? h : 0; lastAt = ms;
      const time_t t = time(nullptr);
      if (gTimeSet && t > 1700000000) {
        struct tm lt; localtime_r(&t, &lt);
        qn += snprintf(q + qn, sizeof(q) - qn, "%02d.%02d. %02d:%02d:%02d ", lt.tm_mday, lt.tm_mon + 1, lt.tm_hour, lt.tm_min, lt.tm_sec);
      } else qn += snprintf(q + qn, sizeof(q) - qn, "[+%lu:%02lu] ", (unsigned long)(s / 60), (unsigned long)(s % 60));
      qn += snprintf(q + qn, sizeof(q) - qn, "%s", msg);
      if (qn > (int)sizeof(q) - 1) { qn = sizeof(q) - 1; q[qn - 1] = '\n'; }
      for (int i = 0; i < qn; i++) if ((uint8_t)q[i] == 0xFF) q[i] = '?';   // 0xFF markiert im Flash das Ende
      portENTER_CRITICAL(&gLogMux);
      if (rtcPendLen + qn <= RPEND_SZ) { memcpy(rtcPend + rtcPendLen, q, qn); rtcPendLen += qn; }
      else rtcPendLost++;
      portEXIT_CRITICAL(&gLogMux);
    }
  }
#if DEBUG_SERIAL
  Serial.print(b);
#endif
  portENTER_CRITICAL(&gLogMux);
  const uint32_t c = rtcLogCur & 1;
  for (const char* p = b; *p; p++) {
    rtcLog[c][rtcLogPos[c]] = *p;
    if (++rtcLogPos[c] >= RLOG_SZ) { rtcLogPos[c] = 0; rtcLogWrap[c] = 1; }
  }
  portEXIT_CRITICAL(&gLogMux);
}
static void rlogRead(String& out, uint32_t which) {       // which: 0 = aktuelle, 1 = vorige Sitzung
  const uint32_t c = (rtcLogCur ^ which) & 1;
  portENTER_CRITICAL(&gLogMux);
  const uint32_t pos = rtcLogPos[c], wrap = rtcLogWrap[c];
  portEXIT_CRITICAL(&gLogMux);
  if (wrap) for (uint32_t i = pos; i < RLOG_SZ; i++) { const char ch = rtcLog[c][i]; if (ch) out += ch; }
  for (uint32_t i = 0; i < pos; i++) { const char ch = rtcLog[c][i]; if (ch) out += ch; }
}
#define LOGF(...) rlogf(1, __VA_ARGS__)    // Ereignis: RTC-Protokoll und dauerhaftes Protokoll (Flash)
#define LOGR(...) rlogf(0, __VA_ARGS__)    // Routine: nur RTC-Protokoll (letzte Sitzungen)
#define LOGS(...) rlogf(2, __VA_ARGS__)    // Zustandszeile: immer ins Flash (nie zusammengefasst)
#include "plog.h"
// Wartende Zeilen ins Flash schreiben (nur aus der Hauptschleife)
static void plogTick(uint32_t now) {
  static uint32_t last = 0;
  if (now - last < 2000) return;
  last = now;
  char b[RPEND_SZ];
  uint32_t n, lost;
  portENTER_CRITICAL(&gLogMux);
  n = rtcPendLen; lost = rtcPendLost;
  memcpy(b, rtcPend, n);
  rtcPendLen = 0; rtcPendLost = 0;
  portEXIT_CRITICAL(&gLogMux);
  if (n) plog::write(b, n);
  if (lost) { char m[64]; const int k = snprintf(m, sizeof(m), "    (%lu Zeilen verloren: Puffer voll)\n", (unsigned long)lost); plog::write(m, k); }
}
static volatile uint32_t gNetOkAt = 0;    // letzter erfolgreicher Netzabruf (Wetter, Radar, ISS, NTP)
static volatile uint32_t gWebReq = 0;     // Anzahl Browser-Anfragen
static volatile uint32_t gIssShort = 0;   // kurze ISS-Aussetzer seit der letzten Zustandszeile
static char gWxErr[80] = "";              // letzter Fehler beim Wetterabruf (Anzeige im Status)
// Ein verschluesselter Abruf (TLS) braucht kurzzeitig rund 60-70 KB. Zusammen mit dem Puffer der feinen Schrift
// (38 KB) reicht der Speicher nicht: der Abruf scheitert ("connection refused"). Deshalb gibt die Anzeige den
// Puffer fuer die Dauer des Abrufs frei (Schrift ist 1-2 s lang grob) und holt ihn danach wieder.
static volatile bool gNetNeedMem = false;
static uint8_t  gStyle        = 0;       // gerade angezeigtes Design (bei Auto-Wechsel != gSet.clockStyle)
static bool wantHiText() { return (gSet.fine & 2) && !gNetNeedMem && gStyle != RADAR_STYLE; }
static void hiPolicy() {                  // nur aus der Hauptschleife aufrufen
  const bool sm = gSet.fine & 1, ht = wantHiText();
  if (sm != fc::hiSmooth || ht != fc::hiTextOn) {
    fc::hiSmooth = sm; fc::hiTextOn = ht;
    if (!ht) fc::hiFree();
    fc::invalidate();
  }
}
static void netMem(bool on) {             // aus dem Netz-Task
  gNetNeedMem = on;
  if (!on) return;
  for (int i = 0; i < 100 && fc::hiLayer; i++) vTaskDelay(pdMS_TO_TICKS(20));   // bis zu 2 s auf die Hauptschleife warten
}

// =====================================================================
// Globale Objekte und Zustand
// =====================================================================

static bool     standby       = false;
static bool     wakePending   = false;   // Sleep-Out laeuft (120 ms Wartezeit)
static uint32_t wakeAt        = 0;
static bool     showAfterDraw = false;   // Display erst nach dem Zeichnen einschalten
static bool     needFull      = true;
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
// Zuletzt eine Browser-Anfrage bearbeitet (millis). Solange der Browser die Uhr benutzt, pausieren Radar- und
// ISS-Abrufe und die offene TLS-Verbindung wird geschlossen: der Speicher (rund 40-70 KB) fehlt sonst dem Netz,
// die Seite kommt nur noch tropfenweise an und die Uhr steht bis zu einer Minute still.
static volatile uint32_t gWebAt = 0;
// Spuren fuer die Fehlersuche: was lief gerade, als die Uhr abstuerzte? Liegen im RTC-Speicher, der einen
// Absturz-Neustart uebersteht; Anzeige im Browser unter Status > Letzter Neustart.
RTC_NOINIT_ATTR static uint32_t rtcMagic, rtcCrumbLoop, rtcCrumbNet, rtcMinHeap, rtcUpMin;
#define gCrumbLoop rtcCrumbLoop
#define gCrumbNet  rtcCrumbNet
static uint32_t gPrevLoop = 0, gPrevNet = 0, gPrevMinHeap = 0, gPrevUpMin = 0;
// Task-Watchdog: welche Tasks liefen gerade auf Kern 0 und 1? (wird im Watchdog-Interrupt kurz vor dem Neustart gemerkt)
RTC_NOINIT_ATTR static char rtcWdt[2][16];
RTC_NOINIT_ATTR static uint32_t rtcWdtMagic;
static char gPrevWdt[2][16] = {"", ""};
extern "C" void IRAM_ATTR esp_task_wdt_isr_user_handler(void) {
  for (int c = 0; c < portNUM_PROCESSORS && c < 2; c++) {
    TaskHandle_t h = xTaskGetCurrentTaskHandleForCore(c);
    const char* n = h ? pcTaskGetName(h) : "?";
    int i = 0;
    for (; n && n[i] && i < 15; i++) rtcWdt[c][i] = n[i];
    rtcWdt[c][i] = 0;
  }
  rtcWdtMagic = 0x57444731u;
}
static bool gPrevValid = false;
static void crumbsBoot() {
  if (rtcMagic == 0x57554852u) {
    gPrevValid = true;
    gPrevLoop = rtcCrumbLoop; gPrevNet = rtcCrumbNet; gPrevMinHeap = rtcMinHeap; gPrevUpMin = rtcUpMin;
  }
  if (rtcWdtMagic == 0x57444731u && esp_reset_reason() == ESP_RST_TASK_WDT) {
    for (int c = 0; c < 2; c++) { memcpy(gPrevWdt[c], rtcWdt[c], 15); gPrevWdt[c][15] = 0; }
  }
  rtcWdtMagic = 0;
  rtcMagic = 0x57554852u; rtcCrumbLoop = 0; rtcCrumbNet = 0; rtcMinHeap = 0; rtcUpMin = 0;
}
// Gescheiterte Speicheranforderungen merken (Hook der Speicherverwaltung; hier nur Werte ablegen, das Protokoll
// schreibt heapGuard). Damit laesst sich klaeren, wer den freien Speicher zeitweise bis auf wenige Bytes aufbraucht.
static volatile uint32_t gAllocFail = 0, gAllocFailSize = 0, gAllocFailNet = 0, gAllocFailLoop = 0;
static char gAllocFailTask[16] = "";
static void allocFailHook(size_t size, uint32_t caps, const char*) {
  if (caps & MALLOC_CAP_EXEC) return;
  gAllocFail = gAllocFail + 1;
  if (size >= gAllocFailSize) {                        // die groesste gescheiterte Anforderung zaehlt
    gAllocFailSize = size; gAllocFailNet = gCrumbNet; gAllocFailLoop = gCrumbLoop;
    const char* n = pcTaskGetName(nullptr);
    int i = 0;
    for (; n && n[i] && i < 15; i++) gAllocFailTask[i] = n[i];
    gAllocFailTask[i] = 0;
  }
}
static volatile uint32_t gSlowFrameUs = 0, gSlowFrameTiles = 0, gSlowFrameStyle = 0, gSlowFrameNet = 0;
static bool webBusy() { const uint32_t w = gWebAt; return w && millis() - w < 6000; }

static volatile uint32_t gDiagSec[4] = {0,0,0,0}, gDiagSecLast[4] = {0,0,0,0};   // wifi, web, render, delay(2)
static volatile uint32_t gDiagGapMax = 0, gDiagFrameMax = 0, gDiagGapLast = 0, gDiagFrameLast = 0;
// Messwerte fuer das Protokoll (je 5 Minuten): Bilder, Summe der Bildzeiten, gesendete Kacheln, Schleifendurchlaeufe
static uint32_t gMFrames = 0, gMFrameUs = 0, gMTiles = 0, gMLoops = 0, gMFrameMax = 0;

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

#define WEEKDAY  (WDK_L[LANG_IDX])    // Wochentage in der eingestellten Sprache (lang.h)
#define WEEKDAY2 (WDK2_L[LANG_IDX])

// 0 = Sonntag; Eingabe "YYYY-MM-DD"
static int dayOfWeekFromIso(const char* s) {
  int y = 0, m = 0, d = 0;
  if (sscanf(s, "%d-%d-%d", &y, &m, &d) != 3 || m < 1 || m > 12) return -1;
  static const int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  if (m < 3) y -= 1;
  return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

// ---- Cache gegen unnoetiges Neuzeichnen (weniger Flackern) ----
enum { K_H1, K_M1, K_SEC, K_DATE, K_STAT, K_WX, K_ATIME, K_BAR, K_BIG, K_BCOL, K_COUNT };
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

// Kalender: Einblendung am Feier- oder besonderen Tag (cal.h, face_party.h)
static uint8_t  gPopId = 0;
static bool     gPopOn = false;          // laeuft gerade
static uint32_t gPopBlockedAt = 0;       // Einblendung liess sich nicht starten (Speicher): 10 min Pause
static bool     gPopSayReq = false;      // Laufschrift mit dem Spruch des Tages starten
static bool     gPopSndReq = false;      // Klang zur Einblendung (soundTick)
static bool popNow() {
  static uint32_t at = 0;
  static bool v = false;
  const uint32_t now = millis();
  if (at && now - at < 250) return v;
  at = now ? now : 1;
  uint8_t id = 0;
  const bool blocked = gPopBlockedAt && now - gPopBlockedAt < 600000UL;
  v = !standby && !blocked && timeValid() && cal::popWant(id, gDimActive);
  if (v && !gPopOn) { gPopId = id; gPopSayReq = true; gPopSndReq = true; }
  gPopOn = v;
  return v;
}

// Design, das gerade Vorrang hat (Silvester vor Nachtmodus vor Feiertags-Einblendung), -1 = keines
static int overrideStyle() {
  if (nyeNow()) return NEWYEAR_STYLE;
  if (nightTv()) return TESTCARD_STYLE;
  if (popNow()) return POP_STYLE;
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

static void enterStandby();
static void startWake();
static void dimTick(uint32_t now, bool force) {
  static uint32_t last = 0;
  static bool offBySchedule = false;                 // Display wurde vom Zeitplan ausgeschaltet
  static bool lastOffSet = false;                    // Einstellung "Display aus" beim letzten Durchlauf
  if (!force && now - last < 1000) return;
  last = now;
  const bool want = dimWindowNow();
  // Display ganz aus statt dimmen: zu Beginn des Fensters aus, am Ende wieder an. Wer es zwischendurch per
  // Beruehrung, Browser oder Wecker einschaltet, behaelt es bis zum naechsten Fenster. Wird die Einstellung
  // waehrend des Fensters eingeschaltet, geht das Display gleich aus; wird sie ausgeschaltet, gleich wieder an.
  if (offBySchedule && !standby) offBySchedule = false;               // zwischendurch eingeschaltet
  if (want && gSet.dimOff && !standby && (want != gDimActive || !lastOffSet)) { enterStandby(); offBySchedule = true; }
  else if ((!want || !gSet.dimOff) && offBySchedule) { offBySchedule = false; if (standby) startWake(); }
  lastOffSet = gSet.dimOff;
  if (want == gDimActive) return;
  gDimActive = want;
  if (!want) gNightTvBlocked = false;
  LOGF("Dimm-Zeitfenster: %s%s\n", want ? "an" : "aus", want && gSet.dimOff ? " (Display aus)" : "");
  if (HW_DIM) {
    blApply();
  } else {
    themeApply();
    needFull = true;
  }
}

// Feiertags-Designs, die gerade in ihrem Zeitfenster liegen (HOL_DAYS_BEFORE Tage vorher bis zum Fest):
//   Weihnachten 14.-26.12., Silvester 21.12.-1.1., Valentinstag 4.-14.2., Halloween 21.-31.10.,
//   Ostern 10 Tage vor Ostersonntag bis Ostermontag. Wird einmal pro Minute neu berechnet.
static uint32_t gHolAt = 0;              // 0 = neu berechnen (z. B. nach "Speichern")
static uint64_t holidayMask() {
  static uint64_t m = 0;
  const uint32_t now = millis();
  if (gHolAt && now - gHolAt < 60000UL) return m;
  gHolAt = now ? now : 1;
  m = 0;
  if (!gSet.holAuto || !timeValid()) return m;
  time_t n = time(nullptr);
  struct tm t;
  localtime_r(&n, &t);
  const int y = t.tm_year + 1900, mo = t.tm_mon + 1, d = t.tm_mday;
  const int doy = fc::dayOfYear(y, mo, d), B = HOL_DAYS_BEFORE;
  auto win = [&](int m1, int d1, int extra) {            // B Tage vor dem Termin bis "extra" Tage danach
    const int e = fc::dayOfYear(y, m1, d1);
    return doy >= e - B && doy <= e + extra;
  };
  if (win(12, 24, 2))                    m |= 1ULL << XMAS_STYLE;
  if (win(12, 31, 0) || (mo == 1 && d == 1)) m |= 1ULL << NEWYEAR_STYLE;
  if (win(2, 14, 0))                     m |= 1ULL << VALENTINE_STYLE;
  if (win(10, 31, 0))                    m |= 1ULL << HALLOWEEN_STYLE;
  int em, ed;
  fc::easterDate(y, em, ed);
  if (win(em, ed, 1))                    m |= 1ULL << EASTER_STYLE;
  return m;
}

// Liste fuer den Design-Wechsel: eigene Auswahl plus die gerade anstehenden Feiertage
static uint64_t listMask() { return gSet.autoMask | holidayMask(); }

// Design-Wechsel nach Zeitplan: nur wenn aktiv und mindestens zwei Designs gewaehlt sind
static int autoCount() {
  int n = 0;
  const uint64_t m = listMask();
  for (int i = 0; i < CLOCK_STYLE_COUNT; i++) if (m & (1ULL << i)) n++;
  return n;
}
// Naechstes Design nach "from": aus der Wechsel-Liste (wenn mindestens zwei gewaehlt), sonst aus allen
static int nextStyle(int from) {
  const bool list = autoCount() >= 2;
  for (int k = 1; k <= CLOCK_STYLE_COUNT; k++) {
    const int i = (from + k) % CLOCK_STYLE_COUNT;
    if (!list || (listMask() & (1ULL << i))) return i;
  }
  return from;
}
static bool autoActive() { return gSet.autoOn && autoCount() >= 2; }
static bool inList(int s) { return s >= 0 && s < CLOCK_STYLE_COUNT && (listMask() & (1ULL << s)); }
// Design zum Start bzw. nach Nachtmodus/Speichern: bei aktivem Wechsel immer eines aus der Liste
static uint8_t listStyle(int from) {
  if (!autoActive()) return gSet.clockStyle;
  return (uint8_t)(inList(from) ? from : nextStyle(from));
}
static uint8_t gPrePop = 255;
static uint32_t gPopSince = 0;          // Design vor der Feiertags-Einblendung (255 = keine Einblendung)
static void styleManual() {            // Auswahl im Browser: sofort uebernehmen, Timer neu starten
  if (gPrePop != 255) gPrePop = gSet.clockStyle;   // waehrend einer Einblendung: danach das gewaehlte Design
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
  if (ov == POP_STYLE && !wasNight) {    // Feiertags-Einblendung (20 s): Design und Wechsel-Timer nur anhalten
    if (gPrePop == 255) { gPrePop = gStyle; gPopSince = now; }
    if (gStyle != ov) { gStyle = (uint8_t)ov; needFull = true; gDbgSrc = 'P'; }
    return;
  }
  if (gPrePop != 255) {                  // Einblendung vorbei: zurueck zum vorherigen Design, Timer laeuft weiter
    gStyle = gPrePop; gPrePop = 255; needFull = true;
    gStyleSince += now - gPopSince;      // Dauer der Einblendung zaehlt nicht zur Anzeigezeit
    if (ov < 0 && !(gSet.autoOn && autoCount() >= 2)) gStyle = gSet.clockStyle;
  }
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
    const uint64_t lm = listMask();
    int k = (int)(esp_random() % (uint32_t)(autoCount() - ((lm >> gStyle) & 1 ? 1 : 0)));
    for (int i = 0; i < CLOCK_STYLE_COUNT; i++) {
      if (!(lm & (1ULL << i)) || i == gStyle) continue;
      if (k-- == 0) { next = i; break; }
    }
  } else {
    for (int k = 1; k <= CLOCK_STYLE_COUNT; k++) {
      const int i = (gStyle + k) % CLOCK_STYLE_COUNT;
      if (listMask() & (1ULL << i)) { next = i; break; }
    }
  }
  if (next >= 0 && next != gStyle) { gStyle = (uint8_t)next; needFull = true; gDbgSrc = 'A'; sndEvent(FX_STYLE); }
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
  {                                             // Kalender: Spruch zur Einblendung, sonst regelmaessig Countdown
    static uint32_t calNext = 0;
    static int calK = 0;
    const bool calOn = (gSet.calFlags & cal::CF_TICK) && timeValid() && !gDimActive;
    if (gPopSayReq || (calOn && (int32_t)(now - calNext) >= 0 && !gTickNow)) {
      char txt[200];
      bool have = false;
      if (gPopSayReq) {
        gPopSayReq = false;
        snprintf(txt, sizeof(txt), "%s: %s", cal::name(gPopId), cal::say(gPopId, calK++));
        have = true;
      } else {
        calNext = now + (uint32_t)(gSet.calTick ? gSet.calTick : 30) * 60000UL;
        have = cal::tickerText(txt, sizeof(txt), calK++);
      }
      if (have) {
        tk::Cfg c;
        c.pos = gSet.tickPos; c.big = gSet.tickBig; c.speed = gSet.tickSpeed;
        c.bg = gSet.tickBg || fc::cur < 0;
        tickRgb(gSet.tickColor, c.r, c.g, c.b);
        if (tk::start(txt, c, now, true) && fc::cur >= 0) fc::setBrightness(HW_DIM ? 100 : effectiveBright());
        return;
      }
    }
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
  boardTftBegin(gSet.rotation, gSet.panel, gSet.pflags);
  tft.fillScreen(C_BG);
}

static void enterStandby() {
  LOGF("Standby\n");
  standby = true;
  wakePending = false;
  tft.fillScreen(C_BG);
  boardTftDisplay(false);
  boardTftSleep(true);
  blOff();                       // nur mit Umbau: Backlight wirklich aus
}

static void startWake() {
  LOGF("Aufwachen\n");
  standby = false;
  needFull = true;
  boardTftSleep(false);
  wakePending = true;
  wakeAt = millis() + 130;       // ST7789/ILI9341: nach Sleep-Out ca. 120 ms warten (ohne delay())
}

// ---------------------------------------------------------------------
// Summer: Wecker und Stundensignal (Muster und Ausgabe in buzzer.h)
// ---------------------------------------------------------------------
static void alarmStart(uint32_t now) {
  gAlarmRinging = true;
  gAlarmSince = now;
  bz::alarm(gSet.alTone);
  if (standby) startWake();               // Display zum Wecken einschalten
  LOGF("Wecker klingelt\n");
}

#if SND_RICH
static bool gSndTestVol = false;          // Test im Browser mit anderer Lautstaerke: danach zuruecksetzen
// Wetter grob einteilen: 0 trocken, 1 Regen, 2 Schnee, 3 Gewitter
static int wxClass(int c) {
  if (c >= 95) return 3;
  if ((c >= 71 && c <= 77) || c == 85 || c == 86) return 2;
  if ((c >= 51 && c <= 67) || (c >= 80 && c <= 82)) return 1;
  return 0;
}
// Klang-Effekte (CYD-Lautsprecher): Ereignisse aus den Designs, Feiertag, Wetterwechsel, Silvester
static void soundFx(const struct tm* t) {
  if (gSndTestVol && !bz::busy()) { gSndTestVol = false; bz::setVolume(gSet.sndVol); }
  const bool quiet = standby || gAlarmRinging || (gSet.bzQuiet && gDimActive);
  const uint16_t ev = gSndEv;
  gSndEv = 0;
  if (!quiet) {
    if ((ev & (1u << FX_UFO)) && (gSet.sndFx & FXB_UFO)) bz::fx(FX_UFO);
    if ((ev & (1u << FX_STYLE)) && (gSet.sndFx & FXB_STYLE)) bz::fx(FX_STYLE);
  }
  if (gPopSndReq) {
    gPopSndReq = false;
    const uint8_t f = sndPopFx(gPopId);
    if (!quiet && (gSet.sndFx & FXB_POP) && f != 255) bz::fx(f);
  }
  static uint32_t wxVer = 0;
  static int wxCls = -1;
  Weather w;
  if (getWeather(w) && w.version != wxVer) {
    wxVer = w.version;
    const int c = wxClass(w.code);
    if (wxCls >= 0 && c != wxCls && c > 0 && !quiet && (gSet.sndFx & FXB_WX)) bz::fx(c == 1 ? FX_RAIN : c == 2 ? FX_SNOW : FX_STORM);
    wxCls = c;
  }
  if (t && (gSet.sndFx & FXB_NYE) && !gAlarmRinging) {     // Silvester: letzte 10 Sekunden ticken, um 0 Uhr Feuerwerk
    static int lastNy = -1;
    const bool eve = t->tm_mon == 11 && t->tm_mday == 31 && t->tm_hour == 23 && t->tm_min == 59 && t->tm_sec >= 50;
    const bool mid = t->tm_mon == 0 && t->tm_mday == 1 && t->tm_hour == 0 && t->tm_min == 0 && t->tm_sec < 3;
    const int key = eve ? t->tm_sec : (mid ? 60 : -1);
    if (key >= 0 && key != lastNy) {
      if (key == 60) bz::fx(FX_NEWYEAR, bz::PR_TEST);
      else bz::fx(FX_NYTICK, bz::PR_CHIME);
    }
    lastNy = key;
  }
}
#endif

// Notbremse: Bleibt der groesste freie Speicherblock lange zu klein (TLS und Browser-Seite brauchen ihn),
// startet die Uhr in einem ruhigen Moment neu - besser als stundenlang nicht erreichbar zu sein.
static void heapGuard(uint32_t now) {
  static uint32_t last = 0, lowSince = 0;
  if (now - last < 10000) return;
  last = now;
  rtcMinHeap = esp_get_minimum_free_heap_size();
  rtcUpMin = now / 60000UL;
  if (gAllocFail) {
    LOGF("Speicher: %lu Anforderung(en) gescheitert, groesste %lu Bytes (Task '%s', Netz-Schritt %lu, Schleife %lu, Design %u)\n",
         (unsigned long)gAllocFail, (unsigned long)gAllocFailSize, gAllocFailTask, (unsigned long)gAllocFailNet,
         (unsigned long)gAllocFailLoop, (unsigned)gStyle);
    gAllocFail = 0; gAllocFailSize = 0;
  }
  {                                                    // neuer Tiefstand unter 8 KB: einmal melden, mit Spur
    static uint32_t lowRep = 8192;
    if (rtcMinHeap < lowRep) {
      lowRep = rtcMinHeap / 2;
      LOGF("Speicher-Tiefstand %lu Bytes (gerade: Netz-Schritt %lu, Design %u, Web %lu, jetzt frei %u/%u)\n",
           (unsigned long)rtcMinHeap, (unsigned long)gCrumbNet, (unsigned)gStyle, (unsigned long)gWebReq,
           (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap());
    }
  }
  if (gSlowFrameUs) {
    LOGF("Langsames Bild: %lu ms, Design %lu, %lu Kacheln, Netz-Schritt %lu\n", (unsigned long)(gSlowFrameUs / 1000),
         (unsigned long)gSlowFrameStyle, (unsigned long)gSlowFrameTiles, (unsigned long)gSlowFrameNet);
    gSlowFrameUs = 0;
  }
  static uint8_t k = 0;
  if (++k >= 60) {                                     // alle 10 Minuten: Zustand und Messwerte ins Protokoll
    k = 0;
    // eine Zeile (rund 150 Byte): Speicher, WLAN, Wetter, Netz, Browser | Messwerte der letzten 10 Minuten
    Weather w; const bool wv = getWeather(w);
    const uint32_t fr = gMFrames;
    LOGS("Zustand: Speicher %u/%u min %u, WLAN %d %d dBm, Wetter %s, Netz-ok vor %lu s, Web %lu | %lu.%lu Bilder/s, "
         "Bild %lu us max %lu, %lu Kacheln, %lu Schleifen/s, Design %u, ISS-Aussetzer %lu\n",
         (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap(), (unsigned)esp_get_minimum_free_heap_size(),
         (int)WiFi.status(), (int)WiFi.RSSI(), wv ? "ok" : "fehlt",
         gNetOkAt ? (unsigned long)((millis() - gNetOkAt) / 1000) : 0UL, (unsigned long)gWebReq,
         (unsigned long)(fr / 600), (unsigned long)(fr % 600 * 10 / 600), fr ? (unsigned long)(gMFrameUs / fr) : 0UL,
         (unsigned long)gMFrameMax, fr ? (unsigned long)(gMTiles / fr) : 0UL, (unsigned long)(gMLoops / 600),
         (unsigned)gStyle, (unsigned long)gIssShort);
    gMFrames = gMFrameUs = gMTiles = gMLoops = gMFrameMax = 0; gIssShort = 0;
  }
  if (ESP.getMaxAllocHeap() >= 16000) { lowSince = 0; return; }
  if (!lowSince) { lowSince = now ? now : 1; LOGF("Speicher knapp: groesster Block %u\n", (unsigned)ESP.getMaxAllocHeap()); return; }
  if (now - lowSince > 10UL * 60000UL && !gAlarmRinging && !bz::busy()) {
    LOGF("Speicher seit 10 min knapp - Neustart\n");
    delay(100);
    ESP.restart();
  }
}

static void soundTick(uint32_t now) {
  bz::tick(now);
  if (gAlarmRinging && now - gAlarmSince >= ALARM_MAX_MS) alarmStop();
  static uint32_t last = 0;
  static int lastMin = -1;
  if (now - last < 500) return;
  last = now;
  if (!timeValid()) {
#if SND_RICH
    soundFx(nullptr);
#endif
    return;
  }
  time_t n = time(nullptr);
  struct tm t;
  localtime_r(&n, &t);
#if SND_RICH
  soundFx(&t);
#endif
  const int m = t.tm_hour * 60 + t.tm_min;
  if (m == lastMin) return;               // jede Minute nur einmal pruefen
  const bool first = (lastMin < 0);
  lastMin = m;
  if (first && t.tm_sec > 5) return;      // nach Start/Neustart nicht nachtraeglich ausloesen
  const int wd = (t.tm_wday + 6) % 7;     // 0 = Montag
  if (gSet.alOn && m == gSet.alTime && (gSet.alDays & (1 << wd))) { alarmStart(now); return; }
  if (gSet.bzHour && t.tm_min == 0 && !gAlarmRinging && !(gSet.bzQuiet && gDimActive))
    bz::chime(gSet.hrTone, t.tm_hour);
}

// ---------------------------------------------------------------------
// Touch (TTP223 bzw. Touchscreen beim CYD): kurz = naechstes Design (aus der Wechsel-Liste), doppelt = Laufschrift, lang = Standby
// ---------------------------------------------------------------------
#if BOARD_CYD
// Kennung dieses Programms im Klartext (RetroCYD erkennt daran die Uhr im anderen Programmbereich)
const char CYD_SELF_MARK[] = "WETTERUHR-APP-ID-1";
#include "dualboot.h"           // zweites Programm RetroCYD: Startauswahl, Halte-Menue, Touch-Kalibrierung
#endif

#if HAS_TOUCH
static void touchTick(uint32_t now) {
  static bool down = false, longDone = false, pending = false;
  static uint32_t downAt = 0, tapAt = 0, hiSince = 0, loSince = 0;
  const bool raw = boardTouchRaw();
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
      LOGR("Touch: Beruehrung erkannt (%lu)\n", (unsigned long)gTouchCount);
      if (gAlarmRinging) { alarmStop(); longDone = true; return; }   // klingelnder Wecker: nur abstellen
      if (gSet.bzTouch && !bz::busy()) bz::click();
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
  if (down && !longDone && now - downAt >= TOUCH_LONG_MS) {   // lange Beruehrung: Standby (CYD: Menue)
    longDone = true; pending = false;
#if BOARD_CYD
    if (!standby) dual::menuOpen();
#else
    if (!standby) enterStandby();
#endif
  }
  if (pending && now - tapAt > TOUCH_DOUBLE_MS) {       // einzelner Tipp: naechstes Design
    pending = false;
    if (!standby && !wakePending) {
      gDbgSrc = 'T';
      touchBlockUntil = now + TOUCH_LOCK_MS;            // Neuaufbau des Bildes erzeugt Stoerimpulse: Touch kurz sperren
      if (gPrePop != 255) {                             // Feiertags-Einblendung: Tippen beendet sie
        gPopBlockedAt = now ? now : 1;
        cal::popForceUntil = 0;
      } else if (overrideStyle() >= 0) {                // Nachtmodus/Silvester: Bild bleibt, Auswahl gilt fuer danach
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
    drawText(&FreeSansBold12pt7b, 1, TR(K_FW_UPDATE), tft.width() / 2, 80, C_WHITE, AL_CENTER, 0, 56, tft.width(), 32);
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
  ArduinoOTA.onStart([]() { gOtaActive = true; otaScreen(0, TR(K_OTA_IDE)); });
  ArduinoOTA.onProgress([](unsigned int done, unsigned int total) { if (total) otaScreen((int)((uint64_t)done * 100 / total), nullptr); });
  ArduinoOTA.onEnd([]() { otaScreen(100, TR(K_OTA_DONE)); });
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

// Namensaufloesung absichern: Ersatz-DNS (Cloudflare, Google) eintragen, und falls der vom Router fehlt
// (kommt vor, nachdem der Notfall-Access-Point lief), den Router selbst. Ohne DNS scheitern Wetter, Zeit, Radar, ISS.
static void dnsFix() {
  esp_netif_t* nif = esp_netif_get_handle_from_ifkey("WIFI_STA_DEF");
  if (!nif) return;
  esp_netif_dns_info_t d;
  memset(&d, 0, sizeof(d));
  const bool mainOk = esp_netif_get_dns_info(nif, ESP_NETIF_DNS_MAIN, &d) == ESP_OK && d.ip.u_addr.ip4.addr != 0;
  if (!mainOk) {
    memset(&d, 0, sizeof(d));
    d.ip.type = ESP_IPADDR_TYPE_V4;
    d.ip.u_addr.ip4.addr = (uint32_t)WiFi.gatewayIP();
    esp_netif_set_dns_info(nif, ESP_NETIF_DNS_MAIN, &d);
  }
  memset(&d, 0, sizeof(d)); d.ip.type = ESP_IPADDR_TYPE_V4;
  d.ip.u_addr.ip4.addr = (uint32_t)IPAddress(1, 1, 1, 1);
  esp_netif_set_dns_info(nif, ESP_NETIF_DNS_BACKUP, &d);
  memset(&d, 0, sizeof(d)); d.ip.type = ESP_IPADDR_TYPE_V4;
  d.ip.u_addr.ip4.addr = (uint32_t)IPAddress(8, 8, 8, 8);
  esp_netif_set_dns_info(nif, ESP_NETIF_DNS_FALLBACK, &d);
  LOGF("DNS: %s%s\n", WiFi.dnsIP(0).toString().c_str(), mainOk ? "" : " (fehlte, Router eingetragen)");
}

static void stopAp() {
  WiFi.softAPdisconnect(true);
  apActive = false;
  LOGF("Access-Point aus\n");
  if (WiFi.status() == WL_CONNECTED) dnsFix();
}

static void wifiBegin() {
  WiFi.persistent(false);                 // VOR dem ersten WiFi-Aufruf: WLAN-Treiber schreibt nichts ins NVS
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
      gNetOkAt = now ? now : 1;
#if ARDUINO_OTA
      if (!gArduinoOtaOn) arduinoOtaBegin();
#endif
      LOGF("WLAN ok: %s\n", WiFi.localIP().toString().c_str());
      dnsFix();
    }
    if (apActive) stopAp();               // Fallback-AP beenden, sobald das WLAN wieder da ist
    // Selbstheilung: WLAN gilt als verbunden, aber seit 20 Minuten klappt kein einziger Abruf (Wetter, Radar, ISS,
    // Zeit) - dann haengt die Verbindung (z. B. IP-Adresse verloren). Neu verbinden.
    const uint32_t okAt = gNetOkAt;
    if (okAt && (int32_t)(millis() - okAt) > (int32_t)(20UL * 60000UL) && !webBusy()) {
      LOGF("Netz: seit 20 min kein Abruf erfolgreich - WLAN neu verbinden\n");
      gNetOkAt = now ? now : 1;
      WiFi.disconnect();
      WiFi.begin(gSet.ssid, gSet.pass);
      lastWifiTry = now;
    }
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
  http.useHTTP10(true);                              // keine "chunked"-Antwort (deren Kopfzeilen werden ohne Pause gelesen)
  if (!http.begin(client, url)) { LOGF("Wetter: begin() fehlgeschlagen\n"); strlcpy(gWxErr, "begin", sizeof(gWxErr)); return false; }

  int code = http.GET();
  if (code != HTTP_CODE_OK) {
    char te[64] = "";
    const int tc = client.lastError(te, sizeof(te));     // genauer TLS-Fehler (z. B. Speicher, Zertifikat, Zeit)
    LOGF("Wetter: HTTP %d (%s), TLS %d %s, Speicher %u/%u (min %u)\n", code, http.errorToString(code).c_str(), tc, te,
         (unsigned)ESP.getFreeHeap(), (unsigned)ESP.getMaxAllocHeap(), (unsigned)esp_get_minimum_free_heap_size());
    snprintf(gWxErr, sizeof(gWxErr), "HTTP %d, TLS %d %s", code, tc, te);
    http.end();
    return false;
  }
  String body = http.getString();
  http.end();

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, body);
  if (err) { LOGF("Wetter: JSON-Fehler %s\n", err.c_str()); snprintf(gWxErr, sizeof(gWxErr), "JSON %s", err.c_str()); return false; }

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

static void radarDrop();
static void weatherStep() {
  static uint32_t lastAttempt = 0;
  static bool haveAttempt = false, lastOk = false;
  {
    uint32_t now = millis();
    if (WiFi.status() == WL_CONNECTED) {
      uint32_t interval = lastOk ? (uint32_t)gSet.updMin * 60000UL : WEATHER_RETRY_MS;
      if ((gRefreshNow || !haveAttempt || now - lastAttempt >= interval) && (!webBusy() || gRefreshNow)) {
        gRefreshNow = false;
        haveAttempt = true;
        lastAttempt = now;
        Weather nw;
        xSemaphoreTake(gNetMutex, portMAX_DELAY);
        gCrumbNet = 11;
        radarDrop();                            // offene Radar-Verbindung (rund 40 KB) vorher schliessen
        netMem(true);
        const bool wxOk = fetchWeather(nw);
        netMem(false);
        xSemaphoreGive(gNetMutex);
        if (wxOk) {
          nw.valid = true;
          nw.fetchedMs = millis();
          xSemaphoreTake(gMutex, portMAX_DELAY);
          nw.version = gWeather.version + 1;
          gWeather = nw;
          xSemaphoreGive(gMutex);
          lastOk = true;
          LOGR("Wetter aktualisiert: %.1f C\n", nw.temp);
          gWxErr[0] = 0;
          gNetOkAt = millis() | 1;
        } else {
          lastOk = false;   // alte Daten bleiben erhalten und werden als veraltet markiert
        }
      }
      if (timeValid()) {                        // Kalender: Schulferien abrufen, wenn noetig (cal.h)
        xSemaphoreTake(gNetMutex, portMAX_DELAY);
        gCrumbNet = 12;
        if (cal::netWanted()) radarDrop();
        cal::netTick(true);
        xSemaphoreGive(gNetMutex);
      }
    }
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
// Radar-Verbindung schliessen (gibt rund 40 KB frei) - nur aus dem Netz-Task aufrufen
static void radarDrop() { if (gRadarClient && gRadarClient->connected()) gRadarClient->stop(); }

// Liest einen HTTP-"chunked"-Rumpf als gewoehnlichen Stream.
// Warten auf das naechste Byte - immer mit Pause (delay) und mit Gesamtfrist.
// Wichtig: Stream::readStringUntil/readBytes der Bibliothek warten in einer Schleife OHNE Pause (bis zum
// Zeitlimit). Haengt die Verbindung, laeuft der Netz-Task dann sekundenlang durch und der Task-Watchdog
// startet die Uhr neu (gemeldet: Absturz-Spur "Flugradar (Daten lesen)", Watchdog: Kern 0 'net').
static int waitByte(Client& c, uint32_t deadline) {
  while (!c.available()) {
    if (!c.connected() || (int32_t)(millis() - deadline) > 0) return -1;
    delay(2);
  }
  return c.read();
}

// Liest die Antwort eines Servers fuer den JSON-Leser: ohne Leerlauf-Schleifen, mit Gesamtfrist.
// chunked = Antwort in Bloecken ("Transfer-Encoding: chunked"), sonst einfach bis zum Ende/Laengenangabe.
class NetStream : public Stream {
 public:
  NetStream(Client& c, bool chunked, int len, uint32_t maxMs) : c_(c), chunked_(chunked), left_(len), deadline_(millis() + maxMs) {}
  int available() override { return done_ ? 0 : 1; }
  int read() override {
    if (done_) return -1;
    if (!chunked_) {
      if (left_ == 0) { done_ = true; return -1; }
      const int b = waitByte(c_, deadline_);
      if (b < 0) { done_ = true; return -1; }
      if (left_ > 0) left_--;
      return b;
    }
    if (remain_ == 0) {
      const long n = readHexLine();
      if (n <= 0) { done_ = true; if (n == 0) skipLine(); return -1; }
      remain_ = n;
    }
    const int b = waitByte(c_, deadline_);
    if (b < 0) { done_ = true; return -1; }
    if (--remain_ == 0) skipLine();                   // CRLF hinter dem Block
    return b;
  }
  int peek() override { return -1; }                  // wird vom JSON-Leser nicht benoetigt
  size_t write(uint8_t) override { return 0; }
  // Rest lesen (damit die Verbindung wiederverwendbar bleibt) - hoechstens 2 s und 16 KB, sonst false
  bool drain() {
    const uint32_t d = millis() + 2000;
    if ((int32_t)(deadline_ - d) > 0) deadline_ = d;
    for (int i = 0; i < 16384 && !done_; i++) if (read() < 0) break;
    return done_ && !timedOut();
  }
  bool timedOut() const { return (int32_t)(millis() - deadline_) > 0; }
 private:
  long readHexLine() {                                // Blockgroesse (hex) bis zum Zeilenende
    long v = 0; int digits = 0;
    for (int i = 0; i < 40; i++) {
      const int ch = waitByte(c_, deadline_);
      if (ch < 0) return -1;
      if (ch == '\n') return digits ? v : -1;
      if (ch == ';') { skipLine(); return digits ? v : -1; }   // Erweiterungen hinter der Groesse
      const int h = (ch >= '0' && ch <= '9') ? ch - '0' : (ch >= 'a' && ch <= 'f') ? ch - 'a' + 10 : (ch >= 'A' && ch <= 'F') ? ch - 'A' + 10 : -1;
      if (h >= 0) { v = v * 16 + h; digits++; }
    }
    return -1;
  }
  void skipLine() { for (int i = 0; i < 200; i++) { const int ch = waitByte(c_, deadline_); if (ch < 0 || ch == '\n') return; } }
  Client& c_;
  bool chunked_;
  int left_;                                          // Restlaenge bei bekannter Groesse, -1 = bis zum Ende
  long remain_ = 0;
  uint32_t deadline_;
  bool done_ = false;
};

// Bis hinter "[" der Flugzeugliste lesen: Schluessel "ac" (adsb.lol, airplanes.live) oder "aircraft" (adsb.fi).
static bool findPlaneList(Stream& s) {
  static const char* const K1 = "\"ac\"";
  static const char* const K2 = "\"aircraft\"";
  int m1 = 0, m2 = 0;
  bool key = false;
  for (int n = 0; n < 4096; n++) {                   // die Liste steht weit vorn in der Antwort
    const int ch = s.read();
    if (ch < 0) return false;
    if (key) {                                       // nach dem Schluessel: ":" und "[" (Leerzeichen erlaubt)
      if (ch == '[') return true;
      if (ch == ':' || ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t') continue;
      key = false;                                   // doch kein Listenanfang: weitersuchen
    }
    m1 = (ch == K1[m1]) ? m1 + 1 : (ch == K1[0] ? 1 : 0);
    m2 = (ch == K2[m2]) ? m2 + 1 : (ch == K2[0] ? 1 : 0);
    if (!K1[m1] || !K2[m2]) { key = true; m1 = m2 = 0; }
  }
  return false;
}

// Leser mit einem Zeichen "Ruecklage": damit laesst sich vor jedem Flugzeug pruefen, ob die Liste zu Ende ist.
struct BackReader {
  Stream& s;
  int pb = -1;
  int read() { if (pb >= 0) { const int c = pb; pb = -1; return c; } return s.read(); }
  size_t readBytes(char* b, size_t n) { size_t k = 0; while (k < n) { const int c = read(); if (c < 0) break; b[k++] = (char)c; } return k; }
  int nextNonSpace() { int c; do { c = read(); } while (c == ' ' || c == '\n' || c == '\r' || c == '\t'); return c; }
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
  if (!gRadarClient->connected() && ESP.getMaxAllocHeap() < 36000) {   // neuer TLS-Aufbau braucht einen grossen Block
    LOGR("Radar: zu wenig Speicher fuer die Verbindung (%u)\n", (unsigned)ESP.getMaxAllocHeap());
    return false;
  }
  if (!http.begin(*gRadarClient, url)) return false;
  gCrumbNet = 24;                                     // Verbindung/TLS/Anfrage
  const int code = http.GET();
  gCrumbNet = 25;                                     // Antwort lesen und auswerten
  if (code != HTTP_CODE_OK) { LOGF("Radar: HTTP %d\n", code); http.end(); gRadarClient->stop(); return false; }

  // Antwort Flugzeug fuer Flugzeug lesen: jedes Flugzeug kommt in ein kleines eigenes JSON-Dokument, das danach
  // wieder frei wird. Vorher stand die ganze Liste (bei vollem Himmel 100+ Flugzeuge) gleichzeitig im Speicher -
  // zusammen mit der TLS-Verbindung fiel der freie Speicher dabei bis auf rund 2 KB (Protokoll vom Geraet).
  JsonDocument filter;                               // nur die noetigen Felder behalten (spart RAM)
  static const char* const KEYS[] = {"flight", "hex", "lat", "lon", "alt_baro", "alt_geom", "gs", "track", "t",
                                     "baro_rate", "geom_rate", "seen_pos"};
  for (const char* k : KEYS) filter[k] = true;
  const int size = http.getSize();                   // < 0: "chunked" (Bloecke selbst entpacken)
  NetStream ns(*gRadarClient, size < 0, size, 10000);   // hoechstens 10 s fuer die ganze Antwort
  ns.setTimeout(0);                                  // Stream-Hilfen nicht zusaetzlich warten lassen (NetStream wartet selbst)

  const double lat0 = gSet.lat, lon0 = gSet.lon;
  const double kx = 111.32 * cos(lat0 * 0.0174533);
  const float maxD2 = km * km * 1.1f;
  auto take = [&](JsonObject a) {
    if (!a["lat"].is<double>() || !a["lon"].is<double>()) return;
    Plane p;
    memset(&p, 0, sizeof(p));
    p.x = (float)((a["lon"].as<double>() - lon0) * kx);
    p.y = (float)((a["lat"].as<double>() - lat0) * 111.0);
    if (p.x * p.x + p.y * p.y > maxD2) return;
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
  };

  out.n = 0;
  int count = 0;
  bool listOk = false;
  if (findPlaneList(ns)) {
    BackReader br{ns};
    JsonDocument item;                               // ein Flugzeug (wird fuer jedes neu gefuellt)
    int ch = br.nextNonSpace();
    if (ch == ']') listOk = true;                    // leere Liste (Himmel frei)
    else br.pb = ch;
    while (!listOk && ch >= 0) {
      const DeserializationError e = deserializeJson(item, br, DeserializationOption::Filter(filter));
      if (e) { LOGF("Radar: JSON %s\n", e.c_str()); break; }
      count++;
      if (item.is<JsonObject>()) take(item.as<JsonObject>());
      ch = br.nextNonSpace();
      if (ch == ']') listOk = true;
      else if (ch != ',') break;
    }
  } else LOGF("Radar: keine Flugzeugliste in der Antwort\n");
  const bool clean = ns.drain();                     // Rest lesen, damit die Verbindung wiederverwendbar bleibt
  if (ns.timedOut()) LOGF("Radar: Antwort zu langsam - abgebrochen\n");
  http.end();                                        // Verbindung bleibt wegen setReuse(true) offen
  if (!clean) gRadarClient->stop();                  // Rest nicht sauber gelesen: Verbindung nicht wiederverwenden
  // Wenig freier Speicher: TLS-Verbindung schliessen (gibt rund 40 KB frei), damit die Browser-Seite
  // weiter erreichbar bleibt. Der naechste Abruf baut sie neu auf.
  if (!listOk || ESP.getFreeHeap() < 60000 || ESP.getMaxAllocHeap() < 24000) gRadarClient->stop();
  gCrumbNet = 26;
  return listOk;
}

static void radarStep() {
  static PlaneList nl;                                // statisch: schont den Task-Stack
  static uint32_t lastTry = 0;
  static bool have = false;
  {
    const uint32_t now = millis();
    const uint32_t wa = gRadarWantedAt;
    const bool wanted = wa != 0 && now - wa < 4000;
    static bool wasWanted = false;
    if (wanted && !wasWanted) have = false;           // gerade eingeschaltet: sofort abrufen
    wasWanted = wanted;
    if (webBusy()) {                                  // Browser aktiv: nicht abrufen, TLS-Speicher freigeben
      if (gRadarClient && gRadarClient->connected()) gRadarClient->stop();
    } else if (wanted && WiFi.status() == WL_CONNECTED && (!have || now - lastTry >= RADAR_POLL_MS)) {
      lastTry = now; have = true;
      bool ok = false;
      xSemaphoreTake(gNetMutex, portMAX_DELAY);
      for (int k = 0; k < 3 && !ok; k++) {
        const uint8_t h = (gRadarHost + k) % 3;
        gCrumbNet = 21 + h;
        ok = fetchPlanesFrom(RADAR_URL[h], nl);
        if (ok) gRadarHost = h;
      }
      xSemaphoreGive(gNetMutex);
      xSemaphoreTake(gMutex, portMAX_DELAY);
      if (ok) { nl.status = 1; nl.radiusKm = (float)gSet.radarKm; nl.fetchedMs = millis(); gPlanes = nl; }
      else    { gPlanes.status = 2; }                  // alte Flugzeuge bleiben sichtbar (werden als veraltet markiert)
      gPlanesVer++;
      xSemaphoreGive(gMutex);
      if (ok) gNetOkAt = millis() | 1;
      else LOGF("Radar: Fehler\n");
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

static int gIssCode = 0;                          // letzter HTTP-Code des ISS-Abrufs (Protokoll)
static bool fetchIss(IssData& d) {
  static uint32_t prevTs = 0;
  WiFiClient client;
  HTTPClient http;
  http.setConnectTimeout(5000);
  http.setTimeout(5000);
  gIssCode = 0;
  http.useHTTP10(true);                              // ohne "chunked": Antwort kann direkt gelesen werden
  if (!http.begin(client, "http://api.open-notify.org/iss-now.json")) return false;
  gIssCode = http.GET();
  if (gIssCode != HTTP_CODE_OK) { http.end(); return false; }
  JsonDocument doc;
  NetStream ns(client, false, http.getSize(), 6000);   // mit Pausen lesen (kein Leerlauf fuer den Watchdog)
  const DeserializationError err = deserializeJson(doc, ns);
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
  http.useHTTP10(true);
  if (!http.begin(client, "http://api.open-notify.org/astros.json")) return 0;
  if (http.GET() != HTTP_CODE_OK) { http.end(); return 0; }
  JsonDocument filter;
  filter["people"][0]["craft"] = true;
  JsonDocument doc;
  NetStream ns(client, false, http.getSize(), 6000);
  const DeserializationError err = deserializeJson(doc, ns, DeserializationOption::Filter(filter));
  http.end();
  if (err) return 0;
  int n = 0;
  for (JsonObject p : doc["people"].as<JsonArray>()) if (strcmp(p["craft"] | "", "ISS") == 0) n++;
  return (uint8_t)n;
}

static void issStep() {
  static IssData nd;
  static uint32_t lastTry = 0, lastCrew = 0, lastOkMs = 0;
  static bool have = false, wasWanted = false;
  static uint16_t fails = 0;                          // Fehlversuche in Folge (Dienst open-notify ist oft kurz weg)
  static uint32_t failSince = 0;
  static bool longLogged = false;
  {
    const uint32_t now = millis();
    const uint32_t wa = gIssWantedAt;
    const bool wanted = wa != 0 && now - wa < 4000;
    if (wanted && !wasWanted) {                       // Design wieder sichtbar: neue Zaehlung (die Zeit dazwischen,
      have = false;                                   // in der gar nicht gefragt wurde, ist kein Ausfall des Dienstes)
      fails = 0; longLogged = false;
    }
    wasWanted = wanted;
    // nach Fehlern seltener fragen: 5 s, 10 s, 20 s, 40 s, dann jede Minute
    const uint32_t every = fails ? min(60000UL, ISS_POLL_MS << min<uint16_t>(fails, 4)) : ISS_POLL_MS;
    if (wanted && !webBusy() && WiFi.status() == WL_CONNECTED && (!have || now - lastTry >= every)) {
      lastTry = now; have = true;
      xSemaphoreTake(gMutex, portMAX_DELAY); nd = gIss; xSemaphoreGive(gMutex);
      gCrumbNet = 31;
      const bool ok = fetchIss(nd);
      gCrumbNet = 32;
      if (ok && (lastCrew == 0 || now - lastCrew > 1800000UL)) { lastCrew = now ? now : 1; const uint8_t c = fetchCrew(); if (c) nd.crew = c; }
      xSemaphoreTake(gMutex, portMAX_DELAY);
      if (ok) { nd.status = 1; nd.fetchedMs = millis(); gIss = nd; }
      else if (!lastOkMs || millis() - lastOkMs > 120000UL) { gIss.status = 2; }   // kurze Aussetzer: Bahn weiterrechnen
      gIssVer++;
      xSemaphoreGive(gMutex);
      if (ok) {
        gNetOkAt = millis() | 1;
        lastOkMs = millis() | 1;
        if (fails) {
          const uint32_t d = (millis() - failSince) / 1000;
          if (d >= 60) LOGF("ISS: Dienst %lu s nicht erreichbar (%u Fehlversuche), wieder ok\n", (unsigned long)d, (unsigned)fails);
          else { gIssShort = gIssShort + 1; LOGR("ISS: kurzer Aussetzer (%u Fehlversuche)\n", (unsigned)fails); }
        }
        fails = 0; longLogged = false;
      } else {
        if (!fails) { failSince = millis(); LOGR("ISS: Fehler HTTP %d\n", gIssCode); }
        if (!longLogged && millis() - failSince > 600000UL) {
          longLogged = true;
          LOGF("ISS: seit 10 min keine Daten (HTTP %d %s)\n", gIssCode, gIssCode < 0 ? HTTPClient::errorToString(gIssCode).c_str() : "");
        }
        if (fails < 60000) fails++;
      }
    }
  }
}

// Ein gemeinsamer Netz-Task fuer Wetter, Kalender, Flugradar und ISS (vorher drei Tasks mit zusammen 30 KB Stack;
// die Abrufe liefen wegen gNetMutex ohnehin nacheinander). Spart rund 18 KB Arbeitsspeicher.
static void netTask(void*) {
  for (;;) {
    gCrumbNet = 1; weatherStep();
    gCrumbNet = 2; radarStep();
    gCrumbNet = 3; issStep();
    gCrumbNet = 0;
    vTaskDelay(pdMS_TO_TICKS(250));
  }
}

// =====================================================================
// Browser-Einstellungsseite (braucht die Definitionen oben)
// =====================================================================
#include "led.h"            // RGB-LED des CYD
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
    snprintf(left, sizeof(left), "%s", TR(K_NO_WIFI));
    lcol = C_RED;
  }
  tft.setTextColor(lcol);
  tft.setCursor(30, 19);
  tft.print(left);

  const char* rt = nullptr;
  uint16_t rcol = C_ORANGE;
  if (!hv)        { rt = TR(K_NO_WX); rcol = C_RED; }
  else if (stale) { rt = TR(K_WX_OLD); }
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
  if (ok) { char d[16]; langDateY(d, sizeof(d), t.tm_mday, t.tm_mon + 1, t.tm_year + 1900); snprintf(b, sizeof(b), "%s, %s", WEEKDAY[t.tm_wday], d); }
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

// Design 27: Grossanzeige - Uhrzeit ueber die ganze Breite (aus der Ferne lesbar), darunter Temperatur und Datum.
// Ziffern als 7-Segment mit runden Enden, in voller Aufloesung; nicht leuchtende Segmente ganz schwach sichtbar.
static const uint8_t SEG7[11] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F, 0x40};

static void bigDigit(int x, int y, int w, int h, int d, uint16_t on, uint16_t off) {
  const uint8_t m = SEG7[(d < 0 || d > 10) ? 10 : d];
  const int t = w / 5, g = 2;
  const int hx = x + t / 2 + g, hw = w - t - 2 * g, vh = h / 2 - t / 2 - 2 * g, r = t / 2;
  tft.fillRoundRect(hx, y, hw, t, r, (m & 0x01) ? on : off);                       // a oben
  tft.fillRoundRect(x + w - t, y + t / 2 + g, t, vh, r, (m & 0x02) ? on : off);     // b rechts oben
  tft.fillRoundRect(x + w - t, y + h / 2 + g, t, vh, r, (m & 0x04) ? on : off);     // c rechts unten
  tft.fillRoundRect(hx, y + h - t, hw, t, r, (m & 0x08) ? on : off);               // d unten
  tft.fillRoundRect(x, y + h / 2 + g, t, vh, r, (m & 0x10) ? on : off);             // e links unten
  tft.fillRoundRect(x, y + t / 2 + g, t, vh, r, (m & 0x20) ? on : off);             // f links oben
  tft.fillRoundRect(hx, y + (h - t) / 2, hw, t, r, (m & 0x40) ? on : off);         // g Mitte
}

static void updateBig(bool ok, const struct tm& t, const Weather& w, bool hv, bool stale) {
  const int W = tft.width();
  const int X[4] = {6, 80, 176, 250}, DW = 64, DH = 134, Y0 = 6;
  const uint16_t on = C_ACCENT, off = scaleColor(C_ACCENT, 12);
  char b[48];
  // Ziffern: nur geaenderte neu zeichnen
  char d4[5] = {(char)(ok ? t.tm_hour / 10 : 10), (char)(ok ? t.tm_hour % 10 : 10),
                (char)(ok ? t.tm_min / 10 : 10), (char)(ok ? t.tm_min % 10 : 10), 0};
  for (int i = 0; i < 4; i++) {
    if (cacheS[K_BIG][0] == 'v' && cacheS[K_BIG][i + 1] == d4[i] + 1) continue;   // [0] = gueltig (resetCaches loescht es)
    tft.fillRect(X[i], Y0, DW, DH, C_BG);
    bigDigit(X[i], Y0, DW, DH, d4[i], on, off);
    cacheS[K_BIG][i + 1] = (char)(d4[i] + 1);
  }
  cacheS[K_BIG][0] = 'v'; cacheS[K_BIG][5] = 0;
  // Doppelpunkt (blinkt mit Sekunden)
  const bool colOn = !ok || !gSet.seconds || (t.tm_sec % 2 == 0);
  snprintf(b, sizeof(b), "%d", colOn);
  if (upd(K_BCOL, b)) {
    const uint16_t c = colOn ? on : off;
    tft.fillCircle(W / 2 + 1, Y0 + DH * 30 / 100, 8, c);
    tft.fillCircle(W / 2 + 1, Y0 + DH * 70 / 100, 8, c);
  }
  // Sekundenbalken
  const int sec = (ok && gSet.seconds) ? t.tm_sec : -1;
  snprintf(b, sizeof(b), "%d", sec);
  if (upd(K_BAR, b)) {
    tft.fillRoundRect(6, Y0 + DH + 6, W - 12, 5, 2, sec >= 0 ? C_DIM : C_BG);
    if (sec >= 0) tft.fillRoundRect(6, Y0 + DH + 6, max(5, (sec + 1) * (W - 12) / 60), 5, 2, on);
  }
  // rechts unten: Datum gross, darunter der Wochentag
  if (ok) { char d[12]; langDate(d, sizeof(d), t.tm_mday, t.tm_mon + 1); snprintf(b, sizeof(b), "%s|%s", d, WEEKDAY[t.tm_wday]); }
  else snprintf(b, sizeof(b), "--|");
  if (upd(K_DATE, b)) {
    char* bar = strchr(b, '|');
    *bar = 0;
    drawText(&FreeSansBold18pt7b, 1, b, W - 6, 190, C_WHITE, AL_RIGHT, 200, 158, W - 200, 40);
    drawText(&FreeSans12pt7b, 1, bar + 1, W - 6, 226, C_ACCENT, AL_RIGHT, 196, 204, W - 196, 30);
  }
  // links unten: Wettersymbol und grosse Temperatur
  const bool wf = (WiFi.status() == WL_CONNECTED);
  if (hv) snprintf(b, sizeof(b), "%d|%d|%d|%d|%d", (int)lroundf(w.temp), w.code, w.isDay, stale, wf);
  else    snprintf(b, sizeof(b), "none|%d", wf);
  if (upd(K_WX, b)) {
    tft.fillRect(0, 156, 196, 84, C_BG);
    if (hv) {
      drawIcon(tft, 32, 202, 24, iconForCode(w.code, w.isDay));
      drawTemp(64, 222, (int)lroundf(w.temp), &FreeSansBold24pt7b, 1, stale ? C_ORANGE : C_WHITE, AL_LEFT, 60, 162, 136, 70);
    } else {
      drawText(&FreeSans12pt7b, 1, TR(K_NO_WX), 8, 212, C_ORANGE, AL_LEFT, 0, 190, 196, 30);
    }
    if (!wf) tft.fillCircle(W - 5, 236, 3, C_RED);           // kein WLAN: roter Punkt
  }
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
    drawText(&FreeSans12pt7b, 1, TR(K_NO_WEATHER), W / 2, 180, C_ORANGE, AL_CENTER, 0, 160, W, 28);
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
  drawText(&FreeSans9pt7b, 1, TR(K_FEELS), lx, 162, C_GREY, AL_LEFT, lx - 2, 148, 56, 18);
  drawTemp(rx, 162, (int)lroundf(w.feels), &FreeSansBold9pt7b, 1, tcol, AL_RIGHT, rx - 52, 148, 54, 18);
  drawText(&FreeSans9pt7b, 1, TR(K_HUMID), lx, 181, C_GREY, AL_LEFT, lx - 2, 167, 56, 18);
  snprintf(b, sizeof(b), "%d %%", w.hum);
  drawText(&FreeSansBold9pt7b, 1, b, rx, 181, tcol, AL_RIGHT, rx - 52, 167, 54, 18);
  drawText(&FreeSans9pt7b, 1, TR(K_WIND), lx, 200, C_GREY, AL_LEFT, lx - 2, 186, 56, 18);
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

// ---------------------------------------------------------------------
// Kalender-Design (CAL_STYLE): Uhrzeit und Datum oben, links das Monatsblatt mit Feiertagen (rot),
// besonderen Tagen (oranger Punkt) und Schulferien (blau hinterlegt), rechts heute / naechster Feiertag /
// Schulferien, rechts unten das Wetter. Neu gezeichnet wird nur, was sich geaendert hat.
// ---------------------------------------------------------------------
static void calLabel(const char* s, int x, int y, uint16_t col) {   // kleine Pixelschrift, y = oben
  tft.setTextWrap(false);
  tft.setFont(nullptr); tft.setTextSize(1); tft.setTextColor(col); tft.setCursor(x, y); tft.print(s);
}
// Text mit FreeSans9 in Zeilen umbrechen (hoechstens maxL Zeilen), Rueckgabe: naechste Grundlinie
static int calWrap(const char* utf, int x, int base, int w, uint16_t col, const GFXfont* f, int maxL) {
  char s[64];
  cal::ascii(s, utf, sizeof(s));
  tft.setTextWrap(false);
  // einzelnes Wort zu breit: kleinere Schrift, sonst kuerzen
  for (char* q = s; *q;) {
    char* e = strchr(q, ' ');
    char wd[40];
    strlcpy(wd, q, min(sizeof(wd), (size_t)(e ? e - q : strlen(q)) + 1));
    if (textW(f, wd) > w) { f = &FreeSans9pt7b; if (textW(f, wd) > w) {
      size_t L = strlen(wd);
      while (L > 2 && textW(f, wd) > w) { wd[--L] = 0; wd[L - 1] = '.'; }
      const size_t off = q - s;
      const size_t rest = e ? strlen(e) : 0;
      memmove(s + off + L, e ? e : s + off + strlen(q), rest + 1);
      memcpy(s + off, wd, L);
      e = rest ? s + off + L : nullptr;
    } }
    q = e ? e + 1 : q + strlen(q);
  }
  tft.setFont(f); tft.setTextSize(1); tft.setTextColor(col);
  char line[64] = "";
  int lines = 0;
  char* p = s;
  while (*p && lines < maxL) {
    char* sp = strchr(p, ' ');
    char word[40];
    const size_t wl = sp ? (size_t)(sp - p) : strlen(p);
    strlcpy(word, p, min(sizeof(word), wl + 1));
    char test[64];
    snprintf(test, sizeof(test), "%s%s%s", line, line[0] ? " " : "", word);
    if (line[0] && textW(f, test) > w) {
      tft.setCursor(x, base); tft.print(line);
      base += 17; lines++;
      strlcpy(line, word, sizeof(line));
    } else strlcpy(line, test, sizeof(line));
    p = sp ? sp + 1 : p + wl;
  }
  if (line[0] && lines < maxL) { tft.setCursor(x, base); tft.print(line); base += 17; }
  tft.setFont(nullptr);
  return base;
}

static void calGrid(int y, int m, int dToday, uint8_t pct) {
  const int X0 = 4, CW = 27, CH = 24, YT = 46, YH = 66, YG = 78;
  tft.fillRect(0, YT, 194, 240 - YT, C_BG);
  char b[32];
  snprintf(b, sizeof(b), "%s %d", CAL_MON[LANG_IDX][m - 1], y);
  drawText(&FreeSansBold9pt7b, 1, b, X0 + 2, YT + 15, C_WHITE, AL_LEFT, X0, YT, 190, 18);
  for (int c = 0; c < 7; c++) {
    const int wd = (c + 1) % 7;
    calLabel(WDK2_L[LANG_IDX][wd], X0 + c * CW + 8, YH, wd == 0 ? scaleColor(B_RED, pct) : C_GREY);
  }
  // Feiertage dieses Monats
  cal::Day days[48];
  const int n = cal::build(y, days, 48, gSet.calFlags);
  const int32_t first = cal::dnum(y, m, 1);
  const int dim = cal::dnum(m == 12 ? y + 1 : y, m == 12 ? 1 : m + 1, 1) - first;
  const int off = (cal::wday(first) + 6) % 7;                      // Montag = 0
  const uint16_t cFer = scaleColor(RGB565C(25, 60, 140), pct), cRed = scaleColor(RGB565C(255, 80, 80), pct);
  const uint16_t cOr = scaleColor(RGB565C(255, 160, 40), pct), cSun = scaleColor(RGB565C(200, 90, 90), pct);
  for (int d = 1; d <= dim; d++) {
    const int k = off + d - 1, cx = X0 + (k % 7) * CW, cy = YG + (k / 7) * CH;
    const int32_t dn = first + d - 1;
    bool pub = false, spec = false;
    for (int i = 0; i < n; i++) if (days[i].dn == dn) { if (cal::isPublic(days[i].id)) pub = true; else spec = true; }
    const bool fer = cal::ferAt(dn) >= 0;
    const int wd = cal::wday(dn);
    if (fer) tft.fillRect(cx + 1, cy + 1, CW - 2, CH - 2, cFer);
    uint16_t tc = C_WHITE;
    if (wd == 6) tc = C_GREY;
    if (wd == 0) tc = cSun;
    if (pub) tc = cRed;
    if (d == dToday) { tft.fillRoundRect(cx + 1, cy + 1, CW - 2, CH - 2, 5, C_ACCENT); tc = C_BG; }
    snprintf(b, sizeof(b), "%d", d);
    tft.setFont(&FreeSansBold9pt7b); tft.setTextColor(tc);
    tft.setCursor(cx + (CW - textW(&FreeSansBold9pt7b, b)) / 2, cy + 17);
    tft.print(b);
    if (spec) tft.fillCircle(cx + CW / 2, cy + CH - 3, 2, d == dToday ? C_BG : cOr);
    if (pub && d != dToday) tft.drawFastHLine(cx + 5, cy + CH - 2, CW - 10, cRed);
  }
  tft.setFont(nullptr);
}

static void calPanel(int32_t dn, uint8_t pct) {
  const int X = 200, W = 116;
  tft.fillRect(196, 46, 124, 146, C_BG);
  tft.drawFastVLine(196, 50, 186, C_LINE);
  const int L = LANG_IDX;
  char b[48];
  int y = 50;
  const cal::Today& td = cal::today(dn);
  int shown = 0;
  for (int i = 0; i < td.n && shown < 2; i++) {
    if (!shown) { calLabel(CAL_L_TODAY[L], X, y, C_GREY); y += 22; }
    y = calWrap(cal::name(td.ids[i]), X, y, W, C_ACCENT, &FreeSansBold9pt7b, 2) - 2;
    shown++;
  }
  if (!shown) {
    cal::Day nx;
    calLabel(CAL_L_NEXT[L], X, y, C_GREY); y += 22;
    if (cal::next(dn, nx, false)) {
      y = calWrap(cal::name(nx.id), X, y, W, cal::isPublic(nx.id) ? scaleColor(RGB565C(255, 90, 90), pct) : C_WHITE, &FreeSansBold9pt7b, 2);
      int yy, mm, d;
      cal::civil(nx.dn, yy, mm, d);
      const int left = nx.dn - dn;
      if (left == 1) strlcpy(b, CAL_L_TOM[L], sizeof(b)); else snprintf(b, sizeof(b), CAL_L_IN[L], left);
      calLabel(b, X, y - 12, C_GREY);
      snprintf(b, sizeof(b), "%s %d.%d.", WDK2_L[L][cal::wday(nx.dn)], d, mm);
      calLabel(b, X, y - 2, C_GREY);
      y += 10;
    }
  }
  // Schulferien
  y = max(y + 4, 124);
  calLabel(CAL_L_FER[L], X, y, C_GREY);
  y += 22;
  const uint16_t cFerT = scaleColor(RGB565C(110, 170, 255), pct);
  if (!(gSet.calFlags & cal::CF_SCHOOL)) {
  } else if (!cal::calBl()) {
    calWrap(CAL_L_NOBL[L], X, y, W, C_GREY, &FreeSans9pt7b, 3);
  } else if (!cal::ferOk()) {
    calWrap(CAL_L_NOFER[L], X, y, W, C_GREY, &FreeSans9pt7b, 1);
  } else {
    const int now = cal::ferAt(dn);
    const int f = now >= 0 ? now : cal::ferNext(dn);
    if (f >= 0) {
      const uint8_t ft = cal::fer[f].type;
      y = calWrap(ft ? FER_SHORT[ft][L] : cal::fer[f].name, X, y, W, cFerT, &FreeSansBold9pt7b, 1);
      if (now >= 0) {
        const int left = cal::fer[f].b - dn;
        if (left == 0) strlcpy(b, CAL_L_LAST[L], sizeof(b)); else snprintf(b, sizeof(b), CAL_L_LEFT[L], left);
      } else {
        const int left = cal::fer[f].a - dn;
        if (left == 1) strlcpy(b, CAL_L_TOM[L], sizeof(b)); else snprintf(b, sizeof(b), CAL_L_IN[L], left);
      }
      calLabel(b, X, y - 12, C_GREY);
    }
  }
}

static void updateCal(bool ok, const struct tm& t, const Weather& w, bool hv, bool stale) {
  const uint8_t pct = HW_DIM ? 100 : effectiveBright();
  char b[64];
  if (ok) snprintf(b, sizeof(b), "%02d:%02d", t.tm_hour, t.tm_min); else strlcpy(b, "--:--", sizeof(b));
  if (upd(K_H1, b)) drawText(&FreeSansBold24pt7b, 1, b, 6, 38, C_ACCENT, AL_LEFT, 0, 0, 150, 44);
  if (ok) snprintf(b, sizeof(b), "%s|%d%s %s %d", WEEKDAY[t.tm_wday], t.tm_mday, LANG_IDX == 0 ? "." : "", CAL_MON[LANG_IDX][t.tm_mon], t.tm_year + 1900);
  else strlcpy(b, "|", sizeof(b));
  if (upd(K_DATE, b)) {
    char* bar = strchr(b, '|');
    *bar = 0;
    tft.fillRect(150, 0, 170, 44, C_BG);
    drawText(&FreeSans9pt7b, 1, b, 314, 17, C_GREY, AL_RIGHT, 150, 2, 170, 18);
    drawText(&FreeSansBold9pt7b, 1, bar + 1, 314, 38, C_WHITE, AL_RIGHT, 150, 22, 170, 20);
    tft.drawFastHLine(4, 44, 312, C_LINE);
  }
  const int32_t dn = ok ? cal::dnum(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday) : -1;
  snprintf(b, sizeof(b), "%ld|%u|%u|%u|%u|%u", (long)dn, cal::ferVer, gSet.calFlags, gSet.calBl, gLang, cal::ferN);
  if (upd(K_STAT, b)) {
    if (ok) { calGrid(t.tm_year + 1900, t.tm_mon + 1, t.tm_mday, pct); calPanel(dn, pct); }
    else { tft.fillRect(0, 46, 320, 194, C_BG); }
    cacheS[K_WX][0] = 1;                                   // Wetter danach neu zeichnen (Bereich wurde geloescht)
  }
  const bool wf = (WiFi.status() == WL_CONNECTED);
  if (hv) snprintf(b, sizeof(b), "c%d|%d|%d|%d|%d", (int)lroundf(w.temp), w.code, w.isDay, stale, wf);
  else    snprintf(b, sizeof(b), "cnone|%d", wf);
  if (upd(K_WX, b)) {
    tft.fillRect(198, 194, 122, 46, C_BG);
    if (hv) {
      drawIcon(tft, 216, 217, 14, iconForCode(w.code, w.isDay));
      drawTemp(238, 228, (int)lroundf(w.temp), &FreeSansBold18pt7b, 1, stale ? C_ORANGE : C_WHITE, AL_LEFT, 234, 196, 84, 40);
    }
    if (!wf) tft.fillCircle(315, 236, 3, C_RED);
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
  const bool pop   = want == POP_STYLE;                // Feiertags-Einblendung
  const bool wantT = pop || fc::isThemed(want);
  const int  wIdx  = pop ? PARTY_FACE : want - FACE_FIRST_STYLE;
  if (pop) f_party::id = gPopId;
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
      if (pop) { gPopBlockedAt = now ? now : 1; gStyle = gSet.clockStyle; needFull = true; return false; }
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

  hiPolicy();                                  // Kantenglaettung / feine Schrift (Browser: Anzeige > Darstellung)
  const uint32_t t0 = micros();
  const bool drew = fc::frame(tft, c);
  if (drew) {
    const uint32_t dt = micros() - t0;
    if (dt > gDiagFrameMax) gDiagFrameMax = dt;
    gMFrames++; gMFrameUs += dt; gMTiles += (uint32_t)fc::lastTiles;
    if (dt > gMFrameMax) gMFrameMax = dt;
    if (dt > 300000UL && dt > gSlowFrameUs) {          // ueber 0,3 s: fuer das Protokoll merken (heapGuard schreibt)
      gSlowFrameUs = dt; gSlowFrameTiles = (uint32_t)fc::lastTiles; gSlowFrameStyle = gStyle; gSlowFrameNet = gCrumbNet;
    }
  }
  if (showAfterDraw) {
    boardTftDisplay(true);
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
    if (gStyle != BIG_STYLE && gStyle != CAL_STYLE) drawStatic();
  }

  const bool ok = timeValid();
  Weather w;
  const bool hv = getWeather(w);
  const bool stale = hv && (millis() - w.fetchedMs > staleMs());

  if (gStyle == CAL_STYLE) {                     // Kalender: Monatsblatt mit Feiertagen und Ferien
    time_t n = time(nullptr);
    struct tm t;
    localtime_r(&n, &t);
    updateCal(ok, t, w, hv, stale);
    tickerClassic(now);
    if (showAfterDraw) { boardTftDisplay(true); blApply(); showAfterDraw = false; }
    return;
  }
  if (gStyle == BIG_STYLE) {                     // Grossanzeige: eigenes Layout ueber den ganzen Bildschirm
    time_t n = time(nullptr);
    struct tm t;
    localtime_r(&n, &t);
    updateBig(ok, t, w, hv, stale);
    tickerClassic(now);
    if (showAfterDraw) { boardTftDisplay(true); blApply(); showAfterDraw = false; }
    return;
  }

  updateStatus(ok, w, hv, stale);
  updateClock(ok);
  updateWeatherBlock(w, hv, stale);
  tickerClassic(now);

  if (showAfterDraw) {
    boardTftDisplay(true);
    blApply();
    showAfterDraw = false;
  }
}

// =====================================================================
// Setup / Loop
// =====================================================================
void setup() {
#if PIN_LED_R >= 0
  led::begin();             // RGB-LED (PWM, led.h)
#else
  boardLedInit();
#endif
  boardTouchInit();
#if DEBUG_SERIAL
  Serial.begin(115200);
#if defined(ARDUINO_USB_CDC_ON_BOOT) && ARDUINO_USB_CDC_ON_BOOT
  Serial.setTxTimeoutMs(0);      // USB-CDC ohne Leser darf die Schleife nie blockieren
#endif
#endif
  bz::init();                    // Summer-Pin sofort auf "aus"
  crumbsBoot();
  rlogBoot();
  heap_caps_register_failed_alloc_callback(allocFailHook);
  const bool plogOk = plog::begin();             // dauerhaftes Protokoll im Flash (rund 6 Tage)
  {
    static const char* const RR[] = {"?", "Einschalten", "Reset-Taste", "Software", "Absturz (PANIC)", "Interrupt-Watchdog",
                                     "Task-Watchdog", "Watchdog", "Tiefschlaf", "Spannungseinbruch (Brownout)", "SDIO"};
    const int rr = (int)esp_reset_reason();
    LOGS("=== Start %s, Grund: %s%s\n", FW_VERSION, rr >= 0 && rr < 11 ? RR[rr] : "?",
         plogOk ? "" : " (Flash-Protokoll nicht verfuegbar)");
    if (gPrevValid) LOGF("Vorher: Schleife %08lx, Netz %lu, min. Speicher %lu, nach %lu min\n", (unsigned long)gPrevLoop,
                         (unsigned long)gPrevNet, (unsigned long)gPrevMinHeap, (unsigned long)gPrevUpMin);
    if (gPrevWdt[0][0]) LOGF("Watchdog: auf Kern 0 lief '%s', auf Kern 1 '%s'\n", gPrevWdt[0], gPrevWdt[1]);
  }
  sntp_set_time_sync_notification_cb([](struct timeval* tv) {
    gNetOkAt = millis() | 1;
    gTimeSet = true;
    struct tm lt; time_t t = tv->tv_sec; localtime_r(&t, &lt);
    LOGF("NTP: Zeit gesetzt %02d.%02d.%d %02d:%02d:%02d\n", lt.tm_mday, lt.tm_mon + 1, lt.tm_year + 1900, lt.tm_hour, lt.tm_min, lt.tm_sec);
  });
  settingsLoad();
  bz::setVolume(gSet.sndVol);
  bz::setIdleOff(!(gSet.sndFx & FXB_HOLD));
  cal::load();                   // gespeicherte Schulferien
  gStyle = listStyle(gSet.clockStyle);   // aktiver Wechsel: gleich mit einem Design aus der Liste starten
  gStyleSince = millis();
  applyTimezone();
  themeApply();
  blInit();
  bootMs = millis();
  lastConnectedMs = bootMs;

  displayInit();
  blApply();
  fc::prealloc();                // Bildpuffer der Designs jetzt holen (vor WLAN/TLS) und dauerhaft behalten
#if BOARD_CYD
  (void)*(volatile const char*)CYD_SELF_MARK;   // Kennung sicher im Programm behalten
  {                              // erkannten Display-Controller merken (RetroCYD uebernimmt ihn beim ersten Start)
    Preferences p;
    if (p.begin("wuhr", false)) { if (p.getUChar("pused", 0) != gPanelUsed) p.putUChar("pused", gPanelUsed); p.end(); }
  }
  dual::detect();                // steckt RetroCYD im zweiten Programmbereich?
  dual::bootChooser();           // nach dem Einschalten: Uhr oder Spiele
#endif
  drawText(&FreeSans12pt7b, 1, TR(K_STARTING), tft.width() / 2, tft.height() / 2, C_GREY, AL_CENTER,
           0, tft.height() / 2 - 30, tft.width(), 45);

  gMutex = xSemaphoreCreateMutex();
  wifiBegin();
  webBegin();
  gNetMutex = xSemaphoreCreateMutex();
  // Prioritaet 0 (wie der Leerlauf-Task): Warteschleifen beim Lesen (HTTP-Zeitlimit bis 8 s, ohne Pause) duerfen den
  // Leerlauf-Task von Kern 0 nicht aushungern - sonst schlaegt der Task-Watchdog zu (gemeldet beim Flugradar).
  // Ohne feste Kernbindung (wie frueher der Radar-Task): kann auch auf Kern 1 laufen, dessen Leerlauf nicht ueberwacht wird.
  // Kern 0 (wie WLAN): Protokoll 2026.11.18-1 zeigte bei jedem TLS-Verbindungsaufbau (Wetter, Radar) ein Bild,
  // das rund 0,64 s brauchte. Auf Kern 1 laeuft die Anzeige; dort soll der Netz-Task nicht mehr hin.
  xTaskCreatePinnedToCore(netTask, "net", 12288, nullptr, tskIDLE_PRIORITY, nullptr, 0);   // Wetter, Kalender, Radar, ISS

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
      for (int k = 0; k < 4; k++) { gDiagSecLast[k] = gDiagSec[k]; gDiagSec[k] = 0; }
#if DEBUG_SERIAL
      Serial.printf("Diag: max Schleifenluecke %lu ms, max Bildzeit %lu us\n", (unsigned long)gDiagGapMax, (unsigned long)gDiagFrameMax);
      Serial.printf("Diag: wlan %lu web %lu zeichnen %lu delay %lu us\n", (unsigned long)gDiagSecLast[0], (unsigned long)gDiagSecLast[1], (unsigned long)gDiagSecLast[2], (unsigned long)gDiagSecLast[3]);
#endif
      gDiagGapMax = 0; gDiagFrameMax = 0;
    }
  }
  gMLoops++;
  uint32_t m0 = micros();
  auto lap = [&](int k) { const uint32_t t = micros(); const uint32_t d = t - m0; m0 = t; if (d > gDiagSec[k]) gDiagSec[k] = d; };
  gCrumbLoop = 1;
  wifiLoop(now);
  lap(0);
  gCrumbLoop = 2;
  webLoop(now);
#if ARDUINO_OTA
  if (gArduinoOtaOn) ArduinoOTA.handle();
#endif
  lap(1);
  gCrumbLoop = 3;
  dimTick(now, false);
  styleTick(now);
  gCrumbLoop = 4;
  soundTick(now);
  heapGuard(now);
  plogTick(now);                         // wartende Protokollzeilen ins Flash
  gCrumbLoop = 5;
#if BOARD_CYD
  const bool dualUi = dual::tick(now);   // Menue oder Kalibrierung auf dem Bildschirm
#else
  const bool dualUi = false;
#endif
  if (!dualUi) tickerTick(now);
#if HAS_TOUCH
  if (!dualUi) touchTick(now);
#endif
#if PIN_LED_R >= 0
  led::tick(now);           // RGB-LED: Modus, Hinweise (Wecker blinkt rot)
#endif
  if (wakePending && (int32_t)(now - wakeAt) >= 0) finishWake();
  m0 = micros();
#if BOARD_CYD
  // Das Halte-Menue kann eben erst in touchTick() aufgegangen sein: dann in diesem Durchlauf nicht mehr
  // zeichnen, sonst landen geaenderte Teile des Uhrbilds ueber dem Menue
  const bool uiOpen = dualUi || dual::ui != dual::UI_NONE;
#else
  const bool uiOpen = dualUi;
#endif
  hiPolicy();                                  // auch im Standby/Menue: Speicher fuer Netzabrufe freigeben
  gCrumbLoop = 6 | ((uint32_t)gStyle << 8);
  if (!standby && !wakePending && !gOtaActive && !uiOpen) renderTick(now);
  gCrumbLoop = 7;
  lap(2);
  if (setDirty && now - setDirtyAt > 3000) {   // Helligkeit erst nach Ruhephase speichern
    settingsSave();
    setDirty = false;
  }
  m0 = micros();
  delay(2);   // gibt nur CPU-Zeit an andere Tasks ab; keine Wartelogik
  lap(3);
}
