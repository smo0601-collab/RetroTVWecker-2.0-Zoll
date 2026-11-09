#pragma once
/*
  cyd_shared.h - gemeinsamer Teil von Wetteruhr und RetroCYD auf dem CYD 2,8 Zoll ("zwei Programme auf einem Geraet")

  Diese Datei liegt IDENTISCH in beiden Projekten (WetterUhr und RetroCYD). Sie enthaelt:
    - Erkennung, welches Programm im anderen Programmbereich (OTA-Partition) steckt
    - Umschalten zwischen den Programmen (Start-Partition setzen, Neustart)
    - Touchscreen XPT2046 mit Koordinaten und gemeinsamer Kalibrierung (gespeichert im NVS der Uhr, "wuhr")
    - Sicherung von WLAN-Daten und Uhr-Einstellungen ausserhalb des NVS (im ungenutzten Bereich "spiffs").
      Hintergrund: Uhr und RetroCYD teilen sich den kleinen NVS-Bereich (20 KB). Wird er voll oder unlesbar,
      loescht der Arduino-Start ihn komplett - dann gingen WLAN und Einstellungen verloren. Aus der Sicherung
      stellt die Uhr sie beim naechsten Start wieder her, und RetroCYD findet das WLAN der Uhr.

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

static bool bakRead(uint8_t slot, void* data, uint16_t size, uint16_t ver);
static bool bakWrite(uint8_t slot, const void* data, uint16_t size, uint16_t ver);
static void calLoad() {
  bool got = false;
  Preferences p;
  if (p.begin("wuhr", true)) {
    if (p.getBytesLength("tcal") == sizeof(Cal)) { p.getBytes("tcal", &cal, sizeof(Cal)); got = true; }
    p.end();
  }
  if (!got) bakRead(1, &cal, sizeof(Cal), 1);         // NVS verloren: aus der Sicherung
  if (cal.ver != 1 || cal.x0 == cal.x1 || cal.y0 == cal.y1) cal = Cal();
}
static void calSave() {
  Preferences p;
  if (p.begin("wuhr", false)) { p.putBytes("tcal", &cal, sizeof(Cal)); p.end(); }
  bakWrite(1, &cal, sizeof(Cal), 1);
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

// ---------------------------------------------------------------------
// Sicherung ausserhalb des NVS: letzter 4-KB-Sektor des Bereichs "spiffs" (0x3D0000, wird sonst nicht genutzt)
//   Platz 0 (Offset    0, 256 B):  WLAN (SSID/Passwort der Uhr)
//   Platz 1 (Offset  256, 256 B):  Touch-Kalibrierung
//   Platz 2 (Offset 1024, 3 KB):   Einstellungen der Uhr (Struktur Settings)
// ---------------------------------------------------------------------
enum : uint8_t { BAK_WIFI = 0, BAK_CAL = 1, BAK_SET = 2 };
struct BakHdr { uint32_t magic; uint16_t size; uint16_t ver; uint32_t crc; };
static const uint32_t BAK_MAGIC = 0x4B425557;   // "WUBK"
struct BakWifi { char ssid[33]; char pass[65]; };

static uint32_t fnv(const uint8_t* d, size_t n) { uint32_t h = 2166136261u; while (n--) { h ^= *d++; h *= 16777619u; } return h; }
static const esp_partition_t* bakPart() {
  return esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, nullptr);
}
static size_t bakOff(uint8_t slot) { return slot == BAK_WIFI ? 0 : slot == BAK_CAL ? 256 : 1024; }
static size_t bakMax(uint8_t slot) { return (slot == BAK_SET ? 3072 : 256) - sizeof(BakHdr); }

static bool bakRead(uint8_t slot, void* data, uint16_t size, uint16_t ver = 1) {
  const esp_partition_t* p = bakPart();
  if (!p || p->size < 4096 || size > bakMax(slot)) return false;
  const size_t base = p->size - 4096 + bakOff(slot);
  BakHdr h;
  if (esp_partition_read(p, base, &h, sizeof(h)) != ESP_OK) return false;
  if (h.magic != BAK_MAGIC || h.size != size || h.ver != ver) return false;
  uint8_t* tmp = (uint8_t*)malloc(size);
  if (!tmp) return false;
  bool ok = esp_partition_read(p, base + sizeof(h), tmp, size) == ESP_OK && fnv(tmp, size) == h.crc;
  if (ok) memcpy(data, tmp, size);
  free(tmp);
  return ok;
}

// Schreibt einen Platz (liest den Sektor, aendert ihn, loescht und schreibt ihn neu). Unveraendert = kein Schreiben.
static bool bakWrite(uint8_t slot, const void* data, uint16_t size, uint16_t ver = 1) {
  const esp_partition_t* p = bakPart();
  if (!p || p->size < 4096 || size > bakMax(slot)) return false;
  const size_t sec = p->size - 4096;
  uint8_t* buf = (uint8_t*)malloc(4096);
  if (!buf) return false;
  bool ok = esp_partition_read(p, sec, buf, 4096) == ESP_OK;
  if (ok) {
    BakHdr h = {BAK_MAGIC, size, ver, fnv((const uint8_t*)data, size)};
    uint8_t* at = buf + bakOff(slot);
    if (memcmp(at, &h, sizeof(h)) == 0 && memcmp(at + sizeof(h), data, size) == 0) { free(buf); return true; }
    memcpy(at, &h, sizeof(h));
    memcpy(at + sizeof(h), data, size);
    ok = esp_partition_erase_range(p, sec, 4096) == ESP_OK && esp_partition_write(p, sec, buf, 4096) == ESP_OK;
  }
  free(buf);
  return ok;
}

static void bakClear() {
  const esp_partition_t* p = bakPart();
  if (p && p->size >= 4096) esp_partition_erase_range(p, p->size - 4096, 4096);
}

// WLAN-Daten der Uhr: zuerst aus dem NVS der Uhr, sonst aus der Sicherung
static bool clockWifi(String& ssid, String& pass) {
  Preferences u;
  if (u.begin("wuhr", true)) {
    ssid = u.getString("ssid", ""); pass = u.getString("pass", "");
    u.end();
    if (ssid.length()) return true;
  }
  BakWifi w;
  if (bakRead(BAK_WIFI, &w, sizeof(w)) && w.ssid[0]) { w.ssid[32] = 0; w.pass[64] = 0; ssid = w.ssid; pass = w.pass; return true; }
  return false;
}

}  // namespace cyd
