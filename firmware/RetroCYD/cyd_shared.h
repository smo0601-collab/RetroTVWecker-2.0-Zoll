#pragma once
/*
  cyd_shared.h - gemeinsamer Teil von Wetteruhr und RetroCYD auf dem CYD 2,8 Zoll ("zwei Programme auf einem Geraet")

  Diese Datei liegt IDENTISCH in beiden Projekten (WetterUhr und RetroCYD). Sie enthaelt:
    - Erkennung, welches Programm im anderen Programmbereich (OTA-Partition) steckt
    - Umschalten zwischen den Programmen (Start-Partition setzen, Neustart)
    - Touchscreen XPT2046 mit Koordinaten und gemeinsamer Kalibrierung (gespeichert im NVS der Uhr, "wuhr")

  Aufteilung des Flash (Partitionsschema "Minimal SPIFFS (1.9MB APP with OTA)"):
    app0 0x010000 und app1 0x1F0000, je 1,9 MB. In einem Bereich liegt die Uhr, im anderen RetroCYD.
    Welcher wo liegt, ist egal und wird beim Start erkannt.
*/
#include <Arduino.h>
#include <Preferences.h>
#include <esp_ota_ops.h>
#include <esp_partition.h>

namespace cyd {

// ---------------------------------------------------------------------
// Kennungen der Programme. Im Programm selbst steht die Kennung im Klartext (CYD_SELF_MARK, im .ino definiert).
// Gesucht wird mit einer verschobenen Schreibweise (jedes Zeichen +1), damit die Kennung des ANDEREN Programms
// nicht im Klartext im eigenen Programm steht und die Suche sich nicht selbst findet.
// ---------------------------------------------------------------------
enum App : uint8_t { APP_NONE = 0, APP_CLOCK = 1, APP_GAMES = 2 };
static const char ENC_CLOCK[] = "XFUUFSVIS.BQQ.JE.2";   // "WETTERUHR-APP-ID-1"
static const char ENC_GAMES[] = "SFUSPDZE.BQQ.JE.2";    // "RETROCYD-APP-ID-1"

static void decode(const char* enc, char* out) {
  while (*enc) *out++ = (char)(*enc++ - 1);
  *out = 0;
}

// Anderer Programmbereich (der, in den ein Update geschrieben wuerde)
static const esp_partition_t* otherPart() { return esp_ota_get_next_update_partition(nullptr); }

// Steckt im Bereich ein gueltiges Programm mit der gesuchten Kennung?
static bool partHas(const esp_partition_t* p, App app) {
  if (!p) return false;
  esp_app_desc_t d;
  if (esp_ota_get_partition_description(p, &d) != ESP_OK) return false;   // kein gueltiges Programm
  char mark[24];
  decode(app == APP_CLOCK ? ENC_CLOCK : ENC_GAMES, mark);
  const size_t ml = strlen(mark);
  uint8_t* buf = (uint8_t*)malloc(4096 + 32);   // nur kurz belegt (kein dauerhafter Speicher)
  if (!buf) return false;
  size_t keep = 0;
  bool found = false;
  for (size_t off = 0; off < p->size && !found; off += 4096) {
    if (esp_partition_read(p, off, buf + keep, 4096) != ESP_OK) break;
    const size_t n = keep + 4096;
    for (size_t i = 0; i + ml <= n; i++)
      if (buf[i] == (uint8_t)mark[0] && memcmp(buf + i, mark, ml) == 0) { found = true; break; }
    keep = ml - 1;
    memmove(buf, buf + n - keep, keep);
  }
  free(buf);
  return found;
}

static App otherApp() {
  const esp_partition_t* p = otherPart();
  if (partHas(p, APP_GAMES)) return APP_GAMES;
  if (partHas(p, APP_CLOCK)) return APP_CLOCK;
  return APP_NONE;
}

// Beim naechsten Neustart das andere Programm starten
static bool bootOther() {
  const esp_partition_t* p = otherPart();
  return p && esp_ota_set_boot_partition(p) == ESP_OK;
}
// Beim naechsten Neustart wieder dieses Programm starten (z. B. nach einem Update des anderen Programms)
static void bootSelf() {
  const esp_partition_t* r = esp_ota_get_running_partition();
  if (r && esp_ota_get_boot_partition() != r) esp_ota_set_boot_partition(r);
}

// Welches Programm steckt in einer hochgeladenen Datei? Erkennung am Dateinamen
// (Release-Dateien: "RetroCYD_..." bzw. "WetterUhr_..._CYD_...", Arduino-Export: "RetroCYD.ino.bin" / "WetterUhr.ino.bin").
static App appFromName(const String& name) {
  String n = name; n.toLowerCase();
  if (n.indexOf("retrocyd") >= 0) return APP_GAMES;
  if (n.indexOf("wetteruhr") >= 0 || n.indexOf("retrotv") >= 0) return APP_CLOCK;
  return APP_NONE;
}

// Waehrend eines Uploads mitlesen, ob die erwartete Kennung in der Datei vorkommt
struct MarkScan {
  char mark[24] = {0};
  size_t ml = 0, hit = 0;
  bool found = false;
  void begin(App app) { decode(app == APP_CLOCK ? ENC_CLOCK : ENC_GAMES, mark); ml = strlen(mark); hit = 0; found = false; }
  void feed(const uint8_t* d, size_t n) {
    for (size_t i = 0; i < n && !found; i++) {
      if (d[i] == (uint8_t)mark[hit]) { if (++hit == ml) found = true; }
      else hit = (d[i] == (uint8_t)mark[0]) ? 1 : 0;
    }
  }
};

// ---------------------------------------------------------------------
// Touchscreen XPT2046 (CLK 25, MOSI 32, MISO 39, CS 33, IRQ 36), Software-SPI
// ---------------------------------------------------------------------
static const int T_CLK = 25, T_MOSI = 32, T_MISO = 39, T_CS = 33, T_IRQ = 36;

static uint16_t tXfer(uint8_t cmd) {
  for (int i = 7; i >= 0; i--) {
    digitalWrite(T_MOSI, (cmd >> i) & 1);
    digitalWrite(T_CLK, HIGH); delayMicroseconds(1);
    digitalWrite(T_CLK, LOW);  delayMicroseconds(1);
  }
  digitalWrite(T_MOSI, LOW);
  uint16_t v = 0;
  for (int i = 0; i < 16; i++) {
    digitalWrite(T_CLK, HIGH); delayMicroseconds(1);
    v = (v << 1) | (digitalRead(T_MISO) ? 1 : 0);
    digitalWrite(T_CLK, LOW);  delayMicroseconds(1);
  }
  return (v >> 3) & 0x0FFF;
}

static void touchBegin() {
  pinMode(T_CS, OUTPUT);   digitalWrite(T_CS, HIGH);
  pinMode(T_CLK, OUTPUT);  digitalWrite(T_CLK, LOW);
  pinMode(T_MOSI, OUTPUT); digitalWrite(T_MOSI, LOW);
  pinMode(T_MISO, INPUT);
  pinMode(T_IRQ, INPUT);
}

// Rohwerte lesen: a = Kanal 0x90, b = Kanal 0xD0 (Mittel aus 4 Messungen). Rueckgabe: Druck z (0 = nicht beruehrt)
static int touchRaw(int& a, int& b) {
  if (digitalRead(T_IRQ) == HIGH) return 0;
  digitalWrite(T_CS, LOW);
  const int z1 = tXfer(0xB0), z2 = tXfer(0xC0);
  int sa = 0, sb = 0;
  for (int i = 0; i < 4; i++) { sa += tXfer(0x90); sb += tXfer(0xD0); }
  tXfer(0x90);
  tXfer(0x00);                                        // PD=00: Stift-Interrupt wieder aktiv
  digitalWrite(T_CS, HIGH);
  a = sa / 4; b = sb / 4;
  int z = z1 + 4095 - z2;
  if (z1 >= 4000 || z < 0) z = 0;
  return z;
}

// Kalibrierung: Rohwerte an drei Punkten des Bildschirms 320x240 (Querformat):
//   oben links (20,20), oben rechts (300,20), unten links (20,220).
// swap = 1: Bildschirm-X folgt Kanal b statt a. rot = Ausrichtung der Uhr bei der Kalibrierung (1 oder 3).
struct Cal {
  uint8_t ver = 1, rot = 1, swap = 0, ok = 0;
  int16_t x0 = 419, x1 = 3481;   // Rohwert bei X = 20 bzw. 300 (Standardwerte fuer das CYD mit ILI9341)
  int16_t y0 = 537, y1 = 3503;   // Rohwert bei Y = 20 bzw. 220
};
static Cal cal;

static void calLoad() {
  Preferences p;
  if (p.begin("wuhr", true)) {
    if (p.getBytesLength("tcal") == sizeof(Cal)) p.getBytes("tcal", &cal, sizeof(Cal));
    p.end();
  }
  if (cal.ver != 1 || cal.x0 == cal.x1 || cal.y0 == cal.y1) cal = Cal();
}
static void calSave() {
  Preferences p;
  if (p.begin("wuhr", false)) { p.putBytes("tcal", &cal, sizeof(Cal)); p.end(); }
}
// Kalibrierung aus drei Messungen (Rohwerte a/b an den Punkten oben links, oben rechts, unten links)
static bool calFrom(const int a[3], const int b[3], uint8_t rot) {
  Cal c;
  c.rot = rot;
  const int da = abs(a[1] - a[0]), db = abs(b[1] - b[0]);   // von links nach rechts: welcher Kanal aendert sich?
  c.swap = db > da;
  const int* X = c.swap ? b : a;
  const int* Y = c.swap ? a : b;
  c.x0 = X[0]; c.x1 = X[1]; c.y0 = Y[0]; c.y1 = Y[2];
  if (abs(c.x1 - c.x0) < 800 || abs(c.y1 - c.y0) < 600) return false;   // unplausibel (z. B. zweimal gleiche Stelle)
  c.ok = 1;
  cal = c;
  return true;
}
// Rohwerte -> Bildschirmkoordinaten 320x240 fuer die Ausrichtung rot (1 oder 3)
static void calMap(int a, int b, uint8_t rot, int& x, int& y) {
  const int rx = cal.swap ? b : a, ry = cal.swap ? a : b;
  x = 20 + (long)(rx - cal.x0) * 280 / (cal.x1 - cal.x0);
  y = 20 + (long)(ry - cal.y0) * 200 / (cal.y1 - cal.y0);
  if (rot != cal.rot) { x = 319 - x; y = 239 - y; }    // um 180 Grad gedreht gegenueber der Kalibrierung
  x = constrain(x, 0, 319); y = constrain(y, 0, 239);
}

}  // namespace cyd
