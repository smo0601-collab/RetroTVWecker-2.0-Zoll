# Aenderungen

Jede Version nennt die geaenderten Dateien. Fuer ein Update genuegt es, genau diese Dateien im Sketch-Ordner
zu ersetzen (neue hinzufuegen, entfernte loeschen), die Binaerdatei zu exportieren und per WLAN einzuspielen
(README, "Update ueber WLAN"). Die installierte Version steht im Browser unter Status > Firmware.

## 2026.11.11-2 - Firmware-Update: nur passende Dateien, richtiges Programm automatisch

- Der Browser prueft die gewaehlte Datei am **Inhalt** (nicht mehr am Namen), bevor etwas uebertragen wird:
  Erstinstallations-Dateien (mit Bootloader), Dateien fuer das andere Board (CYD / ESP32-C3), alte Versionen ohne
  Kennung und fremde Dateien werden mit Erklaerung abgelehnt.
- Uhr-Datei -> Uhr-Update (bei installierten Spielen automatisch ueber RetroCYD), RetroCYD-Datei -> wird neben der
  Uhr installiert, danach auf Wunsch die Spiele starten. Der Dateiname spielt keine Rolle mehr.
- Die Uhr prueft zusaetzlich selbst den ersten Block (Programm-Kennung, Chip) und am Ende die Programm-Kennung.
- RetroCYD 3.9: gleiche Pruefung; eine RetroCYD-Datei im RetroCYD-Browser startet die Uhr und wird dort eingespielt,
  danach starten die Spiele wieder.

Geaenderte Dateien: `webui.h`, `lang_web.h`, `config.h`, `CHANGELOG.md`

## 2026.11.11 - Kalender mit Feiertagen, besonderen Tagen und Schulferien

- Neuer Reiter **Kalender** im Browser: Bundesland waehlen, gesetzliche Feiertage, besondere Tage und Schulferien
  einzeln ein-/ausschalten, Jahresuebersicht (12 Monatsblaetter und Liste), Einblendungen zum Ansehen.
- Gesetzliche Feiertage je Bundesland (auch Fronleichnam, Reformationstag, Buss- und Bettag, Frauentag,
  Weltkindertag, Mariae Himmelfahrt, Oster-/Pfingstsonntag in Brandenburg) werden in der Uhr berechnet.
- Besondere Tage: Valentinstag, Weiberfastnacht, Rosenmontag, Faschingsdienstag, Muttertag, Vatertag, Erntedank,
  Halloween, Sankt Martin, Nikolaus, 1.-4. Advent, Heiligabend, Silvester; dazu Ferienbeginn und letzter Schultag.
- Schulferien kommen von openholidaysapi.org (frei, ohne Schluessel), werden gespeichert und woechentlich erneuert.
- Neues klassisches Design 28 **Kalender**: Monatsblatt (Feiertage rot, besondere Tage mit Punkt, Ferien blau),
  daneben heute / demnaechst / Schulferien mit Countdown und das Wetter.
- Am Feier- oder besonderen Tag erscheint regelmaessig (Standard alle 15 Minuten, 7-22 Uhr, nicht im Dimm-Fenster)
  fuer 20 Sekunden ein eigenes lustiges, animiertes Bild mit Uhrzeit (37 Szenen) und ein passender Spruch als
  Laufschrift.
- Laufschrift mit Countdown ("Morgen ist ...", "Noch 12 Tage bis zu den Herbstferien", "Herbstferien: noch 4 Tage
  frei!"), Standard alle 30 Minuten.

Geaenderte Dateien: `cal.h`, `cal_text.h`, `face_party.h` (neu), `faces.h`, `WetterUhr.ino`, `settings.h`, `webui.h`,
`lang_web.h`, `config.h`, `README.md`, `CHANGELOG.md`

## 2026.11.10-5 - Fortschrittsbalken beim Update

- Browser: Firmware-Update zeigt jetzt neben der Prozentzahl einen Balken.
- RetroCYD 3.8.1: Im Update-Modus fuer die Uhr ("UHR-UPDATE") blieb der Balken am Display leer und ohne Prozent,
  weil die Fortschrittsanzeige in diesem Modus nicht angemeldet war. Behoben.

Geaenderte Dateien: `webui.h`, `config.h`, `CHANGELOG.md`

## 2026.11.10-4 - Menue ohne Reste des Uhrbilds

- CYD: Beim Oeffnen des Halte-Menues (Spiele / Standby / Zurueck) wurde im selben Durchlauf noch einmal das Uhrbild
  aktualisiert - geaenderte Teile (Sekunden, Animationen) lagen dann ueber dem Menue. Jetzt wird in diesem Moment
  nicht mehr gezeichnet.

Geaenderte Dateien: `WetterUhr.ino`, `config.h`, `CHANGELOG.md`

## 2026.11.10-3 - RGB-LED des CYD steuern

- Browser: Helligkeit > RGB-LED (nur CYD). Modi: aus, feste Farbe, Wetter-Farbe, Temperatur-Farbe, Farbwechsel,
  Atmen, Sekundenpuls. Farbe, Helligkeit, "wann" (immer / nur nachts als Nachtlicht / nachts aus).
- Hinweise blinken zusaetzlich (einzeln abschaltbar): Wecker rot (wie bisher), Update blau, kein WLAN orange,
  Gewitter violett, Frost eisblau.
- Aenderungen sind sofort 8 Sekunden lang als Vorschau zu sehen (neue Adresse /ledprev).
- Im Standby bleibt die LED aus (ausser Wecker und Update).
- RetroCYD 3.8: LED-Effekte in den Spielen (siehe RetroCYD.ino).

Geaenderte Dateien: `led.h` (neu), `WetterUhr.ino`, `settings.h`, `webui.h`, `lang_web.h`, `config.h`, `README.md`, `CHANGELOG.md`

## 2026.11.10 - Ortssuche fuer den Standort

- Reiter "Wetter": neues Feld "Ort suchen" ueber Breiten- und Laengengrad. Ort oder Postleitzahl eingeben, Enter oder
  "Suchen", Treffer (Name, PLZ, Kreis, Land, Koordinaten, Hoehe) antippen - die Koordinaten werden eingetragen.
- Suche ueber die Ortssuche von Open-Meteo (gleicher Dienst wie das Wetter, ohne Schluessel), in der eingestellten
  Sprache. Der Browser fragt direkt; klappt das nicht, fragt die Uhr selbst (neue Adresse /geo).
- Koordinaten koennen weiterhin von Hand eingetragen werden.

Geaenderte Dateien: `webui.h`, `lang_web.h`, `config.h`, `README.md`, `CHANGELOG.md`

## 2026.11.09-6 - Feinere Darstellung, Grossanzeige (fuer das CYD 2,8 Zoll)

- Themen-Designs: Kanten glaetten (Scale2x beim Vergroessern) und Schrift in voller Aufloesung (Texte, Datum, Zahlen
  fein darueber gezeichnet). Im Browser unter Anzeige > Darstellung; CYD: an, ESP32-C3: aus.
- Neues klassisches Design 27 "Grossanzeige": Uhrzeit als 7-Segment ueber die ganze Breite, Sekundenbalken,
  grosse Temperatur mit Wettersymbol, Datum und Wochentag - aus der Ferne lesbar.
- RetroCYD 3.6: Spiele per Touch spielbar (ohne Controller), siehe RetroCYD.ino.

Geaenderte Dateien: `faces_hires.h` (neu), `font5x7.h` (neu), `faces.h`, `faces_core.h`, `WetterUhr.ino`, `webui.h`,
`settings.h`, `config.h`, `lang_web.h`, `README.md`, `CHANGELOG.md`

## 2026.11.09-5 - Uhr-Update direkt im Browser der Uhr (CYD mit Spielen)

- Ein Uhr-Update laesst sich jetzt auch bei installierten Spielen ganz normal unter System > Firmware-Update einspielen.
  Die Uhr startet dafuer kurz RetroCYD im Update-Modus (WLAN der Uhr, gleiche IP, Einmal-Schluessel), die Seite
  schickt die Datei automatisch dorthin, RetroCYD schreibt die neue Uhr und startet sie. Kein Umweg ueber das
  RetroCYD-Setup mehr.
- RetroCYD 3.5: Update-Modus fuer die Uhr (ohne Bluetooth/Spiele, nach 3 Minuten ohne Datei zurueck zur Uhr).
- Einmalig: Diese Version selbst muss noch ueber das RetroCYD-Setup (oder per USB) auf die Uhr, weil die alte Uhr den
  neuen Ablauf noch nicht kennt.

Geaenderte Dateien: `webui.h`, `dualboot.h`, `lang_web.h`, `config.h`, `README.md`, `CHANGELOG.md`

## 2026.11.09-4 - WLAN bleibt beim Wechsel Uhr/Spiele erhalten (CYD)

- Uhr und RetroCYD teilen sich den kleinen Einstellungsspeicher (NVS, 20 KB). Wird er voll oder unlesbar, loescht der
  Start ihn komplett - dann waren WLAN und Einstellungen weg. Neu: Die Uhr sichert WLAN, Einstellungen und
  Touch-Kalibrierung zusaetzlich ausserhalb des NVS (ungenutzter Bereich "spiffs") und stellt sie beim naechsten Start
  selbst wieder her.
- Der WLAN-Treiber schreibt nichts mehr ins NVS (`WiFi.persistent(false)` vor dem ersten WLAN-Aufruf); das entlastet
  den gemeinsamen Speicher.
- Status zeigt die Belegung des Einstellungsspeichers und ob aus der Sicherung wiederhergestellt wurde.
- RetroCYD 3.4: nutzt im Setup automatisch das WLAN der Uhr (auch aus der Sicherung), wenn das eigene fehlt oder nicht
  erreichbar ist.

Geaenderte Dateien: `cyd_shared.h`, `settings.h`, `WetterUhr.ino`, `webui.h`, `lang_web.h`, `config.h`, `CHANGELOG.md`

## 2026.11.09-3 - Sechs Designs entfernt

- Entfernt: Labyrinth, Zahnrad-Werk, Blockwelt, Familie, Weihnachtself, Squishy Dumpling (jetzt 27 Designs:
  2 klassische + 25 Themen). Die Nummern ab 4 sind nachgerueckt; gespeicherte Auswahl und Wechsel-Liste werden beim
  ersten Start umgerechnet, ein entferntes Design wird durch ein aehnliches ersetzt.
- Weihnachten kommt im Feiertags-Wechsel jetzt allein (ohne Elf). Kategorie "Heimat & Familie" entfaellt.
- Programm rund 35 KB kleiner.

Geaenderte Dateien: `faces.h`, `config.h`, `settings.h`, `webui.h`, `lang.h`, `lang_web.h`, `WetterUhr.ino`, `README.md`,
`CHANGELOG.md`; entfernt: `face_maze.h`, `face_works.h`, `face_voxel.h`, `face_family.h`, `face_elf.h`, `face_dumpling.h`

## 2026.11.09 - Spiele (RetroCYD) als zweites Programm auf dem CYD

- Nur CYD: RetroCYD 3.2 kann als zweites Programm neben der Uhr liegen. Beim Einschalten Auswahl "Uhr" oder "Spiele"
  am Touchscreen (sonst nach 6 s die Uhr), langes Halten oeffnet ein Menue mit "Spiele", "Standby", "Zurueck".
- Touchscreen mit Koordinaten und Kalibrierung (3 Kreuze; beim ersten Start mit Spielen automatisch, sonst im Browser).
- Browser: System > Spiele (RetroCYD) mit "Spiele starten" und "Touch kalibrieren"; Status zeigt RetroCYD.
- Firmware-Update erkennt RetroCYD-Dateien und installiert sie neben der Uhr; ein Uhr-Update, das die Spiele
  ueberschreiben wuerde, wird abgelehnt (dann ueber RetroCYD einspielen). Inhalt der Datei wird geprueft.
- Neue Datei fuer die Erstinstallation mit Uhr und Spielen: `..._CYD_mitSpielen_erstinstallation.bin`.
- ESP32-C3: unveraendert (nur neue Texte).

Geaenderte Dateien: `cyd_shared.h` (neu), `dualboot.h` (neu), `board.h`, `WetterUhr.ino`, `webui.h`, `lang.h`,
`lang_web.h`, `config.h`, `README.md`, `CHANGELOG.md`

## 2026.11.08 - Display-Anpassung im Browser

- Neu unter *System > Display-Anpassung*: Display-Controller (automatisch, ILI9341, ST7789), Achsen tauschen,
  links/rechts und oben/unten spiegeln, Rot und Blau tauschen, Farben invertieren. Wirkt sofort nach "Speichern".
- CYD: Der Controller wird beim Start automatisch erkannt (ILI9341 oder ST7789); Status zeigt Controller und Kennung.
- Die Compile-Optionen `CYD_ST7789` und `TFT_FORCE_INVERT` entfallen (jetzt im Browser).

Geaenderte Dateien: `board.h`, `config.h`, `settings.h`, `WetterUhr.ino`, `webui.h`, `lang_web.h`, `README.md`, `CHANGELOG.md`

## 2026.11.07 - CYD 2,8 Zoll

- Laeuft jetzt auch auf dem **CYD 2,8 Zoll** (ESP32-2432S028R, "Cheap Yellow Display"): ILI9341 320 x 240,
  Touchscreen statt Touch-Taster, Ton ueber den Lautsprecher-Anschluss, echte Helligkeitsregelung ueber das Backlight,
  RGB-LED blinkt rot beim Wecken. Gleicher Quellcode wie fuer den ESP32-C3; das Board wird automatisch erkannt.
- Option `CYD_ST7789` fuer die CYD-Variante mit ST7789.
- Firmware-Update ueber WLAN prueft, ob die Datei zum Board passt.
- Status-Seite zeigt die Hardware.
- Release-Dateien fuer beide Boards (CYD-Dateien mit `CYD` im Namen).

Geaenderte Dateien: `board.h` (neu), `config.h`, `WetterUhr.ino`, `webui.h`, `buzzer.h`, `lang_web.h`, `faces.h`,
`README.md`, `CHANGELOG.md`

## 2026.10.31-3 - Sprachen

- Sprache waehlbar unter Anzeige > Darstellung: Deutsch, English, Francais, Espanol, Italiano.
- Uebersetzt sind das Display (alle Designs, Wochentage, Datum, Wettertexte, Feiertags-Gruesse, Badminton-Ansagen,
  Flugradar, ISS, Sonne und Mond, Update-Anzeige) und die komplette Browser-Seite inkl. Design-Namen.
- Datum ausserhalb von Deutsch als `31/10`; lange Ueberschriften werden automatisch kleiner, damit sie passen.
- ISS-Tracker: "Lade ISS-Daten" ueberdeckt nicht mehr das Datum.

Geaenderte Dateien: `lang.h` (neu), `lang_web.h` (neu), `WetterUhr.ino`, `webui.h`, `settings.h`, `config.h`, `icons.h`,
`faces_core.h`, `faces_fx.h`, `pet.h`, alle `face_*.h` mit Text oder Datum, `README.md`, `CHANGELOG.md`

## 2026.10.31-2 - Weihnachtself im Feiertags-Wechsel

- Der Weihnachtself kommt zusammen mit Weihnachten vom 14. bis 26.12. automatisch in den Design-Wechsel.

Geaenderte Dateien: `WetterUhr.ino`, `faces.h`, `webui.h`, `config.h`, `README.md`, `CHANGELOG.md`

## 2026.10.31 - Feiertage automatisch im Design-Wechsel

- Weihnachten, Silvester, Valentinstag, Halloween und Ostern kommen jeweils 10 Tage vor dem Termin automatisch in den
  Design-Wechsel und fallen nach dem Fest wieder heraus (Weihnachten 14.-26.12., Silvester 21.12.-1.1.,
  Valentinstag 4.-14.2., Halloween 21.-31.10., Ostern 10 Tage vor Ostersonntag bis Ostermontag).
- Die eigene Liste bleibt unveraendert; abschaltbar unter Anzeige > Extras. Die Status-Seite zeigt, welche
  Feiertags-Designs gerade dazugekommen sind.

Geaenderte Dateien: `WetterUhr.ino`, `webui.h`, `settings.h`, `config.h`, `faces.h`, `README.md`, `CHANGELOG.md`

## 2026.10.30 - Update ueber WLAN

- Firmware-Update im Browser (System > Firmware-Update ueber WLAN): Fortschritt im Browser und auf dem Display,
  die Datei wird geprueft, Neustart nur bei Erfolg, sonst laeuft die alte Firmware weiter.
- Update direkt aus der Arduino IDE ueber den Netzwerk-Port (Passwort = Web-Passwort), abschaltbar mit `ARDUINO_OTA 0`.
- Firmware-Version und Bauzeit unter Status und System (`FW_VERSION` in `config.h`).
- Hinweis auf der Web-Seite, wenn das Partitionsschema kein Update erlaubt.
- Empfohlenes Partitionsschema jetzt *Minimal SPIFFS (1.9MB APP with OTA)*.

Geaenderte Dateien: `WetterUhr.ino`, `webui.h`, `config.h`, `README.md`, `CHANGELOG.md` (neu)

**Einmalig:** Wer bisher *Huge APP (No OTA)* verwendet, laedt diese Version einmal per USB mit dem neuen
Partitionsschema hoch. Danach gehen alle weiteren Updates per WLAN.

## 2026.10.29 - Aquarium, Badminton-Spielstand

- Aquarium neu gestaltet (Korallenriff, neue Fischarten, Schwarm, Qualle), Besuch von Seeschlange oder U-Boot,
  Knoepfe "Fische fuettern" und "Besuch" unter Anzeige > Extras.
- Badminton: spannenderer Spielverlauf ueber drei Saetze; Spielstand bleibt bei Designwechsel und Neustart erhalten.

Dateien: `face_aqua.h`, `face_badminton.h`, `faces.h`, `webui.h`, `README.md`

## 2026.10.28

- Squishy Dumpling, Feiertags-Designs (Weihnachten, Elf, Halloween, Ostern, Valentinstag, Silvester), UFO im
  Flugradar, Weihnachtsschlitten im ISS-Tracker, Silvester-Automatik; Designs entfernt und Nummern umgerechnet;
  Nachtmodus mit Testbild, manuelle Uhrzeit, Summer und Wecker, Touch-Taster.
