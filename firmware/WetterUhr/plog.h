#pragma once
/*
  plog.h - dauerhaftes Protokoll im Flash (uebersteht Neustart, Absturz, Stromausfall und Firmware-Updates)

  Ablage: Bereich "spiffs" des Partitionsschemas "Minimal SPIFFS" (0x3D0000, 128 KB). Genutzt werden die ersten
  31 Sektoren (124 KB) als Ringpuffer; der letzte 4-KB-Sektor bleibt frei - dort liegt die Sicherung von WLAN-Daten
  und Einstellungen (cyd_shared.h).

  Aufbau: Jeder Sektor beginnt mit 8 Bytes Kopf (Kennung + laufende Nummer), danach Textzeilen. Ist ein Sektor voll,
  wird der aelteste geloescht und weitergeschrieben. Zeilen enthalten nie 0xFF, das Ende eines Sektors ist also das
  erste 0xFF. Kein Dateisystem: nichts kann "kaputtgehen", ein Stromausfall kostet hoechstens die halbe letzte Zeile.

  Umfang: bei rund 20 KB pro Tag (Zustand alle 10 min + Ereignisse) etwa 6 Tage.
  Schreiben nur aus der Hauptschleife (plog::write); die Zeilen sammelt WetterUhr.ino vorher im RTC-Speicher.
  Loeschen: nur per USB-Erstinstallation (die ueberschreibt den ganzen Flash).
*/
#include <Arduino.h>
#include <esp_partition.h>

namespace plog {

static const uint32_t SEC = 4096;
static const uint32_t MAGIC = 0x31474C57u;          // "WLG1"
static const esp_partition_t* part = nullptr;
static uint32_t nSec = 0, curSec = 0, curOff = 0, curSeq = 0;
static bool ok = false;

struct Head { uint32_t magic, seq; };

static bool readHead(uint32_t s, Head& h) {
  return esp_partition_read(part, s * SEC, &h, sizeof(h)) == ESP_OK && h.magic == MAGIC && h.seq != 0xFFFFFFFFu;
}
static bool startSector(uint32_t s, uint32_t seq) {
  if (esp_partition_erase_range(part, s * SEC, SEC) != ESP_OK) return false;
  const Head h{MAGIC, seq};
  if (esp_partition_write(part, s * SEC, &h, sizeof(h)) != ESP_OK) return false;
  curSec = s; curSeq = seq; curOff = sizeof(Head);
  return true;
}

// Bereich suchen, letzten Sektor und Schreibstelle finden
static bool begin() {
  part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, nullptr);
  if (!part || part->size < 4 * SEC) return false;
  nSec = part->size / SEC - 1;                         // letzter Sektor: Einstellungs-Sicherung, nicht anfassen
  if (nSec > 64) nSec = 64;
  bool any = false;
  for (uint32_t s = 0; s < nSec; s++) {
    Head h;
    if (readHead(s, h) && (!any || (int32_t)(h.seq - curSeq) > 0)) { any = true; curSec = s; curSeq = h.seq; }
  }
  if (!any) { ok = startSector(0, 1); return ok; }
  // Ende der Daten im aktuellen Sektor: erstes 0xFF
  uint8_t b[256];
  curOff = SEC;
  for (uint32_t o = sizeof(Head); o < SEC && curOff == SEC; o += sizeof(b)) {
    const uint32_t n = min<uint32_t>(sizeof(b), SEC - o);
    if (esp_partition_read(part, curSec * SEC + o, b, n) != ESP_OK) return false;
    for (uint32_t i = 0; i < n; i++) if (b[i] == 0xFF) { curOff = o + i; break; }
  }
  ok = true;
  return true;
}

// Eine oder mehrere ganze Zeilen anhaengen (nur aus der Hauptschleife aufrufen: Loeschen eines Sektors dauert ~40 ms)
static void write(const char* s, size_t n) {
  if (!ok) return;
  while (n) {
    const char* e = (const char*)memchr(s, '\n', n);
    size_t len = e ? (size_t)(e - s) + 1 : n;            // eine Zeile
    if (len > SEC - sizeof(Head)) len = SEC - sizeof(Head);
    if (curOff + len > SEC) {                            // Sektor voll: naechsten (aeltesten) loeschen
      if (!startSector((curSec + 1) % nSec, curSeq + 1)) { ok = false; return; }
    }
    if (esp_partition_write(part, curSec * SEC + curOff, s, len) != ESP_OK) { ok = false; return; }
    curOff += len; s += len; n -= len;
  }
}

// Alles lesen, aelteste Zeilen zuerst; out(text, laenge) wird stueckweise aufgerufen
template <typename F> static void readAll(F out) {
  if (!ok) return;
  uint32_t seq[64], idx[64], m = 0;
  for (uint32_t s = 0; s < nSec; s++) { Head h; if (readHead(s, h)) { seq[m] = h.seq; idx[m] = s; m++; } }
  for (uint32_t i = 1; i < m; i++)                      // nach laufender Nummer sortieren (Ueberlauf-sicher)
    for (uint32_t j = i; j > 0 && (int32_t)(seq[j - 1] - seq[j]) > 0; j--) {
      uint32_t t = seq[j]; seq[j] = seq[j - 1]; seq[j - 1] = t; t = idx[j]; idx[j] = idx[j - 1]; idx[j - 1] = t;
    }
  char b[512];
  for (uint32_t k = 0; k < m; k++) {
    for (uint32_t o = sizeof(Head); o < SEC; o += sizeof(b)) {
      const uint32_t n = min<uint32_t>(sizeof(b), SEC - o);
      if (esp_partition_read(part, idx[k] * SEC + o, b, n) != ESP_OK) return;
      uint32_t u = 0;
      while (u < n && (uint8_t)b[u] != 0xFF) u++;
      if (u) out(b, u);
      if (u < n) break;                                 // Ende der Daten in diesem Sektor
    }
  }
}

static uint32_t usedKB() { return ok ? (curSeq >= nSec ? nSec : curSeq) * SEC / 1024 : 0; }
static uint32_t capKB() { return nSec * SEC / 1024; }

}  // namespace plog
