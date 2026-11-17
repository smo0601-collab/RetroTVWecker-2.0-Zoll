#pragma once
/*
  sdcard.h - Micro-SD-Karte am CYD: Protokoll, Wetter- und Messwerte, Sicherung der Einstellungen

  Nur CYD (der ESP32-C3-Aufbau hat keinen Kartenleser). Steckplatz auf der Rueckseite, eigener SPI-Bus
  (HSPI: SCK 18, MISO 19, MOSI 23, CS 5) - das Display behaelt seinen Bus. Karte FAT32, bis 32 GB.
  Die Karte teilt sich die Uhr mit RetroCYD (das nutzt den Ordner /retrocyd).

  Ordner /wetteruhr auf der Karte:
    log/2026-11-17.txt        komplettes Protokoll je Tag (auch Routine-Meldungen; Flash und RTC haben nur Auszuege)
    wetter_2026-11.csv         jede Wetter-Aktualisierung (alle 15 min): Temperatur, gefuehlt, Feuchte, Wind, Wettercode
    zustand_2026-11.csv        alle 10 min: Speicher, WLAN, Netz, Browser, Bilder/s, Bildzeit, Design ...
    einstellungen.bin          Sicherung der Einstellungen (bei jedem Speichern; wird benutzt, wenn NVS und
                               Flash-Sicherung leer sind, z. B. nach einer USB-Erstinstallation)
  CSV: Trennzeichen ";" und Dezimalkomma - laesst sich direkt in Excel/LibreOffice oeffnen.

  Alle Kartenzugriffe laufen in der Hauptschleife (sd::tick). Andere Tasks legen Zeilen nur in eine Warteschlange
  (sd::put). Karte gezogen oder Fehler: alles laeuft weiter, die Uhr versucht nach 1, 2 und 5 Minuten und danach alle 10 Minuten, die Karte neu einzubinden.
*/
#include <Arduino.h>
#if BOARD_CYD
#include <SD.h>
#include <SPI.h>
#define HAS_SD 1
#else
#define HAS_SD 0
#endif

namespace sd {

enum : char { T_LOG = 'L', T_WX = 'W', T_STATE = 'Z' };

#if HAS_SD
static const int PIN_CS = 5, PIN_SCK = 18, PIN_MISO = 19, PIN_MOSI = 23;
static SPIClass spi(HSPI);
static bool mounted = false, spiUp = false;
static uint32_t lastTry = 0, written = 0, errors = 0;
static uint64_t cardMB = 0;

// Warteschlange: Eintraege "<Typ><Text>\n" (Typ = L/W/Z), von jedem Task befuellbar
static const size_t QSZ = 2048;
static char q[QSZ];
static size_t qLen = 0;
static uint32_t qLost = 0;
static portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

static void put(char type, const char* text) {
  const size_t n = strlen(text);
  if (!n) return;
  const bool nl = text[n - 1] == '\n';
  portENTER_CRITICAL(&mux);
  if (qLen + n + 2 <= QSZ) {
    q[qLen++] = type;
    memcpy(q + qLen, text, n); qLen += n;
    if (!nl) q[qLen++] = '\n';
  } else qLost++;
  portEXIT_CRITICAL(&mux);
}

static bool mount() {
  if (!spiUp) { spi.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS); spiUp = true; }
  SD.end();
  if (!SD.begin(PIN_CS, spi, 20000000, "/sd", 2)) { mounted = false; return false; }
  if (SD.cardType() == CARD_NONE) { SD.end(); mounted = false; return false; }
  cardMB = SD.cardSize() / (1024ULL * 1024ULL);
  if (!SD.exists("/wetteruhr")) SD.mkdir("/wetteruhr");
  if (!SD.exists("/wetteruhr/log")) SD.mkdir("/wetteruhr/log");
  mounted = true;
  return true;
}
static bool begin() { lastTry = millis(); return mount(); }
static bool ok() { return mounted; }

// Dateiname zum Typ (nach aktuellem Datum); false, wenn die Uhrzeit noch nicht stimmt
static bool pathFor(char type, char* p, size_t n, const char** header) {
  const time_t t = time(nullptr);
  if (t < 1700000000) return false;
  struct tm lt; localtime_r(&t, &lt);
  *header = nullptr;
  if (type == T_LOG) snprintf(p, n, "/wetteruhr/log/%04d-%02d-%02d.txt", lt.tm_year + 1900, lt.tm_mon + 1, lt.tm_mday);
  else if (type == T_WX) {
    snprintf(p, n, "/wetteruhr/wetter_%04d-%02d.csv", lt.tm_year + 1900, lt.tm_mon + 1);
    *header = "Datum;Uhrzeit;Temperatur C;gefuehlt C;Feuchte %;Wind km/h;Wettercode;Tag;Min heute C;Max heute C\n";
  } else {
    snprintf(p, n, "/wetteruhr/zustand_%04d-%02d.csv", lt.tm_year + 1900, lt.tm_mon + 1);
    *header = "Datum;Uhrzeit;Laufzeit min;Heap frei;Heap Block;Heap min;WLAN dBm;Wetter ok;Netz-ok vor s;Web-Anfragen;"
              "Bilder/s;Bildzeit us;Bildzeit max us;Kacheln/Bild;Schleifen/s;Design;ISS-Aussetzer\n";
  }
  return true;
}

// aus der Hauptschleife: Warteschlange alle 10 s auf die Karte schreiben, Karte jede Minute neu suchen
static void tick(uint32_t now) {
  static uint32_t last = 0;
  if (!mounted) {
    // Neu einbinden: nach 1, 2, 5 min, danach alle 10 min (ein Versuch ohne Karte haelt die Anzeige kurz an)
    static uint8_t tries = 0;
    static const uint32_t WAIT[] = {60000UL, 120000UL, 300000UL, 600000UL};
    if (now - lastTry > WAIT[tries < 3 ? tries : 3]) { lastTry = now; if (mount()) tries = 0; else if (tries < 3) tries++; }
    if (!mounted) { portENTER_CRITICAL(&mux); if (qLen > QSZ / 2) { qLost++; qLen = 0; } portEXIT_CRITICAL(&mux); return; }
  }
  if (now - last < 10000UL && qLen < QSZ / 2) return;   // alle 10 s, oder frueher, wenn die Schlange halb voll ist
  last = now;
  static char b[QSZ];
  size_t n; uint32_t lost;
  portENTER_CRITICAL(&mux);
  n = qLen; lost = qLost; memcpy(b, q, n); qLen = 0; qLost = 0;
  portEXIT_CRITICAL(&mux);
  if (!n && !lost) return;
  if (lost) { char m[64]; snprintf(m, sizeof(m), "    (%lu Zeilen fuer die SD-Karte verloren)\n", (unsigned long)lost); put(T_LOG, m); }
  // nacheinander je Typ: Datei einmal oeffnen, alle Zeilen dieses Typs anhaengen
  for (const char type : {T_LOG, T_WX, T_STATE}) {
    char path[48]; const char* header;
    bool any = false;
    for (size_t i = 0; i < n;) { const char* e = (const char*)memchr(b + i, '\n', n - i); const size_t end = e ? (size_t)(e - b) + 1 : n;
                                 if (b[i] == type) { any = true; break; } i = end; }
    if (!any || !pathFor(type, path, sizeof(path), &header)) continue;
    File f = SD.open(path, FILE_APPEND);
    if (!f) { errors++; mounted = false; return; }   // Karte weg? Naechste Minute neu einbinden
    if (header && f.size() == 0) f.print(header);
    for (size_t i = 0; i < n;) {
      const char* e = (const char*)memchr(b + i, '\n', n - i);
      const size_t end = e ? (size_t)(e - b) + 1 : n;
      if (b[i] == type && end > i + 1) { f.write((const uint8_t*)b + i + 1, end - i - 1); written += end - i - 1; }
      i = end;
    }
    f.close();
  }
}

// ---- Sicherung der Einstellungen ----
struct BakHead { uint32_t magic; uint16_t size, ver; uint32_t crc; };
static const uint32_t BAK_MAGIC = 0x53445542u;       // "BUDS"
static uint32_t crc(const uint8_t* d, size_t n) { uint32_t h = 2166136261u; while (n--) { h ^= *d++; h *= 16777619u; } return h; }
static bool saveBlob(const char* path, const void* data, uint16_t size, uint16_t ver) {
  if (!mounted) return false;
  const BakHead h{BAK_MAGIC, size, ver, crc((const uint8_t*)data, size)};
  File f = SD.open(path, FILE_WRITE);
  if (!f) return false;
  const bool okw = f.write((const uint8_t*)&h, sizeof(h)) == sizeof(h) && f.write((const uint8_t*)data, size) == size;
  f.close();
  return okw;
}
static bool loadBlob(const char* path, void* data, uint16_t size, uint16_t ver) {
  if (!mounted || !SD.exists(path)) return false;
  File f = SD.open(path, FILE_READ);
  if (!f) return false;
  BakHead h;
  bool okr = f.read((uint8_t*)&h, sizeof(h)) == sizeof(h) && h.magic == BAK_MAGIC && h.size == size && h.ver == ver;
  uint8_t* tmp = okr ? (uint8_t*)malloc(size) : nullptr;
  okr = tmp && f.read(tmp, size) == size && crc(tmp, size) == h.crc;
  if (okr) memcpy(data, tmp, size);
  free(tmp);
  f.close();
  return okr;
}

// Zustand fuer die Status-Seite
static String status() {
  if (!mounted) return "keine Karte (Steckplatz leer oder nicht FAT32)";
  char s[96];
  snprintf(s, sizeof(s), "%llu MB, %llu MB belegt, %lu KB geschrieben%s", (unsigned long long)cardMB,
           (unsigned long long)(SD.usedBytes() / (1024ULL * 1024ULL)), (unsigned long)(written / 1024), errors ? ", Schreibfehler" : "");
  return s;
}
#else
static void put(char, const char*) {}
static bool begin() { return false; }
static bool ok() { return false; }
static void tick(uint32_t) {}
static bool saveBlob(const char*, const void*, uint16_t, uint16_t) { return false; }
static bool loadBlob(const char*, void*, uint16_t, uint16_t) { return false; }
#endif

}  // namespace sd
