#pragma once
/*
  board.h - Unterschiede zwischen den Boards (Auswahl in config.h, automatisch ueber die Board-Wahl der Arduino IDE)

    CYD 2,8 Zoll (ESP32-2432S028R): ILI9341 (oder ST7789-Variante), Touchscreen XPT2046, RGB-LED
    ESP32-C3 SuperMini:             ST7789 (GMT020-02-7P), Touch-Taster TTP223

  Hier stehen: Display-Objekt, Display ein/aus und Schlaf, Touch-Abfrage, RGB-LED.
*/
#include <SPI.h>
#include <Adafruit_GFX.h>
#include "config.h"

#if BOARD_CYD && !CYD_ST7789
  #include <Adafruit_ILI9341.h>
  typedef Adafruit_ILI9341 Tft;
  static Tft tft(&SPI, PIN_TFT_DC, PIN_TFT_CS, PIN_TFT_RST);
#else
  #include <Adafruit_ST7789.h>
  typedef Adafruit_ST7789 Tft;
  #if BOARD_CYD
    static Tft tft(&SPI, PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
  #else
    static Tft tft(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
  #endif
#endif

// Display starten (320x240 quer, gleiche Bildgroesse auf beiden Boards)
static void boardTftBegin(uint8_t rotation) {
#if BOARD_CYD
  SPI.begin(PIN_TFT_SCK, PIN_TFT_MISO, PIN_TFT_MOSI, -1);   // CS steuert die Bibliothek per GPIO
  #if CYD_ST7789
    tft.init(240, 320);
    tft.setSPISpeed(TFT_SPI_HZ);
    tft.invertDisplay(false);
  #else
    tft.begin(TFT_SPI_HZ);
  #endif
#else
  // SPI-Pins explizit setzen (entspricht beim C3 den Standardpins 4/5/6/7).
  SPI.begin(PIN_TFT_SCK, -1, PIN_TFT_MOSI, -1);
  tft.init(240, 320);
#endif
  tft.setRotation(rotation);
}

// Anzeige ein/aus (Bildinhalt bleibt) und Schlafmodus des Controllers
static void boardTftDisplay(bool on) {
#if BOARD_CYD && !CYD_ST7789
  tft.sendCommand(on ? ILI9341_DISPON : ILI9341_DISPOFF);
#else
  tft.enableDisplay(on);
#endif
}
static void boardTftSleep(bool on) {
#if BOARD_CYD && !CYD_ST7789
  tft.sendCommand(on ? ILI9341_SLPIN : ILI9341_SLPOUT);
#else
  tft.enableSleep(on);
#endif
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
  return CYD_ST7789 ? "CYD 2.8\" (ESP32 + ST7789)" : "CYD 2.8\" (ESP32 + ILI9341)";
#else
  return "ESP32-C3 SuperMini + ST7789 2.0\"";
#endif
}
