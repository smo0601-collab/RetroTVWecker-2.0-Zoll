#pragma once
/*
  ======================================================================
  KONFIGURATION - Wetteruhr ESP32-C3 SuperMini + GMT020-02-7P + TTP223

  Hier stehen Pins und FESTE Einstellungen. Alles, was sich im Betrieb
  aendern soll (WLAN, Zeitzone, Standort, Helligkeit, ...), wird ueber die
  Browser-Seite eingestellt und im Flash gespeichert. Die DEFAULT_*-Werte
  gelten nur beim allerersten Start bzw. nach "Werkseinstellungen".
  ======================================================================
*/

// ---------------------------------------------------------------------
// Standardwerte (aenderbar ueber die Browser-Seite)
// ---------------------------------------------------------------------
#define DEFAULT_WIFI_SSID    ""        // leer = beim ersten Start Einrichtungs-Access-Point
#define DEFAULT_WIFI_PASS    ""
#define DEFAULT_TZ           "CET-1CEST,M3.5.0,M10.5.0/3"   // Europe/Berlin
#define DEFAULT_NTP          "pool.ntp.org"
#define DEFAULT_NTP_ON       1         // 1 = Uhrzeit per NTP holen, 0 = nur manuell (Browser)
#define DEFAULT_LAT          52.5200f  // Platzhalter (Berlin) - ueber Browser anpassen
#define DEFAULT_LON          13.4050f
#define DEFAULT_RADAR_KM     25        // Flugradar: Radius in km (5..100)
#define DEFAULT_UPDATE_MIN   15        // Wetter-Intervall in Minuten (5..120)
#define DEFAULT_BRIGHTNESS   100       // 5..100 %
#define DEFAULT_ROTATION     1         // 1 oder 3 (Querformat)
#define DEFAULT_SECONDS      1         // 1 = Sekunden anzeigen
#define DEFAULT_DIM_ENABLE   0         // 1 = Zeitplan-Dimmung aktiv
#define DEFAULT_DIM_START    (22 * 60)  // Beginn der Dimmung (Minuten seit Mitternacht) = 22:00
#define DEFAULT_DIM_END      (7 * 60)   // Ende der Dimmung = 07:00
#define DEFAULT_DIM_LEVEL    15        // Helligkeit waehrend der Dimmung, 5..100 %
#define DEFAULT_DIM_TESTCARD 0         // 1 = im Nachtmodus (Dimm-Zeitfenster) das Testbild zeigen
#define DEFAULT_CLOCK_STYLE  0         // 0..32, siehe CLOCK_STYLE_COUNT (0 = Flip-Karten)
#define DEFAULT_ACCENT       0         // 0..8 (Cyan, Gruen, Orange, Gelb, Rot, Pink, Blau, Weiss, Violett)
#define DEFAULT_HOSTNAME     "wetteruhr"   // erreichbar als http://wetteruhr.local

#define DEFAULT_HOUR_BEEP    0         // Summer: Stundensignal (0/1)
#define DEFAULT_BEEP_QUIET   1         // Summer: kein Stundensignal im Dimm-Zeitfenster (0/1)
#define DEFAULT_TOUCH_BEEP   0         // Summer: kurzer Klick bei Beruehrung des Touch-Tasters (0/1)
#define DEFAULT_ALARM_ON     0         // Wecker aktiv (0/1)
#define DEFAULT_ALARM_TIME   (7 * 60)  // Weckzeit in Minuten seit Mitternacht = 07:00
#define DEFAULT_ALARM_DAYS   0x1F      // Bit 0 = Montag ... Bit 6 = Sonntag (0x1F = Mo-Fr)

#define DEFAULT_UFO          1         // Flugradar: ab und zu ein UFO (0/1)
#define DEFAULT_SLEIGH       2         // ISS-Tracker: 0 = ISS, 1 = immer Weihnachtsschlitten, 2 = Schlitten vom 1. bis 26.12.
#define DEFAULT_NYE_AUTO     1         // Silvester: von 23:30 bis 00:30 automatisch Feuerwerk (0/1)
#define DEFAULT_HOL_AUTO     1         // Feiertags-Designs 10 Tage vorher automatisch in den Design-Wechsel (0/1)
#define HOL_DAYS_BEFORE      10        // so viele Tage vor dem Termin kommen sie dazu

#define DEFAULT_AUTO_ON      0         // automatischer Design-Wechsel (0/1)
#define DEFAULT_AUTO_MASK    0         // Bitmaske der Designs im Wechsel (Bit n = Design n)
#define DEFAULT_AUTO_SEC     300       // Wechselintervall in Sekunden (10..86400)
#define DEFAULT_TICK_TEXT    "Willkommen! Sch\xC3\xB6ne Gr\xC3\xBC\xC3\x9F" "e von deiner Wetteruhr"   // Laufschrift (UTF-8)
#define CLOCK_STYLE_COUNT    33        // Anzahl der Uhr-Designs: 2 klassische + 31 Themen (nicht aendern)

// ---------------------------------------------------------------------
// Firmware-Version (im Browser unter Status und System sichtbar; bei jeder neuen Version erhoehen)
// ---------------------------------------------------------------------
#define FW_VERSION           "2026.10.31"

// Update ueber WLAN aus der Arduino IDE (Netzwerk-Port), 1 = an. Passwort = Web-Passwort (falls gesetzt).
#define ARDUINO_OTA          1

// ---------------------------------------------------------------------
// Einrichtungs-Access-Point (startet ohne WLAN-Daten und nach 60 s ohne Verbindung)
// ---------------------------------------------------------------------
#define AP_SSID              "Wetteruhr-Setup"
#define AP_PASSWORD          "wetteruhr1"   // mindestens 8 Zeichen - bitte aendern
#define AP_FALLBACK_MS       60000UL

// ---------------------------------------------------------------------
// Pins (ESP32-C3 SuperMini, GPIO-Nummer = Arduino-Pinnummer)
// ---------------------------------------------------------------------
#define PIN_TFT_SCK          4    // Display "SCL"
#define PIN_TFT_MOSI         6    // Display "SDA"
#define PIN_TFT_CS           7
#define PIN_TFT_DC           3
#define PIN_TFT_RST          1

// ---------------------------------------------------------------------
// Touch-Taster (TTP223-Modul), -1 = nicht verbaut
//   VCC -> 3V3, GND -> GND, I/O -> PIN_TOUCH. Jumper am Modul unveraendert lassen (Taster-Modus).
//   kurz tippen = naechstes Design aus der Wechsel-Liste (sonst aus allen) | doppelt tippen = Laufschrift abspielen | lange beruehren (1 s) = Standby an/aus
//   Im Standby weckt jede Beruehrung die Uhr.
// ---------------------------------------------------------------------
#define PIN_TOUCH            5    // GPIO des TTP223 (I/O), -1 = nicht verbaut. Ohne Modul unschaedlich (Pulldown).
                                  // Klingelt der Wecker, stellt jede Beruehrung ihn ab.
#define TOUCH_ACTIVE_HIGH    1    // TTP223 Standard: Ausgang HIGH bei Beruehrung
#define TOUCH_MIN_MS         60   // Beruehrung muss mindestens so lange anliegen (filtert Stoerimpulse)
#define TOUCH_LONG_MS        1000
#define TOUCH_DOUBLE_MS      350
#define TOUCH_LOCK_MS        1500 // nach einem Design-Wechsel per Touch so lange keine neue Eingabe (Stoerimpulse beim Neuaufbau)

// ---------------------------------------------------------------------
// Aktiver Piezo-Summer (3 V, mit eingebautem Tongeber), -1 = nicht verbaut
//   Mit Transistor (empfohlen): GPIO -> 1 kOhm -> Basis NPN (z. B. BC547, S8050, 2N2222), Emitter -> GND,
//   Kollektor -> Summer "-", Summer "+" (langes Bein) -> 3V3. BUZZER_ACTIVE_HIGH = 1.
//   Direkt am GPIO nur, wenn der Summer laut Datenblatt hoechstens ca. 12 mA braucht:
//   GPIO -> Summer "+", Summer "-" -> GND.
//   GPIO 10 ist beim SuperMini frei und kein Boot-Pin (nicht 2, 8 oder 9 nehmen).
// ---------------------------------------------------------------------
#define PIN_BUZZER           10
#define BUZZER_ACTIVE_HIGH   1    // 1 = HIGH schaltet den Summer ein (NPN oder direkt), 0 = PNP/invertiert
#define ALARM_MAX_MS         (5UL * 60UL * 1000UL)   // Wecker verstummt spaetestens nach 5 Minuten

// ---------------------------------------------------------------------
// Backlight-PWM (NUR nach Umbau des Moduls, siehe README)
// -1 = kein Umbau: es wird Software-Dimmung (Farben abdunkeln) genutzt.
// Sonst: GPIO, der das Gate eines N-MOSFETs steuert (Low-Side im LED-Minus-Pfad).
// ---------------------------------------------------------------------
#define PIN_TFT_BL           -1
#define BL_ACTIVE_HIGH       1    // 1 = hoher Pegel/Duty = heller (N-MOSFET)
#define BL_PWM_FREQ          2000
#define BL_PWM_CHANNEL       0    // nur fuer aeltere ESP32-Cores (2.x)

// ---------------------------------------------------------------------
// Wetter / Netzwerk
// ---------------------------------------------------------------------
#define WEATHER_RETRY_MS         (60UL * 1000UL)   // Abstand nach fehlgeschlagenem Abruf
#define WEATHER_HTTP_TIMEOUT_MS  10000
#define WIFI_RETRY_MS            15000UL
#define WIFI_REDUCE_TX_POWER     0   // 1 = Sendeleistung auf 8,5 dBm (nur bei instabilem WLAN testen)

// ---------------------------------------------------------------------
// Display
// ---------------------------------------------------------------------
#define TFT_SWAP_RB          0    // 1, falls Rot und Blau vertauscht erscheinen
#define TFT_FORCE_INVERT     -1   // -1 = Bibliotheksstandard, 0 = aus, 1 = an

#define DEBUG_SERIAL         1    // 1 = Statusmeldungen auf USB-Serial (115200)
