#pragma once
/*
  board.h - Unterschiede zwischen den Boards (Auswahl in config.h, automatisch ueber die Board-Wahl der Arduino IDE)

    CYD 2,8 Zoll (ESP32-2432S028R): ILI9341 oder ST7789 (automatisch erkannt), Touchscreen XPT2046, RGB-LED
    ESP32-C3 SuperMini:             ST7789 (GMT020-02-7P), Touch-Taster TTP223

  Hier stehen: Display-Objekt, Display ein/aus und Schlaf, Touch-Abfrage, RGB-LED.
*/
#include <SPI.h>
#include <Adafruit_GFX.h>
#include "config.h"

#if BOARD_CYD
  // CYD: ein Treiber-Objekt fuer beide Controller. ILI9341 und ST7789 nutzen zum Zeichnen dieselben Befehle
  // (Fenster 0x2A/0x2B, Speicher 0x2C, Ausrichtung 0x36); bei ST7789 wird nach dem Start dessen Init gesendet.
  #include <Adafruit_ILI9341.h>
  typedef Adafruit_ILI9341 Tft;
  static Tft tft(&SPI, PIN_TFT_DC, PIN_TFT_CS, PIN_TFT_RST);
#else
  #include <Adafruit_ST7789.h>
  typedef Adafruit_ST7789 Tft;
  static Tft tft(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
#endif

// ---------------------------------------------------------------------
// Display-Anpassung (Browser: System > Display-Anpassung)
//   Controller: 0 = automatisch erkennen, 1 = ILI9341, 2 = ST7789 (nur CYD)
//   Flags schalten gegenueber dem Standard des Controllers um:
// ---------------------------------------------------------------------
enum : uint8_t { PANEL_AUTO = 0, PANEL_ILI9341 = 1, PANEL_ST7789 = 2 };
enum : uint8_t {
  PF_SWAPXY = 0x01,   // Achsen tauschen (Bild um 90 Grad gedreht und gespiegelt)
  PF_MIRX   = 0x02,   // links/rechts spiegeln
  PF_MIRY   = 0x04,   // oben/unten spiegeln
  PF_BGR    = 0x08,   // Rot und Blau tauschen
  PF_INV    = 0x10,   // Farben invertieren (Negativ)
};
static uint8_t  gPanelUsed = PANEL_ILI9341;   // tatsaechlich verwendeter Controller
static bool     gPanelAuto = false;           // durch Erkennung bestimmt
static uint32_t gPanelId   = 0;               // gelesene Kennung (0x04), fuer die Status-Seite

#if BOARD_CYD
// Register lesen: Befehl senden, danach n Bytes holen (MISO an GPIO12)
static uint32_t panelRead(uint8_t cmd, int n) {
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(PIN_TFT_CS, LOW);
  digitalWrite(PIN_TFT_DC, LOW);
  SPI.transfer(cmd);
  digitalWrite(PIN_TFT_DC, HIGH);
  uint32_t v = 0;
  for (int i = 0; i < n; i++) v = (v << 8) | SPI.transfer(0);
  digitalWrite(PIN_TFT_CS, HIGH);
  SPI.endTransaction();
  return v;
}
// ST7789 meldet auf 0x04 die Kennung 85 85 52 (bzw. 81 81 B3); ILI9341 liefert dort 00 00 00.
static uint8_t panelDetect() {
  const uint32_t v = panelRead(0x04, 4);
  gPanelId = v;
  const uint8_t a = (v >> 23) & 0xFF, b = (v >> 24) & 0xFF;   // mit bzw. ohne 1 Bit Vorlauf
  if (a == 0x85 || a == 0x81 || b == 0x85 || b == 0x81) return PANEL_ST7789;
  return PANEL_ILI9341;
}
static void cmd(uint8_t c, const uint8_t* d = nullptr, uint8_t n = 0) { tft.sendCommand(c, d, n); }
static void st7789Init() {
  cmd(0x01); delay(150);                              // Software-Reset
  cmd(0x11); delay(120);                              // Sleep out
  const uint8_t colmod = 0x55; cmd(0x3A, &colmod, 1); // 16 Bit pro Pixel
  cmd(0x13); delay(10);                               // Normalmodus
  cmd(0x29); delay(20);                               // Display an
}
#endif

// Ausrichtung und Farben setzen (nach Start und nach Aenderung im Browser)
static void boardPanelApply(uint8_t rotation, uint8_t flags) {
  tft.setRotation(rotation);                          // setzt die Bildgroesse 320x240 in der Grafik-Bibliothek
  // Standard-MADCTL im Querformat: ILI9341 = MV|BGR, ST7789 = MX|MV (RGB); 180 Grad = MX und MY umschalten
  uint8_t m = (gPanelUsed == PANEL_ST7789) ? 0x60 : 0x28;
  if (rotation == 3) m ^= 0xC0;
  if (flags & PF_SWAPXY) m ^= 0x20;
  if (flags & PF_MIRX)   m ^= 0x40;
  if (flags & PF_MIRY)   m ^= 0x80;
  if (flags & PF_BGR)    m ^= 0x08;
  tft.sendCommand(0x36, &m, 1);
  const bool invDefault = (gPanelUsed == PANEL_ST7789);
  tft.sendCommand(((flags & PF_INV) != 0) != invDefault ? 0x21 : 0x20);
}

// Display starten (320x240 quer, gleiche Bildgroesse auf beiden Boards)
static void boardTftBegin(uint8_t rotation, uint8_t panel, uint8_t flags) {
#if BOARD_CYD
  SPI.begin(PIN_TFT_SCK, PIN_TFT_MISO, PIN_TFT_MOSI, -1);   // CS steuert die Bibliothek per GPIO
  tft.begin(TFT_SPI_HZ);
  gPanelAuto = (panel == PANEL_AUTO);
  gPanelUsed = gPanelAuto ? panelDetect() : (panel == PANEL_ST7789 ? PANEL_ST7789 : PANEL_ILI9341);
  if (!gPanelAuto) gPanelId = panelRead(0x04, 4);
  if (gPanelUsed == PANEL_ST7789) st7789Init();
#else
  (void)panel;
  // SPI-Pins explizit setzen (entspricht beim C3 den Standardpins 4/5/6/7).
  SPI.begin(PIN_TFT_SCK, -1, PIN_TFT_MOSI, -1);
  tft.init(240, 320);
  gPanelUsed = PANEL_ST7789;
#endif
  boardPanelApply(rotation, flags);
}

// Anzeige ein/aus (Bildinhalt bleibt) und Schlafmodus des Controllers (Befehle bei beiden Controllern gleich)
static void boardTftDisplay(bool on) { tft.sendCommand(on ? 0x29 : 0x28); }
static void boardTftSleep(bool on)   { tft.sendCommand(on ? 0x10 : 0x11); }

static String boardPanelInfo() {
  char b[64];
  snprintf(b, sizeof(b), "%s%s, ID %06lX", gPanelUsed == PANEL_ST7789 ? "ST7789" : "ILI9341",
           gPanelAuto ? " (auto)" : "", (unsigned long)((gPanelId >> 7) & 0xFFFFFF));
  return String(b);
}

// ---------------------------------------------------------------------
// Touchscreen XPT2046 (CYD): einfache Abfrage per Software-SPI, nur "beruehrt ja/nein"
// ---------------------------------------------------------------------
#if TOUCH_XPT2046
namespace xpt {
static uint16_t lastZ = 0;

static uint16_t xfer(uint8_t cmd) {
  for (int i = 7; i >= 0; i--) {                       // Befehl senden
    digitalWrite(PIN_XPT_MOSI, (cmd >> i) & 1);
    digitalWrite(PIN_XPT_CLK, HIGH); delayMicroseconds(1);
    digitalWrite(PIN_XPT_CLK, LOW);  delayMicroseconds(1);
  }
  digitalWrite(PIN_XPT_MOSI, LOW);                     // beim Lesen LOW halten (sonst Startbit eines neuen Befehls)
  uint16_t v = 0;
  for (int i = 0; i < 16; i++) {                       // 12-Bit-Ergebnis lesen (16 Takte)
    digitalWrite(PIN_XPT_CLK, HIGH); delayMicroseconds(1);
    v = (v << 1) | (digitalRead(PIN_XPT_MISO) ? 1 : 0);
    digitalWrite(PIN_XPT_CLK, LOW);  delayMicroseconds(1);
  }
  return (v >> 3) & 0x0FFF;
}

static void init() {
  pinMode(PIN_XPT_CS, OUTPUT);   digitalWrite(PIN_XPT_CS, HIGH);
  pinMode(PIN_XPT_CLK, OUTPUT);  digitalWrite(PIN_XPT_CLK, LOW);
  pinMode(PIN_XPT_MOSI, OUTPUT); digitalWrite(PIN_XPT_MOSI, LOW);
  pinMode(PIN_XPT_MISO, INPUT);
  pinMode(PIN_XPT_IRQ, INPUT);
}

// Druck messen: z = Z1 + 4095 - Z2 (gross = fester gedrueckt)
static bool touched() {
  if (digitalRead(PIN_XPT_IRQ) == HIGH) { lastZ = 0; return false; }   // schneller Ausschluss ohne SPI
  digitalWrite(PIN_XPT_CS, LOW);
  const int z1 = xfer(0xB0), z2 = xfer(0xC0);
  xfer(0x90);                                          // letzter Befehl mit PD=00: Stift-Interrupt bleibt aktiv
  digitalWrite(PIN_XPT_CS, HIGH);
  int z = z1 + 4095 - z2;
  if (z < 0) z = 0;
  lastZ = (uint16_t)z;
  return z >= XPT_Z_MIN;
}
}  // namespace xpt
#endif

static void boardTouchInit() {
#if TOUCH_XPT2046
  xpt::init();
#elif PIN_TOUCH >= 0
  pinMode(PIN_TOUCH, TOUCH_ACTIVE_HIGH ? INPUT_PULLDOWN : INPUT_PULLUP);
#endif
}

// true, solange beruehrt wird (ungefiltert, Entprellung macht touchTick)
static bool boardTouchRaw() {
#if TOUCH_XPT2046
  return xpt::touched();
#elif PIN_TOUCH >= 0
  return digitalRead(PIN_TOUCH) == (TOUCH_ACTIVE_HIGH ? HIGH : LOW);
#else
  return false;
#endif
}

// ---------------------------------------------------------------------
// RGB-LED (CYD, LOW = an)
// ---------------------------------------------------------------------
static void boardLed(bool r, bool g, bool b) {
#if PIN_LED_R >= 0
  digitalWrite(PIN_LED_R, r ? LOW : HIGH);
  digitalWrite(PIN_LED_G, g ? LOW : HIGH);
  digitalWrite(PIN_LED_B, b ? LOW : HIGH);
#else
  (void)r; (void)g; (void)b;
#endif
}
static void boardLedInit() {
#if PIN_LED_R >= 0
  boardLed(false, false, false);
  pinMode(PIN_LED_R, OUTPUT); pinMode(PIN_LED_G, OUTPUT); pinMode(PIN_LED_B, OUTPUT);
  boardLed(false, false, false);
#endif
}

static const char* boardName() {
#if BOARD_CYD
  return "CYD 2.8\" (ESP32-2432S028)";
#else
  return "ESP32-C3 SuperMini + ST7789 2.0\"";
#endif
}
