# RetroTV-Wecker 2,0 Zoll

Eine kleine WLAN-Uhr mit Wetter, Wecker und **27 Zifferblättern** im Pixel-Retro-Stil – auf einem
2,0-Zoll-Farbdisplay (320 × 240) mit einem ESP32-C3 SuperMini **oder fertig auf einem CYD 2,8 Zoll**
(„Cheap Yellow Display“, ESP32-2432S028R, ganz ohne Löten). Alles wird bequem im Browser eingestellt,
neue Firmware kommt per WLAN.

![Alle Themen-Designs](docs/images/designs.png)

*Alle Bilder sind echte Bildschirm-Ausgaben der Firmware (am PC gerendert, 2× vergrößert).*

---

## Was die Uhr kann

- **Uhrzeit** per Internet (NTP) mit automatischer Sommer-/Winterzeit oder **von Hand** eingestellt.
- **Wetter** von [Open-Meteo](https://open-meteo.com) (kostenlos, ohne Konto): Temperatur, Feuchte, Wind, Vorhersage. Standort per **Ortssuche** (Ort oder PLZ eingeben, Treffer antippen).
- **27 Designs**: 2 klassische Uhren (Flip-Karten, Digital mit Sekundenbalken) und 25 animierte Szenen –
  Arcade-Spiele, Tiere, Technik, Himmel und Weltraum, Feiertage.
- **Automatischer Design-Wechsel** nach Zeitplan durch eine selbst gewählte Liste. Weihnachten,
  Silvester, Valentinstag, Halloween und Ostern kommen jeweils **10 Tage vor dem Fest von selbst dazu** und danach wieder heraus.
- **Feiertage mit Countdown**: Weihnachten (mit Adventskranz), Halloween, Ostern (echtes Osterdatum),
  Valentinstag, Silvester. An Silvester schaltet die Uhr von 23:30 bis 00:30 von selbst auf **Feuerwerk**.
- **Echte Daten live**: Flugradar mit Flugzeugen in der Umgebung (offene ADS-B-Dienste), ISS-Tracker mit der echten
  Position der Raumstation, Sonne und Mond für deinen Standort.
- **Kleine Überraschungen**: ab und zu ein UFO im Flugradar, im Dezember der Weihnachtsschlitten statt der ISS,
  im Aquarium Besuch von einer Seeschlange oder einem U-Boot.
- **Laufschrift** mit eigenem Text (Umlaute und Sonderzeichen).
- **Helligkeit und Nachtmodus**: Dimmen nach Zeitplan, nachts optional das TV-Testbild.
- **Wecker und Stundensignal** mit einem aktiven Piezo-Summer (optional).
- **Touch-Taster** (optional): durch die Designs blättern, Laufschrift starten, Standby.
- **Firmware-Update über WLAN** direkt im Browser.
- **Fünf Sprachen** für Display und Browser-Seite: Deutsch, English, Français, Español, Italiano.

![Ein Design in fünf Sprachen](docs/images/languages.png)

## Ein paar Designs im Detail

| Badminton | Aquarium | Weihnachten |
|---|---|---|
| ![Badminton](docs/images/design_16.png) | ![Aquarium](docs/images/design_21.png) | ![Weihnachten](docs/images/design_26.png) |
| Match über drei Sätze bis 21 nach echten Regeln: Smash mit Geschwindigkeit, Hawk-Eye, Satz- und Matchball, jubelndes Publikum. Der Spielstand bleibt auch nach Stromausfall erhalten. | Korallenriff mit Lichtmustern, Clownfischen in der Anemone, Neon-Schwarm, Qualle, Schatztruhe – und ab und zu Besuch. | Countdown bis Heiligabend 18 Uhr, Adventskranz mit einer Kerze mehr an jedem Adventssonntag, Schneefall. |

| Flugradar | Halloween | Silvester |
|---|---|---|
| ![Flugradar](docs/images/design_19.png) | ![Halloween](docs/images/design_halloween.png) | ![Silvester](docs/images/design_31.png) |
| Echte Flugzeuge rund um deinen Standort mit Rufzeichen, Typ, Höhe und Geschwindigkeit. | Vollmond, Fledermäuse, Gespenst und flackernder Kürbis; Countdown bis 31.10., zur vollen Minute „BUH!“. | Countdown bis Neujahr, in der letzten Minute riesige Sekunden, um Mitternacht Dauerfeuerwerk. |

Die vollständige Liste aller Designs mit Beschreibung steht in der
[ausführlichen Anleitung](firmware/WetterUhr/README.md#uhr-designs-und-farben).

---

## Hardware

Zwei Varianten, gleicher Quellcode, gleiche Funktionen. Das Board wird beim Kompilieren automatisch erkannt.

### Variante A: CYD 2,8 Zoll (ohne Löten)

| Teil | Hinweis |
|---|---|
| CYD 2,8 Zoll (ESP32-2432S028R) | ESP32, Display ILI9341 320 × 240, resistiver Touchscreen, USB – alles auf einer Platine |
| kleiner Lautsprecher 8 Ω, 0,5–2 W, Stecker JST 1,25 mm | optional, an „SPEAK“, für Wecker und Stundensignal |

Nichts zu verdrahten. Der **Touchscreen** ersetzt den Touch-Taster (tippen = nächstes Design, doppelt = Laufschrift,
1 s halten = Menü mit Spiele/Standby), die **Helligkeit** wird echt über das Backlight geregelt, und die **RGB-LED** ist im Browser
einstellbar (feste Farbe/Nachtlicht, Wetter- oder Temperatur-Farbe, Effekte, Hinweise wie Wecker, kein WLAN, Gewitter, Frost). CYDs gibt es mit ILI9341 oder ST7789 – die Uhr erkennt das selbst. Ist das Bild
gedreht, gespiegelt oder farbig falsch, lässt es sich im Browser unter *System → Display-Anpassung* korrigieren. Details in der [Anleitung](firmware/WetterUhr/README.md#cyd-28-zoll-esp32-2432s028r).
Auf dem CYD werden die Designs geglättet und ihre Schrift in voller Auflösung gezeichnet (schärfer statt Doppelpixel); Design 27 **Großanzeige** zeigt die Uhrzeit über die ganze Breite – aus der Ferne lesbar.

#### Spiele: RetroCYD als zweites Programm (nur CYD)

Auf dem CYD kann neben der Uhr die Spielekonsole **RetroCYD** liegen (20 Spiele in 6 Kategorien, LED-Effekte, Xbox-Controller per Bluetooth oder direkt per Touch,
Quellcode in [`firmware/RetroCYD`](firmware/RetroCYD)). Uhr und Spiele sind zwei Programme in den beiden
Programmbereichen des Flash:

- **Einschalten:** am Bildschirm **Uhr** oder **Spiele** antippen (sonst startet nach 6 s die Uhr).
- **Lange halten:** in der Uhr Menü mit *Spiele / Standby / Zurück*, in RetroCYD Menü mit *ZUR UHR / SETUP / ZURÜCK*.
- **Spielemenü nach Kategorien:** Klassiker, Shooter, Jump & Run, Rennen, Denkspiele, Brett & Karten –
  Kategorie antippen, dann das Spiel.
- **Für Touch gemacht (ab RetroCYD 3.7):** 2048, Lichter aus, Rohrpost, Paare finden, Vier gewinnt (gegen die CPU
  oder zu zweit), Siebzehn und vier, Spurwechsel. Bedienung direkt durch Antippen und Wischen.

![Spielemenü](docs/images/retrocyd_menue.png)

![Neue Spiele](docs/images/retrocyd_neue_spiele.png)

*Neu in RetroCYD 3.7: 2048, Lichter aus, Rohrpost, Paare finden, Vier gewinnt, Siebzehn und vier, Spurwechsel (nachts).*

![Sunset Race](docs/images/sunset_race.png)

*Sunset Race in RetroCYD 3.3: Kurven und Hügel, Palmen, Schilder, Tore – und ein großes eigenes Auto.*

- **Installieren:** neu mit `…_CYD_mitSpielen_erstinstallation.bin`, oder zu einer laufenden Uhr
  `RetroCYD_<Version>_update.bin` im Browser der Uhr einspielen.
- **Updates:** Uhr und Spiele beide im Browser der Uhr. Für ein Uhr-Update wechselt die Uhr kurz in RetroCYD, die Seite erledigt das von selbst.

### Variante B: ESP32-C3 SuperMini + 2,0-Zoll-Display

| Teil | Hinweis |
|---|---|
| ESP32-C3 SuperMini | Mikrocontroller mit WLAN, USB-C |
| 2,0-Zoll-TFT GMT020-02-7P | ST7789, 240 × 320, SPI (Pins: CS, DC, RST, SDA, SCL, VCC, GND) |
| Touch-Taster TTP223 | optional |
| Aktiver Piezo-Summer 3 V + NPN-Transistor (z. B. BC547) + 1 kΩ | optional, für Wecker und Stundensignal |

### Verdrahtung

| Bauteil | Anschluss | ESP32-C3 SuperMini |
|---|---|---|
| Display | SCL | GPIO 4 |
| Display | SDA | GPIO 6 |
| Display | CS | GPIO 7 |
| Display | DC | GPIO 3 |
| Display | RST | GPIO 1 |
| Display | VCC / GND | 3V3 / GND |
| Touch-Taster TTP223 | I/O (VCC an 3V3, GND an GND) | GPIO 5 |
| Summer (über NPN) | GPIO → 1 kΩ → Basis; Emitter an GND; Kollektor an Summer „−“; Summer „+“ an 3V3 | GPIO 10 |

GPIO 2, 8 und 9 bitte frei lassen (Boot-Pins). Alle Pins lassen sich in
[`config.h`](firmware/WetterUhr/config.h) ändern.

---

## Schnellstart

### 1. Arduino IDE einrichten (einmalig)

1. **Boardpaket**: Unter *Datei → Einstellungen → Zusätzliche Boardverwalter-URLs*
   `https://espressif.github.io/arduino-esp32/package_esp32_index.json` eintragen, dann im Boardverwalter
   **esp32 by Espressif Systems** installieren.
2. **Bibliotheken** (Bibliotheksverwalter): *Adafruit GFX Library*, *Adafruit ST7735 and ST7789 Library*,
   *Adafruit ILI9341*, *ArduinoJson* (Version 7).
3. **Board-Einstellungen** unter *Werkzeuge*:
   - CYD 2,8 Zoll: Board **ESP32 Dev Module**
   - ESP32-C3: Board **ESP32C3 Dev Module**, USB CDC On Boot: **Enabled**
   - beide: Partition Scheme: **Minimal SPIFFS (1.9MB APP with OTA)** – wichtig für Updates per WLAN

### 2. Firmware aufspielen (einmalig per USB)

**Ohne Arduino IDE (am einfachsten):** Im Ordner [`releases/`](releases) die Datei
`WetterUhr_<Version>_CYD_erstinstallation.bin` (CYD) bzw. `WetterUhr_<Version>_erstinstallation.bin` (ESP32-C3)
herunterladen, in Chrome oder Edge <https://espressif.github.io/esptool-js/> öffnen, Board per USB verbinden (**Connect**), *Flash Address* `0x0`,
Datei wählen, **Program**. Genaue Schritte stehen in der `LIESMICH.md` der jeweiligen Version.
Dabei werden vorhandene Einstellungen gelöscht.

**Mit der Arduino IDE:** Ordner [`firmware/WetterUhr`](firmware/WetterUhr) herunterladen (oder das Quellcode-Zip aus
[`releases/`](releases)), `WetterUhr.ino` in der Arduino IDE öffnen und per USB hochladen.

### 3. Einrichten im Browser

1. Ohne gespeichertes WLAN öffnet die Uhr den Access-Point **Wetteruhr-Setup** (Passwort `wetteruhr1`).
2. Mit Handy oder PC verbinden und `http://192.168.4.1` öffnen.
3. WLAN, Zeitzone und Standort eintragen, speichern – die Uhr startet neu.
4. Danach ist sie unter ihrer IP-Adresse (kurz auf dem Display eingeblendet) oder `http://wetteruhr.local` erreichbar.
5. **Bitte ein Web-Passwort setzen** (System → Zugang).

---

## Updates über WLAN

Jede neue Version liegt im Ordner [**`releases/`**](releases)
(aktuell: [`releases/2026.11.10-5`](releases/2026.11.10-5): Uhr 2026.11.10-5 mit RetroCYD 3.8.1), außerdem unter [**Releases**](../../releases):

| Datei | Wofür |
|---|---|
| `WetterUhr_<Version>_CYD_mitSpielen_erstinstallation.bin` | CYD 2,8 Zoll: Uhr **und** RetroCYD-Spiele, Erstinstallation per USB (an `0x0`) |
| `WetterUhr_<Version>_CYD_erstinstallation.bin` | CYD 2,8 Zoll: nur die Uhr, Erstinstallation per USB (an Adresse `0x0`) |
| `WetterUhr_<Version>_CYD_update.bin` | CYD 2,8 Zoll: Update-Datei zum Einspielen über WLAN |
| `WetterUhr_<Version>_erstinstallation.bin` | ESP32-C3: komplette Firmware für die Erstinstallation per USB (an Adresse `0x0`) |
| `WetterUhr_<Version>_update.bin` | ESP32-C3: Update-Datei zum Einspielen über WLAN |
| `WetterUhr_<Version>_komplett.zip` | kompletter Quellcode für die Arduino IDE (beide Boards) |
| `RetroCYD_<Version>_update.bin` | CYD: Spiele installieren bzw. aktualisieren (im Browser der Uhr einspielen) |
| `RetroCYD_<Version>_komplett.zip` | Quellcode RetroCYD (Boardpaket *esp32_bluepad32*, Bibliothek *LovyanGFX*) |

Eine Update-Datei für das falsche Board lehnt die Uhr ab.

**Update einspielen:** Browser → *System* → *Firmware-Update über WLAN* → `.bin` wählen → *Update installieren*.
Browser und Display zeigen den Fortschritt. Die Uhr prüft die Datei und startet nur bei Erfolg neu; sonst läuft
die alte Firmware weiter. Alle Einstellungen bleiben erhalten. Unter *Status → Firmware* steht danach die neue Version.

Alternativ in der Arduino IDE unter *Werkzeuge → Port* den Netzwerk-Port **wetteruhr** wählen und normal hochladen.

> **Umstieg von einer älteren Version:** Lief die Uhr bisher mit dem Partitionsschema *Huge APP (No OTA)*,
> muss die neue Version **einmal per USB** installiert werden – entweder mit der `…_erstinstallation.bin`
> (Einstellungen werden gelöscht) oder aus der Arduino IDE mit *Minimal SPIFFS (1.9MB APP with OTA)*
> (Einstellungen bleiben). Danach gehen alle Updates per WLAN.

Was sich in jeder Version geändert hat, steht im [CHANGELOG](firmware/WetterUhr/CHANGELOG.md).

---

## Bedienung im Browser

| Seite | Inhalt |
|---|---|
| Status | WLAN, IP, Uhrzeit, Wetter, aktuelles Design, Dimmung, Wecker, Firmware-Version |
| Anzeige | Designs mit Vorschaubildern, Akzentfarbe, automatischer Wechsel, Extras (Feiertage, Silvester-Automatik, Weihnachtsschlitten, UFO, Aquarium), Sprache |
| Helligkeit | Helligkeit, Standby, Zeitplan-Dimmung, Nachtmodus mit Testbild |
| Laufschrift | Text, Position, Größe, Farbe, Tempo, Dauerschleife |
| Summer & Wecker | Stundensignal, Nachtruhe, Weckzeit mit Wochentagen, Summer-Test |
| WLAN | Netzwerk suchen und eintragen |
| Zeit & Wetter | Zeitzone, NTP oder manuelle Uhrzeit, Standort (mit Ortssuche), Wetter-Intervall, Flugradar-Radius |
| System | Gerätename, Web-Passwort, Firmware-Update über WLAN, Werkseinstellungen |

Alle Einzelheiten – Designs, Dimmung, Wecker, Touch-Bedienung, Fehlersuche – stehen in der
**[ausführlichen Anleitung](firmware/WetterUhr/README.md)**.

---

## Projektaufbau

```
firmware/RetroCYD/         Spielekonsole RetroCYD (zweites Programm auf dem CYD)
firmware/WetterUhr/        Arduino-Sketch (diesen Ordner in der Arduino IDE öffnen)
  WetterUhr.ino            Hauptprogramm: Anzeige, WLAN, Wetter, Flugradar, ISS, Update
  config.h                 Pins (CYD bzw. ESP32-C3, automatisch), Standardwerte, Firmware-Version
  board.h                  Display-Treiber, Touchscreen, RGB-LED je Board
  cyd_shared.h, dualboot.h CYD: Touch-Kalibrierung, Startauswahl Uhr/Spiele, Umschalten zu RetroCYD
  settings.h               Einstellungen im Flash
  webui.h                  Einstellungsseite im Browser
  lang.h, lang_web.h       Texte in fünf Sprachen (Display, Browser)
  faces.h, faces_*.h       Rahmen für die Themen-Designs (160×120-Pixelpuffer, 2× vergrößert)
  face_*.h                 je ein Design
  ticker.h, buzzer.h       Laufschrift, Summer und Wecker
  README.md                ausführliche Anleitung
  CHANGELOG.md             Änderungen je Version
docs/images/               Bilder für diese Seite
releases/<Version>/        Erstinstallations-.bin, Update-.bin und Quellcode-Zip je Version
```

## Technik in Kürze

- Die Themen-Designs zeichnen in einen 160 × 120-Pixelpuffer mit 256 Farben, der auf 320 × 240 verdoppelt wird.
  Gesendet werden nur geänderte Kacheln – das ergibt flüssige Animationen ohne Flackern.
- Wetter, Flugradar und ISS werden in eigenen Hintergrund-Tasks geholt; die Anzeige blockiert nie.
- Der ESP32-C3 hat keine Gleitkomma-Einheit – rechenintensive Effekte (Wasser, Lichtmuster) nutzen Ganzzahl-Tabellen.
- Gebaut mit dem ESP32-Arduino-Core 3.3.x; die Firmware belegt rund 1,65 MB.

## Hinweise

- Die Einstellungsseite läuft über unverschlüsseltes HTTP im eigenen WLAN. Ohne Web-Passwort kann jeder im
  Netzwerk die Uhr bedienen und Firmware einspielen.
- Flugdaten: adsb.lol, airplanes.live, adsb.fi – nur für private, nicht kommerzielle Nutzung. ISS-Daten: open-notify.org.
  Wetter: Open-Meteo.
- Alle Figuren und Szenen sind eigene Gestaltungen im Retro-Stil.

- Druckdateien unter https://makerworld.com/de/models/3523932-retro-tv-case-esp32-c3-supermini-2-display#profileId-4028407
