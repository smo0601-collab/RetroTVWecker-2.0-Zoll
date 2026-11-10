/*
  ============================================================================
   RETRO CYD 3.7  -  Retro-Spielekonsole fuer das "Cheap Yellow Display"
                     (ESP32-2432S028R, 2,8" 240x320) mit Xbox-Controller,
                     microSD und Web-Oberflaeche
  ============================================================================

  20 Spiele in 6 Kategorien, alle im Hoch- und Querformat spielbar. Jedes Spiel
  steht in einer eigenen Datei (g_*.h) im selben Ordner - ALLE Dateien muessen
  im Ordner "RetroCYD" liegen:
    SNAKE        Schlange          g_snake.h    BLOCKS      Steine-Puzzle  g_blocks.h
    BRICKS       Mauer abraeumen   g_bricks.h   PADDLE      Tischtennis    g_paddle.h
    ASTRO ROCKS  Felsen im All     g_rocks.h    ALIEN WAVE  Alien-Formation g_invaders.h
    STAR SWARM   Sturzflug-Shooter g_swarm.h    ROAD HOP    Strasse+Fluss  g_hopper.h
    GIRDER       Kletter-Action    g_girder.h   DIG DEEP    Tunnel graben  g_digger.h
    PUFF JUMP    Schwebe-Hupfer    g_puff.h     SUNSET RACE 3D-Rennen      g_racer.h
    SKY ACE      Flieger-Shooter   g_skyace.h
  Neu in 3.7 (fuer Touch gemacht, mit Controller ebenfalls spielbar):
    2048         Zahlen schieben   g_2048.h     LICHTER AUS Lights Out     g_lights.h
    ROHRPOST     Rohre drehen      g_pipes.h    PAARE FINDEN Memory        g_memory.h
    VIER GEWINNT gegen CPU/zu zweit g_four.h    SIEBZEHN UND VIER Karten   g_blackjack.h
    SPURWECHSEL  Rennen von oben   g_lanes.h
  Gemeinsame Hilfen der Touch-Spiele: g_kit.h.

  MENUE (ab 3.7): Kategorien als Kacheln - KLASSIKER, SHOOTER, JUMP & RUN,
  RENNEN, DENKSPIELE, BRETT & KARTEN. Kategorie antippen zeigt ihre Spiele,
  "< ZURUECK" (oder B, oder die Kopfzeile antippen) fuehrt zurueck.
  Dazu: core_fb.h (flimmerfreier Bildpuffer), webpage.h (Web-Oberflaeche) und
  cyd_shared.h (Zusammenarbeit mit der Wetteruhr, gleiche Datei wie im Uhr-Projekt).

  ---------------------------------------------------------------------------
  ZUSAMMEN MIT DER WETTERUHR (ab 3.2)
  ---------------------------------------------------------------------------
  Uhr und RetroCYD koennen als zwei Programme auf demselben CYD liegen (je ein
  Programmbereich der Partition "Minimal SPIFFS (1.9MB APP with OTA)").
  - Beim Einschalten waehlt man in der Uhr "Uhr" oder "Spiele".
  - Im Spielemenue gibt es dann den Punkt "ZUR UHR".
  - Bildschirm (oder BOOT) lange halten: Menue mit ZUR UHR / SETUP / ZURUECK,
    im Spiel mit ZUR UHR / SPIELMENUE / WEITERSPIELEN.
    Bedienung: tippen = naechster Punkt, halten = waehlen (oder Controller).
  - RetroCYD startet die Uhr beim naechsten Neustart automatisch wieder.
  - Updates: Uhr-Dateien (WetterUhr_..._CYD_update.bin) hier einspielen,
    RetroCYD-Dateien (RetroCYD_..._update.bin) im Browser der Uhr.
  - Ohne Uhr laeuft RetroCYD wie bisher allein.
  Alle Figuren, Namen und Grafiken sind eigene Entwuerfe.

  ---------------------------------------------------------------------------
  EINRICHTUNG (Arduino IDE 2.x)
  ---------------------------------------------------------------------------
  1. Boardverwalter-URLs (Datei > Einstellungen), BEIDE eintragen:
       https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
       https://raw.githubusercontent.com/ricardoquesada/esp32-arduino-lib-builder/master/bluepad32_files/package_esp32_bluepad32_index.json
  2. Boardverwalter: "esp32_bluepad32" installieren.
  3. Bibliotheksverwalter: "LovyanGFX" installieren.
  4. Board: esp32_bluepad32 > "ESP32 Dev Module"
     Partition Scheme:  "Minimal SPIFFS (1.9MB APP with OTA/190KB SPIFFS)"
     !! NEU: Ohne OTA-Partition funktioniert das Web-Update nicht !!
  5. Einmal per USB hochladen (BOOT halten, RST tippen, BOOT loslassen).
     Danach gehen Updates ueber die Web-Oberflaeche.

  ---------------------------------------------------------------------------
  WEB-OBERFLAECHE  (Setup-Modus)
  ---------------------------------------------------------------------------
  Setup-Modus starten:  Menuepunkt "SETUP / WLAN"
                        ODER Bildschirm bzw. BOOT-Taste 1,5 s gedrueckt halten
                        (funktioniert auch ganz ohne Controller)
  Das CYD oeffnet das WLAN "RetroCYD-Setup" (Passwort: retro1234).
  Verbinden (QR-Code auf dem Display), dann http://192.168.4.1 oeffnen.
  Ist ein Heimnetz eingetragen: http://retrocyd.local oder angezeigte IP.

  Im Setup-Bildschirm direkt am Geraet:  X = drehen,  Y = spiegeln

  Einstellbar: Ausrichtung, Spiegelung, Farbinvertierung, Helligkeit,
  Display-Chip, Controller (koppeln, trennen, testen, Kopplungen loeschen),
  Vibration, Snake-Tempo, Highscores, WLAN, Firmware-Update (Datei oder SD).

  Firmware-Datei: Arduino IDE > Sketch > "Kompilierte Binaerdatei exportieren"
  -> RetroCYD.ino.bin hochladen (NICHT die *.merged.bin).

  ---------------------------------------------------------------------------
  STEUERUNG
  ---------------------------------------------------------------------------
  OHNE CONTROLLER (ab 3.6): Touch-Steuerung
    Menue: Spiel antippen = auswaehlen, noch einmal antippen = starten.
    Im Spiel ist der Bildschirm das Steuerkreuz: Mitte = Aktion, ausserhalb = Richtung,
    Ecke oben rechts = Pause-Menue. Shooter feuern automatisch, Sunset Race gibt
    automatisch Gas; bei Girder/Puff Jump heissen die oberen Ecken "laufen + springen".
    Vor dem ersten Spiel 3 Kreuze antippen (Touch einrichten, auch im Halte-Menue).
    Spielende: antippen = nochmal, lange halten = Menue.
    Die Touch-Spiele (siehe unten) bedient man direkt: antippen und wischen.
  Menue:      Steuerkreuz/Stick waehlen (halten = schnell scrollen), A starten
  Allgemein:  Menu-Taste = Pause, View-Taste = zurueck, Y = Screenshot (SD)
  Snake:      Richtung
  Blocks:     links/rechts, runter = schneller, hoch = fallen, A/B drehen
  Bricks:     Steuerkreuz oder Stick, A = Ball abschiessen
  Paddle:     hoch/runter (quer) bzw. links/rechts (hochkant); 2 Controller = 2P
  Astro Rocks: links/rechts drehen, hoch = Schub, A = Feuer, B = Hypersprung
  Alien Wave / Star Swarm: links/rechts, A = Feuer
  Road Hop:   Steuerkreuz = hopsen (Taste halten = weiterhopsen)
  Girder:     laufen, hoch/runter an Leitern, A = springen
  Dig Deep:   graben, A = Pumpe (mehrmals A = aufblasen)
  Puff Jump:  laufen, A = springen/flattern, B halten = einsaugen, B = spucken
  Sunset Race: lenken, A/hoch = Gas, B/runter = Bremse
  Sky Ace:    fliegen, A = Feuer, B = Looping (3x pro Leben)
  Touch-Spiele (3.7): direkt antippen bzw. wischen; Controller: Steuerkreuz
  bewegt einen Rahmen, A = antippen.
    2048:          wischen oder Pfeilfelder; ZURUECK = letzter Zug, B ebenso;
                   4 x 4 oder 5 x 5, das Spiel wird gespeichert (WEITER ...)
    Lichter aus:   Feld antippen; 5 x 5, 6 x 6, 3 Farben; Sterne nach Zuegen
    Rohrpost:      Feld antippen = drehen (B = andersherum); 6 x 5 oder 8 x 6
    Paare finden:  Karte antippen; 1 Spieler, auf Zeit, 2 Spieler;
                   Symbolsaetze FORMEN (ab 3 Siegen) und ZAHLEN (ab 6)
    Vier gewinnt:  Spalte antippen; CPU Tiefe 4/6/8, 2 Spieler, Fuenf gewinnt
    17 und 4:      Knoepfe KARTE, HALTEN, VERDOPPELN, TEILEN; Einsatz - / +;
                   TRAINER zeigt den besten Zug; Chips werden gespeichert
    Spurwechsel:   linke/rechte Bildhaelfte = Spur wechseln; Muenzen sammeln,
                   Autos kaufen; Motorton ueber den Lautsprecher-Anschluss

  ---------------------------------------------------------------------------
  MICRO-SD (optional, FAT32)
  ---------------------------------------------------------------------------
  /retrocyd/highscores.txt          Highscores
  /retrocyd/breakout/levelN.txt     eigene Level (6 Zeilen x 10 Zeichen,
                                    '.' = leer, '1'..'6' = Steinfarbe)
  /retrocyd/shots/                  Screenshots (BMP)
  /retrocyd/update.bin              Firmware fuer "Update von SD-Karte"
  /retrocyd/wlan.txt                Heimnetz einrichten, zwei Zeilen:
                                      ssid=MeinWLAN
                                      pass=MeinPasswort
  ============================================================================
*/

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <Bluepad32.h>
#include <Preferences.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <Update.h>
#include "cyd_shared.h"
#include "touchpad.h"      // Datentypen der Touch-Steuerung

// Kennung dieses Programms im Klartext (die Wetteruhr erkennt daran RetroCYD im anderen Programmbereich)
const char CYD_SELF_MARK[] = "RETROCYD-APP-ID-1";
bool clockPresent = false;   // Wetteruhr im anderen Programmbereich?
uint32_t relayToken = 0;     // Update-Modus fuer die Uhr (von der Uhr angefordert), 0 = aus

#define FW_VERSION "3.7"
#define AP_SSID    "RetroCYD-Setup"
#define HOSTNAME   "retrocyd"

// ============================================================================
//  DISPLAY  (beide Chip-Varianten eingebaut, Auswahl per Einstellung)
// ============================================================================
class LGFX : public lgfx::LGFX_Device {
  lgfx::Panel_ILI9341 _ili;
  lgfx::Panel_ST7789  _st;
  lgfx::Bus_SPI       _bus;
  lgfx::Light_PWM     _light;

  template <typename P> void setupPanel(P& p) {
    auto cfg = p.config();
    cfg.pin_cs          = 15;
    cfg.pin_rst         = -1;
    cfg.pin_busy        = -1;
    cfg.panel_width     = 240;
    cfg.panel_height    = 320;
    cfg.offset_x        = 0;
    cfg.offset_y        = 0;
    cfg.offset_rotation = 0;
    cfg.readable        = true;
    cfg.invert          = false;
    cfg.rgb_order       = false;
    cfg.dlen_16bit      = false;
    cfg.bus_shared      = false;
    p.config(cfg);
    p.setBus(&_bus);
    p.setLight(&_light);
  }

public:
  LGFX() {
    {
      auto cfg = _bus.config();
      cfg.spi_host    = SPI2_HOST;
      cfg.spi_mode    = 0;
      cfg.freq_write  = 40000000;
      cfg.freq_read   = 16000000;
      cfg.spi_3wire   = false;
      cfg.use_lock    = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk    = 14;
      cfg.pin_mosi    = 13;
      cfg.pin_miso    = 12;
      cfg.pin_dc      = 2;
      _bus.config(cfg);
    }
    {
      auto cfg = _light.config();
      cfg.pin_bl      = 21;
      cfg.invert      = false;
      cfg.freq        = 44100;
      cfg.pwm_channel = 7;
      _light.config(cfg);
    }
    setupPanel(_ili);
    setupPanel(_st);
    setPanel(&_ili);
  }

  void selectPanel(bool st7789) {
    if (st7789) setPanel(&_st);
    else        setPanel(&_ili);
  }
};

static LGFX tft;
static Preferences prefs;
static WebServer server(80);
static DNSServer dns;

int SCR_W = 240;
int SCR_H = 320;

// ============================================================================
//  FARBEN (RGB565 - immer als uint16_t uebergeben!)
// ============================================================================
constexpr uint16_t RGB(uint8_t r, uint8_t g, uint8_t b) {
  return (uint16_t)(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}
constexpr uint16_t C_BG     = RGB(8, 8, 20);
constexpr uint16_t C_PANEL  = RGB(20, 20, 45);
constexpr uint16_t C_GRID   = RGB(30, 30, 60);
constexpr uint16_t C_WHITE  = RGB(255, 255, 255);
constexpr uint16_t C_GRAY   = RGB(120, 120, 140);
constexpr uint16_t C_DIM    = RGB(70, 70, 95);
constexpr uint16_t C_YELLOW = RGB(255, 220, 0);
constexpr uint16_t C_RED    = RGB(240, 40, 40);
constexpr uint16_t C_GREEN  = RGB(40, 220, 80);
constexpr uint16_t C_DGREEN = RGB(20, 150, 50);
constexpr uint16_t C_CYAN   = RGB(0, 220, 230);
constexpr uint16_t C_BLUE   = RGB(40, 90, 255);
constexpr uint16_t C_ORANGE = RGB(255, 140, 0);
constexpr uint16_t C_PURPLE = RGB(170, 60, 230);
constexpr uint16_t C_PINK   = RGB(255, 80, 180);

// ============================================================================
//  TEXT-HELFER  (Standardschrift hat keine Umlaute -> ASCII verwenden)
// ============================================================================
void txt(const char* s, int x, int y, float size, uint16_t fg, uint16_t bg,
         uint8_t datum = textdatum_t::middle_center) {
  tft.setTextDatum(datum);
  tft.setTextSize(size);
  tft.setTextColor(fg, bg);
  tft.drawString(s, x, y);
}

void hud(const char* s, int x, int y, int w, float size, uint16_t fg, uint16_t bg) {
  tft.fillRect(x, y, w, (int)(8 * size), bg);
  txt(s, x, y, size, fg, bg, textdatum_t::top_left);
}

void hudR(const char* s, int xr, int y, int w, float size, uint16_t fg, uint16_t bg) {
  tft.fillRect(xr - w, y, w, (int)(8 * size), bg);
  txt(s, xr, y, size, fg, bg, textdatum_t::top_right);
}

void drawStripes(int y0) {
  const uint16_t st[] = {C_RED, C_ORANGE, C_YELLOW, C_GREEN, C_CYAN, C_PURPLE};
  for (int i = 0; i < 6; i++) tft.fillRect(0, y0 + i * 3, SCR_W, 2, st[i]);
}

// ============================================================================
//  EINSTELLUNGEN
// ============================================================================
struct Settings {
  uint8_t rot = 1;          // 1 quer (Standard), 3 quer 180 Grad, 0/2 hochkant
  bool    mirror = false;
  bool    invert = false;
  bool    st7789 = false;
  uint8_t bright = 200;
  bool    rumbleOn = true;
  uint8_t rumbleStr = 160;
  uint8_t snakeSpeed = 1;   // 0 gemuetlich, 1 normal, 2 schnell
  bool    wifiAlways = false;
  bool    allowNew = true;
  String  ssid, pass;
  String  apPass = "retro1234";
} cfg;

void loadSettings() {
  cfg.rot        = prefs.getUChar("rot", 1) & 3;
  // Ab Version 3.1 ist Querformat Standard: eine frueher gespeicherte
  // Hochkant-Einstellung wird einmalig auf Querformat umgestellt.
  if (prefs.getUChar("layoutVer", 0) < 31) {
    if (cfg.rot == 0 || cfg.rot == 2) cfg.rot = 1;
    prefs.putUChar("rot", cfg.rot);
    prefs.putUChar("layoutVer", 31);
  }
  cfg.mirror     = prefs.getBool("mirror", false);
  cfg.invert     = prefs.getBool("invert", false);
  cfg.st7789     = prefs.getBool("st7789", false);
  cfg.bright     = prefs.getUChar("bright", 200);
  cfg.rumbleOn   = prefs.getBool("rumOn", true);
  cfg.rumbleStr  = prefs.getUChar("rumStr", 160);
  cfg.snakeSpeed = prefs.getUChar("snkSpd", 1);
  cfg.wifiAlways = prefs.getBool("wifiAlw", false);
  cfg.allowNew   = prefs.getBool("allowNew", true);
  cfg.ssid       = prefs.getString("ssid", "");
  cfg.pass       = prefs.getString("pass", "");
  cfg.apPass     = prefs.getString("apPass", "retro1234");
  if (cfg.bright < 10) cfg.bright = 10;
  if (cfg.snakeSpeed > 2) cfg.snakeSpeed = 1;
}

void saveSettings() {
  prefs.putUChar("rot", cfg.rot);
  prefs.putBool("mirror", cfg.mirror);
  prefs.putBool("invert", cfg.invert);
  prefs.putBool("st7789", cfg.st7789);
  prefs.putUChar("bright", cfg.bright);
  prefs.putBool("rumOn", cfg.rumbleOn);
  prefs.putUChar("rumStr", cfg.rumbleStr);
  prefs.putUChar("snkSpd", cfg.snakeSpeed);
  prefs.putBool("wifiAlw", cfg.wifiAlways);
  prefs.putBool("allowNew", cfg.allowNew);
  prefs.putString("ssid", cfg.ssid);
  prefs.putString("pass", cfg.pass);
  prefs.putString("apPass", cfg.apPass);
}

void applyDisplay() {
  tft.setRotation((cfg.rot & 3) | (cfg.mirror ? 4 : 0));   // 4..7 = gespiegelt
  tft.invertDisplay(cfg.invert);
  tft.setBrightness(cfg.bright);
  SCR_W = tft.width();
  SCR_H = tft.height();
}

bool landscape() { return SCR_W > SCR_H; }

// ============================================================================
//  CONTROLLER (Bluepad32)
// ============================================================================
ControllerPtr pads[BP32_MAX_GAMEPADS];

bool btQuietNeeded();   // im WLAN-Teil definiert

void onConnected(ControllerPtr c) {
  if (btQuietNeeded()) {   // eigenes WLAN aktiv: Bluetooth ruhig halten
    Serial.println("Controller abgewiesen (Setup-WLAN aktiv)");
    c->disconnect();
    return;
  }
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (pads[i] == nullptr) {
      pads[i] = c;
      Serial.printf("Controller %d verbunden: %s\n", i, c->getModelName().c_str());
      break;
    }
  }
}

void onDisconnected(ControllerPtr c) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++)
    if (pads[i] == c) pads[i] = nullptr;
}

int connectedPads() {
  int n = 0;
  for (auto c : pads) if (c && c->isConnected()) n++;
  return n;
}

struct Keys {
  bool up, down, left, right, a, b, x, y, start, back;
  int16_t ax, ay;   // linker Stick (-512..512)

  void read(ControllerPtr c) {
    uint8_t d = c->dpad();
    int sx = c->axisX(), sy = c->axisY();
    up    = (d & DPAD_UP)    != 0 || sy < -300;
    down  = (d & DPAD_DOWN)  != 0 || sy >  300;
    left  = (d & DPAD_LEFT)  != 0 || sx < -300;
    right = (d & DPAD_RIGHT) != 0 || sx >  300;
    a = c->a(); b = c->b(); x = c->x(); y = c->y();
    uint16_t m = c->miscButtons();
    start = (m & MISC_BUTTON_START)  != 0;   // Xbox "Menu"-Taste
    back  = (m & MISC_BUTTON_SELECT) != 0;   // Xbox "View"-Taste
    ax = sx; ay = sy;
  }
  void merge(const Keys& k) {
    up |= k.up; down |= k.down; left |= k.left; right |= k.right;
    a |= k.a; b |= k.b; x |= k.x; y |= k.y; start |= k.start; back |= k.back;
    if (abs(k.ax) > abs(ax)) ax = k.ax;
    if (abs(k.ay) > abs(ay)) ay = k.ay;
  }
  // Tasten, die zwischen p (vorher) und c (jetzt) neu gedrueckt wurden, merken
  void latch(const Keys& c, const Keys& p) {
    up |= c.up && !p.up; down |= c.down && !p.down;
    left |= c.left && !p.left; right |= c.right && !p.right;
    a |= c.a && !p.a; b |= c.b && !p.b; x |= c.x && !p.x; y |= c.y && !p.y;
    start |= c.start && !p.start; back |= c.back && !p.back;
  }
};

// cur/prv: alle Controller zusammen.  pk[0]/pk[1]: Spieler 1 und 2 einzeln
// (in der Reihenfolge, in der sie verbunden wurden).
// E / PE[]: seit dem letzten Spieltakt neu gedrueckte Tasten (fuer 30-Hz-Spiele)
Keys cur{}, prv{};
Keys pk[2]{}, pkPrev[2]{};
Keys E{}, PE[2]{};
int numPlayers = 0;

#define PRESSED(k) (cur.k && !prv.k)

void touchKeys(Keys& k);     // Touch-Steuerung (weiter unten)
bool touchActive();

void readInput() {
  prv = cur;
  pkPrev[0] = pk[0]; pkPrev[1] = pk[1];
  cur = Keys{}; pk[0] = Keys{}; pk[1] = Keys{};
  int slot = 0;
  for (auto c : pads) {
    if (!c || !c->isConnected() || !c->isGamepad()) continue;
    Keys k{};
    k.read(c);
    cur.merge(k);
    if (slot < 2) pk[slot] = k;
    slot++;
  }
  numPlayers = slot;
  if (touchActive()) {                       // kein Controller: Bildschirm als Steuerkreuz
    Keys t{};
    touchKeys(t);
    cur.merge(t);
    pk[0].merge(t);
  }
  E.latch(cur, prv);
  PE[0].latch(pk[0], pkPrev[0]);
  PE[1].latch(pk[1], pkPrev[1]);
}

void clearEdges() { E = Keys{}; PE[0] = Keys{}; PE[1] = Keys{}; }

// Vibration nur fuer einen Spieler (0/1); -1 = alle
void rumbleFor(int player, uint16_t ms, uint8_t strength);

void rumbleFor(int player, uint16_t ms, uint8_t strength) {
  if (!cfg.rumbleOn) return;
  uint8_t s = (uint8_t)min(255, (int)strength * cfg.rumbleStr / 160);
  int slot = 0;
  for (auto c : pads) {
    if (!c || !c->isConnected() || !c->isGamepad()) continue;
    if (player < 0 || player == slot) c->playDualRumble(0, ms, s, s);
    slot++;
  }
}

void rumble(uint16_t ms, uint8_t strength = 0x80) { rumbleFor(-1, ms, strength); }

struct Repeat {
  uint32_t next = 0;
  bool held = false;
  bool tick(bool down, uint16_t first, uint16_t rep) {
    uint32_t now = millis();
    if (!down) { held = false; return false; }
    if (!held) { held = true; next = now + first; return true; }
    if ((int32_t)(now - next) >= 0) { next = now + rep; return true; }
    return false;
  }
};

// ============================================================================
//  TOUCH + BOOT-TASTE  (nur "beruehrt ja/nein" - keine Kalibrierung noetig)
// ============================================================================
#define T_CLK  25
#define T_MOSI 32
#define T_MISO 39
#define T_CS   33
#define BOOT_PIN 0

void tcalLoad();
void touchInit() {
  cyd::touchBegin();
  tcalLoad();
  pinMode(T_CLK, OUTPUT);
  pinMode(T_MOSI, OUTPUT);
  pinMode(T_CS, OUTPUT);
  pinMode(T_MISO, INPUT);
  digitalWrite(T_CS, HIGH);
  digitalWrite(T_CLK, LOW);
  pinMode(BOOT_PIN, INPUT_PULLUP);
}

uint8_t tXfer(uint8_t out) {
  uint8_t in = 0;
  for (int i = 7; i >= 0; i--) {
    digitalWrite(T_MOSI, (out >> i) & 1);
    digitalWrite(T_CLK, HIGH);
    in |= (digitalRead(T_MISO) ? 1 : 0) << i;
    digitalWrite(T_CLK, LOW);
  }
  return in;
}

uint16_t tRead(uint8_t cmd) {
  tXfer(cmd);
  uint16_t hi = tXfer(0), lo = tXfer(0);
  return (uint16_t)((((hi << 8) | lo) >> 3) & 0x0FFF);
}

bool touchDown() {
  digitalWrite(T_CS, LOW);
  uint16_t z1 = tRead(0xB1);
  uint16_t z2 = tRead(0xC1);
  tRead(0xD0);                       // zurueck in Stromsparmodus
  digitalWrite(T_CS, HIGH);
  if (z1 >= 4000) return false;      // kein Touch-Chip / Leitung offen
  int z = (int)z1 + 4095 - (int)z2;
  return z > 400;
}

bool bootDown() { return digitalRead(BOOT_PIN) == LOW; }

bool touchPrev = false;
int tPauseW = 52, tPauseH = 40;   // Touch-Steuerung: Ecke oben rechts = Menue (Spiele koennen sie verkleinern)
bool gameRestart = false;         // Spiel wurde vom Spielende aus neu gestartet (gleiche Spielart)
uint32_t touchSince = 0;
bool touchFired = false;

// ============================================================================
//  ZUSTAENDE / SPIELELISTE / HIGHSCORES
// ============================================================================
enum State { ST_MENU, ST_GAME, ST_GAMEOVER, ST_SETUP };
State state = ST_MENU;

#define NUM_GAMES 20
struct GameDef {
  const char* name;     // Anzeigename (ASCII, ohne Umlaute)
  const char* hsKey;    // Schluessel fuer den Highscore im Flash (max. 15 Zeichen)
  const char* info;     // kurze Beschreibung fuer die Web-Oberflaeche
  void (*start)();
  void (*loop)();
  bool fb;              // nutzt den flimmerfreien Bildpuffer
};
extern const GameDef GAMES[NUM_GAMES];
uint32_t highscore[NUM_GAMES];
int curGame = 0;

bool paused = false;
uint32_t overGuardUntil = 0;
uint32_t restartAt = 0;

void toMenu();
void startGame(int g);
void enterSetup();
void drawSetup();

// ============================================================================
//  MICRO-SD
// ============================================================================
#define SD_CS   5
#define SD_SCK  18
#define SD_MISO 19
#define SD_MOSI 23

SPIClass sdSPI(VSPI);
bool sdOk = false;

const char* SD_DIR     = "/retrocyd";
const char* SD_HSFILE  = "/retrocyd/highscores.txt";
const char* SD_LVLDIR  = "/retrocyd/breakout";
const char* SD_SHOTDIR = "/retrocyd/shots";
const char* SD_UPDFILE = "/retrocyd/update.bin";
const char* SD_UPDDONE = "/retrocyd/update_installiert.bin";
const char* SD_WIFI    = "/retrocyd/wlan.txt";
const char* SD_WIFIOK  = "/retrocyd/wlan_uebernommen.txt";

// Heimnetz per SD-Karte einrichten (ohne Web-Oberflaeche):
//   /retrocyd/wlan.txt  mit den Zeilen  ssid=MeinWLAN  und  pass=MeinPasswort
// Die Datei wird beim Start uebernommen und umbenannt; das Passwort
// wird darin unkenntlich gemacht.
void sdImportWifi() {
  if (!SD.exists(SD_WIFI)) return;
  File f = SD.open(SD_WIFI, FILE_READ);
  if (!f) return;
  String ssid, pass;
  while (f.available()) {
    String line = f.readStringUntil('\n');
    line.trim();
    if (line.startsWith("ssid=")) ssid = line.substring(5);
    else if (line.startsWith("pass=")) pass = line.substring(5);
  }
  f.close();
  if (ssid.length() == 0) return;
  cfg.ssid = ssid;
  cfg.pass = pass;
  prefs.putString("ssid", cfg.ssid);
  prefs.putString("pass", cfg.pass);
  SD.remove(SD_WIFI);
  if (SD.exists(SD_WIFIOK)) SD.remove(SD_WIFIOK);
  File o = SD.open(SD_WIFIOK, FILE_WRITE);
  if (o) { o.printf("ssid=%s\npass=********\n", ssid.c_str()); o.close(); }
  Serial.printf("WLAN von SD uebernommen: %s\n", ssid.c_str());
}

void sdWriteText(const char* path, const char* text) {
  File f = SD.open(path, FILE_WRITE);
  if (!f) return;
  f.print(text);
  f.close();
}

void sdCreateExampleLevels() {
  const char* lv[3] = {
    "1111111111\n2222222222\n3333333333\n4444444444\n5555555555\n6666666666\n",
    "1........1\n22......22\n333....333\n4444..4444\n5555555555\n6.6.6.6.6.\n",
    "....11....\n...2222...\n..333333..\n.44444444.\n5555555555\n.6.6..6.6.\n",
  };
  char path[48];
  for (int i = 0; i < 3; i++) {
    snprintf(path, sizeof(path), "%s/level%d.txt", SD_LVLDIR, i + 1);
    sdWriteText(path, lv[i]);
  }
}

bool sdInit() {
  sdSPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  if (!SD.begin(SD_CS, sdSPI, 20000000)) {
    Serial.println("SD: keine Karte gefunden");
    return false;
  }
  if (!SD.exists(SD_DIR))     SD.mkdir(SD_DIR);
  if (!SD.exists(SD_SHOTDIR)) SD.mkdir(SD_SHOTDIR);
  if (!SD.exists(SD_LVLDIR)) {
    SD.mkdir(SD_LVLDIR);
    sdCreateExampleLevels();
  }
  return true;
}

void sdLoadHighscores() {
  if (!sdOk) return;
  File f = SD.open(SD_HSFILE, FILE_READ);
  if (!f) return;
  while (f.available()) {
    String line = f.readStringUntil('\n');
    line.trim();
    int eq = line.indexOf('=');
    if (eq < 0) continue;
    String name = line.substring(0, eq);
    uint32_t val = (uint32_t)line.substring(eq + 1).toInt();
    for (int i = 0; i < NUM_GAMES; i++)
      if (name.equalsIgnoreCase(GAMES[i].name) && val > highscore[i]) highscore[i] = val;
  }
  f.close();
}

void sdSaveHighscores() {
  if (!sdOk) return;
  File f = SD.open(SD_HSFILE, FILE_WRITE);
  if (!f) { sdOk = false; return; }
  for (int i = 0; i < NUM_GAMES; i++)
    f.printf("%s=%lu\n", GAMES[i].name, (unsigned long)highscore[i]);
  f.close();
}

void saveHighscores() {
  for (int i = 0; i < NUM_GAMES; i++) prefs.putUInt(GAMES[i].hsKey, highscore[i]);
  sdSaveHighscores();
}

void sdScreenshot() {
  if (!sdOk) return;
  char path[48];
  int n = 0;
  do {
    snprintf(path, sizeof(path), "%s/shot_%03d.bmp", SD_SHOTDIR, n++);
  } while (SD.exists(path) && n < 1000);

  File f = SD.open(path, FILE_WRITE);
  if (!f) { sdOk = false; return; }

  const int W = SCR_W, H = SCR_H;
  const uint32_t rowBytes = W * 3;      // 960 bzw. 720 - durch 4 teilbar
  const uint32_t dataSize = rowBytes * H;
  uint8_t hdr[54] = {0};
  auto put32 = [&](int o, uint32_t v) { for (int i = 0; i < 4; i++) hdr[o + i] = (v >> (8 * i)) & 0xFF; };
  hdr[0] = 'B'; hdr[1] = 'M';
  put32(2, 54 + dataSize);
  put32(10, 54);
  put32(14, 40);
  put32(18, W);
  put32(22, H);
  hdr[26] = 1; hdr[28] = 24;
  put32(34, dataSize);
  f.write(hdr, 54);

  // Zeilenpuffer nur waehrend des Screenshots belegen (spart festen Arbeitsspeicher)
  lgfx::rgb565_t* px = (lgfx::rgb565_t*)malloc(320 * sizeof(lgfx::rgb565_t));
  uint8_t* row = (uint8_t*)malloc(320 * 3);
  if (!px || !row) { free(px); free(row); f.close(); return; }
  for (int y = H - 1; y >= 0; y--) {
    tft.readRect(0, y, W, 1, px);
    for (int x = 0; x < W; x++) {
      row[x * 3 + 0] = px[x].B8();
      row[x * 3 + 1] = px[x].G8();
      row[x * 3 + 2] = px[x].R8();
    }
    f.write(row, rowBytes);
  }
  free(px);
  free(row);
  f.close();
  Serial.printf("Screenshot: %s\n", path);
  rumble(120, 0x60);
}

// ============================================================================
//  HALTE-MENUE  (Bildschirm oder BOOT lange halten)
//  Bedienung ohne Koordinaten: tippen = naechster Punkt, halten = waehlen.
//  Mit Controller: hoch/runter, A = waehlen, B = zurueck.
// ============================================================================
bool holdOpen = false;       // Menue ist offen
bool holdReq = false;        // im Spiel angefordert (wird in handlePause geoeffnet)
bool holdInGame = false;
int holdSel = 0, holdN = 0;
const char* holdItems[4];
const char* holdIds[4];
bool holdT = false, holdWait = true;
uint32_t holdT0 = 0, holdAct = 0;

void toClock();
void fbEnd();
void wifiStop();
extern bool wifiOn;

const char* holdId(int i) { return (i >= 0 && i < holdN) ? holdIds[i] : "back"; }

// true, sobald Bildschirm oder BOOT 1,5 s gehalten wurden (einmal pro Beruehrung)
bool longPress() {
  bool t = (touchActive() ? false : touchDown()) || bootDown();   // im Spiel mit Touch-Steuerung: nur BOOT
  if (t && !touchPrev) { touchSince = millis(); touchFired = false; }
  touchPrev = t;
  if (t && !touchFired && millis() - touchSince > 1500) { touchFired = true; return true; }
  return false;
}

void holdDraw() {
  int w = 220, ih = 30, h = holdN * ih + 62;
  int x = (SCR_W - w) / 2, y = (SCR_H - h) / 2;
  tft.fillRoundRect(x, y, w, h, 10, C_PANEL);
  tft.drawRoundRect(x, y, w, h, 10, C_YELLOW);
  txt("MENUE", SCR_W / 2, y + 16, 2, C_YELLOW, C_PANEL);
  for (int i = 0; i < holdN; i++) {
    int by = y + 32 + i * ih;
    bool sel = (i == holdSel);
    uint16_t bg = sel ? (!strcmp(holdIds[i], "clock") ? C_DGREEN : C_BLUE) : C_BG;
    tft.fillRoundRect(x + 10, by, w - 20, ih - 4, 5, bg);
    if (sel) tft.drawRoundRect(x + 10, by, w - 20, ih - 4, 5, C_WHITE);
    txt(holdItems[i], SCR_W / 2, by + (ih - 4) / 2 + 1, 1.5, sel ? C_WHITE : C_GRAY, bg);
  }
  txt("Tippen = weiter   Halten = waehlen", SCR_W / 2, y + h - 12, 1, C_GRAY, C_PANEL);
}

void holdOpenMenu(bool inGame) {
  holdInGame = inGame;
  holdN = 0;
  if (clockPresent) { holdItems[holdN] = "ZUR UHR"; holdIds[holdN++] = "clock"; }
  if (inGame) {
    holdItems[holdN] = "SPIELMENUE";    holdIds[holdN++] = "menu";
    holdItems[holdN] = "WEITERSPIELEN"; holdIds[holdN++] = "back";
  } else {
    holdItems[holdN] = "SETUP / WLAN";  holdIds[holdN++] = "setup";
    holdItems[holdN] = "TOUCH EINRICHTEN"; holdIds[holdN++] = "tcal";
    holdItems[holdN] = "ZURUECK";       holdIds[holdN++] = "back";
  }
  holdSel = 0;
  holdOpen = true;
  holdWait = true;                     // Finger vom langen Halten erst loslassen
  holdT = false;
  holdAct = millis();
  if (inGame) paused = true;
  holdDraw();
}

// -1 = noch offen, sonst gewaehlter Punkt (Menue ist dann geschlossen)
int holdTick() {
  uint32_t now = millis();
  bool t = touchDown() || bootDown();
  int pick = -1;
  if (holdWait) { if (!t) holdWait = false; }
  else if (t && !holdT) { holdT = true; holdT0 = now; }
  else if (t && holdT && now - holdT0 > 800) { pick = holdSel; holdWait = true; holdT = false; }   // halten = waehlen
  else if (!t && holdT) { holdT = false; holdSel = (holdSel + 1) % holdN; holdAct = now; holdDraw(); }  // tippen = weiter
  if (pick < 0) {
    if (PRESSED(up))   { holdSel = (holdSel + holdN - 1) % holdN; holdAct = now; holdDraw(); }
    if (PRESSED(down)) { holdSel = (holdSel + 1) % holdN; holdAct = now; holdDraw(); }
    if (PRESSED(a) || PRESSED(start)) pick = holdSel;
    if (PRESSED(b) || PRESSED(back)) pick = holdN - 1;          // letzter Punkt = zurueck/weiterspielen
    if (t) holdAct = now;
    if (now - holdAct > 15000) pick = holdN - 1;               // 15 s nichts: schliessen
  }
  if (pick < 0) return -1;
  holdOpen = false;
  touchPrev = true;                    // erst loslassen, dann zaehlt die naechste Beruehrung
  touchFired = true;
  clearEdges();
  return pick;
}

// Zur Uhr wechseln: Start-Partition steht schon auf der Uhr (siehe setup), nur neu starten
void toClock() {
  if (!clockPresent) return;
  fbEnd();
  tft.fillScreen(C_BG);
  txt("UHR STARTET...", SCR_W / 2, SCR_H / 2, 2, C_GREEN, C_BG);
  if (wifiOn) wifiStop();
  cyd::bootOther();
  delay(300);
  ESP.restart();
}

// ============================================================================
//  PAUSE / GAME OVER
// ============================================================================
bool handlePause(void (*redrawAll)()) {
  if (holdOpen) {                                  // Halte-Menue im Spiel
    int c = holdTick();
    if (c < 0) return true;
    const char* id = holdId(c);
    if (!strcmp(id, "clock")) { toClock(); return true; }
    if (!strcmp(id, "menu")) { paused = false; toMenu(); return true; }
    paused = false;                                // weiterspielen
    clearEdges();
    redrawAll();
    return true;
  }
  if (holdReq) { holdReq = false; holdOpenMenu(true); return true; }
  if (PRESSED(back)) { paused = false; toMenu(); return true; }
  if (PRESSED(start)) {
    paused = !paused;
    if (paused) {
      int x = SCR_W / 2 - 70, y = SCR_H / 2 - 25;
      tft.fillRoundRect(x, y, 140, 50, 8, C_PANEL);
      tft.drawRoundRect(x, y, 140, 50, 8, C_YELLOW);
      txt("PAUSE", SCR_W / 2, y + 18, 2, C_YELLOW, C_PANEL);
      txt("View = Menu", SCR_W / 2, y + 38, 1, C_GRAY, C_PANEL);
    } else {
      clearEdges();
      redrawAll();
    }
  }
  return paused;
}

void gameOverText(const char* title, uint32_t score) {
  bool record = score > highscore[curGame];
  if (record) {
    highscore[curGame] = score;
    saveHighscores();
  }
  rumble(400, 0xC0);
  int cx = SCR_W / 2, y = SCR_H / 2 - 50;
  tft.fillRoundRect(cx - 100, y, 200, 100, 10, C_PANEL);
  tft.drawRoundRect(cx - 100, y, 200, 100, 10, record ? C_YELLOW : C_RED);
  txt(title, cx, y + 20, 2, record ? C_YELLOW : C_RED, C_PANEL);
  char buf[32];
  snprintf(buf, sizeof(buf), "Punkte: %lu", (unsigned long)score);
  txt(buf, cx, y + 45, 1.5, C_WHITE, C_PANEL);
  if (record) txt("NEUER REKORD!", cx, y + 63, 1, C_YELLOW, C_PANEL);
  txt("A: Neustart   B: Menu", cx, y + 83, 1, C_GRAY, C_PANEL);
  overGuardUntil = millis() + 900;
  state = ST_GAMEOVER;
}

void gameOver(uint32_t score) { gameOverText("GAME OVER", score); }

bool touchTapped();

void gameOverLoop() {
  if (holdOpen) {
    int c = holdTick();
    if (c < 0) return;
    if (!strcmp(holdId(c), "clock")) toClock(); else toMenu();
    return;
  }
  if (longPress()) { holdOpenMenu(true); return; }
  if ((int32_t)(millis() - overGuardUntil) < 0) { prv = cur; return; }
  if (connectedPads() == 0 && touchTapped()) { startGame(curGame); return; }   // Touch: antippen = nochmal
  if (PRESSED(a) || PRESSED(start)) startGame(curGame);
  else if (PRESSED(b) || PRESSED(back)) toMenu();
}

#include "core_fb.h"

// ============================================================================
//  MENUE  (Kategorien als Kacheln -> Spiele der Kategorie als Liste)
// ============================================================================
#define NUM_CATS 6
struct Category { const char* name; uint16_t col; uint8_t n; uint8_t games[4]; };
extern const Category CATS[NUM_CATS];

int menuLevel = 0;       // 0 = Kategorien, 1 = Spiele einer Kategorie
int menuCat = 0;
int menuSel = 0;         // Auswahl in der aktuellen Ebene
int menuTop = 0;
int lastPadCount = -1;
bool lastWifi = false;
Repeat menuRepUp, menuRepDown;
extern bool wifiOn;
extern String ipStr;

// Ebene 0: Kategorien, dann SETUP (und ZUR UHR)
int menuItems() { return menuLevel == 0 ? NUM_CATS + 1 + (clockPresent ? 1 : 0) : CATS[menuCat].n + 1; }
// Ebene 1: Spielnummer des Listenpunkts, -1 = ZURUECK
int menuGame(int i) { return (menuLevel == 1 && i < CATS[menuCat].n) ? CATS[menuCat].games[i] : -1; }

void menuLayout(int& top, int& itemH, int& visible) {
  top = landscape() ? 66 : 90;
  itemH = 26;
  visible = (SCR_H - 34 - top) / itemH;
  if (visible < 1) visible = 1;
}

// Kachel i der Kategorie-Ebene (2 Spalten)
void catTile(int i, int& x, int& y, int& w, int& h) {
  const int top = landscape() ? 64 : 90;
  const int rows = (menuItems() + 1) / 2;
  const int avail = SCR_H - 34 - top;
  const int tw = min(300, SCR_W - 16);
  h = min(44, avail / rows - 5);
  w = (tw - 6) / 2;
  x = (SCR_W - tw) / 2 + (i & 1) * (w + 6);
  y = top + (i / 2) * (h + 5);
}

void drawCatTile(int i) {
  int x, y, w, h;
  catTile(i, x, y, w, h);
  const bool sel = i == menuSel;
  const bool isCat = i < NUM_CATS, isSetup = i == NUM_CATS;
  const uint16_t col = isCat ? CATS[i].col : (isSetup ? C_PURPLE : C_DGREEN);
  const uint16_t bg = sel ? RGB(40, 40, 85) : C_PANEL;
  tft.fillRoundRect(x, y, w, h, 6, bg);
  tft.fillRoundRect(x, y, 7, h, 3, col);
  if (sel) tft.drawRoundRect(x, y, w, h, 6, C_WHITE);
  const char* name = isCat ? CATS[i].name : (isSetup ? "SETUP / WLAN" : "ZUR UHR");
  const float ts = strlen(name) * 9 <= w - 16 ? 1.5f : 1.0f;
  if (isCat) {
    txt(name, x + 13, y + h / 2 - 5, ts, sel ? C_WHITE : RGB(200, 200, 215), bg, textdatum_t::middle_left);
    char buf[16];
    snprintf(buf, sizeof(buf), "%d SPIELE", CATS[i].n);
    txt(buf, x + 13, y + h / 2 + 8, 1, col, bg, textdatum_t::middle_left);
  } else {
    txt(name, x + 13, y + h / 2 + 1, ts, sel ? C_WHITE : RGB(200, 200, 215), bg, textdatum_t::middle_left);
  }
}

void drawMenuItems() {
  int top, itemH, visible;
  menuLayout(top, itemH, visible);
  const int n = menuItems();
  if (menuSel >= n) menuSel = n - 1;
  if (menuLevel == 0) {
    tft.fillRect(0, top - 4, SCR_W, SCR_H - 32 - top + 4, C_BG);
    for (int i = 0; i < n; i++) drawCatTile(i);
    return;
  }
  if (menuSel < menuTop) menuTop = menuSel;
  if (menuSel >= menuTop + visible) menuTop = menuSel - visible + 1;
  if (menuTop > max(0, n - visible)) menuTop = max(0, n - visible);

  int w = min(250, SCR_W - 24);
  int x = (SCR_W - w) / 2;
  char buf[24];
  tft.fillRect(0, top - 4, SCR_W, visible * itemH + 4, C_BG);
  const uint16_t cc = CATS[menuCat].col;
  for (int v = 0; v < visible && menuTop + v < n; v++) {
    int i = menuTop + v;
    int y = top + v * itemH;
    int h = itemH - 4;
    bool sel = (i == menuSel);
    const int g = menuGame(i);
    uint16_t bg = sel ? (g < 0 ? C_PURPLE : C_BLUE) : C_PANEL;
    tft.fillRoundRect(x, y, w, h, 5, bg);
    if (sel) tft.drawRoundRect(x, y, w, h, 5, C_WHITE);
    if (g >= 0) tft.fillRect(x + 3, y + 4, 3, h - 8, cc);
    const char* name = g < 0 ? "< ZURUECK" : GAMES[g].name;
    txt(name, x + 12, y + h / 2 + 1, 1.5, sel ? C_WHITE : C_GRAY, bg, textdatum_t::middle_left);
    if (g >= 0) {
      snprintf(buf, sizeof(buf), "%lu", (unsigned long)highscore[g]);
      txt(buf, x + w - 8, y + h / 2 + 1, 1, sel ? C_YELLOW : C_DIM, bg, textdatum_t::middle_right);
    }
  }
  // Scroll-Pfeile
  uint16_t up = menuTop > 0 ? C_YELLOW : C_BG;
  uint16_t dn = menuTop + visible < n ? C_YELLOW : C_BG;
  int ax = SCR_W - 6;
  tft.fillTriangle(ax - 4, top + 6, ax + 4, top + 6, ax, top, up);
  int by = top + visible * itemH - 10;
  tft.fillTriangle(ax - 4, by, ax + 4, by, ax, by + 6, dn);
}

void drawMenuStatus() {
  int n = connectedPads();
  int y1 = SCR_H - 24, y2 = SCR_H - 10;
  tft.fillRect(0, SCR_H - 32, SCR_W, 32, C_BG);
  tft.fillCircle(14, y1, 4, n ? C_GREEN : C_RED);
  char buf[32];
  if (n > 1) snprintf(buf, sizeof(buf), "%d Controller verbunden", n);
  else snprintf(buf, sizeof(buf), n ? "Controller verbunden" : "ohne Controller: Touch");
  txt(buf, 24, y1, 1, n ? C_GREEN : C_GRAY, C_BG, textdatum_t::middle_left);
  if (wifiOn) txt(ipStr.c_str(), SCR_W - 8, y1, 1, C_CYAN, C_BG, textdatum_t::middle_right);
  tft.fillCircle(14, y2, 4, sdOk ? C_GREEN : C_DIM);
  txt(sdOk ? "SD-Karte OK" : "keine SD", 24, y2, 1, sdOk ? C_GREEN : C_DIM, C_BG,
      textdatum_t::middle_left);
  txt("Touch halten: Menue", SCR_W - 8, y2, 1, C_DIM, C_BG, textdatum_t::middle_right);
  lastPadCount = n;
  lastWifi = wifiOn;
}

void drawMenu() {
  tft.fillScreen(C_BG);
  drawStripes(6);
  char sub[40];
  if (menuLevel == 0) snprintf(sub, sizeof(sub), "%d SPIELE IN %d KATEGORIEN", NUM_GAMES, NUM_CATS);
  else snprintf(sub, sizeof(sub), "%s", CATS[menuCat].name);
  const uint16_t sc = menuLevel == 0 ? C_CYAN : CATS[menuCat].col;
  if (landscape()) {
    txt("RETRO CYD", SCR_W / 2 + 2, 37, 3, C_DIM, C_BG);
    txt("RETRO CYD", SCR_W / 2, 35, 3, C_YELLOW, C_BG);
    txt(sub, 8, 56, 1, sc, C_BG, textdatum_t::middle_left);
  } else {
    txt("RETRO CYD", SCR_W / 2 + 2, 48, 4, C_DIM, C_BG);
    txt("RETRO CYD", SCR_W / 2, 46, 4, C_YELLOW, C_BG);
    txt(sub, 12, 76, 1, sc, C_BG, textdatum_t::middle_left);
  }
  drawMenuItems();
  drawMenuStatus();
}

// Kategorie oeffnen bzw. zurueck zu den Kategorien
void menuOpenCat(int c) { menuLevel = 1; menuCat = c; menuSel = 0; menuTop = 0; drawMenu(); }
void menuBack() { menuLevel = 0; menuSel = menuCat; menuTop = 0; drawMenu(); }

// Punkt i der aktuellen Ebene ausfuehren
void menuPick(int i) {
  if (menuLevel == 0) {
    if (i < NUM_CATS) menuOpenCat(i);
    else if (i == NUM_CATS) enterSetup();
    else toClock();
    return;
  }
  const int g = menuGame(i);
  if (g < 0) menuBack(); else startGame(g);
}

void Kit_silence();
void toMenu() {
  fbEnd();
  gameMemEnd();
  Kit_silence();
  state = ST_MENU;
  paused = false;
  // nach einem Spiel: dessen Kategorie mit dem Spiel ausgewaehlt zeigen
  for (int c = 0; c < NUM_CATS; c++)
    for (int k = 0; k < CATS[c].n; k++)
      if (CATS[c].games[k] == curGame && menuLevel == 1 && menuCat == c) menuSel = k;
  drawMenu();
}

bool menuTouch();
void touchCalibrate();

void menuLoop() {
  // Touch oder BOOT 1,5 s halten -> Halte-Menue (ZUR UHR / SETUP / ZURUECK), geht auch ohne Controller
  if (holdOpen) {
    int c = holdTick();
    if (c < 0) return;
    const char* id = holdId(c);
    if (!strcmp(id, "clock")) toClock();
    else if (!strcmp(id, "setup")) enterSetup();
    else if (!strcmp(id, "tcal")) { touchCalibrate(); drawMenu(); }
    else drawMenu();
    return;
  }
  if (longPress()) { holdOpenMenu(false); return; }
  if (menuTouch()) return;                 // Kategorie / Spiel antippen

  if (connectedPads() != lastPadCount || wifiOn != lastWifi) drawMenuStatus();
  const int n = menuItems();
  if (menuLevel == 0) {                    // Kacheln: 2 Spalten
    int s = menuSel;
    if (PRESSED(left))  s = (s + n - 1) % n;
    if (PRESSED(right)) s = (s + 1) % n;
    if (menuRepUp.tick(cur.up, 300, 150))     s = s >= 2 ? s - 2 : s;
    if (menuRepDown.tick(cur.down, 300, 150)) s = s + 2 < n ? s + 2 : s;
    if (s != menuSel) { menuSel = s; drawMenuItems(); }
  } else {
    if (menuRepUp.tick(cur.up, 300, 90))     { menuSel = (menuSel + n - 1) % n; drawMenuItems(); }
    if (menuRepDown.tick(cur.down, 300, 90)) { menuSel = (menuSel + 1) % n; drawMenuItems(); }
    if (PRESSED(b) || PRESSED(back) || PRESSED(left)) { menuBack(); return; }
  }
  if (PRESSED(a) || PRESSED(start)) menuPick(menuSel);
}

// ============================================================================
//  SPIELE  (jedes Spiel steht in einer eigenen Datei im Sketch-Ordner)
// ============================================================================
#include "g_snake.h"
#include "g_blocks.h"
#include "g_bricks.h"
#include "g_paddle.h"
#include "g_rocks.h"
#include "g_invaders.h"
#include "g_swarm.h"
#include "g_hopper.h"
#include "g_girder.h"
#include "g_digger.h"
#include "g_puff.h"
#include "g_racer.h"
#include "g_skyace.h"
#include "g_kit.h"          // Hilfen fuer die Touch-Spiele
#include "g_2048.h"
#include "g_lights.h"
#include "g_pipes.h"
#include "g_memory.h"
#include "g_four.h"
#include "g_blackjack.h"
#include "g_lanes.h"
void Kit_silence() { Kit::silence(); }

const GameDef GAMES[NUM_GAMES] = {
  {"SNAKE",       "hs_snake",  "Schlange wird mit jedem Apfel laenger",        Snake::start,  Snake::loop,  false},
  {"BLOCKS",      "hs_tetris", "Fallende Steine zu vollen Reihen stapeln",      Blocks::start, Blocks::loop, false},
  {"BRICKS",      "hs_brk",    "Mit Ball und Schlaeger die Mauer abraeumen",    Bricks::start, Bricks::loop, false},
  {"PADDLE",      "hs_paddle", "Tischtennis gegen den Computer oder zu zweit",  Paddle::start, Paddle::loop, true},
  {"ASTRO ROCKS", "hs_rocks",  "Felsbrocken im All zerschiessen",              Rocks::start,  Rocks::loop,  true},
  {"ALIEN WAVE",  "hs_aliens", "Die anrueckende Alien-Formation abwehren",     Invaders::start, Invaders::loop, true},
  {"STAR SWARM",  "hs_swarm",  "Sturzflug-Angriffe des Insektenschwarms",      Swarm::start,  Swarm::loop,  true},
  {"ROAD HOP",    "hs_hop",    "Kueken ueber Strasse und Fluss ins Nest bringen", Hopper::start, Hopper::loop, true},
  {"GIRDER",      "hs_girder", "Ueber Stahltraeger klettern, Faessern ausweichen", Girder::start, Girder::loop, true},
  {"DIG DEEP",    "hs_dig",    "Tunnel graben und Gegner aufpumpen",           Digger::start, Digger::loop, true},
  {"PUFF JUMP",   "hs_puff",   "Schweben, Gegner einsaugen und ausspucken",    Puff::start,   Puff::loop,   true},
  {"SUNSET RACE", "hs_race",   "3D-Rennen gegen die Zeit in den Sonnenuntergang", Racer::start, Racer::loop, true},
  {"SKY ACE",     "hs_sky",    "Propellerflugzeug-Shooter ueber dem Ozean",    SkyAce::start, SkyAce::loop, true},
  {"2048",        "hs_2048",   "Zahlenkacheln schieben und verschmelzen bis 2048", G2048::start, G2048::loop, true},
  {"LICHTER AUS", "hs_lights", "Ein Tipp schaltet ein Feld und seine Nachbarn um", Lights::start, Lights::loop, true},
  {"ROHRPOST",    "hs_pipes",  "Rohre drehen, bis alles an der Station haengt",  Pipes::start,  Pipes::loop,  true},
  {"PAARE FINDEN","hs_memory", "Verdeckte Karten paarweise aufdecken",            Memory::start, Memory::loop, true},
  {"VIER GEWINNT","hs_four",   "Vier in einer Reihe - gegen die CPU oder zu zweit", Four::start, Four::loop,   true},
  {"SIEBZEHN UND VIER", "hs_bj", "Kartenspiel: moeglichst nah an 21",           BJ::start,     BJ::loop,     true},
  {"SPURWECHSEL", "hs_lanes",  "Auf drei Spuren dem Verkehr ausweichen",         Lanes::start,  Lanes::loop,  true},
};

// Kategorien im Menue (Spielnummern = Position in GAMES)
const Category CATS[NUM_CATS] = {
  {"KLASSIKER",      RGB(40, 220, 80),  4, {0, 1, 2, 3}},      // Snake, Blocks, Bricks, Paddle
  {"SHOOTER",        RGB(255, 80, 80),  4, {4, 5, 6, 12}},     // Astro Rocks, Alien Wave, Star Swarm, Sky Ace
  {"JUMP & RUN",     RGB(255, 170, 0),  4, {7, 8, 9, 10}},     // Road Hop, Girder, Dig Deep, Puff Jump
  {"RENNEN",         RGB(0, 200, 255),  2, {11, 19}},          // Sunset Race, Spurwechsel
  {"DENKSPIELE",     RGB(200, 110, 255), 4, {13, 14, 15, 16}}, // 2048, Lichter aus, Rohrpost, Paare finden
  {"BRETT & KARTEN", RGB(255, 220, 0),  2, {17, 18}},          // Vier gewinnt, Siebzehn und vier
};
// ============================================================================
//  WLAN
// ============================================================================
bool wifiOn = false;
bool apMode = false;
String ipStr;

// ---------------------------------------------------------------------------
//  Bluetooth-Suche und WLAN teilen sich beim ESP32 dieselbe Antenne.
//  Die staendige Suche nach neuen Controllern blockiert das WLAN so stark,
//  dass sich Handys nicht verbinden koennen. Deshalb gilt:
//   - WLAN aus:  Suche laeuft (wenn "Neue Controller zulassen" an ist)
//   - WLAN an:   Suche nur waehrend eines Kopplungsfensters von 60 s
//  Bereits gekoppelte Controller verbinden sich trotzdem jederzeit.
// ---------------------------------------------------------------------------
uint32_t pairUntil = 0;
int btScanState = -1;

int pairSecondsLeft() {
  int32_t ms = (int32_t)(pairUntil - millis());
  return ms > 0 ? (ms + 999) / 1000 : 0;
}

// Im eigenen Setup-WLAN (Access Point) sind WLAN-Verbindungen bei aktivem
// Bluetooth laut Espressif instabil. Dort werden Controller getrennt, solange
// kein Kopplungsfenster laeuft. Im Heimnetz-Modus (STA) ist das nicht noetig.
bool btQuietNeeded() {
  return wifiOn && apMode && pairSecondsLeft() == 0;
}

void disconnectAllPads() {
  for (auto c : pads)
    if (c && c->isConnected()) c->disconnect();
}

void updateBtScan() {
  bool scan = cfg.allowNew && (!wifiOn || pairSecondsLeft() > 0);
  if ((int)scan != btScanState) {
    btScanState = scan;
    BP32.enableNewBluetoothConnections(scan);
    Serial.printf("Bluetooth-Suche: %s\n", scan ? "an" : "aus");
  }
}

void startPairing() {
  pairUntil = millis() + 60000;
  updateBtScan();
}

void wifiStart() {
  if (wifiOn) return;
  wifiOn = true;               // Suche sofort stoppen, damit das WLAN Luft hat
  updateBtScan();
  wifiOn = false;

  WiFi.persistent(false);
  WiFi.setHostname(HOSTNAME);
  bool connected = false;
  // Heimnetz: zuerst die eigenen WLAN-Daten, dann (neben der Wetteruhr) die der Uhr -
  // aus deren Einstellungen oder, falls der Speicher geloescht wurde, aus der Sicherung (cyd_shared.h).
  String ss[2], pw[2];
  int nTry = 0;
  if (cfg.ssid.length()) { ss[nTry] = cfg.ssid; pw[nTry] = cfg.pass; nTry++; }
  {
    String cs, cp;
    if (clockPresent && cyd::clockWifi(cs, cp) && (nTry == 0 || cs != ss[0] || cp != pw[0])) { ss[nTry] = cs; pw[nTry] = cp; nTry++; }
  }
  for (int i = 0; i < nTry && !connected; i++) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ss[i].c_str(), pw[i].c_str());
    uint32_t t = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t < 15000) {
      delay(100);
      BP32.update();
    }
    connected = (WiFi.status() == WL_CONNECTED);
    if (!connected) {
      Serial.printf("WLAN %s nicht erreichbar\n", ss[i].c_str());
      WiFi.disconnect(true);
      delay(100);
    }
  }
  if (!connected && nTry) Serial.println("Heimnetz nicht erreichbar -> eigenes WLAN");
  if (connected) {
    apMode = false;
    ipStr = WiFi.localIP().toString();
  } else {
    WiFi.mode(WIFI_AP);
    delay(100);
    IPAddress apIp(192, 168, 4, 1);
    WiFi.softAPConfig(apIp, apIp, IPAddress(255, 255, 255, 0));
    bool ok = WiFi.softAP(AP_SSID, cfg.apPass.length() >= 8 ? cfg.apPass.c_str() : nullptr,
                          6, 0, 4);   // Kanal 6, sichtbar, max. 4 Geraete
    Serial.printf("Access Point %s: %s\n", AP_SSID, ok ? "OK" : "FEHLER");
    delay(200);
    apMode = true;
    ipStr = WiFi.softAPIP().toString();
    dns.start(53, "*", WiFi.softAPIP());     // Handy oeffnet Seite automatisch
    disconnectAllPads();                     // Bluetooth-Funkverkehr stoppen
  }
  MDNS.begin(HOSTNAME);
  MDNS.addService("http", "tcp", 80);
  server.begin();
  wifiOn = true;
  Serial.printf("WLAN aktiv (%s): http://%s\n", apMode ? "AP" : "STA", ipStr.c_str());
}

void wifiStop() {
  if (!wifiOn) return;
  server.stop();
  dns.stop();
  MDNS.end();
  WiFi.softAPdisconnect(true);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  wifiOn = false;
  pairUntil = 0;
  updateBtScan();
}

// ============================================================================
//  FIRMWARE-UPDATE: Anzeige
// ============================================================================
int otaLastPct = -1;
String otaErr;

void otaScreen(const char* title) {
  tft.fillScreen(C_BG);
  drawStripes(8);
  txt(title, SCR_W / 2, SCR_H / 2 - 40, 2, C_YELLOW, C_BG);
  txt("Nicht ausschalten!", SCR_W / 2, SCR_H / 2 - 16, 1, C_RED, C_BG);
  tft.drawRect(20, SCR_H / 2, SCR_W - 40, 16, C_WHITE);
  otaLastPct = -1;
}

void otaProgress(size_t done, size_t total) {
  if (total == 0) return;
  int pct = (int)(done * 100 / total);
  if (pct == otaLastPct) return;
  otaLastPct = pct;
  int w = (SCR_W - 44) * pct / 100;
  tft.fillRect(22, SCR_H / 2 + 2, w, 12, C_GREEN);
  char buf[8];
  snprintf(buf, sizeof(buf), "%d %%", pct);
  hud(buf, SCR_W / 2 - 15, SCR_H / 2 + 26, 50, 1.5, C_WHITE, C_BG);
}

void otaFinish(bool ok) {
  if (relayToken) {                                // Update-Modus: in jedem Fall zurueck zur Uhr
    txt(ok ? "Fertig - Uhr startet..." : "Update fehlgeschlagen", SCR_W / 2, SCR_H / 2 + 60, 1.5, ok ? C_GREEN : C_RED, C_BG);
    if (!ok) txt(otaErr.c_str(), SCR_W / 2, SCR_H / 2 + 80, 1, C_GRAY, C_BG);
    restartAt = millis() + (ok ? 1500 : 4000);
    return;
  }
  if (ok) {
    txt("Fertig - Neustart...", SCR_W / 2, SCR_H / 2 + 60, 1.5, C_GREEN, C_BG);
    restartAt = millis() + 1500;
  } else {
    txt("Update fehlgeschlagen", SCR_W / 2, SCR_H / 2 + 60, 1.5, C_RED, C_BG);
    txt(otaErr.c_str(), SCR_W / 2, SCR_H / 2 + 80, 1, C_GRAY, C_BG);
    delay(2500);
    if (state == ST_SETUP) drawSetup(); else toMenu();
  }
}

// ============================================================================
//  WEB-OBERFLAECHE
// ============================================================================
#include "webpage.h"   // HTML-Seite liegt in eigener Datei

String jsonEsc(const String& s) {
  String o;
  for (size_t i = 0; i < s.length(); i++) {
    char c = s[i];
    if (c == '"' || c == '\\') { o += '\\'; o += c; }
    else if ((uint8_t)c >= 0x20) o += c;
  }
  return o;
}

const char* jb(bool b) { return b ? "true" : "false"; }

void sendJson(const String& s) { server.send(200, "application/json", s); }

void handleRoot() {
  // direkt aus dem Flash senden, ohne Kopie im Arbeitsspeicher
  server.send_P(200, "text/html; charset=utf-8", PAGE, strlen_P(PAGE));
}

void handleNotFound() {
  // Captive Portal: alles auf die Startseite umleiten
  server.sendHeader("Location", String("http://") + ipStr + "/", true);
  server.send(302, "text/plain", "");
}

void handleState() {
  String j;
  j.reserve(1024);
  j += "{\"fw\":\"" FW_VERSION "\"";
  j += ",\"ip\":\"" + ipStr + "\"";
  j += ",\"ap\":"; j += jb(apMode);
  j += ",\"apName\":\"" AP_SSID "\"";
  j += ",\"ssid\":\"" + jsonEsc(cfg.ssid) + "\"";
  j += ",\"apPass\":\"" + jsonEsc(cfg.apPass) + "\"";
  j += ",\"wifiAlways\":"; j += jb(cfg.wifiAlways);
  j += ",\"heap\":" + String(ESP.getFreeHeap());
  j += ",\"sd\":"; j += jb(sdOk);
  j += ",\"rot\":" + String(cfg.rot);
  j += ",\"mirror\":"; j += jb(cfg.mirror);
  j += ",\"invert\":"; j += jb(cfg.invert);
  j += ",\"st7789\":" + String(cfg.st7789 ? 1 : 0);
  j += ",\"bright\":" + String(cfg.bright);
  j += ",\"rumbleOn\":"; j += jb(cfg.rumbleOn);
  j += ",\"rumbleStr\":" + String(cfg.rumbleStr);
  j += ",\"snakeSpeed\":" + String(cfg.snakeSpeed);
  j += ",\"allowNew\":"; j += jb(cfg.allowNew);
  j += ",\"pairing\":" + String(pairSecondsLeft());
  j += ",\"hs\":[";
  for (int i = 0; i < NUM_GAMES; i++) { if (i) j += ","; j += String(highscore[i]); }
  j += "],\"games\":[";
  for (int i = 0; i < NUM_GAMES; i++) {
    if (i) j += ",";
    j += "{\"n\":\"" + String(GAMES[i].name) + "\",\"d\":\"" + String(GAMES[i].info) + "\"}";
  }
  j += "],\"pads\":[";
  bool firstPad = true;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    ControllerPtr c = pads[i];
    if (!c || !c->isConnected()) continue;
    if (!firstPad) j += ",";
    firstPad = false;
    int bat = c->battery() ? (int)c->battery() * 100 / 255 : 0;
    j += "{\"i\":" + String(i) + ",\"model\":\"" + jsonEsc(c->getModelName()) +
         "\",\"bat\":" + String(bat) + "}";
  }
  j += "]}";
  sendJson(j);
}

void handleSettings() {
  bool oldSt = cfg.st7789;
  uint8_t oldRot = cfg.rot;
  bool oldMirror = cfg.mirror;
  if (server.hasArg("rot"))        cfg.rot = server.arg("rot").toInt() & 3;
  if (server.hasArg("mirror"))     cfg.mirror = server.arg("mirror").toInt();
  if (server.hasArg("invert"))     cfg.invert = server.arg("invert").toInt();
  if (server.hasArg("st7789"))     cfg.st7789 = server.arg("st7789").toInt();
  if (server.hasArg("bright"))     cfg.bright = constrain(server.arg("bright").toInt(), 10, 255);
  if (server.hasArg("rumbleOn"))   cfg.rumbleOn = server.arg("rumbleOn").toInt();
  if (server.hasArg("rumbleStr"))  cfg.rumbleStr = constrain(server.arg("rumbleStr").toInt(), 20, 255);
  if (server.hasArg("snakeSpeed")) cfg.snakeSpeed = constrain(server.arg("snakeSpeed").toInt(), 0, 2);
  if (server.hasArg("allowNew"))   cfg.allowNew = server.arg("allowNew").toInt();
  saveSettings();
  updateBtScan();

  if (cfg.st7789 != oldSt) {
    sendJson("{\"ok\":true,\"restart\":true}");
    restartAt = millis() + 800;
    return;
  }
  applyDisplay();
  if (cfg.rot != oldRot || cfg.mirror != oldMirror) {
    if (state == ST_SETUP) drawSetup();
    else toMenu();                   // laufendes Spiel passt nicht mehr ins neue Format
  }
  sendJson("{\"ok\":true}");
}

void handleWifi() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");
  String apPass = server.arg("apPass");
  ssid.trim();
  if (apPass.length() > 0 && apPass.length() < 8) {
    sendJson("{\"ok\":false,\"err\":\"Setup-Passwort: mindestens 8 Zeichen\"}");
    return;
  }
  if (ssid != cfg.ssid) cfg.pass = pass;
  else if (pass.length()) cfg.pass = pass;
  if (ssid.length() == 0) cfg.pass = "";
  cfg.ssid = ssid;
  cfg.apPass = apPass;
  cfg.wifiAlways = server.arg("always").toInt();
  saveSettings();
  sendJson("{\"ok\":true}");
  restartAt = millis() + 1000;
}

void handleBt() {
  String a = server.arg("action");
  int i = server.arg("idx").toInt();
  bool valid = i >= 0 && i < BP32_MAX_GAMEPADS && pads[i] && pads[i]->isConnected();
  if (a == "pair") startPairing();
  else if (a == "forget") BP32.forgetBluetoothKeys();
  else if (a == "disconnect" && valid) pads[i]->disconnect();
  else if (a == "rumble" && valid) pads[i]->playDualRumble(0, 400, 0xC0, 0xC0);
  sendJson("{\"ok\":true}");
}

void handleHsReset() {
  for (int i = 0; i < NUM_GAMES; i++) highscore[i] = 0;
  saveHighscores();
  if (state == ST_MENU) drawMenuItems();
  sendJson("{\"ok\":true}");
}

void handleReboot() {
  sendJson("{\"ok\":true}");
  restartAt = millis() + 500;
}

// Zwei Programme: ein Update landet immer im ANDEREN Programmbereich. Liegt dort die Wetteruhr,
// darf hier nur die Uhr aktualisiert werden; RetroCYD selbst wird dann ueber die Uhr aktualisiert.
cyd::App otaExpect = cyd::APP_NONE;
cyd::MarkScan otaScan;
bool otaBegun = false;

String otaCheckTarget(cyd::App app) {
  otaExpect = cyd::APP_NONE;
  if (!clockPresent) return "";                                   // RetroCYD allein: wie bisher
  if (app == cyd::APP_CLOCK) { otaExpect = cyd::APP_CLOCK; return ""; }
  if (app == cyd::APP_GAMES) return "RetroCYD-Updates bitte im Browser der Wetteruhr einspielen (System > Firmware-Update) - hier wuerde die Uhr ueberschrieben.";
  return "Dateiname muss WetterUhr (Uhr-Update) enthalten.";
}

void handleUploadData() {
  HTTPUpload& up = server.upload();
  if (up.status == UPLOAD_FILE_START) {
    otaErr = "";
    otaBegun = false;
    size_t sz = server.hasArg("size") ? (size_t)server.arg("size").toInt() : 0;
    if (relayToken && strtoul(server.arg("t").c_str(), nullptr, 16) != relayToken) { otaErr = "Ungueltige Anfrage"; return; }
    otaErr = otaCheckTarget(cyd::appFromName(up.filename));
    if (!otaErr.isEmpty()) return;
    if (otaExpect != cyd::APP_NONE) otaScan.begin(otaExpect);
    otaScreen(otaExpect == cyd::APP_CLOCK ? "UHR-UPDATE" : "WLAN-UPDATE");
    otaBegun = true;
    if (!Update.begin(sz > 0 ? sz : UPDATE_SIZE_UNKNOWN, U_FLASH)) otaErr = Update.errorString();
  } else if (up.status == UPLOAD_FILE_WRITE) {
    if (otaExpect != cyd::APP_NONE) otaScan.feed(up.buf, up.currentSize);
    if (otaErr.isEmpty() && Update.write(up.buf, up.currentSize) != up.currentSize)
      otaErr = Update.errorString();
  } else if (up.status == UPLOAD_FILE_END) {
    if (otaErr.isEmpty() && otaExpect != cyd::APP_NONE && !otaScan.found) {
      Update.abort();
      otaErr = "Die Datei ist keine Wetteruhr-Firmware fuer das CYD";
    }
    if (otaErr.isEmpty() && !Update.end(true)) otaErr = Update.errorString();
  } else if (up.status == UPLOAD_FILE_ABORTED) {
    Update.abort();
    otaErr = "Uebertragung abgebrochen";
  }
}

void handleUploadDone() {
  bool ok = otaErr.isEmpty() && !Update.hasError();
  if (ok) sendJson("{\"ok\":true}");
  else sendJson(String("{\"ok\":false,\"err\":\"") + jsonEsc(otaErr) + "\"}");
  if (otaBegun || ok) otaFinish(ok);
}

// Welches Programm steckt in einer Datei auf der SD-Karte? (Kennung suchen)
cyd::App sdFileApp(File& f) {
  cyd::MarkScan mc, mg;
  mc.begin(cyd::APP_CLOCK); mg.begin(cyd::APP_GAMES);
  uint8_t* buf = (uint8_t*)malloc(2048);
  if (!buf) return cyd::APP_NONE;
  f.seek(0);
  while (f.available()) {
    int n = f.read(buf, 2048);
    if (n <= 0) break;
    mc.feed(buf, n); mg.feed(buf, n);
    if (mc.found || mg.found) break;
  }
  free(buf);
  f.seek(0);
  return mc.found ? cyd::APP_CLOCK : (mg.found ? cyd::APP_GAMES : cyd::APP_NONE);
}

void handleSdUpdate() {
  if (!sdOk || !SD.exists(SD_UPDFILE)) {
    sendJson("{\"ok\":false,\"err\":\"keine /retrocyd/update.bin auf der SD-Karte\"}");
    return;
  }
  File f = SD.open(SD_UPDFILE, FILE_READ);
  size_t sz = f.size();
  {
    String e = otaCheckTarget(sdFileApp(f));
    if (!e.isEmpty()) {
      f.close();
      sendJson(String("{\"ok\":false,\"err\":\"") + jsonEsc(e) + "\"}");
      return;
    }
  }
  otaScreen("SD-UPDATE");
  otaErr = "";
  if (!Update.begin(sz, U_FLASH)) otaErr = Update.errorString();
  else if (Update.writeStream(f) != sz) otaErr = Update.errorString();
  else if (!Update.end(true)) otaErr = Update.errorString();
  f.close();
  bool ok = otaErr.isEmpty();
  if (ok) {
    if (SD.exists(SD_UPDDONE)) SD.remove(SD_UPDDONE);
    SD.rename(SD_UPDFILE, SD_UPDDONE);   // nicht nochmal installieren
    sendJson("{\"ok\":true}");
  } else {
    sendJson(String("{\"ok\":false,\"err\":\"") + jsonEsc(otaErr) + "\"}");
  }
  otaFinish(ok);
}

void webSetup() {
  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/state", HTTP_GET, handleState);
  server.on("/api/settings", HTTP_POST, handleSettings);
  server.on("/api/wifi", HTTP_POST, handleWifi);
  server.on("/api/bt", HTTP_POST, handleBt);
  server.on("/api/hsreset", HTTP_POST, handleHsReset);
  server.on("/api/reboot", HTTP_POST, handleReboot);
  server.on("/api/sdupdate", HTTP_POST, handleSdUpdate);
  server.on("/update", HTTP_POST, handleUploadDone, handleUploadData);
  server.onNotFound(handleNotFound);
  Update.onProgress(otaProgress);
}

// ============================================================================
//  SETUP-BILDSCHIRM
// ============================================================================
int setupLastPads = -1;
int setupLastPair = -1;

void drawSetupStatus() {
  int n = connectedPads();
  int p = pairSecondsLeft();
  int y = SCR_H - 44;
  tft.fillRect(0, y, SCR_W, 44, C_BG);
  char buf[40];
  if (p > 0)       snprintf(buf, sizeof(buf), "Kopplung aktiv: noch %d s", p);
  else if (apMode) snprintf(buf, sizeof(buf), "Controller pausiert (Setup-WLAN)");
  else             snprintf(buf, sizeof(buf), "Controller: %d verbunden", n);
  txt(buf, SCR_W / 2, y + 7, 1, p ? C_YELLOW : (n || apMode ? C_GREEN : C_RED), C_BG);
  txt(n ? "A koppeln  X drehen  Y spiegeln" : "Einstellungen ueber die Webseite",
      SCR_W / 2, y + 21, 1, C_GRAY, C_BG);
  txt(n ? "B zurueck" : "Touch / BOOT = zurueck", SCR_W / 2, y + 35, 1, C_GRAY, C_BG);
  setupLastPads = n;
  setupLastPair = p;
}

void drawSetup() {
  tft.fillScreen(C_BG);
  drawStripes(6);
  txt("SETUP", SCR_W / 2, 36, 3, C_YELLOW, C_BG);
  if (!wifiOn) {
    txt("WLAN startet...", SCR_W / 2, SCR_H / 2, 1.5, C_CYAN, C_BG);
    return;
  }
  const uint8_t L = textdatum_t::top_left;
  bool land = landscape();
  int tx = land ? 8 : 12;
  int y = 58;
  char buf[64];

  if (apMode) {
    txt("1. WLAN verbinden:", tx, y, 1, C_CYAN, C_BG, L);            y += 12;
    txt(AP_SSID, tx, y, 1.5, C_WHITE, C_BG, L);                     y += 16;
    if (cfg.apPass.length() >= 8) {
      snprintf(buf, sizeof(buf), "Passwort: %s", cfg.apPass.c_str());
      txt(buf, tx, y, 1, C_WHITE, C_BG, L);                         y += 14;
    }
  } else {
    txt("Im WLAN:", tx, y, 1, C_CYAN, C_BG, L);                      y += 12;
    txt(cfg.ssid.c_str(), tx, y, 1.5, C_WHITE, C_BG, L);            y += 18;
  }
  y += 4;
  txt(apMode ? "2. Im Browser oeffnen:" : "Im Browser oeffnen:", tx, y, 1, C_CYAN, C_BG, L);
  y += 12;
  snprintf(buf, sizeof(buf), "http://%s", ipStr.c_str());
  txt(buf, tx, y, 1.5, C_YELLOW, C_BG, L);                           y += 16;
  txt("http://retrocyd.local", tx, y, 1, C_GRAY, C_BG, L);           y += 14;

  String qr;
  if (!apMode) qr = String("http://") + ipStr + "/";
  else if (cfg.apPass.length() >= 8) qr = String("WIFI:T:WPA;S:") + AP_SSID + ";P:" + cfg.apPass + ";;";
  else qr = String("WIFI:T:nopass;S:") + AP_SSID + ";;";

  int qs = land ? 100 : 116;
  int qx = land ? SCR_W - qs - 8 : (SCR_W - qs) / 2;
  int qy = land ? 62 : y + 8;
  tft.fillRect(qx - 5, qy - 5, qs + 10, qs + 10, C_WHITE);
  tft.qrcode(qr.c_str(), qx, qy, qs, 4);

  drawSetupStatus();
}

void enterSetup() {
  state = ST_SETUP;
  touchPrev = true;          // erst loslassen, dann zaehlt die naechste Beruehrung
  drawSetup();
  wifiStart();
  drawSetup();
}

void leaveSetup() {
  if (!cfg.wifiAlways) wifiStop();
  toMenu();
}

void setupLoop() {
  bool t = touchDown() || bootDown();
  bool tap = t && !touchPrev;
  touchPrev = t;
  if (PRESSED(b) || PRESSED(back) || tap) { leaveSetup(); return; }
  if (PRESSED(x)) { cfg.rot = (cfg.rot + 1) & 3; saveSettings(); applyDisplay(); drawSetup(); }
  if (PRESSED(y)) { cfg.mirror = !cfg.mirror;   saveSettings(); applyDisplay(); drawSetup(); }
  if (PRESSED(a)) startPairing();
  if (connectedPads() != setupLastPads || pairSecondsLeft() != setupLastPair) drawSetupStatus();
}

// ============================================================================
//  START / HAUPTSCHLEIFE
// ============================================================================
// ============================================================================
//  TOUCH-STEUERUNG  (ohne Controller: der Bildschirm ist das Steuerkreuz)
//  Mitte antippen = Aktion (je Spiel A oder B), ausserhalb der Mitte = Richtung,
//  Ecke oben rechts = Pause/Menue. Resistiver Touch kennt nur EINEN Punkt, deshalb
//  feuern Shooter automatisch, das Rennauto gibt automatisch Gas, und bei den
//  Huepfspielen bedeuten die oberen Ecken "laufen + springen".
//  Kalibrierung: 3 Kreuze antippen (beim ersten Spiel ohne Controller, oder Halte-Menue).
// ============================================================================
TCal tcal{};
uint8_t tOrient() { return (cfg.rot & 3) | (cfg.mirror ? 4 : 0) | (cfg.st7789 ? 8 : 0); }
void tcalLoad() {
  if (prefs.getBytesLength("tcal") == sizeof(TCal)) prefs.getBytes("tcal", &tcal, sizeof(TCal));
  else tcal.ok = 0;
}
bool tcalValid() { return tcal.ok && tcal.ver == 1 && tcal.orient == tOrient() && tcal.x0 != tcal.x1 && tcal.y0 != tcal.y1; }

// Beruehrpunkt in Bildschirmkoordinaten; false = nicht beruehrt oder nicht eingerichtet
bool touchXY(int& x, int& y) {
  if (!tcalValid()) return false;
  int a, b;
  if (cyd::touchRaw(a, b) < 300) return false;
  const int rx = tcal.swap ? b : a, ry = tcal.swap ? a : b;
  x = 20 + (long)(rx - tcal.x0) * (SCR_W - 40) / (tcal.x1 - tcal.x0);
  y = 20 + (long)(ry - tcal.y0) * (SCR_H - 40) / (tcal.y1 - tcal.y0);
  x = constrain(x, 0, SCR_W - 1); y = constrain(y, 0, SCR_H - 1);
  return true;
}

bool tWaitTap(int& a, int& b, uint32_t timeout) {
  const uint32_t t0 = millis();
  int ra, rb;
  while (cyd::touchRaw(ra, rb) >= 300) { if (millis() - t0 > timeout) return false; delay(10); }
  uint32_t since = 0;
  while (millis() - t0 < timeout) {
    if (cyd::touchRaw(ra, rb) >= 300) {
      if (!since) since = millis();
      if (millis() - since > 120) {
        long sa = 0, sb = 0; int n = 0;
        const uint32_t t1 = millis();
        while (millis() - t1 < 150) { if (cyd::touchRaw(ra, rb) >= 300) { sa += ra; sb += rb; n++; } delay(5); }
        if (n >= 3) {
          a = sa / n; b = sb / n;
          while (cyd::touchRaw(ra, rb) >= 300 && millis() - t0 < timeout) delay(10);
          return true;
        }
        since = 0;
      }
    } else since = 0;
    BP32.update();
    delay(10);
  }
  return false;
}

void touchCalibrate() {
  const int PX[3] = {20, SCR_W - 20, 20}, PY[3] = {20, 20, SCR_H - 20};
  int A[3], B[3];
  for (int i = 0; i < 3; i++) {
    tft.fillScreen(C_BG);
    txt("TOUCH EINRICHTEN", SCR_W / 2, SCR_H / 2 - 12, 2, C_YELLOW, C_BG);
    txt("Auf das rote Kreuz tippen", SCR_W / 2, SCR_H / 2 + 12, 1.5, C_WHITE, C_BG);
    tft.drawFastHLine(PX[i] - 12, PY[i], 25, C_WHITE);
    tft.drawFastVLine(PX[i], PY[i] - 12, 25, C_WHITE);
    tft.drawCircle(PX[i], PY[i], 6, C_RED);
    if (!tWaitTap(A[i], B[i], 20000)) { tft.fillScreen(C_BG); return; }
  }
  TCal c{};
  c.ver = 1; c.orient = tOrient();
  c.swap = abs(B[1] - B[0]) > abs(A[1] - A[0]);
  const int* X = c.swap ? B : A;
  const int* Y = c.swap ? A : B;
  c.x0 = X[0]; c.x1 = X[1]; c.y0 = Y[0]; c.y1 = Y[2];
  const bool ok = abs(c.x1 - c.x0) > 800 && abs(c.y1 - c.y0) > 600;
  tft.fillScreen(C_BG);
  if (ok) {
    c.ok = 1; tcal = c;
    prefs.putBytes("tcal", &tcal, sizeof(TCal));
    txt("TOUCH EINGERICHTET", SCR_W / 2, SCR_H / 2, 2, C_GREEN, C_BG);
  } else {
    txt("BITTE NOCH EINMAL", SCR_W / 2, SCR_H / 2, 2, C_RED, C_BG);
  }
  delay(900);
  tft.fillScreen(C_BG);
  clearEdges();
}

// Je Spiel: was die Mitte bedeutet und was automatisch passiert (TouchCfg: touchpad.h)
TouchCfg touchCfg(int g) {
  const char* n = (g >= 0 && g < NUM_GAMES) ? GAMES[g].name : "";
  if (!strcmp(n, "2048"))        return {0, false, false, false, false, "Wischen schiebt die Kacheln (oder Pfeilfelder)", true};
  if (!strcmp(n, "LICHTER AUS")) return {0, false, false, false, false, "Feld antippen schaltet es und die Nachbarn um", true};
  if (!strcmp(n, "ROHRPOST"))    return {0, false, false, false, false, "Rohrstueck antippen dreht es", true};
  if (!strcmp(n, "PAARE FINDEN")) return {0, false, false, false, false, "Karten antippen und Paare finden", true};
  if (!strcmp(n, "VIER GEWINNT")) return {0, false, false, false, false, "Spalte antippen wirft den Stein ein", true};
  if (!strcmp(n, "SIEBZEHN UND VIER")) return {0, false, false, false, false, "Knoepfe antippen", true};
  if (!strcmp(n, "SPURWECHSEL")) return {0, false, false, false, false, "Linke / rechte Haelfte = Spur wechseln", true};
  if (!strcmp(n, "SUNSET RACE")) return {0,   false, true,  false, false, "Links/rechts lenken, unten bremsen, Gas automatisch"};
  if (!strcmp(n, "SKY ACE"))     return {'B', true,  false, false, false, "Zum Finger fliegen, Feuer automatisch, Mitte = Bombe"};
  if (!strcmp(n, "ASTRO ROCKS")) return {'B', true,  false, false, false, "Links/rechts drehen, oben Schub, Mitte = Sprung"};
  if (!strcmp(n, "ALIEN WAVE") || !strcmp(n, "STAR SWARM"))
                                 return {0,   true,  false, false, false, "Links/rechts fahren, Feuer automatisch"};
  if (!strcmp(n, "GIRDER"))      return {'A', false, false, true,  false, "Mitte springen, obere Ecken = laufen + springen"};
  if (!strcmp(n, "PUFF JUMP"))   return {'B', false, false, true,  true,  "Oben schweben, Mitte einsaugen/spucken"};
  if (!strcmp(n, "BLOCKS"))      return {'A', false, false, false, false, "Mitte drehen, links/rechts schieben, unten fallen"};
  if (!strcmp(n, "SNAKE") || !strcmp(n, "ROAD HOP"))
                                 return {0,   false, false, false, false, "In die Richtung tippen"};
  return {'A', false, false, false, false, "Richtung tippen, Mitte = Aktion"};
}
TouchCfg tcfg;
bool tPausePrev = false;

bool touchActive() { return state == ST_GAME && !holdOpen && connectedPads() == 0 && tcalValid(); }

void touchKeys(Keys& k) {
  int x, y;
  const bool t = touchXY(x, y);
  const bool inPause = t && x > SCR_W - tPauseW && y < tPauseH;
  if (inPause) { if (!tPausePrev) holdReq = true; tPausePrev = true; return; }   // Ecke oben rechts: Menue
  tPausePrev = false;
  if (tcfg.direct) return;                       // Spiel liest die Beruehrung selbst (g_kit.h)
  if (t) {
    const int dx = x - SCR_W / 2, dy = y - SCR_H / 2;
    const int zx = SCR_W * 16 / 100, zy = SCR_H * 20 / 100;
    if (abs(dx) < zx && abs(dy) < zy) {                 // Mitte = Aktion
      if (tcfg.center == 'A') k.a = true;
      else if (tcfg.center == 'B') k.b = true;
    } else {
      const long nx = (long)dx * SCR_H, ny = (long)dy * SCR_W;   // Seitenverhaeltnis ausgleichen
      if (abs(dx) >= zx && 2 * labs(nx) >= labs(ny)) { if (dx < 0) k.left = true; else k.right = true; }
      if (abs(dy) >= zy && 2 * labs(ny) >= labs(nx)) { if (dy < 0) k.up = true; else k.down = true; }
      k.ax = (int16_t)constrain((long)dx * 512 / (SCR_W * 35 / 100), -512L, 512L);
      k.ay = (int16_t)constrain((long)dy * 512 / (SCR_H * 35 / 100), -512L, 512L);
      if (tcfg.diagA && k.up && (k.left || k.right)) { k.up = false; k.a = true; }
      else if (tcfg.upIsA && k.up) { k.up = false; k.a = true; }
    }
  }
  if (tcfg.autoA) k.a |= ((millis() / 110) % 2) == 0;   // Dauerfeuer (Druecken/Loslassen im Wechsel)
  if (tcfg.autoGas) k.a = !k.down;
}

// kurzes Antippen erkennen (beim Loslassen), fuer Spielende und Menue
bool tTapDown = false; uint32_t tTapAt = 0; int tTapX = 0, tTapY = 0;
bool touchTapped() {
  int x, y;
  const bool t = touchXY(x, y);
  if (t) { if (!tTapDown) { tTapDown = true; tTapAt = millis(); } tTapX = x; tTapY = y; return false; }
  if (!tTapDown) return false;
  tTapDown = false;
  return millis() - tTapAt < 600;
}

// Menue: Kategorie antippen = oeffnen; Spiel antippen = auswaehlen, noch einmal = starten
bool menuTouch() {
  if (!touchTapped()) return false;
  const int n = menuItems();
  if (menuLevel == 0) {
    for (int i = 0; i < n; i++) {
      int x, y, w, h;
      catTile(i, x, y, w, h);
      if (tTapX >= x && tTapX < x + w && tTapY >= y && tTapY < y + h) {
        menuSel = i;
        drawMenuItems();
        menuPick(i);
        return true;
      }
    }
    return false;
  }
  int top, itemH, visible;
  menuLayout(top, itemH, visible);
  const int w = min(250, SCR_W - 24), x0 = (SCR_W - w) / 2;
  if (tTapX > SCR_W - 20 && tTapY < top + 20) { menuSel = max(0, menuTop - 1); drawMenuItems(); return true; }
  if (tTapX > SCR_W - 20 && tTapY > top + visible * itemH - 20) { menuSel = min(n - 1, menuTop + visible); drawMenuItems(); return true; }
  if (tTapY < top - 6) { menuBack(); return true; }      // Kopfzeile antippen = zurueck zu den Kategorien
  if (tTapX < x0 || tTapX > x0 + w || tTapY < top) return false;
  const int v = (tTapY - top) / itemH;
  if (v < 0 || v >= visible || menuTop + v >= n) return false;
  const int i = menuTop + v;
  if (i != menuSel && menuGame(i) >= 0) { menuSel = i; drawMenuItems(); return true; }
  menuPick(i);
  return true;
}

// Vor dem Spiel ohne Controller: Touch einrichten (falls noetig) und kurz die Bedienung zeigen
void touchIntro(int g) {
  if (connectedPads() > 0) return;
  if (!tcalValid()) touchCalibrate();
  if (!tcalValid()) return;
  tcfg = touchCfg(g);
  tft.fillScreen(C_BG);
  if (tcfg.direct) {                                // Touch-Spiel: nur kurzer Hinweis
    drawStripes(8);
    txt(GAMES[g].name, SCR_W / 2, SCR_H / 2 - 40, 2, C_YELLOW, C_BG);
    txt(tcfg.hint, SCR_W / 2, SCR_H / 2, 1, C_WHITE, C_BG);
    tft.fillRect(SCR_W - 20, 8, 4, 13, C_GRAY); tft.fillRect(SCR_W - 13, 8, 4, 13, C_GRAY);
    txt("Menue", SCR_W - 14, 30, 1, C_GRAY, C_BG);
    txt("Ecke oben rechts = Menue", SCR_W / 2, SCR_H / 2 + 20, 1, C_GRAY, C_BG);
    txt("Antippen zum Starten", SCR_W / 2, SCR_H / 2 + 50, 1.5, C_CYAN, C_BG);
  } else {
  const int zx = SCR_W * 16 / 100, zy = SCR_H * 20 / 100;
  tft.drawRoundRect(SCR_W / 2 - zx, SCR_H / 2 - zy, 2 * zx, 2 * zy, 8, C_YELLOW);
  if (tcfg.center) txt(tcfg.center == 'A' ? "AKTION" : "B", SCR_W / 2, SCR_H / 2, 1.5, C_YELLOW, C_BG);
  tft.fillTriangle(12, SCR_H / 2, 30, SCR_H / 2 - 14, 30, SCR_H / 2 + 14, C_CYAN);
  tft.fillTriangle(SCR_W - 12, SCR_H / 2, SCR_W - 30, SCR_H / 2 - 14, SCR_W - 30, SCR_H / 2 + 14, C_CYAN);
  tft.fillTriangle(SCR_W / 2, 50, SCR_W / 2 - 14, 68, SCR_W / 2 + 14, 68, C_CYAN);
  tft.fillTriangle(SCR_W / 2, SCR_H - 12, SCR_W / 2 - 14, SCR_H - 30, SCR_W / 2 + 14, SCR_H - 30, C_CYAN);
  tft.fillRect(SCR_W - 40, 8, 6, 20, C_WHITE); tft.fillRect(SCR_W - 30, 8, 6, 20, C_WHITE);
  txt("Menue", SCR_W - 32, 38, 1, C_GRAY, C_BG);
  txt("TOUCH-STEUERUNG", SCR_W / 2, 16, 1.5, C_YELLOW, C_BG);
  txt(tcfg.hint, SCR_W / 2, SCR_H / 2 + zy + 22, 1, C_WHITE, C_BG);
  txt("Antippen zum Starten", SCR_W / 2, SCR_H / 2 + zy + 38, 1, C_GRAY, C_BG);
  }
  const uint32_t t0 = millis();
  int a, b;
  while (cyd::touchRaw(a, b) >= 300 && millis() - t0 < 3000) delay(10);   // Finger vom Menue erst loslassen
  while (millis() - t0 < 6000) {
    if (cyd::touchRaw(a, b) >= 300) { while (cyd::touchRaw(a, b) >= 300) delay(10); break; }
    BP32.update();
    if (connectedPads() > 0) break;
    delay(15);
  }
  tTapDown = false;
}

void startGame(int g) {
  gameRestart = (state == ST_GAMEOVER && g == curGame);
  tPauseW = 52; tPauseH = 40;
  if (state != ST_GAME && state != ST_GAMEOVER) touchIntro(g);
  tcfg = touchCfg(g);
  curGame = g;
  paused = false;
  clearEdges();
  state = ST_GAME;
  fbEnd();
  gameMemBegin();
  if (!gameMem || (GAMES[g].fb && !fbBegin())) {
    tft.fillScreen(C_BG);
    txt("Zu wenig Speicher", SCR_W / 2, SCR_H / 2 - 10, 1.5, C_RED, C_BG);
    txt("WLAN 'immer an' ausschalten", SCR_W / 2, SCR_H / 2 + 12, 1, C_GRAY, C_BG);
    delay(2500);
    toMenu();
    return;
  }
  tft.fillScreen(C_BG);
  GAMES[g].start();
}

// ============================================================================
//  UPDATE-MODUS FUER DIE UHR
//  Die Uhr kann sich nicht selbst ueberschreiben. Bei einem Uhr-Update im Browser der
//  Uhr startet sie RetroCYD mit einem Einmal-Schluessel ("relay" im NVS der Uhr). RetroCYD
//  verbindet sich dann mit dem WLAN der Uhr (gleicher Name, gleiche IP), nimmt die Datei
//  entgegen, schreibt sie in den Bereich der Uhr und startet die neue Uhr. Kein Bluetooth,
//  keine Spiele; nach 3 Minuten ohne Datei geht es zur Uhr zurueck.
// ============================================================================
void relayMode() {
  tft.fillScreen(C_BG);
  drawStripes(8);
  txt("UHR-UPDATE", SCR_W / 2, SCR_H / 2 - 40, 2, C_YELLOW, C_BG);
  txt("WLAN verbindet...", SCR_W / 2, SCR_H / 2 - 10, 1.5, C_CYAN, C_BG);
  String ss, pw, host = "wetteruhr";
  {
    Preferences u;
    if (u.begin("wuhr", true)) { host = u.getString("host", "wetteruhr"); u.end(); }
  }
  bool ok = cyd::clockWifi(ss, pw);
  if (ok) {
    WiFi.persistent(false);
    WiFi.setHostname(host.c_str());          // gleicher Name wie die Uhr -> gleiche IP vom Router
    WiFi.mode(WIFI_STA);
    WiFi.begin(ss.c_str(), pw.c_str());
    uint32_t t = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - t < 20000) delay(100);
    ok = WiFi.status() == WL_CONNECTED;
  }
  if (!ok) {
    txt("Kein WLAN - zurueck zur Uhr", SCR_W / 2, SCR_H / 2 + 20, 1.5, C_RED, C_BG);
    delay(3000);
    ESP.restart();                           // Start-Partition steht auf der Uhr
  }
  MDNS.begin(host.c_str());
  MDNS.addService("http", "tcp", 80);
  server.on("/fwready", HTTP_GET, []() {
    bool m = strtoul(server.arg("t").c_str(), nullptr, 16) == relayToken;
    server.send(200, "text/plain", m ? "ready" : "no");
  });
  server.on("/update", HTTP_POST, handleUploadDone, handleUploadData);
  server.onNotFound([]() { server.send(404, "text/plain", "RetroCYD Update-Modus"); });
  server.begin();
  wifiOn = true;
  tft.fillRect(0, SCR_H / 2 - 22, SCR_W, 30, C_BG);
  txt("Warte auf die Datei", SCR_W / 2, SCR_H / 2 - 10, 1.5, C_WHITE, C_BG);
  txt(WiFi.localIP().toString().c_str(), SCR_W / 2, SCR_H / 2 + 14, 1.5, C_CYAN, C_BG);
  const uint32_t t0 = millis();
  while (true) {
    server.handleClient();
    if (restartAt && (int32_t)(millis() - restartAt) >= 0) ESP.restart();
    if (!restartAt && !otaBegun && millis() - t0 > 180000) ESP.restart();   // nichts gekommen
    delay(2);
  }
}

void setup() {
  Serial.begin(115200);

  prefs.begin("retrocyd", false);
  (void)*(volatile const char*)CYD_SELF_MARK;   // Kennung sicher im Programm behalten
  clockPresent = cyd::partHas(cyd::otherPart(), cyd::APP_CLOCK);
  if (clockPresent) {
    cyd::bootOther();          // naechster Neustart (Strom aus, "ZUR UHR") startet wieder die Uhr
    // Erster Start neben der Uhr: Display-Chip von der Uhr uebernehmen (WLAN der Uhr nutzt wifiStart direkt)
    Preferences u;
    if (u.begin("wuhr", true)) {
      if (!prefs.isKey("st7789") && u.isKey("pused")) prefs.putBool("st7789", u.getUChar("pused", 1) == 2);
      u.end();
    }
  }
  Serial.printf("RetroCYD %s, Wetteruhr im anderen Bereich: %s\n", FW_VERSION, clockPresent ? "ja" : "nein");
  loadSettings();
  for (int i = 0; i < NUM_GAMES; i++) highscore[i] = prefs.getUInt(GAMES[i].hsKey, 0);

  tft.selectPanel(cfg.st7789);
  tft.init();
  applyDisplay();
  tft.fillScreen(C_BG);

  if (clockPresent) {                            // Uhr-Update angefordert? (Einmal-Schluessel von der Uhr)
    Preferences u;
    if (u.begin("wuhr", false)) {
      relayToken = u.getUInt("relay", 0);
      if (relayToken) u.remove("relay");
      u.end();
    }
    if (relayToken) relayMode();                 // kehrt nicht zurueck
  }

  touchInit();

  sdOk = sdInit();
  if (sdOk) {
    sdLoadHighscores();
    saveHighscores();
    sdImportWifi();
  }

  BP32.setup(&onConnected, &onDisconnected);
  BP32.enableVirtualDevice(false);
  updateBtScan();

  webSetup();
  randomSeed(esp_random());

  if (cfg.wifiAlways) {
    txt("WLAN verbindet...", SCR_W / 2, SCR_H / 2, 1.5, C_CYAN, C_BG);
    wifiStart();
  }
  toMenu();
}

void loop() {
  static uint32_t lastFrame = 0;

  if (restartAt && (int32_t)(millis() - restartAt) >= 0) ESP.restart();

  BP32.update();
  readInput();
  updateBtScan();             // Kopplungsfenster abgelaufen? -> Suche aus

  if (wifiOn) {
    server.handleClient();
    if (apMode) dns.processNextRequest();
  }

  if (PRESSED(y) && state != ST_SETUP) sdScreenshot();
  Kit::soundService();        // Motorton aus, wenn nicht mehr angefordert

  if (state == ST_GAME && !holdOpen && longPress()) holdReq = true;   // Halte-Menue im Spiel

  switch (state) {
    case ST_MENU:     menuLoop();        break;
    case ST_GAME:     GAMES[curGame].loop(); break;
    case ST_GAMEOVER: gameOverLoop();    break;
    case ST_SETUP:    setupLoop();       break;
  }

  uint32_t el = millis() - lastFrame;
  if (el < 16) delay(16 - el);
  else delay(1);
  lastFrame = millis();
}
